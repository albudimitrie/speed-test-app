#include "iTest.h"
#include "TCPDownload.h"
#include "TCPUpload.h"
#include "TCPLatency.h"
#include "UDPDownload.h"
#include "UDPLatency.h"
#include "UDPUpload.h"

iTest * TestFactory::makeTcpDownloadTest(int duration, json &stats)
{
    return new TCPDownload{duration, stats};
}

iTest* TestFactory::makeTcpUploadTest(int duration, uint64_t bytes_to_send, json &stats, std::string client_addr)
{
    return new TCPUpload{stats, duration, bytes_to_send, client_addr};
}
iTest *TestFactory::makeTCPLatencyTest(json &stats)
{
    return new TCPLatency{stats};
}

iTest *TestFactory::makeUDPDownloadTest(int duration, uint64_t bytes_to_send, json &stats, std::string client_addr, uint64_t bitrate)
{
    return new UDPDownload{duration, bytes_to_send, stats, client_addr, bitrate};

}

iTest *TestFactory::makeUDPLatencyTest(json &stats, std::string client_address)
{
    return new UDPLatency{stats, client_address};
}

iTest * TestFactory::makeUDPUploadTest(int duration, uint64_t bytes_to_send, json &stats, std::string client_addr, uint64_t bitrate)
{
    return new UDPUpload{duration, bytes_to_send, stats, client_addr, bitrate};
}