#ifndef COMMANDPROCESSORSMANAGER_H
#define COMMANDPROCESSORSMANAGER_H

#include <unordered_map>
#include <memory>
#include <sstream>
#include <mutex>
#include "commandprocessor.h"

namespace bulk
{

template<class HandlerType>
class CommandProcessorsManager
{
    using CommandProcessorType = CommandProcessor<HandlerType>;
    using CommandProcessorPtr = std::unique_ptr<CommandProcessorType>;
public:
    CommandProcessorsManager(const CommandProcessorsManager&) = delete;

    static CommandProcessorsManager& instance()
    {
        static CommandProcessorsManager instance;
        return instance;
    }

    void* createCommandProcessor(std::size_t bulk)
    {
        auto processor = std::make_unique<CommandProcessorType>(bulk);
        auto handler = processor.get();
        std::lock_guard<std::mutex> guard(m_processorsMutex);
        m_commandProcessors.insert(std::make_pair(handler, std::move(processor) ) );
        handler->startWork();
        return handler;
    }

    void processCommandBuffer(void* handle, const char *data, std::size_t size)
    {
        std::string dataStr(data, size);
        std::stringstream commandStream(dataStr);

        std::lock_guard<std::mutex> guard(m_processorsMutex);

        auto handler = m_commandProcessors.at(reinterpret_cast<CommandProcessorType*>(handle) ).get();
        handler->appendCommandsStream(commandStream);
    }

    void removeCommandProcessor(void* handle)
    {
        std::lock_guard<std::mutex> guard(m_processorsMutex);
        auto itr = m_commandProcessors.find(reinterpret_cast<CommandProcessorType*>(handle) );
        if(itr != m_commandProcessors.end() )
        {
            itr->second->stopWork();
            m_commandProcessors.erase(itr);
        }
    }
private:
    CommandProcessorsManager() = default;

    std::mutex m_processorsMutex;
    std::unordered_map<CommandProcessorType*, CommandProcessorPtr> m_commandProcessors;
};
}

#endif
