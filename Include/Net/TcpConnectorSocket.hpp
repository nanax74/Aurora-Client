#pragma once

#include <string>
#include <cstdint>
#include <memory>
#include <winsock2.h>

#include "Net/IConnectorSocket.hpp"

namespace Net
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

        void onReadable() override;
        void onWritable() override;
        void onDisconnected() override;
    };
}