#include <stdexcept>
#include <chrono>

#include "Net/Client.hpp"
#include "Net/IConnectorSocket.hpp"

namespace Net
{
    bool Client::connect(const char* ipAddress, uint16_t port)
    {
        connector = TcpConnectorSocket::create(ipAddress, port);
        if (!connector->connect())
        {
            return false;
        }
        return true;
    }

    void Client::disconnect()
    {
        connector.reset();
    }
}
