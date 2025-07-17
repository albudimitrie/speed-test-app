#pragma once
#include "iTest.h"

class UDPLatency: public iTest
{
    json &_stats;
    std::string _client_addr;
    double round_trip_time(int test_socket);
public:
    UDPLatency(json &stats, std::string client_address);
    virtual ~UDPLatency()=default;
    virtual void run(int client_socket)override;

};