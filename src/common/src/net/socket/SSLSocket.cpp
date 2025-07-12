#include "SSLSocket.h"

#include <cstring>
#include <logger/logger.h>
#include <mbedtls/debug.h>

namespace {
    void debug_func(void *, int level, const char * file, int line, const char * str) {
        Logger::log("[MBEDTLS] %s\n", str);
    }
}

bool SSLSocket::init() {
    mbedtls_net_init(&socket);
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_set_bio(&ssl, &socket, mbedtls_net_send, mbedtls_net_recv, nullptr);

    if (mbedtls_ssl_setup(&ssl, &conf) != 0) {
        Logger::log("SSL setup failed!\n");
        return false;
    }

    return true;
}

SSLSocket::SSLSocket() {
    mbedtls_ssl_config_init(&conf);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_entropy_init(&entropy);

    if (mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, reinterpret_cast<const unsigned char*>("Test"), std::strlen("Test")) != 0) {
        Logger::log("SSL drbg_seed failed!\n");
        goto exit;
    }

    if (mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT) != 0) {
        Logger::log("SSL config_defaults failed!\n");
        goto exit;
    }
    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE);

    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
    mbedtls_ssl_conf_dbg(&conf, debug_func, nullptr);
    mbedtls_debug_set_threshold(1);

    if (!init()) {
        goto exit;
    }
    state = INITIALIZED;
    didOwnInit = true;

    return;

exit:
    close();
    state = ERRORED;
}

SSLSocket::SSLSocket(
    const mbedtls_entropy_context& entropy,
    const mbedtls_ctr_drbg_context& ctr_drbg,
    const mbedtls_ssl_config& conf
) : entropy(entropy), ctr_drbg(ctr_drbg), conf(conf) {
    init();
    state = INITIALIZED;
}

SSLSocket::~SSLSocket() {
    close();
}

nn::Result SSLSocket::connect(const std::string& host, u16 port) {
    if (state != INITIALIZED) {
        close();
        state = ERRORED;
        Logger::log("SSL tried to connect while not initialized!\n");
        return err::ResultInitializationFailed();
    }

    auto portStr = std::to_string(port);
    nn::Result res = nn::ResultSuccess();

    if (mbedtls_net_connect(&socket, host.c_str(), portStr.c_str(), MBEDTLS_NET_PROTO_TCP) != 0) {
        Logger::log("SSL connect failed!\n");
        res = err::ResultConnectionFailed();
        goto exit;
    }

    if (mbedtls_ssl_set_hostname(&ssl, host.c_str()) != 0) {
        Logger::log("SSL set_hostname failed!\n");
        res = err::ResultUnknownError();
        goto exit;
    }

    if (mbedtls_ssl_handshake(&ssl) != 0) {
        Logger::log("SSL handshake failed!\n");
        res = err::ResultUnknownError();
        goto exit;
    }

    state = CONNECTED;
    return res;

exit:
    close();
    state = ERRORED;
    return res;
}

int SSLSocket::send(const void* buf, int size) {
    if (size == 0) return 0;
    if (state != CONNECTED) {
        Logger::log("SSL Socket tried to send while not connected!\n");
        return -1;
    }

    auto ret = mbedtls_ssl_write(&ssl, static_cast<const unsigned char*>(buf), size);
    if (ret >= 0) return size;
    if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
        return 0;
    }

    close();
    state = ERRORED;
    return ret;
}

int SSLSocket::recv(void* buf, int size) {
    if (size == 0) {
        Logger::log("Attempted to read into an empty buffer!");
        return 0;
    }
    if (state != CONNECTED) {
        Logger::log("Attempted to read while socket is not connected!");
        return -1;
    }

    auto ret = mbedtls_ssl_read(&ssl, static_cast<unsigned char*>(buf), size);

    if (ret >= 0) return ret;

    if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
        return 0;
    }

    close();
    state = ERRORED;
    return -1;
}

void SSLSocket::close() {
    state = CLOSED;
    mbedtls_ssl_close_notify(&ssl);
    mbedtls_net_free(&socket);
    mbedtls_ssl_free(&ssl);

    if (didOwnInit) {
        mbedtls_ssl_config_free(&conf);
        mbedtls_ctr_drbg_free(&ctr_drbg);
        mbedtls_entropy_free(&entropy);
    }
}
