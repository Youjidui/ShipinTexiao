#include "StdAfx.h"
#include "PathSettings.h"
#include <stdlib.h>
#include <tchar.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi")

const TCHAR pszZipShaderFileName[] = _T("shaders.o");
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
	GetCompiledResFileName(p, nBufferSize - (p - pszPathBuffer), pszResFileName);
	return (PathFileExists(pszPathBuffer));
}

BOOL PathSettings::BuildZipResourcePath(OUT LPTSTR pszPathBuffer, IN int nBufferSize)
{
	return BuildResourcePath(pszPathBuffer, nBufferSize, pszZipShaderFileName);
}

BOOL PathSettings::GetCompiledResFileName(OUT LPTSTR pszBuffer, IN int nBufferSize, IN LPCTSTR pszResFileName)
{
	bool bOK = false;
	int size = lstrlen(pszResFileName);
	TCHAR* pszCompiledName = new TCHAR[size + 4];
	lstrcpy(pszCompiledName, pszResFileName);
	//LPTSTR pstart1 = StrRChr(pszCompiledName, NULL, '/');
	//if(pstart1)	pstart1++;
	//else pstart1 = pszCompiledName;
	//LPTSTR pstart2 = StrRChr(pszCompiledName, NULL, '\\');
	//if(pstart2)	pstart2++;
	//else pstart2 = pszCompiledName;
	//LPTSTR pstart = max(pstart1, pstart2);
	LPTSTR p = pszCompiledName + lstrlen(pszCompiledName) - 1;
	while(*p != '/' && *p != '\\' && p >= pszCompiledName) p--;
	p++;
	LPTSTR pstart = p;
	LPTSTR pend = StrRChr(pszCompiledName, NULL, '.');
	if(pend)	*pend = '\0';
	if(lstrlen(pstart) + 2 < nBufferSize)
	{
		lstrcat(pszBuffer, pstart);
		lstrcat(pszBuffer, _T(".o"));
		bOK = true;
	}
	delete[] pszCompiledName;
	return bOK;
}
