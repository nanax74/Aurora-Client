#pragma once

#include <cstdint>
#include <memory>
#include <thread>

#include "Net/Transport/Connection.hpp"
#include "Net/Protocol/Registry.hpp"

namespace Net::Transport
{
	class Client
	{
	private:
		std::shared_ptr<Connection> connection;
		std::thread receiveThread;
		bool isRunning = false;

		void receiveLoop();

		std::shared_ptr<Protocol::Registry> registry;
		std::function<void(std::shared_ptr<Protocol::Registry>, std::unique_ptr<Connection>, std::unique_ptr<Protocol::IData>)> dispatcher;

	public:
		Client(std::function<void(std::shared_ptr<Protocol::Registry>, std::unique_ptr<Connection>, std::unique_ptr<Protocol::IData>)> dispatcher = nullptr);

		bool connect(const char* ipAddress, uint16_t port);
		void disconnect();

		void send(std::unique_ptr<Protocol::IData> data);

		void onReceive(std::vector<std::byte> data);
	};
}
