// Resource.cpp : ���� DLL Ӧ�ó���ĵ���������
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
	_tcscat_s(szFind, _T("\\*.*"));     //����һ��Ҫָ��ͨ�������Ȼ�����ȡ�����ļ���Ŀ¼

	HANDLE hFind = ::FindFirstFile(szFind, &findFileData);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		return;
	}

	//�����ļ���
	while (TRUE)
	{
		if (findFileData.cFileName[0] != _T('.'))
		{//���ǵ�ǰ·�����߸�Ŀ¼�Ŀ�ݷ�ʽ
			if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{//����һ����ͨĿ¼
				//������һ����Ҫɨ����ļ���·��
				_tcscpy_s(szFind, MAX_PATH, lpPath);    
				_tcscat_s(szFind, _T("\\"));    
				_tcscat_s(szFind, findFileData.cFileName);
				///_tcscat_s(szNextDir, _T("\\*"));
				//������Ŀ¼
				TraverseFolder(szFind, f, pUserData);
			}
			else
			{
				TCHAR file[MAX_PATH];
				_stprintf_s(file, MAX_PATH, _T("%s\\%s"), lpPath, findFileData.cFileName);
				f(file, pUserData);
			}
		}

		//����ǵ�ǰ·�����߸�Ŀ¼�Ŀ�ݷ�ʽ����������ͨĿ¼����Ѱ����һ��Ŀ¼�����ļ�
		bRet = ::FindNextFile(hFind, &findFileData);
		if (!bRet)
		{//��������ʧ��
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
