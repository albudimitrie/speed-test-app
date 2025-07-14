#pragma once
#include <iostream>
#include "nlohmann/json.hpp"
#include <cstdint>
#define BUFFER_SIZE 1024*64
#define DOWNLOAD_PORT 12346
#define UPLOAD_PORT 12347
#define LATENCY_PORT 12348
using json=nlohmann::json;

class iTest
{
public:
    virtual ~iTest() = default;
    virtual void run(int client_socket) = 0;

};

class TestFactory
{
public:
    static iTest * makeTcpDownloadTest(int duration, json &stats);
    static iTest *makeTcpUploadTest(int duration, uint64_t bytes_to_send, json& stats, std::string client_addr);
    static iTest *makeTCPLatencyTest(json &stats);
    static iTest *makeUDPDownloadTest(int duration, uint64_t bytes_to_send, json &stats, std::string client_addr);
};


