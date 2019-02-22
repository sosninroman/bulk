#ifndef MULTITHREADLOGGER_H
#define MULTITHREADLOGGER_H

#include <iostream>
#include <thread>
#include <functional>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "threadsafequeue.h"
#include "bulkconstants.h"
#include <map>

namespace bulk
{

struct StatBlock
{
    int blocksNum = 0;
    int commandsNum = 0;
};

namespace internal
{

class FileThread
{
public:
    FileThread(ThreadSafeQueue<std::function<int()>>& queue, std::atomic_int& freeThreads):
        m_queue(queue), m_freeThreads(freeThreads)
    {
        m_isWorking = true;
        m_thread = std::thread(&FileThread::process, this);
    }

    FileThread(FileThread&) = delete;

    FileThread(FileThread&& rhs):
        m_queue(rhs.m_queue),
        m_thread(std::move(rhs.m_thread)),
        m_statBlock(rhs.m_statBlock),
        m_freeThreads(rhs.m_freeThreads)
    {}

    ~FileThread()
    {
        m_thread.join();
    }

    void process();

    StatBlock statBlock() const
    {
        std::lock_guard<std::mutex> guard(m_statMutex);
        return m_statBlock;
    }

    bool isWorking() const {return m_isWorking;}
private:
    ThreadSafeQueue<std::function<int()>>& m_queue;
    std::thread m_thread;
    StatBlock m_statBlock;
    mutable std::mutex m_statMutex;
    std::atomic_int& m_freeThreads;
    std::atomic_int m_isWorking{false};
};

}

template<size_t ThreadsNum>
class MultiThreadLogger
{
public:
    MultiThreadLogger():
        m_consoleLog(spdlog::stdout_logger_st(CONSOLE_LOGGER_NAME)),
        m_freeThreads(0)
    {
        m_consoleLog->set_pattern("bulk: %v");
        m_fileThreads.reserve(ThreadsNum);
    }

    MultiThreadLogger(const MultiThreadLogger&) = delete;
    MultiThreadLogger(const MultiThreadLogger&&) = delete;

    ~MultiThreadLogger()
    {
        m_bulks.cancelReading();
        m_fileLoggingTasks.cancelReading();
        if(m_consoleThread.joinable() )
        {
            m_consoleThread.join();
        }
    }

    void startWork()
    {
        if(!m_isWorking)
        {
            m_isWorking = true;
            assert(!m_consoleThread.joinable() );

            if(!m_fileThreads.empty() )
            {
                m_fileThreads.clear();
            }

            m_bulks.setReadingAvailable(true);
            m_fileLoggingTasks.setReadingAvailable(true);

            m_consoleThread = std::thread(&MultiThreadLogger::process, this);
        }
    }

    void stopWork()
    {
        if(m_isWorking)
        {
            m_isWorking = false;

            while(!m_bulks.empty() )
            {
                std::this_thread::yield();
            }
            m_bulks.cancelReading();
            assert(m_consoleThread.joinable() );
            m_consoleThread.join();

            while(!allFileThreadsAreStopped() )
            {
                if(m_fileLoggingTasks.empty() )
                {
                    m_fileLoggingTasks.cancelReading();
                }
                std::this_thread::yield();
            }
        }
    }

    bool allFileThreadsAreStopped()
    {
        for(const auto& fileThread : m_fileThreads)
        {
            if(fileThread.isWorking())
            {
                return false;
            }
        }
        return true;
    }

    void handleCommands(std::queue<std::string> commands)
    {
        if(m_isWorking)
        {
            m_bulks.push(commands);
        }
    }

    std::map<std::string, StatBlock> getStat() const
    {
        std::lock_guard<std::mutex> guard(m_statMutex);
        std::map<std::string, StatBlock> result;
        result.insert(std::make_pair(CONSOLE_THREAD_STAT_NAME, m_statBlock));
        size_t ind = 0;
        for(const auto& thread : m_fileThreads)
        {
            const std::string threadName = FILE_THREAD_STAT_NAME + std::to_string(ind);
            result.insert(std::make_pair(threadName, thread.statBlock() ) );
            ++ind;
        }
        assert(ind <= ThreadsNum);
        while(ind != ThreadsNum)
        {
            const std::string threadName = FILE_THREAD_STAT_NAME + std::to_string(ind);
            result.insert(std::make_pair(threadName, StatBlock() ) );
            ++ind;
        }
        return result;
    }
private:
    void process()
    {
        while(true)
        {
            std::shared_ptr<std::queue<std::string>> commands = m_bulks.pop();
            if(!commands)
            {
                return;
            }
            else
            {
                if(!m_freeThreads)
                {
                    if(m_fileThreads.size() < ThreadsNum)
                    {
                        m_fileThreads.emplace_back(m_fileLoggingTasks, m_freeThreads);
                    }
                    while(!m_freeThreads)
                    {
                        std::this_thread::yield();
                    }
                }

                std::string logName("bulk");
                const auto nowDuration = std::chrono::system_clock::now().time_since_epoch();
                const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(nowDuration).count();
                logName.append(std::to_string(ms) ).append(std::to_string(m_uniqueNamePostfix++) ).append(".log");

                m_fileLoggingTasks.emplace([cmds = *commands, logName]() mutable {
                    auto fileLog = spdlog::basic_logger_mt(logName, logName);
                    fileLog->set_pattern("%v");
                    const int sz = cmds.size();
                    while(!cmds.empty() )
                    {
                        auto cmd = cmds.front();
                        cmds.pop();
                        fileLog->info(cmd);
                    }
                    return sz;
                });

                std::string str;
                const auto sz = commands->size();
                while(!commands->empty() )
                {
                    auto cmd = commands->front();
                    commands->pop();
                    str.append(cmd);
                    if(!commands->empty() )
                        str.append(",");
                }
                m_consoleLog->info(str);

                std::lock_guard<std::mutex> guard(m_statMutex);
                ++m_statBlock.blocksNum;
                m_statBlock.commandsNum += sz;
            }
        }
    }
private:
    ThreadSafeQueue<std::queue<std::string>> m_bulks;
    ThreadSafeQueue<std::function<int()>> m_fileLoggingTasks;

    std::thread m_consoleThread;
    std::shared_ptr<spdlog::logger> m_consoleLog = nullptr;

    mutable std::mutex m_statMutex;
    StatBlock m_statBlock;

    std::vector<internal::FileThread> m_fileThreads;

    bool m_isWorking = false;

    std::atomic_int m_freeThreads;

    size_t m_uniqueNamePostfix = 0;
};

}

#endif
