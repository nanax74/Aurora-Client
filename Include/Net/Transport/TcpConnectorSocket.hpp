#pragma once

#include <string>
#include <cstdint>
#include <memory>
#include <functional>
#include <winsock2.h>

#include "Net/Transport/IConnectorSocket.hpp"

namespace Net::Transport
{
    class TcpConnectorSocket : public IConnectorSocket
    {
    private:
        std::string ip;
        uint16_t port;

        TcpConnectorSocket(std::string ip, uint16_t port);

    public:
        ~TcpConnectorSocket();
        
        static std::unique_ptr<TcpConnectorSocket> create(std::string ip, uint16_t port);

        std::shared_ptr<IConnectionSocket> connect(std::function<void(std::shared_ptr<IConnectionSocket>, std::vector<std::byte>)> onReceive) override;
    };
}