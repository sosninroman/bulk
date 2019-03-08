#include <gtest/gtest.h>
#include "commandprocessor.h"
#include "memorylogger.h"
#include <sstream>

TEST(BULK_TEST, test1)
{
    bulktest::MemLogger logger;
    bulk::CommandProcessor<bulktest::MemLogger> cmdHandler(1,logger);

    std::string str("1\n2\n3");
    std::istringstream stream(str);

    cmdHandler.startWork();
    cmdHandler.appendCommandsStream(stream);
    cmdHandler.stopWork();

    ASSERT_EQ(logger.m_values->size(), 3);
    for(size_t i = 0; i < logger.m_values->size(); ++ i)
    {
        if(i == 0)
            ASSERT_EQ((*logger.m_values)[i], std::string("1"));
        if(i == 1)
            ASSERT_EQ((*logger.m_values)[i], std::string("2"));
        if(i == 2)
            ASSERT_EQ((*logger.m_values)[i], std::string("3"));
    }
}

TEST(BULK_TEST, test2)
{
    bulktest::MemLogger logger;
    bulk::CommandProcessor<bulktest::MemLogger> cmdHandler(2,logger);

    std::string str("1\n2\n3\n4");
    std::istringstream stream(str);

    cmdHandler.startWork();
    cmdHandler.appendCommandsStream(stream);
    cmdHandler.stopWork();

    ASSERT_EQ(logger.m_values->size(), 2);
    for(size_t i = 0; i < logger.m_values->size(); ++ i)
    {
        if(i == 0)
            ASSERT_EQ((*logger.m_values)[i], std::string("12"));
        if(i == 1)
            ASSERT_EQ((*logger.m_values)[i], std::string("34"));
    }
}

TEST(BULK_TEST, test3)
{
    bulktest::MemLogger logger;
    bulk::CommandProcessor<bulktest::MemLogger> cmdHandler(2,logger);

    std::string str("1\n2\n3");
    std::istringstream stream(str);

    cmdHandler.startWork();
    cmdHandler.appendCommandsStream(stream);
    cmdHandler.stopWork();

    ASSERT_EQ(logger.m_values->size(), 2);
    for(size_t i = 0; i < logger.m_values->size(); ++ i)
    {
        if(i == 0)
            ASSERT_EQ((*logger.m_values)[i], std::string("12"));
        if(i == 1)
            ASSERT_EQ((*logger.m_values)[i], std::string("3"));
    }
}

TEST(BULK_TEST, test4)
{
    bulktest::MemLogger logger;
    bulk::CommandProcessor<bulktest::MemLogger> cmdHandler(2,logger);

    std::string str("\n");
    std::istringstream stream(str);

    cmdHandler.startWork();
    cmdHandler.appendCommandsStream(stream);
    cmdHandler.stopWork();

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

    std::string str("1\n\n2");
    std::istringstream stream(str);

    cmdHandler.startWork();
    cmdHandler.appendCommandsStream(stream);
    cmdHandler.stopWork();

    ASSERT_EQ(logger.m_values->size(), 3);
    for(size_t i = 0; i < logger.m_values->size(); ++ i)
    {
        if(i == 0)
            ASSERT_EQ((*logger.m_values)[i], std::string("1"));
        if(i == 1)
            ASSERT_EQ((*logger.m_values)[i], std::string(""));
        if(i == 2)
            ASSERT_EQ((*logger.m_values)[i], std::string("2"));
    }
}

TEST(BULK_TEST, test6)
{
    bulktest::MemLogger logger;
    bulk::CommandProcessor<bulktest::MemLogger> cmdHandler(2,logger);

    std::string str("1\n{\n2\n3\n4");
    std::istringstream stream(str);

    cmdHandler.startWork();
    cmdHandler.appendCommandsStream(stream);
    cmdHandler.stopWork();

    ASSERT_EQ(logger.m_values->size(), 1);
    for(size_t i = 0; i < logger.m_values->size(); ++ i)
    {
        if(i == 0)
            ASSERT_EQ((*logger.m_values)[i], std::string("1"));
    }
}

TEST(BULK_TEST, test7)
{
    bulktest::MemLogger logger;
    bulk::CommandProcessor<bulktest::MemLogger> cmdHandler(2,logger);

    std::string str("{\n1\n2\n3\n4");
    std::istringstream stream(str);

    cmdHandler.startWork();
    cmdHandler.appendCommandsStream(stream);
    cmdHandler.stopWork();

    ASSERT_TRUE(logger.m_values->empty() );
}

TEST(BULK_TEST, test8)
{
    bulktest::MemLogger logger;
    bulk::CommandProcessor<bulktest::MemLogger> cmdHandler(2,logger);

    std::string str("1\n2\n3\n4\n{");
    std::istringstream stream(str);

    cmdHandler.startWork();
    cmdHandler.appendCommandsStream(stream);
    cmdHandler.stopWork();

    ASSERT_EQ(logger.m_values->size(), 2);
    for(size_t i = 0; i < logger.m_values->size(); ++ i)
    {
        if(i == 0)
            ASSERT_EQ((*logger.m_values)[i], std::string("12"));
        if(i == 1)
            ASSERT_EQ((*logger.m_values)[i], std::string("34"));
    }
}

TEST(BULK_TEST, test9)
{
    bulktest::MemLogger logger;
    bulk::CommandProcessor<bulktest::MemLogger> cmdHandler(2,logger);

    std::string str("1\n{\n2\n3\n4\n}");
    std::istringstream stream(str);

    cmdHandler.startWork();
    cmdHandler.appendCommandsStream(stream);
    cmdHandler.stopWork();

    ASSERT_EQ(logger.m_values->size(), 2);
    for(size_t i = 0; i < logger.m_values->size(); ++ i)
    {
        if(i == 0)
            ASSERT_EQ((*logger.m_values)[i], std::string("1"));
        if(i == 1)
            ASSERT_EQ((*logger.m_values)[i], std::string("234"));
    }
}

TEST(BULK_TEST, test10)
{
    bulktest::MemLogger logger;
    bulk::CommandProcessor<bulktest::MemLogger> cmdHandler(2,logger);

    std::string str("1\n{\n2\n3\n}\n4");
    std::istringstream stream(str);

    cmdHandler.startWork();
    cmdHandler.appendCommandsStream(stream);
    cmdHandler.stopWork();

    ASSERT_EQ(logger.m_values->size(), 3);
    for(size_t i = 0; i < logger.m_values->size(); ++ i)
    {
        if(i == 0)
            ASSERT_EQ((*logger.m_values)[i], std::string("1"));
        if(i == 1)
            ASSERT_EQ((*logger.m_values)[i], std::string("23"));
        if(i == 2)
            ASSERT_EQ((*logger.m_values)[i], std::string("4"));
    }
}

TEST(BULK_TEST, test11)
{
    bulktest::MemLogger logger;
    bulk::CommandProcessor<bulktest::MemLogger> cmdHandler(2,logger);

    std::string str("{\n1\n2\n3\n4\n}");
    std::istringstream stream(str);

    cmdHandler.startWork();
    cmdHandler.appendCommandsStream(stream);
    cmdHandler.stopWork();

    ASSERT_EQ(logger.m_values->size(), 1);
    for(size_t i = 0; i < logger.m_values->size(); ++ i)
    {
        if(i == 0)
            ASSERT_EQ((*logger.m_values)[i], std::string("1234"));
    }
}

TEST(BULK_TEST, test12)
{
    bulktest::MemLogger logger;
    bulk::CommandProcessor<bulktest::MemLogger> cmdHandler(2,logger);

    std::string str("}\n");
    std::istringstream stream(str);

    cmdHandler.startWork();
    ASSERT_THROW(cmdHandler.appendCommandsStream(stream), std::invalid_argument);
}

TEST(BULK_TEST, test13)
{
    bulktest::MemLogger logger;
    bulk::CommandProcessor<bulktest::MemLogger> cmdHandler(2,logger);

    std::string str("{\n1\n{\n2\n}\n3\n4\n}");
    std::istringstream stream(str);

    cmdHandler.startWork();
    cmdHandler.appendCommandsStream(stream);
    cmdHandler.stopWork();

    ASSERT_EQ(logger.m_values->size(), 1);
    for(size_t i = 0; i < logger.m_values->size(); ++ i)
    {
        if(i == 0)
            ASSERT_EQ((*logger.m_values)[i], std::string("1234"));
    }
}

TEST(BULK_TEST, test14)
{
    bulktest::MemLogger logger;
    bulk::CommandProcessor<bulktest::MemLogger> cmdHandler(2,logger);

    std::string str("");
    std::istringstream stream(str);

    cmdHandler.startWork();
    cmdHandler.appendCommandsStream(stream);
    cmdHandler.stopWork();

    ASSERT_EQ(logger.m_values->size(), 0);
}

TEST(BULK_TEST, test15)
{
    bulktest::MemLogger logger;
    bulk::CommandProcessor<bulktest::MemLogger> cmdHandler(2,logger);

    std::string str1("\n1\n2\n3\n");
    std::string str2("4\n5\n6");
    std::istringstream stream1(str1);
    std::istringstream stream2(str2);

    cmdHandler.startWork();
    cmdHandler.appendCommandsStream(stream1);
    cmdHandler.appendCommandsStream(stream2);
    cmdHandler.stopWork();

    ASSERT_EQ(logger.m_values->size(), 4);
    for(size_t i = 0; i < logger.m_values->size(); ++ i)
    {
        if(i == 0)
            ASSERT_EQ((*logger.m_values)[i], std::string("1"));
        if(i == 1)
            ASSERT_EQ((*logger.m_values)[i], std::string("23"));
        if(i == 2)
            ASSERT_EQ((*logger.m_values)[i], std::string("45"));
        if(i == 3)
            ASSERT_EQ((*logger.m_values)[i], std::string("6"));
    }
}

TEST(BULK_TEST, test16)
{
    bulktest::MemLogger logger;
    bulk::CommandProcessor<bulktest::MemLogger> cmdHandler(2,logger);

    std::string str1("1\n2\n3");
    std::string str2("\n4\n5\n6");
    std::istringstream stream1(str1);
    std::istringstream stream2(str2);

    cmdHandler.startWork();
    cmdHandler.appendCommandsStream(stream1);
    cmdHandler.appendCommandsStream(stream2);
    cmdHandler.stopWork();

    ASSERT_EQ(logger.m_values->size(), 3);
    for(size_t i = 0; i < logger.m_values->size(); ++ i)
    {
        if(i == 0)
            ASSERT_EQ((*logger.m_values)[i], std::string("12"));
        if(i == 1)
            ASSERT_EQ((*logger.m_values)[i], std::string("34"));
        if(i == 2)
            ASSERT_EQ((*logger.m_values)[i], std::string("56"));
    }
}

TEST(BULK_TEST, test17)
{
    bulktest::MemLogger logger;
    bulk::CommandProcessor<bulktest::MemLogger> cmdHandler(2,logger);

    std::string str1("1\n2\n3\n");
    std::string str2("\n4\n5\n6");
    std::istringstream stream1(str1);
    std::istringstream stream2(str2);

    cmdHandler.startWork();
    cmdHandler.appendCommandsStream(stream1);
    cmdHandler.appendCommandsStream(stream2);
    cmdHandler.stopWork();

    ASSERT_EQ(logger.m_values->size(), 4);
    for(size_t i = 0; i < logger.m_values->size(); ++ i)
    {
        if(i == 0)
            ASSERT_EQ((*logger.m_values)[i], std::string("12"));
        if(i == 1)
            ASSERT_EQ((*logger.m_values)[i], std::string("3"));
        if(i == 2)
            ASSERT_EQ((*logger.m_values)[i], std::string("45"));
        if(i == 3)
            ASSERT_EQ((*logger.m_values)[i], std::string("6"));
    }
}

TEST(BULK_TEST, test18)
{
    bulktest::MemLogger logger;
    bulk::CommandProcessor<bulktest::MemLogger> cmdHandler(2,logger);

    std::string str("}");
    std::istringstream stream(str);

    cmdHandler.startWork();
    ASSERT_NO_THROW(cmdHandler.appendCommandsStream(stream) );
    ASSERT_THROW(cmdHandler.stopWork(), std::invalid_argument);
}

TEST(BULK_TEST, test19)
{
    bulktest::MemLogger logger;
    bulk::CommandProcessor<bulktest::MemLogger> cmdHandler(2,logger);

    std::string str1("1\n2\n3\n4");
    std::string str2("4\n5\n6");
    std::istringstream stream1(str1);
    std::istringstream stream2(str2);

    cmdHandler.startWork();
    cmdHandler.appendCommandsStream(stream1);
    cmdHandler.appendCommandsStream(stream2);
    cmdHandler.stopWork();

    ASSERT_EQ(logger.m_values->size(), 3);
    for(size_t i = 0; i < logger.m_values->size(); ++ i)
    {
        if(i == 0)
            ASSERT_EQ((*logger.m_values)[i], std::string("12"));
        if(i == 1)
            ASSERT_EQ((*logger.m_values)[i], std::string("344"));
        if(i == 2)
            ASSERT_EQ((*logger.m_values)[i], std::string("56"));
    }
}
