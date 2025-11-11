#pragma once

#include <string>
#include <cstdint>
#include <memory>
#include <optional>
#include <functional>
#include <winsock2.h>

#include "Net/Transport/IConnectionSocket.hpp"

namespace Net::Transport
{
    class TcpConnectionSocket : public IConnectionSocket, public std::enable_shared_from_this<TcpConnectionSocket>
    {
    private:
        SOCKET fd;
        std::string ip;
        uint16_t port;

        std::function<void(std::shared_ptr<IConnectionSocket>, std::vector<std::byte>)> onReceive;

        TcpConnectionSocket(int fd, std::string ip, uint16_t port, std::function<void(std::shared_ptr<IConnectionSocket>, std::vector<std::byte>)> onReceive);

    public:
        static std::shared_ptr<TcpConnectionSocket> create(SOCKET fd, std::string ip, uint16_t port, std::function<void(std::shared_ptr<IConnectionSocket>, std::vector<std::byte>)> onReceive);

        bool send(const std::vector<std::byte>& data) override;
        std::optional<std::vector<std::byte>> receive() override;

        SOCKET getFd() override;

        bool onReceivable() override;
        void onDisconnected() override;
    };
}