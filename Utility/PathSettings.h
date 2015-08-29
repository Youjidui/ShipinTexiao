#pragma once

class PathSettings
{
public:
	static void GetResourceDir(OUT LPTSTR pszPathBuffer, IN int nBufferSize);
	static BOOL BuildResourcePath(OUT LPTSTR pszPathBuffer, IN int nBufferSize, IN LPCTSTR pszResFileName);

};
