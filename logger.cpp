#include "logger.h"

namespace bulk
{

void SpdLogger::handleCommands(std::queue<std::string> commands)
{
    std::string logName("bulk");
    auto nowDuration = std::chrono::system_clock::now().time_since_epoch();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(nowDuration).count();
    logName.append(std::to_string(ms) ).append(".log");
    openLog(logName);
    writeCommands(commands);
}

void SpdLogger::openLog(const std::string& logfile)
{
    m_fileLog = spdlog::basic_logger_mt(FILE_LOGGER_NAME, logfile);
    m_fileLog->set_pattern("%v");
}

void SpdLogger::writeCommands(std::queue<std::string>& commands)
{
    std::string str;
    while(!commands.empty() )
    {
        auto cmd = commands.front();
        commands.pop();
        str.append(cmd);
        if(!commands.empty() )
            str.append(",");
        m_fileLog->info(cmd);
    }
    m_consoleLog->info(str);
}

}
