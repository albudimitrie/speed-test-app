#include <iostream>
class TCPServer
{
    int _socket;
    int _port;
    bool _running;
public:
    TCPServer(int port= 12345)
    {
        _port=port;
        _running=false;
    }
    void start();
    void stop();

};