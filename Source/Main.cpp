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

    while(!client.connect("192.168.68.201", 55555))
    {
        Debug::Logger::printf("Failed initial connection. Will retry in 10 seconds.");
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

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
        else if(cmd == "input")
        {
            std::string type;
            std::cin >> type;

            std::string temp;
            try
            {
                if(type == "button")
                {
                    uint32_t button;
                    uint32_t timeMs;

                    std::cin >> temp;
                    button = std::stoul(temp, nullptr, 16);
                    std::cin >> temp;
                    timeMs = std::stoul(temp);

                    std::unique_ptr<Net::Protocol::Entry::InputButton> input = std::make_unique<Net::Protocol::Entry::InputButton>(button, timeMs);
                    client.send(std::move(input));
                }
                else if(type == "lstick")
                {
                    float x;
                    float y;
                    uint32_t timeMs;

                    std::cin >> temp;
                    x = std::stof(temp);
                    std::cin >> temp;
                    y = std::stof(temp);
                    std::cin >> temp;
                    if (!temp.empty() && temp[0] == '-')
                    {
                        throw std::invalid_argument("negative value not allowed");
                    }
                    timeMs = static_cast<unsigned int>(std::stoul(temp));

                    std::unique_ptr<Net::Protocol::Entry::InputLeftStick> input = std::make_unique<Net::Protocol::Entry::InputLeftStick>(x, y, timeMs);
                    client.send(std::move(input));
                }
                else if(type == "rstick")
                {
                    float x;
                    float y;
                    uint32_t timeMs;

                    std::cin >> temp;
                    x = std::stof(temp);
                    std::cin >> temp;
                    y = std::stof(temp);
                    std::cin >> temp;
                    timeMs = std::stoul(temp);

                    std::unique_ptr<Net::Protocol::Entry::InputRightStick> input = std::make_unique<Net::Protocol::Entry::InputRightStick>(x, y, timeMs);
                    client.send(std::move(input));
                }
                else if(type == "clear")
                {
                    std::unique_ptr<Net::Protocol::Entry::InputClear> input = std::make_unique<Net::Protocol::Entry::InputClear>();
                    client.send(std::move(input));
                }
            }
            catch (const std::invalid_argument& e)
            {
                std::cout << "[Invalid argument: " << e.what() << "] [Arg: " << temp << "]" << std::endl;
                continue;
            }
        }
        else
        {
            std::cout << "Unknown command: " << cmd << std::endl;
        }
    }

    client.disconnect();
    printf("Disconnected from server.\n");

    cleanupWinsock();
    Debug::Logger::shutdown();
    return 0;
}