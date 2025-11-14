#include <chrono>
#include <memory>
#include <vector>
#include <atomic>
#include <thread>

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
    Client::Client() : registry(std::make_shared<Protocol::Registry>())
    {
        Protocol::Processor::registerData(registry);
        reconnecting.store(false);
        isRunning.store(false);
    }

    Client::~Client()
    {
        disconnect();
    }

    bool Client::connect(const std::string& ipAddress, uint16_t port)
    {
        lastIp = ipAddress;
        lastPort = port;

        auto connector = TcpConnectorSocket::create(ipAddress, port);
        auto socket = connector->connect(
            [this](std::shared_ptr<IConnectionSocket> socket, std::vector<std::byte> data) {
                this->onReceive(data);
            }
        );

        if (!socket)
        {
            Debug::Logger::printf("[Client] Failed to connect.");
            return false;
        }

        connection = std::make_shared<Connection>(socket);

        // start loop
        isRunning = true;

        if (receiveThread.joinable())
        {
            receiveThread.join();
        }

        receiveThread = std::thread(&Client::receiveLoop, this);

        Debug::Logger::printf("[Client] Connected to server.");
        return true;
    }

    void Client::disconnect()
    {
        isRunning = false;

        if (receiveThread.joinable())
        {
            if (receiveThread.get_id() == std::this_thread::get_id())
            {
                // allow death of receiving thread
                receiveThread.detach();
            }
            else
            {
                receiveThread.join();
            }
        }

        connection.reset();
    }

    void Client::send(std::unique_ptr<Protocol::IData> data)
    {
        if (connection)
            connection->send(std::move(data));
    }

    void Client::receiveLoop()
    {
        while (isRunning)
        {
            auto conn = connection;
            if (!conn)
                break;

            auto sock = conn->getSocket();
            if (!sock)
                break;

            auto received = sock->receive();
            if (!received)
            {
                // soft exit。onDisconnect は呼ばない
                break;
            }

            onReceive(received.value());
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        // receiveLoop が終わったということは切断されたということなので、
        // メインスレッドに復帰させて再接続
        if (!reconnecting.exchange(true))
        {
            std::thread([this]() {
                Debug::Logger::printf("[Aurora] Disconnected. Reconnecting in 5 seconds...");
                disconnect();

                while (true)
                {
                    std::this_thread::sleep_for(std::chrono::seconds(5));
                    Debug::Logger::printf("[Aurora] Trying to reconnect...");

                    if (this->connect(this->lastIp, this->lastPort))
                    {
                        Debug::Logger::printf("[Aurora] Reconnected!");
                        reconnecting.store(false);
                        break;
                    }
                }
            }).detach();
        }
    }

    void Client::onReceive(const std::vector<std::byte>& data)
    {
        if (data.size() < sizeof(Protocol::ID))
            return;

        Data::ByteReader reader(data);
        Protocol::ID id = static_cast<Protocol::ID>(reader.read<int>());

        std::vector<std::byte> payload = reader.readBytes(reader.remain());
        auto protocolData = std::unique_ptr<Protocol::IData>(registry->deserialize(id, payload));

        if (protocolData)
            protocolData->process(connection);
    }

    bool Client::isConnected() const
    {
        return connection != nullptr;
    }
}
