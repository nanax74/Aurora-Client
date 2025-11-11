#pragma once

#include <string>
#include <cstdint>
#include <memory>
#include <winsock2.h>

#include "Net/Transport/IConnectorSocket.hpp"

namespace Net::Transport
{
    class TcpConnectorSocket : public IConnectorSocket
    {
    private:
        SOCKET fd;
        std::string ip;
        uint16_t port;

        TcpConnectorSocket(std::string ip, uint16_t port);

    public:
        ~TcpConnectorSocket();
        
        static std::unique_ptr<TcpConnectorSocket> create(std::string ip, uint16_t port);

        bool connect();

        bool send(const std::vector<std::byte>& data) override;
        std::vector<std::byte> receive() override;
    };
}