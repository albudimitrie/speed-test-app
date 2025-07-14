#pragma once

#include <string>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>


class SocketManager {
public:
    SocketManager();
    ~SocketManager();

    int createTcpSocket(int port);
    int createUdpSocket(int port);
    int connectTo(const std::string& ip, int port);

    int acceptClient(int server_socket);

    int sendData(int socket_fd, char* buffer, int len);
    int receiveData(int socket_fd, char* buffer, int len);

    int sendTo(int socket_fd, char* buffer, int len, const std::string& ip, int port);
    int receiveFrom(int socket_fd, char* buffer, int len, std::string& sender_ip, int& sender_port);
    std::string receiveFrom(int socket_fd, int len, std::string &sender_ip, int &sender_port);
    void closeSocket(int socket_fd);

private:
    int createSocket(int type);
};
