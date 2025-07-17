#include "DiskTest.h"
#include "TCPClient.h"
#include <iostream>
#include <fstream>
#include <chrono>

DiskTest::DiskTest(uint64_t block_size, bool read, bool write, uint64_t file_size ,json &stats)
    :_block_size{block_size}, _read_mode{read}, _write_mode{write}, _stats{stats}, _filesize{file_size}
{
    _test_type="DISK_TEST";
    _stats["TEST"]="DISK I/O TEST";
}
void DiskTest::run(TCPClient &client)
{
    if(_read_mode)
    {
        read_test();
    }
    if(_write_mode)
    {
        write_test();
    }
}

void DiskTest::delete_file(std::string filename)
{
    std::remove(filename.c_str());
}
void DiskTest::generate_file(std::string filename)
{  
    std::cout<<"Generating file...\n";
    std::ofstream file{filename, std::ios::out | std::ios::binary};
    uint64_t total_written = 0;
    uint64_t block_size= 16*1024*1024;
    char *buffer = new char[block_size];
    while(total_written <= _filesize)
    {
        file.write(buffer, block_size);
        total_written+=block_size;
    }
    delete buffer;
    std::cout<<"File generated succesfully!\n";
    file.close();
}

std::string DiskTest::get_test_type()
{
    return _test_type;
}
void DiskTest::read_test()
{
    std::cout<<"Starting read test\n";
    if (std::filesystem::exists(file_name)) {

        delete_file(file_name);
    }
    generate_file(file_name);
    std::cout<<"---------------Starting the test...----------------\n";
    std::cout<<"---------------Please wait until the test is done-----------------\n";
    std::ifstream file{file_name, std::ios::in | std::ios::binary};
    uint64_t total_read=0;
    char *buffer = new char[_block_size];
    auto start = std::chrono::steady_clock::now();
    while (true) {
        file.read(buffer, _block_size);
        std::streamsize bytes_read = file.gcount();
        if (total_read >= _filesize) break;
        total_read += bytes_read;
    }
    auto end = std::chrono::steady_clock::now();
    double elapsed= std::chrono::duration<double>(end-start).count();
    delete_file(file_name);
    delete buffer;
    double Mbps, MBps, Gbps, GBps;
    Mbps = (total_read * 8)/(elapsed * 1000*1000);
    MBps =(total_read )/(elapsed * 1024*1024); 
    Gbps =(total_read * 8)/(elapsed * 1000*1000*1000);
    GBps =(total_read )/(elapsed * 1024*1024*1024);
    _stats["READ DISK TEST"]["Mbps"]=Mbps;
    _stats["READ DISK TEST"]["MBps"]=MBps;
    _stats["READ DISK TEST"]["Gbps"]=Gbps;
    _stats["READ DISK TEST"]["GBps"]=GBps;
    _stats["READ DISK TEST"]["elapsed"]=elapsed;
    _stats["READ DISK TEST"]["total_read"]=total_read;
    _stats["READ DISK TEST"]["block_size"]=_block_size;
    //std::cout<<_stats<<"\n"; 
}
void DiskTest::write_test()
{

    std::ofstream file{file_name, std::ios::out | std::ios::binary};
    uint64_t total_written= 0;
    char *buffer= new char[_block_size];
    auto start = std::chrono::steady_clock::now();
    std::cout<<"Write test started.Please wait and do not turn off the application!\n";
    while(true)
    {
        if(total_written >= _filesize)
        {
            std::cout<<"File written succesfully...\nWaiting for the results...\n";
            break;
        }
        file.write(buffer, _block_size);
        total_written+=_block_size;

    }
    auto end = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(end - start).count();
    delete_file(file_name);
    delete buffer;
    double Mbps, MBps, Gbps, GBps;
    Mbps = (total_written * 8)/(elapsed * 1000*1000);
    MBps =(total_written )/(elapsed * 1024*1024); 
    Gbps =(total_written * 8)/(elapsed * 1000*1000*1000);
    GBps =(total_written )/(elapsed * 1024*1024*1024);
    _stats["WRITE DISK TEST"]["Mbps"]=Mbps;
    _stats["WRITE DISK TEST"]["MBps"]=MBps;
    _stats["WRITE DISK TEST"]["Gbps"]=Gbps;
    _stats["WRITE DISK TEST"]["GBps"]=GBps;
    _stats["WRITE DISK TEST"]["elapsed"]=elapsed;
    _stats["WRITE DISK TEST"]["total_written"]=total_written;
    _stats["WRITE DISK TEST"]["block_size"]=_block_size;
    //std::cout<<_stats;
}