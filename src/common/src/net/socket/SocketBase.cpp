#include "SocketBase.h"

#include <logger/logger.h>
#include <nn/nifm.h>
#include <regex>

namespace {
    constexpr inline auto DefaultTcpAutoBufferSizeMax = 192 * 1024; /* 192kb */
    constexpr inline auto MinTransferMemorySize = (2 * DefaultTcpAutoBufferSizeMax + (128 * 1024));
    constexpr inline auto MinSocketAllocatorSize = 128 * 1024;

    constexpr inline auto SocketAllocatorSize = MinSocketAllocatorSize * 1;
    constexpr inline auto TransferMemorySize = MinTransferMemorySize * 1;

    constexpr inline auto SocketPoolSize = SocketAllocatorSize + TransferMemorySize;
};

char socketPool[SocketPoolSize] __attribute__((aligned(0x4000)));

in_addr SocketBase::resolve(const std::string& domain) {
    auto ent = nn::socket::GetHostByName(domain.c_str());
    return *reinterpret_cast<in_addr*>(*ent->h_addr_list);
}

std::optional<std::tuple<bool, const std::string, u16, const std::string>> SocketBase::parseURL(const char *url) {
    const std::string s(url);
    return parseURL(s).value();
}

std::optional<std::tuple<bool, const std::string, u16, const std::string>> SocketBase::parseURL(const std::string& url) {
    static auto url_regex = std::regex(R"((?:ws|http)(s?)://([^:/]+)(?::(\d+))?(/.+)?)");
    std::smatch m;
    std::regex_match(url, m, url_regex);

    if (m.empty()) return std::nullopt;

    std::string domain = m[2];
    std::string port;
    std::string path;
    bool isSSL = !m[1].str().empty();

    if (m.size() >= 4) {
        port = m[3].str();
        if (port.empty()) {
            if (isSSL) {
                port = "443";
            } else {
                port = "80";
            }
        }
    } else {
        if (isSSL) {
            port = "443";
        } else {
            port = "80";
        }
    }
    if (m.size() >= 5) {
        path = m[4].str();
        if (path.empty()) {
            path = "/";
        }
    } else {
        path = "/";
    }

    u16 portInt = std::stoi(port);
    return std::make_tuple(isSSL, std::move(domain), portInt, std::move(path));
}

static bool didInit = false;
static nn::Result initResult = nn::ResultSuccess();

nn::Result SocketBase::initCommon() {
    if (!didInit) {
        didInit = true;

        Logger::log("Initializing network.\n");
        if (nn::nifm::Initialize().IsFailure()) {
            Logger::log("Failed to initialize nifm.\n");
            initResult = err::ResultInitializationFailed();
            return initResult;
        };

        nn::nifm::SubmitNetworkRequest();
        while (nn::nifm::IsNetworkRequestOnHold()) {}

        if (!nn::nifm::IsNetworkAvailable()) {
            Logger::log("Network not available.\n");
            initResult =  err::ResultUnknownError();
            return initResult;
        }

        if (nn::socket::Initialize(socketPool, SocketPoolSize, SocketAllocatorSize, 0xE).IsFailure()) {
            Logger::log("Failed to initialize socket pool.\n");
            initResult =  err::ResultInitializationFailed();
        }
    }

    return initResult;
}

int SocketBase::send(const std::string& message) {
    return send(message.c_str(), message.length());
}

int SocketBase::sendAll(const void* buf, int size) {
    int bytesSent = 0;
    while (bytesSent < size) {
        auto sent = send(&static_cast<const char*>(buf)[bytesSent], size - bytesSent);
        if (sent < 0) {
            return sent;
        }
        bytesSent += sent;
    }
    return bytesSent;
}

int SocketBase::sendAll(const std::string& message) {
    return sendAll(message.c_str(), message.length());
}

int SocketBase::recvAll(void* buf, int size) {
    int bytesReceived = 0;
    while (bytesReceived < size) {
        auto received = recv(&static_cast<char*>(buf)[bytesReceived], size - bytesReceived);
        if (received < 0) {
            return received;
        }
        bytesReceived += received;
    }
    return bytesReceived;
}
