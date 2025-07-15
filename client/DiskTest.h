#pragma once
#include "iTest.h"
#include <cstdint>
#include "nlohmann/json.hpp"
#define file_name "DISKTESTFILE_IO"
using json=nlohmann::json;
class TCPClient;
class DiskTest : public iTest
{
    uint64_t _filesize;
    uint64_t _block_size;
    bool _read_mode;
    bool _write_mode;
    bool _random_access;
    json &_stats;
    void read_test();
    void write_test();
    void generate_file(std::string filename);
    void delete_file(std::string filename);
public:
    DiskTest(uint64_t block_size, bool read, bool write, uint64_t file_size ,json &stats);
    virtual ~DiskTest()=default;
    virtual void run(TCPClient &client)override;
    virtual std::string get_test_type() override;

};