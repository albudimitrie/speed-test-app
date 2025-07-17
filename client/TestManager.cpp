#include "TestManager.h"
#include "SocketManager.h"
#include <vector>

void TestManager::add_test(iTest *test)
{
    _existing_tests[test->get_test_type()] = test;
}
TestManager::TestManager(ClientConfig &config)
    :_config{config}
{
    add_test(TestFactory::makeTCPDownloadTest(config.duration_seconds));
    add_test(TestFactory::makeTCPUploadSizeTest(config.bytes_to_send));
    add_test(TestFactory::makeTCPUploadTimeTest(config.duration_seconds));
    add_test(TestFactory::makeTCPLatencyTest());
    add_test(TestFactory::makeUDPDownloadTest(config.duration_seconds, config.bytes_to_send, config.bitrate));
    add_test(TestFactory::makeUDPLatencytest());
    add_test(TestFactory::makeUDPUploadtest(config.duration_seconds, config.bytes_to_send, config.bitrate));
}
void TestManager::send_config_to_server(TCPClient &client)
{   
    std::cout<<"Sent config file\n";
    json config_json;
    if(_config.test_type == TestType::Network)
    {
        config_json["test_type"]="NETWORK_TEST";
    }
    else
    {
        config_json["test_type"]="DISK_TEST";
    }
    if(_config.protocol == Protocol::TCP)
    {
        config_json["protocol"]="TCP";
    }
    else
    {
        config_json["protocol"]="UDP";
    }
    config_json["duration"]=_config.duration_seconds;
    config_json["bytes_to_send"]=_config.bytes_to_send;
    config_json["disk_block_size"]=_config.disk_block_size;
    config_json["bitrate"]=_config.bitrate;
    std::string config_str = config_json.dump();
    client.send_data(config_str);
}
std::string TestManager::receive_results_from_server(TCPClient &client)
{
    std::string res_str = client.receive_data(1024);
    return res_str;
}
void TestManager::print_statistics(std::string result_string)
{
    _stats=json::parse(result_string);
    if(_config.json_output)
    {
        std::cout<<std::endl<<std::endl;
        std::cout<<_stats.dump(4);
    }
    else
    {
        if(_config.protocol==Protocol::TCP)
        {
            std::cout<<std::endl<<std::endl;
            print_tcp_nicely();
        }
        else if(_config.protocol==Protocol::UDP)
        {
            std::cout<<std::endl<<std::endl;
            print_udp_nicely();
        }
        else{
            print_disk_nicely();
        }

    }
}
void TestManager::start_test(TCPClient &client)
{
    //DOWNLOAD TEST
    if(_config.test_type == TestType::Network)
    {
    send_config_to_server(client);
    iTest * test= choose_test_type(client);
    test->run(client);
    }
    //UPLOAD TEST FOR NETTEST
    if(_config.test_type==TestType::Network)
    {
        std::cout<<"Choosing next test\n";
        iTest * test= choose_test_type(client);
        test->run(client);
    }
    //LATENCY TEST FOR NETTEST
    if(_config.test_type==TestType::Network)
    {
        iTest * test= choose_test_type(client);
        test->run(client);
    }
    if(_config.test_type==TestType::Network)
    {
        //TODO primire rezultate de la server
        print_statistics(receive_results_from_server(client));
    }


}
iTest * TestManager::choose_test_type(TCPClient &client)
{
    std::string test=client.receive_data(SIZE);
    std::cout<<std::endl<<test<<std::endl;
    json test_json = json::parse(test);
    iTest *t = _existing_tests[test_json["type"]];
    return t;
}

void TestManager::threat_disk_test()
{
    std::cout<<"Disk test\n";
    iTest * test = TestFactory::makeDiskTest(_config.disk_block_size, _config.read, _config.write, _config.fsize, _stats);
    TCPClient client{0,"1.2.3.4"};
    test->run(client);
    if(_config.json_output)
    {
        std::cout<<std::endl<<std::endl;
        std::cout<<_stats.dump(4);
    }
    else{
        std::cout<<std::endl<<std::endl;
        print_disk_nicely();
    }
    delete test;
}


void TestManager::print_disk_nicely()
{
    std::cout << "\n Disk I/O Test Results\n";

    auto format_unit = [](double value, const std::string& unit) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << value << " " << unit;
        return oss.str();
    };

    auto format_bytes = [](double bytes) -> std::string {
        const char* units[] = {"B", "KB", "MB", "GB"};
        int unit_index = 0;
        while (bytes >= 1024.0 && unit_index < 3) {
            bytes /= 1024.0;
            ++unit_index;
        }
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << bytes << " " << units[unit_index];
        return oss.str();
    };

    std::vector<std::string> tests = {"READ DISK TEST", "WRITE DISK TEST"};

    for (const auto& test_name : tests)
    {
        if (!_stats.contains(test_name)) continue;

        const auto& test = _stats[test_name];

        double total = 0.0;
        if (test.contains("total_read"))
            total = test["total_read"].is_number() ? test["total_read"].get<double>() : static_cast<double>(test["total_read"].get<int64_t>());
        else if (test.contains("total_written"))
            total = test["total_written"].is_number() ? test["total_written"].get<double>() : static_cast<double>(test["total_written"].get<int64_t>());

        std::cout << "\n- " << test_name << "\n";
        std::cout << "   - Total transferred : "
                  << format_unit(total / (1024.0 * 1024.0 * 1024.0), "GB") << "\n";
        std::cout << "   - Execution time    : "
                  << format_unit(test["elapsed"].get<double>(), "sec") << "\n";
        std::cout << "   - Block size        : "
                  << format_bytes(test["block_size"].get<double>()) << "\n";

        std::cout << "   - Speeds:\n";
        std::cout << "       - " << format_unit(test["MBps"].get<double>(), "MB/s") << "\n";
        std::cout << "       - " << format_unit(test["Mbps"].get<double>(), "Mbps") << "\n";
        std::cout << "       - " << format_unit(test["GBps"].get<double>(), "GB/s") << "\n";
        std::cout << "       - " << format_unit(test["Gbps"].get<double>(), "Gbps") << "\n";
    }

    std::cout << "\n";
}


void TestManager::print_tcp_nicely()
{
    std::cout << "═══════════════════════════════════════\n";
    if (_stats.contains("TCP_DOWNLOAD")) {
        std::cout << "              TCP DOWNLOAD\n";
        std::cout << "───────────────────────────────────────\n";
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "  Speed       : " << _stats["TCP_DOWNLOAD"]["MBps"].get<double>() << " MB/s ("
                  << _stats["TCP_DOWNLOAD"]["Mbps"].get<double>() << " Mbps)\n";
        std::cout << "  Duration    : " << _stats["TCP_DOWNLOAD"]["duration"].get<double>() << " s\n\n";
        }
        if (_stats.contains("TCP_UPLOAD")) {
            std::cout << "              TCP UPLOAD\n";
            std::cout << "───────────────────────────────────────\n";
            std::cout << std::fixed << std::setprecision(2);
            std::cout << "  Speed       : " << _stats["TCP_UPLOAD"]["MBps"].get<double>() << " MB/s ("
                      << _stats["TCP_UPLOAD"]["Mbps"].get<double>() << " Mbps)\n";
            std::cout << "  Duration    : " << _stats["TCP_UPLOAD"]["duration"].get<double>() << " s\n\n";
        }

        if (_stats.contains("TCP_LATENCY")) {
            std::cout << "              TCP LATENCY\n";
            std::cout << "───────────────────────────────────────\n";
            std::cout << std::fixed << std::setprecision(3);
            std::cout << "  Avg Latency : "
                      << _stats["TCP_LATENCY"]["average"].get<double>()<< " ms\n";
            std::cout << "  Tests       : " << _stats["TCP_LATENCY"]["tests"].get<int>() << "\n";
        }
        std::cout << "═══════════════════════════════════════\n";
}
void TestManager::print_udp_nicely()
{
       std::cout << "═══════════════════════════════════════\n";

    if (_stats.contains("UDP_DOWNLOAD_TIME")) {
        std::cout << "              UDP DOWNLOAD\n";
        std::cout << "───────────────────────────────────────\n";
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "  Speed       : " << _stats["UDP_DOWNLOAD_TIME"]["MBps"].get<double>() << " MB/s ("
                  << _stats["UDP_DOWNLOAD_TIME"]["Mbps"].get<double>() << " Mbps)\n";
        std::cout << "  Speed (Gbps): " << _stats["UDP_DOWNLOAD_TIME"]["Gbps"].get<double>() << " Gbps\n";
        std::cout << "  Duration    : " << _stats["UDP_DOWNLOAD_TIME"]["elapsed"].get<double>() << " s\n";
        std::cout << "  Loss        : "
                  << std::setprecision(3)
                  << _stats["UDP_DOWNLOAD_TIME"]["packets_loss_percent"].get<double>() << " %\n\n";
    }

    if (_stats.contains("UDP_UPLOAD_TIME")) {
        std::cout << "              UDP UPLOAD\n";
        std::cout << "───────────────────────────────────────\n";
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "  Speed       : " << _stats["UDP_UPLOAD_TIME"]["MBps"].get<double>() << " MB/s ("
                  << _stats["UDP_UPLOAD_TIME"]["Mbps"].get<double>() << " Mbps)\n";
        std::cout << "  Speed (Gbps): " << _stats["UDP_UPLOAD_TIME"]["Gbps"].get<double>() << " Gbps\n";
        std::cout << "  Duration    : " << _stats["UDP_UPLOAD_TIME"]["elapsed"].get<double>() << " s\n";
        std::cout << "  Loss        : "
                  << std::setprecision(3)
                  << _stats["UDP_UPLOAD_TIME"]["packet_loss_percent"].get<double>() << " %\n\n";
    }

    if (_stats.contains("UDP_LATENCY")) {
        std::cout << "              UDP LATENCY\n";
        std::cout << "───────────────────────────────────────\n";
        std::cout << std::fixed << std::setprecision(3);
        std::cout << "  Avg Latency : " << _stats["UDP_LATENCY"]["average"].get<double>() << " ms\n";
        std::cout << "  Tests       : " << _stats["UDP_LATENCY"]["tests"].get<int>() << "\n";
    }

    std::cout << "═══════════════════════════════════════\n";
}