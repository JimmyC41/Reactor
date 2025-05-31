#include <cerrno>
#include <iostream>
#include <format>
#include <iomanip>
#include <spdlog/spdlog.h>
#include "ConnectionContext.hpp"
#include "Router.hpp"

// TODO: Refactor long method

void ConnectionContext::onReadable() {
    if (m_stream.getFd() < 0)
        return;

    char buffer[TCP_SEGMENT_SIZE];
    while (true)
    {
        ssize_t n = m_stream.receive(buffer, sizeof(buffer));
        if (n > 0)
        {
            m_readBuffer.insert(m_readBuffer.end(), buffer, buffer + n);
            spdlog::info("[fd {}] Read {} bytes, total buffer: {}",
                m_stream.getFd(), n, m_readBuffer.size());
            continue;
        }

        if (errno == EWOULDBLOCK) break;    // No data to read, return to event loop
        if (errno == EINTR) continue;       // Read syscall interrupted by a signal, try again

        spdlog::error("[fd {}] Read receive error: {} ({})",
            m_stream.getFd(), strerror(errno), errno);
        m_closedByPeer = true;
        // logBuffer(true);
        break;
    }

    // To handle pipeline edge case - while waiting to write, the client sends additional data
    if (m_connState != ConnState::Reading)
        return;
    
    if (m_closedByPeer)
    {
        spdlog::error("[fd {}] Peer closed, aborting write", m_stream.getFd());
        return;
    }

    while (m_bufferOffset < m_readBuffer.size())
    {
        std::string_view data(m_readBuffer.data() + m_bufferOffset,
            m_readBuffer.size() - m_bufferOffset);

        Request request;
        if (!request.parse(data))
        {
            // logBuffer(true);
            spdlog::info("[fd {}] Parse failed, waiting for more data", m_stream.getFd());
            break;
        }

        // Write to buffer
        Response response = Router::processRequest(request);
        auto responseStr = response.renderString();
        m_writeBuffer.insert(m_writeBuffer.end(), responseStr.begin(), responseStr.end());
        m_connState = ConnState::Writing;

        // Advance m_bufferOffset
        auto headerEnd = data.find("\r\n\r\n") + 4;
        m_bufferOffset += headerEnd + request.body().size();
        if (m_bufferOffset >= m_readBuffer.size())
        {
            m_readBuffer.clear();
            m_bufferOffset = 0;
            break;
        }
    }
}

void ConnectionContext::onWritable() {
    if (m_writeBuffer.empty())
        return;

    // logBuffer(false);

    while (!m_writeBuffer.empty())
    {
        ssize_t n = m_stream.send(m_writeBuffer.data(), m_writeBuffer.size());
        
        if (n > 0)
        {
            spdlog::info("[fd {}] Sent {} bytes, remaining {}",
                m_stream.getFd(), n, m_writeBuffer.size() - n);
            m_writeBuffer.erase(m_writeBuffer.begin(), m_writeBuffer.begin() + n);
            continue;
        }

        if (errno == EINTR) continue;
        if (errno == EWOULDBLOCK) return;

        spdlog::error("[fd {}] Send error: {} ({})",
            m_stream.getFd(), strerror(errno), errno);
        m_closedByPeer = true;
        return;
    }
}

void ConnectionContext::logBuffer(bool read) const
{
    std::vector<char> buffer = read ? m_readBuffer : m_writeBuffer;
    if (!buffer.empty())
    {
        std::ostringstream oss;
        oss << "[fd " << m_stream.getFd() << "] "
            << "Raw " << (read ? "read" : "write")
            << " buffer (" << buffer.size() << " bytes): "
            << "for client " << m_stream.getFd();

        for (char c : buffer)
        {
            if (std::isprint(static_cast<unsigned char>(c)))
                oss << c;
            else
                oss << "\\x" << std::hex << std::setw(2) << std::setfill('0') << (unsigned char)c;
        }
        spdlog::info("{}", oss.str());
    }
}
