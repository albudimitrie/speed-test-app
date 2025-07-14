#include "iTest.h"
class TCPClient;
class TCPLatency : public iTest
{
public:
    TCPLatency();
    virtual ~TCPLatency()=default;
    virtual std::string get_test_type()override {return _test_type;}
    virtual void run(TCPClient &client)override;

};