#ifndef CommandProcessor_H
#define CommandProcessor_H

#include <cassert>
#include <chrono>
#include <deque>
#include <queue>
#include <string>

namespace bulk
{

const std::string BRACE_LEFT = std::string("{");
const std::string BRACE_RIGHT = std::string("}");

template<class HandlerType>
class CommandProcessor
{
    enum class Status
    {
        BULK_IS_LOADING = 0,
        BULK_IS_READY,
        BULK_IS_READY_START_BRACE_BULK,
        BRACE_BULK_IS_LOADING
    };
public:
    CommandProcessor(size_t bulkSize, const HandlerType& logger = HandlerType()):
        m_handler(logger), m_bulkSize(bulkSize)
    {}

    void processCommandsStream(std::istream &stream);
private:
    Status pushCmd(const std::string& cmd);
    std::string popCmd();

    Status handleLeftBrace();
    Status handleRightBrace();

    std::queue<std::string> flush()
    {
        std::queue<std::string> cmds(m_buffer);
        m_buffer.clear();
        if(m_status == Status::BULK_IS_READY_START_BRACE_BULK)
        {
            m_status = Status::BRACE_BULK_IS_LOADING;
        }
        if(m_status == Status::BULK_IS_READY)
        {
            m_status = Status::BULK_IS_LOADING;
        }
        return cmds;
    }

    void processCommands(const std::queue<std::string> cmds);
private:
    HandlerType m_handler;

    Status m_status = Status::BULK_IS_LOADING;
    size_t m_bulkSize;
    std::deque<std::string> m_buffer;
    size_t m_braceCounter = 0;
};

template<class HandlerType>
void CommandProcessor<HandlerType>::processCommandsStream(std::istream &stream)
{
    for(std::string command; std::getline(stream, command);)
    {
        auto status = pushCmd(command);

        if(status == Status::BULK_IS_READY || status == Status::BULK_IS_READY_START_BRACE_BULK)
        {
            processCommands(flush() );
        }
    }
    assert(m_status == Status::BULK_IS_LOADING || m_status == Status::BRACE_BULK_IS_LOADING);
    if(!m_buffer.empty() )
    {
        if(m_status == Status::BULK_IS_LOADING)
        {
            processCommands(flush() );
        }
        else
        {
            flush();
        }
    }
}

template<class HandlerType>
void CommandProcessor<HandlerType>::processCommands(const std::queue<std::string> cmds)
{
    m_handler.handleCommands(cmds);
}

template<class HandlerType>
typename CommandProcessor<HandlerType>::Status CommandProcessor<HandlerType>::pushCmd(const std::string& cmd)
{
    assert(m_status != Status::BULK_IS_READY);

    if(cmd == BRACE_LEFT)
    {
        return handleLeftBrace();
    }
    if(cmd == BRACE_RIGHT)
    {
        return handleRightBrace();
    }

    m_buffer.push_back(cmd);

    if(m_status == Status::BULK_IS_LOADING && m_buffer.size() == m_bulkSize)
    {
        m_status = Status::BULK_IS_READY;
    }

    return m_status;
}

template<class HandlerType>
typename CommandProcessor<HandlerType>::Status CommandProcessor<HandlerType>::handleLeftBrace()
{
    assert(m_status == Status::BULK_IS_LOADING || m_status == Status::BRACE_BULK_IS_LOADING);
    ++m_braceCounter;
    if(m_status == Status::BULK_IS_LOADING)
    {
        m_status = Status::BULK_IS_READY_START_BRACE_BULK;
    }
    return m_status;
}

template<class HandlerType>
typename CommandProcessor<HandlerType>::Status CommandProcessor<HandlerType>::handleRightBrace()
{
    assert(m_status == Status::BRACE_BULK_IS_LOADING);
    assert(m_braceCounter);

    --m_braceCounter;
    if(!m_braceCounter)
    {
        m_status = Status::BULK_IS_READY;
    }
    return m_status;
}

}
#endif
