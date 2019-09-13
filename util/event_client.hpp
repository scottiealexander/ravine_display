#ifndef TCP_CLIENT_HPP_
#define TCP_CLIENT_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/* ========================================================================== */
double get_time();
/* ========================================================================== */
class EventClient
{
public:
    virtual ~EventClient() {}
    virtual bool sync_connect() = 0;
    virtual void sync_send(uint8_t) const = 0;
    virtual uint8_t sync_read() const = 0;
};
/* ========================================================================== */
class LogClient : public EventClient
{
public:
    inline bool sync_connect() override { return true; }
    inline void sync_send(uint8_t msg) const override
    {
        printf("[SEND]: %d @ %.6f\n", (int)msg, get_time());
    }
    inline uint8_t sync_read() const override { return 'x'; }
};
/* ========================================================================== */
class TCPClient : public EventClient
{
public:
    TCPClient(const char* ip, int port);
    ~TCPClient();
    bool sync_connect() override;
    inline void sync_send(uint8_t msg) const override { write(_fd, &msg, 1); }
    inline uint8_t sync_read() const override
    {
        uint8_t msg;
        read(_fd, &msg, 1);
        return msg;
    }

private:
    int _fd;
    sockaddr_in _addr;
};
/* ========================================================================== */
#endif
