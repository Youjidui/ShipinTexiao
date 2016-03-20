#pragma once

#include "LoggingExportMacros.h"

LOGGING_CLASS_EXPORT void LogWindowsError(LPCTSTR lpszFunction);
LOGGING_CLASS_EXPORT void LogDirectXError(LPCTSTR lpszFunction, HRESULT hr);

#ifdef _UNICODE
#define __FUNCTION_NAME__ __FUNCTIONW__
#else
#define __FUNCTION_NAME__ __FUNCTION__
#endif

#define CHECK_AND_LOG_WINDOWS_API_ERROR() \
do {\
if(0 != GetLastError())\
{\
	LogWindowsError(__FUNCTION_NAME__);\
	ASSERT(false);\
}\
} while (0)

#define CHECK_AND_LOG_DIRECTX_API_ERROR(HRESULT_VALUE) \
do {\
if(FAILED(HRESULT_VALUE))\
{\
	LogDirectXError(__FUNCTION_NAME__, HRESULT_VALUE);\
	ASSERT(false);\
}\
} while (0)
