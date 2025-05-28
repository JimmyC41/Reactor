#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>
#include <iostream>
#include "KQueueReactor.hpp"
#include "EventHandler.hpp"

KQueueReactor::KQueueReactor()
{
    m_kq = kqueue();
    if (m_kq < 0)
        throw std::runtime_error(std::string("kqueue() failed: ") + strerror(errno));
}

KQueueReactor::~KQueueReactor()
{
    close(m_kq);
}

// Set the fd to not block on read() and write()
void KQueueReactor::setNonBlocking(int fd)
{
    // fcntl is a sys call to change properties of an open fd
    int old_flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, old_flags | O_NONBLOCK);
}

void KQueueReactor::setListener(int listenFd)
{
    m_listen_fd = listenFd;
    setNonBlocking(listenFd);

    // Register socket for read events and clear on each delivery
    struct kevent kev;
    EV_SET(&kev, listenFd, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, nullptr);

    if (kevent(m_kq, &kev, 1, nullptr, 0, nullptr) < 0)
        throw std::runtime_error("kevent add listener failed");
}

void KQueueReactor::addClient(int fd, EventFilter filter, void* udata)
{
    setNonBlocking(fd);

    // Register the client fd for read or write
    struct kevent kev;
    EV_SET(&kev, fd, filter, EV_ADD | EV_CLEAR, 0, 0, udata);

    if (kevent(m_kq, &kev, 1, nullptr, 0, nullptr) < 0)
        throw std::runtime_error("kevent add fd failed");
}

void KQueueReactor::removeClient(int fd)
{
    struct kevent kevs[2];
    
    // Un-register read and write interest for given fd
    EV_SET(&kevs[0], fd, EVFILT_READ,  EV_DELETE, 0, 0, nullptr);
    EV_SET(&kevs[1], fd, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);
    kevent(m_kq, kevs, 2, nullptr, 0, nullptr);
}

void KQueueReactor::run(EventHandler* handler)
{
    struct kevent events[MAX_EVENTS];
    while (true)
    {
        // Blocks until there is an event 'ready'
        int noEvents = kevent(m_kq, nullptr, 0, events, MAX_EVENTS, nullptr);
        
        for (size_t i = 0; i < noEvents; ++i)
        {
            auto &ev = events[i];
            int fd = ev.ident;
            int filter = ev.filter;

            if (fd == m_listen_fd && filter == EVFILT_READ)
                handler->handleAccept();    // New client
            else if (filter == EVFILT_READ)
                handler->handleRead(fd);    // Client socket is readable
            else if (filter == EVFILT_WRITE)
                handler->handleWrite(fd);   // Client socket is writable
        }
    }
}