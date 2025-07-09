#include "RawSocket.h"

#include <bits/errno.h>
#include <bits/fcntl.h>
#include <logger/logger.h>

RawSocket::RawSocket() {
    initCommon();

    socket = nn::socket::Socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (socket < 0) {
        Logger::log("Failed to create socket: %d\n", nn::socket::GetLastErrno());
        close();
        state = ERRORED;
    }

    state = INITIALIZED;
    return;
}

RawSocket::~RawSocket() {
    close();
}

nn::Result RawSocket::connect(const std::string& host, u16 port) {
    if (state != INITIALIZED) {
        Logger::log("Attempted to connect before initializing!\n");
        close();
        state = ERRORED;
        return err::ResultInitializationFailed();
    }

    auto addr = resolve(host);
    nn::Result res = nn::ResultSuccess();

    sockaddr_in serverAddress = {0};
    serverAddress.sin_addr = addr;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_family = htons(AF_INET);

    if (auto err = nn::socket::Connect(socket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)); err.IsFailure()) {
        Logger::log("Failed to connect to server: %d/%d\n", err.GetInnerValueForDebug(), nn::socket::GetLastErrno());
        res = err::ResultConnectionFailed();
        goto exit;
    }

    if (nn::socket::Fcntl(socket, F_SETFL, nn::socket::Fcntl(socket, F_GETFL) | O_NONBLOCK) < 0) {
        Logger::log("Failed to fcntl socket: %d\n", nn::socket::GetLastErrno());
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

int RawSocket::send(const void* buf, int size) {
    if (size == 0) return 0;
    if (state != CONNECTED) {
        Logger::log("Sending in invalid state: %d\n", state);
        return -1;
    }

    auto sent = nn::socket::Send(socket, buf, size, 0);
    if (sent < 0) {
        Logger::log("Send error: %d/%d\n", sent, nn::socket::GetLastErrno());
        close();
    }
    return sent;
}

int RawSocket::recv(void* buf, int size) {
    if (size == 0) return 0;
    if (state != CONNECTED) {
        Logger::log("Receiving in invalid state: %d\n", state);
        return -1;
    }

    auto received = nn::socket::Recv(socket, buf, size, 0);
    if (received < 0) {
        auto err = nn::socket::GetLastErrno();
        if (err == EAGAIN) return 0;
        Logger::log("Receive error: %d/%d\n", received, nn::socket::GetLastErrno());
        close();
    }
    return received;
}

void RawSocket::close() {
    state = CLOSED;
    nn::socket::Close(socket);
}




