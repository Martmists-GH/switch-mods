#include <string>
#include "logger.h"

static Logger logger; // do not move inside, something something concurrency
Logger &Logger::instance() {
    return logger;
}

void Logger::log(const std::string &fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log(fmt.c_str(), args);
    va_end(args);
}

void Logger::log(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log(fmt, args);
    va_end(args);
}

void Logger::log(const char *fmt, va_list args) {
    va_list args_copy;
    va_copy(args_copy, args);

    auto size = vsnprintf(nullptr, 0, fmt, args_copy);
    char* buffer = (char*)malloc(size + 1);
    vsnprintf(buffer, size + 1, fmt, args);
    buffer[size] = 0;
    for (const auto& listener : instance().mListeners) {
        listener(buffer, size);
    }
    free(buffer);
}

void Logger::addListener(const LogCallback &callback) {
    instance().mListeners.push_back(callback);
}
