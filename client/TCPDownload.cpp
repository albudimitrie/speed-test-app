#include "TCPDownload.h"
#include "TCPClient.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include "nlohmann/json.hpp"
#include <iostream>
#include <chrono>

using json=nlohmann::json;
#define BUFFER_SIZE 1024*32

void TCPDownload::run(TCPClient &client)
{
    uint64_t total_received = 0;
    int received=0;
    int socket=client.get_client_socket();
    char buffer[BUFFER_SIZE + 1];
    //folosesc clock ca sa ma asigur ca nu primesc mai mult de _duration secunde, e o metoda de precautie
    //testul se opreste dupa ce recv primeste 0 sau <0, serverul face shutdown dupa cele 10 secunde
    auto start=std::chrono::steady_clock::now();
    while(true)
    {
        auto now = std::chrono::steady_clock::now();
        if(std::chrono::duration_cast<std::chrono::seconds>(now -start).count() >= _duration)
        {
            break;
        }
        received=recv(socket, buffer,BUFFER_SIZE, 0);
        if (received <= 0) {
            if (received == 0) {
                std::cout << "Conexiune inchisa de server (EOF)" << std::endl;
            } else {
                std::cerr << "Eroare la primire: " << strerror(errno) << std::endl;
            }
            break;
        }
        buffer[received]='\0';
        total_received+=received;
    }
    //trimit la server cat am primit ca el sa calculeze si sa mi trimita inapoi rezutlatele
    json response;
    std::cout<<"Am primit "<<total_received<<std::endl;
    response["total_received"]=total_received;
    client.send_data(response.dump());
    
}