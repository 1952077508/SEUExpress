#ifndef ARGUMENT_PARSER_H
#define ARGUMENT_PARSER_H

#include <string>
#include <stdexcept>

struct DownloadOptions {
    std::string url;
    std::string filePath;
    int numThreads;
    bool resumable;
};

class ArgumentParser {
public:
    static DownloadOptions parseArguments(int argc, char* argv[]);
};

int test_argument_parser(int argc, char* argv[]);
#endif // ARGUMENT_PARSER_H
