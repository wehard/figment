#pragma once

#include "Core.h"
#include "cstdio"
#include "cstdarg"

// Virtual
class Logger
{
public:
    virtual void Log(const char *prefix, const char *message, va_list args) = 0;
    virtual ~Logger() {}
};

class BrowserConsoleLogger : public Logger
{
public:
    void Log(const char *prefix, const char *message, va_list args) override
    {
        printf("%s", prefix);
        vprintf(message, args);
        printf("\n");
    }
};

class Log
{
public:
    static void Init()
    {
        s_Logger = Figment::CreateUniquePtr<BrowserConsoleLogger>();
    }

    static void Shutdown()
    {
    }



    static void Info(const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        LogWithPrefix("[INFO]: ", format, args);
        va_end(args);
    }

    static void Warn(const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        LogWithPrefix("[WARN]: ", format, args);
        va_end(args);
    }

    static void Error(const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        LogWithPrefix("[ERROR]: ", format, args);
        va_end(args);
    }

    static void Debug(const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        LogWithPrefix("[DEBUG]: ", format, args);
        va_end(args);
    }

private:
    static Figment::UniquePtr<Logger> s_Logger;

    static void LogWithPrefix(const char *prefix, const char *format, va_list args)
    {
        s_Logger->Log(prefix, format, args);
    }
};


#define FIG_LOG_INFO(...) Log::Info(__VA_ARGS__)
#define FIG_LOG_WARN(...) Log::Warn(__VA_ARGS__)
#define FIG_LOG_ERROR(...) Log::Error(__VA_ARGS__)
#define FIG_LOG_DEBUG(...) Log::Debug(__VA_ARGS__)
