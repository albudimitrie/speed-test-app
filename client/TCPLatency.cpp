#include "TCPLatency.h"
#include "TCPClient.h"
#include "SocketManager.h"
#include "nlohmann/json.hpp"
using json=nlohmann::json;

TCPLatency::TCPLatency()
{
    _test_type="TCP_LATENCY";
}
void TCPLatency::run(TCPClient &client)
{
    std::cout<<"Starting TCPLatency test\n";
    SocketManager sm;
    std::string details = client.receive_data(128);
    json details_json= json::parse(details);
    int test_socket=  sm.connectTo(client.get_server_addr(), details_json["port"]);
    char buffer[64];
    int n=0;
    while(true)
    {
        n = sm.receiveData(test_socket, buffer, 64);
        if(n <= 0)
        {
            std::cout<<"Latency test ended\n";
            break;
        }
        sm.sendData(test_socket, buffer, 64);
    }
    sm.closeSocket(test_socket);
}