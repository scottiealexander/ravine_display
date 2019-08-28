#ifndef TCP_CLIENT_HPP_
#define TCP_CLIENT_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// #include <time.h>


class TCPClient
{
public:
    TCPClient(const char* ip, int port);
    ~TCPClient();
    bool sync_connect();
    inline void sync_send(uint8_t msg) { write(_fd, &msg, 1); }
    inline uint8_t sync_read()
    {
        uint8_t msg;
        read(_fd, &msg, 1);
        return msg;
    }
private:
    int _fd;
    sockaddr_in _addr;
};

#endif
