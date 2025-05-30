#include <cerrno>
#include <iostream>
#include "ConnectionContext.hpp"
#include "Router.hpp"

// TODO: Refactor, methods are super super uggggggly

void ConnectionContext::onReadable()
{
    if (m_stream.getFd() < 0)
        return;

    char buffer[TCP_SEGMENT_SIZE];

    while (true)
    {
        ssize_t n = m_stream.receive(buffer, sizeof(buffer));

        if (n > 0)
            m_readBuffer.append(buffer, n);
        else if (n == 0)
        {
            m_closedByPeer = true;
            break;
        }
        else
        {
            const int err = errno;
            switch (err)
            {
            case EINTR:         // Interrupted by signal
                continue;   
            case EWOULDBLOCK:   // No more room in socket buffer
                break;
            case EBADF:
            case EPIPE:         // Peer closed connection
            case ECONNRESET:    // Peer reset connection
                m_closedByPeer = true;
                break;
            default:
                throw std::runtime_error(
                        std::string("[INFO] receive error: ") +
                        strerror(err) + " (" + std::to_string(err) + ")");
            }
            
            break;
        }
    }

    if (!m_readBuffer.empty() && m_connState == ConnState::Reading)
    {
        Request request;
        Response response;

        if (!request.parse(m_readBuffer))
            response = Router::createBadRequest();
        else
            response = Router::processRequest(request);
        
        m_writeBuffer = response.renderString();
        m_connState = ConnState::Writing;
    }
}

void ConnectionContext::onWritable()
{
    while (!m_writeBuffer.empty())
    {
        ssize_t n = m_stream.send(m_writeBuffer.data(), m_writeBuffer.size());

        if (n > 0)
        {
            m_writeBuffer.erase(0, n);
            continue;
        }
        else if (n == 0)
            return;

        const int err = errno;

        switch (err)
        {
        case EINTR:
            continue;
        case EWOULDBLOCK:
            return;
        case EPIPE:
        case ECONNRESET:
        case EBADF:
            m_closedByPeer = true;
            return;

        default:
            throw std::runtime_error(
                std::string("[INFO] send error: ") +
                strerror(err) + " (" + std::to_string(err) + ")");
        }
    }
}