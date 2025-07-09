#pragma once
#include "SocketBase.h"
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>
#include <mbedtls/net_sockets.h>

class SSLSocket final : public SocketBase {
    bool didOwnInit = false;
    mbedtls_entropy_context entropy {};
    mbedtls_ctr_drbg_context ctr_drbg {};
    mbedtls_ssl_config conf {};

    mbedtls_net_context socket {};
    mbedtls_ssl_context ssl {};

    bool init();

public:
    SSLSocket();

    SSLSocket(
        const mbedtls_entropy_context& entropy,
        const mbedtls_ctr_drbg_context& ctr_drbg,
        const mbedtls_ssl_config& conf
    );
    ~SSLSocket() override;

    nn::Result connect(const std::string& host, u16 port) override;
    int send(const void* buf, int size) override;
    int recv(void* buf, int size) override;
    void close() override;
};
