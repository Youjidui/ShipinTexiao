#pragma once

#include "MeshResource.h"
#include "FxEffectResource.h"
#include "PixelShaderResource.h"
#include "VertexShaderResource.h"
#include "BaseTexture.h"
#include "VideoBuffer.h"



enum BlendMode
{
	BLENDMODE_ALPHA = 0,
	BLENDMODE_ADD,
	BLENDMODE_SUB,
	BLENDMODE_MUL,
	BLENDMODE_DIV,
	BLENDMODE_COLORBURN,
	BLENDMODE_COLORDODGE,
	BLENDMODE_DARKEN,
	BLENDMODE_LIGHTEN,
	BLENDMODE_DIFFERENCE,
	BLENDMODE_HARDLIGHT,
	BLENDMODE_SOFTLIGHT,
	BLENDMODE_OVERLAY,
	BLENDMODE_SCREEN,
};


//shared resources, such as shaders, meshes, matrix, but not video buffers
class CResourceManager
{
public:
	CResourceManager(void);
	~CResourceManager(void);

public:
	CBaseMesh* CreateQuadMesh(LPDIRECT3DDEVICE9 pDevice);
	CBaseMesh* CreateQuadGridMesh(LPDIRECT3DDEVICE9 pDevice);
	CBaseMesh* CreateQuadWHMipmapMesh(LPDIRECT3DDEVICE9 pDevice);	//WH = 0	width of src >= width of target and height of src >= height of target
	CBaseMesh* CreateQuadWMipmapMesh(LPDIRECT3DDEVICE9 pDevice);	//W = 1
	CBaseMesh* CreateQuadHMipmapMesh(LPDIRECT3DDEVICE9 pDevice);	//H = 2
	CBaseMesh* CreateQuadInstanceMesh(LPDIRECT3DDEVICE9 pDevice);
	CBaseMesh* CreateMesh(LPDIRECT3DDEVICE9 pDevice, LPD3DXMESH pMesh, LPCTSTR pszResFileName);
	CBaseMesh* CreateMesh(LPDIRECT3DDEVICE9 pDevice, LPD3DXMESH pMesh, LPDIRECT3DVERTEXDECLARATION9 pVertexDecl, LPCTSTR pszResFileName);
	CBaseMesh* CreateMesh(LPDIRECT3DDEVICE9 pDevice,
		void*               pVertexInfo,
		UINT                uVBufferSize,
		UINT                uVertexStride,
		D3DPRIMITIVETYPE    ePrimitiveType,
		UINT                uPrimitiveCount,
		DWORD               dwFVF,
		const D3DVERTEXELEMENT9*  pVertexElement,
		const unsigned short**     ppIndexInfo,
		UINT                 uIBufferSize,
		UINT                 uIBNum,
		LPCTSTR          pszMeshName);
	CBaseMesh* CreateMesh(LPDIRECT3DDEVICE9 pDevice,
		void*               pVertexInfo_indexed,
		UINT                uVBufferSize_indexed,
		UINT                uVertexStride_indexed,
		void*				 pVertexInfo_instance,
		UINT				 uVBufferSize_instance,
		UINT				 uVertexStride_instance,
		D3DPRIMITIVETYPE    ePrimitiveType,
		UINT                uPrimitiveCount,
		DWORD               dwFVF,
		const D3DVERTEXELEMENT9*  pVertexElement,
		const unsigned short**    ppIndexInfo,
		UINT                uIBufferSize,
		UINT                uIBNum,
		LPCTSTR          pszMeshName);
	CBaseMesh* FindMesh(LPCTSTR pszResName);
	CBaseFx* CreateEffect(LPDIRECT3DDEVICE9 pDevice, LPCTSTR lpszEffectName, const char** ppszMacros = NULL, int nMacroCount = 0);
	CVertexShader* CreateVertexShader(LPDIRECT3DDEVICE9 pDevice, LPCTSTR lpszShaderName, const char** ppszMacros = NULL, int nMacroCount = 0);
	CPixelShader* CreatePixelShader(LPDIRECT3DDEVICE9 pDevice, LPCTSTR lpszShaderName, const char** ppszMacros = NULL, int nMacroCount = 0);
	CPixelShader* CreateBlendPixelShader( LPDIRECT3DDEVICE9 pDevice, BlendMode eMode );
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
