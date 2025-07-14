#include "TCPLatency.h"
#include "SocketManager.h"
#include <chrono>



TCPLatency::TCPLatency(json &stats)
    :_stats{stats}
{

}

void TCPLatency::run(int client_socket)
{
    SocketManager sm;
    json type, port;
    int new_socket=sm.createTcpSocket(LATENCY_PORT);
    type["type"]="TCP_LATENCY";
    port["port"]=LATENCY_PORT;
    sm.sendData(client_socket,(char *)type.dump().c_str(), 1024);
    sm.sendData(client_socket, (char *)port.dump().c_str(), 64);
    int test_socket = sm.acceptClient(new_socket);
    double total_time = 0;
    for(int i=0; i<10;i++)
    {
        total_time+=round_trip_time(test_socket);
    }
    shutdown(test_socket, SHUT_RDWR);
    sm.closeSocket(test_socket);
    double average = total_time/10;
    _stats["TCP_LATENCY"]["average"]=average;
    _stats["TCP_LATENCY"]["tests"]=10;
    std::cout<<"AVERAGE TCP LATENCY(10 tests):"<<average<<std::endl;
}

double TCPLatency::round_trip_time(int test_socket)
{
    std::string ping{"ping"};
    SocketManager sm;
    auto start = std::chrono::steady_clock::now();
    sm.sendData(test_socket, (char *)ping.c_str(), 64);
    char pong[64];
    sm.receiveData(test_socket,pong, 64);
    auto end= std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    double ms = elapsed.count();
    return ms;
}