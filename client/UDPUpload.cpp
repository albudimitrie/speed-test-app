#include "UDPUpload.h"
#include "TCPClient.h"
#include "SocketManager.h"
#include "nlohmann/json.hpp"
#include <thread>

using json = nlohmann::json;


UDPUpload::UDPUpload(int duration, uint64_t bytes_to_send, uint64_t bitrate_bps)
    :_duration{duration}, _bytes_to_send{bytes_to_send}, _bitrate_bps{bitrate_bps}
{
    _test_type="UDP_UPLOAD";
    if(_bitrate_bps == 0) 
    {
        _bitrate_bps = 20 * 1024 * 1024; 
    }
}
void UDPUpload::run(TCPClient &client)
{
    std::string type = client.receive_data(128);
    std::cout<<type<<"\n";
    json res = json::parse(type);
    if(res["criteria"]=="SIZE")
    {
        run_size_test(client);
    }
    else if(res["criteria"]=="TIME")
    {
        std::cout<<"Starting time udp upload test\n";
        run_time_test(client);
    }
}
void UDPUpload::run_size_test(TCPClient &client)
{
    SocketManager sm;
    std::string info = client.receive_data(128);
    std::cout<<info<<"\n";
    json info_port = json::parse(info);
    int port = info_port["port"];
    int test_socket = sm.createUdpSocket(port);
    uint64_t seq_nr = 0;
    size_t avg_packet_size = 1000;
    auto packet_interval = calculatePacketInterval(avg_packet_size);
    std::string ip_addr = client.get_server_addr();
    std::cout<<ip_addr<<std::endl;
    uint64_t total_sent=0;
    uint64_t seq = 0;
    auto start_time = std::chrono::steady_clock::now();
    auto next_send_time = start_time;
    while(true)
    {
        json buffer;
        std::string dummy_payload(800, 'X');
        buffer["end"]="false";
        buffer["data"]=dummy_payload;
        buffer["sequence_number"]=seq++;
        std::string payload = buffer.dump();
        if(total_sent >= _bytes_to_send)
        {
            buffer["end"]="true";
            std::string payload = buffer.dump();
            std::cout<<"Sending ending frame\n";
            for(int i=0; i<128;i++)
            {
                sm.sendTo(test_socket, (char *)payload.c_str(), payload.size(), ip_addr, port);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            break;
        }
        auto current_time = std::chrono::steady_clock::now();
        if(next_send_time > current_time)
        {
            std::this_thread::sleep_until(next_send_time);
        }
        int sent=sm.sendTo(test_socket,(char *)payload.c_str(), 1024, ip_addr, port);
        total_sent+=sent;
        next_send_time += packet_interval;
    }
    sm.closeSocket(test_socket);
}

void UDPUpload::run_time_test(TCPClient &client)
{
    SocketManager sm;
    std::string info = client.receive_data(128);
    std::cout<<info<<"\n";
    json info_port = json::parse(info);
    int port = info_port["port"];
    int test_socket = sm.createUdpSocket(port);
    uint64_t seq_nr = 0;
    size_t avg_packet_size = 1000;
    auto packet_interval = calculatePacketInterval(avg_packet_size);
    std::string ip_addr = client.get_server_addr();
    std::cout<<ip_addr<<std::endl;
    auto start = std::chrono::steady_clock::now();
    auto next_send_time = start;
    while(true)
    {
        json buffer;
        buffer["sequence_number"]=seq_nr++;
        buffer["end"]="false";
        auto now = std::chrono::steady_clock::now();
        if(std::chrono::duration_cast<std::chrono::seconds>(now-start).count()>=_duration)
        {
            buffer["end"]="true";
            std::cout<<"Sent ending frame\n";
            //sending 128 frames of stop because who know, maybe the frames will get lost
            for(int i=0; i<128;i++)
            {
                sm.sendTo(test_socket, (char *)buffer.dump().c_str(), 128, ip_addr, port);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            break;
        }
        auto current_time = std::chrono::steady_clock::now();
        if(next_send_time > current_time) {
            std::this_thread::sleep_until(next_send_time);
        }
        sm.sendTo(test_socket, (char *)buffer.dump().c_str(), 1024, ip_addr, port);
        next_send_time+=packet_interval;
    }
    sm.closeSocket(test_socket);
}


std::chrono::microseconds UDPUpload::calculatePacketInterval(size_t packet_size)
{
      uint64_t interval_us = (packet_size * 8 * 1000000ULL) / _bitrate_bps;
    return std::chrono::microseconds(interval_us);
}