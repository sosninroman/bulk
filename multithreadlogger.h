#ifndef MULTITHREADLOGGER_H
#define MULTITHREADLOGGER_H

#include <thread>
#include <functional>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "threadsafequeue.h"
#include "bulkconstants.h"

namespace bulk
{

namespace internal
{

class FileThread
{
public:
    FileThread(ThreadSafeQueue<std::function<int()>>& queue, std::atomic_int& freeThreads):
        m_queue(queue), m_freeThreads(freeThreads)
    {
        m_thread = std::thread(&FileThread::process, this);
        ++m_freeThreads;
    }

    FileThread(FileThread&) = delete;

    FileThread(FileThread&& rhs):
        m_queue(rhs.m_queue),
        m_thread(std::move(rhs.m_thread)),
        m_blocksNum(rhs.m_blocksNum),
        m_commandsNum(rhs.m_commandsNum),
        m_freeThreads(rhs.m_freeThreads)
    {}

    ~FileThread()
    {
        --m_freeThreads;
        m_thread.join();
    }

    void process()
    {
        while(true)
        {
            ++m_freeThreads;
            auto task = m_queue.pop();
            if(!task)
            {
                return;
            }
            else
            {
                ++m_blocksNum;
                --m_freeThreads;
                m_commandsNum += (*task)();
            }
        }
    }

    int blocksNum() const {return m_blocksNum;}
    int commandsNum() const {return m_commandsNum;}
private:
    ThreadSafeQueue<std::function<int()>>& m_queue;
    std::thread m_thread;
    int m_blocksNum = 0;
    int m_commandsNum = 0;
    std::atomic_int& m_freeThreads;
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
        m_fileThreads.reserve(ThreadsNum);
        m_consoleLog->set_pattern("bulk: %v");
        m_consoleThread = std::thread(&MultiThreadLogger::process, this);
    }

    MultiThreadLogger(const MultiThreadLogger&) = delete;
    MultiThreadLogger(const MultiThreadLogger&&) = delete;

    ~MultiThreadLogger()
    {
        m_bulks.cancelReading();
        m_fileLoggingTasks.cancelReading();
        m_consoleThread.join();
    }

    void handleCommands(std::queue<std::string> commands)
    {
        m_bulks.push(commands);
    }

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
                auto nowDuration = std::chrono::system_clock::now().time_since_epoch();
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(nowDuration).count();
                logName.append(std::to_string(ms) ).append(".log");
                std::queue<std::string> cmds = *commands;

                m_fileLoggingTasks.emplace([cmds, logName]() mutable {
                    auto fileLog = spdlog::basic_logger_mt(logName, logName);
                    fileLog->set_pattern("%v");
                    int sz = cmds.size();
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
                ++m_blocksNum;
                m_commandsNum += sz;
            }
        }
    }
private:
    ThreadSafeQueue<std::queue<std::string>> m_bulks;
    ThreadSafeQueue<std::function<int()>> m_fileLoggingTasks;

    std::thread m_consoleThread;
    std::shared_ptr<spdlog::logger> m_consoleLog = nullptr;

    int m_blocksNum = 0;
    int m_commandsNum = 0;

    std::vector<internal::FileThread> m_fileThreads;

    std::atomic_int m_freeThreads;
};

}

#endif
