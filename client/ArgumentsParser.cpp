#include "ArgumentsParser.h"
#include <stdexcept>
#include <string>
#include "utils.h"

ArgParser::ArgParser(int argc, char* argv[]) {
    parse_args(argc, argv);
}

const ClientConfig& ArgParser::get_config() const {
    return config;
}

void ArgParser::parse_args(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i)
     {
        std::string arg = argv[i];

        if (arg == "-nettest") {
            config.test_type = TestType::Network;
        } else if (arg == "-disktest") {
            config.test_type = TestType::Disk;
        } else if (arg == "-tcp") {
            config.protocol = Protocol::TCP;
        } else if (arg == "-udp") {
            config.protocol = Protocol::UDP;
        } else if (arg == "-json") {
            config.json_output = true;
        } else if (arg == "-t" && i + 1 < argc) 
        {
            config.duration_seconds = std::stoi(argv[++i]);
        } else if (arg == "-b" && i + 1 < argc)
         {
            config.bytes_to_send = utils::parseSizeWithSuffix(argv[++i]);
        } else if (arg == "-s" && i + 1 < argc) 
        {
            if(config.test_type!= TestType::Disk)
            {
                throw std::runtime_error{"You can have -s only in DiskTest\n"};
            }
            config.disk_block_size = utils::parseSizeWithSuffix(argv[++i]);
        } else if (arg =="-port" && i+1 <argc)
        {
            config.port=std::stoi(argv[++i]);
            int port=config.port;
            if(port==12346 || port == 12347 || port == 12348)
            {
                throw std::runtime_error{"You cannot use this port. It's already used for the tests. Please provide another one\n"};
            }
        } else if( arg == "-host" && i+1 < argc)
        {
            config.address=std::string{argv[++i]};
        }
        else if(arg=="-read")
        {
            config.read=true;
        }
        else if(arg=="-write" )
        {
            config.write=true;
        }
        else if(arg=="-h" | arg =="-help")
        {
            config._help=true;
        }
        else if(arg == "-bitrate" && i+1 <argc)
        {
            if(config.protocol != Protocol::UDP)
            {
                throw std::runtime_error{"You can set bitrate only in UDP tests\n"};
            }
            config.bitrate= utils::parseSizeWithSuffix(argv[++i]);
        }
        else if(arg=="-fsize" && i+1 < argc)
        {
            if(config.test_type != TestType::Disk)
            {
                throw std::runtime_error{"You can have filesize only in Disktest\n"};
            }
            config.fsize=utils::parseSizeWithSuffix(argv[++i]);
        }
        else {
            throw std::runtime_error("Unknown or malformed argument: " + arg);
        }
    }



    if (config.duration_seconds != -1 && config.bytes_to_send != UINT64_MAX)
    {
    throw std::runtime_error("Cant have -t and -b simultaneously on udp test\n ");
    }
    if((config.port == -1 || config.address=="EMPTY") && config.test_type==TestType::Network)
    {   
        throw std::runtime_error{"You have to provide port and host address!\n"};
    }
    if(config.read == false && config.write == false)
    {
        config.read= true;
        config.write= true;
    }
    if(config.test_type==TestType::Network)
    {
        if(config.duration_seconds ==-1  && config.bytes_to_send==UINT64_MAX)
        {
            config.duration_seconds =10;
            //setting default value
        }
    }
    if(config.test_type== TestType::Disk)
    {
        if(config.disk_block_size == UINT64_MAX)
        {
            config.disk_block_size = 8*1024*1024;
        }
    }
    if(config._help)
    {
        print_help_menu();
    }
    
}

void ArgParser::print_help_menu()
{
    std::cout << "═════════════════════════════════════════════════\n";
    std::cout << "               Network & Disk Tester\n";
    std::cout << "═════════════════════════════════════════════════\n";
    std::cout << "Usage: ./speedtest_client [options]\n\n";
    std::cout << "Test Type:\n";
    std::cout << "  -nettest               Perform a network test (TCP/UDP).\n";
    std::cout << "  -disktest              Perform a disk read/write test.\n\n";

    std::cout << "Protocol (for network tests):\n";
    std::cout << "  -tcp                   Use TCP protocol.\n";
    std::cout << "  -udp                   Use UDP protocol.\n\n";

    std::cout << "Output:\n";
    std::cout << "  -json                  Output results in JSON format.\n";
    std::cout << "  -h, -help              Show this help message.\n\n";

    std::cout << "Network Options:\n";
    std::cout << "  -host <address>        Server address to connect to.\n";
    std::cout << "  -port <port>           Port number to connect to. You cannot use 12346, 12347 and 12348 as ports because they are used in tests\n";
    std::cout << "  -t <seconds>           Test duration (seconds). Cannot be used with -b.\n";
    std::cout << "  -b <size>              Bytes to send. Accepts suffixes (e.g., 10M, 10G).\n";
    std::cout << "  -bitrate <rate>        Bitrate for UDP test only (e.g., 10M ; M=Mbps, B=bps, G=Gbps, Default=8M).\n\n";

    std::cout << "Disk Options:\n";
    std::cout << "  -read                  Perform read test.\n";
    std::cout << "  -write                 Perform write test.\n";
    std::cout << "  -s <block_size>        Disk block size (e.g., 4KB, 1MB).\n";
    std::cout << "  -fsize <file_size>     Total file size for disk test (e.g., 100M; Default=4GB).\n\n";

    std::cout << "Notes:\n";
    std::cout << "  -t and -b are mutually exclusive in UDP tests.\n";
    std::cout << "  -s can be used only in  disk test\n";
    std::cout << "  If neither -read nor -write is specified in disktest, both are assumed.\n";
    std::cout << "═════════════════════════════════════════════════\n";
    exit(0);

}
