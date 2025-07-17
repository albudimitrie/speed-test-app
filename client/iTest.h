#pragma once
#include <string>
#include <cstdint>
#define BUFFER_SIZE 1024*64
#define DOWNLOAD_PORT 12346
#define UPLOAD_PORT 12347
#define LATENCY_PORT 12348

#include "nlohmann/json.hpp"
using json=nlohmann::json;

class TCPClient;
class iTest
{
protected:
    std::string _test_type;
public:
    virtual std::string get_test_type() = 0;
    virtual ~iTest() = default;
    virtual void run(TCPClient &client) = 0;
};

class TestFactory
{
public:
    static iTest *makeTCPDownloadTest(int duration);
    static iTest *makeTCPUploadSizeTest(uint64_t bytes_to_send);
    static iTest *makeTCPUploadTimeTest(int duration);
    static iTest *makeTCPLatencyTest();
    static iTest *makeUDPDownloadTest(int duration, uint64_t bytes_to_send, uint64_t bitrate_bps);
    static iTest *makeDiskTest(uint64_t block_size, bool read, bool write,uint64_t file_size,json &stats);
    static iTest *makeUDPLatencytest();
    static iTest * makeUDPUploadtest(int duration, uint64_t bytes_to_send, uint64_t bitrate_bps);
};