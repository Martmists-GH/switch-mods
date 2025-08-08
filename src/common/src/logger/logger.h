#pragma once

#include <functional>
#include <cstdarg>
#include <string>

typedef std::function<void(const char *, size_t)> LogCallback;

#ifndef LOGGER_CAPACITY
#define LOGGER_CAPACITY 5
#endif

class Logger {
    int useIndex = 0;
    std::array<LogCallback, LOGGER_CAPACITY> mListeners;

    static Logger &instance();

public:
    static void log(const std::string &fmt, ...);
    static void log(const char *fmt, ...);
    static void log(const char *fmt, va_list args);

    static void addListener(const LogCallback &callback);

    [[deprecated]] Logger() = default;
};
