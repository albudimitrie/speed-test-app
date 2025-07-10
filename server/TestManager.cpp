#include "TestManager.h"
#include "nlohmann/json.hpp"

using json=nlohmann::json;
void TestManager::receive_and_parse_client_config(TCPServer &server)
{
    std::string config=server.receive_data(1024);
    json config_json = json::parse(config);
    //TODO
}