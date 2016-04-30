#include "StdAfx.h"
#include ".\basefx.h"
#include "BaseTexture.h"
#include <assert.h>
#include "../Utility/ColorConvertor.h"
#include "../Utility/SafeDelete.h"
#include "../Utility/PathSettings.h"
#include "../Logger/Logging.h"
#include "../Logger/LogDxError.h"
#include "ShaderResources.h"
#include <dxerr.h>
#include <Shlwapi.h>
//#pragma comment(lib, "Shlwapi")

#pragma warning(disable:4996)

CBaseFx::CBaseFx()
: m_pDevice(NULL)
, m_pEffect(NULL)
{
}

CBaseFx::~CBaseFx()
{	
	Destroy();
}

//#define _SHADER_COMPILED_FILE

HRESULT CBaseFx::Create ( LPDIRECT3DDEVICE9 pDevice,const TCHAR* szFxName, const char** ppszMacros, int nMacroCount)
{
	m_pDevice		  = pDevice;
	m_strResID          = szFxName;

	//wcscpy(m_szShaderFile,szFxName);
	HRESULT hr = E_FAIL;

	TCHAR szExeFilePath[MAX_PATH];
#if defined(_SHADER_SOURCE_FILE)
	PathSettings::BuildResourcePath(szExeFilePath, sizeof(szExeFilePath), szFxName);
#elif defined(_SHADER_COMPILED_FILE)
	PathSettings::BuildCompiledResourcePath(szExeFilePath, sizeof(szExeFilePath), szFxName);
#else
	GetModuleFileName(NULL, szExeFilePath, MAX_PATH);
#endif
	ASSERT(PathFileExists(szExeFilePath));

	LPD3DXEFFECT pEffect = NULL;
	ID3DXBuffer* pBuffer = NULL;
	{
		try
		{
#ifdef _SHADER_SOURCE_FILE
			LPD3DXMACRO pMacros = NULL;
			if(ppszMacros)
			{
				int nAllocCount = nMacroCount + 1;
				pMacros = new D3DXMACRO[nAllocCount];
				ZeroMemory(pMacros, sizeof(D3DXMACRO) * nAllocCount);
				for(int i = 0; i < nMacroCount; ++i)
				{
					pMacros[i].Name = ppszMacros[i];
				}
			}

			hr = D3DXCreateEffectFromFile(pDevice, szExeFilePath, pMacros, NULL, 0, NULL, &pEffect, &pBuffer);
			if(SUCCEEDED(hr))
			{
				m_pEffect = pEffect;
			}
			else
			{
				LPCTSTR pszErrorString = DXGetErrorString(hr);
				LPCTSTR pszErrorDesc = DXGetErrorDescription(hr);
				TRACE(pszErrorString);
				TRACE(pszErrorDesc);

				char buf[MAX_PATH];
				wcstombs(buf, pszErrorDesc, MAX_PATH);
				LOG_ERROR_FORMAT("%s:D3DXCreateEffectFromFile failed because %s", __FUNCTION__, buf);
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
				return E_FAIL;//Shader²»´æÔÚ
			} 
			DWORD dwFileSize = GetFileSize( hFile, NULL);
			byte* sBuffer = new byte[dwFileSize + 2]; 
			DWORD dwReaded;
			ReadFile(hFile, sBuffer, dwFileSize, &dwReaded,NULL);   
			sBuffer[dwReaded + 1] = _T('\0');
			CloseHandle(hFile);

			if(FAILED(hr = D3DXCreateEffect(m_pDevice,sBuffer,dwReaded,
				NULL,
				NULL,
				D3DXFX_NOT_CLONEABLE,
				NULL,
				&pEffect,&pBuffer)))
			{
				CHECK_AND_LOG_DIRECTX_API_ERROR(hr);
			}
			else
			{
				m_pEffect = pEffect;
				//SetYUVA2RGBAMatrix();
			}
			delete[] sBuffer;
#else
			int nBufferSize = 0;
			//LPCTSTR pszShortName = PathFindFileName(szFxName);
			szExeFilePath[0] = '\0';
			BOOL bOK = PathSettings::GetCompiledResFileName(szExeFilePath, MAX_PATH, szFxName);
			ASSERT(bOK);
			LPVOID pSrcBuffer = GetShaderBufferAndSize(szExeFilePath, nBufferSize);
			if(pSrcBuffer)
			{
			if(FAILED(hr = D3DXCreateEffect(m_pDevice, pSrcBuffer, nBufferSize,
				NULL,
				NULL,
				D3DXFX_NOT_CLONEABLE,
				NULL,
				&pEffect,&pBuffer)))
			{
				CHECK_AND_LOG_DIRECTX_API_ERROR(hr);
			}
			else
			{
				m_pEffect = pEffect;
				//SetYUVA2RGBAMatrix();
			}
			delete[] pSrcBuffer;
			}
#endif
		}
		catch (...)
		{
			hr = E_FAIL;
			assert(0);
		}	
	}
	SAFE_RELEASE(pBuffer);
	return hr;
}


LPD3DXEFFECT CBaseFx::GetFxPtr()
{
	return m_pEffect;
}
HRESULT CBaseFx::Destroy()
{
	SAFE_RELEASE(m_pEffect);
	return S_OK;
}
HRESULT CBaseFx::OnLost()
{
	LPD3DXEFFECT pEffect = GetFxPtr();
	if(pEffect)
		pEffect->OnLostDevice();
	return S_OK;
}
HRESULT CBaseFx::OnRestore()
{
	LPD3DXEFFECT pEffect = GetFxPtr();
	if(pEffect)
		pEffect->OnResetDevice();
	return S_OK;
}

//HRESULT CBaseFx::SetTexture(LPCSTR pName, CBaseTexture* pBaseTex)
//{
//	CheckParamName(pName);
//
//	HRESULT hr = E_FAIL;
//	LPD3DXEFFECT pEffect = GetFxPtr();
//	if(pEffect)
//		hr = pEffect->SetTexture(pName,pBaseTex->GetTexture());
//	return hr;
//}

HRESULT CBaseFx::SetTexture(LPCSTR pName, LPDIRECT3DTEXTURE9 pTex)
{
	CheckParamName(pName);

	HRESULT hr = E_FAIL;
	LPD3DXEFFECT pEffect = GetFxPtr();
	if(pEffect)
	{
		hr = pEffect->SetTexture(pName, pTex);
		CHECK_AND_LOG_DIRECTX_API_ERROR(hr);
	}
	return hr;
}

HRESULT CBaseFx::SetMatrix(LPCSTR pName, D3DXMATRIX* pMat)
{
	CheckParamName(pName);

	HRESULT hr = E_FAIL;
	LPD3DXEFFECT pEffect = GetFxPtr();
	if(pEffect)
		hr = pEffect->SetMatrix(pName,pMat);
	return hr;
}

HRESULT CBaseFx::SetMatrixArray(LPCSTR pName, D3DXMATRIX* pMat, UINT uCount)
{
	CheckParamName(pName);

	HRESULT hr = E_FAIL;
	LPD3DXEFFECT pEffect = GetFxPtr();
	if(pEffect)
		hr = pEffect->SetMatrixArray(pName,pMat,uCount);
	return hr;
}

HRESULT CBaseFx::SetVector(LPCSTR pName, D3DXVECTOR4* pVec)
{
	CheckParamName(pName);

	HRESULT hr = E_FAIL;
	LPD3DXEFFECT pEffect = GetFxPtr();
	if(pEffect)
		hr = pEffect->SetVector(pName,pVec);
	return hr;
}

HRESULT CBaseFx::SetVectorArray(LPCSTR pName, D3DXVECTOR4* pVec, UINT uCount)
{
	CheckParamName(pName);

	HRESULT hr = E_FAIL;
	LPD3DXEFFECT pEffect = GetFxPtr();
	if(pEffect)
		hr = pEffect->SetVectorArray(pName,pVec,uCount);
	return hr;
}

HRESULT CBaseFx::SetInt(LPCSTR pName, INT iValue)
{
	CheckParamName(pName);

	HRESULT hr = E_FAIL;
	LPD3DXEFFECT pEffect = GetFxPtr();
	if(pEffect)
		hr = pEffect->SetInt(pName,iValue);
	return hr;
}

HRESULT CBaseFx::SetBool(LPCSTR pName, BOOL bValue)
{
	CheckParamName(pName);

	HRESULT hr = E_FAIL;
	LPD3DXEFFECT pEffect = GetFxPtr();
	if(pEffect)
		hr = pEffect->SetBool(pName,bValue);
	return hr;
}

HRESULT CBaseFx::SetBoolArray(LPCSTR pName, BOOL* pBValue, UINT uCount)
{
	CheckParamName(pName);

	HRESULT hr = E_FAIL;
	LPD3DXEFFECT pEffect = GetFxPtr();
	if(pEffect)
		hr = pEffect->SetBoolArray(pName,pBValue,uCount);
	return hr;
}


HRESULT CBaseFx::SetFloat(LPCSTR pName, FLOAT fValue)
{
	CheckParamName(pName);

	HRESULT hr = E_FAIL;
	LPD3DXEFFECT pEffect = GetFxPtr();
	if(pEffect)
		hr = pEffect->SetFloat(pName,fValue);
	return hr;
}


HRESULT CBaseFx::SetFloatArray( LPCSTR pName, FLOAT* pf, UINT Count )
{
	CheckParamName(pName);

	HRESULT hr = E_FAIL;
	LPD3DXEFFECT pEffect = GetFxPtr();
	if(pEffect)
		hr = pEffect->SetFloatArray(pName,pf,Count);
	return hr;
}





HRESULT CBaseFx::SetTechnique(LPCSTR pName)
{
	LPD3DXEFFECT pEffect = GetFxPtr();

#ifdef _DEBUG
	D3DXHANDLE hParam = pEffect->GetTechniqueByName(pName);
	assert( hParam != NULL );
#endif

	HRESULT hr = E_FAIL;
	if(pEffect)
		hr = pEffect->SetTechnique(pName);
	return hr;
}

HRESULT CBaseFx::Begin(UINT *pPasses, DWORD Flags)
{
	HRESULT hr = E_FAIL;
	LPD3DXEFFECT pEffect = GetFxPtr();
	if(pEffect)
		hr = pEffect->Begin(pPasses,Flags);
	return hr;
}
HRESULT CBaseFx::End()
{
	HRESULT hr = E_FAIL;
	LPD3DXEFFECT pEffect = GetFxPtr();
	if(pEffect)
		hr = pEffect->End();
	return hr;

}

HRESULT CBaseFx::BeginPass(UINT uPass)
{
	HRESULT hr = E_FAIL;
	LPD3DXEFFECT pEffect = GetFxPtr();
	if(pEffect)
		hr = pEffect->BeginPass(uPass);
	return hr;

}
HRESULT CBaseFx::EndPass()
{
	HRESULT hr = E_FAIL;
	LPD3DXEFFECT pEffect = GetFxPtr();
	if(pEffect)
		hr = pEffect->EndPass();
	return hr;

}

HRESULT CBaseFx::CommitChanges()
{
	HRESULT hr = E_FAIL;
	LPD3DXEFFECT pEffect = GetFxPtr();
	if(pEffect)
		hr = pEffect->CommitChanges();
	return hr;
}

void	CBaseFx::CheckParamName(LPCSTR pName)
{
#ifdef _DEBUG
	LPD3DXEFFECT pEffect = GetFxPtr();
	if( pEffect )
	{
		D3DXHANDLE hParam = pEffect->GetParameterByName(NULL,pName);
		assert( hParam != NULL );
	}
#endif
}

HRESULT CBaseFx::SetYUVA2RGBAMatrix(int VideoStandard/* = 0*/)
{
	HRESULT hr = E_FAIL;
	LPD3DXEFFECT pEffect = GetFxPtr();
	if( pEffect )
	{
		D3DXHANDLE hMatRGB2YUV = pEffect->GetParameterByName(NULL,"matRGBA2YUVA");
		ASSERT( hMatRGB2YUV != NULL );
		D3DXHANDLE hMatYUV2RGB = pEffect->GetParameterByName(NULL,"matYUVA2RGBA");
		ASSERT(hMatYUV2RGB);

		if(VideoStandard)	// == 0 (HD)
		{
			if(hMatRGB2YUV)
			{
				hr = pEffect->SetMatrix(hMatRGB2YUV,&g_matRGBA2YUVA_SD);
				ASSERT(SUCCEEDED(hr));
			}
			if(hMatYUV2RGB)
			{
				hr = pEffect->SetMatrix(hMatYUV2RGB,&g_matYUVA2RGBA_SD);
				ASSERT(SUCCEEDED(hr));
			}
		}		
		else
		{		
			if(hMatRGB2YUV)
			{
				hr = pEffect->SetMatrix(hMatRGB2YUV,&g_matRGBA2YUVA_HD);
				ASSERT(SUCCEEDED(hr));
			}
			if(hMatYUV2RGB)
			{
				hr = pEffect->SetMatrix(hMatYUV2RGB,&g_matYUVA2RGBA_HD);
				ASSERT(SUCCEEDED(hr));
			}
		}		
	}
	return hr;
}
