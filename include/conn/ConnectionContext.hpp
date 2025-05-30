#pragma once
#include "TCPStream.hpp"

enum class ConnState{ Reading, Writing };

/**
 * Encapsulates I/O for an active client connection
 * Associated with a single client fd, managed by the server
 */
class ConnectionContext
{
private:
    TCPStream   m_stream;
    std::string m_readBuffer;
    std::string m_writeBuffer;
    ConnState   m_connState;
    bool        m_closedByPeer;
    static const int  TCP_SEGMENT_SIZE{4096};

public:
    explicit ConnectionContext(TCPStream* stream)
        : m_stream(std::move(*stream))
        , m_connState(ConnState::Reading)
        , m_closedByPeer(false) {}

    // fd is readable: read request and prepare response to buffer
    void onReadable();

    // fd is writable: flush write buffer
    void onWritable();

    bool writeReady() const {
        return m_connState == ConnState::Writing 
            && !m_writeBuffer.empty();
    }

    bool isClosedByPeer() const { return m_closedByPeer; }

    void setConnState(ConnState state) { m_connState = state; }

    int getStreamFd() const { return m_stream.getFd(); }

    // For testing
    void prepareDummyResponse()
    {
        m_writeBuffer = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Length: 4\r\n"
            "Connection: close\r\n"
            "\r\n"
            "ACK\n";
        m_connState = ConnState::Writing;
    }
};