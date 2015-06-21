#pragma once

#include <string>
#include <d3dx9.h>

class  CVertexShader
{
public:
  CVertexShader(void);
  virtual ~CVertexShader(void);
   HRESULT Create ( LPDIRECT3DDEVICE9 pDevice,
		                LPCTSTR szShaderName);
   HRESULT GetFunction(void* pData, UINT* pSizeOfData);
   LPDIRECT3DVERTEXSHADER9 GetVertexShaderPtr() { return m_pShader; }
protected:
  HRESULT Destroy();

private:
	LPDIRECT3DDEVICE9 m_pDevice;
	LPDIRECT3DVERTEXSHADER9 m_pShader;
	LPD3DXCONSTANTTABLE m_pConstTable;
	std::wstring m_strResID;
};

