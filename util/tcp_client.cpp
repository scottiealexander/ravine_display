#include <strings.h>
#include <netdb.h>

#include "tcp_client.hpp"

TCPClient::TCPClient(const char* ip, int port)
{
    _fd =  socket(AF_INET, SOCK_STREAM, 0);
    hostent* server = gethostbyname(ip);


    bzero((char*) &_addr, sizeof(_addr));
    _addr.sin_family = AF_INET;

    bcopy((char*) server->h_addr, (char*) &_addr.sin_addr.s_addr, server->h_length);
    _addr.sin_port = htons(port);
}

TCPClient::~TCPClient()
{
    close(_fd);
}

bool TCPClient::sync_connect()
{
    int ret = connect(_fd, (sockaddr*) &_addr, sizeof(_addr));
    return ret >= 0;
}
