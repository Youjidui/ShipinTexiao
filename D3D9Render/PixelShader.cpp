#include "StdAfx.h"
#include ".\pixelshader.h"
#include "../Utility/SafeDelete.h"

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
							  const TCHAR* szShaderName)
{
	HRESULT hr = E_FAIL;
	m_strResID	= szShaderName;
	m_pDevice	= pDevice;
	//wcscpy(m_szShaderFile,szShaderName);
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
		return hr;//Shader²»´æÔÚ
	} 
	DWORD dwFileSize = GetFileSize( hFile, NULL);
	BYTE *PBuffer = new BYTE[dwFileSize];  DWORD dwReaded;
	ReadFile(hFile, PBuffer, dwFileSize, &dwReaded,NULL);   
	if(SUCCEEDED( m_pDevice->CreatePixelShader((DWORD*)PBuffer,
		(LPDIRECT3DPIXELSHADER9*)&m_pShader)))
	{
        hr = D3DXGetShaderConstantTable( (DWORD*)PBuffer, &m_pConstTable );
	}
	delete[] PBuffer;
	CloseHandle( hFile );
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
