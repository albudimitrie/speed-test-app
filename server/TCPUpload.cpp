#include "TCPUpload.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include "SocketManager.h"


TCPUpload::TCPUpload(json &config, int duration, uint64_t bytes_to_send, std::string client_addr)
    :config{config}, _duration{duration}, _bytes_to_send{bytes_to_send}, _client_addr{client_addr}
{
}

void TCPUpload::run(int client_socket)
{
    if(_bytes_to_send == UINT64_MAX)
    {
        run_time_based_test(client_socket);
    }
    else{
        run_size_based_test(client_socket);
    }

}
void TCPUpload::run_time_based_test(int client_socket)
{
    std::cout<<"Starting tcp upload time test\n";
    SocketManager sm;
    json type;
    type["type"]="TCP_UPLOAD_TIME";
    //sending test type to client
    send(client_socket,type.dump().c_str(), 1024, 0);
    uint64_t total_received=0;
    int received=0;
    char buffer[BUFFER_SIZE+1];
    char port_char[1024];
    int n=sm.receiveData(client_socket, port_char, 1024);
    if(n<=0)
    {
        std::cout<<"Eroare primire port\n";
        throw std::runtime_error{"Eroare primire port\n"};
    }
    std::string port{port_char};
    json port_json=json::parse(port);
    int new_port = port_json["port"];
    int test_socket= sm.connectTo(_client_addr, new_port);
    //std::cout<<_client_addr<<"\n";
    //std::cout<<"Connected succesfully\n";
    auto start = std::chrono::steady_clock::now();
    while(true)
    {
        received = sm.receiveData(test_socket, buffer, BUFFER_SIZE);
        if(received<=0)
        {
            if(received==0)
            {
                std::cout<<"Client interrupted flow\n";
            }
            if(received< 0)
            {
                std::cout<<"Errors occured while receiving\n";
            }
            break;
        }
        total_received+=received;
        auto now =std::chrono::steady_clock::now();
        if(std::chrono::duration_cast<std::chrono::seconds>(now-start).count()>=_duration)
        {
            std::cout<<"Duration time elapsed\n";
            break;
        }
    }
    sm.closeSocket(test_socket);
    //TODO primire detalii de la client
    //TODO pregatire statistici si scriere in config
    double Mbps=(total_received*8)/(_duration*1000000);
    double MBps=(total_received)/(_duration*1000000);
    config["TCP_UPLOAD"]["Mbps"]=Mbps;
    config["TCP_UPLOAD"]["MBps"]=MBps;
    config["TCP_UPLOAD"]["duration"]=_duration;
    std::cout<<"Mbps:"<<Mbps<<std::endl;
    std::cout<<"MBps:"<<MBps<<std::endl;

}
void TCPUpload::run_size_based_test(int client_socket)
{
    std::cout<<"Starting TCP UPLOAD TEST\n";
    json type;
    type["type"]="TCP_UPLOAD_SIZE";
    send(client_socket,type.dump().c_str(), 1024, 0);
    uint64_t total_received=0;
    int received=0;
    char buffer[BUFFER_SIZE+1];
    auto start= std::chrono::steady_clock::now();
    while(true)
    {
        received=recv(client_socket, buffer, BUFFER_SIZE, 0);
        if(received<=0)
        {
            if(received==0)
            {
                std::cout<<"Client interrupted flow\n";
            }
            if(received< 0)
            {
                std::cout<<"Errors occured while receiving\n";
            }
            break;
        }
        else if(total_received==_bytes_to_send)
        {
            break;
            
        }
    }
    auto now = std::chrono::steady_clock::now();
    auto elapsed=std::chrono::duration_cast<std::chrono::seconds>(now-start).count();
    double Mbps=(total_received*8)/(elapsed*1000000);
    double MBps=(total_received)/(elapsed*1000000);
    config["TCP_UPLOAD"]["Mbps"]=Mbps;
    config["TCP_UPLOAD"]["MBps"]=MBps;
    config["TCP_UPLOAD"]["duration"]=elapsed;
    std::cout<<"Mbps:"<<Mbps<<std::endl;
    std::cout<<"MBps:"<<MBps<<std::endl;
}