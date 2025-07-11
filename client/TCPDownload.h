#pragma once
#include "iTest.h"



class TCPDownload : public iTest
{
int _duration;
public:
    TCPDownload(int duration) {_test_type="TCP_DOWNLOAD"; _duration=duration; };
    virtual std::string get_test_type()override{ return _test_type;};
    virtual void run(TCPClient &client) override;
    virtual ~TCPDownload() = default;
};