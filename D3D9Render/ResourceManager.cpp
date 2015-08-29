#include "StdAfx.h"
#include "ResourceManager.h"


typedef struct _QuadVec2UV
{
	D3DXVECTOR3  _vPos;
	D3DXVECTOR2  _vUV0;	
	D3DXVECTOR2  _vUV1;	
}QuadVec2UV,*LPQuadVec2UV;


CResourceManager::CResourceManager(void)
: m_pQuadMesh(NULL)
, m_matWorld(NULL)
, m_matPrj(NULL)
, m_matView(NULL)
{
	m_matWorld    = new D3DXMATRIXA16;
	m_matView     = new D3DXMATRIXA16;
	m_matPrj      = new D3DXMATRIXA16;
	CreateQuadMatrix(m_matWorld, m_matView, m_matPrj);
}

CResourceManager::~CResourceManager(void)
{
	delete m_matPrj;
	delete m_matView;
	delete m_matWorld;
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


		m_pQuadMesh = new CBaseMesh();
		if(m_pQuadMesh)
		{
			HRESULT hr = m_pQuadMesh->Create(lpDevice,
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
				delete m_pQuadMesh;
				m_pQuadMesh = NULL;
			}
		}
	}
	return m_pQuadMesh;
}

void CResourceManager::GetQuadMatrix( D3DXMATRIX** ppMatWorld, D3DXMATRIX** ppMatView , D3DXMATRIX** ppMatPrj )
{
	*ppMatWorld = m_matWorld;
	*ppMatView = m_matView;
	*ppMatPrj = m_matPrj;
}

void CResourceManager::CreateQuadMatrix( D3DXMATRIX* pMatWorld, D3DXMATRIX* pMatView , D3DXMATRIX* pMatPrj )
{
	D3DXMatrixIdentity( pMatWorld );
	D3DXMatrixIdentity( pMatView  );
	D3DXMatrixOrthoLH( pMatPrj, 1.0, 1.0,0.1f, 100.0f );

	D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.0f, -100.0f); 
	D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f,  0.0f );
	D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f,  0.0f );
	D3DXMatrixLookAtLH( pMatView, &vFromPt, &vLookatPt, &vUpVec );
}
