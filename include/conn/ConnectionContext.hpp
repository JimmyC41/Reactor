#pragma once
#include <vector>
#include "TCPStream.hpp"

enum class ConnState { Reading, Writing };

/**
 * State for a single client connection
 */

class ConnectionContext
{
private:
    TCPStream           m_stream;           // To send and receive raw bytes
    std::vector<char>   m_readBuffer;
    std::vector<char>   m_writeBuffer;
    ConnState           m_connState;        // Set to Reading or Writing
    bool                m_closedByPeer;     // Flag to signal client has closed or disconnected
    size_t              m_bufferOffset;     // Pointer to what's been read

    static constexpr int TCP_SEGMENT_SIZE = 4096;
    static constexpr int WRITE_BUFFER_SIZE = 1024;

    void logBuffer(bool read) const;        // Log buffer for debug

public:
    explicit ConnectionContext(TCPStream* stream)
        : m_stream(std::move(*stream))
        , m_connState(ConnState::Reading)
        , m_closedByPeer(false)
        , m_bufferOffset(0)
    {
        m_readBuffer.reserve(TCP_SEGMENT_SIZE);
        m_writeBuffer.reserve(WRITE_BUFFER_SIZE);
    }

    void onReadable();  // Read raw bytes, process request, populate write buffer
    void onWritable();

    void setConnState(ConnState state) { m_connState = state; }
    bool isClosedByPeer() const { return m_closedByPeer; }
    int getStreamFd() const { return m_stream.getFd(); }
    bool writeReady() const { return !m_writeBuffer.empty()
            && m_connState == ConnState::Writing; }
};