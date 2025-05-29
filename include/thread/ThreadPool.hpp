#pragma once

#include <thread>
#include <vector>
#include <tbb/concurrent_queue.h>

class ThreadPool
{
public:
    ThreadPool(int numThreads);
    ~ThreadPool();
    void submitTask(std::function<void()> f);

private:
    tbb::concurrent_queue<std::function<void()>> m_tasks;
    std::vector<std::thread>                    m_workers;
    std::atomic<bool>                           m_done;
};