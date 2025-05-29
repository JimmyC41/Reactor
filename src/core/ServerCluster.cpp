#include <iostream>
#include <format>
#include "ServerCluster.hpp"

ServerCluster::ServerCluster(int port, int instances)
    : m_port(port), m_instances(instances > 0 ? instances : 1) {}

ServerCluster::~ServerCluster()
{
    for (auto& server : m_threads)
        if (server.joinable())
            server.join();
}

void ServerCluster::run()
{
    std::cout << std::format(
        "Starting {} server instances on port {}\n", 
        m_instances, m_port
    );

    m_threads.reserve(m_instances);
    
    for (std::size_t i{0}; i < m_instances; ++i)
    {
        m_threads.emplace_back([this]
            {
                Server instance(m_port);
                instance.run();
            }
        );
    }
}