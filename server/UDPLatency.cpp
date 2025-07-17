#include "UDPLatency.h"
#include "SocketManager.h"


UDPLatency::UDPLatency(json &stats, std::string client_address)
    :_stats{stats}, _client_addr{client_address}
{

}
void UDPLatency::run(int client_socket)
{
    SocketManager sm;
    json type, port;
    int new_socket=sm.createUdpSocket(LATENCY_PORT);
    struct timeval tv;
    tv.tv_sec = 1; 
    tv.tv_usec = 0;
    setsockopt(new_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    type["type"]="UDP_LATENCY";
    port["port"]=LATENCY_PORT;
    port["times"]=10;
    sm.sendData(client_socket,(char *)type.dump().c_str(), 1024);
    sm.sendData(client_socket, (char *)port.dump().c_str(), 512);
    double total_time = 0;
    int succesful=0;
    for(int i=0; i<10;i++)
    {
        double time=round_trip_time(new_socket);
        std::cout<<time<<"\n";
        if(time!=-1)
        {
            succesful++;
            total_time+=time;
        }
    }
    
    sm.closeSocket(new_socket);
    double average = total_time/succesful;
    _stats["UDP_LATENCY"]["average"]=average;
    _stats["UDP_LATENCY"]["tests"]=succesful;
    std::cout<<"\nAVERAGE UDP LATENCY(10 tests):"<<average<<std::endl;
}
double UDPLatency::round_trip_time(int test_socket)
{
    std::string ping{"ping"};
    SocketManager sm;
    char buffer[200];

    //adaugare timeout in caz ca se pierde un frame
    auto start = std::chrono::steady_clock::now();
    sm.sendTo(test_socket, (char *)ping.c_str(), 64, _client_addr, LATENCY_PORT);
    char pong[64];
    int port= LATENCY_PORT;
    int rec= sm.receiveFrom(test_socket,buffer, 64, _client_addr, port);
    if(rec<=0)
    {
        return -1;
    }
    auto end= std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    double ms = elapsed.count();
    return ms;
}