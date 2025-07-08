#include <iostream>

class ClientHandler
{
    int _client_socket;
public:
    ClientHandler(int socket);
    void handle();

};
