#pragma once

#include <string>
#include <fmt/format.h>

class Logger
{
private:
	Logger()
	{
		_log.resize(100);
	}

	enum class LogLevel
	{
		Verbose = 0,
		Info = 1,
		Debug = 2,
		Warning = 3,
		Error = 4
	};

	LogLevel _logLevel = LogLevel::Verbose;

	std::vector<size_t> _lineOffsets;
	std::string _log;

private:
	template <typename ... Args>
	void Log(LogLevel severity, std::string const &format, Args... args)
	{
		if (severity < _logLevel) {
			return ;
		}

		std::string finalString = "";

		switch (severity) {
			case LogLevel::Verbose:
				finalString += "[Verbose] ";
				break ;
			case LogLevel::Info:
				finalString += "[Info] ";
				break ;
			case LogLevel::Debug:
				finalString += "[Debug] ";
				break ;
			case LogLevel::Warning:
				finalString += "[Warning] ";
				break ;
			case LogLevel::Error:
				finalString += "[Error] ";
				break ;
		}

		finalString += format;

		size_t oldSize = _log.size();

		// Format log entry
		finalString = fmt::format(finalString, std::forward<Args>(args)...);

		// Add new log entry to the log string
		_log += finalString;

		// For each line added to the log, save where the line starts in the log string
		for (size_t newSize = _log.size(); oldSize < newSize; oldSize++) {
			if (_log[oldSize] == '\n') {
				_lineOffsets.push_back(oldSize + 1);
			}
		}
	}

public:
	Logger(Logger &) = delete;
	void operator=(Logger &) = delete;

	static Logger &Instance()
	{
		static Logger logger;
		return logger;
	}

	template <typename ... Args>
	static void Verbose(std::string const &format, Args... args)
	{
		Instance().Log(LogLevel::Verbose, format, std::forward<Args>(args)...);
	}

	template <typename ... Args>
	static void Info(std::string const &format, Args... args)
	{
		Instance().Log(LogLevel::Info, format, std::forward<Args>(args)...);
	}

	template <typename ... Args>
	static void Debug(std::string const &format, Args... args)
	{
		Instance().Log(LogLevel::Debug, format, std::forward<Args>(args)...);
	}

	template <typename ... Args>
	static void Warn(std::string const &format, Args... args)
	{
		Instance().Log(LogLevel::Warning, format, std::forward<Args>(args)...);
	}

	template <typename ... Args>
	static void Error(std::string const &format, Args... args)
	{
		Instance().Log(LogLevel::Error, format, std::forward<Args>(args)...);
	}

	static std::string const &GetLog()
	{
		return Instance()._log;
	}

	static std::vector<size_t> const &GetLineOffsets()
	{
		return Instance()._lineOffsets;
	}
};
