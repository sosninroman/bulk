#include <gtest/gtest.h>
#include "commandprocessor.h"
#include "memorylogger.h"
#include <sstream>

TEST(BULK_TEST, test1)
{
    bulktest::MemLogger logger;
    bulk::CommandProcessor<bulktest::MemLogger> cmdHandler(1,logger);

    std::string str("cmd1\ncmd2\ncmd3");
    std::istringstream stream(str);

    cmdHandler.processCommandsStream(stream);

    ASSERT_EQ(logger.m_values->size(), 3);
    for(size_t i = 0; i < logger.m_values->size(); ++ i)
    {
        if(i == 0)
            ASSERT_EQ((*logger.m_values)[i], std::string("cmd1"));
        if(i == 1)
            ASSERT_EQ((*logger.m_values)[i], std::string("cmd2"));
        if(i == 2)
            ASSERT_EQ((*logger.m_values)[i], std::string("cmd3"));
    }
}

TEST(BULK_TEST, test2)
{
    bulktest::MemLogger logger;
    bulk::CommandProcessor<bulktest::MemLogger> cmdHandler(2,logger);

    std::string str("cmd1\ncmd2\ncmd3\ncmd4");
    std::istringstream stream(str);

    cmdHandler.processCommandsStream(stream);

    ASSERT_EQ(logger.m_values->size(), 2);
    for(size_t i = 0; i < logger.m_values->size(); ++ i)
    {
        if(i == 0)
            ASSERT_EQ((*logger.m_values)[i], std::string("cmd1cmd2"));
        if(i == 1)
            ASSERT_EQ((*logger.m_values)[i], std::string("cmd3cmd4"));
    }
}

TEST(BULK_TEST, test3)
{
    bulktest::MemLogger logger;
    bulk::CommandProcessor<bulktest::MemLogger> cmdHandler(2,logger);

    std::string str("cmd1\ncmd2\ncmd3");
    std::istringstream stream(str);

    cmdHandler.processCommandsStream(stream);

    ASSERT_EQ(logger.m_values->size(), 2);
    for(size_t i = 0; i < logger.m_values->size(); ++ i)
    {
        if(i == 0)
            ASSERT_EQ((*logger.m_values)[i], std::string("cmd1cmd2"));
        if(i == 1)
            ASSERT_EQ((*logger.m_values)[i], std::string("cmd3"));
    }
}

TEST(BULK_TEST, test4)
{
    bulktest::MemLogger logger;
    bulk::CommandProcessor<bulktest::MemLogger> cmdHandler(2,logger);

    std::string str("\n");
    std::istringstream stream(str);

    cmdHandler.processCommandsStream(stream);

    ASSERT_EQ(logger.m_values->size(), 1);
    for(size_t i = 0; i < logger.m_values->size(); ++ i)
    {
        if(i == 0)
            ASSERT_EQ((*logger.m_values)[i], std::string(""));
    }
}

TEST(BULK_TEST, test5)
{
    bulktest::MemLogger logger;
    bulk::CommandProcessor<bulktest::MemLogger> cmdHandler(1,logger);

    std::string str("cmd1\n\ncmd2");
    std::istringstream stream(str);

    cmdHandler.processCommandsStream(stream);

    ASSERT_EQ(logger.m_values->size(), 3);
    for(size_t i = 0; i < logger.m_values->size(); ++ i)
    {
        if(i == 0)
            ASSERT_EQ((*logger.m_values)[i], std::string("cmd1"));
        if(i == 1)
            ASSERT_EQ((*logger.m_values)[i], std::string(""));
        if(i == 2)
            ASSERT_EQ((*logger.m_values)[i], std::string("cmd2"));
    }
}

TEST(BULK_TEST, test6)
{
    bulktest::MemLogger logger;
    bulk::CommandProcessor<bulktest::MemLogger> cmdHandler(2,logger);

    std::string str("cmd1\n{\ncmd2\ncmd3\ncmd4");
    std::istringstream stream(str);

    cmdHandler.processCommandsStream(stream);

    ASSERT_EQ(logger.m_values->size(), 1);
    for(size_t i = 0; i < logger.m_values->size(); ++ i)
    {
        if(i == 0)
            ASSERT_EQ((*logger.m_values)[i], std::string("cmd1"));
    }
}

TEST(BULK_TEST, test7)
{
    bulktest::MemLogger logger;
    bulk::CommandProcessor<bulktest::MemLogger> cmdHandler(2,logger);

    std::string str("{\ncmd1\ncmd2\ncmd3\ncmd4");
    std::istringstream stream(str);

    cmdHandler.processCommandsStream(stream);

    ASSERT_TRUE(logger.m_values->empty() );
}
