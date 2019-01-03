#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H

#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>

namespace bulk
{

template<class T>
class ThreadSafeQueue
{
public:
    ThreadSafeQueue():
        m_readers(0),
        m_readingAvailable(true)
    {}

    ThreadSafeQueue(ThreadSafeQueue& rhs) = delete;
    ThreadSafeQueue(ThreadSafeQueue&&) = delete;

    ~ThreadSafeQueue()
    {
        cancelReading();
    }

    void cancelReading()
    {
        if(m_readingAvailable)
        {
            m_readingAvailable = false;
        }
        m_cond.notify_all();
        while(m_readers)
        {
            std::this_thread::yield();
        }
    }

    void setReadingAvailable(bool available)
    {
        if(m_readingAvailable != available)
        {
            m_readingAvailable = available;
            if(!m_readingAvailable)
            {
                cancelReading();
            }
        }
    }

    void push(T value)
    {
        auto insertVal = std::make_shared<T>(std::move(value) );
        std::lock_guard<std::mutex> guard(m_mutex);
        m_data.emplace(std::move(insertVal) );
        m_cond.notify_one();
    }

    template<class... Args>
    void emplace(Args&&... args)
    {
        auto insertVal = std::make_shared<T>(std::forward<Args>(args)...);
        std::lock_guard<std::mutex> guard(m_mutex);
        m_data.emplace(std::move(insertVal) );
        m_cond.notify_one();
    }

    std::shared_ptr<T> pop()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        ++m_readers;
        m_cond.wait(lock, [this](){return !m_data.empty() || !m_readingAvailable;});
        std::shared_ptr<T> result;
        if(m_readingAvailable)
        {
            result = m_data.front();
            m_data.pop();
        }
        --m_readers;
        return result;
    }
private:
    std::queue<std::shared_ptr<T>> m_data;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    std::atomic_int m_readers;
    std::atomic_bool m_readingAvailable;
};

}
#endif
