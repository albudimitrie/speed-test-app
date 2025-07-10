#include "iTest.h"



class TCPDownload : public iTest
{
public:
    TCPDownload() {_test_type="TCP_DOWNLOAD";};
    virtual std::string get_test_type()override{ return _test_type;};
    virtual void run(TCPClient &client) override;
    virtual ~TCPDownload() = default;
};