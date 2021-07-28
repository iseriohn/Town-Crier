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

int gmail_self_test(unsigned char* data, size_t data_len) {
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


  HttpRequest httpRequest("onlinebanking.mtb.com", url, header);
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
  LL_CRITICAL("%s", api_response.c_str());
  try {
  }
  catch (const std::exception &e) {
    LL_CRITICAL("%s", e.what());
    return INTERNAL_ERR;
  }

    /*
  GmailScraper testScraper;

  gmail_error err= testScraper.get_data(query);
  if (err != NOT_FOUND) {
    LL_CRITICAL("err should have been NOT_FOUND");
    rc = -1;
  }

  string flight_num = "FJM273";
  flight_num.append(32 - flight_num.length(), 0);

  string unx_epoch = "\x58\xEF\xA4\x04";
  unx_epoch.insert(unx_epoch.begin(), 32 - unx_epoch.length(), '\0');

  testScraper.handle((uint8_t *) (flight_num + unx_epoch).c_str(), 32 * 2, &delay);
  return rc;
  */
    return 0;
}
//1477276620,
//filed_departuerrime\":1477276620

