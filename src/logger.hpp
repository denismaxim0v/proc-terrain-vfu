#pragma once
#include <iostream>
#include <string>

enum class LogLevel
{
    Info,
    Warning,
    Error
};

inline void Log(LogLevel level, const std::string& msg)
{
    switch (level)
    {
    case LogLevel::Info:
        std::cout << "[INFO] " << msg << "\n";
        break;

    case LogLevel::Warning:
        std::cout << "[WARN] " << msg << "\n";
        break;

    case LogLevel::Error:
        std::cerr << "[ERROR] " << msg << "\n";
        break;
    }
}