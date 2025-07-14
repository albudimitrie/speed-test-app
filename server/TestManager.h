#pragma once
#include <string>
#include "TCPServer.h"
#include <unistd.h>
#include <cstdint>

class iTest;
enum class TestType { None, Network, Disk };
enum class Protocol { None, TCP, UDP };

struct ClientConfig {
    TestType test_type = TestType::None;
    Protocol protocol = Protocol::None;
    bool json_output = false;

    int duration_seconds = 10;
    uint64_t bytes_to_send = UINT64_MAX;

    int disk_block_size = -1;
};
class TestManager
{
    int _socket;
    ClientConfig _config;
    std::string _client_addr;
    void run_tcp_test(int duration,uint64_t bytes_to_send);
    void run_udp_test(int duration, uint64_t bytes_to_send);
    void run_udp_time_test(int duration);
    void run_udp_size_test(int duration);
public:
    TestManager(std::string client_addr) :_client_addr{client_addr}{};
    void receive_and_parse_client_config(TCPServer &server);
    void run_test();
};