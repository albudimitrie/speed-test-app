#include "TCPDownload.h"
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "nlohmann/json.hpp"
using json=nlohmann::json;

TCPDownload::TCPDownload(int duration)
    :_duration{duration}
{

}
void TCPDownload::run(int client_socket)
{
    //i will send a frame to the client TCP_DOWNLOAD TO LET HIM KNOW WHAT TEST I RUN ON HIM
    //after getting the request to this, the server will 
    //transmit data for n seconds based on the duration in 
    //the config file 
    //o sa trimit BUFFER_SIZE frames timp de n secunde apoi o sa trimit un frame de stop
    json type;
    type["type"]="TCP_DOWNLOAD";
    send(client_socket, type.dump().c_str(), 1024, 0);


    int total_duration=this->_duration;
    char buffer[BUFFER_SIZE+1];
    char ending_buffer[1024];
    strcpy(ending_buffer, "STOP TEST");
    auto start=std::chrono::steady_clock::now();
    while(true)
    {
        send(client_socket, buffer, BUFFER_SIZE, 0);
        auto now= std::chrono::steady_clock::now();
        if(std::chrono::duration_cast<std::chrono::seconds>(now -start).count() >= total_duration)
        {
            send(client_socket, ending_buffer,1024, 0);
            break;
        }
    }
    char result[1024];
    int n=recv(client_socket, result, 1024, 0);
    if(n<0)
    {
        throw std::runtime_error{"Client didnt send anything\nConnection error\n"};
    }
    result[n]='\0';
    std::string res_str{result};
    json res_json = json::parse(res_str);
    int total_received=res_json["total_received"];
    //TODO CALCULEAZA THROUGHPUT
    //TODO ar trebui sa fac sa primesc de la client cat a primit si sa calculez ca sa pun in report, asta o sa faca TestManagerul
}