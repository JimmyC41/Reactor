#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>
#include <format>
#include <iostream>
#include <spdlog/spdlog.h>
#include "KQueueReactor.hpp"
#include "EventHandler.hpp"
#include "ConnectionContext.hpp"

KQueueReactor::KQueueReactor()
{
    m_kq = kqueue();
    if (m_kq < 0)
        throw std::runtime_error("kqueue() failed");
    m_pendingChanges.reserve(64);
    m_pendingClosures.reserve(64);
}

KQueueReactor::~KQueueReactor()
{
    close(m_kq);
}

void KQueueReactor::setNonBlocking(int fd)
{
    int old_flags = fcntl(fd, F_GETFL, 0);
    if (fcntl(fd, F_GETFL, 0) < 0) return;
    fcntl(fd, F_SETFL, old_flags | O_NONBLOCK);
}

void KQueueReactor::setListener(int listenFd)
{
    m_listenFd = listenFd;
    setNonBlocking(listenFd);
    
    struct kevent kev;
    EV_SET(&kev, listenFd, static_cast<int16_t>(EVFILT_READ), EV_ADD | EV_CLEAR, 0, 0, nullptr);

    if (kevent(m_kq, &kev, 1, nullptr, 0, nullptr) < 0)
    {
        spdlog::error("[fd {}] Failed to add server fd for reads: {} ({})",
            listenFd, strerror(errno), errno);
        return;
    }
    spdlog::info("[fd {}] Server registered for reads", listenFd);
}

void KQueueReactor::addClient(int fd, EventFilter filter, void* udata)
{
    setNonBlocking(fd);

    struct kevent kev;
    EV_SET(&kev, fd, static_cast<int16_t>(filter), EV_ADD | EV_CLEAR, 0, 0, udata);

    if (kevent(m_kq, &kev, 1, nullptr, 0, nullptr) < 0)
    {
        spdlog::error("[fd {}] Failed to register client for {}",
            fd, (filter == EventFilter::Readable) ? "reads" : "writes");

        close(fd);
        return;
    }

    spdlog::info("[fd {}] New connection; registered client",
            fd, (filter == EventFilter::Readable) ? "reads" : "writes");
}

void KQueueReactor::queueEventChange(
    int fd, EventFilter filter,
    int flags, void* udata, bool closeAfter)
{
    struct kevent kev;
    EV_SET(&kev, fd, static_cast<int16_t>(filter), flags, 0, 0, udata);

    m_pendingChanges.push_back(kev);
    if (closeAfter)
        m_pendingClosures.push_back(fd);
    
    spdlog::info("[fd {}] Queued for {}",
        fd, (filter == EventFilter::Readable) ? "reads" : "writes");
}

void KQueueReactor::batchUpdate()
{
    if (!m_pendingChanges.empty()) {
        if (kevent(m_kq, m_pendingChanges.data(), m_pendingChanges.size(), nullptr, 0, nullptr) < 0)
        {
            if (errno != ENOENT)
                spdlog::error("kevent batch update failed: {} ({})",
                    strerror(errno), errno);
            else
                spdlog::info("Ignored ENONET for {} events", m_pendingChanges.size());
        }
        m_pendingChanges.clear();
    }

    for (int fd : m_pendingClosures)
    {
        spdlog::info("[fd {}] Socket closed");
        close(fd);
    }
    m_pendingClosures.clear();
}

void KQueueReactor::run(EventHandler* handler)
{
    struct kevent events[MAX_EVENTS];
    while (true)
    {
        batchUpdate();

        int noEvents = kevent(m_kq, nullptr, 0, events, MAX_EVENTS, nullptr);
        if (noEvents < 0)
        {
            spdlog::error("kevent wait failed: {} ({})", strerror(errno), errno);
            continue;
        }

        spdlog::info("kevent returned {} events", noEvents);

        for (size_t i = 0; i < noEvents; ++i)
        {
            auto& ev = events[i];
            int fd = ev.ident;
            auto* ctx = static_cast<ConnectionContext*>(ev.udata);

            if (ev.flags & EV_EOF)
            {
                spdlog::error("[fd {}] kqueue error: {} ({})",
                    fd, strerror(events[i].data), events[i].data);
                continue;
            }

            if (fd == m_listenFd && ev.filter == EVFILT_READ)
                handler->handleAccept();
            else if (ev.filter == EVFILT_READ)
                handler->handleRead(ctx);
            else if (ev.filter == EVFILT_WRITE)
                handler->handleWrite(ctx);
        }
    }
}