#include "UDPDownload.h"
#include "SocketManager.h"
#include <cstdint>
#include <chrono>
#include <unistd.h>
#include <thread>


UDPDownload::UDPDownload(int duration, uint64_t bytes_to_send, json &stats, std::string client_addr)
    :_duration{duration}, _bytes_to_send{bytes_to_send}, _config{_config}, _client_addr{client_addr}
{

}


void UDPDownload::run(int client_socket)
{
    if(_bytes_to_send == UINT64_MAX)
    {
        run_udp_time_test(client_socket);
    }
    else{
        run_udp_size_test(client_socket);
    }
}

void UDPDownload::run_udp_size_test(int client_socket)
{
    SocketManager sm;
    json type;
    type["type"]="UDP_DOWNLOAD";
    //added some delay for synchronizing the client-server tansmission/reception
    //the frames were sent so fast that the client couldnt process them quick enough
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    int test_socket=sm.createUdpSocket(DOWNLOAD_PORT);
    sm.sendData(client_socket, (char *)type.dump().c_str(), 128);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    json test;
    test["criteria"]="SIZE";
    sm.sendData(client_socket, (char *)test.dump().c_str(), 128);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    json port;
    port["port"]=DOWNLOAD_PORT;
    sm.sendData(client_socket, (char *)port.dump().c_str(), 128);
    uint64_t total_sent = 0;
    std::cout<<"Bytes to send "<<_bytes_to_send<<"\n";
    while(true)
    {
        json buffer;
        std::string dummy_payload(900, 'X');
        buffer["end"]="false";
        buffer["data"]=dummy_payload;
        std::string payload = buffer.dump();
        if(total_sent >= _bytes_to_send)
        {
            buffer["end"]="true";
            std::string payload = buffer.dump();
            for(int i=0; i<512;i++)
            {
                sm.sendTo(test_socket, (char *)payload.c_str(), payload.size(), _client_addr, DOWNLOAD_PORT);
            }
            break;
        }
        int sent=sm.sendTo(test_socket,(char *)payload.c_str(), payload.size(), _client_addr, DOWNLOAD_PORT);
        total_sent+=sent;
    }
    sm.closeSocket(test_socket);
    char results[1024];
    sm.receiveData(client_socket, results, 1024);
    std::string res_str{results};
    json response = json::parse(res_str);
    std::cout<<response;

}
void UDPDownload::run_udp_time_test(int client_socket)
{
    SocketManager sm;
    json type;
    type["type"]="UDP_DOWNLOAD";
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    int test_socket = sm.createUdpSocket(DOWNLOAD_PORT);
    sm.sendData(client_socket, (char *)type.dump().c_str(), 128);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    json test;
    test["criteria"]="TIME";
    sm.sendData(client_socket, (char *)test.dump().c_str(), 128);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    json port;
    port["port"]=DOWNLOAD_PORT;
    sm.sendData(client_socket, (char *)port.dump().c_str(), 128);

    int seq_nr = 0;
    auto start = std::chrono::steady_clock::now();
    while(true)
    {
        json buffer;
        buffer["sequence_number"]=seq_nr++;
        buffer["end"]="false";
        sm.sendTo(test_socket, (char *)buffer.dump().c_str(), 1024, _client_addr, DOWNLOAD_PORT);
        auto now = std::chrono::steady_clock::now();
        if(std::chrono::duration_cast<std::chrono::seconds>(now-start).count()>=_duration)
        {
            buffer["end"]="true";
            std::cout<<"Sent ending frame\n";
            //sending 512 frames of stop because who know, maybe the frames will get lost
            for(int i=0; i<128;i++){
            sm.sendTo(test_socket, (char *)buffer.dump().c_str(), 128, _client_addr, DOWNLOAD_PORT);
            }
            break;
        }
    }
    sm.closeSocket(test_socket);
    char results[1024];
    sm.receiveData(client_socket, results, 1024);
    std::string res_str{results};
    json response = json::parse(res_str);
    std::cout<<response;



}