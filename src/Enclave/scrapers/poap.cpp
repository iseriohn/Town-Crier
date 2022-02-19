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

#include "poap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <iostream>

#include <debug.h>
#include "tls_client.h"
#include "eth_ecdsa.h"
#include "commons.h"
#include "hybrid_cipher.h"
#include "Constants.h"
#include "log.h"
#include "error_codes.h"
#include "eth_ecdsa.h"
#include "external/keccak.h"

#include "external/slre.h"
#include "external/picojson.h"

using std::string;

err_code PoapScraper::handle_long_resp(const char *data, size_t data_len, char *resp_data) {
  try {
    if (data_len != 130) {
      LL_INFO("invalid message length.");
      return WEB_ERROR;
    }
  
    //try {
  // compute address
    uint8_t before_hash[100];
    uint8_t hash[32];
    unsigned char addr[20];
    memcpy(before_hash, data + 1, 65);
    auto ret = keccak(before_hash, 64, hash, 32);
    if (ret) {
      LL_CRITICAL("Keccak256 error %d\n", ret);
      return UNKNOWN_ERROR;
    }
    memcpy(addr, hash + 12, 20);
    string wallet_addr = ucharToHexString(addr, 20);
    LL_DEBUG("Wallet address: 0x%s", wallet_addr.c_str());

  // compute message hash
    memcpy(before_hash, POAP_HASH_PREFIX.c_str(), POAP_HASH_PREFIX.size());
    memcpy(before_hash + POAP_HASH_PREFIX.size(), wallet_addr.c_str(), 40);
    ret = keccak(before_hash, POAP_HASH_PREFIX.size() + 40, hash, 32);
    if (ret) {
      LL_CRITICAL("Keccak256 error %d\n", ret);
      return UNKNOWN_ERROR;
    }
    LL_DEBUG("message hash: 0x%s", ucharToHexString(hash, 32).c_str());
    LL_DEBUG("public key: 0x%s", ucharToHexString(reinterpret_cast<const unsigned char*>(data), 65).c_str());

    // verify signature
    ret = ecdsa_verify(hash, 32, reinterpret_cast<const uint8_t*>(data), reinterpret_cast<const uint8_t*>(data + 65), reinterpret_cast<const uint8_t*>(data + 97), (const uint8_t)data[129]);
    if (ret != 0) {
      LL_INFO("invalid signature");
      return WEB_ERROR;
    }
      
    // valid signature, search for poap
    if (POAP_LIST.find(wallet_addr) == POAP_LIST.end()) {
      LL_INFO("POAP not found");
      return INVALID_PARAMS;
    }
    wallet_addr.copy(resp_data, wallet_addr.size());
  } catch (const std::exception &e) {
    LL_CRITICAL("exception while verify poap query: %s", e.what());
    return WEB_ERROR;
  }
      
  return NO_ERROR;
}
