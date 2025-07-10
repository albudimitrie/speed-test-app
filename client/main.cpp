#include <iostream>
#include "TCPClient.h"
#include "ArgumentsParser.h"
#include "TestManager.h"

int main(int argc, char **argv)
{
    try
    {
        ArgParser parser{argc, argv};
        ClientConfig config = parser.get_config();
        TCPClient client{config.port,config.address};
        client.connect_to_server();
        TestManager testManager{config};
        testManager.start_test(client);
    }
    catch(std::exception &e)
    {
        std::cout<<e.what();
    }
    return 0;

}