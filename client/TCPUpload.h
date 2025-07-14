#pragma once
#include "iTest.h"
#include <cstdint>

class TCPClient;
class TCPUploadSize : public iTest
{
    uint64_t _bytes_to_send;
public:
    TCPUploadSize(uint64_t bytes_to_send);
    virtual std::string get_test_type()override;
    virtual ~TCPUploadSize()=default;
    virtual void run(TCPClient &client)override;
};


class TCPUploadTime : public iTest
{
    int _duration;
public:
    TCPUploadTime(int duration);
    virtual std::string get_test_type()override;
    virtual ~TCPUploadTime()=default;
    virtual void run(TCPClient &client)override;
};