#pragma once
#include <string>
#include <netinet/in.h>

class TCPStream
{
private:
    int         m_fd;
    std::string m_peerIP;
    int         m_peerPort;

public:
    TCPStream(int fd, struct sockaddr_in* peerAddr);
    ~TCPStream();

    ssize_t send(const char* buffer, size_t len);
    ssize_t receive(char* buffer, size_t len);

    int getFd() const { return m_fd; }

    // TCPStream moved into std::unique_ptr in Server::handleAccept
    TCPStream(TCPStream&& other) noexcept;
    TCPStream& operator=(TCPStream&& other) noexcept;

    // Move only object
    TCPStream(const TCPStream&) = delete;
    TCPStream& operator=(const TCPStream&) = delete;
};