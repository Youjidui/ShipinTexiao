#include "StdAfx.h"
#include "ResourceManager.h"


typedef struct _QuadVec2UV
{
	D3DXVECTOR3  _vPos;
	D3DXVECTOR2  _vUV0;	
	D3DXVECTOR2  _vUV1;	
}QuadVec2UV,*LPQuadVec2UV;


CResourceManager::CResourceManager(void)
{
}

CResourceManager::~CResourceManager(void)
{
}


CBaseFx* CResourceManager::CreateEffect( LPDIRECT3DDEVICE9 lpDevice, LPCTSTR lpszEffectName )
{
	return m_FxRes.Create(lpDevice, lpszEffectName);
}

CVertexShader* CResourceManager::CreateVertexShader( LPDIRECT3DDEVICE9 lpDevice, LPCTSTR lpszShaderName )
{
	return m_VSRes.Create(lpDevice, lpszShaderName);
}

CPixelShader* CResourceManager::CreatePixelShader( LPDIRECT3DDEVICE9 lpDevice, LPCTSTR lpszShaderName )
{
	return m_PSRes.Create(lpDevice, lpszShaderName);
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
				NULL );
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
