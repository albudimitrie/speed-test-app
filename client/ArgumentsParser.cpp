#include "ArgumentsParser.h"
#include <stdexcept>
#include <string>

ArgParser::ArgParser(int argc, char* argv[]) {
    parse_args(argc, argv);
}

const ClientConfig& ArgParser::get_config() const {
    return config;
}

void ArgParser::parse_args(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
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
        } else if (arg == "-t" && i + 1 < argc) {
            if(config.protocol != Protocol::UDP)
            {
                throw std::runtime_error{"You can only have -t in UDP tests\n"};
            }
            config.duration_seconds = std::stoi(argv[++i]);
        } else if (arg == "-b" && i + 1 < argc) {
            if(config.protocol != Protocol::UDP)
            {
                throw std::runtime_error{"You can only have -b in UDP tests\n"};
            }
            config.bytes_to_send = std::stoi(argv[++i]);
        } else if (arg == "-s" && i + 1 < argc) {
            if(config.test_type != TestType::Disk)
            {
                throw std::runtime_error{"You can only have -s in Disk tests\n"};
            }
            config.disk_block_size = std::stoi(argv[++i]);
        } else {
            throw std::runtime_error("Unknown or malformed argument: " + arg);
        }
    }
    if (config.duration_seconds != -1 && config.bytes_to_send != -1) {
    throw std::runtime_error("Cant have -t and -b simultaneously on udp test\n ");
}
}
