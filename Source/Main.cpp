#include <winsock2.h>
#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdio>
#include "Data/ByteWriter.hpp"
#include "Net/Client.hpp"

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

    Net::Client client;
    if(client.connect("192.168.68.201", 55555))
    {
        printf("Connected to server.\n");

        // Example: Send messages
        for (int i = 0; i < 5; ++i)
        {
            std::string message = "Hello from client " + std::to_string(i);
            Data::ByteWriter writer;
            writer.write<int>(0x1);
            writer.writeString(message);
            client.send(writer.data());
            printf("Sent: %s\n", message.c_str());
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