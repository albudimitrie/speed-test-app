#include "iTest.h"
#include "TCPDownload.h"
iTest *TestFactory::makeTCPDownloadTest()
{
    return new TCPDownload{};
}