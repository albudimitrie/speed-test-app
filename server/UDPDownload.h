#pragma once
#include "iTest.h"
#include "nlohmann/json.hpp"
#include <cstdint>
using json=nlohmann::json;

class UDPDownload : public iTest
{
    std::string _client_addr;
    int _duration;
    uint64_t _bytes_to_send;
    json &_config;
    void run_udp_size_test(int client_socket);
    void run_udp_time_test(int client_socket);
public:
    UDPDownload(int duration, uint64_t bytes_to_send, json &stats, std::string client_addr);
    virtual ~UDPDownload()=default;
    virtual void run(int client_socket)override;
};