#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>
#include <mbedtls/net_sockets.h>

#include "socket/SocketBase.h"

class ClientWithHttp {
    bool sslInitDone = false;
    mbedtls_entropy_context entropy {};
    mbedtls_ctr_drbg_context ctr_drbg {};
    mbedtls_ssl_config conf {};

protected:
    bool initSSL();
    std::unique_ptr<SocketBase> makeSocket(bool useSSL);

protected:
    std::unordered_map<std::string, std::string> defaultHeaders;

    std::string buildRequest(const std::string& method, const std::string& path, const std::unordered_map<std::string, std::string>& headers = {}, const std::string& body = "") const;
    static std::tuple<int, std::unordered_map<std::string, std::string>, std::string> parseResponse(const std::string& response);
};
