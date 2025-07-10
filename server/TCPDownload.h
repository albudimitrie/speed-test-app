#include "iTest.h"
#include "chrono"
#define BUFFER_SIZE 1024*32
class TCPDownload : public iTest
{
    int _duration;
public:
    TCPDownload(int duration);
    virtual void run(int client_socket) override;
    virtual ~TCPDownload()=default;

};