#pragma once

#include <sys/event.h>
#include <vector>
#include "Event.hpp"

class EventHandler;

/**
 * Manages the kernel event queue
 */

class KQueueReactor
{
public:
    KQueueReactor();
    ~KQueueReactor();

    // Set listening socket to non-blocking and register with kqueue
    void setListener(int listenFd);

    // Wait for events and dispatch to handler
    void run(EventHandler* handler);
    
    // Add a fd to watch for read/writes
    void addClient(int fd, EventFilter filter, void* udata);

    // Remove a fd from watching
    void removeClient(int fd);

private:
    int m_kq;
    int m_listen_fd;

    // Helper to make socket non-blocking
    void setNonBlocking(int fd);

    // Buffer for events
    static const int MAX_EVENTS = 10500;
};