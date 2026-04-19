#pragma once
#include <iostream>

enum class LogLevel
{
    Info,
    Warning,
    Error
};

template<typename... Args>
inline void Log(LogLevel level, Args&&... args)
{
    std::ostream& out = (level == LogLevel::Error) ? std::cerr : std::cout;

    switch (level)
    {
    case LogLevel::Info:
        out << "[INFO] ";
        break;
    case LogLevel::Warning:
        out << "[WARN] ";
        break;
    case LogLevel::Error:
        out << "[ERROR] ";
        break;
    }

    (out << ... << args) << "\n"; // fold expression (C++17)
}