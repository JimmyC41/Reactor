#include <cerrno>
#include <iostream>
#include "ConnectionContext.hpp"
#include "Router.hpp"

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
            fprintf(stderr, "recv error: %s (%d)\n", strerror(err), err);

            switch (err)
            {
            case EINTR:         // Interrupted by signal
                continue;   
            case EAGAIN:        // No more data
                break;
            case ECONNRESET:    // Peer reset connection
                m_closedByPeer = true;
                break;
            default:
                throw std::runtime_error("Non trivial receive error");
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
            m_writeBuffer.erase(0, n);
        else if (n == 0)
            break;
        else if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
            break;
        else
            throw std::runtime_error("[INFO] send error\n");
    }
}