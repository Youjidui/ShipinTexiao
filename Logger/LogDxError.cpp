#include "StdAfx.h"
#include "LogDxError.h"
#include <strsafe.h>
#include "Logging.h"

#include <DxErr.h>
#pragma comment(lib, "DxErr")
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi")

#pragma warning(disable:4995)
#pragma warning(disable:4996)

LPTSTR WindowsErrorMessage(LPCTSTR lpszFunction)	//caller needs to delete the returned buffer
{ 
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf = NULL;
	LPTSTR lpDisplayBuf = NULL;
	DWORD dw = GetLastError(); 

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL );

	// Display the error message and exit the process

	//lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
	//	(lstrlen((LPCTSTR)lpMsgBuf)+lstrlen(lpszFunction)+40)*sizeof(TCHAR)); 
	//StringCchPrintf((LPTSTR)lpDisplayBuf, 
	//	LocalSize(lpDisplayBuf),
	//	TEXT("%s failed with error %d: %s"), 
	//	lpszFunction, dw, lpMsgBuf); 
	const int size = lstrlen((LPCTSTR)lpMsgBuf)+lstrlen(lpszFunction)+40;
	lpDisplayBuf = new TCHAR[size];
	StringCchPrintf(lpDisplayBuf, 
		size,
		TEXT("%s failed with error %d: %s"), 
		lpszFunction, dw, lpMsgBuf); 
	//MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 
	LocalFree(lpMsgBuf);
	//LocalFree(lpDisplayBuf);
	return lpDisplayBuf;
}

LPTSTR DirectXErrorMessage(LPCTSTR lpszFunction, HRESULT hr)	//caller needs to delete the returned buffer
{
	//LPCTSTR pszErrorString = DXGetErrorString(hr);
	LPCTSTR pszErrorDesc = DXGetErrorDescription(hr);

	const int size = lstrlen((LPCTSTR)pszErrorDesc)+lstrlen(lpszFunction)+40;
	LPTSTR lpDisplayBuf = new TCHAR[size];
	StringCchPrintf(lpDisplayBuf, 
		size,
		TEXT("%s failed with error %d: %s"), 
		lpszFunction, hr, pszErrorDesc); 
	return lpDisplayBuf;
}

void LogErrorMessage(LPCTSTR lpszErrorMessage)
{
	TRACE(lpszErrorMessage);

	const size_t size = lstrlen(lpszErrorMessage) * 2 + 2;
	size_t nNumofCharConverted = 0;
	char* buf = new char[size];
	wcstombs_s(&nNumofCharConverted, buf, size, lpszErrorMessage, size);
	LOG_ERROR_FORMAT(buf);
	delete[] buf;
}

void LogWindowsError(LPCTSTR lpszFunction)
{
	LPTSTR lpszErrorMessage = WindowsErrorMessage(lpszFunction);
	LogErrorMessage(lpszErrorMessage);
	delete[] lpszErrorMessage;
}


void LogDirectXError(LPCTSTR lpszFunction, HRESULT hr)
{
	LPTSTR lpszErrorMessage = DirectXErrorMessage(lpszFunction, hr);
	LogErrorMessage(lpszErrorMessage);
	delete[] lpszErrorMessage;
}
