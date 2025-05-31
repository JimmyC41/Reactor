#pragma once
#include <memory>
#include <unordered_map>
#include "EventHandler.hpp"
#include "TCPStream.hpp"
#include "TCPListener.hpp"
#include "KQueueReactor.hpp"
#include "ConnectionContext.hpp"

using ClientMap = std::unordered_map<int, std::unique_ptr<ConnectionContext>>;

class Server : public EventHandler
{
private:
    TCPListener         m_listener;
    KQueueReactor       m_reactor;
    ClientMap           m_clientMap;

    static constexpr size_t m_maxConnections = 10000;
    static constexpr size_t m_logFileSize = 100 * 1024 * 1024;
    static constexpr size_t m_logFileCount = 10;

    void unsubscribeClient(int clientFd);

public:
    explicit Server(int port);
    void run() { m_reactor.run(this); }
    void handleAccept() override;
    void handleRead(ConnectionContext* ctx) override;
    void handleWrite(ConnectionContext* ctx) override;
};