#include "StdAfx.h"
#include ".\vertexshader.h"
#include <wchar.h>
#include "../Utility/SafeDelete.h"

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
							   LPCTSTR szShaderName)
{
	HRESULT hr = E_FAIL;
	m_pDevice		 = pDevice;
	//wcscpy(m_szShaderFile,szShaderName);
	m_strResID = szShaderName;
	HANDLE hFile; 
	hFile = CreateFile( szShaderName, 
		GENERIC_READ,              // open for reading 
		FILE_SHARE_READ,
		NULL,                      // no security 
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,     // normal file 
		NULL );                     // no attr. template 

	if (hFile == INVALID_HANDLE_VALUE) 
	{ 
		return hr;//shader²»´æÔÚ
	} 
	DWORD dwFileSize = GetFileSize( hFile, NULL);
	BYTE *PBuffer = new BYTE[dwFileSize];  DWORD dwReaded;
	ReadFile(hFile, PBuffer, dwFileSize, &dwReaded,NULL);   
	if(SUCCEEDED(m_pDevice->CreateVertexShader((DWORD*)PBuffer,&m_pShader)))
	{
        hr = D3DXGetShaderConstantTable( (DWORD*)PBuffer, &m_pConstTable );
	}
	delete[] PBuffer;
	CloseHandle( hFile );
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
