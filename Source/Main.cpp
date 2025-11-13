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
#include "Util/String.hpp"

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
            if(cmd == "exit")
            {
                break;
            }
            else if(cmd == "ping")
            {
                std::unique_ptr<Net::Protocol::Entry::Ping> ping = std::make_unique<Net::Protocol::Entry::Ping>();
                client.send(std::move(ping));
            }
            else if(cmd == "notification")
            {
                std::string msg;
                std::getline(std::cin, msg);
                std::unique_ptr<Net::Protocol::Entry::Notification> notification = std::make_unique<Net::Protocol::Entry::Notification>(msg);
                client.send(std::move(notification));
            }
            else if(cmd == "read")
            {
                std::string input;
                uint32_t address;
                uint32_t size;
                try
                {
                    std::cin >> input;
                    address = std::stoul(input, nullptr, 16);
                    std::cin >> input;
                    size = std::stoul(input, nullptr, 16);
                }
                catch (const std::invalid_argument& e)
                {
                    std::cout << "Invalid argument: " << input << std::endl;
                    continue;
                }

                std::unique_ptr<Net::Protocol::Entry::ReadMemory> readMemory = std::make_unique<Net::Protocol::Entry::ReadMemory>(address, size);
                client.send(std::move(readMemory));
            }
            else if(cmd == "write")
            {
                std::string input;
                uint32_t address;
                std::vector<std::byte> value;
                try
                {
                    std::cin >> input;
                    address = std::stoul(input, nullptr, 16);
                    std::cin >> input;
                    value = Util::String::fromHexString(input);
                }
                catch (const std::invalid_argument& e)
                {
                    std::cout << "[Invalid argument: " << e.what() << "] [Arg: " << input << "]" << std::endl;
                    continue;
                }

                std::unique_ptr<Net::Protocol::Entry::WriteMemorySuper> writeMemorySuper = std::make_unique<Net::Protocol::Entry::WriteMemorySuper>(address, value);
                client.send(std::move(writeMemorySuper));
            }
            else
            {
                std::cout << "Unknown command: " << cmd << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
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