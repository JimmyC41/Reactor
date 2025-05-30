#pragma once

#include <vector>
#include <thread>
#include "Server.hpp"

class ServerCluster
{
private:
    int m_port;
    int m_instances;
    std::vector<std::thread> m_threads;

public:
    ServerCluster(int port,
        int instances = std::thread::hardware_concurrency());    
    ~ServerCluster();
    
    void run();
};