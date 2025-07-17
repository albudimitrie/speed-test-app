#include "SocketManager.h"
#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include <iostream>

SocketManager::SocketManager() {}

SocketManager::~SocketManager() {}

int SocketManager::createSocket(int type) {
    int sock = socket(AF_INET, type, 0);
    if (sock < 0) {
        throw std::runtime_error("Failed to create socket");
    }
    return sock;
}
int SocketManager::createTcpSocket(int port) {
    int sock = createSocket(SOCK_STREAM);

    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(sock);
        throw std::runtime_error("Failed to set SO_REUSEADDR on TCP socket");
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
         int err = errno; // salvezi codul erorii
        close(sock);
        std::cerr << "Failed to bind TCP socket on port " << ntohs(addr.sin_port)
              << ": " << strerror(err) << " (errno: " << err << ")\n";
        throw std::runtime_error("Failed to bind TCP socket");
    }

    if (listen(sock, 5) < 0) {
        close(sock);
        throw std::runtime_error("Failed to listen on TCP socket");
    }

    return sock;
}


int SocketManager::createUdpSocket(int port) {
    int sock = createSocket(SOCK_DGRAM);

    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(sock);
        throw std::runtime_error("Failed to set SO_REUSEADDR on UDP socket");
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        close(sock);
        throw std::runtime_error("Failed to bind UDP socket");
    }

    return sock;
}


int SocketManager::connectTo(const std::string& ip, int port) {
    int sock = createSocket(SOCK_STREAM);

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr) <= 0) {
        close(sock);
        throw std::runtime_error("Invalid IP address");
    }

    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(sock);
        throw std::runtime_error("Failed to connect to server");
    }

    return sock;
}

int SocketManager::acceptClient(int server_socket) {
    sockaddr_in client_addr{};
    socklen_t addr_len = sizeof(client_addr);
    int client_socket = accept(server_socket, (sockaddr*)&client_addr, &addr_len);
    if (client_socket < 0) {
        throw std::runtime_error("Failed to accept client");
    }
    return client_socket;
}

int SocketManager::sendData(int socket_fd, char* buffer, int len) {
    return send(socket_fd, buffer, len, 0);
}

int SocketManager::receiveData(int socket_fd, char* buffer, int len) {
    int n=recv(socket_fd, buffer, len, 0);
    buffer[n]='\0';
    return n;
}

int SocketManager::sendTo(int socket_fd, char* buffer, int len, const std::string& ip, int port) {
    sockaddr_in dest_addr{};
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &dest_addr.sin_addr) <= 0) {
        throw std::runtime_error("Invalid destination IP");
    }

    return sendto(socket_fd, buffer, len, 0, (sockaddr*)&dest_addr, sizeof(dest_addr));
}

int SocketManager::receiveFrom(int socket_fd, char* buffer, int len, std::string& sender_ip, int& sender_port) {
    sockaddr_in sender_addr{};
    socklen_t addr_len = sizeof(sender_addr);

    int bytes_received = recvfrom(socket_fd, buffer, len, 0, (sockaddr*)&sender_addr, &addr_len);
    if (bytes_received >= 0) {
        char ip_buf[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(sender_addr.sin_addr), ip_buf, sizeof(ip_buf));
        sender_ip = std::string(ip_buf);
        sender_port = ntohs(sender_addr.sin_port);
    }
    return bytes_received;
}
std::string SocketManager::receiveFrom(int socket_fd, int len, std::string &sender_ip, int &sender_port)
{

    sockaddr_in sender_addr{};
    socklen_t addr_len = sizeof(sender_addr);
    char buffer[len +1];
    int bytes_received = recvfrom(socket_fd, buffer, len, 0, (sockaddr*)&sender_addr, &addr_len);
    if (bytes_received >= 0) {
        char ip_buf[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(sender_addr.sin_addr), ip_buf, sizeof(ip_buf));
        sender_ip = std::string(ip_buf);
        sender_port = ntohs(sender_addr.sin_port);
        buffer[bytes_received]='\0';
    }

    return std::string{buffer};
}


void SocketManager::closeSocket(int socket_fd) {
    close(socket_fd);
}
