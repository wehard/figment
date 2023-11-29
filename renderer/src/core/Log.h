#pragma once

#include "Core.h"

// Virtual
class Logger
{
public:
    virtual void Log(const char *message, ...) = 0;
};

class BrowserConsoleLogger : public Logger
{
public:
    void Log(const char *message, ...) override
    {
        va_list args;
        va_start(args, message);
        emscripten_log_vprintf(EM_LOG_CONSOLE, message, args);
        va_end(args);
    }
};

class Log
{
public:
    static void Init();

    static void Shutdown();

    static void Info(const char *message, ...);

    static void Warn(const char *message, ...);

    static void Error(const char *message, ...);

    static void Debug(const char *message, ...);
};
