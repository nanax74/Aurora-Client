#include <winsock2.h>
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
        
    }

    cleanupWinsock();
    return 0;
}