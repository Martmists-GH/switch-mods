#pragma once
#include <optional>
#include <string>
#include <nn/types.h>
#include <nn/socket.h>

enum SocketState {
    UNINITIALIZED,
    INITIALIZED,
    CONNECTED,
    DISCONNECTED,
    ERRORED,
    CLOSED
};

namespace err {
    R_DEFINE_NAMESPACE_RESULT_MODULE(99);
    R_DEFINE_ERROR_RESULT(UnknownError, 1);
    R_DEFINE_ERROR_RESULT(NetworkNotAvailable, 1);
    R_DEFINE_ERROR_RESULT(ConnectionFailed, 1);
    R_DEFINE_ERROR_RESULT(InitializationFailed, 1);
}

#define htons(x) ((x & 0xff00) >> 8) | ((x & 0x00ff) << 8)
#define ntohs(x) htons(x)
#define htonl(x) (((uint32_t)htons((x) & 0xFFFF) << 16) | htons((x) >> 16))
#define ntohl(x) htonl(x)
#define htonll(x) (((uint64_t)htonl((x) & 0xFFFFFFFF) << 32) | htonl((x) >> 32))
#define ntohll(x) htonll(x)

class SocketBase {
protected:
    SocketState state = UNINITIALIZED;

    static in_addr resolve(const std::string& domain);
    static nn::Result initCommon();

public:
    static std::optional<std::tuple<bool, const std::string, u16, const std::string>> parseURL(const char *url);
    static std::optional<std::tuple<bool, const std::string, u16, const std::string>> parseURL(const std::string& url);

    virtual ~SocketBase() = default;
    virtual nn::Result connect(const std::string& host, u16 port) = 0;
    virtual int send(const void* buf, int size) = 0;
    int send(const std::string& message);
    int sendAll(const void* buf, int size);
    int sendAll(const std::string& message);
    virtual int recv(void* buf, int size) = 0;
    int recvAll(void* buf, int size);
    virtual void close() = 0;
};



