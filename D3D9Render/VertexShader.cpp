#include "StdAfx.h"
#include ".\vertexshader.h"
#include <wchar.h>
#include "../Utility/SafeDelete.h"
#include "../Utility/PathSettings.h"
#include "../Logger/Logging.h"
#include "../Logger/LogDxError.h"
#include "ShaderResources.h"

//#include <DxErr.h>
//#pragma comment(lib, "DxErr")
#include <Shlwapi.h>
//#pragma comment(lib, "Shlwapi")

#pragma warning(disable:4996)

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
	m_strResID = szShaderName;

	TCHAR szExeFilePath[MAX_PATH];
#if defined(_SHADER_SOURCE_FILE)
	PathSettings::BuildResourcePath(szExeFilePath, sizeof(szExeFilePath), szShaderName);
#elif defined(_SHADER_COMPILED_FILE)
	PathSettings::BuildCompiledResourcePath(szExeFilePath, sizeof(szExeFilePath), szShaderName);
#else
	GetModuleFileName(NULL, szExeFilePath, MAX_PATH);
#endif
	ASSERT(PathFileExists(szExeFilePath));

	LPVOID pSrcBuffer = NULL;
#ifdef _SHADER_SOURCE_FILE
	LPD3DXBUFFER pCompiledShader = NULL, pErrorInfo = NULL;
	LPD3DXCONSTANTTABLE pConstTable = NULL;
	LPCSTR pShaderProfile = D3DXGetVertexShaderProfile(pDevice);

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
		pSrcBuffer = pCompiledShader->GetBufferPointer();
	}
	else
	{
		CHECK_AND_LOG_DIRECTX_API_ERROR(hr);
		LOG_ERROR_FORMAT("Compiling %s error:%s", szShaderName, pErrorInfo->GetBufferPointer());
	}
	if(pMacros)
	{
		delete[] pMacros;
		pMacros = NULL;
	}

#elif defined(_SHADER_COMPILED_FILE)
	HANDLE hFile = CreateFile( szExeFilePath, 
		GENERIC_READ,              // open for reading 
		FILE_SHARE_READ,
		NULL,                      // no security 
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,     // normal file 
		NULL );                     // no attr. template 

	if (hFile == INVALID_HANDLE_VALUE) 
	{
		CHECK_AND_LOG_WINDOWS_API_ERROR();
	}
	else
	{
		DWORD dwFileSize = GetFileSize( hFile, NULL);
		pSrcBuffer = new BYTE[dwFileSize+1];
		DWORD dwReaded;
		BOOL bOK = ReadFile(hFile, pSrcBuffer, dwFileSize, &dwReaded, NULL);
		if(!bOK)
		{
			CHECK_AND_LOG_WINDOWS_API_ERROR();
		}
	}
#else
	int nBufferSize = 0;
	//LPCTSTR pszShortName = PathFindFileName(szShaderName);
	szExeFilePath[0] = '\0';
	BOOL bOK = PathSettings::GetCompiledResFileName(szExeFilePath, MAX_PATH, szShaderName);
	ASSERT(bOK);
	pSrcBuffer = GetShaderBufferAndSize(szExeFilePath, nBufferSize);
	if(!pSrcBuffer)
	{
		CHECK_AND_LOG_WINDOWS_API_ERROR();
	}
#endif

	hr = m_pDevice->CreateVertexShader((DWORD*)pSrcBuffer, &m_pShader);
	if(SUCCEEDED(hr))
	{
		hr = D3DXGetShaderConstantTable( (DWORD*)pSrcBuffer, &m_pConstTable );
		if(FAILED(hr))
		{
			CHECK_AND_LOG_DIRECTX_API_ERROR(hr);
		}
	}
	else
	{
		CHECK_AND_LOG_DIRECTX_API_ERROR(hr);
	}

#ifdef _SHADER_SOURCE_FILE
	SAFE_RELEASE(pCompiledShader);
	SAFE_RELEASE(pErrorInfo);
	SAFE_RELEASE(pConstTable);
#else
	delete[] pSrcBuffer;
	pSrcBuffer = NULL;
#endif

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
