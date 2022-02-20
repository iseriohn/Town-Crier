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

#include "coinbase.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <iostream>

#include <debug.h>
#include "tls_client.h"
#include "commons.h"
#include "hybrid_cipher.h"
#include "Constants.h"
#include "log.h"
#include "error_codes.h"

#include "external/slre.h"
#include "external/picojson.h"

using std::string;

err_code CoinbaseScraper::handle_long_resp(const char *data, size_t data_len, char *resp_data) {
  string api_response;
  
  char url[256];
  std::vector<string> header;
  try {
    char* loc = strstr((char*)data, (char*)"\n");
    int pos = loc - (char*)data;
    const int offset = 29; // coinbase
    strncpy(url, (char*)data + offset, pos - offset);
    url[pos - offset] = 0;
    data = data + pos + 1;
    data_len -= pos + 1;
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
  } catch (const std::runtime_error &e) {
    LL_INFO("%s", e.what());
    return WEB_ERROR;
  }

  HttpRequest httpRequest("accounts.coinbase.com", url, header, true);
  HttpsClient httpClient(httpRequest);

  try {
    HttpResponse response = httpClient.getResponse();
    api_response = response.getContent();
    httpClient.close();
  } catch (const std::runtime_error &e) {
    LL_INFO("%s", e.what());
    httpClient.close();
    return WEB_ERROR;
  }
 
  LL_INFO("HTTP response received.");
  if (api_response.empty()) {
    LL_INFO("api return empty");
    return WEB_ERROR;
  }

  try {
    picojson::value response_struct;
    string err = picojson::parse(response_struct, api_response);

    if (!err.empty() || !response_struct.is<picojson::object>()) {
      LL_INFO("can't parse response"); // %s", api_response.c_str());
      return WEB_ERROR;
    }
    if (response_struct.contains("error")) {
      if (response_struct.get("error").is<string>()) {
        err = response_struct.get("error").get<string>();
      }

      LL_INFO("%s", err.c_str());
      return WEB_ERROR;
    }

    string name = "";
    picojson::value response_ex_struct = response_struct.get("first_name");
    if (response_ex_struct.is<string>()) {
      name = (string)response_ex_struct.get<string>();
    } else return INVALID_PARAMS;
    response_ex_struct = response_struct.get("last_name");
    if (response_ex_struct.is<string>()) {
      name = name + " " + (string)response_ex_struct.get<string>();
    } else return INVALID_PARAMS;
    std::transform(name.begin(), name.end(), name.begin(), ::toupper);

/* comment out for demo
    string dob = "";
    response_ex_struct = response_struct.get("dob_year");
    if (response_ex_struct.is<int64_t>()) {
      dob = std::to_string(response_ex_struct.get<int64_t>());
    } else return INVALID_PARAMS;
    response_ex_struct = response_struct.get("dob_month");
    if (response_ex_struct.is<int64_t>()) {
      string month = std::to_string(response_ex_struct.get<int64_t>());
      dob = dob + "-" + std::string(2 - month.length(), '0') + month;
    } else return INVALID_PARAMS;
    response_ex_struct = response_struct.get("dob_day");
    if (response_ex_struct.is<int64_t>()) {
      string day = std::to_string(response_ex_struct.get<int64_t>());
      dob = dob + "-" + std::string(2 - day.length(), '0') + day;
    } else return INVALID_PARAMS;

    name = dob + name;
*/
    name.copy(resp_data, name.size());
  } catch (const std::exception &e) {
    LL_INFO("%s", e.what());
    return INVALID_PARAMS;
  }
  
  return NO_ERROR;
}
