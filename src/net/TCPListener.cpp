#include <iostream>
#include "TCPListener.hpp"

TCPListener::TCPListener(int port)
    : m_fd(0), m_port(port), m_listening(false) {}

TCPListener::~TCPListener()
{
    if (m_fd >= 0)
        close(m_fd);
}

int TCPListener::start()
{
    if (m_listening == true)
        return 0;
    
    m_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (m_fd < 0)
    {
        perror("socket() failed");
        return -1;
    }

    // Allow immediate re-binding of the port on kill
    int opt = 1;
    setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Allow multiple listeners on the same port (required for server cluster to run)
    if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt(SO_REUSEPORT) failed");
        close(m_fd);
        return -1;
    }

    // Bind the socket to all network interfaces
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(m_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("bind() failed");
        close(m_fd);
        return -1;
    }

    if (listen(m_fd, SOMAXCONN) < 0)
    {
        perror("listen() failed");
        close(m_fd);
        return -1;
    }

    m_listening = true;
    std::cout << "[INFO] Server running at: " << inet_ntoa(addr.sin_addr) << ":"
            << ntohs(addr.sin_port) << '\n';

    return 0;
}

TCPStream* TCPListener::accept()
{
    if (!m_listening)
        return nullptr;
    
    struct sockaddr_in peerAddr;
    socklen_t len = sizeof(peerAddr);

    // On the listening socket identified by m_fd, accept the next pending connection
    // clientFd is the client's fd and is paired to the client IP and port
    int clientFd = ::accept(m_fd, (struct sockaddr*)& peerAddr, &len);
    if (clientFd < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return nullptr;
        
        perror("accept() failed");
        return nullptr;
    }

    return new TCPStream(clientFd, &peerAddr);
}