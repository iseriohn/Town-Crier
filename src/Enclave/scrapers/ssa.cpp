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

#include "ssa.h"

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

static double manual_parse_response(const char *resp) {
  double ret = 0;
  const char *end;
  const char *temp = resp;

  std::string buf_string(resp);
  std::size_t pos = buf_string.find("price_usd\": \"");

  if (pos == std::string::npos) {
    return 0.0;
  }
  temp += (pos + 13);
  end = temp;
  while (*end != '"') {
    end += 1;
  }

  ret = std::strtod(temp, NULL);
  return ret;
}

err_code SSAScraper::handle_long_resp(const char *data, size_t data_len, char *resp_data) {
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

  HttpRequest httpRequest("secure.ssa.gov", url, header, true);
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


  
  picojson::value response_struct;
  string err = picojson::parse(response_struct, api_response);
  
  if (!err.empty() || !response_struct.is<picojson::object>()) {
    LL_CRITICAL("can't parse %s", api_response.c_str());
    return WEB_ERROR;
  }
  if (response_struct.contains("error")) {
    if (response_struct.get("error").is<string>()) {
      err = response_struct.get("error").get<string>();
    }

    LL_CRITICAL("%s", err.c_str());
    return WEB_ERROR;
  }

  string user_id = "";
  if (!response_struct.contains("data")) {
          return NO_ERROR;
  }
  picojson::value response_ex_struct = response_struct.get("data").get("id");
  if (response_ex_struct.is<string>()) {
    user_id = (string)response_ex_struct.get<string>();
  }
  string email = "";
  response_ex_struct = response_struct.get("data").get("email");
  if (response_ex_struct.is<string>()) {
    email = (string)response_ex_struct.get<string>();
  }
  string residence = "";
  response_ex_struct = response_struct.get("data").get("state");
  if (response_ex_struct.is<string>()) {
    residence = (string)response_ex_struct.get<string>();
  }
  LL_INFO("[DEMO ONLY, TO BE SEALED] (user_id: %s; email: %s; residence state: %s)", 
          user_id.c_str(), email.c_str(), residence.c_str());
  return NO_ERROR;
}
