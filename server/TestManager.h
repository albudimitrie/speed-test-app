#include <string>
#include "TCPServer.h"
enum class TestType { None, Network, Disk };
enum class Protocol { None, TCP, UDP };

struct ClientConfig {
    TestType test_type = TestType::None;
    Protocol protocol = Protocol::None;
    bool json_output = false;

    int duration_seconds = 10;
    int bytes_to_send = -1;

    int disk_block_size = -1;
};
class TestManager
{
    ClientConfig _config;
public:
    TestManager()=default;
    void receive_and_parse_client_config(TCPServer &server);
};