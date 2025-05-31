#pragma once
#include <string>
#include <memory>
#include "TCPStream.hpp"

class TCPListener {
private:
    int     m_fd;
    int     m_port;
    bool    m_listening;

public:
    explicit TCPListener(int port);
    ~TCPListener();

    int start();
    int getFd() const { return m_fd; }

    // On new connection, create a TCPStream object
    std::unique_ptr<TCPStream> accept();
};