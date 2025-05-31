#pragma once
#include <vector>
#include <sys/event.h>
#include "Event.hpp"

class EventHandler;

using kevVector = std::vector<struct kevent>;

class KQueueReactor
{
private:
    int                 m_kq;
    int                 m_listenFd;
    kevVector           m_pendingChanges;
    std::vector<int>    m_pendingClosures;

    static constexpr int MAX_EVENTS = 4096;
    
    void setNonBlocking(int fd);
    void batchUpdate();

public:
    KQueueReactor();
    ~KQueueReactor();

    void setListener(int listenFd);
    void run(EventHandler* handler);
    void addClient(int fd, EventFilter filter, void* udata);
    int listenFd() const { return m_listenFd; }

    // closeAfter is a flag to close the fd after applying the event change
    void queueEventChange(
        int fd, EventFilter filter, int flags,
        void* udata, bool closeAfter = false
    );
};