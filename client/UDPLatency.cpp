#include "UDPLatency.h"
#include "TCPClient.h"
#include "SocketManager.h"
#include "nlohmann/json.hpp"
using json=nlohmann::json;

UDPLatency::UDPLatency()
{
    _test_type="UDP_LATENCY";
}
void UDPLatency::run(TCPClient &client)
{
    std::cout<<"Starting UDPLatency test\n";
    SocketManager sm;
    std::string details = client.receive_data(512);
    json details_json= json::parse(details);
    int port = details_json["port"];
    int times=details_json["times"];
    int test_socket= sm.createUdpSocket(port);
    struct timeval tv;
    tv.tv_sec = 1; 
    tv.tv_usec = 0;
    setsockopt(test_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    char buffer[64];
    int n=0;
    std::string ip_server = client.get_server_addr();
    for(int i=0; i<times;i++){
        sm.receiveFrom(test_socket, 64, ip_server ,port);
        sm.sendTo(test_socket, buffer, 64, ip_server, port);
    }
    std::cout<<"Ended udp latency test\n";
    sm.closeSocket(test_socket);
}