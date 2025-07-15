#pragma once
#include "ArgumentsParser.h"
#include "TCPClient.h"
#include "nlohmann/json.hpp"
#include "iTest.h"
#include <map>

#define SIZE 1024
using json=nlohmann::json;

class iTest;
class TestManager
{
    json _stats;
    ClientConfig &_config;
    std::map<std::string,iTest *> _existing_tests;
    void send_config_to_server(TCPClient &client);
    std::string receive_results_from_server(TCPClient &client);
    iTest * choose_test_type(TCPClient &client);
    void add_test(iTest *test);
    void print_disk_nicely();
    void print_tcp_nicely();
    void print_udp_nicely();
public:
    TestManager(ClientConfig &config);
    void print_statistics(std::string results_str);
    void start_test(TCPClient &client);
    void threat_disk_test();


};