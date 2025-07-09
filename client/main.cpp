#include <iostream>
#include "TCPClient.h"
#include "ArgumentsParser.h"

int main(int argc, char **argv)
{
    try
    {
        ArgParser parser{argc, argv};
        ClientConfig config = parser.get_config();
        std::cout<<"Bytes to send "<< config.bytes_to_send<<std::endl;
        std::cout<<"Test Type" << static_cast<int>(config.test_type) <<std::endl;
        std::cout<<"Protocol " << static_cast<int>(config.protocol) << std::endl;
        std::cout<<"Json output " << static_cast<int>(config.json_output)<<std::endl;
    }
    catch(std::exception &e)
    {
        std::cout<<e.what();
    }

    return 0;

}