#pragma once
#include "iTest.h"
#include <cstdint>

class TCPClient;
class UDPDownload : public iTest
{
    int _duration;
    uint64_t _bytes_to_send;
    uint64_t _bitrate_bps;
    void run_size_test(TCPClient &client);
    void run_time_test(TCPClient &client);
    std::chrono::microseconds calculatePacketInterval(size_t packet_size);
public:
    UDPDownload(int duration, uint64_t bytes_to_send, uint64_t bitrate_bps);
    virtual ~UDPDownload()=default;
    virtual std::string get_test_type(){return _test_type;}
    virtual void run(TCPClient &client)override;
};