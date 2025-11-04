#include <winsock.h>

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

void socketTest()
{
    initWinsock();

    cleanupWinsock();
}

int main(int argc, char** argv)
{
    socketTest();
    return 0;
}