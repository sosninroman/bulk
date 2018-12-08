#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include "logger.h"
#include <spdlog/common.h>
#include <queue>

int main()
{
    try
    {
        std::queue<std::string> q;
        q.push(std::string("a"));
        q.push(std::string("b"));
        q.push(std::string("c"));
        bulk::SpdLogger log;
        log.handleCommands(q);
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        std::cout << "Log init failed: " << ex.what() << std::endl;
    }
    return 0;
}
