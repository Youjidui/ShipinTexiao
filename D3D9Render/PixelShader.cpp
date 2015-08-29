#include "StdAfx.h"
#include ".\pixelshader.h"
#include "../Utility/SafeDelete.h"
#include <DxErr.h>
#pragma comment(lib, "DxErr")
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi")


CPixelShader::CPixelShader(void)
: m_pDevice(NULL)
, m_pShader(NULL)
, m_pConstTable(NULL)
{

}

CPixelShader::~CPixelShader(void)
{
}

HRESULT CPixelShader::Create (  LPDIRECT3DDEVICE9 pDevice,
							  const TCHAR* szShaderName,
							  const char** ppszMacros,
							  int nMacroCount)
{
	HRESULT hr = E_FAIL;
	m_strResID	= szShaderName;
	m_pDevice	= pDevice;

	TCHAR szExeFilePath[MAX_PATH];
	GetModuleFileName(NULL, szExeFilePath, MAX_PATH);
	LPTSTR p = szExeFilePath + lstrlen(szExeFilePath) - 1;
	while(*p != '/' && *p != '\\') p--;
	p++;
	lstrcpy(p, szShaderName);

	//HANDLE hFile;  
	//hFile = CreateFile( szExeFilePath, 
	//	GENERIC_READ,              // open for reading 
	//	FILE_SHARE_READ,
	//	NULL,                      // no security 
	//	OPEN_EXISTING,
	//	FILE_ATTRIBUTE_NORMAL,     // normal file 
	//	NULL );                     // no attr. template 

	//if (hFile == INVALID_HANDLE_VALUE) 
	//{ 
	//	return hr;//Shader²»´æÔÚ
	//} 
	//DWORD dwFileSize = GetFileSize( hFile, NULL);
	//BYTE *PBuffer = new BYTE[dwFileSize];  DWORD dwReaded;
	//ReadFile(hFile, PBuffer, dwFileSize, &dwReaded,NULL);
	 
	ASSERT(PathFileExists(szExeFilePath));

	LPD3DXBUFFER pCompiledShader = NULL, pErrorInfo = NULL;
	LPD3DXCONSTANTTABLE pConstTable = NULL;
	//hr  = D3DXCompileShaderFromFile(szExeFilePath, NULL, NULL, "main", "ps_3_0", 0, &pCompiledShader, &pErrorInfo, &pConstTable);
	LPCSTR pShaderProfile = D3DXGetPixelShaderProfile(pDevice);

	LPD3DXMACRO pMacros = NULL;
	if(ppszMacros)
	{
		int nAllocCount = nMacroCount + 1;
		pMacros = new D3DXMACRO[nAllocCount];
		ZeroMemory(pMacros, sizeof(D3DXMACRO) * nAllocCount);
		for(int i = 0; i < nMacroCount; ++i)
		{
			pMacros[i].Name = ppszMacros[i];
			pMacros[i].Definition = "\n";
		}
	}

	hr  = D3DXCompileShaderFromFile(szExeFilePath, pMacros, NULL, "main", pShaderProfile, 0, &pCompiledShader, &pErrorInfo, &pConstTable);
	if(SUCCEEDED(hr))
	{
		LPDIRECT3DPIXELSHADER9 pShader = NULL;
		hr = pDevice->CreatePixelShader((DWORD*)pCompiledShader->GetBufferPointer(), &pShader);
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

	//delete[] PBuffer;
	//CloseHandle( hFile );
	return hr;
}


HRESULT CPixelShader::GetFunction(void* pData, UINT* pSizeOfData)
{
    return (m_pShader)->GetFunction(pData, pSizeOfData);
}


UINT CPixelShader::GetSamplerIndex(D3DXHANDLE hTexName)
{
    return GetConstTablePtr()->GetSamplerIndex(hTexName);
}


HRESULT CPixelShader::Destroy()
{
	SAFE_RELEASE(m_pShader);
	SAFE_RELEASE(m_pConstTable);
	return S_OK;
}
