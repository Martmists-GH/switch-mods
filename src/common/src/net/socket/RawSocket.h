#pragma once
#include "SocketBase.h"

class RawSocket final : public SocketBase {
    int socket;

public:
    RawSocket();
    ~RawSocket() override;
    nn::Result connect(const std::string& host, u16 port) override;
    int send(const void* buf, int size) override;
    int recv(void* buf, int size) override;
    void close() override;
};
