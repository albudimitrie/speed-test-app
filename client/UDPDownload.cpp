#include "UDPDownload.h"
#include "TCPClient.h"
#include "SocketManager.h"
#include "nlohmann/json.hpp"
#include <thread>

using json = nlohmann::json;

UDPDownload::UDPDownload(int duration, uint64_t bytes_to_send, uint64_t bitrate_bps)
    :_duration{duration}, _bytes_to_send{bytes_to_send}, _bitrate_bps{bitrate_bps}
{
    _test_type="UDP_DOWNLOAD";
    if(_bitrate_bps == 0) 
    {
        _bitrate_bps = 20 * 1024 * 1024; 
    }
}
void UDPDownload::run(TCPClient &client)
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
        std::cout<<"Starting time udp download test\n";
        run_time_test(client);
    }



}
void UDPDownload::run_size_test(TCPClient &client)
{
    SocketManager sm;
    std::string info = client.receive_data(128);
    json info_port = json::parse(info);
    int port = info_port["port"];
    int socket = sm.createUdpSocket(port);
    std::cout<<"Created udp socket\n";
    std::string ip_server;
    uint64_t total_amount = _bytes_to_send;
    uint64_t total_received = 0;
    char buffer[1025];
    const size_t avg_packet_size = 1024; 
    auto packet_interval = calculatePacketInterval(avg_packet_size);
    auto start = std::chrono::steady_clock::now();
    auto last_packet_time = start;
    while(true)
    {
        int received= sm.receiveFrom(socket, buffer,1024, ip_server, port );
        buffer[received]='\0';
        std::string buffer_str{buffer};
        json test=json::parse(buffer_str);
        if(test["end"]=="true")
        {
            std::cout<<"End frame received\n";
            break;
        }
        if(total_received >= total_amount)
        {
            std::cout<<"Amount exceeded, received everything\n";
            break;
        }
        total_received+=received;

        //limitare la bitrate ales
        auto current_time = std::chrono::steady_clock::now();
        auto time_since_last = current_time - last_packet_time;
        if(time_since_last < packet_interval)
        {
            std::this_thread::sleep_for(packet_interval - time_since_last);
        }
        
        last_packet_time = std::chrono::steady_clock::now();
    }
    sm.closeSocket(socket);
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration<double>(now - start).count();
    json reply;
    reply["total_received"]=total_received;
    reply["elapsed"]=elapsed;
    client.send_data(reply.dump());

}

void UDPDownload::run_time_test(TCPClient &client)
{
    SocketManager sm;
    std::string info = client.receive_data(128);
    std::cout<<info<<"\n";
    json info_port = json::parse(info);
    int port = info_port["port"];
    int socket = sm.createUdpSocket(port);
    std::cout<<"Created udp socket\n";
    std::string ip_server;
    int expected_seq = 0;
    int total_frames_lost = 0;
    int total_bytes_received = 0;
    int total_frames_received = 0;
    char buffer[1025];
    const size_t avg_packet_size = 1024;
    auto packet_interval = calculatePacketInterval(avg_packet_size);
    std::cout<<"Ready to receive frames\n";
    auto start = std::chrono::steady_clock::now();
    auto last_packet_time = start;
    while(true)
    {
        auto now = std::chrono::steady_clock::now();
        if(std::chrono::duration_cast<std::chrono::seconds>(now-start).count() >=_duration)
        {
            std::cout<<"Time elapsed\n";
            break;
        }
        int received =sm.receiveFrom(socket, buffer, 1024, ip_server, port);
        if(received <= 0)
        {
            std::cout<<"Server finished sending frames\n";
            break;
        }
        buffer[received]='\0';
        json frame = json::parse(std::string{buffer});
        if(frame["end"]=="true")
        {
            std::cout<<"Received the end=true frame\n";
            break;
        }
        if(frame["sequence_number"] == expected_seq)
        {
            expected_seq++;
            total_frames_received++;
            total_bytes_received +=received;
        }
        else
        {
            int frame_received = frame["sequence_number"];
            total_frames_lost = frame_received - expected_seq;
            expected_seq=frame_received + 1;
        }
        auto current_time = std::chrono::steady_clock::now();
        auto time_since_last = current_time - last_packet_time;
        
        if(time_since_last < packet_interval) {
            std::this_thread::sleep_for(packet_interval - time_since_last);
        }
        
        last_packet_time = std::chrono::steady_clock::now();
    }
    sm.closeSocket(socket);
    json results;
    results["total_frames_lost"]=total_frames_lost;
    results["total_bytes_received"]=total_bytes_received;
    results["total_frames_received"]=total_frames_received;
    client.send_data(results.dump());
}


std::chrono::microseconds UDPDownload::calculatePacketInterval(size_t packet_size)
{
    uint64_t interval_us = (packet_size * 8 * 1000000ULL) / _bitrate_bps; //transform secunde in us si vad care e intervalul
    return std::chrono::microseconds(interval_us);
}
