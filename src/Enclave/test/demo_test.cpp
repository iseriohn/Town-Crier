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
#include <string.h>
#include <cstring>
#include <iostream>

#include <debug.h>
#include "tls_client.h"
#include "encoding.h"
#include "../log.h"
#include "commons.h"
#include "hybrid_cipher.h"
#include "scrapers/error_codes.h"
#include "Constants.h"

#include "external/slre.h"
#include "external/picojson.h"

int demo_self_test(uint32_t study, uint8_t* addr, unsigned char* sealed_data, size_t sealed_data_len) {
    string wallet = ucharToHexString(reinterpret_cast<unsigned char*>(addr), 20);
  //LL_INFO("New participant of Study #%d with address 0x%s.", study, wallet.c_str());
  LL_INFO("New query received.");
  LL_INFO("Encrypted HTTP request header: %s", sealed_data);
  string plain = decrypt_query(sealed_data, sealed_data_len);
  auto data = plain.c_str();
  auto data_len = plain.size();
  LL_INFO("Sealed HTTP header decrypted.");
  LL_DEBUG("Decrypted HTTP header: %s", data);
  
  char* loc = strstr((char*)data, (char*)"\n");
  int pos = loc - (char*)data;
  char url[256];
  const int offset = 22; // SSA
  //const int offset = 29; // coinbase
  //const int offset = 0; // spotify
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
  LL_DEBUG("url: %s", url);

  const string mtb = "onlinebanking.mtb.com";
  const string coinbase = "accounts.coinbase.com";
  const string tax = "otc.tax.ny.gov";
  const string chase = "secure01b.chase.com";
  const string spotify = "api.spotify.com";
  const string ssa = "secure.ssa.gov";

  HttpRequest httpRequest(ssa, url, header, true);
  HttpsClient httpClient(httpRequest);

  string api_response;
  try {
    HttpResponse response = httpClient.getResponse();
    api_response = response.getContent();
  } catch (const std::runtime_error &e) {
    LL_CRITICAL("%s", e.what());
    httpClient.close();
    return WEB_ERROR;
  }
 
  LL_INFO("HTTP response received.");
  LL_DEBUG("HTTP response: %s", api_response.c_str());
  if (api_response.empty()) {
    LL_CRITICAL("api return empty");
    return WEB_ERROR;
  }

  try {
    //return form_transaction(0, );
  }
  catch (const std::exception &e) {
    LL_CRITICAL("%s", e.what());
    return UNKNOWN_ERROR;
  }
    
  return NO_ERROR;
}

