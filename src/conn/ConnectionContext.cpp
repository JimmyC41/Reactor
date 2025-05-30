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
        {
            m_readBuffer.append(buffer, n);
            continue;
        }
        if (n == 0 || errno == EPIPE || errno == ECONNRESET || errno == EBADF)
        {
            m_closedByPeer = true;
            break;
        }
        if (errno == EINTR) continue;
        if (errno == EWOULDBLOCK) break;

        throw std::runtime_error(
            "[INFO] receive error: " + std::string(strerror(errno)) + " (" + std::to_string(errno) + ")");
    }

    // prepareDummyResponse();

    if (!m_readBuffer.empty() && m_connState == ConnState::Reading)
    {
        Request request;
        Response response = request.parse(m_readBuffer) ?
            Router::processRequest(request) : Router::createBadRequest();
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
        if (n == 0 || errno == EPIPE || errno == ECONNRESET || errno == EBADF) {
            m_closedByPeer = true;
            return;
        }
        if (errno == EINTR) continue;
        if (errno == EWOULDBLOCK) return;
        throw std::runtime_error(
            "[INFO] send error: " + std::string(strerror(errno)) + " (" + std::to_string(errno) + ")");
    }
}