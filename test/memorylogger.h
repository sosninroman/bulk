#ifndef MEMORYLOGGERTEST_H
#define MEMORYLOGGERTEST_H

#include <map>
#include <queue>
#include <memory>

namespace bulktest
{

struct MemLogger
{
    MemLogger():m_values(new std::map<size_t, std::string>()){}

    void handleCommands(std::queue<std::string> commands)
    {
        std::string str;
        while(!commands.empty() )
        {
            auto cmd = commands.front();
            commands.pop();
            str.append(cmd);
        }
        (*m_values)[m_bulkCounter++] = str;
    }

    std::shared_ptr<std::map<size_t, std::string> > m_values;
    size_t m_bulkCounter = 0;
};

}
#endif
