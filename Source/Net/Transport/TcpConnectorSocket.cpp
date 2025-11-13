#include <memory>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "Net/Transport/TcpConnectorSocket.hpp"
#include "Net/Transport/IConnectionSocket.hpp"
#include "Net/Transport/TcpConnectionSocket.hpp"

namespace Net::Transport
{
    TcpConnectorSocket::~TcpConnectorSocket() {}

    TcpConnectorSocket::TcpConnectorSocket(std::string ip, uint16_t port) : ip(ip), port(port) {}

    std::unique_ptr<TcpConnectorSocket> TcpConnectorSocket::create(std::string ip, uint16_t port)
    {
        return std::unique_ptr<TcpConnectorSocket>(new TcpConnectorSocket(ip, port));
    }

    std::shared_ptr<IConnectionSocket> TcpConnectorSocket::connect(std::function<void(std::shared_ptr<IConnectionSocket>, std::vector<std::byte>)> onReceive)
    {
        SOCKET fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd == INVALID_SOCKET)
        {
            return nullptr;
        }

        int flag = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&flag, sizeof(flag)) < 0)
        {
            perror("setsockopt(SO_REUSEADDR) failed");
            return nullptr;
        }

        if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag)) < 0)
        {
            perror("setsockopt(TCP_NODELAY) failed");
            return nullptr;
        }

        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr);

        if (::connect(fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            return nullptr;
        }

        return TcpConnectionSocket::create(fd, ip, port, onReceive);
    }
}