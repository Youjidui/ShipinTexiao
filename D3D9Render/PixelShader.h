#pragma once

#include <string>
#include <d3d9.h>
#include <d3dx9.h>

class  CPixelShader
{
public:
	CPixelShader(void);
	virtual ~CPixelShader(void);
	HRESULT Create ( LPDIRECT3DDEVICE9 pDevice, const TCHAR* szShaderName, const char** ppszMacros = NULL, int nMacroCount = 0);
	HRESULT GetFunction(void* pData, UINT* pSizeOfData);

	UINT GetSamplerIndex(D3DXHANDLE hTexName);
	LPDIRECT3DPIXELSHADER9 GetPixelShader() { return (LPDIRECT3DPIXELSHADER9)m_pShader; }
	LPD3DXCONSTANTTABLE GetConstTablePtr() { return m_pConstTable; }

	//special function
	HRESULT SetYUVA2RGBAMatrix(int VideoStandard = 0);

protected:
  HRESULT Destroy();

private:
	LPDIRECT3DDEVICE9 m_pDevice;
	LPDIRECT3DPIXELSHADER9 m_pShader;
	LPD3DXCONSTANTTABLE m_pConstTable;
	std::wstring m_strResID;
};
