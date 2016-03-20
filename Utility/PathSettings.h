#pragma once

class PathSettings
{
public:
	static void GetResourceDir(OUT LPTSTR pszPathBuffer, IN int nBufferSize);
	static BOOL BuildResourcePath(OUT LPTSTR pszPathBuffer, IN int nBufferSize, IN LPCTSTR pszResFileName);
	static BOOL BuildCompiledResourcePath( OUT LPTSTR pszPathBuffer, IN int nBufferSize, IN LPCTSTR pszResFileName );
	static BOOL BuildZipResourcePath(OUT LPTSTR pszPathBuffer, IN int nBufferSize);
	static BOOL GetCompiledResFileName(OUT LPTSTR pszBuffer, IN int nBufferSize, IN LPCTSTR pszResFileName);
};
