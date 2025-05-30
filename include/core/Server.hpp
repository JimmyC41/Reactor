#pragma once
#include <unordered_map>
#include "EventHandler.hpp"
#include "TCPStream.hpp"
#include "TCPListener.hpp"
#include "KQueueReactor.hpp"
#include "ConnectionContext.hpp"
#include "ThreadPool.hpp"

using ClientMap = std::unordered_map<int, std::unique_ptr<ConnectionContext>>;

class Server : public EventHandler
{
private:
    TCPListener     m_listener;
    KQueueReactor   m_reactor;
    ClientMap       m_clientMap; // Maps client fd to the respective connection context
    ThreadPool      m_pool;

public:
    Server(int port) : m_listener(port)
    {
        // Creates a listening server socket
        m_listener.start();

        // Make server socket non-blocking and register to kqueue for new reads
        m_reactor.setListener(m_listener.getFd());
    }

    void run() { m_reactor.run(this); }
    void handleAccept() override;
    void handleRead(ConnectionContext* clientFd) override;
    void handleWrite(ConnectionContext* clientFd) override;
};