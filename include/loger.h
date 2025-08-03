#pragma once

#include <mutex>
#include <iostream>
#include <string>
#include <format>
#include <chrono>


#define LOG_DEBUG_MSG(msg)   Logger::instance().log(Logger::Level::DEBUG,   msg)
#define LOG_INFO_MSG(msg)    Logger::instance().log(Logger::Level::INFO,    msg)
#define LOG_WARN_MSG(msg)    Logger::instance().log(Logger::Level::WARNING, msg)
#define LOG_ERROR_MSG(msg)   Logger::instance().log(Logger::Level::ERROR,   msg)

class Logger {
public:
    enum class Level { DEBUG, INFO, WARNING, ERROR };
    
    static Logger& instance() {
        static Logger logger;
        return logger;
    }

    void setLevel(Level level) { level_ = level; }

    void log(Level level, const std::string& message) {
        if (level < level_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        std::cerr << std::format("[{}] {}\n", levelToString(level), message);
    }

private:
    Logger() = default;
    std::mutex mutex_;
    Level level_ = Level::INFO;

    static std::string_view levelToString(Level level) {
        switch(level) {
            case Level::DEBUG:   return "DEBUG";
            case Level::INFO:    return "INFO";
            case Level::WARNING: return "WARN";
            case Level::ERROR:   return "ERROR";
            default:             return "UNKNOWN";
        }
    }
};
