#include "TCPDownload.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include "nlohmann/json.hpp"
using json=nlohmann::json;
#define BUFFER_SIZE 1024*32

void TCPDownload::run(TCPClient &client)
{
    int total_received=0, received;
    int socket=client.get_client_socket();
    char buffer[BUFFER_SIZE + 1];
    while(true)
    {
        received=recv(socket, buffer,BUFFER_SIZE, 0);
        buffer[received]='\0';
        if(strstr(buffer, "STOP TEST")!= NULL)
        {
            std::cout<<"Received stop frame for TCP DOWNLOAD test\n";
            break;
        } 
        total_received+=received;
    }
    //trimit la server cat am primit ca el sa calculeze si sa mi trimita inapoi rezutlatele
    json response;
    response["total_received"]=total_received;
    client.send_data(response.dump());
    
}