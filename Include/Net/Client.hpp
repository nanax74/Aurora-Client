#pragma once

#include <cstdint>
#include <memory>
#include <thread>

#include "Net/TcpConnectorSocket.hpp"

namespace Net
{
	class Client
	{
	private:
		std::unique_ptr<TcpConnectorSocket> connector;
		std::thread receiveThread;
		bool isRunning = false;

		void receiveLoop();

	public:
		bool connect(const char* ipAddress, uint16_t port);
		void disconnect();

		void send();
	};
}
