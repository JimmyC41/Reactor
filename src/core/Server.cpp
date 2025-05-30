#include <iostream>
#include <memory>
#include <format>
#include "Server.hpp"
#include "Event.hpp"
#include "ConnectionContext.hpp"

void Server::handleAccept()
{
    // std::cout << std::format(
        // "[INFO] kevent: server socket {} is readable\n", m_listener.getFd());

    // accept() on the listening fd, returns client fd
    auto clientStreamPtr = m_listener.accept();
    if (!clientStreamPtr)
        return;

    // Create a ConnectionContext for the new client
    auto context = std::make_unique<ConnectionContext>(clientStreamPtr);
    auto clientFd = clientStreamPtr->getFd();
    
    // Register the client (fd + context) with the kqueue reactor
    m_reactor.addClient(clientFd, Readable, context.get());

    // Track client fd and connection context for subsequent read/writes
    m_clientMap[clientFd] = std::move(context);
}

void Server::handleRead(ConnectionContext* ctx)
{
    int clientFd = ctx->getStreamFd();

    // std::cout << std::format(
        // "[INFO] kevent: client socket {} is readable\n", clientFd);
    
    // Receive in read buffer, prepare response in write buffer
    ctx->onReadable();

    // Register the client fd for writes
    if (ctx->writeReady())
        m_reactor.addClient(clientFd, EventFilter::Writable, ctx);

    /*
    
    // Register the client for write if connection context has received and prepared write buffer
    if (ctx->writeReady())
    {
        m_reactor.addClient(clientFd, EventFilter::Writable, ctx);
    }

    // Register the client for further reads if not finished reading
    else if (!ctx->isClosedByPeer())
    {   
        m_reactor.addClient(clientFd, EventFilter::Readable, ctx);
    }

    // Terminate the connection if closed by peer
    else
    {
        m_reactor.removeClient(clientFd);
        m_clientMap.erase(clientFd);
        close(clientFd);
    }

    */
}

void Server::handleWrite(ConnectionContext* ctx)
{
    int clientFd = ctx->getStreamFd();

    // std::cout << std::format(
        // "[INFO] kevent: client socket {} is writable\n", clientFd);
    
    // Flush write buffer
    ctx->onWritable();

     // On writing finish or peer close, clean up
    if (!ctx->writeReady() || ctx->isClosedByPeer())
    {
        m_reactor.removeClient(clientFd);
        close(clientFd);
        m_clientMap.erase(clientFd);
        
        // std::cout << std::format(
            // "[INFO] kevent: closed client socket {}\n", clientFd);
    }
    
    /*
    // Register the client for readfs if writes have flushed (recall connection is 'keep-alive')
    else if (ctx->isClosedByPeer())
    {
        ctx->setConnState(ConnState::Reading);
        m_reactor.addClient(clientFd, EventFilter::Readable, ctx);
    }

    // Terminate the connection if closed by peer
    else
    {
        m_reactor.removeClient(clientFd);
        m_clientMap.erase(clientFd);
        close(clientFd);
    }
    */
}