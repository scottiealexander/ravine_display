// g++ -std=c++11 -Wall -Wextra -pthread -I../util -o event_test event_test.cpp ../util/event_client.cpp
#include <cstdio>
#include <sstream>
#include <thread>
#include <chrono>
#include <cinttypes>

#include "event_client.hpp"

void usage(const char* name)
{
    printf("Usage: %s <ip_addr> <port_number>\n", name);
}

int main(int narg, const char** args)
{
    if (narg < 3)
    {
        printf("[ERROR]: invalid inputs\n");
        usage(args[0]);
        return -1;
    }

    int port;
    std::stringstream ss(args[2]);
    ss >> port;

    if (port == 0)
    {
        printf("[ERROR]: invalid port # \"%s\"\n", args[2]);
        return -1;
    }

    TCPClient client(args[1], port);

    // LogClient client;

    if (!client.sync_connect())
    {
        printf("[ERROR]: failed to connect to server\n");
        return -1;
    }

    // pause before we commense streaming...
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    printf("[INFO]: starting send loop\n");
    for (uint8_t k = 0x00; k < 0x04; ++k)
    {
        client.sync_send(k);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    printf("[INFO]: send close signal\n");
    client.sync_send(0xff);

    return 0;
}
