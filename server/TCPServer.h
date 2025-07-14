#pragma once
#include <iostream>
#include <string>
class TCPServer
{
    int _socket;
    int _port;
    bool _running;
    std::string _client_addr;
public:
    TCPServer(int port= 12345)
    {
        _port=port;
        _running=false;
    }
    int get_socket() { return _socket;}
    void start();
    void stop();
    std::string receive_data(int max_len);
    void send_data(const std::string &data);
    std::string get_client_addr() { return _client_addr;}

};