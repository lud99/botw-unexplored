#pragma once

#include <string>

void LogInit();

void Log(std::string thing);
void Log(std::string thing, std::string arg);
void Log(std::string thing, int arg);
void Log(std::string thing, float arg);
void Log(int thing);
void Log(float thing);

void LogSave(std::string thing);