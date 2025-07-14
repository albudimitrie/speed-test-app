#pragma once

#include "iTest.h"


class TCPLatency : public iTest
{
    json &_stats;
    double round_trip_time(int test_socket);
public:
    TCPLatency(json &stats);
    virtual ~TCPLatency()=default;
    virtual void run(int client_socket)override;

};


