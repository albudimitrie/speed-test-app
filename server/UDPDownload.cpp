#include "UDPDownload.h"
#include "SocketManager.h"
#include <cstdint>
#include <chrono>
#include <unistd.h>
#include <thread>


UDPDownload::UDPDownload(int duration, uint64_t bytes_to_send, json &stats, std::string client_addr, uint64_t bitrate)
    :_duration{duration}, _bytes_to_send{bytes_to_send}, _config{stats}, _client_addr{client_addr} , _bitrate{_bitrate}
{
    if(_bitrate == 0) {
        _bitrate = 20 * 1024 * 1024;
    }

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
    std::cout<<"Starting udp download size test\n";
    SocketManager sm;
    json type;
    type["type"]="UDP_DOWNLOAD";
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
    int little_delay=10;
    uint64_t frames_sent= 0;
    const size_t payload_size = 1024;
    auto packet_interval = calculatePacketInterval(payload_size);
    auto start_time = std::chrono::steady_clock::now();
    auto next_send_time = start_time;
    while(true)
    {
        json buffer;
        std::string dummy_payload(800, 'X');
        buffer["end"]="false";
        buffer["data"]=dummy_payload;
        std::string payload = buffer.dump();
        if(total_sent >= _bytes_to_send)
        {
            buffer["end"]="true";
            std::string payload = buffer.dump();
            for(int i=0; i<128;i++)
            {
                sm.sendTo(test_socket, (char *)payload.c_str(), payload.size(), _client_addr, DOWNLOAD_PORT);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            break;
        }
        auto current_time = std::chrono::steady_clock::now();
        if(next_send_time > current_time)
        {
            std::this_thread::sleep_until(next_send_time);
        }
        int sent=sm.sendTo(test_socket,(char *)payload.c_str(), payload.size(), _client_addr, DOWNLOAD_PORT);
        frames_sent++;
        total_sent+=sent;
        next_send_time += packet_interval;
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
    const size_t avg_packet_size = 1000;
    auto packet_interval = calculatePacketInterval(avg_packet_size);
    
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
            for(int i=0; i<128;i++){
            sm.sendTo(test_socket, (char *)buffer.dump().c_str(), 128, _client_addr, DOWNLOAD_PORT);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            break;
        }
        auto current_time = std::chrono::steady_clock::now();
        if(next_send_time > current_time) {
            std::this_thread::sleep_until(next_send_time);
        }
        sm.sendTo(test_socket, (char *)buffer.dump().c_str(), 1024, _client_addr, DOWNLOAD_PORT);
        next_send_time+=packet_interval;
    }

    sm.closeSocket(test_socket);
    char results[1024];
    sm.receiveData(client_socket, results, 1024);
    std::string res_str{results};
    json response = json::parse(res_str);
    std::cout<<response;
    uint64_t total_frames_received =response["total_frames_received"].get<uint64_t>();
    uint64_t total_frames_lost = response["total_frames_lost"].get<uint64_t>();
    uint64_t total_bytes_received = response["total_bytes_received"].get<uint64_t>();


    uint64_t total_frames_sent = total_frames_received + total_frames_lost;
    double packet_loss_percent = 0.0;
    if (total_frames_sent > 0) 
    {
        packet_loss_percent = 100.0 * total_frames_lost / total_frames_sent;
    }
    double mbps = (total_bytes_received * 8.0) / 1000000.0 / _duration;
    double MBps = (total_bytes_received) / 1024.0 / 1024.0 / _duration;
    double gbps = (total_bytes_received * 8.0) / 1000000000.0 / _duration;
    double GBps = (total_bytes_received) / (1024.0 * 1024.0 * 1024.0) / _duration;
    _config["UDP_DOWNLOAD_TIME"]["packets_loss_percent"]= packet_loss_percent;
    _config["UDP_DOWNLOAD_TIME"]["Mbps"]=mbps;
    _config["UDP_DOWNLOAD_TIME"]["MBps"]=MBps;
    _config["UDP_DOWNLOAD_TIME"]["Gbps"]=gbps;
    _config["UDP_DOWNLOAD_TIME"]["GBps"]=GBps;
    _config["UDP_DOWNLOAD_TIME"]["elapsed"]=_duration;

}


std::chrono::microseconds UDPDownload::calculatePacketInterval(size_t packet_size)
{
    uint64_t interval_us = (packet_size * 8 * 1000000ULL) / _bitrate;
    return std::chrono::microseconds(interval_us);
}