#include "iTest.h"
#include "TCPDownload.h"
#include "TCPUpload.h"
#include "TCPLatency.h"
#include "UDPDownload.h"
#include "DiskTest.h"
#include "UDPLatency.h"
#include "UDPUpload.h"

iTest *TestFactory::makeTCPDownloadTest(int duration)
{
    return new TCPDownload{duration};
}

iTest *TestFactory::makeTCPUploadSizeTest(uint64_t bytes_to_send)
{
    return new TCPUploadSize{bytes_to_send};

}
iTest *TestFactory::makeTCPUploadTimeTest(int duration)
{
    return new TCPUploadTime{duration};
}
iTest *TestFactory::makeTCPLatencyTest()
{
    return new TCPLatency{};
}

iTest *TestFactory::makeUDPDownloadTest(int duration, uint64_t bytes_to_send, uint64_t bitrate_bps)
{
    return new UDPDownload{duration, bytes_to_send, bitrate_bps};
}
iTest *TestFactory::makeDiskTest(uint64_t block_size, bool read, bool write,uint64_t file_size, json &stats)
{
    return new DiskTest{block_size, read,write,file_size,stats};
}
iTest *TestFactory::makeUDPLatencytest()
{
    return new UDPLatency();
}
iTest * TestFactory::makeUDPUploadtest(int duration, uint64_t bytes_to_send, uint64_t bitrate_bps)
{
    return new UDPUpload{duration, bytes_to_send,bitrate_bps}; 
}