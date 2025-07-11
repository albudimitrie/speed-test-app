#pragma once
#include "iTest.h"
#include "chrono"
#include "nlohmann/json.hpp"
using json=nlohmann::json;
class TCPDownload : public iTest
{
    int _duration;
    json &_stats;
public:
    TCPDownload(int duration, json &stats);
    virtual void run(int client_socket) override;
    virtual ~TCPDownload()=default;

};