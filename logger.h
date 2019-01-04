#ifndef BULKLOGGER_H
#define BULKLOGGER_H

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <queue>
#include <iostream>
#include "bulkconstants.h"

namespace bulk
{

const std::string FILE_LOGGER_NAME = std::string("file_log");

class ConsequentLogger
{
public:
    ConsequentLogger():
        m_consoleLog(spdlog::stdout_logger_st(CONSOLE_LOGGER_NAME))
    {
        m_consoleLog->set_pattern("bulk: %v");
    }

    void handleCommands(std::queue<std::string> commands);
    void startWork(){return;}
    void stopWork() {return;}
private:
    void openLog(const std::string &logfile);
    void writeCommands(std::queue<std::string> &commands);

    std::shared_ptr<spdlog::logger> m_fileLog = nullptr;
    std::shared_ptr<spdlog::logger> m_consoleLog = nullptr;
};

}

#endif
