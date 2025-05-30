#include <arpa/inet.h>
#include <sys/socket.h>
#include <cerrno>
#include "TCPStream.hpp"

TCPStream::TCPStream(int fd, struct sockaddr_in* address)
    : m_fd(fd)
{
    char ip[50];

    // Convert socket info to an IP string and port
    inet_ntop(
        AF_INET,
        &address->sin_addr,
        ip,
        sizeof(ip)
    );

    // MSG_NOSIGNAL to suppress SIGPIPE, a fatal error when a socket
    // tries to write when the peer already closed its end
    // int on = 1;
    // setsockopt(m_fd, SOL_SOCKET, SO_NOSIGPIPE, &on, sizeof(on));

    m_peerIP = ip;
    m_peerPort = ntohs(address->sin_port);
}

TCPStream::~TCPStream()
{
    close(m_fd);
}

ssize_t TCPStream::send(char* buffer, std::size_t len)
{
    // Kernel looks up the fd's {clientIP, clientPort, ...} tuple to send
    ssize_t bytesSent;
    do { bytesSent = ::write(m_fd, buffer, len); }
    while (bytesSent < 0 && errno == EINTR);
    return bytesSent;
}

ssize_t TCPStream::receive(char* buffer, std::size_t len)
{
    ssize_t bytesRead;
    do { bytesRead = ::read(m_fd, buffer, len); }
    while (bytesRead < 0 && errno == EINTR);
    return bytesRead;
}