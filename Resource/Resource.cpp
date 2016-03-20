// Resource.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "../zip_utils_src/zip.h"
#include <tchar.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi")
#include "ZipResource.h"


typedef bool (*TraverseFolderCallbackFunction)(LPCTSTR pszFile, void* pUserData);

void TraverseFolder(LPCTSTR lpPath, TraverseFolderCallbackFunction f, void* pUserData)
{
	TCHAR szFind[MAX_PATH] = {_T("\0")};
	WIN32_FIND_DATA findFileData;
	BOOL bRet;

	_tcscpy_s(szFind, MAX_PATH, lpPath);
	_tcscat_s(szFind, _T("\\*.*"));     //这里一定要指明通配符，不然不会读取所有文件和目录

	HANDLE hFind = ::FindFirstFile(szFind, &findFileData);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		return;
	}

	//遍历文件夹
	while (TRUE)
	{
		if (findFileData.cFileName[0] != _T('.'))
		{//不是当前路径或者父目录的快捷方式
			if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{//这是一个普通目录
				//设置下一个将要扫描的文件夹路径
				_tcscpy_s(szFind, MAX_PATH, lpPath);    
				_tcscat_s(szFind, _T("\\"));    
				_tcscat_s(szFind, findFileData.cFileName);
				///_tcscat_s(szNextDir, _T("\\*"));
				//遍历该目录
				TraverseFolder(szFind, f, pUserData);
			}
			else
			{
				TCHAR file[MAX_PATH];
				_stprintf_s(file, MAX_PATH, _T("%s\\%s"), lpPath, findFileData.cFileName);
				f(file, pUserData);
			}
		}

		//如果是当前路径或者父目录的快捷方式，或者是普通目录，则寻找下一个目录或者文件
		bRet = ::FindNextFile(hFind, &findFileData);
		if (!bRet)
		{//函数调用失败
			//cout << "FindNextFile failed, error code: " 
			//  << GetLastError() << endl;
			break;
		}
	}

	::FindClose(hFind);
}

bool AddToZipFile(LPCTSTR pszFile, void* pUserData)
{
	HZIP hz = (HZIP)pUserData;
	if(hz)
	{
		int n = lstrlen(pszFile);
		if(n > 2)
		{
			if((_T('o') == pszFile[n-1] || _T('O') == pszFile[n-1])
				&& (_T('.') == pszFile[n-2]))
			{
				LPCTSTR pszShortName = PathFindFileName(pszFile);
				ZipAdd(hz, pszShortName, pszFile);
			}
		}
	}
	return true;
}


bool ZipShaderResourcesToFile(LPCTSTR pszTargetFilename, LPCTSTR pszSrcResourceDir)
{
	HZIP hz = CreateZip(pszTargetFilename, 0);
	TraverseFolder(pszSrcResourceDir, &AddToZipFile, hz);
	CloseZip(hz);
	return true;
}
