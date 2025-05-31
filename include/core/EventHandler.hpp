#pragma once

class ConnectionContext;

class EventHandler {
public:
    virtual void handleAccept() = 0;
    virtual void handleRead(ConnectionContext* ctx) = 0;
    virtual void handleWrite(ConnectionContext* ctx) = 0;
    virtual ~EventHandler() = default;
};