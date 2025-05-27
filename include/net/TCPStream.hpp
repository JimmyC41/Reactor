#pragma once
#include <string>
#include <unistd.h>
#include <netinet/in.h>

/**
 * Single TCP conection.
 * Provides methods to send and receive data over a TCP/IP connection.
 */

class TCPStream
{
private:
    int         m_fd;
    std::string m_peerIP;
    int         m_peerPort;

public:
    friend class TCPListener;
    TCPStream(int fd, struct sockaddr_in* address);
    ~TCPStream();

    ssize_t send(char* buffer, std::size_t len);
    ssize_t receive(char* buffer, std::size_t len);

    std::string getPeerIP() { return m_peerIP; }
    int getPeerPort() { return m_peerPort; }

private:
    TCPStream();
    TCPStream(const TCPStream& stream);
};