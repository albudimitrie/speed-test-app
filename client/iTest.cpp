#include "iTest.h"
#include "TCPDownload.h"
iTest *TestFactory::makeTCPDownloadTest(int duration)
{
    return new TCPDownload{duration};
}