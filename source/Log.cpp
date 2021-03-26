#include "Log.h"

#include <iostream>
#include <fstream>
#include <dirent.h>

#include "SavefileIO.h"

void LogInit()
{
    // Create main dir
    if (!SavefileIO::DirectoryExists("sdmc:/switch/botw-unexplored"))
        mkdir("sdmc:/switch/botw-unexplored", 0777);
        
    // Clear the log
    std::ofstream file("sdmc:/switch/botw-unexplored/log.txt", std::ofstream::out | std::ofstream::trunc);
    file.close();
}

void Log(std::string thing)
{
    LogSave(thing);
}

void Log(std::string thing, std::string arg)
{
    std::string joined = thing + " " + arg;
    LogSave(joined);
}

void Log(std::string thing, int arg)
{
    std::string joined = thing + " " + std::to_string(arg);
    LogSave(joined);
}

void Log(std::string thing, float arg)
{
    std::string joined = thing + " " + std::to_string(arg);
    LogSave(joined);
}

void Log(int thing)
{
    LogSave(std::to_string(thing));
}

void Log(float thing)
{
    LogSave(std::to_string(thing));
}

void LogSave(std::string thing)
{
    // Print to nxlink console
    std::cout << thing << "\n";

    std::ofstream file("sdmc:/switch/botw-unexplored/log.txt", std::ios::app); // Open file at end
    if (file.is_open())
        file << thing << "\n";

    file.close();
}