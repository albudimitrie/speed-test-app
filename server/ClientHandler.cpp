#include "ClientHandler.h"
#include <thread>
#include <unistd.h>
#include <sys/socket.h>
#include <string>
#define SIZE 1024

ClientHandler::ClientHandler(int socket)
    :_client_socket{socket}
{

}
void ClientHandler::handle()
{
    char buffer[SIZE];
    while(true)
    {
        int n = read(this->_client_socket, buffer, sizeof(buffer)-1);
        if(n<=0)
        {
            break;
        }
        buffer[n]='\0';
        std::cout<<"Received from client\n";
        //primire fisier configurare de la client
    }
    close(this->_client_socket);
    std::cout<<"Client disconnected\n";
}