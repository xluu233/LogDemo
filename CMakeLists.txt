cmake_minimum_required(VERSION 3.25)
project(LogDemo)

set(CMAKE_CXX_STANDARD 17)

# 往工程中添加spdlog库，这个库是一个interface类型的库
#add_library(spdlog INTERFACE)

# 包含这个interface库的工程路径
#target_include_directories(
#        spdlog
#        INTERFACE
#        "spdlog/include"
#)

include_directories(${PROJECT_SOURCE_DIR}/spdlog/include)
link_directories(${PROJECT_SOURCE_DIR}/libs)

add_executable(LogDemo main.cpp)
target_link_libraries(LogDemo
        ${PROJECT_SOURCE_DIR}/libs/spdlogd.lib)
