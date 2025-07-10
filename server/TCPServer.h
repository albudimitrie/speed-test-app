#include <iostream>
#include <string>
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
    std::string receive_data(int max_len);
    void send_data(const std::string &data);

};