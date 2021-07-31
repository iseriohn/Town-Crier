//
// Copyright (c) 2016-2018 by Cornell University.  All Rights Reserved.
//
// Permission to use the "TownCrier" software ("TownCrier"), officially
// docketed at the Center for Technology Licensing at Cornell University
// as D-7364, developed through research conducted at Cornell University,
// and its associated copyrights solely for educational, research and
// non-profit purposes without fee is hereby granted, provided that the
// user agrees as follows:
//
// The permission granted herein is solely for the purpose of compiling
// the TownCrier source code. No other rights to use TownCrier and its
// associated copyrights for any other purpose are granted herein,
// whether commercial or non-commercial.
//
// Those desiring to incorporate TownCrier software into commercial
// products or use TownCrier and its associated copyrights for commercial
// purposes must contact the Center for Technology Licensing at Cornell
// University at 395 Pine Tree Road, Suite 310, Ithaca, NY 14850; email:
// ctl-connect@cornell.edu; Tel: 607-254-4698; FAX: 607-254-5454 for a
// commercial license.
//
// IN NO EVENT SHALL CORNELL UNIVERSITY BE LIABLE TO ANY PARTY FOR
// DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES,
// INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF TOWNCRIER AND ITS
// ASSOCIATED COPYRIGHTS, EVEN IF CORNELL UNIVERSITY MAY HAVE BEEN
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// THE WORK PROVIDED HEREIN IS ON AN "AS IS" BASIS, AND CORNELL
// UNIVERSITY HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
// ENHANCEMENTS, OR MODIFICATIONS.  CORNELL UNIVERSITY MAKES NO
// REPRESENTATIONS AND EXTENDS NO WARRANTIES OF ANY KIND, EITHER IMPLIED
// OR EXPRESS, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, OR THAT THE USE
// OF TOWNCRIER AND ITS ASSOCIATED COPYRIGHTS WILL NOT INFRINGE ANY
// PATENT, TRADEMARK OR OTHER RIGHTS.
//
// TownCrier was developed with funding in part by the National Science
// Foundation (NSF grants CNS-1314857, CNS-1330599, CNS-1453634,
// CNS-1518765, CNS-1514261), a Packard Fellowship, a Sloan Fellowship,
// Google Faculty Research Awards, and a VMWare Research Award.
//

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

#include <debug.h>
#include "tls_client.h"
#include "scrapers/gmail.h"
#include "../log.h"
#include "commons.h"
#include "hybrid_cipher.h"

string ucharToHexString(unsigned char* charArray, uint32_t charArrayLength) {
    if(charArray == nullptr) {
        return "nullptr";
    }

    if(charArrayLength > 65536) {
        return "charArrayLength overflow";
    }

    constexpr char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    std::string s(charArrayLength * 2, ' ');
    for (int i = 0; i < charArrayLength; ++i) {
        s[2 * i] = hexmap[(charArray[i] & 0xF0) >> 4];
        s[2 * i + 1] = hexmap[charArray[i] & 0x0F];
    }
    return s;
} 

int gmail_self_test(uint32_t study, uint8_t* addr, unsigned char* sealed_data, size_t sealed_data_len) {
  LL_INFO("New participant of Study #%d with address 0x%s.", study, ucharToHexString(reinterpret_cast<unsigned char*>(addr), 20).c_str());
  string plain = decrypt_query(sealed_data, sealed_data_len);
  auto data = plain.c_str();
  auto data_len = plain.size();
  LL_INFO("Sealed http header decrypted.");
/*
  // Build header for https request
  char* loc = strstr((char*)data, (char*)"Host:");
  int pos = loc - (char*)data;
  char url[256];
  strncpy(url, (char*)data + 4, pos - 5);
  url[pos - 5] = 0;

  data = data + pos;
  data_len -= pos;
  std::vector<string> header;
  loc = strstr((char*)data, "\n");
  while (loc!= NULL) {
    pos = loc - (char*)data;
    char tmp[1000000];
    strncpy(tmp, (char*)data, pos);
    tmp[pos] = 0;
    if (pos > 0) header.push_back(string(tmp));
    data = data + pos + 1;
    data_len -= pos + 1;
    loc = strstr((char*)data, "\n");
  }
  if (data_len > 0) header.push_back(string((char*)data));
*/
  char* loc = strstr((char*)data, (char*)"\n");
  int pos = loc - (char*)data;
  char url[256];
  const int offset = 0;
  strncpy(url, (char*)data + offset, pos - offset);
  url[pos - offset] = 0;
  data = data + pos + 1;
  data_len -= pos + 1;
  std::vector<string> header;
  loc = strstr((char*)data, "\n");
  while (loc!= NULL) {
    pos = loc - (char*)data;
    char tmp[1000000];
    strncpy(tmp, (char*)data, pos);
    tmp[pos] = 0;
    if (pos > 0) header.push_back(string(tmp));
    data = data + pos + 1;
    data_len -= pos + 1;
    loc = strstr((char*)data, "\n");
  }
  if (data_len > 0) {
      header.push_back(string((char*)data));
  }

  const string mtb = "onlinebanking.mtb.com";
  const string coinbase = "www.coinbase.com";
  const string tax = "otc.tax.ny.gov";
  const string chase = "secure01b.chase.com";
  HttpRequest httpRequest(coinbase, url, header, true);
  HttpsClient httpClient(httpRequest);

  string api_response;
  try {
    HttpResponse response = httpClient.getResponse();
    api_response = response.getContent();
  } catch (const std::runtime_error &e) {
    LL_CRITICAL("%s", e.what());
    httpClient.close();
    return HTTP_ERROR;
  }
  if (api_response.empty()) {
    LL_CRITICAL("api return empty");
    return HTTP_ERROR;
  }
  LL_INFO("Receive demographic data from Coinbase");
  try {
    return form_transaction();
  }
  catch (const std::exception &e) {
    LL_CRITICAL("%s", e.what());
    return INTERNAL_ERR;
  }
    
  return 0;
}

