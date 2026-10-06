#include "Log.h"

#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace
{
    bool logLevelSet = false;

    bool globalUseColor = true;
    bool colorSet = false;

    bool CanUseColor()
    {
#ifdef _WIN32
        DWORD dwMode = 0;
        const auto stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        GetConsoleMode(stdoutHandle, &dwMode);
        if (!(dwMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING))
        {
            SetConsoleMode(stdoutHandle, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
            dwMode = 0;
            GetConsoleMode(stdoutHandle, &dwMode);
            if (!(dwMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING))
                return false;
        }

        const auto stderrHandle = GetStdHandle(STD_ERROR_HANDLE);
        GetConsoleMode(stderrHandle, &dwMode);
        if (!(dwMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING))
        {
            SetConsoleMode(stderrHandle, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
            dwMode = 0;
            GetConsoleMode(stderrHandle, &dwMode);
            if (!(dwMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING))
                return false;
        }
#endif

        return true;
    }
} // namespace

namespace con
{
    LogLevel _globalLogLevel = LogLevel::INFO;
    std::atomic_size_t _warningCount(0);
    std::atomic_size_t _errorCount(0);

    void init()
    {
        if (!logLevelSet)
            set_log_level(LogLevel::INFO);

        if (!colorSet)
            set_use_color(true);
    }

    void set_log_level(const LogLevel value)
    {
        logLevelSet = true;
        _globalLogLevel = value;
    }

    void set_use_color(const bool value)
    {
        colorSet = true;
        globalUseColor = value && CanUseColor();
    }

    void reset_counts()
    {
        _warningCount = 0;
        _errorCount = 0;
    }

    size_t warning_count()
    {
        return _warningCount;
    }

    size_t error_count()
    {
        return _errorCount;
    }

    void _debug_internal(const std::string& str)
    {
        if (globalUseColor)
            std::cout << "\x1B[90m" << str << "\x1B[0m\n";
        else
            std::cout << str << '\n';
    }

    void _info_internal(const std::string& str)
    {
        if (globalUseColor)
            std::cout << "\x1B[37m" << str << "\x1B[0m\n";
        else
            std::cout << str << '\n';
    }

    void _warn_internal(const std::string& str)
    {
        if (globalUseColor)
            std::cout << "\x1B[33mWARN: " << str << "\x1B[0m\n";
        else
            std::cout << "WARN: " << str << '\n';
    }

    void _error_internal(const std::string& str)
    {
        if (globalUseColor)
            std::cerr << "\x1B[31mERROR: " << str << "\x1B[0m\n";
        else
            std::cerr << "ERROR: " << str << '\n';
    }
} // namespace con
