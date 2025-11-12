#include <memory>
#include <winsock2.h>
#include <string>
#include <chrono>
#include <thread>
#include <cstdio>
#include <iostream>
#include "Debug/Logger.hpp"
#include "Net/Protocol/Entry.hpp"
#include "Net/Transport/Client.hpp"

bool initWinsock()
{
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    return result == 0;
}

void cleanupWinsock()
{
    WSACleanup();
}

int main(int argc, char** argv)
{
    Debug::Logger::init();
    if (!initWinsock()) return 1;

    Net::Transport::Client client;
    if(client.connect("192.168.68.201", 55555))
    {
        printf("Connected to server.\n");

        // Example: Send messages
        while(true)
        {
            std::string cmd;
            std::cin >> cmd;
            if(cmd == "exit") break;
            if(cmd == "ping")
            {
                std::unique_ptr<Net::Protocol::Entry::Ping> ping = std::unique_ptr<Net::Protocol::Entry::Ping>(new Net::Protocol::Entry::Ping());
                client.send(std::move(ping));
            }
            else
            {
                std::cout << "Unknown command: " << cmd << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        client.disconnect();
        printf("Disconnected from server.\n");
    }
    else
    {
        printf("Failed to connect to server.\n");
    }

    cleanupWinsock();
    Debug::Logger::shutdown();
    return 0;
}