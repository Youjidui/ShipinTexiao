#pragma once

//#include "BaseTexture.h"

class  CBaseFx
{
public:
	CBaseFx();
	virtual ~CBaseFx();
	HRESULT Create (LPDIRECT3DDEVICE9 pDevice, const TCHAR* szFxName, const char** ppszMacros = NULL, int nMacroCount = 0);
	LPD3DXEFFECT GetFxPtr();
	HRESULT GetFunction(void* pData, UINT* pSizeOfData){return S_OK;};

public:
	//wrap of D3DXEFFECT
	HRESULT SetTexture(LPCSTR pName, LPDIRECT3DTEXTURE9 pTex);
	HRESULT SetMatrix(LPCSTR pName, D3DXMATRIX* pMat);
	HRESULT SetMatrixArray(LPCSTR pName, D3DXMATRIX* pMat, UINT uCount);
	HRESULT SetVector(LPCSTR pName, D3DXVECTOR4* pVec);
	HRESULT SetVectorArray(LPCSTR pName, D3DXVECTOR4* pVec, UINT uCount);
	HRESULT SetInt(LPCSTR pName, INT iValue);
	HRESULT SetBool(LPCSTR pName, BOOL bValue);
	HRESULT SetBoolArray(LPCSTR pName, BOOL* pBValue, UINT uCount);
	HRESULT SetFloat(LPCSTR pName, FLOAT fValue);
	HRESULT SetFloatArray(LPCSTR pName, FLOAT* pf, UINT Count);
	void	CheckParamName(LPCSTR pName);
	HRESULT SetTechnique(LPCSTR pName);

	HRESULT Begin(UINT *pPasses, DWORD Flags);
	HRESULT End();

	HRESULT BeginPass(UINT uPass);
	HRESULT EndPass();

	HRESULT CommitChanges();

	//special function
	HRESULT SetYUVA2RGBAMatrix(int VideoStandard = 0);

protected:
	HRESULT Destroy();
	virtual HRESULT OnLost();
	virtual HRESULT OnRestore(); 

private:
	LPDIRECT3DDEVICE9 m_pDevice;
	LPD3DXEFFECT m_pEffect;
	std::wstring m_strResID;
};
