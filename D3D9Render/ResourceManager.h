#pragma once

#include "MeshResource.h"
#include "FxEffectResource.h"
#include "PixelShaderResource.h"
#include "VertexShaderResource.h"

class CResourceManager
{
public:
	CResourceManager(void);
	~CResourceManager(void);

public:
	CBaseMesh* CreateQuadMesh(LPDIRECT3DDEVICE9 lpDevice);
	CBaseFx* CreateEffect(LPDIRECT3DDEVICE9 lpDevice, LPCTSTR lpszEffectName, const char** ppszMacros = NULL, int nMacroCount = 0);
	CVertexShader* CreateVertexShader(LPDIRECT3DDEVICE9 lpDevice, LPCTSTR lpszShaderName, const char** ppszMacros = NULL, int nMacroCount = 0);
	CPixelShader* CreatePixelShader(LPDIRECT3DDEVICE9 lpDevice, LPCTSTR lpszShaderName, const char** ppszMacros = NULL, int nMacroCount = 0);
	void GetQuadMatrix( D3DXMATRIX** ppMatWorld, D3DXMATRIX** ppMatView , D3DXMATRIX** ppMatPrj );

private:
	void CreateQuadMatrix( D3DXMATRIX* pMatWorld, D3DXMATRIX* pMatView , D3DXMATRIX* pMatPrj );

private:
	CBaseMesh* m_pQuadMesh;
	D3DXMATRIXA16 *m_matWorld, *m_matPrj, *m_matView;
	CFxEffectResource m_FxRes;
	CPixelShaderResource m_PSRes;
	CVertexShaderResource m_VSRes;
};
