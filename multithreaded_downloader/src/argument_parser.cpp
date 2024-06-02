//argument_parser.cpp
#include <string>
#include <iostream>
#include <stdexcept>
#include "../include/argument_parser.h"

DownloadOptions ArgumentParser::parseArguments(int argc, char *argv[])
{
    DownloadOptions options;

    // 解析命令行参数
    // 这里假设命令行参数顺序为：URL 目标路径 线程数 断点续传选项（可选）
    if (argc < 3)
    {
        throw std::invalid_argument("Usage: <URL> <TargetFilePath> <NumThreads> [Resumable]");
    }

    options.url = argv[1];

    options.filePath = argv[2];

    // 设置默认值
    options.numThreads = 1;
    options.numThreads = std::stoi(argv[3]);

    options.resumable = false;
    if (argc >= 5 && std::string(argv[4]) == "Resumable")
    {
        options.resumable = true;
    }

    // 验证参数的有效性
    // 可以添加更多的验证逻辑，如检查 URL 格式是否合法，目标路径是否存在等

    return options;
}