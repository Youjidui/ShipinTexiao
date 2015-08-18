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
	CBaseFx* CreateEffect(LPDIRECT3DDEVICE9 lpDevice, LPCTSTR lpszEffectName);
	CVertexShader* CreateVertexShader(LPDIRECT3DDEVICE9 lpDevice, LPCTSTR lpszShaderName);
	CPixelShader* CreatePixelShader(LPDIRECT3DDEVICE9 lpDevice, LPCTSTR lpszShaderName);

private:
	CBaseMesh* m_pQuadMesh;
	CFxEffectResource m_FxRes;
	CPixelShaderResource m_PSRes;
	CVertexShaderResource m_VSRes;
};
