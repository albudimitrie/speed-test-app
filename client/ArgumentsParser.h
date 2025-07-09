#include <iostream>


enum class TestType { None, Network, Disk };
enum class Protocol { None, TCP, UDP };

struct ClientConfig {
    TestType test_type = TestType::None;
    Protocol protocol = Protocol::None;
    bool json_output = false;

    int duration_seconds = -1;
    int bytes_to_send = -1;

    int disk_block_size = -1;
    //TODO adaugare port
    //TODO adaugare adresa server
};


class ArgParser {
public:
    ArgParser(int argc, char* argv[]);
    const ClientConfig& get_config() const;


private:
    ClientConfig config;
    void parse_args(int argc, char* argv[]);
};
