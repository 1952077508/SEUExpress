#ifndef MY_TEST_H
#define MY_TEST_H
#include "argument_parser.h"
#include <iostream>


void test_ArgumentParser()
{
    std::cout << "Testing ArgumentParser class..." << std::endl;
    int temp_argc = 5;
    char *temp_argv[] = {
        (char *)"test.exe",
        (char *)"https://www.google.com",
        (char *)"./downloaded_file",
        (char *)"4",
        (char *)"Resumable"};
    try
    {
        DownloadOptions options = ArgumentParser::parseArguments(temp_argc, temp_argv);
        std::cout << "URL: " << options.url << std::endl;
        std::cout << "Target File Path: " << options.filePath << std::endl;
        std::cout << "Number of Threads: " << options.numThreads << std::endl;
        std::cout << "Resumable: " << (options.resumable ? "Yes" : "No") << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return;
    }
    std::cout << "ArgumentParser test passed." << std::endl;
}

void testall()
{
    test_ArgumentParser();
}

#endif
