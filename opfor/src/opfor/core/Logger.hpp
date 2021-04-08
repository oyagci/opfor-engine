#pragma once

#include <fmt/format.h>
#include <string_view>
#include <unordered_map>

class Logger
{
  public:
    enum class LogLevel
    {
        Verbose = 0,
        Info = 1,
        Debug = 2,
        Warning = 3,
        Error = 4
    };

    using Callback = std::function<void(LogLevel, std::string_view)>;

  private:
    std::unordered_map<std::string, Callback> _callbacks;
    LogLevel _logLevel = LogLevel::Verbose;

    Logger()
    {
        _callbacks["stdout"] = &StdoutLog;
    }

    template <auto Severity, typename... Args> void Log(std::string_view const format, Args... args)
    {
        if (Severity < _logLevel)
        {
            return;
        }

        fmt::memory_buffer buff;
        fmt::format_to(buff, "[{:s}] ", Severity);
        fmt::format_to(buff, format, std::forward<Args>(args)...);
        fmt::format_to(buff, "\n");

        for (const auto &[_, cb] : _callbacks)
        {
            cb(Severity, {buff.data(), buff.size()});
        }
    }

    static void StdoutLog(LogLevel const severity, std::string_view const data)
    {
        auto *sout = severity >= LogLevel::Warning ? stderr : stdout;
        fmt::print(sout, data);
    }

  public:
    Logger(Logger &) = delete;
    void operator=(Logger &) = delete;

    static Logger &Instance()
    {
        static Logger logger;
        return logger;
    }

    template <typename... Args> static void Verbose(std::string_view const format, Args... args)
    {
        Instance().Log<LogLevel::Verbose>(format, std::forward<Args>(args)...);
    }

    template <typename... Args> static void Info(std::string_view const format, Args... args)
    {
        Instance().Log<LogLevel::Info>(format, std::forward<Args>(args)...);
    }

    template <typename... Args> static void Debug(std::string_view const format, Args... args)
    {
        Instance().Log<LogLevel::Debug>(format, std::forward<Args>(args)...);
    }

    template <typename... Args> static void Warn(std::string_view const format, Args... args)
    {
        Instance().Log<LogLevel::Warning>(format, std::forward<Args>(args)...);
    }

    template <typename... Args> static void Error(std::string_view const format, Args... args)
    {
        Instance().Log<LogLevel::Error>(format, std::forward<Args>(args)...);
    }

    static void BindCallback(std::string const &name, Callback cb)
    {
        auto &cbs = Instance()._callbacks;
        if (cbs.find(name) != std::end(cbs))
        {
            Warn("Logger {} already bound.", name);
        }
        cbs[name] = cb;
        Info("Logger \"{:s}\" enabled.", name);
    }

    static void UnBindCallback(std::string const &name)
    {
        auto &cbs = Instance()._callbacks;
        auto const pos = cbs.find(name);
        if (pos != std::end(cbs))
        {
            cbs.erase(pos);
            Info("Logger \"{:s}\" disabled.", name);
        }
    }

    static LogLevel GetLogLevel()
    {
      return Instance()._logLevel;
    }

    static void SetLogLevel(LogLevel lvl)
    {
        Instance()._logLevel = lvl;
    }
};

template <> struct fmt::formatter<Logger::LogLevel> : formatter<string_view>
{
    template <typename FormatContext> auto format(Logger::LogLevel c, FormatContext &ctx)
    {
        string_view name = "unknown";
        switch (c)
        {
        case Logger::LogLevel::Verbose:
            name = "Verbose";
            break;
        case Logger::LogLevel::Info:
            name = "Info";
            break;
        case Logger::LogLevel::Debug:
            name = "Debug";
            break;
        case Logger::LogLevel::Warning:
            name = "Warning";
            break;
        case Logger::LogLevel::Error:
            name = "Error";
            break;
        }
        return formatter<string_view>::format(name, ctx);
    }
};
