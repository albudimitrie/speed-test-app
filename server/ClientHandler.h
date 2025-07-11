#pragma once
#include <iostream>
#include "TCPServer.h"

class ClientHandler
{
    TCPServer & _server;
public:
    ClientHandler(TCPServer &server);
    void handle();

};
