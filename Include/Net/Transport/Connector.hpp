#pragma once

#include <memory>

#include "Net/Transport/IConnectorSocket.hpp"

namespace Net
{
    namespace Protocol
    {
        class IData;
    }

    namespace Transport
    {
        class Connector
        {
        private:
            std::shared_ptr<IConnectorSocket> socket;
        
        public:
            explicit Connector(std::shared_ptr<IConnectorSocket> socket);
            void send(std::unique_ptr<Protocol::IData> data);
        };
    }
}