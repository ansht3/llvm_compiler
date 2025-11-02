#include "utils/Logger.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>

void Logger::log(LogLevel level, const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::string prefix;
    switch (level) {
        case LogLevel::DEBUG:
            prefix = "[DEBUG]";
            break;
        case LogLevel::INFO:
            prefix = "[INFO]";
            break;
        case LogLevel::WARNING:
            prefix = "[WARN]";
            break;
        case LogLevel::ERROR:
            prefix = "[ERROR]";
            break;
    }
    
    std::cout << prefix << " " << message << std::endl;
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warning(const std::string& message) {
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

