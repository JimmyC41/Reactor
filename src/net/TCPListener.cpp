#include <iostream>
#include <format>
#include <cstring>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <spdlog/spdlog.h>
#include "TCPListener.hpp"


TCPListener::TCPListener(int port)
    : m_fd(0)
    , m_port(port)
    , m_listening(false) {}

TCPListener::~TCPListener()
{
    if (m_fd >= 0)
        close(m_fd);
}

int TCPListener::start() {
    if (m_listening)
        return 0;

    m_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (m_fd < 0)
    {
        spdlog::error("Server socket() failed: {} ({})", strerror(errno), errno);
        return -1;
    }

    int opt = 1;
    
    // Allow socket to bind to address in TIME_WAIT
    setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Allow multiple sockets to bind to the same port (load balancing)
    setsockopt(m_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    // Prevent fatal error when writing to a closed socket, EPIPE errno returned instead
    setsockopt(m_fd, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt));

    // Disable batching of small packets for latency
    setsockopt(m_fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));

    // Set up an IPv4 socket address
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(m_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        spdlog::error("Server close() failed: {} ({})", strerror(errno), errno);
        close(m_fd);
        return -1;
    }

    if (listen(m_fd, SOMAXCONN) < 0)
    {
        spdlog::error("Server listen() failed: {} ({})", strerror(errno), errno);
        close(m_fd);
        return -1;
    }

    m_listening = true;
    return 0;
}

std::unique_ptr<TCPStream> TCPListener::accept() {
    if (!m_listening)
    {
        spdlog::error("Server not listening, but accept called");
        return nullptr;
    }

    struct sockaddr_in peerAddr;
    socklen_t len = sizeof(peerAddr);

    int clientFd;
    while ((clientFd = ::accept(m_fd, (struct sockaddr*)&peerAddr, &len)) < 0 && errno == EINTR);

    if (clientFd < 0)
        return nullptr;
    return std::make_unique<TCPStream>(clientFd, &peerAddr);
}