#include <iostream>

//#include "commandprocessor.h"
//#include "logger.h"
//#include "multithreadlogger.h"
//#include <sstream>

#include "async.h"

int main(int, char *[]) {
    std::size_t bulk = 5;
    auto h = async::connect(bulk);
    auto h2 = async::connect(bulk);
    async::receive(h, "\n1", 2);
    async::receive(h2, "1\n", 2);
    async::receive(h, "\n2\n3\n4\n5\n6\n{\na\n", 15);
    async::receive(h, "b\nc\nd\n}\n8", 11);
    async::receive(h, "9\n", 2);
    async::disconnect(h);
    async::disconnect(h2);

    return 0;
}

//int main(int argc, char* argv[])
//{
//    if(argc < 2)
//    {
//        return 0;
//    }
//    int bulkSize = std::atoi(argv[1]);
//    try
//    {
//        bulk::CommandProcessor<bulk::MultiThreadLogger<2>> processor(bulkSize);
//        std::string data("\n2\n3\n4\n5\n6\n{\na\n");
//        std::stringstream stream(data);
//        processor.processCommandsStream(stream);
//        //processor.processCommandsStream(std::cin);

//        std::cout << "Statistics: " << std::endl;
//        std::cout << "Total: " << processor.linesProcessed() << " lines, " <<
//                     processor.blocksProcessed() << " blocks, " <<
//                     processor.commandsProcessed() << " commands" << std::endl;
//        auto threadsInfo = processor.handler().getStat();
//        for(auto& pair : threadsInfo)
//        {
//            std::cout << pair.first << ": " <<
//                         pair.second.blocksNum << " blocks " <<
//                         pair.second.commandsNum << " commands " << std::endl;
//        }
//    }
//    catch (const spdlog::spdlog_ex& ex)
//    {
//        std::cout << ex.what() << std::endl;
//    }
//    catch (const std::exception& ex)
//    {
//        std::cout << ex.what() << std::endl;
//    }
//    return 0;
//}
