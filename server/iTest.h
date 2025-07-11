#pragma once
#include <iostream>
#include "nlohmann/json.hpp"
#define BUFFER_SIZE 1024*32
using json=nlohmann::json;

class iTest
{
public:
    virtual ~iTest() = default;
    virtual void run(int client_socket) = 0;

};

class TestFactory
{
public:
    static iTest * makeTcpDownloadTest(int duration, json &stats);
};


