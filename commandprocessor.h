#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H

#include <cassert>
#include <deque>
#include <queue>
#include <string>
#include <stdexcept>

namespace bulk
{

const std::string OPEN_BLOCK_SYMBOL = std::string("{");
const std::string CLOSE_BLOCK_SYMBOL = std::string("}");

template<class HandlerType>
class CommandProcessor
{
    enum class Status
    {
        EMPTY,
        BLOCK_EMPTY,
        LOADING,
        BLOCK_LOADING,
        READY,
        READY_START_BLOCK_BULK
    };
public:
    explicit CommandProcessor(size_t bulkSize, const HandlerType& logger = HandlerType()):
        m_handler(logger), m_bulkSize(bulkSize)
    {}

    void processCommandsStream(std::istream &stream);
private:
    Status pushCmd(const std::string& cmd);
    std::string popCmd();

    Status handleOpenBlockSymbol();
    Status handleCloseBlockSymbol();

    std::queue<std::string> flush()
    {
        if(m_buffer.empty() )
            return std::queue<std::string>();

        std::queue<std::string> cmds(m_buffer);
        m_buffer.clear();

        if(m_status == Status::READY_START_BLOCK_BULK)
        {
            m_status = Status::BLOCK_EMPTY;
        }
        else
        {
            m_status = Status::EMPTY;
        }

        return cmds;
    }

    //throw std::invalid_argument
    void processCommands(const std::queue<std::string> cmds);
private:
    HandlerType m_handler;

    Status m_status = Status::EMPTY;
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

        if(status == Status::READY || status == Status::READY_START_BLOCK_BULK)
        {
            processCommands(flush() );
        }
    }
    assert(m_status == Status::LOADING || m_status == Status::BLOCK_LOADING
           || m_status == Status::EMPTY || m_status == Status::BLOCK_EMPTY);

    switch(m_status)
    {
    case Status::LOADING:
        processCommands(flush() );
        break;
    case Status::BLOCK_LOADING:
        flush();
        break;
    default:
        break;
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
    assert(m_status != Status::READY && m_status != Status::READY_START_BLOCK_BULK);

    if(cmd == OPEN_BLOCK_SYMBOL)
    {
        return handleOpenBlockSymbol();
    }
    if(cmd == CLOSE_BLOCK_SYMBOL)
    {
        return handleCloseBlockSymbol();
    }

    m_buffer.push_back(cmd);

    switch(m_status)
    {
    case Status::EMPTY:
        m_status = Status::LOADING;
        break;
    case Status::BLOCK_EMPTY:
        m_status = Status::BLOCK_LOADING;
        break;
    default:
        break;
    }

    if(m_status == Status::LOADING && m_buffer.size() == m_bulkSize)
    {
        m_status = Status::READY;
    }

    return m_status;
}

template<class HandlerType>
typename CommandProcessor<HandlerType>::Status CommandProcessor<HandlerType>::handleOpenBlockSymbol()
{
    assert(m_status == Status::LOADING || m_status == Status::BLOCK_LOADING
           || m_status == Status::EMPTY || m_status == Status::BLOCK_EMPTY);

    ++m_braceCounter;
    switch(m_status)
    {
    case Status::LOADING:
        m_status = Status::READY_START_BLOCK_BULK;
        break;
    case Status::EMPTY:
        m_status = Status::BLOCK_EMPTY;
        break;
    default:
        break;
    }

    return m_status;
}

template<class HandlerType>
typename CommandProcessor<HandlerType>::Status CommandProcessor<HandlerType>::handleCloseBlockSymbol()
{
    if((m_status != Status::BLOCK_LOADING && m_status != Status::BLOCK_EMPTY) || !m_braceCounter)
        throw std::invalid_argument("invalid command sequence: too much close block symbols!");

    --m_braceCounter;
    if(!m_braceCounter)
    {
        m_status = Status::READY;
    }
    return m_status;
}

}
#endif
