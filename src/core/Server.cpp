#include <iostream>
#include <memory>
#include <format>
#include <unistd.h>
#include "Server.hpp"
#include "Logger.hpp"

Server::Server(int port) : m_listener(port)
{
    Logger::initialize("logs/server.log", m_logFileSize, m_logFileCount);
    m_listener.start();
    m_reactor.setListener(m_listener.getFd());
    spdlog::info("Server listening on port {}", port);
}

void Server::handleAccept() {
    if (m_clientMap.size() >= m_maxConnections)
    {
        spdlog::error("Max connections reached, rejecting");
        return;
    }

    auto clientStreamPtr = std::unique_ptr<TCPStream>(m_listener.accept());
    if (!clientStreamPtr)
        throw std::runtime_error("TCPStream::accept() returned nullptr");

    auto clientFd = clientStreamPtr->getFd();
    try
    {
        auto context = std::make_unique<ConnectionContext>(clientStreamPtr.get());
        m_reactor.addClient(clientFd, EventFilter::Readable, context.get());
        m_clientMap[clientFd] = std::move(context);
    } catch (const std::exception& e)
    {
        spdlog::error("[fd {}] Failed to register client fd: {}", clientFd, e.what());
    }
}

void Server::handleRead(ConnectionContext* ctx) {
    int clientFd = ctx->getStreamFd();

    // Receive raw bytes, process, write response to write buffer
    ctx->onReadable();
    
    // Queue the client fd to be writable
    if (ctx->writeReady())
        m_reactor.queueEventChange(clientFd, EventFilter::Writable, EV_ADD | EV_CLEAR, ctx);

    // OR clean up the connection if client has closed
    // when n == 0 (graceful close) or n < 0 (read error)
    else if (ctx->isClosedByPeer())
        unsubscribeClient(clientFd);
}

void Server::handleWrite(ConnectionContext* ctx) {
    int clientFd = ctx->getStreamFd();

    // Write to client
    ctx->onWritable();

    // Clean up connection if write was successful
    if (ctx->isClosedByPeer() || !ctx->writeReady())
        unsubscribeClient(clientFd);
}

void Server::unsubscribeClient(int clientFd)
{
    m_reactor.queueEventChange(clientFd, EventFilter::Readable, EV_DELETE, nullptr, true);
    m_reactor.queueEventChange(clientFd, EventFilter::Writable, EV_DELETE, nullptr, false);
    m_clientMap.erase(clientFd);
    spdlog::info("[fd {}] Queued client to be unsubscribed", clientFd);
}