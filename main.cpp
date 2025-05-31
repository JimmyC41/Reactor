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
        Server httpServer(PORT);
        httpServer.run();
    }
    catch (const std::exception& ex)
    {
        std::cerr << std::format("[ERROR] {}\n", ex.what());
        return 1;
    }
    return 0;
}