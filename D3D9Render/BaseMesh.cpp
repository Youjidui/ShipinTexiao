#include "StdAfx.h"
#include <assert.h>
#include ".\basemesh.h"
#include "../Utility/SafeDelete.h"
#include <shlwapi.h>
#pragma comment(lib, "shlwapi")

CBaseMesh::CBaseMesh(void)
: m_pDevice(NULL)
,m_pDecl(NULL)
,m_ePrimitiveType(D3DPT_TRIANGLELIST)
,m_uPrimitiveCount(0)
,m_uNumVertices(0)
,m_dwFVF(0)
,m_uVertexStride(0)
, m_nIBnum(0)
, m_pMesh(NULL)
{
    for (int i=0;i<8;i++)
    {
      m_pIB[i] = NULL;
    }
    m_pVB[0] = NULL;
    m_pVB[1] = NULL;
}

CBaseMesh::~CBaseMesh(void)
{
	Destroy();
}

HRESULT  CBaseMesh::Destroy()
{
	HRESULT hr =S_OK;	
	for (int i=0;i<8;i++)	
        SAFE_RELEASE(m_pIB[i]);		
	try
	{
		SAFE_RELEASE(m_pMesh);
	}
	catch (...)
	{
		assert(0);
	}
    SAFE_RELEASE(m_pVB[0]);
    SAFE_RELEASE(m_pVB[1]);
    SAFE_RELEASE(m_pDecl);
	return hr;
}


HRESULT  CBaseMesh::Create(  LPDIRECT3DDEVICE9 pDevice,
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
							 LPCTSTR                pszResID)
{

	m_pDevice		  = pDevice;
	m_ePrimitiveType  = ePrimitiveType;
	m_uPrimitiveCount = uPrimitiveCount;
	m_uNumVertices    = uVBufferSize/uVertexStride;
	m_dwFVF           = dwFVF;
	m_uVertexStride   = uVertexStride; 
	m_nIBnum          = uIBNum;
	m_strResID = pszResID;

	if( FAILED( m_pDevice->CreateVertexBuffer( uVBufferSize,
		0,
		dwFVF,
		D3DPOOL_MANAGED,
		&m_pVB[0],
		NULL )))
	{
		return E_FAIL;
	}

	void* pVBuffer;
	if(FAILED(m_pVB[0]->Lock( 0,0,&pVBuffer,0 )))
		return E_FAIL;

	memcpy( pVBuffer,pVertexInfo,uVBufferSize );
	m_pVB[0]->Unlock();

	if(pVertexElement)
	{
		if(FAILED(m_pDevice->CreateVertexDeclaration( pVertexElement,
			&m_pDecl)))
			return false;
	}

	for(UINT i=0;i<uIBNum;i++)
	{
		const unsigned short* pIndexInfo = ppIndexInfo[i];
		if(pIndexInfo)
		{
			if( FAILED( m_pDevice->CreateIndexBuffer( uIBufferSize,
				0,
				D3DFMT_INDEX16,
				D3DPOOL_MANAGED,
				&(m_pIB[i]),
				NULL)))

			{
				return E_FAIL;
			}

			void* pIBuffer;
			if(FAILED(m_pIB[i]->Lock( 0,0,&pIBuffer,0 )))
				return E_FAIL;
			memcpy(pIBuffer,pIndexInfo,uIBufferSize);
			m_pIB[i]->Unlock();
		}
	}

	return S_OK;		
}

HRESULT CBaseMesh::Create(LPDIRECT3DDEVICE9 pDevice,
			   LPD3DXMESH pMesh,
			   LPCTSTR                pszResID)
{
	m_pDevice		  = pDevice;
	m_pMesh = pMesh;
	m_strResID = pszResID;
	return S_OK;
}

HRESULT CBaseMesh::Create(LPDIRECT3DDEVICE9 pDevice,
			   LPDIRECT3DVERTEXBUFFER9* pVB,
			   LPDIRECT3DINDEXBUFFER9  pIB,
			   LPDIRECT3DVERTEXDECLARATION9 pDcl,
			   LPCTSTR                pszResID)
{
	m_pDevice		  = pDevice;
	m_pVB[0]          = pVB[0];
	m_pVB[1]          = pVB[1];
	m_pIB[0]          = pIB;
	m_pDecl           = pDcl;
	m_strResID = pszResID;
	return S_OK;
}



// added by xp [1/14/2009]
// 建立一个专为点精灵创建vertex buffer的mesh
HRESULT CBaseMesh::CreatePointSpriteMesh(LPDIRECT3DDEVICE9 pDevice,
							  const UINT          uVBufferSize,
							  const UINT		  uVertexStride,
							  const D3DVERTEXELEMENT9*  pVertexElement,
							  LPCTSTR                pszResID)
{

	m_pDevice		  = pDevice;
	m_ePrimitiveType  = D3DPT_POINTLIST;
	m_uNumVertices    = uVBufferSize/uVertexStride;
	m_uVertexStride   = uVertexStride; 
	m_uPrimitiveCount = m_uNumVertices;
	m_strResID = pszResID;

	if( FAILED( m_pDevice->CreateVertexBuffer( uVBufferSize,
		D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
		0,
		D3DPOOL_MANAGED,
		&m_pVB[0],
		NULL )))
	{
		return E_FAIL;
	}

	if(pVertexElement)
	{
		if(FAILED(m_pDevice->CreateVertexDeclaration( pVertexElement,
			&m_pDecl)))
			return false;
	}
	return S_OK;
}

HRESULT CBaseMesh::Create( LPDIRECT3DDEVICE9 pDevice,
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

			   LPCTSTR                pszResID)
{
	m_pDevice		  = pDevice;
	m_ePrimitiveType  = ePrimitiveType;
	m_uPrimitiveCount = uPrimitiveCount;
	m_uNumVertices    = uVBufferSize_indexed/uVertexStride_indexed;
	m_dwFVF           = dwFVF;
	m_uVertexStride   = uVertexStride_indexed;    
	m_nIBnum          = uIBNum;
	m_strResID = pszResID;

	bool	bSucceed = true;

	if(pVertexElement)
	{
		if(FAILED(m_pDevice->CreateVertexDeclaration( pVertexElement, &m_pDecl)))
			bSucceed = false;
	}

	// Comments: 3D视频显示，需要和DX11共享资源；因此使用TPRNEDER_POOL来控制
	if( bSucceed && SUCCEEDED( m_pDevice->CreateVertexBuffer( uVBufferSize_indexed,
		0,
		dwFVF,
		D3DPOOL_MANAGED,
		&m_pVB[0],
		NULL )))
	{
		void* pVBuffer;
		if(SUCCEEDED(m_pVB[0]->Lock( 0,0,&pVBuffer,0 )))
		{
			memcpy( pVBuffer,pVertexInfo_indexed,uVBufferSize_indexed );
			m_pVB[0]->Unlock();
		}
		else
			bSucceed = false;
	}
	else
		bSucceed = false;

	if( bSucceed && SUCCEEDED( m_pDevice->CreateVertexBuffer( uVBufferSize_instance,
		0,
		0,
		D3DPOOL_MANAGED,
		&m_pVB[1],
		NULL ) ) )
	{
		void* pVBuffer;
		if(SUCCEEDED(m_pVB[1]->Lock( 0,0,&pVBuffer,0 )))
		{
			memcpy( pVBuffer,pVertexInfo_instance,uVBufferSize_instance );
			m_pVB[1]->Unlock();
		}
		else
			bSucceed = false;
	}
	else
		bSucceed = false;
	if( bSucceed )
	{
		for(UINT i=0;i<uIBNum;i++)
		{
			const unsigned short* pIndexInfo = ppIndexInfo[i];
			if(pIndexInfo)
			{
				if( SUCCEEDED( m_pDevice->CreateIndexBuffer( uIBufferSize,
					0,
					D3DFMT_INDEX16,
					D3DPOOL_MANAGED,
					&(m_pIB[i]),
					NULL)))

				{
					void* pIBuffer;
					if(SUCCEEDED(m_pIB[i]->Lock( 0,0,&pIBuffer,0 )))
					{
						memcpy(pIBuffer,pIndexInfo,uIBufferSize);
						m_pIB[i]->Unlock();
					}
					else
					{
						bSucceed = false;
						break;
					}
				}
			}
		}
	}

	if( bSucceed )
		return S_OK;
	else
	{
		Destroy();
		return E_FAIL;
	}
}


BOOL CBaseMesh::DrawInstance(int nNumInstance)
{
    m_pDevice->SetStreamSource(0,m_pVB[0],0,sizeof(float) * 5);
    m_pDevice->SetStreamSourceFreq(0,D3DSTREAMSOURCE_INDEXEDDATA|nNumInstance);
    m_pDevice->SetStreamSource(1,m_pVB[1],0,sizeof(float));
    m_pDevice->SetStreamSourceFreq(1,D3DSTREAMSOURCE_INSTANCEDATA|1ul);
    m_pDevice->SetIndices(m_pIB[0]);
    m_pDevice->SetVertexDeclaration(m_pDecl);
    
    HRESULT hr = S_OK;
	hr = m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,4,0,2);    

    m_pDevice->SetStreamSourceFreq(0,1);
    m_pDevice->SetStreamSourceFreq(1,1);
	m_pDevice->SetVertexDeclaration(NULL);
	m_pDevice->SetIndices(NULL);
	m_pDevice->SetStreamSource(0, NULL, 0, 0);
	m_pDevice->SetStreamSource(1, NULL, 0, 0);

    assert(SUCCEEDED(hr));
    return (SUCCEEDED(hr));
}

BOOL CBaseMesh::DrawInstance(UINT strideInstance, int nNumInstance)
{
	m_pDevice->SetStreamSource(0,m_pVB[0],0,m_uVertexStride);
	m_pDevice->SetStreamSourceFreq(0,D3DSTREAMSOURCE_INDEXEDDATA|nNumInstance);
	m_pDevice->SetStreamSource(1,m_pVB[1],0,strideInstance);
	m_pDevice->SetStreamSourceFreq(1,D3DSTREAMSOURCE_INSTANCEDATA|1ul);
	m_pDevice->SetIndices(m_pIB[0]);
	m_pDevice->SetVertexDeclaration(m_pDecl);

	HRESULT hr = m_pDevice->DrawIndexedPrimitive(m_ePrimitiveType,0,0,m_uNumVertices,0,m_uPrimitiveCount);

	m_pDevice->SetStreamSourceFreq(0,1);
	m_pDevice->SetStreamSourceFreq(1,1);
	m_pDevice->SetVertexDeclaration(NULL);
	m_pDevice->SetIndices(NULL);
	m_pDevice->SetStreamSource(0, NULL, 0, 0);
	m_pDevice->SetStreamSource(1, NULL, 0, 0);

	return (SUCCEEDED(hr));
}


bool  CBaseMesh::Draw( const UINT iB_id, 
					  const UINT uNumVertices, 
					  const UINT uPrimitiveCount
					  )
{
	HRESULT hr = S_OK;  

	m_pDevice->SetStreamSource( 0, m_pVB[0], 0, m_uVertexStride );
	hr |= m_pDevice->SetVertexDeclaration( m_pDecl );	

	if( iB_id<8 && m_pIB[iB_id] ) 
	{
		hr |= m_pDevice->SetIndices( m_pIB[iB_id] );
		hr |= m_pDevice->DrawIndexedPrimitive( m_ePrimitiveType,
			0,
			0,
			uNumVertices,
			0,
			uPrimitiveCount );
		m_pDevice->SetIndices(NULL);
	}else
		hr |= m_pDevice->DrawPrimitive( m_ePrimitiveType,
		0,
		uPrimitiveCount);

	m_pDevice->SetVertexDeclaration(NULL);
	m_pDevice->SetStreamSource(0, NULL, 0, 0);

	if(FAILED(hr))
		assert(0);
	return SUCCEEDED(hr);

}

bool  CBaseMesh::DrawMesh( const UINT iB_id, 
						   const UINT uNumVertices, 
						   const UINT uPrimitiveCount,                           
						   LPDIRECT3DVERTEXSHADER9 pShader )
{
	m_pDevice->SetVertexShader(pShader);

	Draw( iB_id,uNumVertices, uPrimitiveCount ); 

	m_pDevice->SetVertexShader( NULL );
	return TRUE;

}

bool  CBaseMesh::DrawMeshFx( const UINT iB_id, 
						  const UINT uNumVertices, 
						  const UINT uPrimitiveCount                           
						  )
{	
	Draw( iB_id,uNumVertices, uPrimitiveCount ); 
	
	return TRUE;

}
bool  CBaseMesh::DrawMesh(const UINT iB_id, LPDIRECT3DVERTEXSHADER9 pShader)
{	
	return DrawMesh( iB_id,m_uNumVertices, m_uPrimitiveCount, pShader); 	
}

bool  CBaseMesh::DrawMeshFx( const UINT iB_id )
{
	return Draw( iB_id,m_uNumVertices, m_uPrimitiveCount); 		
}

//bool CBaseMesh::DrawMeshOffset( const UINT  iB_id,
//							   const UINT uNumVertices,
//							   const UINT uPrimitiveCount,
//							   const UINT uOffsetBytes,
//							   LPCTSTR pszShaderName )
//{
//	HRESULT hr = S_OK;  
//
//	m_pDevice->SetStreamSource( 0, m_pVB[0], uOffsetBytes, m_uVertexStride );
//	m_pEngine->SetVertexShader(pszShaderName);
//	hr = m_pDevice->SetVertexDeclaration( m_pDecl );
//
//	if( iB_id<8 && m_pIB[iB_id] ) 
//	{
//		hr |= m_pDevice->SetIndices( m_pIB[iB_id] );
//		hr |= m_pDevice->DrawIndexedPrimitive( m_ePrimitiveType,
//			0,
//			0,
//			uNumVertices,
//			0,
//			uPrimitiveCount );
//	}else
//		hr |= m_pDevice->DrawPrimitive( m_ePrimitiveType,
//		0,
//		uPrimitiveCount);
//
//	m_pEngine->SetVertexShader( NULL );
//	if(FAILED(hr))
//		assert(0);
//	return SUCCEEDED(hr);
//
//}

bool	CBaseMesh::LockVertices( void** ppVertices )
{
	if(FAILED(m_pVB[0]->Lock(0,0,ppVertices,0)))
		return false;
	return true;
}

bool	CBaseMesh::UnlockVertices()
{
	if(FAILED(m_pVB[0]->Unlock()))
		return false;
	return true;
}
