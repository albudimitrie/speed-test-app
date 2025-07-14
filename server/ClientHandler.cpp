#include "ClientHandler.h"
#include <thread>
#include <unistd.h>
#include <sys/socket.h>
#include <string>
#include "TestManager.h"

#define SIZE 1024

ClientHandler::ClientHandler(TCPServer &server)
    :_server{server}
{

}
void ClientHandler::handle()
{
    TestManager test_manager{_server.get_client_addr()};
    test_manager.receive_and_parse_client_config(_server);
    test_manager.run_test();
}