#include "StdAfx.h"
#include "Sony3DBrokenGlassRender.h"

struct VertexData
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;
	// 	D3DXVECTOR2 texcoord;
	D3DXVECTOR3 seqNumer;	// x,y: quad seq number -> split point seq number; z: quad seq number -> triangle seq number
};

struct InstanceData
{
	FLOAT	quadIndex;			// which quad it belongs to;
};

static const D3DVERTEXELEMENT9 vertexElement[] = 
{
	{0,0,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,0},
	{0,12,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_NORMAL,0},
	// 	{0,24,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,0},
	{0,24,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,0},
	{1,0 ,D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,1},
	D3DDECL_END()
};

Sony3DBrokenGlassRender::Sony3DBrokenGlassRender(void)
{
	m_pMesh = null;
	//m_uResID_fx = INVALID_RESID;

	//m_uResID_Mesh = INVALID_RESID;
	//m_resRandom1024x3 = INVALID_RESID;
	//m_resRandomARGB = INVALID_RESID;
}

Sony3DBrokenGlassRender::~Sony3DBrokenGlassRender(void)
{
}


bool Sony3DBrokenGlassRender::Init(CRenderEngine* pEngine)
{
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	m_pQuadMesh = pResMgr->CreateQuadMesh(pDevice);
	ASSERT(m_pQuadMesh);
	m_pSony3DBrokenGlassEffect = pResMgr->CreateEffect(pDevice, _T("NewEffects/GSony3DBrokenGlassfx.fx"));
	ASSERT(m_pSony3DBrokenGlassEffect);


	LPCTSTR pszMeshName = _T("ResID_Mesh");
	m_pMesh = pResMgr->FindMesh(pszMeshName);

	if( !m_pMesh )
	{
		VertexData vertexQuadData[6];
		vertexQuadData[0].position = D3DXVECTOR3(-0.5f,0.5f,0.f);
		// 			vertexQuadData[0].texcoord = D3DXVECTOR2(0.f,0.f);
		vertexQuadData[1].position = vertexQuadData[4].position = D3DXVECTOR3(-0.5f,-0.5f,0.f);
		// 			vertexQuadData[1].texcoord = vertexQuadData[4].texcoord = D3DXVECTOR2(0.f,1.f);
		vertexQuadData[2].position = vertexQuadData[3].position = D3DXVECTOR3(0.5f,0.5f,0.f);
		// 			vertexQuadData[2].texcoord = vertexQuadData[3].texcoord = D3DXVECTOR2(1.f,0.f);
		vertexQuadData[5].position = D3DXVECTOR3(0.5f,-0.5f,0.f);
		// 			vertexQuadData[5].texcoord = D3DXVECTOR2(1.f,1.f);
		for( int i=0; i<6; ++i )
			vertexQuadData[i].normal = D3DXVECTOR3(0.f,0.f,-1.f);
		vertexQuadData[0].seqNumer = D3DXVECTOR3(0.f,1.f,0.f);
		vertexQuadData[1].seqNumer = D3DXVECTOR3(0.f,0.f,0.f);
		vertexQuadData[2].seqNumer = D3DXVECTOR3(1.f,1.f,0.f);
		vertexQuadData[3].seqNumer = D3DXVECTOR3(1.f,1.f,1.f);
		vertexQuadData[4].seqNumer = D3DXVECTOR3(0.f,0.f,1.f);
		vertexQuadData[5].seqNumer = D3DXVECTOR3(1.f,0.f,1.f);

		const size_t instanceQuadNum = Sony3DBrokenGlassEffectParam::maxSizeX * Sony3DBrokenGlassEffectParam::maxSizeY;
		InstanceData instanceQuadData[instanceQuadNum];
		for( int i=0; i<instanceQuadNum; ++i )
			instanceQuadData[i].quadIndex = i;

		const WORD indexData[6] = { 0, 1, 2, 3, 4, 5 };
		const WORD *indexDataPointer = indexData;

		ID3DXMesh	*pMesh = NULL;
		m_pMesh = pResMgr->CreateMesh(pDevice, pMesh,NULL , pszMeshName);

/*
		m_uResID_Mesh = pResMan->CreateMesh(
			vertexQuadData,sizeof(vertexQuadData),sizeof(VertexData),
			instanceQuadData,sizeof(instanceQuadData),sizeof(InstanceData),
			D3DPT_TRIANGLELIST,2,0x00,vertexElement,
			&indexDataPointer,sizeof(indexData),1,
			&uuid_mesh);
			*/
	}

	if( m_resRandom1024x3 == INVALID_RESID )
	{
		m_resRandom1024x3 = pResMan->CreateTexture(1024,3,D3DUSAGE_DYNAMIC,D3DFMT_R32F,D3DPOOL_DEFAULT,&uuid_random);
		CBaseTexture* pTex = (CBaseTexture*)(m_pResMan->GetResource(m_resRandom1024x3));
		LPDIRECT3DSURFACE9	pSuf = pTex->GetSurface();
		srand(1234);
		D3DLOCKED_RECT lockRect;
		pSuf->LockRect(&lockRect,NULL,D3DLOCK_DISCARD);
		for( int row=0; row<3; ++row )
		{
			float* pBits = (float*)((BYTE*)lockRect.pBits+lockRect.Pitch*row);
			for( int i=0; i<1024; ++i )
				pBits[i] = ((rand()/(float)(RAND_MAX))-0.5f)*2.f;
		}
		pSuf->UnlockRect();
		pSuf->Release();
	}
	if( m_resRandomARGB == INVALID_RESID )
	{
		m_resRandomARGB = pResMan->CreateTexture(1024,1,D3DUSAGE_DYNAMIC,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&uuid_randomARGB);
		CBaseTexture* pTex = (CBaseTexture*)(m_pResMan->GetResource(m_resRandomARGB));
		LPDIRECT3DSURFACE9	pSuf = pTex->GetSurface();
		srand(3456);
		D3DLOCKED_RECT lockRect;
		pSuf->LockRect(&lockRect,NULL,D3DLOCK_DISCARD);
		for( int i=0; i<1024; ++i )
		{
			BYTE* pRandom = (BYTE*)(lockRect.pBits) + i * 4;
			pRandom[0] = (rand()/(float)(RAND_MAX)*255);
			pRandom[1] = (rand()/(float)(RAND_MAX)*255);
			pRandom[2] = (rand()/(float)(RAND_MAX)*255);
			pRandom[3] = (rand()/(float)(RAND_MAX)*255);
		}
		pSuf->UnlockRect();
		pSuf->Release();
	}

	return true;
}


}

void Sony3DBrokenGlassRender::Uninit()
{

}

bool Sony3DBrokenGlassRender::Render(CVideoBuffer* pDst, CVideoBuffer* pSrc1, CVideoBuffer* pSrc2, Sony3DBrokenGlassEffectParam* pParam)
{
	return true;
}