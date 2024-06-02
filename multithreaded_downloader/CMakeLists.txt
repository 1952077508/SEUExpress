cmake_minimum_required(VERSION 3.5)
project(multithreaded_downloader)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加头文件目录
include_directories(include src)

# 添加源文件
set(SOURCES 
    src/argument_parser.cpp
    # src/download_manager.cpp
    # src/network_module.cpp
    # src/file_module.cpp
    # src/thread_pool.cpp
    main.cpp
)

# 生成可执行文件
add_executable(downloader ${SOURCES})
