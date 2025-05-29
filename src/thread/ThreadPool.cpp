#include "ThreadPool.hpp"

ThreadPool::ThreadPool(int numThreads) : m_workers(numThreads)
{
    for (std::size_t i{0}; i < numThreads; ++i)
    {
        m_workers.emplace_back([this]
        {
            std::function<void()> task;
            while (!m_done)
            {
                if (m_tasks.try_pop(task))
                    task();
                else
                    std::this_thread::yield();
            }
        });
    }
}

ThreadPool::~ThreadPool()
{
    m_done = true;
    for (auto& worker : m_workers)
    {
        if (worker.joinable())
            worker.join();
    }
    
}

void ThreadPool::submitTask(std::function<void()> task)
{
    m_tasks.push(std::move(task));
}