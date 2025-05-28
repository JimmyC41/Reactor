#pragma once

class EventHandler
{
public:
    virtual ~EventHandler() = default;

    // Accept client connection, create a connection context, register with kqueue
    virtual void handleAccept() = 0;
    virtual void handleRead(int fd) = 0;
    virtual void handleWrite(int fd) = 0;
};