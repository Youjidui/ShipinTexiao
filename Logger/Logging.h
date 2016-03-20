#pragma once

#include <string>
#include "LoggingExportMacros.h"

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

	void setLogLevel(LOG_LEVEL level);
	void log(LOG_LEVEL level, const char* stringFormat, ...);
};

LOGGING_CLASS_EXPORT Logging* CreateLoggerInstance(const char* const strLogInstanceName, const char* const strLogFilePathName);
LOGGING_CLASS_EXPORT void SetLoggerOutputLevel(Logging* pLoggerInstance, Logging::LOG_LEVEL);
LOGGING_CLASS_EXPORT Logging* GetLoggerInstance();
LOGGING_CLASS_EXPORT void DestroyLoggerInstance(Logging* pLoggerInstance);

#define LOG_(log_level, str) \
{\
	GetLoggerInstance()->log(log_level, str);\
}

#define LOG_FORMAT_(log_level, format, ...) \
do{\
	GetLoggerInstance()->log(log_level, format, ##__VA_ARGS__);\
}while(0)

#define LOG_FATAL(str)					LOG_(Logging::L_FATAL, str)
#define LOG_FATAL_FORMAT(format, ...)	LOG_FORMAT_(Logging::L_FATAL, format, ##__VA_ARGS__)

#define LOG_ERROR(str)					LOG_(Logging::L_ERROR, str)
#define LOG_ERROR_FORMAT(format, ...)	LOG_FORMAT_(Logging::L_ERROR, format, ##__VA_ARGS__)

#define LOG_WARN(str)					LOG_(Logging::L_WARN, str)
#define LOG_WARN_FORMAT(format, ...)	LOG_FORMAT_(Logging::L_WARN, format, ##__VA_ARGS__)

#define LOG_INFO(str)					LOG_(Logging::L_INFO, str)
#define LOG_INFO_FORMAT(format, ...)	LOG_FORMAT_(Logging::L_INFO, format, ##__VA_ARGS__)

#define LOG_DEBUG(str)					LOG_(Logging::L_DEBUG, str)
#define LOG_DEBUG_FORMAT(format, ...)	LOG_FORMAT_(Logging::L_DEBUG, format, ##__VA_ARGS__)

