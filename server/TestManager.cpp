#include "TestManager.h"
#include "nlohmann/json.hpp"
#include "iTest.h"
#include "SocketManager.h"

using json=nlohmann::json;
void TestManager::receive_and_parse_client_config(TCPServer &server)
{

    _socket=server.get_socket();
    std::string config=server.receive_data(1024);
    std::cout<<"Received config file\n"<<config<<"\n";
    json config_json = json::parse(config);
    //TODO
    if(config_json["test_type"] == "NETWORK_TEST")
    {
        _config.test_type=TestType::Network;
    }
    else if(config_json["test_type"]=="DISK_TEST")
    {
        _config.test_type=TestType::Disk;
    }

    if(config_json["protocol"]=="TCP")
    {
        _config.protocol=Protocol::TCP;
    }
    else if(config_json["protocol"]=="UDP")
    {
        _config.protocol=Protocol::UDP;
    }
    _config.duration_seconds=config_json["duration"];
    _config.bytes_to_send = config_json["bytes_to_send"].get<uint64_t>();
    _config.bitrate = config_json["bitrate"].get<uint64_t>();
    _config.disk_block_size= config_json["disk_block_size"];
}


void TestManager::run_test()
{
    if(_config.test_type == TestType::Network)
    {
        if(_config.protocol == Protocol::TCP)
        {
            run_tcp_test(_config.duration_seconds, _config.bytes_to_send);
        }
        else if(_config.protocol == Protocol::UDP)
        {
            run_udp_test(_config.duration_seconds, _config.bytes_to_send);
            
        }

    }
    else{
        std::cout<<"None or disktest provided\nIdle state\n";
    }


}

void TestManager::run_tcp_test(int duration, uint64_t bytes_to_send)
{
    std::cout<<"Starting tcp download test\n";
    json stats;
    //TCP DOWNLOAD
    try{
    iTest *download_tcp = TestFactory::makeTcpDownloadTest(duration, stats);
    download_tcp->run(_socket);
    iTest *upload_tcp = TestFactory::makeTcpUploadTest(duration, bytes_to_send, stats, _client_addr);
    upload_tcp->run(_socket);
    iTest *latency_tcp=TestFactory::makeTCPLatencyTest(stats);
    latency_tcp->run(_socket);
    send_results_to_client(stats);


    //delete download_tcp;
    delete upload_tcp;
    delete latency_tcp;
    }
    catch(std::exception &e)
    {
        std::cout<<e.what()<<std::endl;
    }

}

void TestManager::run_udp_test(int duration, uint64_t bytes_to_send)
{
    json stats;
    try
    {
        iTest *download_udp = TestFactory::makeUDPDownloadTest(duration, bytes_to_send,stats,_client_addr, _config.bitrate );
        download_udp->run(_socket);

        iTest *upload_udp = TestFactory::makeUDPUploadTest(duration, bytes_to_send, stats, _client_addr, _config.bitrate);
        upload_udp->run(_socket);

        iTest *latency_udp = TestFactory::makeUDPLatencyTest(stats,_client_addr);
        latency_udp->run(_socket);
        
        send_results_to_client(stats);
        delete download_udp;
        delete upload_udp;
        delete latency_udp;
    }
    catch(std::exception &e)
    {
        std::cout<<e.what();
    }
}

void TestManager::send_results_to_client(json &stats)
{
    SocketManager sm;
    std::string stats_str = stats.dump();
    int len = stats_str.length();
    sm.sendData(_socket, (char *)stats_str.c_str(),len);

}