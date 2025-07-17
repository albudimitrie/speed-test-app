#include "UDPUpload.h"
#include "SocketManager.h"
#include <cstdint>
#include <chrono>
#include <unistd.h>
#include <thread>

UDPUpload::UDPUpload(int duration, uint64_t bytes_to_send, json &stats, std::string client_addr, uint64_t bitrate)
    :_duration{duration}, _bytes_to_send{bytes_to_send}, _config{stats}, _client_addr{client_addr}, _bitrate{bitrate}
{
    if(_bitrate == 0) {
        _bitrate = 20 * 1024 * 1024;
    }
}

void UDPUpload::run(int client_socket)
{
    if(_bytes_to_send == UINT64_MAX)
    {
        run_udp_time_test(client_socket);
    }
    else{
        run_udp_size_test(client_socket);
    }
}
void UDPUpload::run_udp_size_test(int client_socket)
{
    std::cout<<"Starting udp upload size test\n";
    SocketManager sm;
    json type;
    type["type"]="UDP_UPLOAD";
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    sm.sendData(client_socket, (char *)type.dump().c_str(), 128);
    json criteria;
    criteria["criteria"]="SIZE";
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    sm.sendData(client_socket, (char *)criteria.dump().c_str(), 128);
    json port;
    port["port"]=UPLOAD_PORT;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    sm.sendData(client_socket, (char *)port.dump().c_str(), 128);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    int u_port=UPLOAD_PORT;
    int socket= sm.createUdpSocket(UPLOAD_PORT);
    std::string ip_server;
    uint64_t total_amount = _bytes_to_send;
    uint64_t total_received = 0;
    uint64_t expected=0;
    uint64_t total_lost=0;
    char buffer[1025];
    size_t avg_packet_size = 1024; 
    auto packet_interval = calculatePacketInterval(avg_packet_size);
    auto start = std::chrono::steady_clock::now();
    auto last_packet_time = start;
    while(true)
    {
        int received= sm.receiveFrom(socket, buffer,1024, ip_server, u_port );
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
        uint64_t seq = test["sequence_number"].get<uint64_t>();
        if(expected == seq)
        {
            total_received+=received;
            expected++;
        }else
        {
            total_lost+=seq-expected;
            expected=seq+1;
        }

        //limitare la bitrate ales
        auto current_time = std::chrono::steady_clock::now();
        auto time_since_last = current_time - last_packet_time;
        if(time_since_last < packet_interval)
        {
            std::this_thread::sleep_for(packet_interval - time_since_last);
        }
        
        last_packet_time = std::chrono::steady_clock::now();
    }
    auto now = std::chrono::steady_clock::now();
    double elapsed= std::chrono::duration<double>(now-start).count();
    sm.closeSocket(socket);
    std::cout<<"Total primit:" <<total_received<<std::endl;
    std::cout<<"Total pierdut:" <<total_lost<<std::endl;
    std::cout<<"Total asetptat:" <<total_amount<<std::endl;
    double mbps = (total_received * 8.0) / 1000000.0 / elapsed;
    double MBps = (total_received) / 1024.0 / 1024.0 / elapsed;
    double gbps = (total_received * 8.0) / 1000000000.0 / elapsed;
    double GBps = (total_received) / (1024.0 * 1024.0 * 1024.0) / elapsed;

    double total_sent = total_amount; 
    double packet_loss_percent = 0.0;
    if (total_sent > 0) 
    {
        packet_loss_percent = 100.0 * total_lost / total_sent;
    }
    _config["UDP_UPLOAD_SIZE"]["packet_loss_percent"]=packet_loss_percent;
    _config["UDP_UPLOAD_SIZE"]["elapsed"]=elapsed;
    _config["UDP_UPLOAD_SIZE"]["Mbps"]=mbps;
    _config["UDP_UPLOAD_SIZE"]["MBps"]=MBps;
    _config["UDP_UPLOAD_SIZE"]["Gbps"]=gbps;
    _config["UDP_UPLOAD_SIZE"]["GBps"]=GBps;
}

void UDPUpload::run_udp_time_test(int client_socket)
{
    SocketManager sm;
    json type;
    type["type"]="UDP_UPLOAD";
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    sm.sendData(client_socket, (char *)type.dump().c_str(), 128);
    json criteria;
    criteria["criteria"]="TIME";
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    sm.sendData(client_socket, (char *)criteria.dump().c_str(), 128);
    json port;
    port["port"]=UPLOAD_PORT;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    sm.sendData(client_socket, (char *)port.dump().c_str(), 128);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    int u_port = UPLOAD_PORT;
    int test_socket= sm.createUdpSocket(u_port);
    std::string ip_client;
    uint64_t expected_seq = 0;
    uint64_t total_frames_lost = 0;
    uint64_t total_bytes_received = 0;
    uint64_t total_frames_received = 0;
    char buffer[1025];
    const size_t avg_packet_size = 1024;
    auto packet_interval = calculatePacketInterval(avg_packet_size);
    std::cout<<"Ready to receive framess\n";
    auto start = std::chrono::steady_clock::now();
    auto last_packet_time = start;
    std::cout<<ip_client<<"\n";
    while(true)
    {
        auto now = std::chrono::steady_clock::now();
        if(std::chrono::duration_cast<std::chrono::seconds>(now-start).count() >=_duration)
        {
            std::cout<<"Time elapsed\n";
            break;
        }
        int received =sm.receiveFrom(test_socket, buffer, 1024, ip_client, u_port);
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
        uint64_t seq = frame["sequence_number"].get<uint64_t>();
        if(seq == expected_seq)
        {
            expected_seq++;
            total_frames_received++;
            total_bytes_received +=received;
        }
        else
        {
            total_frames_lost = seq - expected_seq;
            expected_seq=seq + 1;
        }
        auto current_time = std::chrono::steady_clock::now();
        auto time_since_last = current_time - last_packet_time;
        
        if(time_since_last < packet_interval) 
        {
            std::this_thread::sleep_for(packet_interval - time_since_last);
        }
        
        last_packet_time = std::chrono::steady_clock::now();
    }
    sm.closeSocket(test_socket);
    std::cout<<"Total received:"<< total_bytes_received<<std::endl;
    std::cout<<"Total lost:"<< total_frames_lost*1024ULL<<std::endl;
    std::cout<<"Elapsed:"<<_duration<<std::endl;

    double mbps = (total_bytes_received * 8.0) / 1000000.0 / _duration;
    double MBps = (total_bytes_received) / 1024.0 / 1024.0 / _duration;
    double gbps = (total_bytes_received * 8.0) / 1000000000.0 / _duration;
    double GBps = (total_bytes_received) / (1024.0 * 1024.0 * 1024.0) / _duration;

    double total_sent = total_bytes_received/1024.0 + total_frames_lost; 
    double packet_loss_percent = 0.0;
    if (total_sent > 0) 
    {
        packet_loss_percent = 100.0 * total_frames_lost / total_sent;
    }
    _config["UDP_UPLOAD_TIME"]["packet_loss_percent"]=packet_loss_percent;
    _config["UDP_UPLOAD_TIME"]["elapsed"]=_duration;
    _config["UDP_UPLOAD_TIME"]["Mbps"]=mbps;
    _config["UDP_UPLOAD_TIME"]["MBps"]=MBps;
    _config["UDP_UPLOAD_TIME"]["Gbps"]=gbps;
    _config["UDP_UPLOAD_TIME"]["GBps"]=GBps;
}

std::chrono::microseconds UDPUpload::calculatePacketInterval(size_t packet_size)
{
    uint64_t interval_us = (packet_size * 8 * 1000000ULL) / _bitrate;
    return std::chrono::microseconds(interval_us);
}