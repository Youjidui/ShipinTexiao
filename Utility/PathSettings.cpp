#include "StdAfx.h"
#include "PathSettings.h"
#include <stdlib.h>
#include <tchar.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi")


const TCHAR pszResDir[] = _T("../Resource/");
const TCHAR pszCompiledResDir[] = _T("../Resource/build/");

void PathSettings::GetResourceDir( LPTSTR pszPathBuffer, int nBufferSize )
{
	GetModuleFileName(NULL, pszPathBuffer, nBufferSize);
	LPTSTR p = pszPathBuffer + lstrlen(pszPathBuffer) - 1;
	while(*p != '/' && *p != '\\') p--;
	p++;
	//*p = '\0';
	lstrcpy(p, pszResDir);
}

BOOL PathSettings::BuildResourcePath( OUT LPTSTR pszPathBuffer, IN int nBufferSize, IN LPCTSTR pszResFileName )
{
	GetModuleFileName(NULL, pszPathBuffer, nBufferSize);
	LPTSTR p = pszPathBuffer + lstrlen(pszPathBuffer) - 1;
	while(*p != '/' && *p != '\\') p--;
	p++;
	lstrcpy(p, pszResDir);
	//p += _countof(pszResDir);
	//lstrcpy(p, pszResFileName);
	lstrcat(p, pszResFileName);
	return (PathFileExists(pszPathBuffer));
}

BOOL PathSettings::BuildCompiledResourcePath( OUT LPTSTR pszPathBuffer, IN int nBufferSize, IN LPCTSTR pszResFileName )
{
	GetModuleFileName(NULL, pszPathBuffer, nBufferSize);
	LPTSTR p = pszPathBuffer + lstrlen(pszPathBuffer) - 1;
	while(*p != '/' && *p != '\\') p--;
	p++;
	lstrcpy(p, pszCompiledResDir);
	//p += _countof(pszResDir);
	//lstrcpy(p, pszResFileName);
	int size = lstrlen(pszResFileName);
	TCHAR* pszCompiledName = new TCHAR[size + 4];
	lstrcpy(pszCompiledName, pszResFileName);
	LPTSTR pstart = StrRChr(pszCompiledName, NULL, '/');
	if(pstart)	pstart++;
	else pstart = pszCompiledName;
	LPTSTR pend = StrRChr(pszCompiledName, NULL, '.');
	if(pend)	*pend = '\0';
	lstrcat(p, pstart);
	lstrcat(p, _T(".o"));
	delete[] pszCompiledName;
	return (PathFileExists(pszPathBuffer));
}
