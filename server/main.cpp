#include <iostream>
#include "TCPServer.h"


int main(int argc, char **argv)
{
    try{
        if(argc < 2)
        {
            throw std::runtime_error{"Not enough arguments given\n"};
        }

        TCPServer server{atoi(argv[1])};
        server.start();

    }
    catch(std::exception &e)
    {
        std::cout<<e.what();
    }
    return 0;
}