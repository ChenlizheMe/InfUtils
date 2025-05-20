#pragma once

#include <iostream>
#include <mutex>
#include <sstream>

namespace infutils
{
enum LogLevel
{
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARN = 2,
    LOG_ERROR = 3,
    LOG_FATAL = 4
};

class InfLog
{
  public:
    static InfLog &GetInstance()
    {
        static InfLog instance;
        return instance;
    }

    template <typename... Args>
    void Log(LogLevel level, const char *file, int line, Args &&...args)
    {
        if (logLevel > level)
            return;

        std::ostringstream oss;
        (oss << ... << args);

        std::lock_guard<std::mutex> lock(mutex_);
        std::cout << LogLevelToColor(level) << "[" << LogLevelToString(level)
                  << "] "
                  << "(" << file << ":" << line << ") " << oss.str()
                  << "\033[0m" << std::endl;
    }

    void SetLogLevel(int level)
    {
        std::cout << "Log level set to: "
                  << LogLevelToString(static_cast<LogLevel>(level))
                  << std::endl;
        logLevel = level;
    }

    int GetLogLevel() const { return logLevel; }

  private:
    InfLog() { logLevel = LOG_DEBUG; }
    InfLog(const InfLog &) = delete;
    InfLog &operator=(const InfLog &) = delete;

    std::mutex mutex_;

    int logLevel;

    const char *LogLevelToString(LogLevel level)
    {
        switch (level) {
        case LOG_DEBUG:
            return "DEBUG";
        case LOG_INFO:
            return "INFO";
        case LOG_WARN:
            return "WARN";
        case LOG_ERROR:
            return "ERROR";
        case LOG_FATAL:
            return "FATAL";
        default:
            return "UNKNOWN";
        }
    }

    const char *LogLevelToColor(LogLevel level)
    {
        switch (level) {
        case LOG_DEBUG:
            return "\033[36m"; // 浅蓝色 (cyan)
        case LOG_INFO:
            return "\033[37m"; // 白色
        case LOG_WARN:
            return "\033[33m"; // 黄色
        case LOG_ERROR:
            return "\033[31m"; // 红色
        case LOG_FATAL:
            return "\033[35m"; // 洋红
        default:
            return "\033[0m"; // 默认颜色
        }
    }
};
} // namespace infutils

#define INFLOG_INTERNAL(level, ...)                                    \
    do {                                                               \
        if ((level) >= infutils::InfLog::GetInstance().GetLogLevel())  \
            infutils::InfLog::GetInstance().Log(                       \
                static_cast<infutils::LogLevel>(level),                \
                __FILE__, __LINE__, __VA_ARGS__);                      \
    } while (false)

#define INFLOG_DEBUG(...)  INFLOG_INTERNAL(infutils::LOG_DEBUG,  __VA_ARGS__)
#define INFLOG_INFO(...)   INFLOG_INTERNAL(infutils::LOG_INFO,   __VA_ARGS__)
#define INFLOG_WARN(...)   INFLOG_INTERNAL(infutils::LOG_WARN,   __VA_ARGS__)
#define INFLOG_ERROR(...)  INFLOG_INTERNAL(infutils::LOG_ERROR,  __VA_ARGS__)
#define INFLOG_FATAL(...)                                              \
    do { INFLOG_INTERNAL(infutils::LOG_FATAL, __VA_ARGS__); std::abort(); } while(false)

#define INFLOG_SET_LEVEL(level)                                                \
    infutils::InfLog::GetInstance().SetLogLevel(level)

#define INFLOG_GET_LEVEL() infutils::InfLog::GetInstance().GetLogLevel()