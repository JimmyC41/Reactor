#pragma once
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <cerrno>
#include "TCPStream.hpp"


/**
 * Manages the server side socket to start listening for inbound connections.
 */

class TCPListener
{
private:
    int         m_fd;       // File descriptor for the listening socket
    int         m_port;     // TCP port number to listen on
    bool        m_listening;

public:
    TCPListener(int port);
    ~TCPListener();

    int start();            // Primes a socket to listen for new TCP connections
    TCPStream* accept();    // Returns a TCPStream for a client

private:
    TCPListener() {}
};

/**
 * listener.start()
 * 
 * while loop:
 *      TCPStream* client = listener.accept()
 *      if (client != nullptr):
 *          do_work(client)
 */