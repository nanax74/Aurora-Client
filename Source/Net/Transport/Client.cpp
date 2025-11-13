#include <chrono>
#include <memory>
#include <vector>

#include "Data/ByteReader.hpp"
#include "Net/Protocol/IData.hpp"
#include "Net/Protocol/Processor.hpp"
#include "Net/Transport/Client.hpp"
#include "Net/Transport/Connection.hpp"
#include "Net/Transport/IConnectionSocket.hpp"
#include "Net/Transport/TcpConnectorSocket.hpp"
#include "Debug/Logger.hpp"

namespace Net::Transport
{
    Client::Client(std::function<void(std::shared_ptr<Protocol::Registry>, std::unique_ptr<Connection>, std::unique_ptr<Protocol::IData>)> dispatcher) : registry(std::shared_ptr<Protocol::Registry>(new Protocol::Registry())), dispatcher(dispatcher)
    {
        Protocol::Processor::registerData(registry);
    }

    bool Client::connect(const char* ipAddress, uint16_t port)
    {
        auto connector = TcpConnectorSocket::create(ipAddress, port);
        connection = std::shared_ptr<Connection>(new Connection(connector->connect([this](std::shared_ptr<IConnectionSocket> socket, std::vector<std::byte> data){ this->onReceive(data); })));
        if(!connection)
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
        connection.reset();
    }

    void Client::send(std::unique_ptr<Protocol::IData> data)
    {
        if (connection)
        {
            connection->send(std::move(data));
        }
    }

    void Client::receiveLoop()
    {
        while (isRunning)
        {
            if (connection)
            {
                auto received = connection->getSocket()->receive();
                if(received) onReceive(received.value());
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Prevent busy-waiting
        }
    }

    void Client::onReceive(std::vector<std::byte> data)
    {
        if (data.size() < sizeof(Protocol::ID))
        {
            Debug::Logger::printf("Received data size ({}) is less than Protocol::ID size ({}). Dispatching InvalidData.", data.size(), sizeof(Protocol::ID));
            return;
        }

        Data::ByteReader reader(data);
        Protocol::ID id = static_cast<Protocol::ID>(reader.read<int>());
        Debug::Logger::printf("[Received] ID: {}", static_cast<uint32_t>(id));

        std::vector<std::byte> payload(reader.readBytes(reader.remain()));
        std::unique_ptr<Protocol::IData> protocolData(registry->deserialize(id, payload));
        if(protocolData == nullptr) return;
        protocolData->process(connection);
    }
}
