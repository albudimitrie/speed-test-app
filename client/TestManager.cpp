#include "TestManager.h"
#include "nlohmann/json.hpp"
#include "iTest.h"
#define SIZE 1024
using json=nlohmann::json;

void TestManager::add_test(iTest *test)
{
    _existing_tests[test->get_test_type()] = test;
}
TestManager::TestManager(ClientConfig &config)
    :_config{config}
{
    add_test(TestFactory::makeTCPDownloadTest(config.duration_seconds));
}
void TestManager::send_config_to_server(TCPClient &client)
{   
    std::cout<<"Sent config file\n";
    json config_json;
    if(_config.test_type == TestType::Network)
    {
        config_json["test_type"]="NETWORK_TEST";
    }
    else{
        config_json["test_type"]="DISK_TEST";
    }
    if(_config.protocol == Protocol::TCP)
    {
        config_json["protocol"]="TCP";
    }
    else{
        config_json["protocol"]="UDP";
    }
    config_json["duration"]=_config.duration_seconds;
    config_json["bytes_to_send"]=_config.bytes_to_send;
    config_json["disk_block_size"]=_config.disk_block_size;
    std::string config_str = config_json.dump();
    client.send_data(config_str);
}
std::string TestManager::receive_results_from_server(TCPClient &client)
{
    //TODO
    return std::string{};

}
void TestManager::print_statistics()
{
    //TODO
}
void TestManager::start_test(TCPClient &client)
{
    //DOWNLOAD TEST
    send_config_to_server(client);
    iTest * test= choose_test_type(client);
    test->run(client);


    //UPLOAD TEST FOR NETTEST
    if(_config.test_type==TestType::Network)
    {
        iTest * test= choose_test_type(client);
        test->run(client);
    }


    //LATENCY TEST FOR NETTEST
    if(_config.test_type==TestType::Network)
    {
        iTest * test= choose_test_type(client);
        test->run(client);
    }

    //TODO RECEIVE RESULT FROM SERVER
    std::string result = receive_results_from_server(client);
    print_statistics();


}
iTest * TestManager::choose_test_type(TCPClient &client)
{
    std::string test=client.receive_data(SIZE);
    json test_json = json::parse(test);
    iTest *t = _existing_tests[test_json["type"]];
    return t;
}