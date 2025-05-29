#pragma once

#include <thread>
#include <vector>
#include <thread>
#include <tbb/concurrent_queue.h>

class ThreadPool
{
public:
    ThreadPool(int numThreads = std::thread::hardware_concurrency());
    ~ThreadPool();
    void submitTask(std::function<void()> f);

private:
    tbb::concurrent_queue<std::function<void()>> m_tasks;
    std::vector<std::thread>                    m_workers;
    std::atomic<bool>                           m_done;
};