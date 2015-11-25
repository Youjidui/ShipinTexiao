#include "StdAfx.h"
#include "PageRollRender.h"
#include "../Utility/SafeDelete.h"


#define GRID 2
#define FAN  8
#define NumGrid 101


#pragma warning(disable: 4244)


CPageRollRender::CPageRollRender(void)
: m_pEngine(NULL)
, m_pQuadMesh(NULL)
, m_pMesh(NULL)
//, m_pVertexDecl(NULL)
, m_pEffect(NULL)
, m_pTrans_Draw_BG_Effect(NULL)
, m_pMipMapGenerator(NULL)
{
}

CPageRollRender::~CPageRollRender(void)
{
	Uninit();
}

bool CPageRollRender::Init( CRenderEngine* pEngine )
{
	HRESULT hr = E_FAIL;
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = pEngine->GetDevice();
	CResourceManager* pResMan = pEngine->GetResourceManager();

	m_pQuadMesh = pResMan->CreateQuadMesh(pDevice);
	ASSERT(m_pQuadMesh);

	m_pEffect = pResMan->CreateEffect(pDevice, _T("NewEffects/GPageRollTrans.fx"));
	ASSERT(m_pEffect);

	m_pTrans_Draw_BG_Effect = pResMan->CreateEffect(pDevice, _T("NewEffects/fx_Trans_Draw_BG.fx"));
	ASSERT(m_pTrans_Draw_BG_Effect);

	hr = CreateMesh();
	ASSERT(SUCCEEDED(hr));

	m_pMipMapGenerator = new CMipMapGenerator;
	m_pMipMapGenerator->Init(pEngine);

	return true;
}

void CPageRollRender::Uninit()
{
	delete m_pMipMapGenerator;
	m_pMipMapGenerator = NULL;
}

bool CPageRollRender::Render(CVideoBuffer*pDstDef, CVideoBuffer *pSrcA, CVideoBuffer *pSrcB, FxParamBase* pParamBase)
{
	HRESULT hr = E_FAIL;
	RESET_RENDER_TARGET(m_pEngine);
	SET_DEPTH_STENCIL(m_pEngine);

	PageRollFxParam* pParam = (PageRollFxParam*)pParamBase;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	CVideoBufferManager* pVM = m_pEngine->GetVideoBufferManager();

	if(pParam->structTrans.bReverse)
	{
		std::swap(pSrcA, pSrcB);
	}

	//const Custom_Profile* pProfile = m_pEngine->GetCurProfile();	    
	//m_fAspect = m_pResMan->GetAspect() * pProfile->nEditWidth  / (float)(pProfile->nEditHeight * m_pResMan->GetAspectVerifyCoef());    
	int nEditWidth, nEditHeight;
	m_pEngine->GetTargetVideoSize(nEditWidth, nEditHeight);
	float fAspect = nEditWidth * 1.0f / nEditHeight;

	D3DXMatrixOrthoLH(&m_matProject,fAspect,1.0f,0.5f,100.0f);

	CVideoBuffer* pMipMap[2] = {NULL,NULL};
	CVideoBuffer* pYUVA = pDstDef;
	//pMipMap[0] = m_pResMan->GetTemp_Video(0,TRUE);
	VideoBufferInfo mipMapInfo = pDstDef->GetVideoBufferInfo();
	mipMapInfo.eUsage = VideoBufferInfo::_IN_OUT_WITH_MIPMAP;
	pMipMap[0] = pVM->CreateVideoBuffer(mipMapInfo);
	//Generate MipMap Is Very Slower,don't need don't generate
	if(!pParam->structRear.bUseForeGround && pParam->structRear.fMatteRatio < 1.0f)
	{
		//pMipMap[1] = m_pResMan->GetTemp_Video(1,TRUE);
		pMipMap[1] = pVM->CreateVideoBuffer(mipMapInfo);
	}
	{

		//pYUVA = (CBaseTexture*)pDstDef->pContainer;
		//GenerateMipMap(pSrcDef[0],pMipMap[0],TRUE);
		//if(pMipMap[1])
		//	GenerateMipMap(pSrcDef[1],pMipMap[1],TRUE);
		m_pMipMapGenerator->Render(pMipMap[0], pSrcA, pParamBase);
		if(pMipMap[1])
			m_pMipMapGenerator->Render(pMipMap[1], pSrcB, pParamBase);
	}	
	//LPDIRECT3DSURFACE9 pRtSurf = NULL;
	//pYUVA->GetTexture()->GetSurfaceLevel(0,&pRtSurf);
	//hr = pDevice->SetRenderTarget(0,pRtSurf);
	//SAFE_RELEASE(pRtSurf);
	//hr = pDevice->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,0x00008080,1.0f,0);
	//D3DVIEWPORT9 vPort;
	//hr = pDevice->GetViewport(&vPort);
	//vPort.X = 0;
	//vPort.Y = 0;
	//vPort.Width = nEditWidth;
	//vPort.Height = nEditHeight;
	//hr = pDevice->SetViewport(&vPort);
	m_pEngine->SetRenderTarget(pYUVA);

	m_pEffect->SetTexture("g_txFrontPicture",pMipMap[0]->GetTexture());
	if(pMipMap[1])
		m_pEffect->SetTexture("g_txRearPicture",pMipMap[1]->GetTexture());		


	D3DXCOLOR crMatterColor = pParam->structRear.crMaatte;
	//D3DXVECTOR4 vAlpha(ppSrcDef[0]->fAlphaValue,ppSrcDef[1]->fAlphaValue,0.0f,0.0f);
	D3DXVECTOR4 vAlpha(1.0f, 1.0f, 0.0f, 0.0f);

	//RGBA2(pSrcDef[0]->bufferFormat == FMT_RGBA32?FMT_RGBA32 : FMT_YUVA32,&crMatterColor,&crMatterColor);

	D3DXVECTOR4 vHighLight(
		201.f - pParam->structLight.fFrontSmoothness * 10.f, pParam->structLight.fFrontHighLight/200.f,
		201.f - pParam->structLight.fRearSmoothness * 10.f, pParam->structLight.fRearHighLight/200.f); 
	D3DXVECTOR4 vMatterColor = D3DXVECTOR4((float*)crMatterColor) ; 

	m_pEffect->SetVector("g_vMatterColor",&vMatterColor);
	m_pEffect->SetVector("g_vHightLight",&vHighLight);
	m_pEffect->SetFloat("g_fDiffuseCo",pParam->structLight.fLightRatio);		
	m_pEffect->SetVector("g_vAlpha",&vAlpha);

	D3DXMATRIXA16	matTex;
	D3DXMatrixIdentity(&matTex);
	int pMipMap0_GetImagesInfo_Base_Width = mipMapInfo.nAllocWidth;
	int pMipMap0_GetImagesInfo_Base_Height = mipMapInfo.nAllocHeight;
	matTex._11 = nEditWidth * 1.0f / pMipMap0_GetImagesInfo_Base_Width;
	matTex._22 = nEditHeight * 1.0f / pMipMap0_GetImagesInfo_Base_Height;
	matTex._31 = 0.5f / pMipMap0_GetImagesInfo_Base_Width;
	//if(pParam->bOdd)
	//	matTex._32 = 0.5f / pMipMap[0]->GetImagesInfo()->Base_Height;	

	Draw(pParam, &matTex);

	D3DXSaveSurfaceToFile(L"draw.bmp",D3DXIFF_BMP,pYUVA->GetSurface(), NULL, NULL);

	Trans_Draw_BG(pSrcB, 1);

	//if(pSrcDef[0]->IsYUV16Buffer())
	//{
	//	RECT rcImage;
	//	SetRect(&rcImage,0,0,pProfile->nEditWidth,pProfile->nEditHeight);
	//	m_pEngine->ConvertYUVA_YUYV(pYUVA,&rcImage,pDstDef);
	//}

	//D3DXSaveSurfaceToFile(L"trans_draw_bg.bmp",D3DXIFF_BMP,pYUVA->GetSurface(), NULL, NULL);

	for(int i = 0; i < sizeof(pMipMap)/sizeof(pMipMap[0]); ++i)
	{
		if(pMipMap[i])
		{
			pVM->ReleaseVideoBuffer(pMipMap[i]);
			pMipMap[i] = NULL;
		}
	}

	return true;
}

bool CPageRollRender::Draw(PageRollFxParam* pParam, D3DXMATRIX*	matTex)
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	CVideoBufferManager* pVM = m_pEngine->GetVideoBufferManager();

	D3DXMATRIXA16 matWorld, matTrans, matAspect;

	// Begin the scene
	if( SUCCEEDED( pDevice->BeginScene() ) )
	{		
		float fAngle = D3DXToRadian(180 - pParam->structGeometry.fAngle);
		D3DXMatrixRotationZ( &matWorld, fAngle );
		//         D3DXMatrixRotationZ(&matPrevRotate,D3DXToRadian(90 + pParam->structGeometry.fAngle)); 
		//         m_pEffect->SetMatrix("g_matPrevRotate",&matPrevRotate);       
		int nEditWidth, nEditHeight;
		m_pEngine->GetTargetVideoSize(nEditWidth, nEditHeight);
		float fAspect = nEditWidth * 1.0f / nEditHeight;
		D3DXVECTOR4 vCorner[4] = 
		{
			D3DXVECTOR4(-0.5f * fAspect,-0.50f,0.0f,1.0f),
			D3DXVECTOR4(-0.5f * fAspect,0.50f,0.0f,1.0f),
			D3DXVECTOR4(0.5f * fAspect,-0.50f,0.0f,1.0f),
			D3DXVECTOR4(0.5f * fAspect,0.50f,0.0f,1.0f),
		};

		D3DXMatrixIdentity(&matTrans);
		//if(!m_pParam->bOdd)
		//	D3DXMatrixTranslation(&matTrans, 0.0f, 0.5f / nEditHeight, 0.0f);
		D3DXVec4TransformArray(vCorner,sizeof(D3DXVECTOR4),vCorner,sizeof(D3DXVECTOR4),&matWorld,4);
		float fMin = 10.0f,fMax = -10.0f;
		for(int i = 0; i < 4;i ++)
		{
			fMin = min(fMin,vCorner[i].x);
			fMax = max(fMax,vCorner[i].x);
		}
		fMax += pParam->structGeometry.fRadius / fAspect;
		float fProcess = fMin + (fMax - fMin) * pParam->structTrans.fTransition;

		D3DXVECTOR4 vMisc = D3DXVECTOR4(fProcess, 
			pParam->structGeometry.fRadius / (pParam->bPageRoll ? 2.0f : 1.0f),
			pParam->structRear.fMatteRatio,
			pParam->structRear.bUseForeGround);
		m_pEffect->SetVector("g_vMisc",&vMisc);

		D3DXMatrixScaling(&matAspect, fAspect,1.0f,1.0f);
		m_pEffect->SetMatrix("g_matRotate",&(matAspect * matWorld));
		D3DXMatrixRotationZ( &matWorld, -fAngle );
		m_pEffect->SetMatrix("g_matWorld",&matWorld);
		D3DXMATRIX *matView = NULL, *matProj= NULL;
		pResMgr->GetPerspectiveMatrix( &matView, &matProj);		//CGPURender的子类
		m_pEffect->SetMatrix("g_matWVP", &(matWorld * matTrans * *matView * m_matProject));	

		D3DXVECTOR4 vLightDir = D3DXVECTOR4(cosf(D3DXToRadian( pParam->structLight.fAnglePhi)),0.0f,
			sinf(D3DXToRadian( pParam->structLight.fAnglePhi)),0.0f);
		D3DXVECTOR4 vEyeDir = D3DXVECTOR4(cosf(D3DXToRadian( pParam->structLight.fAngleTheta)),
			0.0f,sinf(D3DXToRadian( pParam->structLight.fAngleTheta)),0.0f);
		m_pEffect->SetVector("g_vLightDir",&vLightDir);
		m_pEffect->SetVector("g_vEyeDir",&vEyeDir);

		//matTexRear = matTexFront = *matTex;
		//m_pEffect->SetMatrix("g_matTexFront",&matTexFront);
		//m_pEffect->SetMatrix("g_matTexRear",&matTexRear);		
		m_pEffect->SetMatrix("g_matTexFront", matTex);
		m_pEffect->SetMatrix("g_matTexRear", matTex);		

		LPDIRECT3DVERTEXBUFFER9 pVB = NULL;	
		LPDIRECT3DINDEXBUFFER9	pIB = NULL;
		IDirect3DVertexDeclaration9 * pVertexDecl = NULL;
		ASSERT(m_pMesh->GetMesh());
		m_pMesh->GetMesh()->GetVertexBuffer(&pVB);
		m_pMesh->GetMesh()->GetIndexBuffer(&pIB);
		pVertexDecl =  m_pMesh->GetVertexDeclarationPtr();
		ASSERT(NULL != pVertexDecl);
		//pVertexDecl = m_pVertexDecl;

		pDevice->SetStreamSource(0,pVB,0,sizeof(D3DXVECTOR3));	
		pDevice->SetIndices(pIB);
		pDevice->SetVertexDeclaration(pVertexDecl);
		SAFE_RELEASE(pVB);
		SAFE_RELEASE(pIB);

		if(pParam->bPageRoll)
			m_pEffect->SetTechnique("PageRoll");
		else
			m_pEffect->SetTechnique("PageTurn");

		UINT cPass,uPass = 0;				
		m_pEffect->Begin(&cPass,0);

		m_pEffect->BeginPass(uPass);				
		pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,NumGrid * NumGrid,0,(NumGrid - 1) * (NumGrid - 1) * 2);
		m_pEffect->EndPass();

		m_pEffect->End();

		// End the scene
		pDevice->EndScene();
	}	

	return TRUE;
}


//void  CGPURender::Trans_Draw_BG(TP_VBufferDef * pBGDef,const BOOL bForPageRoll)
void  CPageRollRender::Trans_Draw_BG(CVideoBuffer* pBGDef,const BOOL bForPageRoll)
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	CVideoBufferManager* pVM = m_pEngine->GetVideoBufferManager();
	D3DXMATRIXA16 matWorld,matCombine,matTex;
	// Begin the scene
	if( SUCCEEDED( pDevice->BeginScene() ) )
	{
		//Draw BackGround
		UINT cPass;
		D3DVIEWPORT9 vPort;
		pDevice->GetViewport(&vPort);
		::GenerateMatrix(pBGDef, &matTex, mat_Image);
		const VideoBufferInfo& vbInfo = pBGDef->GetVideoBufferInfo();
		float pBGDef_OffsetX = 0.0f, pBGDef_OffsetY = 0.0f;
		GenerateWorld(vbInfo.nWidth, vbInfo.nHeight,
			&D3DXVECTOR2(pBGDef_OffsetX, pBGDef_OffsetY),
			&D3DXVECTOR2(vPort.Width,vPort.Height), &matWorld);
		D3DXMATRIX *matView = NULL, *matProj= NULL;
		pResMgr->GetPerspectiveMatrix( &matView, &matProj);		//CGPURender的子类
		matCombine = matWorld * *matView * *matProj;
		m_pTrans_Draw_BG_Effect->SetTechnique("BG");
		m_pTrans_Draw_BG_Effect->SetMatrix("g_matWorldViewProj",&matCombine);
		m_pTrans_Draw_BG_Effect->SetMatrix("g_matTexBG",&matTex);
		m_pTrans_Draw_BG_Effect->SetTexture("g_txBG",pBGDef->GetTexture());
		float pBGDef_fAlphaValue = 1.0f;
		m_pTrans_Draw_BG_Effect->SetFloat("g_fAlpha",pBGDef_fAlphaValue);

		m_pTrans_Draw_BG_Effect->Begin(&cPass,0);
		m_pTrans_Draw_BG_Effect->BeginPass(bForPageRoll ? 0 : 1);
		m_pQuadMesh->DrawMeshFx();
		m_pTrans_Draw_BG_Effect->EndPass();
		m_pTrans_Draw_BG_Effect->End();
		// End the scene
		pDevice->EndScene();
	}	
}

//void CGPURender::GenerateWorld(const RECT * pSrcRcImage,const D3DXVECTOR2 * pOffset,const D3DXVECTOR2 * pDstSize,D3DXMATRIXA16 * pMatWorld)
void CPageRollRender::GenerateWorld(int nSrcWidth, int nSrcHeight, const D3DXVECTOR2 * pOffset, const D3DXVECTOR2 * pDstSize, D3DXMATRIXA16 * pMatWorld)
{
	D3DXMatrixIdentity(pMatWorld);

	pMatWorld->_11 = nSrcWidth  / (float)pDstSize->x;
	pMatWorld->_22 = nSrcHeight / (float)pDstSize->y;
	pMatWorld->_41 = -0.5f + pMatWorld->_11 / 2.0f + pOffset->x / (float)pDstSize->x;
	pMatWorld->_42 = 0.5f  - pMatWorld->_22 / 2.0f - pOffset->y / (float)pDstSize->y;      
}


HRESULT CPageRollRender::CreateMesh()
{
	HRESULT hr = E_FAIL;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	LPCTSTR pszMeshName = _T("PageRollMesh");
	m_pMesh = pResMgr->FindMesh(pszMeshName);
	if(!m_pMesh)
	{
		D3DVERTEXELEMENT9 VertexElem[] = 
		{		
			{ 0, 0,     D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION,  0 },		        	
			D3DDECL_END()
		};
		LPDIRECT3DVERTEXDECLARATION9 pVertexDecl = NULL;
		hr =  pDevice->CreateVertexDeclaration( VertexElem, &pVertexDecl );
		ASSERT(SUCCEEDED(hr));
		//m_pVertexDecl = pVertexDecl;

		ID3DXMesh	*pMesh = NULL;
		hr = (D3DXCreateMesh((NumGrid - 1 ) * (NumGrid - 1) * 2,NumGrid * NumGrid,
			D3DXMESH_DYNAMIC|D3DXMESH_WRITEONLY, VertexElem, pDevice, &pMesh));
		ASSERT(SUCCEEDED(hr));

		D3DXVECTOR3* pVertices;
		WORD * pIB = NULL;
		hr = pMesh->LockVertexBuffer(0,(LPVOID*)&pVertices);
		ASSERT(SUCCEEDED(hr));
		hr = pMesh->LockIndexBuffer(0,(LPVOID*)&pIB);
		ASSERT(SUCCEEDED(hr));

		for(int y = 0; y < NumGrid;y ++)
		{
			for(int x = 0; x < NumGrid;x ++)
			{
				pVertices[x + y * NumGrid] = D3DXVECTOR3(-0.5f + x / float(NumGrid - 1),0.5f - y / float(NumGrid - 1),0.0f);
			}
		}

		int nIndex = 0;    
		for(int y = 0; y < NumGrid - 1;y ++)
		{
			for(int x = 0; x < NumGrid - 1;x ++)
			{
				pIB[y * (NumGrid -1) * 6 + x * 6 + 0] = x + y * NumGrid;
				pIB[y * (NumGrid -1) * 6 + x * 6 + 1] = x + 1 + (y + 1) * NumGrid ;
				pIB[y * (NumGrid -1) * 6 + x * 6 + 2] = x  + (y + 1) * NumGrid ;

				pIB[y * (NumGrid -1) * 6 + x * 6 + 3] = x + y * NumGrid;
				pIB[y * (NumGrid -1) * 6 + x * 6 + 4] = x + 1 + (y + 0) * NumGrid ;
				pIB[y * (NumGrid -1) * 6 + x * 6 + 5] = x + 1  + (y + 1) * NumGrid ;

			}
		}

		pMesh->UnlockVertexBuffer();
		pMesh->UnlockIndexBuffer();

		m_pMesh = pResMgr->CreateMesh(pDevice, pMesh, pVertexDecl, pszMeshName);
		ASSERT(m_pMesh);
	}
	if(m_pMesh)	hr = S_OK;
	return hr;
}
