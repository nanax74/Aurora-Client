#include <chrono>
#include <iostream>
#include <vector>

#include "Net/Client.hpp"
#include "Data/ByteReader.hpp"

namespace Net
{
    bool Client::connect(const char* ipAddress, uint16_t port)
    {
        connector = TcpConnectorSocket::create(ipAddress, port);
        if (!connector->connect())
        {
            return false;
        }
        isRunning = true;
        receiveThread = std::thread(&Client::receiveLoop, this);
        return true;
    }

    void Client::disconnect()
    {
        isRunning = false;
        if (receiveThread.joinable())
        {
            receiveThread.join();
        }
        connector.reset();
    }

    void Client::sendString(const std::string& data)
    {
        if (connector)
        {
            std::vector<std::byte> byteData(reinterpret_cast<const std::byte*>(data.data()), reinterpret_cast<const std::byte*>(data.data() + data.size()));
            connector->send(byteData);
        }
    }

    void Client::send(const std::vector<std::byte>& data)
    {
        if (connector)
        {
            connector->send(data);
        }
    }

    void Client::receiveLoop()
    {
        while (isRunning)
        {
            if (connector)
            {
                std::vector<std::byte> receivedData = connector->receive();
                if (!receivedData.empty())
                {
                    Data::ByteReader reader(receivedData);
                    int length = reader.read<int>();
                    std::string message = reader.readString(length);
                    std::cout << "Message: " << message << std::endl;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Prevent busy-waiting
        }
    }
}
