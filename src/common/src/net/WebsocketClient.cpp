#include "WebsocketClient.h"

#include <format>
#include <random>
#include <hk/svc/api.h>
#include <logger/logger.h>
#include <nn/os/os_Mutex.h>

void WebsocketClient::reconnect(const std::string& url) {
    if (isOpen) {
        close();
    }

    auto res = SocketBase::parseURL(url);
    if (!res.has_value()) {
        return;
    }

    auto tuple = res.value();
    auto isSSL = std::get<0>(tuple);

    sock = makeSocket(isSSL);
    sock->connect(std::get<1>(tuple), std::get<2>(tuple));

    auto body = buildRequest("GET", std::get<3>(tuple), {
        {"Upgrade", "websocket"},
        {"Connection", "Upgrade"},
        {"Host", std::format("{}:{}", std::get<1>(tuple), std::get<2>(tuple))},
        {"Sec-WebSocket-Key", "dGhlIHNhbXBsZSBub25jZQ=="},
        {"Sec-WebSocket-Version", "13"},
    });

    if (const int r = sock->sendAll(body); r < 0) {
        Logger::log("Websocket send failed: %d\n", r);
        close();
    } else {
        std::string collected;
        char buffer[1024];
        do {
            memset(buffer, 0, 1024);
            auto recv = sock->recv(buffer, 1024);
            if (recv < 0) {
                Logger::log("Websocket recv failed: %d\n", recv);
                close();
                return;
            }
            collected.append(std::string(buffer, recv));
            // Logger::log("Collected response:\n%s\n", collected.c_str());
        } while (collected.find("\r\n\r\n") == std::string::npos);
        Logger::log("Websocket ready!\n");
        isOpen = true;
    }
}


WebsocketClient::WebsocketClient(const std::string& url) {
    reconnect(url);
}

WebsocketClient::~WebsocketClient() {
    close();
}


bool WebsocketClient::open() const {
    return isOpen;
}

void WebsocketClient::close() {
    isOpen = false;
    // TODO: Send Close packet?
    sock->close();
}

bool WebsocketClient::send(const std::string& message) {
    if (!isOpen) return false;

    size_t payloadSize = message.size();
    std::vector<u8> frame;

    frame.push_back(0x81);

    if (payloadSize <= 125) {
        frame.push_back(0x80 | static_cast<u8>(payloadSize));
    } else if (payloadSize <= 0xFFFF) {
        frame.push_back(0x80 | 126);
        uint16_t len = htons(static_cast<uint16_t>(payloadSize));
        frame.insert(frame.end(), reinterpret_cast<u8*>(&len), reinterpret_cast<u8*>(&len) + sizeof(len));
    } else {
        frame.push_back(0x80 | 127);
        uint64_t len = htonll(static_cast<uint64_t>(payloadSize));
        frame.insert(frame.end(), reinterpret_cast<u8*>(&len), reinterpret_cast<u8*>(&len) + sizeof(len));
    }

    uint32_t maskKey;
    std::mt19937 gen{};
    std::uniform_int_distribution<uint32_t> dis(0, 0xFFFFFFFF);
    maskKey = dis(gen);
    frame.insert(frame.end(), reinterpret_cast<u8*>(&maskKey), reinterpret_cast<u8*>(&maskKey) + sizeof(maskKey));

    for (size_t i = 0; i < payloadSize; ++i) {
        u8 maskedByte = message[i] ^ reinterpret_cast<u8*>(&maskKey)[i % 4];
        frame.push_back(maskedByte);
    }

    return sock->sendAll(frame.data(), frame.size()) == frame.size();
}

std::optional<std::string> WebsocketClient::receive() {
    if (!isOpen) return std::nullopt;
start:

    char h[2];
    if (sock->recvAll(&h, 2) < 0) {
        return std::nullopt;
    }

    auto hsize = h[1] & 0x7F;

    uint64_t size;
    if (hsize == 126) {
        uint16_t tmp;
        if (sock->recvAll(&tmp, sizeof(uint16_t)) < 0) {
            return std::nullopt;
        }
        size = (uint64_t) ntohs(tmp);
    } else if (hsize == 127) {
        uint64_t tmp;
        if (sock->recvAll(&tmp, sizeof(uint64_t)) < 0) {
            return std::nullopt;
        }
        size = ntohll(tmp);
    } else {
        size = (uint64_t) hsize;
    }

    char buffer[size];
    memset(buffer, 0, size);

    if ((h[1] & 0x80) != 0) {
        hk::svc::OutputDebugString("WARNING: Server masked message!", strlen("WARNING: Server masked message!"));
    }

    if (sock->recvAll(buffer, size) < 0) {
        return std::nullopt;
    }

    // Server must never mask, we can safely ignore that part

    auto opcode = h[0] & 0x0f;

    // If ping, respond with pong and restart read
    if (opcode == 9) {
        h[0] = (h[0] & 0xf0) | 10;
        h[1] = h[1] | 0x80;

        std::vector<u8> payload;
        payload.insert(payload.end(), reinterpret_cast<u8*>(&h), reinterpret_cast<u8*>(&h) + sizeof(h));
        if (hsize == 126) {
            uint16_t tmp = htons((uint16_t)size);
            payload.insert(payload.end(), reinterpret_cast<u8*>(&tmp), reinterpret_cast<u8*>(&tmp) + sizeof(tmp));
        } else if (hsize == 127) {
            uint64_t tmp = htonll(size);
            payload.insert(payload.end(), reinterpret_cast<u8*>(&tmp), reinterpret_cast<u8*>(&tmp) + sizeof(tmp));
        }

        uint32_t maskKey = 0;
        std::mt19937 gen{};
        std::uniform_int_distribution<uint32_t> dis(0, 0xFFFFFFFF);
        maskKey = dis(gen);

        payload.insert(payload.end(), reinterpret_cast<u8*>(&maskKey), reinterpret_cast<u8*>(&maskKey) + sizeof(maskKey));

        for (size_t i = 0; i < size; ++i) {
            u8 maskedByte = buffer[i] ^ reinterpret_cast<u8*>(&maskKey)[i % 4];
            payload.push_back(maskedByte);
        }

        if (sock->sendAll(payload.data(), payload.size()) < 0) {
            close();
            return std::nullopt;
        }

        goto start;
    }

    // If close, close socket
    if (opcode == 8) {
        // Respond with same close payload
        h[1] = h[1] | 0x80;

        std::vector<u8> payload;
        payload.insert(payload.end(), reinterpret_cast<u8*>(&h), reinterpret_cast<u8*>(&h) + sizeof(h));
        if (hsize == 126) {
            uint16_t tmp = htons((uint16_t)size);
            payload.insert(payload.end(), reinterpret_cast<u8*>(&tmp), reinterpret_cast<u8*>(&tmp) + sizeof(tmp));
        } else if (hsize == 127) {
            uint64_t tmp = htonll(size);
            payload.insert(payload.end(), reinterpret_cast<u8*>(&tmp), reinterpret_cast<u8*>(&tmp) + sizeof(tmp));
        }

        uint32_t maskKey = 0;
        std::mt19937 gen{};
        std::uniform_int_distribution<uint32_t> dis(0, 0xFFFFFFFF);
        maskKey = dis(gen);

        payload.insert(payload.end(), reinterpret_cast<u8*>(&maskKey), reinterpret_cast<u8*>(&maskKey) + sizeof(maskKey));

        for (size_t i = 0; i < size; ++i) {
            u8 maskedByte = buffer[i] ^ reinterpret_cast<u8*>(&maskKey)[i % 4];
            payload.push_back(maskedByte);
        }

        if (sock->sendAll(payload.data(), payload.size()) < 0) {
            close();
            return std::nullopt;
        }

        // Close socket
        sock->close();
        isOpen = false;

        return std::nullopt;
    }

    if (opcode == 1) {
        // If text, return text
        return std::string(buffer, size);
    } else if (opcode == 2) {
        // If binary, return binary data
        return std::string(buffer, size);
    }

    if (h[0] & 0x80) {
        // TODO: Handle additional frames?
        Logger::log("Unhandled unfinished frame!\n");
    }

    // If unknown opcode, ignore
    Logger::log("Unhandled opcode: %d\n", opcode);

    return std::nullopt;
}

bool WebsocketClient::sendJson(const nlohmann::json& message) {
    return send(message.dump());
}

nlohmann::json WebsocketClient::receiveJson() {
    auto res = receive();
    if (res.has_value()) {
        return nlohmann::json::parse(res.value());
    } else {
        return nlohmann::json();
    }
}
