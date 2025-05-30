#pragma once

class ConnectionContext;

class EventHandler
{
public:
    virtual ~EventHandler() = default;

    // Accept client connection, create a connection context, register with kqueue
    virtual void handleAccept() = 0;
    virtual void handleRead(ConnectionContext* ctx) = 0;
    virtual void handleWrite(ConnectionContext* ctx) = 0;
};