#include "TCPClient.h"
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>


TCPClient::TCPClient(int port, std::string server_address)
    :_port{port}, _server_address{server_address}
{
}
void TCPClient::connect_to_server()
{
    this->_client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(this->_client_socket < 0)
    {
        throw std::runtime_error{"Socket creation failed\n"};
    }
    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(this->_port);

    if (inet_pton(AF_INET, _server_address.c_str(), &serv_addr.sin_addr) <= 0) {
        throw std::runtime_error{"Bad server address\n"};
    }
    if (connect(this->_client_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        throw std::runtime_error{"Connection failed\n"};
    }
}

void TCPClient::send_data(const std::string &data)
{
    send(this->_client_socket, data.c_str(), data.length(), 0);
}
std::string TCPClient::receive_data(int max_len)
{
    char buffer[MAX_LEN+1];
    int n= recv(this->_client_socket, buffer, MAX_LEN, 0);
    if( n < 0)
    {
        throw std::runtime_error{"No data received(empty)\n"};
    }
    buffer[n]='\0';
    return std::string{buffer};
}