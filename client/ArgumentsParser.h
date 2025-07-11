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

    int duration_seconds = 10;
    uint64_t bytes_to_send = UINT64_MAX;

    int disk_block_size = -1;
    //TODO adaugare port
    //TODO adaugare adresa server
    int port = -1;
    std::string address= "EMPTY";
};


class ArgParser {
public:
    ArgParser(int argc, char* argv[]);
    const ClientConfig& get_config() const;


private:
    ClientConfig config;
    void parse_args(int argc, char* argv[]);
};
