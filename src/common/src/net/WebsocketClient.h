#pragma once

#include <nlohmann/json.hpp>
#include <nn/os/os_MutexTypes.h>

#include "ClientWithHttp.h"

class WebsocketClient : public ClientWithHttp {
    std::unique_ptr<SocketBase> sock;
    bool isOpen = false;

public:
    explicit WebsocketClient(const std::string& url);
    ~WebsocketClient();

    bool open() const;
    void close();

    void send(const std::string& message);
    std::optional<std::string> receive();

    void sendJson(const nlohmann::json& message);
    nlohmann::json receiveJson();
};
