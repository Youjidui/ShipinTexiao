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

struct Vertex{
	Vertex(){memset (this, 0, sizeof(Vertex));}
	Vertex(float x,float y, float z,
		float u, float v)
	{ _x = x; _y = y; _z = z;
	_u = u; _v = v;    }
	float _x,_y,_z;
	float _u,_v;
};

void TransPackMatrix(const TransformParam &pTransParam, D3DXMATRIX* matWorldOut)
{
	D3DXMATRIX matWorldTrans,matWorldRot;
	float fTransX = pTransParam.fWorldTranslateX;
	float fTransY = pTransParam.fWorldTranslateY;
	float fTransZ = pTransParam.fWorldTranslateZ;
	D3DXMatrixTranslation(&matWorldTrans,fTransX,fTransY,fTransZ);

	float fRotX = pTransParam.fWorldRotateX*2.f*D3DX_PI;
	float fRotY = pTransParam.fWorldRotateY*2.f*D3DX_PI;
	float fRotZ = pTransParam.fWorldRotateZ*2.f*D3DX_PI;

	D3DXMatrixRotationYawPitchRoll(&matWorldRot,fRotY,fRotX,fRotZ);

	D3DXMATRIX matLocalTrans,matLocalRot,matLocalScale;

	fTransX = pTransParam.fLocalTranslateX;
	fTransY = pTransParam.fLocalTranslateY;
	fTransZ = pTransParam.fLocalTranslateZ;

	D3DXMatrixTranslation(&matLocalTrans,fTransX,fTransY,fTransZ);

	fRotX = pTransParam.fLocalRotateX*2.f*D3DX_PI;
	fRotY = pTransParam.fLocalRotateY*2.f*D3DX_PI;
	fRotZ = pTransParam.fLocalRotateZ*2.f*D3DX_PI;

	D3DXMatrixRotationYawPitchRoll(&matLocalRot,fRotY,fRotX,fRotZ);

	float fScaleX = pTransParam.fScaleX;
	float fScaleY = pTransParam.fScaleY;
	float fScaleZ = pTransParam.fScaleZ;

	D3DXMatrixScaling(&matLocalScale, fScaleX,fScaleY,fScaleZ);

	*matWorldOut = matLocalScale * matLocalRot * matLocalTrans * matWorldRot * matWorldTrans ; 
}

Sony3DBrokenGlassRender::Sony3DBrokenGlassRender(void)
{
	m_pMesh = NULL;
	//m_uResID_fx = INVALID_RESID;

	//m_uResID_Mesh = INVALID_RESID;
	//m_resRandom1024x3 = INVALID_RESID;
	//m_resRandomARGB = INVALID_RESID;
	m_pRandom1024x3Texture = NULL;
	m_pRandomARGBTexture = NULL;

	D3DXMatrixIdentity(&m_matWorld);
	D3DXVECTOR3 vEyePt( 0.0f, 0.0f,-0.5f/tanf(D3DX_PI/8) );
	D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );	
	D3DXMatrixLookAtLH( &m_matView, &vEyePt, &vLookatPt, &vUpVec );
	D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4, 1.0f, 0.1f, 1000.0f );

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
		/*
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
		*/
		Vertex _ver[6];
		_ver[0] = Vertex(-0.5f, 0.5f, 0.f, .0f, .0f);
		_ver[1] = Vertex(-0.5f, -0.5f, 0.f, .0f, .0f);
		_ver[2] = Vertex(0.5f, 0.5f, 0.f, .0f, .0f);
		_ver[3] = Vertex(0.5f, 0.5f, 0.f, .0f, .0f);
		_ver[4] = Vertex(-0.5f,-0.5f, 0.f, .0f, .0f);
		_ver[5] = Vertex(0.5f, -0.5f, 0.f, .0f, .0f);

		const size_t instanceQuadNum = Sony3DBrokenGlassEffectParam::maxSizeX * Sony3DBrokenGlassEffectParam::maxSizeY;
		InstanceData instanceQuadData[instanceQuadNum];
		for( int i=0; i<instanceQuadNum; ++i )
			instanceQuadData[i].quadIndex = i;

		//const WORD indexData[6] = { 0, 1, 2, 3, 4, 5 };
		//const WORD *indexDataPointer = indexData;

		WORD pIndex[6] = {0, 1, 2, 3, 4, 5};
		const WORD * ppIB[1];
		ppIB[0] = pIndex;

		ID3DXMesh	*pMesh = NULL;
		m_pMesh = pResMgr->CreateMesh(pDevice, 
			_ver,
			6*sizeof(Vertex),
			sizeof(Vertex),
			D3DPT_TRIANGLELIST,
			2,
			0,
			vertexElement,
			ppIB,
			6*sizeof(WORD),
			1,
			pszMeshName);

/*
		m_uResID_Mesh = pResMan->CreateMesh(
			vertexQuadData,sizeof(vertexQuadData),sizeof(VertexData),
			instanceQuadData,sizeof(instanceQuadData),sizeof(InstanceData),
			D3DPT_TRIANGLELIST,2,0x00,vertexElement,
			&indexDataPointer,sizeof(indexData),1,
			&uuid_mesh);
*/
	}

	if( m_pRandom1024x3Texture == NULL )
	{
		//m_resRandom1024x3 = pResMan->CreateTexture(1024,3,D3DUSAGE_DYNAMIC,D3DFMT_R32F,D3DPOOL_DEFAULT,&uuid_random);
		//CBaseTexture* pTex = (CBaseTexture*)(m_pResMan->GetResource(m_resRandom1024x3));
		
		CVideoBufferManager* pBufMgr = m_pEngine->GetVideoBufferManager();
		VideoBufferInfo mediaBI = {D3DFMT_A8R8G8B8, VideoBufferInfo::VIDEO_MEM, VideoBufferInfo::_IN, 1024, 3, 0, 0};
		m_pRandom1024x3Texture = pBufMgr->CreateVideoBuffer(mediaBI);
		
		LPDIRECT3DSURFACE9	pSuf = m_pRandom1024x3Texture->GetSurface();
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
	if( m_pRandomARGBTexture == NULL )
	{
		//m_resRandomARGB = pResMan->CreateTexture(1024,1,D3DUSAGE_DYNAMIC,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&uuid_randomARGB);
		
		CVideoBufferManager* pBufMgr = m_pEngine->GetVideoBufferManager();
		VideoBufferInfo mediaBI = {D3DFMT_A8R8G8B8, VideoBufferInfo::VIDEO_MEM, VideoBufferInfo::_IN, 1024, 1, 0, 0};
		m_pRandomARGBTexture = pBufMgr->CreateVideoBuffer(mediaBI);
		//CBaseTexture* pTex = m_pRandomARGBTexture->GetTexture();
		LPDIRECT3DSURFACE9	pSuf = m_pRandomARGBTexture->GetSurface();
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


void Sony3DBrokenGlassRender::Uninit()
{

}

bool Sony3DBrokenGlassRender::Render(CVideoBuffer* pDst, CVideoBuffer* pSrc1, CVideoBuffer* pSrc2, Sony3DBrokenGlassEffectParam* pParam)
{

	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();

	//TP_VBufferDef * pSrcDef = ppSrcDef[0];

	//CGPURender::Base_Render_Mini(pSrcDef, pDstDef);
	
	//TPGSony3DBrokenGlassEffectParam* pParam = (TPGSony3DBrokenGlassEffectParam*)pParamRaw;

	// 如果不需要特技处理

	// 	testTime.Begin();

	// 参数调整
	int numXY[2] = { min(pParam->maxSizeX,pParam->divideX), min(pParam->divideY,pParam->divideY) };


#ifdef _TRANS
	if(pParam->bReverse)
		std::swap(ppSrcDef[0],ppSrcDef[1]);
	pSrcDef = ppSrcDef[0];
	TP_VBufferDef *pDst0 = NULL, *pDstReal = pDstDef;
	handle_tpr hDst0 = NewRTBuffer(pDstDef->OffsetX,pDstDef->OffsetY,pDstDef->GetImageWidth(),pDstDef->GetImageHeight());
	pDst0 = m_pResMan->GetBufferDef(hDst0);
	pDstDef = pDst0;

	if ( pParam->fallingDirection <= 1 )
	{
		float fLinesInterval = 1.f / pParam->divideY;
		float fTimeBegin = -fLinesInterval * 2.f;	// refer to dropDisturb(in fx)
		float fTimeEnd = 1.f + fLinesInterval * 2.f + ( ( pParam->fallingDirection == 0 ) ? 1.f/2.f : sqrt(fLinesInterval/2.f) );

		pParam->time = fTimeBegin + pParam->time * (fTimeEnd-fTimeBegin);
	}
	else
	{
		float fDist = sqrt((float)(pParam->divideY*pParam->divideY+pParam->divideX*pParam->divideX))/2.f;
		float fLinesInterval = 1.f / fDist;
		float fTimeBegin = -fLinesInterval * 1.f;	// refer to dropDisturb(in fx)
		if ( pParam->fallingDirection == 3 )
			fTimeBegin -= fLinesInterval * 1.f;
		float fTimeEnd = 1.f + fLinesInterval * 1.f;
		if ( pParam->fallingDirection == 3 )
			fTimeEnd += sqrt(sqrt(2.f)/8.f);
		else 
			fTimeEnd += sqrt(fLinesInterval/1.f) * 0.3f;



		pParam->time = fTimeBegin + pParam->time * (fTimeEnd-fTimeBegin);
	}

#endif	

	// loading和计算工作


	// 对CG和VIDEO分情况考虑
	bool bYUYV	= false; //TODO: (pSrcDef->IsYUV16Buffer());
	bool bAlpha = false; //TODO: (/*(!pSrcDef->bDiscardAlpha)&&*/(pSrcDef->pAlpha!=NULL));
	bool bYUYVA = bYUYV&&bAlpha;

	//bool bYUVA = (pSrcDef->bufferFormat == FMT_YUVA32);
	bool bYUVA = (pSrc1->GetVideoBufferInfo().format == FMT_YUVA32);

	// dest分配ALPHA通道 //TODO:
	/*
	if(bYUYV&&(pDstDef->pAlpha==NULL))
	{
		pDstDef->pAlpha = m_pEngine->AllocateVideoAlphaRT();
		assert(pDstDef->pAlpha!=NULL);
	}
	*/
	//////////////////////////////////////////////////////////////////////////
	//渲染pass
	//////////////////////////////////////////////////////////////////////////

	// 分配临时BUffer

	// 渲染目标标志位设定
	//pDstDef->bClearFullTarget = true;		// 基类处理，尤其是逐字情况
	//TODO:
	//pDstDef->bDiscardAlpha = false;
	//pDstDef->bRenderToFullTarget = false;

	// setRT
	//m_pEngine->SetRenderTarget(0,pDstDef->handle,pDstDef->COLOR_BLACK(),0x00);
	m_pEngine->SetRenderTarget(pDst);
	
	//float fAspect = m_pResMan->GetAspect() * m_pEngine->GetCurProfile()->nEditWidth / (float) (m_pEngine->GetCurProfile()->nEditHeight  * m_pResMan->GetAspectVerifyCoef());    
	int nEditWidth, nEditHeight;
	m_pEngine->GetTargetVideoSize(nEditWidth, nEditHeight);
	float fAspect = nEditWidth * 1.0f / nEditHeight;
	
	VideoBufferInfo lSrcBufInfo = pSrc1->GetVideoBufferInfo();
	VideoBufferInfo lDesBufInfo = pDst->GetVideoBufferInfo();

	D3DXMATRIX matPosQuad;
	D3DXMatrixScaling(&matPosQuad,
		(float)lSrcBufInfo.nWidth/lDesBufInfo.nWidth,
		(float)lSrcBufInfo.nHeight/lSrcBufInfo.nHeight,1.f);
	D3DXMATRIX matPosQuadTemp;
	D3DXMatrixTranslation(&matPosQuadTemp,
		-0.5f+(/*pSrcDef->OffsetX*/0 + lSrcBufInfo.nWidth/2.f)/lDesBufInfo.nWidth,
		0.5f-(/*pSrcDef->OffsetY*/0 + lSrcBufInfo.nHeight/2.f)/lDesBufInfo.nHeight,
		0.f);
	matPosQuad = matPosQuad * matPosQuadTemp;



	// VS constant

	// 如果是改变了Dst大小的特技，需要重新定位
	//TODO:
	//D3DXMATRIX	*matWorld,*matView,*matPorj;
	//m_pEngine->GetQuadMatrix(&matWorld,&matView,&matPorj);

	D3DXMATRIX matViewChanged,matProjChanged;
	float fovY = D3DXToRadian(max( pParam->perspPackParam.fovDegree, 1.f ));
	float eyePointZ = -0.5f / tanf(fovY/2.f);
	D3DXMatrixLookAtLH(&matViewChanged,&D3DXVECTOR3(0.f,0.f,eyePointZ),&D3DXVECTOR3(0.f,0.f,0.f),&D3DXVECTOR3(0.f,1.f,0.f));
	D3DXMatrixPerspectiveFovLH(&matProjChanged,fovY,fAspect,0.1f,100.f);

	D3DXMATRIX matForField;
	D3DXMatrixIdentity(&matForField);
	matForField._42 = 0.f; //!pParam->bOdd ? 0.5f / lSrcBufInfo.nHeight : 0.f;  
	m_pSony3DBrokenGlassEffect->SetFloat("g_offsetField", 0.f/*!pParam->bOdd ? -0.5f/pSrcDef->BaseHeight : 0.f*/);

	D3DXMATRIX matTransform;
	TransPackMatrix(pParam->transformPackParam, &matTransform);

	D3DXMATRIX matAspect;
	D3DXMatrixIdentity(&matAspect);
	matAspect._11 *= fAspect;
	D3DXMATRIX matWorldTransed = m_matWorld * matForField * matPosQuad * matAspect * matTransform;
	D3DXMATRIX matCombined = matWorldTransed*matViewChanged*matProjChanged;
	D3DXMATRIX matWorldForNormal, matWorldInv;
	D3DXMatrixInverse(&matWorldInv,NULL,&matWorldTransed);
	D3DXMatrixTranspose(&matWorldForNormal,&matWorldInv);

	// 直接从[-0.5,0.5]得到贴图坐标
	D3DXMATRIX matQUADtoTex;
	D3DXMatrixIdentity(&matQUADtoTex);
	matQUADtoTex._22 = -1.f;
	matQUADtoTex._31 = 0.5f;
	matQUADtoTex._32 = 0.5f;
	D3DXMATRIX matTextureSrc;
	GenerateMatrix(pSrc1,&matTextureSrc,mat_Image);
	matTextureSrc = matQUADtoTex*matTextureSrc;
	m_pSony3DBrokenGlassEffect->SetMatrix("g_matWorldViewProj",&matCombined);
	m_pSony3DBrokenGlassEffect->SetMatrix("g_matWorldNormal",&matWorldForNormal);
	m_pSony3DBrokenGlassEffect->SetMatrix("g_matTexture",&matTextureSrc);	
	m_pSony3DBrokenGlassEffect->SetFloat("g_time",(pParam->time));
	m_pSony3DBrokenGlassEffect->GetFxPtr()->SetIntArray( "g_numXY", numXY, 2 );

	if( pParam->fallingDirection == 1 || pParam->fallingDirection == 3 )
		m_pSony3DBrokenGlassEffect->SetBool("g_bReverseDir",BOOL(TRUE));
	else
		m_pSony3DBrokenGlassEffect->SetBool("g_bReverseDir",BOOL(FALSE));

	D3DXVECTOR4 vCenter(-pParam->centerX,-pParam->centerY,0.f,0.f);
	m_pSony3DBrokenGlassEffect->SetVector("g_centerXY",&vCenter);


	D3DXVECTOR3 lightPos(0,0,-1);
	D3DXVECTOR4 lightParam(1.f,0.f,0.f,0.f);
	if( pParam->lightingPackParam.bEnabled )
	{
		float radianX = D3DXToRadian(pParam->lightingPackParam.lightXDegree);
		float radianY = D3DXToRadian(pParam->lightingPackParam.lightYDegree);
		D3DXMATRIX matRotX,matRotY;	// x first, then y;
		D3DXMatrixRotationY(&matRotX,-radianX);
		D3DXVECTOR3 vecRotY(matRotX._11,matRotX._12,matRotX._13);
		D3DXMatrixRotationAxis(&matRotY,&vecRotY,radianY);
		D3DXVec3TransformNormal(&lightPos,&lightPos,&matRotX);
		D3DXVec3TransformNormal(&lightPos,&lightPos,&matRotY);
		m_pSony3DBrokenGlassEffect->SetVector("g_lightDir",&(D3DXVECTOR4(-lightPos,1.f)));
		lightParam.x = pParam->lightingPackParam.ambient1;
		lightParam.y = pParam->lightingPackParam.diffuse2;
		m_pSony3DBrokenGlassEffect->SetVector("g_lightAmbDiff",&lightParam);
	}
	else
	{
		m_pSony3DBrokenGlassEffect->SetVector("g_lightDir",&(D3DXVECTOR4(-lightPos,1.f)));
		m_pSony3DBrokenGlassEffect->SetVector("g_lightAmbDiff",&lightParam);
	}

	// PS constant

	// TEXTURE & stage state & sample state
	m_pSony3DBrokenGlassEffect->SetTexture("g_txColor",pSrc1->GetTexture());
	if( bYUYVA )
	{
		m_pSony3DBrokenGlassEffect->SetTexture("g_txAlpha",pSrc1->GetTexture());
	}
	m_pSony3DBrokenGlassEffect->SetTexture("g_txRandom",m_pRandom1024x3Texture->GetTexture());
	m_pSony3DBrokenGlassEffect->SetTexture("g_txRandomARGB",m_pRandomARGBTexture->GetTexture());

	// render state
	//setCurAlphaRenderState(pDstDef->bNeedBlend);
	//if ( bNeedBlend )
	if(true)
	{
		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		pDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
		pDevice->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ZERO);
		pDevice->SetRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD);
	}
	else
	{
		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	}



	pDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);
	pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	//pDevice->ApplyCurRenderState(); //TODO

	// shader
	if( pParam->fallingDirection == 0 || pParam->fallingDirection == 1 )
		m_pSony3DBrokenGlassEffect->SetTechnique("RenderYUVA");
	else if( pParam->fallingDirection == 2 || pParam->fallingDirection == 3 )
		m_pSony3DBrokenGlassEffect->SetTechnique("RenderYUVA_EXPLODE");


	//BeginAntiAlias();
	// begin&end scene
	if ( SUCCEEDED(pDevice->BeginScene()))
	{
		UINT cPass,uPass;
		m_pSony3DBrokenGlassEffect->Begin(&cPass,0);
		for(uPass=0;uPass<cPass;uPass++)
		{
			m_pSony3DBrokenGlassEffect->BeginPass(uPass);
			
			//((CBaseMesh*)(m_pResMan->GetResource(m_uResID_Mesh)))->DrawInstance(sizeof(InstanceData),numXY[0]*numXY[1]);
			//m_pQuadMesh->DrawMeshFx();
			m_pSony3DBrokenGlassEffect->EndPass();
		}
		m_pSony3DBrokenGlassEffect->End();

		pDevice->EndScene();
	}
	//EndAntiAlias(pDstDef);
	// 恢复设备状态
	pDevice->SetRenderTarget(1,NULL);

	// 释放临时buffer

	//////////////////////////////////////////////////////////////////////////
	//渲染pass结束
	//////////////////////////////////////////////////////////////////////////

#ifdef _TRANS
	pDst0->fAlphaValue = ppSrcDef[0]->fAlphaValue;
	BlendTwoBuffer(pDst0,ppSrcDef[1],pDstReal);
	FreeRTBuffer(pDst0->handle);
	pDstDef = pDstReal;
#else
	//TODO:
	//pDstDef->bIsCG_BlenedBuffer = pSrcDef->bIsCG_BlenedBuffer;
#endif

	// dest标志位设定
	//TODO:
	//pDstDef->bContainedAlpha = true;


	// 	testTime.Count();

	//	m_pResMan->DumpResourceToFile(pSrcDef->handle,L"C:/src.dds");
	//	m_pResMan->DumpResourceToFile(pDstDef->handle,L"C:/dst.dds");



	return true;
}