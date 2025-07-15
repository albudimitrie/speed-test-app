#include <iostream>
#include "TCPClient.h"
#include "ArgumentsParser.h"
#include "TestManager.h"
#include "iTest.h"

int main(int argc, char **argv)
{
    try
    {
        ArgParser parser{argc, argv};
        ClientConfig config = parser.get_config();
        TestManager testManager{config};
        if(config.test_type==TestType::Disk)
        {
            testManager.threat_disk_test();
        }
        else{
        TCPClient client{config.port,config.address};
        client.connect_to_server();
        testManager.start_test(client);
        }
    }
    catch(std::exception &e)
    {
        std::cout<<e.what();
    }
    return 0;

}