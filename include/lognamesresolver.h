#ifndef LOGNAMESRESOLVER_H
#define LOGNAMESRESOLVER_H

namespace bulk
{

class LogNamesResolver
{
public:
    LogNamesResolver(const LogNamesResolver&) = delete;
    static LogNamesResolver& instance()
    {
        static LogNamesResolver instance;
        return instance;
    }

    auto getNextFileLogPrefix(){return m_fileLogPrefix++;}
    auto getNextConsoleLogPrefix() {return m_consoleLogPrefix++;}
private:
    LogNamesResolver() = default;
    unsigned long long m_fileLogPrefix = 0;
    unsigned long long m_consoleLogPrefix = 0;
};

}

#endif
