#include <iostream> 
#include <string>
#define MAX_LEN 1024

class TCPClient
{
    int _port;
    std::string _server_address;
    int _client_socket;
public:
    int get_client_socket() { return _client_socket;}
    TCPClient(int port, std::string _server_address);
    void connect_to_server();
    void send_data(const std::string &data);
    std::string receive_data(int max_len);


};