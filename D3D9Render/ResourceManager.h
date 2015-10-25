#pragma once

#include "MeshResource.h"
#include "FxEffectResource.h"
#include "PixelShaderResource.h"
#include "VertexShaderResource.h"

//shared resources, such as shaders, meshes, matrix, but not video buffers
class CResourceManager
{
public:
	CResourceManager(void);
	~CResourceManager(void);

public:
	CBaseMesh* CreateQuadMesh(LPDIRECT3DDEVICE9 lpDevice);
	CBaseMesh* CreateQuadGridMesh(LPDIRECT3DDEVICE9 lpDevice);
	CBaseMesh* CreateQuadWHMipmapMesh(LPDIRECT3DDEVICE9 lpDevice);	//WH = 0	width of src >= width of target and height of src >= height of target
	CBaseMesh* CreateQuadWMipmapMesh(LPDIRECT3DDEVICE9 lpDevice);	//W = 1
	CBaseMesh* CreateQuadHMipmapMesh(LPDIRECT3DDEVICE9 lpDevice);	//H = 2
	CBaseMesh* CreateQuadInstanceMesh(LPDIRECT3DDEVICE9 lpDevice);
	CBaseMesh* CreateMesh(LPDIRECT3DDEVICE9 pDevice, LPD3DXMESH pMesh, LPCTSTR pszResFileName);
	CBaseMesh* FindMesh(LPCTSTR pszResName);
	CBaseFx* CreateEffect(LPDIRECT3DDEVICE9 lpDevice, LPCTSTR lpszEffectName, const char** ppszMacros = NULL, int nMacroCount = 0);
	CVertexShader* CreateVertexShader(LPDIRECT3DDEVICE9 lpDevice, LPCTSTR lpszShaderName, const char** ppszMacros = NULL, int nMacroCount = 0);
	CPixelShader* CreatePixelShader(LPDIRECT3DDEVICE9 lpDevice, LPCTSTR lpszShaderName, const char** ppszMacros = NULL, int nMacroCount = 0);
	void GetOrthoMatrix( D3DXMATRIX** ppMatView , D3DXMATRIX** ppMatPrj );
	void GetPerspectiveMatrix( D3DXMATRIX** ppMatView , D3DXMATRIX** ppMatPrj );

private:
	void CreateOrthoMatrix( D3DXMATRIX* pMatView , D3DXMATRIX* pMatPrj );
	void CreatePerspectiveMatrix( D3DXMATRIX* pMatView , D3DXMATRIX* pMatPrj );

private:
	CBaseMesh* m_pQuadMesh;
	CBaseMesh* m_pQuadGridMesh;
	CBaseMesh* m_pQuadWHMipmapMesh;
	CBaseMesh* m_pQuadWMipmapMesh;
	CBaseMesh* m_pQuadHMipmapMesh;
	CBaseMesh* m_pQuadInstanceMesh;
	D3DXMATRIXA16 *m_matPrjOrtho, *m_matViewOrtho,
		*m_matPrjPersp, *m_matViewPersp;
	CFxEffectResource m_FxRes;
	CPixelShaderResource m_PSRes;
	CVertexShaderResource m_VSRes;
	CMeshResource m_MeshRes;
};
