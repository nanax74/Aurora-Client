#pragma once

#include <vector>
#include <winsock2.h>

namespace Net::Transport
{
    class IConnectorSocket
    {
    public:
        virtual ~IConnectorSocket() = default;
        virtual bool send(const std::vector<std::byte>& data) = 0;
        virtual std::vector<std::byte> receive() = 0;
    };
}