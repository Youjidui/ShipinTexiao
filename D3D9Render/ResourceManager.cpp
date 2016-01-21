#include "StdAfx.h"
#include "ResourceManager.h"

#pragma warning(disable:4244)

typedef struct _QuadVec2UV
{
	D3DXVECTOR3  _vPos;
	D3DXVECTOR2  _vUV0;	
	D3DXVECTOR2  _vUV1;	
}QuadVec2UV,*LPQuadVec2UV;


typedef struct _QuadVec
{
	D3DXVECTOR3  _vPos;
	D3DXVECTOR2  _vUV;  
}QuadVec,*LPQuadVec;


CResourceManager::CResourceManager(void)
: m_pQuadMesh(NULL)
, m_pQuadGridMesh(NULL)
, m_pQuadWHMipmapMesh(NULL)
, m_pQuadWMipmapMesh(NULL)
, m_pQuadHMipmapMesh(NULL)
, m_pQuadInstanceMesh(NULL)
, m_matPrjOrtho(NULL)
, m_matViewOrtho(NULL)
, m_matPrjPersp(NULL)
, m_matViewPersp(NULL)
{
	m_matViewOrtho     = new D3DXMATRIXA16;
	m_matPrjOrtho      = new D3DXMATRIXA16;
	m_matViewPersp     = new D3DXMATRIXA16;
	m_matPrjPersp      = new D3DXMATRIXA16;
	CreateOrthoMatrix(m_matViewOrtho, m_matPrjOrtho);
	CreatePerspectiveMatrix(m_matViewPersp, m_matPrjPersp);
}

CResourceManager::~CResourceManager(void)
{
	delete m_matPrjOrtho;
	delete m_matViewOrtho;
	delete m_matPrjPersp;
	delete m_matViewPersp;

	delete m_pQuadMesh;
	delete m_pQuadGridMesh;
	delete m_pQuadWHMipmapMesh;
	delete m_pQuadWMipmapMesh;
	delete m_pQuadHMipmapMesh;
	delete m_pQuadInstanceMesh;
}


CBaseFx* CResourceManager::CreateEffect( LPDIRECT3DDEVICE9 lpDevice, LPCTSTR lpszEffectName, const char** ppszMacros, int nMacroCount )
{
	return m_FxRes.Create(lpDevice, lpszEffectName, ppszMacros, nMacroCount);
}

CVertexShader* CResourceManager::CreateVertexShader( LPDIRECT3DDEVICE9 lpDevice, LPCTSTR lpszShaderName, const char** ppszMacros, int nMacroCount )
{
	return m_VSRes.Create(lpDevice, lpszShaderName, ppszMacros, nMacroCount);
}

CPixelShader* CResourceManager::CreatePixelShader( LPDIRECT3DDEVICE9 lpDevice, LPCTSTR lpszShaderName, const char** ppszMacros, int nMacroCount )
{
	return m_PSRes.Create(lpDevice, lpszShaderName, ppszMacros, nMacroCount);
}

void CResourceManager::GetOrthoMatrix( D3DXMATRIX** ppMatView , D3DXMATRIX** ppMatPrj )
{
	*ppMatView = m_matViewOrtho;
	*ppMatPrj = m_matPrjOrtho;
}

void CResourceManager::GetPerspectiveMatrix( D3DXMATRIX** ppMatView , D3DXMATRIX** ppMatPrj )
{
	*ppMatView = m_matViewPersp;
	*ppMatPrj = m_matPrjPersp;
}

void CResourceManager::CreateOrthoMatrix( D3DXMATRIX* pMatView , D3DXMATRIX* pMatPrj )
{
	ASSERT(pMatView);
	ASSERT(pMatPrj);
	//D3DXMatrixIdentity( pMatWorld );
	D3DXMatrixIdentity( pMatView  );
	D3DXMatrixOrthoLH( pMatPrj, 1.0, 1.0,0.1f, 100.0f );

	D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.0f, -100.0f); 
	D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f,  0.0f );
	D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f,  0.0f );
	D3DXMatrixLookAtLH( pMatView, &vFromPt, &vLookatPt, &vUpVec );
}

void CResourceManager::CreatePerspectiveMatrix( D3DXMATRIX* pMatView , D3DXMATRIX* pMatPrj )
{
	ASSERT(pMatView);
	ASSERT(pMatPrj);
	//D3DXMatrixIdentity(pMatWorld);
	D3DXVECTOR3 vEyePt( 0.0f, 0.0f,-0.5f/tanf(D3DX_PI/8) );
	D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );	
	D3DXMatrixLookAtLH( pMatView, &vEyePt, &vLookatPt, &vUpVec );

	D3DXMatrixPerspectiveFovLH( pMatPrj, D3DX_PI/4, 1.0f, 0.1f, 1000.0f );
}

CBaseMesh* CResourceManager::CreateQuadMesh(LPDIRECT3DDEVICE9 lpDevice)
{
	if(!m_pQuadMesh)
	{
		D3DVERTEXELEMENT9 _VertexElemShader[] = 
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,  0 },		
			{ 0, 12,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,  0 },
			{ 0, 20,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,  1 },
			D3DDECL_END()
		};

		QuadVec2UV QuadMesh[4];

		QuadMesh[0]._vPos  = D3DXVECTOR3( -0.5f,0.5f,0.0f );  
		QuadMesh[1]._vPos  = D3DXVECTOR3( 0.5f,0.5f,0.0f );
		QuadMesh[2]._vPos  = D3DXVECTOR3( 0.5f,-0.5f,0.0f);
		QuadMesh[3]._vPos  = D3DXVECTOR3( -0.5f,-0.5f,0.0f );

		QuadMesh[0]._vUV1 = QuadMesh[0]._vUV0  = D3DXVECTOR2( 0.0f,0.0f );
		QuadMesh[1]._vUV1 = QuadMesh[1]._vUV0  = D3DXVECTOR2( 1.0f,0.0f );
		QuadMesh[2]._vUV1 = QuadMesh[2]._vUV0  = D3DXVECTOR2( 1.0f,1.0f );
		QuadMesh[3]._vUV1 = QuadMesh[3]._vUV0  = D3DXVECTOR2( 0.0f,1.0f  );


		CBaseMesh* pMesh = new CBaseMesh();
		if(pMesh)
		{
			HRESULT hr = pMesh->Create(lpDevice,
				QuadMesh,
				sizeof(QuadMesh),
				sizeof(QuadVec2UV),
				D3DPT_TRIANGLEFAN,
				2,
				D3DFVF_XYZ|D3DFVF_TEX8,
				_VertexElemShader,
				NULL,
				0,0,
				_T("QuadMesh") );
			if(FAILED(hr))
			{
				//m_pQuadMesh->Destroy();
				delete pMesh;
			}
			else m_pQuadMesh = pMesh;
		}
	}
	return m_pQuadMesh;
}

CBaseMesh* CResourceManager::CreateQuadGridMesh( LPDIRECT3DDEVICE9 lpDevice )
{
	if(!m_pQuadGridMesh)
	{
		QuadVec pBuffer[252 * 4];   unsigned short pIndex[252 * 6];  
		for(int i=0;i<252;i++)
		{
			pBuffer[ i*4+0 ]._vPos = D3DXVECTOR3 ( -0.5f, 0.5f,(float)i);
			pBuffer[ i*4+0 ]._vUV  = D3DXVECTOR2 (  0.0f, 0.0f );
			pBuffer[ i*4+1 ]._vPos = D3DXVECTOR3 (  0.5f, 0.5f,(float)i);
			pBuffer[ i*4+1 ]._vUV  = D3DXVECTOR2 (  1.0f, 0.0f );
			pBuffer[ i*4+2 ]._vPos = D3DXVECTOR3 (  0.5f,-0.5f,(float)i);
			pBuffer[ i*4+2 ]._vUV  = D3DXVECTOR2 (  1.0f, 1.0f );
			pBuffer[ i*4+3 ]._vPos = D3DXVECTOR3 (  -0.5f,-0.5f,(float)i);
			pBuffer[ i*4+3 ]._vUV  = D3DXVECTOR2 (  0.0f, 1.0f );

			pIndex[i*6 + 0] = i*4+0;
			pIndex[i*6 + 1] = i*4+1;
			pIndex[i*6 + 2] = i*4+2;
			pIndex[i*6 + 3] = i*4+2;
			pIndex[i*6 + 4] = i*4+3;
			pIndex[i*6 + 5] = i*4+0;
		}

		D3DVERTEXELEMENT9 _VertexElemShader2[] = 
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,  0 },		
			{ 0, 12,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,  0 },		
			D3DDECL_END()
		};
		unsigned short* ppIndex[2]; ppIndex[0] = pIndex; ppIndex[1] = NULL;

		CBaseMesh* pMesh = new CBaseMesh;
		if(pMesh)
		{
			HRESULT hr = pMesh->Create(lpDevice,
				pBuffer,
				sizeof(pBuffer),
				sizeof(QuadVec),
				D3DPT_TRIANGLELIST,
				252*2,
				D3DFVF_XYZ|D3DFVF_TEX1,
				_VertexElemShader2,
				(const unsigned short**)ppIndex,
				sizeof(pIndex),1,
				_T("QuadGridMesh"));
			if(FAILED(hr))
				delete pMesh;
			else m_pQuadGridMesh = pMesh;
		}
	}
	return m_pQuadGridMesh;
}

CBaseMesh* CResourceManager::CreateQuadWHMipmapMesh( LPDIRECT3DDEVICE9 lpDevice )
{
	if(!m_pQuadWHMipmapMesh)
	{
		QuadVec QuadMipMapMesh[] =
		{
			D3DXVECTOR3( -0.5f,-0.5f,0.0f ),D3DXVECTOR2( 0.0f,1.0f ), 
			D3DXVECTOR3( -0.5f,0.5f,0.0f ),D3DXVECTOR2( 0.0f,0.0f ),
			D3DXVECTOR3( 0.5f,-0.5f,0.0f),D3DXVECTOR2( 1.0f,1.0f ),
			D3DXVECTOR3( 0.5f,0.5f,0.0f ),D3DXVECTOR2( 1.0f,0.0f ),     

			D3DXVECTOR3( -0.5f,-0.65f,1.0f ),D3DXVECTOR2( 0.0f,1.0f ), 
			D3DXVECTOR3( -0.5f,-0.5f,1.0f ),D3DXVECTOR2( 0.0f,1.0f ), 
			D3DXVECTOR3( 0.5f,-0.65f,1.0f ),D3DXVECTOR2( 1.0f,1.0f ),
			D3DXVECTOR3( 0.5f,-0.5f,1.0f),D3DXVECTOR2( 1.0f,1.0f ),                        


			D3DXVECTOR3( 0.5f,-0.5f,2.0f),D3DXVECTOR2( 1.0f,1.0f ),
			D3DXVECTOR3( 0.5f,0.5f,2.0f ),D3DXVECTOR2( 1.0f,0.0f ),     
			D3DXVECTOR3( 0.65f,-0.5f,2.0f),D3DXVECTOR2( 1.0f,1.0f ),
			D3DXVECTOR3( 0.65f,0.5f,2.0f ),D3DXVECTOR2( 1.0f,0.0f ), 

			D3DXVECTOR3( 0.5f,0.65f,0.0f),D3DXVECTOR2( 1.0f,0.0f ),
			D3DXVECTOR3( -0.5f,0.65f,0.0f ),D3DXVECTOR2( 0.0f,0.0f ), 

			D3DXVECTOR3( -0.65f,0.5f,0.0f),D3DXVECTOR2( 0.0f,0.0f ),
			D3DXVECTOR3( -0.65f,-0.5f,0.0f ),D3DXVECTOR2( 0.0f,1.0f ), 
		};
		//0 All Border
		unsigned short pMinMapIndex0[] =
		{
			//Center
			1,2,0,
			1,3,2,
			//Down
			5,7,6,
			5,6,4,
			//Right
			9,10,8,
			9,10,11,
			//Top
			13,12,3,
			13,3,1,
			//Left
			14,1,0,
			14,0,15,
		};
		D3DVERTEXELEMENT9 _VertexElemShader2[] = 
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,  0 },		
			{ 0, 12,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,  0 },		
			D3DDECL_END()
		};
		unsigned short* ppIndex[2];
		ppIndex[0] = pMinMapIndex0; ppIndex[1] = NULL;

		CBaseMesh* pMesh = new CBaseMesh;
		if(pMesh)
		{
			HRESULT hr = pMesh->Create(lpDevice,
				QuadMipMapMesh,
				sizeof(QuadMipMapMesh),
				sizeof(QuadVec),
				D3DPT_TRIANGLELIST,
				10,
				D3DFVF_XYZ|D3DFVF_TEX1,
				_VertexElemShader2,
				(const unsigned short**)ppIndex,
				sizeof(pMinMapIndex0),1,
				_T("QuadWHMipmapMesh"));
			if(FAILED(hr))	delete pMesh;
			else	m_pQuadWHMipmapMesh = pMesh;
		}
	}
	return m_pQuadWHMipmapMesh;
}



CBaseMesh* CResourceManager::CreateQuadWMipmapMesh( LPDIRECT3DDEVICE9 lpDevice )
{
	if(!m_pQuadWMipmapMesh)
	{
		QuadVec QuadMipMapMesh[] =
		{
			D3DXVECTOR3( -0.5f,-0.5f,0.0f ),D3DXVECTOR2( 0.0f,1.0f ), 
			D3DXVECTOR3( -0.5f,0.5f,0.0f ),D3DXVECTOR2( 0.0f,0.0f ),
			D3DXVECTOR3( 0.5f,-0.5f,0.0f),D3DXVECTOR2( 1.0f,1.0f ),
			D3DXVECTOR3( 0.5f,0.5f,0.0f ),D3DXVECTOR2( 1.0f,0.0f ),     

			D3DXVECTOR3( -0.5f,-0.65f,1.0f ),D3DXVECTOR2( 0.0f,1.0f ), 
			D3DXVECTOR3( -0.5f,-0.5f,1.0f ),D3DXVECTOR2( 0.0f,1.0f ), 
			D3DXVECTOR3( 0.5f,-0.65f,1.0f ),D3DXVECTOR2( 1.0f,1.0f ),
			D3DXVECTOR3( 0.5f,-0.5f,1.0f),D3DXVECTOR2( 1.0f,1.0f ),                        


			D3DXVECTOR3( 0.5f,-0.5f,2.0f),D3DXVECTOR2( 1.0f,1.0f ),
			D3DXVECTOR3( 0.5f,0.5f,2.0f ),D3DXVECTOR2( 1.0f,0.0f ),     
			D3DXVECTOR3( 0.65f,-0.5f,2.0f),D3DXVECTOR2( 1.0f,1.0f ),
			D3DXVECTOR3( 0.65f,0.5f,2.0f ),D3DXVECTOR2( 1.0f,0.0f ), 

			D3DXVECTOR3( 0.5f,0.65f,0.0f),D3DXVECTOR2( 1.0f,0.0f ),
			D3DXVECTOR3( -0.5f,0.65f,0.0f ),D3DXVECTOR2( 0.0f,0.0f ), 

			D3DXVECTOR3( -0.65f,0.5f,0.0f),D3DXVECTOR2( 0.0f,0.0f ),
			D3DXVECTOR3( -0.65f,-0.5f,0.0f ),D3DXVECTOR2( 0.0f,1.0f ), 
		};
		//1 Left Right Border
		unsigned short pMinMapIndex1[] =
		{
			//Center
			1,2,0,
			1,3,2,
			//Right
			9,10,8,
			9,10,11,
			//Left
			14,1,0,
			14,0,15,
		};
		D3DVERTEXELEMENT9 _VertexElemShader2[] = 
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,  0 },		
			{ 0, 12,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,  0 },		
			D3DDECL_END()
		};
		unsigned short* ppIndex[2];
		ppIndex[0] = pMinMapIndex1;
		ppIndex[1] = NULL;

		CBaseMesh* pMesh = new CBaseMesh;
		if(pMesh)
		{
			HRESULT hr = pMesh->Create(lpDevice,
				QuadMipMapMesh,
				sizeof(QuadMipMapMesh),
				sizeof(QuadVec),
				D3DPT_TRIANGLELIST,
				6,
				D3DFVF_XYZ|D3DFVF_TEX1,
				_VertexElemShader2,
				(const unsigned short**)ppIndex,
				sizeof(pMinMapIndex1),1,
				_T("QuadWMipmapMesh"));
			if(FAILED(hr))	delete pMesh;
			else	m_pQuadWMipmapMesh = pMesh;
		}
	}
	return m_pQuadWMipmapMesh;
}


CBaseMesh* CResourceManager::CreateQuadHMipmapMesh( LPDIRECT3DDEVICE9 lpDevice )
{
	if(!m_pQuadHMipmapMesh)
	{
		QuadVec QuadMipMapMesh[] =
		{
			D3DXVECTOR3( -0.5f,-0.5f,0.0f ),D3DXVECTOR2( 0.0f,1.0f ), 
			D3DXVECTOR3( -0.5f,0.5f,0.0f ),D3DXVECTOR2( 0.0f,0.0f ),
			D3DXVECTOR3( 0.5f,-0.5f,0.0f),D3DXVECTOR2( 1.0f,1.0f ),
			D3DXVECTOR3( 0.5f,0.5f,0.0f ),D3DXVECTOR2( 1.0f,0.0f ),     

			D3DXVECTOR3( -0.5f,-0.65f,1.0f ),D3DXVECTOR2( 0.0f,1.0f ), 
			D3DXVECTOR3( -0.5f,-0.5f,1.0f ),D3DXVECTOR2( 0.0f,1.0f ), 
			D3DXVECTOR3( 0.5f,-0.65f,1.0f ),D3DXVECTOR2( 1.0f,1.0f ),
			D3DXVECTOR3( 0.5f,-0.5f,1.0f),D3DXVECTOR2( 1.0f,1.0f ),                        


			D3DXVECTOR3( 0.5f,-0.5f,2.0f),D3DXVECTOR2( 1.0f,1.0f ),
			D3DXVECTOR3( 0.5f,0.5f,2.0f ),D3DXVECTOR2( 1.0f,0.0f ),     
			D3DXVECTOR3( 0.65f,-0.5f,2.0f),D3DXVECTOR2( 1.0f,1.0f ),
			D3DXVECTOR3( 0.65f,0.5f,2.0f ),D3DXVECTOR2( 1.0f,0.0f ), 

			D3DXVECTOR3( 0.5f,0.65f,0.0f),D3DXVECTOR2( 1.0f,0.0f ),
			D3DXVECTOR3( -0.5f,0.65f,0.0f ),D3DXVECTOR2( 0.0f,0.0f ), 

			D3DXVECTOR3( -0.65f,0.5f,0.0f),D3DXVECTOR2( 0.0f,0.0f ),
			D3DXVECTOR3( -0.65f,-0.5f,0.0f ),D3DXVECTOR2( 0.0f,1.0f ), 
		};
		//2 Top Bottom Border
		unsigned short pMinMapIndex2[] =
		{
			//Center
			1,2,0,
			1,3,2,
			//Down
			5,7,6,
			5,6,4,

			//Top
			13,12,3,
			13,3,1,            
		};
		D3DVERTEXELEMENT9 _VertexElemShader2[] = 
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,  0 },		
			{ 0, 12,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,  0 },		
			D3DDECL_END()
		};
		unsigned short* ppIndex[2];
		ppIndex[0] = pMinMapIndex2;
		ppIndex[1] = NULL;

		CBaseMesh* pMesh = new CBaseMesh;
		if(pMesh)
		{
			HRESULT hr = pMesh->Create(lpDevice,
				QuadMipMapMesh,
				sizeof(QuadMipMapMesh),
				sizeof(QuadVec),
				D3DPT_TRIANGLELIST,
				10,
				D3DFVF_XYZ|D3DFVF_TEX1,
				_VertexElemShader2,
				(const unsigned short**)ppIndex,
				sizeof(pMinMapIndex2),1,
				_T("QuadHMipmapMesh"));
			if(FAILED(hr))	delete pMesh;
			else	m_pQuadHMipmapMesh = pMesh;
		}
	}
	return m_pQuadHMipmapMesh;
}

CBaseMesh* CResourceManager::CreateQuadInstanceMesh( LPDIRECT3DDEVICE9 lpDevice )
{
	if(!m_pQuadInstanceMesh)
	{
		//Create Instance Quad Mesh
		struct INSTANCEVERTEX
		{
			D3DXVECTOR3 position;
			D3DXVECTOR2 uv;
		};    

		LPDIRECT3DVERTEXDECLARATION9 pDcl = NULL;
		LPDIRECT3DVERTEXBUFFER9 pVB[2] = {NULL,NULL};
		LPDIRECT3DINDEXBUFFER9  pIB = NULL;

		D3DVERTEXELEMENT9 declInstance[] = 
		{	
			{ 0, 0,     D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION,  0 },
			{ 0, 12,    D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,  0 },
			{ 1, 0,     D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,  1 },	
			D3DDECL_END()
		};

		lpDevice->CreateVertexDeclaration( declInstance,&pDcl);

		if( FAILED( lpDevice->CreateVertexBuffer( 4 * sizeof(INSTANCEVERTEX),
			0, 0,
			D3DPOOL_DEFAULT, &pVB[0], NULL ) ) )
		{
			return FALSE;
		}

		HRESULT hr = S_OK;

		INSTANCEVERTEX* pVertices;
		if( FAILED( pVB[0]->Lock( 0, 0, (void**)&pVertices, 0 ) ) )
			return NULL;
		pVertices[0].position = D3DXVECTOR3(-0.5f,-0.5f,0.0f);
		pVertices[0].uv = D3DXVECTOR2(0.0f,1.0f);

		pVertices[1].position = D3DXVECTOR3(-0.5f,0.5f,0.0f);
		pVertices[1].uv = D3DXVECTOR2(0.0f,0.0f);

		pVertices[2].position = D3DXVECTOR3(0.5f,-0.5f,0.0f);
		pVertices[2].uv = D3DXVECTOR2(1.0f,1.0f);

		pVertices[3].position = D3DXVECTOR3(0.5f,0.5f,0.0f);
		pVertices[3].uv = D3DXVECTOR2(1.0f,0.0f);

		pVB[0]->Unlock();

		if(FAILED(hr = lpDevice->CreateIndexBuffer(6 * sizeof(WORD),0,D3DFMT_INDEX16,D3DPOOL_DEFAULT,
			&pIB,NULL)))
			return NULL;

		WORD * pWord = NULL;
		pIB->Lock(0,0,(void**)&pWord,0);
		pWord[0] = 0;
		pWord[1] = 1;
		pWord[2] = 2;

		pWord[3] = 1;
		pWord[4] = 3;
		pWord[5] = 2;
		pIB->Unlock();

		if(FAILED(hr = lpDevice->CreateVertexBuffer(1000 * sizeof(float),0,0,D3DPOOL_DEFAULT,
			&pVB[1],NULL)))
			return NULL;

		float *pVB1 = NULL;
		pVB[1]->Lock(0,0,(void**)&pVB1,0);

		for(int i = 0;i<1000;i ++)
			pVB1[i] =  i;
		pVB[1]->Unlock();

		CBaseMesh* pMesh    = new CBaseMesh;
		if( SUCCEEDED( pMesh->Create( lpDevice,
			pVB,
			pIB,
			pDcl,
			_T("QuadInstanceMesh"))))
		{
			m_pQuadInstanceMesh = pMesh;
		}
		else
			delete pMesh;
	}
	return m_pQuadInstanceMesh;
}

CBaseMesh* CResourceManager::CreateMesh( LPDIRECT3DDEVICE9 pDevice, LPD3DXMESH pMesh, LPCTSTR lpszMeshName )
{
	return m_MeshRes.Create(pDevice, pMesh, lpszMeshName);
}

CBaseMesh* CResourceManager::CreateMesh( LPDIRECT3DDEVICE9 pDevice, LPD3DXMESH pMesh, LPDIRECT3DVERTEXDECLARATION9 pVertexDecl, LPCTSTR lpszMeshName )
{
	return m_MeshRes.Create(pDevice, pMesh, pVertexDecl, lpszMeshName);
}

CBaseMesh* CResourceManager::CreateMesh(LPDIRECT3DDEVICE9 pDevice,
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
										LPCTSTR          pszMeshName)
{
	return m_MeshRes.Create(
		pDevice, pVertexInfo, uVBufferSize, uVertexStride
		, ePrimitiveType, uPrimitiveCount, dwFVF, pVertexElement
		, ppIndexInfo, uIBufferSize, uIBNum, pszMeshName
		);
}

CBaseMesh* CResourceManager::CreateMesh( LPDIRECT3DDEVICE9 pDevice,
										void* pVertexInfo_indexed, UINT uVBufferSize_indexed, UINT uVertexStride_indexed, 
										void* pVertexInfo_instance, UINT uVBufferSize_instance, UINT uVertexStride_instance, 
										D3DPRIMITIVETYPE ePrimitiveType, UINT uPrimitiveCount,
										DWORD dwFVF, const D3DVERTEXELEMENT9* pVertexElement, 
										const unsigned short** ppIndexInfo, UINT uIBufferSize, UINT uIBNum,
										LPCTSTR pszMeshName )
{
	return m_MeshRes.Create(pDevice,
		pVertexInfo_indexed,
		uVBufferSize_indexed,
		uVertexStride_indexed,
		pVertexInfo_instance,
		uVBufferSize_instance,
		uVertexStride_instance,
		ePrimitiveType,
		uPrimitiveCount,
		dwFVF,
		pVertexElement,
		ppIndexInfo,
		uIBufferSize,
		uIBNum,
		pszMeshName
		);
}



CBaseMesh* CResourceManager::FindMesh( LPCTSTR pszResName )
{
	return m_MeshRes.Find(pszResName);
}


CPixelShader* CResourceManager::CreateBlendPixelShader( LPDIRECT3DDEVICE9 pDevice, BlendMode eMode )
{
	CPixelShader* pShader = NULL;
	switch(eMode)
	{
	case BLENDMODE_ADD:
		pShader = //m_pResManager->m_BlendMode_AddPS;
			CreatePixelShader(pDevice, _T("BlendShaders/BlendMode_AddPS.PSH"));
		break;
	case BLENDMODE_MUL:
		pShader = //m_pResManager->m_BlendMode_MulPS;
		CreatePixelShader(pDevice, _T("BlendShaders/BlendMode_MulPS.PSH"));
		break;
	case BLENDMODE_SUB:
		pShader = //m_pResManager->m_BlendMode_SubPS;
		CreatePixelShader(pDevice, _T("BlendShaders/BlendMode_SubPS.PSH"));
		break;
	case BLENDMODE_ALPHA:
		pShader = //m_pResManager->m_BlendMode_AlphaBlendPS;
		CreatePixelShader(pDevice, _T("BlendShaders/BlendMode_AlphaBlendPS.PSH"));
		break;
	case BLENDMODE_SCREEN:
		pShader = //m_pResManager->m_BlendMode_ScreenPS;
		CreatePixelShader(pDevice, _T("BlendShaders/BlendMode_ScreenPS.PSH"));
		break;
	case BLENDMODE_DIV:
		pShader = //m_pResManager->m_BlendMode_DivPS;
		CreatePixelShader(pDevice, _T("BlendShaders/BlendMode_DivPS.PSH"));
		break;
	case BLENDMODE_DARKEN:
		pShader = //m_pResManager->m_BlendMode_DarkenPS;
			CreatePixelShader(pDevice, _T("BlendShaders/BlendMode_DarkenPS.PSH"));
		break;
	case BLENDMODE_LIGHTEN:
		pShader = //m_pResManager->m_BlendMode_LightenPS;
		CreatePixelShader(pDevice, _T("BlendShaders/BlendMode_LightenPS.PSH"));
		break;
	case BLENDMODE_COLORBURN:
		pShader = //m_pResManager->m_BlendMode_ColorBurnPS;
			CreatePixelShader(pDevice, _T("BlendShaders/BlendMode_ColorBurnPS.PSH"));
		break;
	case BLENDMODE_COLORDODGE:
		pShader = //m_pResManager->m_BlendMode_ColorDodgePS;
		CreatePixelShader(pDevice, _T("BlendShaders/BlendMode_ColorDodgePS.PSH"));
		break;
	case BLENDMODE_SOFTLIGHT:
		pShader = //m_pResManager->m_BlendMode_SoftLightPS;
		CreatePixelShader(pDevice, _T("BlendShaders/BlendMode_SoftLightPS.PSH"));
		break;
	case BLENDMODE_HARDLIGHT:
		pShader = //m_pResManager->m_BlendMode_HardLightPS;
		CreatePixelShader(pDevice, _T("BlendShaders/BlendMode_HardLightPS.PSH"));
		break;
	case BLENDMODE_DIFFERENCE:
		pShader= //m_pResManager->m_BlendMode_DifferencePS;
		CreatePixelShader(pDevice, _T("BlendShaders/BlendMode_DifferencePS.PSH"));
		break;
	default:
		pShader = //m_pResManager->m_BlendMode_AlphaBlendPS;
		CreatePixelShader(pDevice, _T("BlendShaders/BlendMode_AlphaBlendPS.PSH"));
		break;	   
	}
	return pShader;
}
