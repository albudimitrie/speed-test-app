#include "TCPClient.h"
#include <string>
class iTest
{
protected:
    std::string _test_type;
public:
    virtual std::string get_test_type() = 0;
    virtual ~iTest() = default;
    virtual void run(TCPClient &client) = 0;
};

class TestFactory
{
public:
    static iTest *makeTCPDownloadTest();
};