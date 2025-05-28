#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>
#include <memory>
#include "Server.hpp"

int main()
{
    constexpr int PORT = 8080;
    
    try
    {
        Server server(PORT);
        server.run();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "[ERROR] " << ex.what() << "\n";
        return 1;
    }

    return 0;
}