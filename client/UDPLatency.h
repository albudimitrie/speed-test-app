#pragma once
#include "iTest.h"
class TCPClient;
class UDPLatency : public iTest
{

public:
    UDPLatency();
    virtual ~UDPLatency()=default;
    virtual std::string get_test_type()override {return _test_type;}
    virtual void run(TCPClient &client)override;

};