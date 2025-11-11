#pragma once

#include <memory>
#include <functional>
#include "Net/Transport/IConnectionSocket.hpp"

namespace Net::Transport
{
    class IConnectorSocket
    {
    public:
        virtual ~IConnectorSocket() = default;
        virtual std::shared_ptr<IConnectionSocket> connect(std::function<void(std::shared_ptr<IConnectionSocket>, std::vector<std::byte>)> onReceive) = 0;
    };
}