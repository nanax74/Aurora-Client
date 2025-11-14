#include "Net/Transport/TcpConnectionSocket.hpp"
#include "Debug/Logger.hpp"
#include <memory>
#include <optional>
#include <iostream>

namespace Net::Transport
{
    TcpConnectionSocket::TcpConnectionSocket(int fd, std::string ip, uint16_t port, std::function<void(std::shared_ptr<IConnectionSocket>, std::vector<std::byte>)> onReceive) : fd(fd), ip(ip), port(port), onReceive(onReceive) {}

    std::shared_ptr<TcpConnectionSocket> TcpConnectionSocket::create(SOCKET fd, std::string ip, uint16_t port, std::function<void(std::shared_ptr<IConnectionSocket>, std::vector<std::byte>)> onReceive)
    {
        return std::shared_ptr<TcpConnectionSocket>(new TcpConnectionSocket(fd, ip, port, onReceive));
    }

    bool TcpConnectionSocket::send(const std::vector<std::byte>& data)
    {
        return ::send(fd, (const char*)data.data(), data.size(), 0) > 0;
    }

    std::optional<std::vector<std::byte>> TcpConnectionSocket::receive()
    {
        if (!fd) return std::nullopt;

        std::vector<std::byte> buffer(1024 * 4);
        int received = ::recv(fd, reinterpret_cast<char*>(buffer.data()), buffer.size(), 0);

        if (received <= 0)
        {
            return std::nullopt;
        }

        buffer.resize(received);
        return buffer;
    }

    SOCKET TcpConnectionSocket::getFd()
    {
        return fd;
    }

    bool TcpConnectionSocket::onReceivable()
    {
        return false;
    }

    void TcpConnectionSocket::onDisconnected()
    {
        std::cout << "Disconnected from: ("<< ip << ":" << port << ")" << std::endl;
    }
}