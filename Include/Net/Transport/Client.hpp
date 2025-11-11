#pragma once

#include <cstdint>
#include <memory>
#include <thread>

#include "Net/Transport/Connector.hpp"

namespace Net::Transport
{
	class Client
	{
	private:
		std::unique_ptr<Connector> connector;
		std::thread receiveThread;
		bool isRunning = false;

		void receiveLoop();

	public:
		std::unique_ptr<Connector> connect(const char* ipAddress, uint16_t port);
		void disconnect();

		void sendString(const std::string& data);
		void send(const std::vector<std::byte>& data);
	};
}
