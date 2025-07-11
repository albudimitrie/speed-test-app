#include "TCPServer.h"
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <thread>
#include <unistd.h>
#include "ClientHandler.h"
#include <cerrno>
#include <cstring>

void TCPServer::start()
{   
    int sock= socket(AF_INET, SOCK_STREAM,IPPROTO_TCP);
    if(sock < 0)
    {
        throw std::runtime_error{"Socket creation failed"};
    }
    sockaddr_in server_address;
    server_address.sin_port=htons(this->_port);
    server_address.sin_addr.s_addr=htonl(INADDR_ANY);
    server_address.sin_family=AF_INET;

    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
    }
    if(bind(sock, (const sockaddr *)&server_address, sizeof(server_address))<0)
    {
        throw std::runtime_error{"Bind function failed\n"};
    }
    if(listen(sock, 5) < 0)
    {
        throw std::runtime_error{"Listen failed\n"};
    }
    this->_running= true;
    std::cout<<"Server is listening on port "<< this->_port<<std::endl;
    while(this->_running)
    {
        sockaddr_in client_addr;
        socklen_t client_len=sizeof(client_addr);
        int client_socket= accept(sock, (sockaddr *)&client_addr, &client_len);
        this->_socket=client_socket;
        if(client_socket < 0)
        {
            throw std::runtime_error{"Accept failed\n"};
        }
        std::cout<<"Client connected\n";
        ClientHandler handler{*this};
        handler.handle();
    }
    this->stop();
    
}
void TCPServer::stop()
{
    close(this->_socket);
    this->_running=false;
}

std::string TCPServer::receive_data(int max_len)
{
    char buffer[max_len+1];
    int n=recv(_socket, buffer, max_len, 0);
    if (n == -1) {
    std::cerr << "Receive failed: " << strerror(errno) << " (errno " << errno << ")\n";
    }
    buffer[n]='\0';
    return std::string{buffer};
}
void TCPServer::send_data(const std::string &data)
{
    send(_socket, data.c_str(), data.length(), 0);
}