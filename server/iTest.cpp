#include "iTest.h"
#include "TCPDownload.h"




iTest * TestFactory::makeTcpDownloadTest(int duration, json &stats)
{
    return new TCPDownload{duration, stats};
}


