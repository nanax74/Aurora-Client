#include <memory>
#include <winsock2.h>
#include <string>
#include <chrono>
#include <thread>
#include <cstdio>
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
    if (!initWinsock()) return 1;

    Net::Transport::Client client;
    if(client.connect("192.168.68.201", 55555))
    {
        printf("Connected to server.\n");

        // Example: Send messages
        for (int i = 0; i < 5; ++i)
        {
            std::unique_ptr<Net::Protocol::Entry::Ping> ping = std::unique_ptr<Net::Protocol::Entry::Ping>(new Net::Protocol::Entry::Ping());
            client.send(std::move(ping));
            printf("Sent ping\n");
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
    return 0;
}