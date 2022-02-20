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

#include "event_handler.h"
#include <string>
#include <inttypes.h>

#include "scrapers/scrapers.h"
#include "scrapers/ssa.h"
#include "scrapers/coinbase.h"
#include "scrapers/yahoo_yql_stock.h"
#include "scrapers/Scraper.h"
#include "scrapers/flight.h"
#include "scrapers/error_codes.h"
#include "scrapers/stock_ticker.h"
#include "scrapers/ups_tracking.h"
#include "scrapers/steam2.h"
#include "scrapers/coinmarketcap.h"
#include "scrapers/bitcoinfees.h"
#include "scrapers/current_weather.h"
#include "scrapers/wolfram.h"
#include "scrapers/poap.h"
#include "eth_transaction.h"
#include "eth_ecdsa.h"
#include "eth_abi.h"
#include "Enclave_t.h"
#include "external/keccak.h"
#include "Constants.h"
#include "time.h"
#include "log.h"
#include "encoding.h"

#include "hybrid_cipher.h"
#include "env.h"

const size_t MAX_LEN = 2000000;

int unseal_data(unsigned char* sealed_data,
    size_t sealed_data_len,
    unsigned char* unsealed_data,
    uint32_t* unsealed_data_len) {
  unsigned char sealed_bytes[MAX_LEN];
  try {
    size_t buffer_used = (size_t)ext::b64_pton(reinterpret_cast<char*>(sealed_data),
        sealed_bytes,
        sizeof sealed_bytes);
    if (buffer_used > 0) {
      auto sealed_sgx_t = reinterpret_cast<sgx_sealed_data_t*>(sealed_bytes);
      *unsealed_data_len = sgx_get_encrypt_txt_len(sealed_sgx_t);

      sgx_status_t st;
      st = sgx_unseal_data(sealed_sgx_t, NULL, 0, unsealed_data, unsealed_data_len);
      if (st != SGX_SUCCESS) {
        LL_CRITICAL("exception when unsealing dataset");
        return -1;
      }
    }
  }
  catch (const std::exception &e) {
    LL_CRITICAL("exception while handling request: %s", e.what());
    return -1;
  }
  return 0;
}

int seal_data(unsigned char* unsealed_data,
              uint32_t unsealed_data_len,
              unsigned char* sealed_data,
              size_t* sealed_data_len) {
  auto len = sgx_calc_sealed_data_size(0, unsealed_data_len);
  sgx_sealed_data_t *seal_buffer = (sgx_sealed_data_t *) malloc(MAX_LEN);
  sgx_status_t st = sgx_seal_data(0, NULL, unsealed_data_len,
      unsealed_data, len, seal_buffer);
  //memcpy(secret_sealed, seal_buffer, len);

  if (st != SGX_SUCCESS) {
    LL_CRITICAL("Failed to seal. Ecall returned %d", st);
    free(seal_buffer);
    return -1;
  }

  *sealed_data_len = static_cast<size_t>(
      ext::b64_ntop((unsigned char*)seal_buffer, len, (char*)sealed_data, MAX_LEN));
  free(seal_buffer);
  return 0;
}

/*
data format: user wallet (20 bytes) || source (1 byte) || data
*/
int raffle_run(
                   unsigned char* sealed_data, 
                   size_t sealed_data_len,
                   unsigned char* dataset,
                   size_t dataset_len,
                   unsigned char* newdata,
                   size_t* newdata_len,
                   unsigned char* resp) {
                   //unsigned char* signature) {
  // data_entry: 
	// "[0][wallet addr]!\n"
	// "[source][wallet addr]:[credential]\n"
  try {
    LL_INFO("run raffle!");
    LL_INFO("Random number %.*s from contract 0x%.*s", 
        sealed_data_len - 40, sealed_data + 40,
        40, sealed_data); 
    size_t num_entries = 0;
    size_t num_entries_poap = 0;
    bool new_identity = true;
    unsigned char participants[MAX_LEN];
    uint8_t decrypted_text[MAX_LEN];
    uint32_t decrypted_text_length = 0;
    int ret = unseal_data(dataset, dataset_len, decrypted_text, &decrypted_text_length);
    if (ret != 0) {
      LL_CRITICAL("Error when unsealing data");
      return -1;
    }
    LL_INFO("dateset unsealed");

    if (decrypted_text_length > 0) {
      size_t start = 0;
      while (start < decrypted_text_length) {
        if (decrypted_text_length - start >= 23 && decrypted_text[start + 21] == uint8_t('!')) {
          memcpy(resp, ucharToHexString(decrypted_text + start + 1, 20).c_str(), 40);
          LL_INFO("Raffle winner 0x%s already chosen", resp);
          LL_INFO("%d identities existed in database, %d used POAPs", num_entries, num_entries_poap);
          return 0;
        }
        memcpy(participants + num_entries * 20, decrypted_text + start + 1, 20);
        if ((int)decrypted_text[start] == TYPE_POAP) {
          num_entries_poap += 1;
        }
        start = start + 22;
        num_entries += 1;
        while (start < decrypted_text_length && decrypted_text[start] != '\n') {
          start += 1;
        }
        start = start + 1;
      }
    }
    LL_INFO("%d identities existed in database, %d used POAPs", num_entries, num_entries_poap);

    if (num_entries > 0) {
      auto rand_idx = get_random_number(sealed_data, sealed_data_len, num_entries);
      LL_INFO("random number: %d", rand_idx);
      memcpy(resp, ucharToHexString(participants + rand_idx * 20, 20).c_str(), 40);
      LL_INFO("winner address 0x%s", resp);
      
      memcpy(decrypted_text + decrypted_text_length, (char*)"\0", 1);
      memcpy(decrypted_text + decrypted_text_length + 1, participants + rand_idx * 20, 20);
      memcpy(decrypted_text + decrypted_text_length + 21, (char*)"!", 1);
      memcpy(decrypted_text + decrypted_text_length + 22, (char*)"\n", 1);
      ret = seal_data(decrypted_text, decrypted_text_length + 23, newdata, newdata_len);
      if (ret != 0) {
        LL_CRITICAL("Error when sealing data");
        return -1;
      }
      
      LL_INFO("Encrypted new database (%d bytes): %s", *newdata_len, newdata);
    } else {
      resp = (unsigned char*)"No participant yet";
    }
  }
  catch (const std::exception &e) {
    LL_CRITICAL("exception while handling request: %s", e.what());
    return -1;
  }
  return 0;
}

/*
data format: user wallet (20 bytes) || source (1 byte) || data
*/
int identity_token(
                   unsigned char* sealed_data, 
                   size_t sealed_data_len,
                   unsigned char* dataset,
                   size_t dataset_len,
                   unsigned char* newdata,
                   size_t* newdata_len,
                   int* resp) {
                   //unsigned char* signature) {
  string plain;
  try {  
    plain = decrypt_query(sealed_data, sealed_data_len);
  }
  catch (const std::exception &e) {
    LL_INFO("exception while decrypting request: %s", e.what());
    *resp = INVALID_QUERY;
    return 0;
  }
  LL_INFO("Sealed HTTP header decrypted.");
  
  auto data = plain.c_str();
  auto data_len = plain.size();
  if (data_len < 21) {
    LL_INFO("Invalid query format. Please send your query as [wallet address]||[source]||[data]");
    *resp = INVALID_QUERY;
    return 0;
  }
  unsigned char wallet_addr[20];
  memcpy(wallet_addr, data, 20);
  uint8_t source = data[20];
  data = data + 21;
  data_len -= 21;
  //LL_DEBUG("Source: %d", source);
  //LL_DEBUG("Wallet address: 0x%s", ucharToHexString(wallet_addr, 20).c_str());
  //LL_DEBUG("Decrypted HTTP header: %s", data);
  
  char http_resp[500] = {0};
  switch (source) {
    case TYPE_SSA: {
      SSAScraper scraper;
      switch (scraper.handle_long_resp(data, data_len, http_resp)) {
        case NO_ERROR:
          break;
        default: {
          *resp = INVALID_QUERY;
          return 0;
        }
      }
      break;
    }
    case TYPE_COINBASE: {
      CoinbaseScraper scraper;
      switch (scraper.handle_long_resp(data, data_len, http_resp)) {
        case NO_ERROR:
          break;
        default: {
          *resp = INVALID_QUERY;
          return 0;
        }
      }
      break;
    }
    case TYPE_POAP: {
      PoapScraper scraper;
      switch (scraper.handle_long_resp(data, data_len, http_resp)) {
        case NO_ERROR:
          break;
        case INVALID_PARAMS: {
          *resp = POAP_NOT_FOUND;
          return 0;
        }
        default: {
          *resp = INVALID_QUERY;
          return 0;
        }
      }
      break;
    }
    default: {
      *resp = INVALID_QUERY;
      return 0;
    }
  }
  //LL_DEBUG("[DEMO ONLY, TO BE SEALED] credential info (%d bytes): %s", strlen(http_resp), http_resp);
  string credential = (char*)http_resp;

  // data_entry: "[source][wallet addr]:[credential]"
  try {
    // encrypt all entries as a whole
    size_t num_entries = 0;
    bool new_identity = true;
    uint8_t decrypted_text[MAX_LEN];
    uint32_t decrypted_text_length = 0;
    int ret = unseal_data(dataset, dataset_len, decrypted_text, &decrypted_text_length);
    if (ret != 0) {
      LL_CRITICAL("Error when unsealing data");
      return -1;
    }
    LL_INFO("dateset unsealed");

    if (decrypted_text_length > 0) {
      size_t start = 0;
      while (start < decrypted_text_length) {
			  if (decrypted_text_length - start >= 23 && decrypted_text[start + 21] == uint8_t('!')) {
				  *resp = RAFFLE_END;
          LL_INFO("Raffle already ended"); 
				  return 0;
			  }

        num_entries += 1;
        start = start + 22;
        if (memcmp(http_resp, decrypted_text + start, credential.size()) == 0) {
          new_identity = false;
          LL_INFO("Identity found in dataset!"); // TODO!!!
          *resp = ID_EXISTS;
        }
        size_t pos = start;
        while (pos < decrypted_text_length && decrypted_text[pos] != '\n') {
          pos += 1;
        }
        start = pos + 1;
      }
    }
    LL_INFO("%d identities existed in database", num_entries);

    if (new_identity) { // TODO!!!
      LL_INFO("New identity!");
      *resp = NEW_ID;
      memcpy(decrypted_text + decrypted_text_length, &source, 1);
      memcpy(decrypted_text + decrypted_text_length + 1, wallet_addr, 20);
      memcpy(decrypted_text + decrypted_text_length + 21, (char*)":", 1);
      memcpy(decrypted_text + decrypted_text_length + 22, credential.c_str(), credential.size());
      memcpy(decrypted_text + decrypted_text_length + 22 + credential.size(), (char*)"\n", 1);
      ret = seal_data(decrypted_text, decrypted_text_length + 23 + credential.size(), newdata, newdata_len);
      if (ret != 0) {
        LL_CRITICAL("Error when sealing data");
        return -1;
      }
      LL_INFO("Encrypted new database (%d bytes): %s", *newdata_len, newdata);
    }
    /*
     * encrypt each entry one-by-one
    bool new_identity = true;
    string sealed = (char*)dataset;
    size_t start = 0;
    size_t pos = sealed.find("\n");
    while (pos != std::string::npos) {
      unsigned char secret_sealed[2000];
      size_t buffer_used = (size_t)ext::b64_pton(sealed.substr(start, pos - start).c_str(),
          secret_sealed,
          sizeof secret_sealed);

      auto secret = reinterpret_cast<sgx_sealed_data_t*>(secret_sealed);
      uint32_t decrypted_text_length = sgx_get_encrypt_txt_len(secret);
      uint8_t decrypted_text[decrypted_text_length];
      sgx_status_t st;

      st = sgx_unseal_data(secret, NULL, 0, decrypted_text, &decrypted_text_length);
      if (st != SGX_SUCCESS) {
        LL_CRITICAL("exception when unsealing dataset");
        return -1;
      }

      string entry = (char*)decrypted_text;
      if (memcmp(decrypted_text + 21, http_resp, credential.size()) == 0) {
        new_identity = false;
        LL_INFO("Identity found in dataset!"); // Issue to the original wallet.");
        *resp = ID_EXISTS;
        break;
      }
      start = pos + 1;
      pos = sealed.find("\n", start);
    }

    if (new_identity) {
      LL_INFO("New identity!");
      *resp = NEW_ID;
      unsigned char new_entry[100];
      memcpy(new_entry, wallet_addr, 20);
      memcpy(new_entry + 20, (char*)":", 1);
      memcpy(new_entry + 21, credential.c_str(), credential.size());
      auto len = sgx_calc_sealed_data_size(0, 21 + credential.size());
      sgx_sealed_data_t *seal_buffer = (sgx_sealed_data_t *) malloc(len);
      sgx_status_t st = sgx_seal_data(0, NULL, 21 + credential.size(),
          new_entry, len, seal_buffer);
      unsigned char secret_sealed[2000];
      memcpy(secret_sealed, seal_buffer, len);
      free(seal_buffer);

      if (st != SGX_SUCCESS) {
        LL_CRITICAL("Failed to seal. Ecall returned %d", st);
        return -1;
      }

      *newdata_len = static_cast<size_t>(
          ext::b64_ntop(secret_sealed, len, (char*)newdata, 2000));

      LL_INFO("Encrypted new identity (%d bytes): %s", *newdata_len, newdata);
    }
 */   
  }
  catch (const std::exception &e) {
    LL_CRITICAL("exception while handling request: %s", e.what());
    return -1;
  }

/*  
  try {
    LL_INFO("Signing message");
    uint8_t contract_wallet[80];
    bytes32 hash(0);
    memcpy(contract_wallet, ETH_HASH_PREFIX.c_str(), ETH_HASH_PREFIX.size());
    memcpy(contract_wallet + ETH_HASH_PREFIX.size(), contract_addr, 20);
    memcpy(contract_wallet + ETH_HASH_PREFIX.size() + 20, wallet_addr, 20);
    auto ret = keccak(contract_wallet, ETH_HASH_PREFIX.size() + 40, &hash[0], 32);
    if (ret) {
      LL_CRITICAL("Keccak256 returned %d\n", ret);
      return TC_INTERNAL_ERROR;
    }
    
    bytes32 r;
    bytes32 s;
    uint8_t v;
    ret = ecdsa_sign(&hash[0], 32, &r[0], &s[0], &v);
    if (ret) {
      LL_CRITICAL("Error: signing returned %d", ret);
      return TC_INTERNAL_ERROR;
    }
    
    memcpy(signature, &v, 1);
    memcpy(signature + 1, &r[0], 32);
    memcpy(signature + 33, &s[0], 32);
    
  }
  catch (const std::exception &e) {
    LL_CRITICAL("%s", e.what());
    return INTERNAL_ERR;
  }
*/
  return 0;
}



/*
 * testing data
 *
Request(app, 1, ['FJM273', pad(1492100100, 64)]);
Request(app, 2, ['f68d2a32cf17b1312c6db3f236a38c94', '4c9f92f6ec1e2a20a1413d0ac1b867a3', '32884794', pad(1456380265, 64), pad(1, 64), 'Portal']);
Request(app, 3, ['GOOG', pad(1262390400,64)]);;
Request(app, 4, ['1ZE331480394808282']);
Request(app, 5, ['bitcoin']);
 */

int handle_request(int nonce,
                   uint64_t id,
                   uint64_t type,
                   const uint8_t *data,
                   size_t data_len,
                   uint8_t *raw_tx,
                   size_t *raw_tx_len) {
  try {
    string tc_address = getContractAddress();
    LL_DEBUG("serving tc address: %s", tc_address.c_str());

    return do_handle_request(nonce, id, type, data, data_len, raw_tx, raw_tx_len);
  }
  catch (const std::exception &e) {
    LL_CRITICAL("exception while handling request: %s", e.what());
  }
  catch (...) {
    LL_CRITICAL("unknown error while handling request");
  }

  return TC_INTERNAL_ERROR;
}

int do_handle_request(int nonce,
                      uint64_t id,
                      uint64_t type,
                      const uint8_t *data,
                      size_t data_len,
                      uint8_t *raw_tx,
                      size_t *raw_tx_len) {
  bytes resp_data;
  int error_flag = 0;

  switch (type) {
    /*
    case TYPE_LOOP_THROUGH: {
      printf_sgx("nonce: %d\n", nonce);
      printf_sgx("id: %" PRIu64 "\n", id);
      printf_sgx("type: %" PRIu64 "\n", type);
      printf_sgx("data len: %zu\n", data_len);

      if (data_len > TC_REQUEST_PAYLOAD_LIMIT) {
        LL_CRITICAL("data (%zu bytes) exceeds limit (%d bytes)", data_len, TC_REQUEST_PAYLOAD_LIMIT);
        return -1;
      }
      dump_buf("data:", data, data_len);
      return -1;
    }
     */
  case TYPE_BITCOIN_FEE: {
    BitcoinFees bitcoinFees;
    int fastestFee;
    switch (bitcoinFees.handle(data, data_len, &fastestFee)) {
    case UNKNOWN_ERROR:
    case WEB_ERROR:
      error_flag = TC_INTERNAL_ERROR;
      break;
    case INVALID_PARAMS:
      error_flag = TC_INPUT_ERROR;
      break;
    case NO_ERROR:
      append_as_uint256(resp_data, fastestFee, sizeof(fastestFee));
      break;
    }
    break;
  }
  case TYPE_FLIGHT_INS: {
    FlightScraper flightHandler;
    int delay = 0;
    switch (flightHandler.handle(data, data_len, &delay)) {
    case UNKNOWN_ERROR:
    case WEB_ERROR:
      error_flag = TC_INTERNAL_ERROR;
      break;
    case INVALID_PARAMS:
      error_flag = TC_INPUT_ERROR;
      break;
    case NO_ERROR:
      append_as_uint256(resp_data, delay, sizeof(delay));
      break;
    };
    break;
  }
  case TYPE_STEAM_EX: {
    SteamScraper steamHandler;
    int found;
    switch (steamHandler.handle(data, data_len, &found)) {
    case UNKNOWN_ERROR:
    case WEB_ERROR:
      error_flag = TC_INTERNAL_ERROR;
      break;
    case INVALID_PARAMS:
      error_flag = TC_INPUT_ERROR;
      break;
    case NO_ERROR:
      append_as_uint256(resp_data, found, sizeof(found));
      break;
    }
    break;
  }
  case TYPE_FINANCE_INFO: {
    YahooYQLStock yahooYQLStock;
    int closing_price = 0;
    switch (yahooYQLStock.handle(data, data_len, &closing_price)) {
    case INVALID_PARAMS:
      error_flag = TC_ERR_FLAG_INVALID_INPUT;
      break;
    case WEB_ERROR:
      error_flag = TC_INTERNAL_ERROR;
      break;
    case NO_ERROR:
      LL_DEBUG("closing pricing is %d", closing_price);
      append_as_uint256(resp_data, closing_price, sizeof(closing_price));
      break;
    default:
      LL_CRITICAL("unknown state!");
      error_flag = TC_ERR_FLAG_INTERNAL_ERR;
    }
    break;
  }
  case TYPE_UPS_TRACKING: {
    LL_CRITICAL("not supported yet");
    error_flag = TC_ERR_FLAG_INTERNAL_ERR;
    break;
    USPSScraper uSPSScraper;
    int pkg_status;
    switch (uSPSScraper.handle(data, data_len, &pkg_status)) {
    case UNKNOWN_ERROR:
    case WEB_ERROR:
      error_flag = TC_INTERNAL_ERROR;
      break;
    case INVALID_PARAMS:
      error_flag = TC_INPUT_ERROR;
    case NO_ERROR:
      append_as_uint256(resp_data, pkg_status, sizeof(pkg_status));
      break;
    };
    break;
  }
  case TYPE_COINMARKET: {
    CoinMarket coinMarket;
    int coin_value;
    switch (coinMarket.handle(data, data_len, &coin_value)) {
    case UNKNOWN_ERROR:
    case WEB_ERROR:
      error_flag = TC_INTERNAL_ERROR;
      break;
    case INVALID_PARAMS:
      error_flag = TC_INPUT_ERROR;
      break;
    case NO_ERROR:
      append_as_uint256(resp_data, coin_value, sizeof(coin_value));
      break;
    };
    break;
  }
  case TYPE_WEATHER: {
    WeatherScraper weatherScraper;
    int temperature;
    switch (weatherScraper.handle(data, data_len, &temperature)) {
    case UNKNOWN_ERROR:
    case WEB_ERROR:
      error_flag = TC_INTERNAL_ERROR;
      break;
    case INVALID_PARAMS:
      error_flag = TC_INPUT_ERROR;
      break;
    case NO_ERROR:
      append_as_uint256(resp_data, temperature, sizeof(temperature));
      break;
    };
    break;
  }
    /*
    case TYPE_CURRENT_VOTE: {
      double r1 = 0, r2 = 0;
      yahoo_current("GOOG", &r1);
      google_current("GOOG", &r2);
      break;
    }
     */
  case TYPE_WOLFRAM: {
    WolframScraper wolframScraper;
    string status;
    switch (wolframScraper.handle(data, data_len, &status)) {
    case UNKNOWN_ERROR:
    case WEB_ERROR:
      error_flag = TC_INTERNAL_ERROR;
      break;
    case INVALID_PARAMS:
      error_flag = TC_INPUT_ERROR;
      break;
    case NO_ERROR:
      LL_INFO("wolfram returned: %s", status.c_str());
      resp_data.insert(resp_data.end(), status.begin(), status.end());
      break;
    default:
      error_flag = TC_ERR_FLAG_INVALID_INPUT;
      break;
    }
    break;
  }
  case TYPE_FLIGHT_INS_ENC: {
    FlightScraper flightHandler;
    int delay = 0;
    switch (flightHandler.handleEncryptedQuery(data, data_len, &delay)) {
    case UNKNOWN_ERROR:
    case WEB_ERROR:
      error_flag = TC_INTERNAL_ERROR;
      break;
    case INVALID_PARAMS:
      error_flag = TC_INPUT_ERROR;
      break;
    case NO_ERROR:
      append_as_uint256(resp_data, delay, sizeof(delay));
      break;
    };
    break;
  }
    /*
    case TYPE_ENCRYPT_TEST: {
      HybridEncryption dec_ctx;
      ECPointBuffer tc_pubkey;
      dec_ctx.queryPubkey(tc_pubkey);

      string cipher_b64(data, data + data_len);
      hexdump("encrypted query: ", data, data_len);

      try {
        HybridCiphertext cipher = dec_ctx.decode(cipher_b64);
        vector<uint8_t> cleartext;
        dec_ctx.hybridDecrypt(cipher, cleartext);
        hexdump("decrypted message", &cleartext[0], cleartext.size());

        // decrypted message is the base64 encoded data
        string encoded_message(cleartext.begin(), cleartext.end());
        uint8_t decrypted_data[cleartext.size()];
        int decrypted_data_len = ext::b64_pton(encoded_message.c_str(),
                                               decrypted_data, sizeof decrypted_data);

        if (decrypted_data_len == -1) {
          throw runtime_error("can't decode user message");
        }

        hexdump("decoded message", decrypted_data, (size_t) decrypted_data_len);
      }
      catch (const std::exception &e) {
        LL_CRITICAL("decryption error: %s. See dump above.", e.what());
      }
      catch (...) {
        LL_CRITICAL("unknown exception happened while decrypting. See dump above.");
      }

      return TC_INTERNAL_TEST;
    }
     */
  default :
    LL_CRITICAL("Unknown request type: %"
                    PRIu64, type);
    error_flag = TC_ERR_FLAG_INVALID_INPUT;
  }

  return form_transaction(nonce, id, type, data, data_len, error_flag, resp_data, raw_tx, raw_tx_len);
}
