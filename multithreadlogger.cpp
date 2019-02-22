#include "multithreadlogger.h"

namespace bulk
{

namespace internal
{

void FileThread::process()
{
    while(true)
    {
        ++m_freeThreads;
        auto task = m_queue.pop();
        --m_freeThreads;
        if(!task)
        {
            m_isWorking = false;
            return;
        }
        else
        {
            try
            {
                auto cmdNum = (*task)();
                std::lock_guard<std::mutex> guard(m_statMutex);
                ++m_statBlock.blocksNum;
                m_statBlock.commandsNum += cmdNum;
            }
            catch(const std::exception& ex)
            {
                std::cout << ex.what();
            }
        }
    }
}

}//internal

}
