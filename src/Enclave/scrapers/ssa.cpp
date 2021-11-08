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
#include "scraper_utils.h"

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

err_code SSA::handle(const uint8_t *req, size_t data_len, int *resp_data) {
  if (data_len != 32) {
    LL_CRITICAL("data_len %zu*32 is not 32", data_len / 32);
    return INVALID_PARAMS;
  }

  string coin_id(string(req, req + 0x20).c_str());

  try {
    int price = 0;
    *resp_data = price;

    return NO_ERROR;
  }
  catch (const std::invalid_argument &e) {
    return INVALID_PARAMS;
  }
  catch (const std::exception &e) {
    LL_CRITICAL("https error: %s", e.what());
    return WEB_ERROR;
  }
  catch (...) {
    return INVALID_PARAMS;
  }
}

