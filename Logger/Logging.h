#pragma once

#include <string>

#ifdef LOGGING_LIBRARY_EXPORT
#define LOGGING_CLASS_EXPORT __declspec(dllexport)
#else
#define LOGGING_CLASS_EXPORT __declspec(dllimport)
#endif

class LOGGING_CLASS_EXPORT Logging
{
public:
	enum LOG_LEVEL{L_FATAL = 0, L_ERROR, L_WARN, L_INFO, L_DEBUG};

private:
	std::string m_strLogInstanceName;
	std::string m_strLogFilePathName;

public:
	Logging(const std::string& strLogInstanceName, const std::string& strLogFilePathName);
	~Logging(void);

	void log(LOG_LEVEL level, const char* stringFormat, ...);
};

LOGGING_CLASS_EXPORT Logging* CreateLoggerInstance(const char* const strLogInstanceName, const char* const strLogFilePathName);
LOGGING_CLASS_EXPORT Logging* GetLoggerInstance();
LOGGING_CLASS_EXPORT void DestroyLoggerInstance(Logging* pLoggerInstance);

#define LOG_(log_level, str) \
{\
	GetLoggerInstance()->log(log_level, str);\
}

#define LOG_FORMAT_(log_level, format, ...) \
{\
	GetLoggerInstance()->log(log_level, format, ...);\
}

#define LOG_FATAL(str)					LOG_(Logging::L_FATAL, str)
#define LOG_FATAL_FORMAT(format, ...)	LOG_(Logging::L_FATAL, format, ...)

#define LOG_ERROR(str)					LOG_(Logging::L_ERROR, str)
#define LOG_ERROR_FORMAT(format, ...)	LOG_(Logging::L_ERROR, format, ...)

#define LOG_WARN(str)					LOG_(Logging::L_WARN, str)
#define LOG_WARN_FORMAT(format, ...)	LOG_(Logging::L_WARN, format, ...)

#define LOG_INFO(str)					LOG_(Logging::L_INFO, str)
#define LOG_INFO_FORMAT(format, ...)	LOG_(Logging::L_INFO, format, ...)

#define LOG_DEBUG(str)					LOG_(Logging::L_DEBUG, str)
#define LOG_DEBUG_FORMAT(format, ...)	LOG_(Logging::L_DEBUG, format, ...)

