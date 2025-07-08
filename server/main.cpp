#include <iostream>
#include "TCPServer.h"

int main()
{
    try{
        TCPServer server{12345};
        server.start();

    }
    catch(std::exception &e)
    {
        std::cout<<e.what();
    }
    return 0;
}