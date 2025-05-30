#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>
#include <memory>
#include "ServerCluster.hpp"

int main()
{
    constexpr int PORT = 8080;
    try
    {
        // ServerCluster cluster(PORT);
        // cluster.run();
        
        Server httpServer(PORT);
        httpServer.run();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "[ERROR] " << ex.what() << "\n";
        return 1;
    }
    return 0;
}