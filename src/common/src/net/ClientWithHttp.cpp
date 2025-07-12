#include "ClientWithHttp.h"

#include <cstring>
#include <format>
#include <logger/logger.h>

#include "socket/RawSocket.h"
#include "socket/SSLSocket.h"

namespace {
    void debug_func(void *, int level, const char * file, int line, const char * str) {
        Logger::log("[MBEDTLS] %s", str);
    }
}

bool ClientWithHttp::initSSL() {
    if (sslInitDone) return true;

    mbedtls_ssl_config_init(&conf);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_entropy_init(&entropy);

    if (mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, reinterpret_cast<const unsigned char*>("Test"), std::strlen("Test")) != 0) {
        return false;
    }

    if (mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT) != 0) {
        return false;
    }
    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE);

    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
    mbedtls_ssl_conf_dbg(&conf, debug_func, nullptr);

    sslInitDone = true;
    return true;
}

std::unique_ptr<SocketBase> ClientWithHttp::makeSocket(bool useSSL) {
    if (useSSL) {
        initSSL();
        return std::make_unique<SSLSocket>(entropy, ctr_drbg, conf);
    } else {
        return std::make_unique<RawSocket>();
    }
}

std::string ClientWithHttp::buildRequest(const std::string& method, const std::string& path, const std::unordered_map<std::string, std::string>& headers, const std::string& body) const {
    std::string headersAsString;
    std::unordered_map<std::string, std::string> combinedHeaders = defaultHeaders;

    for (const auto& header : headers) {
        combinedHeaders.emplace(header.first, header.second);
    }

    if (!body.empty()) {
        combinedHeaders.emplace("Content-Length", std::to_string(body.length()));
    }
    if (!combinedHeaders.contains("Connection")) {
        combinedHeaders.emplace("Connection", "close");
    }

    for (auto& combinedHeader : combinedHeaders) {
        headersAsString += combinedHeader.first + ": " + combinedHeader.second + "\r\n";
    }

    if (body.empty()) {
        return std::format("{} {} HTTP/1.1\r\n{}\r\n", method, path, std::move(headersAsString));
    }

    return std::format("{} {} HTTP/1.1\r\n{}\r\n{}", method, path, std::move(headersAsString), body);
}

std::tuple<int, std::unordered_map<std::string, std::string>, std::string> ClientWithHttp::parseResponse(const std::string& response) {
    int i = 0;
    int j = 0;

    int httpCode = 0;
    std::unordered_map<std::string, std::string> headers;
    std::string body;

    while ((j = response.find("\r\n", i)) != std::string::npos) {
        auto line = response.substr(i, j - i);

        if (line.empty()) {
            // Body starts on next line
            body = response.substr(j + 2);
            break;
        } else if (i == 0) {
            // HTTP Code
            auto codeStr = response.substr(9, 3);
            httpCode = std::stoi(codeStr);
        } else {
            // Header
            auto delimPos = line.find(':');
            auto key = line.substr(0, delimPos);
            auto value = line.substr(delimPos + 2, line.size() - delimPos - 1);
            headers.emplace(std::move(key), std::move(value));
        }

        i = j + 2;
    }

    return std::make_tuple(httpCode, std::move(headers), std::move(body));
}
