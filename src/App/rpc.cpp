#include "rpc.h"

#include <sgx_quote.h>

#include "Common/external/base64.hxx"
#include "Common/Constants.h"
#include "Common/version.h"
#include "config.h"
#include "Enclave_u.h"
#include "attestation.h"
#include "converter.h"
#include "tc_exception.h"

#include <iostream>
#include <fstream>
using namespace std;

extern std::mutex mtx;           // mutex for critical section

::grpc::Status RpcServer::attest(::grpc::ServerContext* context,
                                 const ::rpc::Empty* request,
                                 ::rpc::Attestation* response)
{
  try {
    std::vector<uint8_t> attestation;
    get_attestation(this->eid, &attestation);

    const auto* mr_enclave_p =
        (reinterpret_cast<sgx_quote_t*>(attestation.data()))
            ->report_body.mr_enclave.m;

    char b64_buf[4096] = {0};
    int buf_used = ext::b64_ntop(
        attestation.data(), attestation.size(), b64_buf, sizeof b64_buf);
    if (buf_used > 0) {
      response->set_quote(b64_buf);
    }

    buf_used =
        ext::b64_ntop(mr_enclave_p, SGX_HASH_SIZE, b64_buf, sizeof b64_buf);
    if (buf_used > 0) {
      response->set_mr_enclave(b64_buf);
    }
  } catch (tc::EcallException& e) {
    response->set_error(e.what());
  } catch (std::exception& e) {
    response->set_error(e.what());
  } catch (...) {
    response->set_error("unknown");
  }

  return grpc::Status::OK;
}

::grpc::Status RpcServer::raffle(::grpc::ServerContext* context,
                                 const ::rpc::Data* request,
                                 ::rpc::Data* response)
{

  mtx.lock();
  //LOG4CXX_ERROR(this->logger, "Receive input data: " << request->data());
int ecall_ret = TC_SUCCESS;

  //unsigned char signature[65];
  unsigned char resp[100] = {0};
  try {
    ifstream dataset;
    dataset.open(config->getDatasetPath());
    string file = "";
    if (dataset.is_open()) {
      string line;
      while (getline (dataset,line)) {
        file = file + line + "\n";
      }
      dataset.close();
    }
  
    auto req_data = request->data();
    unsigned char newdata[2000000] = {0};
    size_t newdata_len = 0;
    auto st = raffle_run(eid,
                             &ecall_ret,
                             reinterpret_cast<unsigned char*>(const_cast<char*>(req_data.c_str())), 
                             req_data.length(),
                             reinterpret_cast<unsigned char*>(const_cast<char*>(file.c_str())),
                             file.length(),
                             newdata,
                             &newdata_len,
                             resp
                             //signature
                             );

    if (st != SGX_SUCCESS || ecall_ret != TC_SUCCESS) {
      LOG4CXX_ERROR(this->logger, "ecall to handle_request failed with " << st)
        throw std::runtime_error("ecall failed");
    } else {
      this->logger->info("ecall succeeds");

      if (newdata_len > 0) {
        ofstream dataset;
        dataset.open (config->getDatasetPath(), ios::out | ios::binary); 
        if (dataset.is_open()) {
          dataset << newdata << endl;
          dataset.close();
          this->logger->info("new identity written in dataset (sealed)");
        }
      }
    }
  }
  catch (const std::exception& e) {
    LOG4CXX_ERROR(this->logger, "exception: " << e.what())
  }
  
  mtx.unlock();
  
  response->set_data(string(reinterpret_cast<char*>(resp)));
  return grpc::Status::OK;
}

::grpc::Status RpcServer::id_nft(::grpc::ServerContext* context,
                                 const ::rpc::Data* request,
                                 ::rpc::Data* response)
{

  mtx.lock();
  //LOG4CXX_ERROR(this->logger, "Receive input data: " << request->data());
int ecall_ret = TC_SUCCESS;

  //unsigned char signature[65];
  int resp;
  try {
    ifstream dataset;
    dataset.open(config->getDatasetPath());
    string file = "";
    if (dataset.is_open()) {
      string line;
      while (getline (dataset,line)) {
        file = file + line + "\n";
      }
      dataset.close();
    }
   
    auto req_data = request->data();
    unsigned char newdata[2000000] = {0};
    size_t newdata_len = 0;
    auto st = identity_token(eid,
                             &ecall_ret,
                             reinterpret_cast<unsigned char*>(const_cast<char*>(req_data.c_str())), 
                             req_data.length(),
                             reinterpret_cast<unsigned char*>(const_cast<char*>(file.c_str())),
                             file.length(),
                             newdata,
                             &newdata_len,
                             &resp
                             //signature
                             );

    if (st != SGX_SUCCESS || ecall_ret != TC_SUCCESS) {
      LOG4CXX_ERROR(this->logger, "ecall to handle_request failed with " << st)
        throw std::runtime_error("ecall failed");
    } else {
      this->logger->info("ecall succeeds");

      if (newdata_len > 0) {
        ofstream dataset;
        dataset.open (config->getDatasetPath(), ios::out | ios::binary); 
        if (dataset.is_open()) {
          dataset << newdata << endl;
          dataset.close();
          this->logger->info("new identity written in dataset (sealed)");
        }
      }
    }
  }
  catch (const std::exception& e) {
    LOG4CXX_ERROR(this->logger, "exception: " << e.what())
  }
  
  mtx.unlock();
  
  //char b64_buf[4096] = {0};
  //int buf_used = ext::b64_ntop(
  //      signature, 65, b64_buf, sizeof b64_buf);
  //if (buf_used > 0) {
  //  response->set_data(b64_buf);
  //}
  if (resp == ID_EXISTS) {
    response->set_data("Already registered");
  } else if (resp == NEW_ID) {
    response->set_data("Newly registered");
  } else if (resp == POAP_NOT_FOUND) {
    response->set_data("POAP not found");
  } else if (resp == RAFFLE_END) {
    response->set_data("Raffle already ended");
  } else {
    response->set_data("Invalid query");
  }

  return grpc::Status::OK;
}

::grpc::Status RpcServer::participate(::grpc::ServerContext* context,
                                 const ::rpc::Data* request,
                                 ::rpc::Empty* response)
{
  return grpc::Status::OK;
}

::grpc::Status RpcServer::status(::grpc::ServerContext* context,
                                 const ::rpc::Empty* request,
                                 ::rpc::Status* response)
{
  response->set_version(GIT_COMMIT_HASH);
  return grpc::Status::OK;
}
::grpc::Status RpcServer::process(::grpc::ServerContext* context,
                                  const ::rpc::Request* request,
                                  ::rpc::Response* response)
{
  int ecall_ret = TC_SUCCESS;

  try {
    auto req_id = request->id();
    auto req_type = request->type();
    auto req_data_ptr =
        reinterpret_cast<const uint8_t*>(request->data().data());
    auto req_data_len = request->data().size();
    auto nonce = request->nonce();

    size_t resp_data_len;
    auto st = handle_request(eid,
                             &ecall_ret,
                             nonce,
                             req_id,
                             req_type,
                             req_data_ptr,
                             req_data_len,
                             resp_buffer,
                             &resp_data_len);

    if (st != SGX_SUCCESS || ecall_ret != TC_SUCCESS) {
      LOG4CXX_ERROR(this->logger, "ecall to handle_request failed with " << st)
      throw std::runtime_error("ecall failed");
    } else {
      this->logger->info("ecall succeeds");

      string resp_txn = bufferToHex(resp_buffer, resp_data_len, true);
      LOG4CXX_INFO(this->logger, "response tx: " << resp_txn);

      response->set_error_code(0);
      response->set_response_tx((const char*)resp_buffer, resp_data_len);
 
      return grpc::Status::OK;
    }
  }

  catch (const std::exception& e) {
    LOG4CXX_ERROR(this->logger, "exception: " << e.what())
  }

  // error code defined in Constants.h
  response->set_error_code(ecall_ret);
  return grpc::Status::OK;
}
