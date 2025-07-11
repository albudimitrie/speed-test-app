#pragma once
#include "iTest.h"
#include "nlohmann/json.hpp"
#include <unistd.h>
#include <cstdint>
using json=nlohmann::json;


class TCPUpload : public iTest
{
    json &config;
    int _duration;
    uint64_t _bytes_to_send;
    void run_time_based_test(int client_socket);
    void run_size_based_test(int client_socket);

public:
    TCPUpload(json &config, int duration, uint64_t bytes_to_send);
    virtual ~TCPUpload()=default;
    virtual void run(int client_socket) override;

};