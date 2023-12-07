#include <iostream>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/cfg/env.h"
#include "spdlog/fmt/ostr.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/callback_sink.h"
/**
 * 简单的日志输出
 */
void simple_output()
{
    spdlog::info("Welcome to spdlog!");
    spdlog::error("Some error message with arg: {}", 1);

    spdlog::warn("Easy padding in numbers like {:08d}", 12);
    spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
    spdlog::info("Support for floats {:03.2f}", 1.23456);
    spdlog::info("Support for floats {:02.2f}", 123.23456);

    spdlog::info("Positional args are {1} {0}..", "too", "supported");
    spdlog::info("{:<30}", "left aligned");
}

/**
 * 写入文件的logger
 */
void basic_logfile_example()
{
    std::shared_ptr<spdlog::logger> logger;

    try
    {
        logger = spdlog::basic_logger_mt("basic_logger", "logs/basic-log.txt");
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        std::cout << "Log init failed: " << ex.what() << std::endl;
    }

    //使用logger输出的信息会被写入到文件中

    for (int i = 0; i < 100; ++i)
    {
        logger->info("write info: {} to logger file",i);
    }
}


void backtrace_test()
{
    //调试消息可以存储在环缓冲区中，而不是立即记录。
    //这仅在需要时显示调试日志很有用（例如发生错误时）。
    //在需要时，调用dump_backtrace将它们倒入日志中。

    spdlog::enable_backtrace(10); // Store the latest 32 messages in a buffer.
    // or my_logger->enable_backtrace(10)..

    for(int i = 0; i < 100; i++)
    {
        spdlog::debug("Backtrace message {}", i);
    }

    spdlog::dump_backtrace(); // log them now! show the last 32 messages
    // or my_logger->dump_backtrace(32)..
}

void level_test()
{
    auto logger = spdlog::basic_logger_mt("basic_logger", "logs/level-log.txt");

    logger->trace("spdlog level trace output");
    logger->debug("spdlog level debug output");
    logger->info("spdlog level info output");
    logger->warn("spdlog level warn output");
    logger->error("spdlog level error output");

    logger->set_level(spdlog::level::warn);

    logger->trace("spdlog level trace output");
    logger->debug("spdlog level debug output -2");
    logger->info("spdlog level info output -2 ");
    logger->warn("spdlog level warn output -2");
    logger->error("spdlog level error output -2");

}

void log_pattern_test()
{
    spdlog::info("Welcome to spdlog!");
//    spdlog::set_pattern("[%c] [pid %P][thread id: %t] [%l] %v");
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [pid %P][thread id: %t] [%l] %v");
    spdlog::info("Welcome to spdlog!");
}

void get_logger()
{

    //通过spdlog::get()获取已经创建的logger
    auto test_logger = spdlog::get("spd_logger_info");
    test_logger->info("getlogger::helloworld");
}

void create_logger()
{
//    auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/mylog.txt", 1024*1024*10, 3);
    auto my_logger = std::make_shared<spdlog::logger>("spd_logger_info", sink);
    my_logger->set_level(spdlog::level::debug);
    my_logger->warn("this should appear in both console and file");
    my_logger->info("this message should not appear in the console, only in the file");

    //需要注册了之后，其他地方才能获取
    spdlog::register_logger(my_logger);
    my_logger->info("just create a logger");
}


void multi_sink_example()
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::warn);
    console_sink->set_pattern("[multi_sink_example] [%^%l%$] %v");

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/multisink.txt", true);
    file_sink->set_level(spdlog::level::trace);

    spdlog::logger logger("multi_sink", {console_sink, file_sink});
    logger.set_level(spdlog::level::debug);
    logger.warn("this should appear in both console and file");
    logger.info("this message should not appear in the console, only in the file");
}

void multi_logger_test()
{
    auto sharedFileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/multi_logger.txt");
    auto firstLogger = std::make_shared<spdlog::logger>("loggerOne", sharedFileSink);
    auto secondLogger = std::make_shared<spdlog::logger>("loggerTwo", sharedFileSink);

    for(int i = 0; i < 10; i ++)
    {
        firstLogger->info("[loggerOne]: Hello {}.", i);
    }

    for(int j = 0; j < 10; j ++)
    {
        secondLogger->info("[loggerTwo]: Hello {}. ", j);
    }
}


int main() {

    std::cout << "hello world" << std::endl;

//    simple_output();
//    basic_logfile_example();
//    backtrace_test();
//    level_test();
//    log_pattern_test();

//    create_logger();
//    get_logger();

//    multi_sink_example();
    multi_logger_test();

    return 0;
}
