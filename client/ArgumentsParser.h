#pragma once
#include <iostream>
#include <string>
#include <cstdint>

enum class TestType { None, Network, Disk };
enum class Protocol { None, TCP, UDP };

struct ClientConfig {
    TestType test_type = TestType::None;
    Protocol protocol = Protocol::None;
    bool json_output = false;

    int duration_seconds = -1;
    uint64_t bytes_to_send = UINT64_MAX;

    uint64_t disk_block_size = UINT64_MAX;
    //TODO adaugare port
    //TODO adaugare adresa server
    int port = -1;
    std::string address= "EMPTY";

    //client side(for disk test) options
    bool read = false;
    bool write= false;
    uint64_t fsize=4ULL*1024*1024*1024;//default 4GB
};


class ArgParser {
public:
    ArgParser(int argc, char* argv[]);
    const ClientConfig& get_config() const;


private:
    ClientConfig config;
    void parse_args(int argc, char* argv[]);
};
