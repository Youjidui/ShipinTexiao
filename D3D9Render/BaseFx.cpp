#include "StdAfx.h"
#include ".\basefx.h"
#include "BaseTexture.h"
#include <assert.h>
#include "../Utility/ColorConvertor.h"
#include "../Utility/SafeDelete.h"

CBaseFx::CBaseFx()
: m_pDevice(NULL)
, m_pEffect(NULL)
{
}

CBaseFx::~CBaseFx()
{	
	Destroy();
}
HRESULT CBaseFx::Create ( LPDIRECT3DDEVICE9 pDevice,const TCHAR* szFxName)
{
	m_pDevice		  = pDevice;
	m_strResID          = szFxName;

	//wcscpy(m_szShaderFile,szFxName);
	HRESULT hr = S_OK;

	TCHAR szExeFilePath[MAX_PATH];
	GetModuleFileName(NULL, szExeFilePath, MAX_PATH);
	LPTSTR p = szExeFilePath + lstrlen(szExeFilePath) - 1;
	while(*p != '/' && *p != '\\') p--;
	p++;
	lstrcpy(p, szFxName);

	LPD3DXEFFECT pEffect = NULL;
	ID3DXBuffer* pBuffer = NULL;	
	//HANDLE hFile = INVALID_HANDLE_VALUE;
	//for(int i = 0;i < 3;i ++)
	{
		try
		{
			hr = D3DXCreateEffectFromFile(pDevice, szExeFilePath, NULL, NULL, 0, NULL, &pEffect, &pBuffer);
			if(SUCCEEDED(hr))
			{
				m_pEffect = pEffect;
			}
			else
			{
				pBuffer;
			}
			//hFile = CreateFile( szExeFilePath, 
			//	GENERIC_READ,              // open for reading 
			//	FILE_SHARE_READ,
			//	NULL,                      // no security 
			//	OPEN_EXISTING,
			//	FILE_ATTRIBUTE_NORMAL,     // normal file 
			//	NULL );                     // no attr. template 

			//if (hFile == INVALID_HANDLE_VALUE) 
			//{ 
			//	return E_FAIL;//Shader²»´æÔÚ
			//} 
			//DWORD dwFileSize = GetFileSize( hFile, NULL);
			//static byte sBuffer[500 * 1024]; 
			//DWORD dwReaded;
			//ReadFile(hFile, sBuffer, dwFileSize, &dwReaded,NULL);   
			//sBuffer[dwReaded + 1] = L'\0';
			//CloseHandle(hFile);

			//if(FAILED(hr = D3DXCreateEffect(m_pDevice,sBuffer,dwReaded,
			//	NULL,
			//	NULL,
			//	/*D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT|D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT*/
			//	/*D3DXFX_NOT_CLONEABLE*/(1 << 11),
			//	NULL,
			//	&pEffect,&pBuffer)))
			//{
			//	/*
			//	if(pBuffer)
			//	{
			//		TCHAR buffer[1024];
			//		LPVOID p=pBuffer->GetBufferPointer();
			//		swprintf(buffer,L"%s",(LPTSTR)p);
			//		SAFE_RELEASE(pBuffer);
			//	}
			//	*/
			//	assert(0);
			//}
			//else
			//{
			//	m_pEffect = pEffect;
			//	//SetYUVA2RGBAMatrix();

			//	return hr;
			//}	
			SAFE_RELEASE(pBuffer);
		}
		catch (...)
		{
			hr = E_FAIL;
			assert(0);
		}	
		Sleep(10);
	}
	return hr;
}
/*
HRESULT CBaseFx::SetYUVA2RGBAMatrix()
{
	LPD3DXEFFECT pEffect = m_pEffect;
	if(pEffect == NULL)
		return E_FAIL;

	D3DXHANDLE hMatRGB2YUV = pEffect->GetParameterByName(NULL,"matRGBA2YUVA");
	D3DXHANDLE hMatYUV2RGB = pEffect->GetParameterByName(NULL,"matYUVA2RGBA");

	if(0)//m_pResMan->m_dwVideoMode == VM_SD)
	{
		if(hMatRGB2YUV)
			pEffect->SetMatrix(hMatRGB2YUV,&g_matRGBA2YUVA_SD);
		if(hMatYUV2RGB)
			pEffect->SetMatrix(hMatYUV2RGB,&g_matYUVA2RGBA_SD);
	}		
	else
	{		
		if(hMatRGB2YUV)
			pEffect->SetMatrix(hMatRGB2YUV,&g_matRGBA2YUVA_HD);
		if(hMatYUV2RGB)
			pEffect->SetMatrix(hMatYUV2RGB,&g_matYUVA2RGBA_HD);
	}		
	return S_OK;
}
*/
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

HRESULT CBaseFx::SetTexture(LPCSTR pName, CBaseTexture* pBaseTex)
{
	CheckParamName(pName);

	HRESULT hr = E_FAIL;
	LPD3DXEFFECT pEffect = GetFxPtr();
	if(pEffect)
		hr = pEffect->SetTexture(pName,pBaseTex->GetTexture());
	return hr;
}

HRESULT CBaseFx::SetTexture(LPCSTR pName, LPDIRECT3DTEXTURE9 pTex)
{
	CheckParamName(pName);

	HRESULT hr = E_FAIL;
	LPD3DXEFFECT pEffect = GetFxPtr();
	if(pEffect)
		hr = pEffect->SetTexture(pName, pTex);
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

