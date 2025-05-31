#include <cerrno>
#include <iostream>
#include <format>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <spdlog/spdlog.h>
#include "TCPStream.hpp"

TCPStream::TCPStream(int fd, struct sockaddr_in* peerAddr)
    : m_fd(fd)
{
    int opt = 1;

    // // Prevent fatal error when writing to a closed socket, EPIPE errno returned instead
    setsockopt(m_fd, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt));

    m_peerIP = inet_ntoa(peerAddr->sin_addr);
    m_peerPort = ntohs(peerAddr->sin_port);
}

TCPStream::~TCPStream()
{
    if (m_fd >= 0)
        close(m_fd);
}

ssize_t TCPStream::send(const char* buffer, size_t len)
{
    return ::send(m_fd, buffer, len, 0);
}

ssize_t TCPStream::receive(char* buffer, size_t len)
{
    return ::recv(m_fd, buffer, len, 0);
}

TCPStream::TCPStream(TCPStream&& other) noexcept
    : m_fd(other.m_fd)
    , m_peerIP(std::move(other.m_peerIP))
    , m_peerPort(other.m_peerPort)
{
    other.m_fd = -1;
}

TCPStream& TCPStream::operator=(TCPStream&& other) noexcept
{
    if (this != &other)
    {
        if (m_fd >= 0) close(m_fd);
        m_fd = other.m_fd;
        m_peerIP = std::move(other.m_peerIP);
        m_peerPort = other.m_peerPort;
        other.m_fd = -1;
    }
    return *this;
}