#include "TCPDownload.h"
#include "TCPClient.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include "nlohmann/json.hpp"
#include <iostream>
#include <chrono>
#include "SocketManager.h"

using json=nlohmann::json;

void TCPDownload::run(TCPClient &client)
{
    uint64_t total_received = 0;
    int received=0;
    char buffer[BUFFER_SIZE + 1];
    //folosesc clock ca sa ma asigur ca nu primesc mai mult de _duration secunde, e o metoda de precautie
    //testul se opreste dupa ce recv primeste 0 sau <0, serverul face shutdown dupa cele 10 secunde

    //test on another socket
    std::string port_str=client.receive_data(1024);
    json port=json::parse(port_str);
    int new_port=port["port"];
    std::cout<<"NEW PORT     "<<new_port<<"\n";
    SocketManager sm;
    int new_socket=sm.connectTo(client.get_server_addr(), new_port);
    auto start=std::chrono::steady_clock::now();
    while(true)
    {
        auto now = std::chrono::steady_clock::now();
         if(std::chrono::duration_cast<std::chrono::seconds>(now -start).count() >= _duration)
         {
             break;
         }
        received=sm.receiveData(new_socket, buffer, BUFFER_SIZE);
        if (received <= 0) {
            if (received == 0) {
                std::cout << "Conexiune inchisa de server (EOF)" << std::endl;
            } else {
                std::cerr << "Eroare la primire: " << strerror(errno) << std::endl;
            }
            sm.closeSocket(new_socket);
            break;
        }
        buffer[received]='\0';
        total_received+=received;
    }
    //trimit la server cat am primit ca el sa calculeze si sa mi trimita inapoi rezutlatele
    //trimit rezultatele la socketul original
    json response;
    std::cout<<"Am primit "<<total_received<<std::endl;
    response["total_received"]=total_received;
    client.send_data(response.dump());
    
}