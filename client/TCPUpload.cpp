#include "TCPUpload.h"
#include "TCPClient.h"
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <chrono>
#include "SocketManager.h"
#include <nlohmann/json.hpp>
using json=nlohmann::json;

TCPUploadSize::TCPUploadSize(uint64_t bytes_to_send)
    :_bytes_to_send{bytes_to_send}
{
    _test_type="TCP_UPLOAD_SIZE";
}
std::string TCPUploadSize::get_test_type()
{
    return _test_type;
}
void TCPUploadSize::run(TCPClient &client)
{
    std::cout<<"Starting TCP UPLOAD SIZE test\n";
    uint64_t total_sent=0;
    int sent=0;
    char buffer[BUFFER_SIZE + 1];
    strcpy(buffer, "TEST_UPLOAD_SIZE");
    while(true)
    {
        sent=client.send_data(buffer);
        total_sent+=sent;
        if(total_sent>=_bytes_to_send)
        {
            break;
        }
    }
    shutdown(client.get_client_socket(), SHUT_RDWR);

}
//----------------------------------------------------------
TCPUploadTime::TCPUploadTime(int duration)
    :_duration{duration}
{
    _test_type="TCP_UPLOAD_TIME";
}
std::string TCPUploadTime::get_test_type()
{
    return _test_type;
}
void TCPUploadTime::run(TCPClient &client)
{
    SocketManager sm;
    std::cout<<"Starting TCPUploadTime test\n";
    char buffer[BUFFER_SIZE + 1];
    strcpy(buffer, "TESTUPLOADTIMEabcdefsasafa");

    int new_socket=sm.createTcpSocket(UPLOAD_PORT);
    json port;
    port["port"]=UPLOAD_PORT;
    int sent =client.send_data(port.dump().c_str(), 1024);
    std::cout<<"Trimis <<<"<< sent<<std::endl;
    int test_socket=sm.acceptClient(new_socket);

    auto start=std::chrono::steady_clock::now();
    while(true)
    {
        sm.sendData(test_socket, buffer, BUFFER_SIZE);
        auto now = std::chrono::steady_clock::now();
        if(std::chrono::duration_cast<std::chrono::seconds>(now-start).count()>=_duration)
        {
            break;
        }
    }
    shutdown(test_socket, SHUT_WR);
    sm.closeSocket(test_socket);

}



