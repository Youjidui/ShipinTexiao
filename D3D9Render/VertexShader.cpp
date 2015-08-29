#include "StdAfx.h"
#include ".\vertexshader.h"
#include <wchar.h>
#include "../Utility/SafeDelete.h"

#include <DxErr.h>
#pragma comment(lib, "DxErr")
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi")


CVertexShader::CVertexShader(void)
: m_pDevice(NULL)
, m_pShader(NULL)
, m_pConstTable(NULL)
{

}

CVertexShader::~CVertexShader(void)
{
}

HRESULT CVertexShader::Create ( LPDIRECT3DDEVICE9 pDevice,
							   LPCTSTR szShaderName, const char** ppszMacros, int nMacroCount)
{
	HRESULT hr = E_FAIL;
	m_pDevice		 = pDevice;
	//wcscpy(m_szShaderFile,szShaderName);
	m_strResID = szShaderName;

	TCHAR szExeFilePath[MAX_PATH];
	GetModuleFileName(NULL, szExeFilePath, MAX_PATH);
	LPTSTR p = szExeFilePath + lstrlen(szExeFilePath) - 1;
	while(*p != '/' && *p != '\\') p--;
	p++;
	lstrcpy(p, szShaderName);
	ASSERT(PathFileExists(szExeFilePath));

	//HANDLE hFile; 
	//hFile = CreateFile( szShaderName, 
	//	GENERIC_READ,              // open for reading 
	//	FILE_SHARE_READ,
	//	NULL,                      // no security 
	//	OPEN_EXISTING,
	//	FILE_ATTRIBUTE_NORMAL,     // normal file 
	//	NULL );                     // no attr. template 

	//if (hFile == INVALID_HANDLE_VALUE) 
	//{ 
	//	return hr;//shader²»´æÔÚ
	//} 
	//DWORD dwFileSize = GetFileSize( hFile, NULL);
	//BYTE *PBuffer = new BYTE[dwFileSize];  DWORD dwReaded;
	//ReadFile(hFile, PBuffer, dwFileSize, &dwReaded,NULL);   
	//if(SUCCEEDED(m_pDevice->CreateVertexShader((DWORD*)PBuffer,&m_pShader)))
	//{
 //       hr = D3DXGetShaderConstantTable( (DWORD*)PBuffer, &m_pConstTable );
	//}
	//delete[] PBuffer;
	//CloseHandle( hFile );


	LPD3DXBUFFER pCompiledShader = NULL, pErrorInfo = NULL;
	LPD3DXCONSTANTTABLE pConstTable = NULL;
	LPCSTR pShaderProfile = D3DXGetVertexShaderProfile(pDevice);

	LPD3DXMACRO pMacros = NULL;
	if(ppszMacros)
	{
		pMacros = new D3DXMACRO[nMacroCount];
		ZeroMemory(pMacros, sizeof(D3DXMACRO) * nMacroCount);
		for(int i = 0; i < nMacroCount; ++i)
		{
			pMacros[i].Name = ppszMacros[i];
		}
	}

	hr  = D3DXCompileShaderFromFile(szExeFilePath, pMacros, NULL, "main", pShaderProfile, 0, &pCompiledShader, &pErrorInfo, &pConstTable);
	if(SUCCEEDED(hr))
	{
		LPDIRECT3DVERTEXSHADER9 pShader = NULL;
		hr = pDevice->CreateVertexShader((DWORD*)pCompiledShader->GetBufferPointer(), &pShader);
		if(SUCCEEDED(hr))
		{
			//hr = D3DXGetShaderConstantTable( (DWORD*)PBuffer, &m_pConstTable );
			m_pConstTable = pConstTable;
			m_pShader = pShader;
		}
		else
		{
			LPCTSTR pszErrorString = DXGetErrorString(hr);
			LPCTSTR pszErrorDesc = DXGetErrorDescription(hr);
			TRACE(pszErrorString);
			TRACE(pszErrorDesc);
		}
	}
	else
	{
		LPCTSTR pszErrorString = DXGetErrorString(hr);
		LPCTSTR pszErrorDesc = DXGetErrorDescription(hr);
		TRACE(pszErrorString);
		TRACE(pszErrorDesc);
	}

	return hr;
}


HRESULT CVertexShader::GetFunction(void* pData, UINT* pSizeOfData)
{
    return (m_pShader)->GetFunction(pData, pSizeOfData);
}


HRESULT CVertexShader::Destroy()
{
	SAFE_RELEASE(m_pShader);
	SAFE_RELEASE(m_pConstTable);
	return S_OK;
}
