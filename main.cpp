#include <iostream>
#include "commandprocessor.h"
#include "logger.h"
#include "multithreadlogger.h"

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        return 0;
    }
    int bulkSize = std::atoi(argv[1]);
    try
    {
        //bulk::CommandProcessor<bulk::ConsequentLogger> processor(bulkSize);
        bulk::CommandProcessor<bulk::MultiThreadLogger<2>> processor(bulkSize);
        processor.processCommandsStream(std::cin);
    }
    catch (const spdlog::spdlog_ex& ex)
    {
        std::cout << ex.what() << std::endl;
    }
    catch (const std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
    }
    return 0;
}
