# LogDemo
 C++日志库demo

## 背景

之前做开发基本是使用Qt, Qt中提供了qDebug作为输出调试工具，但是其明显功能有限，需要自行封装一套工具类，最近考虑换成主流的日志工具，在参考了网上很多开源库后，发现spdlog和glog似乎还挺不错的。

- [spdlog](https://github.com/gabime/spdlog): 达到20k+的C++日志库，而且一直在维护
- [glog](https://github.com/google/glog)：google开源的日志库，我对google的东西一直有着迷之自信

如果是对于其他语言来说，我更愿意自行封装一套工具类，在其中提供接口，接口的实际调用使用开源库来实现。这样的好处是：
- 统一log风格
- 在日后换用其他开源库的时候只需要替换工具类中接口，不会造成整个项目的改动


但是对于`C++`这门语言来说，我觉得没有必要：`C++`项目一旦选型，很难有机会再换用其他技术框架。每个库之间的风格差异和功能都很大， 自行封装一层接口的成本也挺高的。

- [spdlog--log4cxx有点笨重，试一试spdlog](https://blog.csdn.net/bandaoyu/article/details/110730431)

## spdlog

**spdlog作为C++主流开源日志库，有如下特性：**

- 性能好
- 只包含头文件
- 无需依赖第三方库
- 支持跨平台 Linux / Windows / Android / MacOS
- 支持多线程
- 可对日志文件进行循环输出
- 可每日生成日志文件
- 支持控制台日志输出
- 可选的异步日志
- 支持日志输出级别
- 可自定义日志格式
- 兼容性好，支持C++11


### 编译

spdlog的示例cmakelist中展示了两种方式，静态库链接和包含头文件路径，具体使用那种看项目需求。

#### 方式一
最简单的方式就是直接将spdlog源码放在项目目录下，cmake配置如下：

```

# 往工程中添加spdlog库，这个库是一个interface类型的库
add_library(spdlog INTERFACE)

# 包含这个interface库的工程路径
target_include_directories(
        spdlog
        INTERFACE
        "spdlog/include"
)


# 在cmake最后链接上去就可以了
target_link_libraries(xxx spdlog)
```
> 这种就是header only方式，编译简单，但是这样有个缺点，每次改动都会造成重新编译，艹

#### 方式二

在Windows和Linux下编译的方式是不同的

- Windows + MSVC + SisualStudio
- Linux中编译 && 安装


我这里使用的是Windows环境，编译工具是Cmake+VS, 这一套东西应该很成熟了，就不过多介绍了，总之最后生成.lib，就是windows平台的静态库。

开发用的Clion + MSVC，在项目中新增libs文件夹，将生成的.lib文件拷贝过来即可，cmake配置如下：
```
include_directories(${PROJECT_SOURCE_DIR}/spdlog/include)
link_directories(${PROJECT_SOURCE_DIR}/libs)
```

如果想换用MinGW, 可以自行去折腾....  Linux配置应该也是一样的，只是换成.a文件

### 简单示例

```
#include <iostream>
#include "spdlog/spdlog.h"

int main() {

    std::cout << "hello world" << std::endl;

    spdlog::info("Welcome to spdlog!");
    spdlog::error("Some error message with arg: {}", 1);

    spdlog::warn("Easy padding in numbers like {:08d}", 12);
    spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
    spdlog::info("Support for floats {:03.2f}", 1.23456);
    spdlog::info("Positional args are {1} {0}..", "too", "supported");
    spdlog::info("{:<30}", "left aligned");

    return 0;
}

```

输出：
```
hello world
[2023-12-07 16:58:41.121] [info] Welcome to spdlog!
[2023-12-07 16:58:41.122] [error] Some error message with arg: 1
[2023-12-07 16:58:41.122] [warning] Easy padding in numbers like 00000012
[2023-12-07 16:58:41.122] [critical] Support for int: 42;  hex: 2a;  oct: 52; bin: 101010
[2023-12-07 16:58:41.122] [info] Support for floats 1.23
[2023-12-07 16:58:41.122] [info] Positional args are supported too..
[2023-12-07 16:58:41.122] [info] left aligned       

```

从上述的输出信息我们可以和std::cout标准库不一样的地方：

- 时间tag信息
- 日志等级：warn / debug / info 等等
- 不同数据类型和格式的输出方式


### Sink + Logger
spdlog 是由Sink + Logger两部分组成，是整个组件的核心部分。

**Logger**
- 一个logger中存储了多个sink，也就是logger和sink是一对多的关系，也可以是多对多的关系
- 当调用logger的日志输出函数时，logger会调用自身存储的所有sink对象的log(log_msg) 函数进行输出。

**Sink**
- 可以理解为一个日志文件，一个日志输出窗口，sink是实际写入日志的对象，每个 sink 应该只负责一个日志输出配置（例如文件、控制台、数据库），并且每个 Sinks 都有自己的 formatter 对象的私有实例。
 - sink 有 _mt 多线程 和 _st 单线程之分；单线程不能在多线程中使用。

> 理解了sink和logger才能更好的使用spdlog
 
#### 日志级别

```
enum level_enum
{
    trace = SPDLOG_LEVEL_TRACE,         // 描述事件的日志级别，显示代码的逐步执行，在标准操作期间可以忽略。『最低级』
    debug = SPDLOG_LEVEL_DEBUG,         // 当需要更详细的信息时，使用此调试信息
    info = SPDLOG_LEVEL_INFO,           // 突出强调应用程序的运行过程中，输出一些提示信息
    warn = SPDLOG_LEVEL_WARN,           // 应用程序内部发生了意外行为，但它仍在继续工作，关键业务功能按预期运行。
    err = SPDLOG_LEVEL_ERROR,           // 一项或多项功能无法正常工作，导致某些功能无法正常工作。
    critical = SPDLOG_LEVEL_CRITICAL,   // 一项或多项关键业务功能不起作用，整个系统无法实现业务功能。
    off = SPDLOG_LEVEL_OFF,             // 用于关闭所有日志记录。『最高级』
    n_levels
};
```

日志级别 Log Level 相关，通常只用到 debug、info、err 这三个级别比较多。

logger默认创建是Info级别，也就是info之上的级别才能输出，我们通过set_level函数设置其输出等级：
```
void level_test()
{
    auto logger = spdlog::basic_logger_mt("basic_logger", "logs/level-log.txt");

    logger->trace("spdlog level trace output");
    logger->debug("spdlog level debug output");
    logger->info("spdlog level info output");
    logger->warn("spdlog level warn output");
    logger->error("spdlog level error output");

    //在设置warn级别之后，warn以上的级别都不再输出。
    logger->set_level(spdlog::level::warn);

    logger->trace("spdlog level trace output");
    logger->debug("spdlog level debug output -2");
    logger->info("spdlog level info output -2 ");
    logger->warn("spdlog level warn output -2");
    logger->error("spdlog level error output -2");

}

```
输出为：
```
[2023-12-07 18:47:13.822] [basic_logger] [info] spdlog level info output
[2023-12-07 18:47:13.824] [basic_logger] [warning] spdlog level warn output
[2023-12-07 18:47:13.824] [basic_logger] [error] spdlog level error output
[2023-12-07 18:47:13.824] [basic_logger] [warning] spdlog level warn output -2
[2023-12-07 18:47:13.824] [basic_logger] [error] spdlog level error output -2
```

#### 输出格式pattern
通过set_pattern可设定日志格式，如set_pattern("[%Y-%m-%d %H:%M:%S.%e][%l](%@): %v");

比如默认的就是`[2023-12-07 18:47:13.824]`带有这样的时间信息

```
void log_pattern_test()
{
    spdlog::info("Welcome to spdlog!");
    spdlog::set_pattern("[%c] [pid %P][thread id: %t] [%l] %v");
    spdlog::info("Welcome to spdlog!");
}
```
输出：
```
[2023-12-07 18:55:51.512] [info] Welcome to spdlog!
[Thu Dec 7 18:55:51 2023] [pid 32564][thread id: 24252] [info] Welcome to spdlog!
```
可以看到前面的tag发生了一些改变

具体的输出格式可以参考：https://blog.csdn.net/alwaysrun/article/details/122771208

#### 写入文件
```
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
```

上述创建了一个`std::shared_ptr<spdlog::logger>`对象，使用此对象输出的log信息都会被写入到设定的文件中。

**除此之外还有daily files，就是根据事件每天都会写入一个新的文件**
```
#include "spdlog/sinks/daily_file_sink.h"
void daily_example()
{
    // Create a daily logger - a new file is created every day at 2:30 am
    auto logger = spdlog::daily_logger_mt("daily_logger", "logs/daily.txt", 2, 30);
}
```

**循环文件：设置文件大小，超出之后会覆盖**

```
#include "spdlog/sinks/rotating_file_sink.h"
void rotating_example()
{
    // Create a file rotating logger with 5mb size max and 3 rotated files
    auto max_size = 1024*1024 * 5;
    auto max_files = 3;
    auto logger = spdlog::rotating_logger_mt("some_logger_name", "logs/rotating.txt", max_size, max_files);
}
```

#### Backtrace support
回溯支持？我没太理解，根据其给出的示例，就是将log信息缓存起来了并不是马上显示，而是在需要的时候才显示出来。

比如：
```
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
```

输出结果：
```
[2023-12-07 18:24:40.392] [info] ****************** Backtrace Start ******************
[2023-12-07 18:24:40.392] [debug] Backtrace message 90
[2023-12-07 18:24:40.392] [debug] Backtrace message 91
[2023-12-07 18:24:40.392] [debug] Backtrace message 92
[2023-12-07 18:24:40.392] [debug] Backtrace message 93
[2023-12-07 18:24:40.392] [debug] Backtrace message 94
[2023-12-07 18:24:40.392] [debug] Backtrace message 95
[2023-12-07 18:24:40.392] [debug] Backtrace message 96
[2023-12-07 18:24:40.392] [debug] Backtrace message 97
[2023-12-07 18:24:40.392] [debug] Backtrace message 98
[2023-12-07 18:24:40.392] [debug] Backtrace message 99
[2023-12-07 18:24:40.397] [info] ****************** Backtrace End **************
```

#### 创建自定义logger

```
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

```


#### 一个logger关联多个sink
```
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

```

#### 一个sink关联多个sink
```
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

```

### 最后

更多示例参考官网[example](https://github.com/gabime/spdlog)

以上代码请参考：[LogDemo](https://github.com/xluu233/LogDemo)
