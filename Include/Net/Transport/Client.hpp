#pragma once

#include <cstdint>
#include <memory>
#include <thread>
#include <atomic>
#include <string>
#include <vector>

#include "Net/Transport/Connection.hpp"
#include "Net/Protocol/Registry.hpp"

namespace Net::Transport
{
    class Client
    {
    private:
        std::shared_ptr<Connection> connection;

        std::thread receiveThread;
        std::atomic_bool isRunning{false};
        std::atomic_bool reconnecting{false};

        std::string lastIp;
        uint16_t lastPort = 0;

        std::shared_ptr<Protocol::Registry> registry;

        void receiveLoop();

    public:
        Client();
        ~Client();  // 追加: 安全にスレッドを止める

        bool connect(const std::string& ipAddress, uint16_t port);
        void disconnect();

        void send(std::unique_ptr<Protocol::IData> data);

        void onReceive(const std::vector<std::byte>& data);
        void onDisconnect();

        bool isConnected() const;
    };
}
