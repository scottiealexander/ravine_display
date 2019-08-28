#include <chrono>
#include <thread>

#include "../util/event_client.hpp"

// g++ -o test_event_client -I . ../util/event_client.cpp test_event_client.cpp

int main(int narg, const char** args)
{
    EventClient* evt;
    if (narg > 1)
    {
        evt = new TCPClient("127.0.0.1", 65000);
    }
    else
    {
        evt = new LogClient();
    }

    if (!evt->sync_connect())
    {
        printf("Connect failed\n");
    }

    float dur = 0.0f;
    for (int k = 0; k < 100; ++k)
    {
        auto t1 = std::chrono::high_resolution_clock::now();
        evt->sync_send(0x61);
        auto t2 = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
        dur += static_cast<float>(duration.count());
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    evt->sync_send('x');

    printf("Mean write duration: %f us\n", dur / 100.0f);

    delete evt;

    return 0;
}
