#include "ArgumentsParser.h"
#include "TCPClient.h"
#include <map>
class iTest;
class TestManager
{
    ClientConfig &_config;
    std::map<std::string,iTest *> _existing_tests;
    void send_config_to_server(TCPClient &client);
    std::string receive_results_from_server(TCPClient &client);
    iTest * choose_test_type(TCPClient &client);
    void add_test(iTest *test);
public:
    TestManager(ClientConfig &config);
    void print_statistics();
    void start_test(TCPClient &client);


};