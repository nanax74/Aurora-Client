/*
 * Remember to link to ws2_32.lib
 */
#include <memory>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>

#include "Net/TcpConnectorSocket.hpp"

namespace Net
{
    TcpConnectorSocket::~TcpConnectorSocket() { closesocket(fd); }

    TcpConnectorSocket::TcpConnectorSocket(std::string ip, uint16_t port) : ip(ip), port(port) {}

    std::unique_ptr<TcpConnectorSocket> TcpConnectorSocket::create(std::string ip, uint16_t port)
    {
        return std::unique_ptr<TcpConnectorSocket>(new TcpConnectorSocket(ip, port));
    }

    bool TcpConnectorSocket::connect()
    {
        fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd == INVALID_SOCKET) {
            return false;
        }

        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr);

        if (::connect(fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            return false;
        }

        return true;
    }

    bool TcpConnectorSocket::send(const std::vector<std::byte>& data)
    {
        return ::send(fd, (const char*)data.data(), data.size(), 0) > 0;
    }

    std::vector<std::byte> TcpConnectorSocket::receive()
    {
        if (fd == INVALID_SOCKET) return {};
    
        u_long available = 0;
        if (ioctlsocket(fd, FIONREAD, &available) != 0) {
            return {};
        }

        if (available == 0) return {};

        std::vector<std::byte> buffer = std::vector<std::byte>(available);
        int res = ::recv(fd, (char*)buffer.data(), buffer.size(), 0);
        if (res <= 0)
        {
            return {};
        }
        
        buffer.resize((res));
        return buffer;
    }
}