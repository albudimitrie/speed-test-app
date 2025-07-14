#include "TCPDownload.h"
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "nlohmann/json.hpp"
#include <unistd.h> 
#include "SocketManager.h"

using json=nlohmann::json;

TCPDownload::TCPDownload(int duration, json &stats)
    :_duration{duration}, _stats{stats}
{

}
void TCPDownload::run(int client_socket)
{
    SocketManager sm;
    json type,port;
    type["type"]="TCP_DOWNLOAD";
    port["port"]=DOWNLOAD_PORT;
    send(client_socket, type.dump().c_str(), 1024, 0);
    int new_socket =sm.createTcpSocket(DOWNLOAD_PORT);
    send(client_socket, port.dump().c_str(), 1024, 0);
    int test_socket = sm.acceptClient(new_socket);

    int total_duration=this->_duration;
    char buffer[BUFFER_SIZE+1];
    char ending_buffer[1024];
    strcpy(ending_buffer, "STOP TEST");
    auto start=std::chrono::steady_clock::now();
    while(true)
    {
        sm.sendData(test_socket, buffer, BUFFER_SIZE);
        auto now= std::chrono::steady_clock::now();
        if(std::chrono::duration_cast<std::chrono::seconds>(now -start).count() >= total_duration)
        {
            break;
        }
    }
    char result[1024];
    shutdown(test_socket, SHUT_RDWR);
    sm.closeSocket(test_socket);
    int n=recv(client_socket, result, 1024, 0);
    if(n<0)
    {
        throw std::runtime_error{"Client didnt send anything\nConnection error\n"};
    }
    result[n]='\0';
    std::string res_str{result};
    json res_json = json::parse(res_str);
    uint64_t total_received = res_json.at("total_received").get<uint64_t>();
    std::cout<<"Clientul a primit "<<total_received<<"\n";
    double Mbps = (total_received * 8.0) /(_duration * 1000000);
    double MBps = (total_received) / (_duration * 1000000);
    _stats["TCP_DOWNLOAD"]["Mbps"]=Mbps;
    _stats["TCP_DOWNLOAD"]["MBps"]=MBps;
    _stats["TCP_DOWNLOAD"]["duration"]=_duration;
    std::cout<<"Mbps:"<<Mbps<<std::endl;
    std::cout<<"MBps:"<<MBps<<std::endl;
}