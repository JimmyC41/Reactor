#pragma once

#include <sys/event.h>

enum EventFilter
{
    Readable = EVFILT_READ,
    Writable = EVFILT_WRITE
};

struct Event
{
    int fd;
    int filter;
};
