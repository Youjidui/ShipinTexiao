#pragma once

#include <assert.h>
#include <d3dx9mesh.h>
//#include "../DXSDK/Include/d3dx9mesh.h"
#include <string>
#pragma comment(lib, "d3dx9.lib")

//class CRenderEngine;

class  CBaseMesh 
{
public:
	CBaseMesh(void);
	~CBaseMesh(void);
	 HRESULT Create( LPDIRECT3DDEVICE9 pDevice,
		             const void*               pVertexInfo,
		             const UINT                uVBufferSize,
					 const UINT                uVertexStride,
				     const D3DPRIMITIVETYPE    ePrimitiveType,
					 const UINT                uPrimitiveCount,
					 const DWORD               dwFVF,
                     const D3DVERTEXELEMENT9*  pVertexElement, 
					 const unsigned short**    ppIndexInfo,
					 const UINT                uIBufferSize,
					 const UINT                uIBNum,
                     LPCTSTR                pszResID);	

     HRESULT Create(LPDIRECT3DDEVICE9 pDevice,
                    LPD3DXMESH pMesh,
					LPCTSTR                pszResID);	

	// added by xp [1/14/2009]
	// 建立一个专为点精灵创建vertex buffer的mesh
	HRESULT CreatePointSpriteMesh(LPDIRECT3DDEVICE9 pDevice,
								  const UINT          uVBufferSize,
								  const UINT		  uVertexStride,
								  const D3DVERTEXELEMENT9*  pVertexElement,
								  LPCTSTR                pszResID);	
    //Create Instance
    HRESULT Create(LPDIRECT3DDEVICE9 pDevice,
        LPDIRECT3DVERTEXBUFFER9* pVB,
        LPDIRECT3DINDEXBUFFER9  pIB,
        LPDIRECT3DVERTEXDECLARATION9 pDcl,
		LPCTSTR                pszResID);	
	


    BOOL    DrawSubset(UINT uAttributeID)
    {
        HRESULT hr = m_pMesh->DrawSubset(uAttributeID);
        assert(SUCCEEDED(hr));
        return SUCCEEDED(hr);
    }
	LPD3DXMESH GetMesh()
	{
		return m_pMesh;
	}

	//等把特技代码移植完了，这些Draw函数都要整理
    BOOL DrawInstance(int nNumInstance);
	BOOL DrawInstance(UINT strideInstance, int nNumInstance);
   bool       DrawMesh( const UINT  iB_id = 0, LPDIRECT3DVERTEXSHADER9 pShader = NULL );
   bool       DrawMeshFx( const UINT  iB_id = 0  );
   bool       DrawMesh( const UINT  iB_id,
	                    const UINT uNumVertices,
                        const UINT uPrimitiveCount,
                        LPDIRECT3DVERTEXSHADER9 pShader = NULL);
   bool       DrawMeshFx( const UINT  iB_id,
	   const UINT uNumVertices,
	   const UINT uPrimitiveCount
	   );

   //bool			DrawMeshOffset( const UINT  iB_id,
			//					const UINT uNumVertices,
			//					const UINT uOffsetBytes,
			//					const UINT uPrimitiveCount,
			//					LPCTSTR pszShaderName = NULL);
	bool		LockVertices( void** ppVertices );
	bool		UnlockVertices();

	/*
	create stream0 vb as indexed data;
	create stream1 vb as instance data;
	*/
	HRESULT Create( LPDIRECT3DDEVICE9 pDevice,
		const void*               pVertexInfo_indexed,
		const UINT                uVBufferSize_indexed,
		const UINT                uVertexStride_indexed,
		const void*					pVertexInfo_instance,
		const UINT					uVBufferSize_instance,
		const UINT					uVertexStride_instance,

		const D3DPRIMITIVETYPE    ePrimitiveType,
		const UINT                uPrimitiveCount,
		const DWORD               dwFVF,
		const D3DVERTEXELEMENT9*  pVertexElement, 

		const unsigned short**    ppIndexInfo,
		const UINT                uIBufferSize,
		const UINT                uIBNum,

		LPCTSTR                pszResID);	

protected:
    HRESULT    Destroy();
	bool    Draw(const UINT  iB_id,
		const UINT uNumVertices,
		const UINT uPrimitiveCount
		);

protected:
	LPDIRECT3DDEVICE9 m_pDevice;
   	LPDIRECT3DVERTEXBUFFER9 m_pVB[2];
	LPDIRECT3DINDEXBUFFER9  m_pIB[8];
    LPDIRECT3DVERTEXDECLARATION9 m_pDecl;
	D3DPRIMITIVETYPE        m_ePrimitiveType;
	UINT                    m_uPrimitiveCount;
	UINT                    m_uNumVertices;
	DWORD                   m_dwFVF;
	UINT                    m_uVertexStride;
	UINT                    m_nIBnum;
    LPD3DXMESH              m_pMesh;
	std::wstring			m_strResID;
};
