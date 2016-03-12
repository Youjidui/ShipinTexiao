#include "StdAfx.h"
#include "CubeTransRender.h"
#include "../FxParam.h"
#include "../Utility/SafeDelete.h"
#include "../Utility/mathmacros.h"

#pragma warning(disable: 4244)

CCubeTransRender::CCubeTransRender(void)
: m_pEngine(NULL)
, m_pQuadMesh(NULL)
, m_pMesh(NULL)
, m_pInstanceMesh(NULL)
, m_pEffect(NULL)
, m_pMipMapGenerator(NULL)
{
}

CCubeTransRender::~CCubeTransRender(void)
{
	Uninit();
}

bool CCubeTransRender::Init( CRenderEngine* pEngine )
{
	HRESULT hr = E_FAIL;
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = pEngine->GetDevice();
	CResourceManager* pResMan = pEngine->GetResourceManager();

	m_pQuadMesh = pResMan->CreateQuadMesh(pDevice);
	ASSERT(m_pQuadMesh);
	m_pEffect = pResMan->CreateEffect(pDevice, _T("NewEffects/G3DCube.fx"));
	ASSERT(m_pEffect);

	hr = CreateMesh();
	ASSERT(SUCCEEDED(hr));

	m_pMipMapGenerator = new CMipMapGenerator;
	bool bOK = m_pMipMapGenerator->Init(pEngine);
	return bOK;
}

void CCubeTransRender::Uninit()
{
	delete m_pMipMapGenerator;
	m_pMipMapGenerator = NULL;
}

bool CCubeTransRender::Render( CVideoBuffer* pDest, CVideoBuffer *pSrcA, CVideoBuffer *pSrcB, FxParamBase* pParamBase )
{
	HRESULT hr = E_FAIL;

	RESET_RENDER_TARGET(m_pEngine);
	SET_DEPTH_STENCIL(m_pEngine);

	CubeFxParam* pParam = (CubeFxParam*)pParamBase;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	CVideoBufferManager* pVM = m_pEngine->GetVideoBufferManager();

#ifdef _3D_CUBE_TRANS
	if(pParam->bReverse)
		std::swap(pSrcA, pSrcB);
#endif	
	CVideoBuffer* pSrcDef = pSrcA;	
	const VideoBufferInfo& srcBufInfo = pSrcDef->GetVideoBufferInfo();

	//CBaseTexture * pMipMap = m_pResMan->GetTemp_Video(0,TRUE);
	VideoBufferInfo mipMapInfo = pSrcDef->GetVideoBufferInfo();
	mipMapInfo.eUsage = VideoBufferInfo::_IN_OUT_WITH_MIPMAP;
	CVideoBuffer* pMipMap = pVM->CreateVideoBuffer(mipMapInfo);
	ASSERT(pMipMap);

#ifdef _3D_CUBE_TRANS
	const TransformParam trans;
	//GenerateMipMap(pSrcDef,pMipMap,TRUE);
#else
	const TransformParam& trans = pParam->trans;
	//GenerateMipMap(pSrcDef,pMipMap);
#endif
	if(pMipMap)
	{
		bool bOK = m_pMipMapGenerator->Render(pMipMap, pSrcA, pParamBase);
		ASSERT(bOK);
	}
	//D3DXSaveTextureToFile(_T("./Cube_Mipmap.bmp"), D3DXIFF_BMP, pMipMap->GetTexture(), NULL);
	
#ifdef _3D_CUBE_TRANS
	//CBaseTexture * pMipMap1 = m_pResMan->GetTemp_Video(0,TRUE);
	VideoBufferInfo mipMapInfo1 = pSrcB->GetVideoBufferInfo();
	mipMapInfo1.eUsage = VideoBufferInfo::_IN_OUT_WITH_MIPMAP;
	CVideoBuffer* pMipMap1 = pVM->CreateVideoBuffer(mipMapInfo1);
	ASSERT(pMipMap1);
	//if(ppSrcDef[1]->IsYUV16Buffer())
	//	m_pEngine->ConvertYUYV_YUVA(ppSrcDef[1],pMipMap);
	//else
	//{
	//	GenerateMipMap(ppSrcDef[1],pMipMap1,TRUE);		
	//}
	if(pMipMap1)
	{
		bool bOK = m_pMipMapGenerator->Render(pMipMap1, pSrcB, pParamBase);
		ASSERT(bOK);
	}
	//D3DXSaveTextureToFile(_T("./Cube_Mipmap_1.bmp"), D3DXIFF_BMP, pMipMap1->GetTexture(), NULL);
#endif
	//const Custom_Profile *pProfile = m_pEngine->GetCurProfile();
    //m_fAspect = m_pResMan->GetAspect() * m_pEngine->GetCurProfile()->nEditWidth / (float) (m_pEngine->GetCurProfile()->nEditHeight  * m_pResMan->GetAspectVerifyCoef());
	int nEditWidth, nEditHeight;
	m_pEngine->GetTargetVideoSize(nEditWidth, nEditHeight);
	float fAspect = nEditWidth * 1.0f / nEditHeight;
	
	//D3DXCOLOR crBack =pSrcDef->COLOR_BLACK();
	D3DXCOLOR crBack = D3DCOLOR_ARGB(0XFF,0,0,0);
#ifdef _3D_CUBE_TRANS
	float fTransition = 1.0f - fabs(pParam->fTransition * 2.0f - 1.0f);
	float fIntervalX = LERP(1.0f,pParam->shape.fIntervalX,fTransition);
	float fIntervalY = LERP(1.0f,pParam->shape.fIntervalY,fTransition);	
	
	crBack = pParam->crBackgroundColor;
	//RGBA2(pDstDef->bufferFormat,&crBack,&crBack);
	crBack.a = pParam->fBackgroundTransparency;

	if(pParam->shape.nDirecttion > 1)
		fAspect = 1.0f;
#endif	

	D3DXMATRIXA16 matTex,matRot,matWorld,matCombine,
		matPrevScale,matPrevTrans,
		matScale,
		matLocalRot,matGlobalRot,
		matLocalTrans,matGlobalTrans,
		matView,matProj,matAspect;

	D3DXMatrixIdentity(&matTex);
#ifdef _3D_CUBE_TRANS
	matTex._11 = nEditWidth  / (float)mipMapInfo.nAllocWidth;
	matTex._22 = nEditHeight / (float)mipMapInfo.nAllocHeight;
#else
	matTex._11 = srcBufInfo.nWidth / (float)mipMapInfo.nAllocWidth;
	matTex._22 = srcBufInfo.nHeight / (float)mipMapInfo.nAllocHeight;
#endif
	
	matTex._31 = 0.5f / (float)mipMapInfo.nAllocWidth;
    //matTex._32 = pParam->bOdd ? 0.5f / (float)pMipMap->GetImagesInfo()->Base_Height : 0.0f;

	D3DVIEWPORT9 vPort;
	hr = pDevice->GetViewport(&vPort);
	ASSERT(SUCCEEDED(hr));
	vPort.Width = nEditWidth;
	vPort.Height = nEditHeight;

	bool pSrcDef_IsYUV16Buffer = false;
	float pSrcDef_OffsetX = 0.0f;
	float pSrcDef_OffsetY = 0.0f;
	D3DXMatrixScaling(&matPrevScale,srcBufInfo.nWidth * (pSrcDef_IsYUV16Buffer ? 2.0f : 1.0f)  / (float)vPort.Width, srcBufInfo.nHeight / (float)vPort.Height, 1.0f);
	D3DXMatrixTranslation(&matPrevTrans, -0.5f + matPrevScale._11 * 0.5f + pSrcDef_OffsetX / (float)vPort.Width, 0.5f - matPrevScale._22 * 0.5f - pSrcDef_OffsetY / (float)vPort.Height,0.0f);
	matPrevTrans._41 *= fAspect;
	D3DXMatrixScaling(&matScale,trans.fScaleX,trans.fScaleY,trans.fScaleZ);
	
	D3DXMATRIXA16 matRotX,matRotY,matRotZ;
	D3DXMatrixRotationX(&matRotX,trans.fLocalRotateX);
	D3DXMatrixRotationY(&matRotY,trans.fLocalRotateY);
	D3DXMatrixRotationZ(&matRotZ,trans.fLocalRotateZ);
	matLocalRot = matRotX * matRotY * matRotZ;

	D3DXMatrixRotationX(&matRotX,trans.fWorldRotateX);
	D3DXMatrixRotationY(&matRotY,trans.fWorldRotateY);
	D3DXMatrixRotationZ(&matRotZ,trans.fWorldRotateZ);
	matGlobalRot = matRotX * matRotY * matRotZ;
	//D3DXMatrixRotationYawPitchRoll(&matLocalRot,trans.fLocalRotateY,trans.fLocalRotateX,trans.fLocalRotateZ);
	//D3DXMatrixRotationYawPitchRoll(&matGlobalRot,trans.fWorldRotateX,trans.fWorldRotateY,trans.fWorldRotateZ);

	D3DXMatrixTranslation(&matLocalTrans,trans.fLocalTranslateX,trans.fLocalTranslateY,trans.fLocalTranslateZ);
	D3DXMatrixTranslation(&matGlobalTrans,trans.fWorldTranslateX,trans.fWorldTranslateY,trans.fWorldTranslateZ);
	matLocalTrans._41 /= 2.0f;
	matLocalTrans._42 /= 2.0f;
	matLocalTrans._43 *= 0.35f;

	matGlobalTrans._41 /= 2.0f;
	matGlobalTrans._42 /= 2.0f;
	matGlobalTrans._43 *= 0.35f;	

	D3DXVECTOR3 vEyePt( 0.0f, 0.0f,-0.5f/tan(D3DXToRadian(pParam->fPerspective) / 2.0f) );
	D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );

	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );

	D3DXMatrixPerspectiveFovLH( &matProj,D3DXToRadian(pParam->fPerspective), fAspect, 0.001f, 1000.0f );
      
	float pSrcDef_fAlphaValue = 1.0f;
	D3DXVECTOR4	vAlpha(pSrcDef_fAlphaValue,0.0f,0.0f,0.0f);

#ifdef _3D_CUBE_TRANS
	int nDir = pParam->bReverse ? 1.0f : 1.0f;
	switch(pParam->shape.nDirecttion) {
	case 0:D3DXMatrixRotationX(&matLocalRot,nDir * -D3DX_PI / 2.0f * pParam->fTransition);break;
	case 1:D3DXMatrixRotationX(&matLocalRot,nDir * D3DX_PI / 2.0f * pParam->fTransition);break;
	case 2:D3DXMatrixRotationY(&matLocalRot,nDir * -D3DX_PI / 2.0f * pParam->fTransition);break;
	case 3:D3DXMatrixRotationY(&matLocalRot,nDir * D3DX_PI / 2.0f * pParam->fTransition);break;
	}
	D3DXMATRIXA16 matRoundRot;
	D3DXMatrixIdentity(&matRoundRot);
	switch(pParam->shape.nRotate )
	{
	case 1:	D3DXMatrixRotationZ(&matRoundRot,nDir * D3DX_PI * 2.0f * pParam->fTransition);break;
	case 2:	D3DXMatrixRotationZ(&matRoundRot,nDir * -D3DX_PI * 2.0f * pParam->fTransition);break;
	}
	matRot =  matLocalRot * matRoundRot;
	D3DXMatrixIdentity(&matWorld);

	vAlpha.y = pSrcDef_fAlphaValue;
#else	
	matRot =  matPrevScale *  matScale * matLocalRot;
	matWorld =  matPrevTrans * matLocalTrans  * matGlobalRot * matGlobalTrans ;
#endif	
	D3DXMatrixIdentity(&matAspect);
	matAspect._11 = fAspect;
	matRot = matAspect * matRot;
	matCombine = matWorld * matView * matProj;	
	
	bool pParam_bOdd = false;
	D3DXVECTOR4 vShape(pParam->shape.nDiveX,pParam->shape.nDiveY,fIntervalX,fIntervalY);
	TRACE("pParam->shape.fIntervalX = %f, pParam->shape.fIntervalY = %f\n", fIntervalX, fIntervalY);
	hr = m_pEffect->SetVector("g_vShape",&vShape);
	ASSERT(SUCCEEDED(hr));
	D3DXVECTOR4 vMisc(fAspect,pParam->light.fDiffuse,pParam->light.fAmbient,pParam_bOdd ? 0.0f : 0.5f * 2.0f  /  vPort.Height);
	hr = m_pEffect->SetVector("g_vMisc",&vMisc);
	ASSERT(SUCCEEDED(hr));
	vAlpha.z = D3DCMP_LESS;
	hr = m_pEffect->SetVector("g_vAlpha",&vAlpha);
	ASSERT(SUCCEEDED(hr));

	pParam->light.fDirectionX = D3DXToRadian(pParam->light.fDirectionX );
	pParam->light.fDirectionY = D3DXToRadian(pParam->light.fDirectionY );
	D3DXVECTOR4 vLightDir(cosf(pParam->light.fDirectionY ) * sinf(pParam->light.fDirectionX),
		sinf(pParam->light.fDirectionY ),- cosf(pParam->light.fDirectionY ) * cosf(pParam->light.fDirectionX),0.0f);
	D3DXVec4Normalize(&vLightDir,&vLightDir);
#ifdef _3D_CUBE_TRANS
	vLightDir.w = pParam->shape.nDirecttion;
#endif
	hr = m_pEffect->SetVector("g_vLightDir",&vLightDir);
	ASSERT(SUCCEEDED(hr));

	D3DXVECTOR4 vSlideColor;
	D3DXCOLOR crSlideColor = pParam->shape.crSlideColor;
	//if(pSrcDef->bufferFormat != FMT_RGBA32)
	//	m_pEngine->ColorConv_RGBA_YUVA(&crSlideColor,&crSlideColor);
	vSlideColor = D3DXVECTOR4(crSlideColor.r,crSlideColor.g,crSlideColor.b,pParam->shape.fSlideTransparency);	
	hr = m_pEffect->SetVector("g_vSlideColor",&vSlideColor);
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetMatrix("g_matWorldViewProj",&matCombine);
	ASSERT(SUCCEEDED(hr));
	//m_pEffect->SetMatrix("g_matWorld",&matWorld);
	hr = m_pEffect->SetMatrix("g_matRot",&matRot);
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetMatrix("g_matTex",&matTex);
	ASSERT(SUCCEEDED(hr));

	hr = m_pEffect->SetTexture("g_txColor",pMipMap->GetTexture());
	ASSERT(SUCCEEDED(hr));
#ifdef _3D_CUBE_TRANS
	hr = m_pEffect->SetTexture("g_txColor1",pMipMap1->GetTexture());
	ASSERT(SUCCEEDED(hr));

	hr = m_pEffect->SetTechnique("Trans");
	ASSERT(SUCCEEDED(hr));
#else
	hr = m_pEffect->SetTechnique("Picture");
	ASSERT(SUCCEEDED(hr));
#endif

	UINT cPass,uPass;
	DWORD pSrcDef_bufferFormat = FMT_RGBA32;
	if(pSrcDef_bufferFormat == FMT_RGBA32)
		uPass = 0;
	else
		uPass = 1;
	if(pParam->light.bEnabled)
		uPass += 2;

	//handle_tpr hTemp[2] = {INVALID_RESID,INVALID_RESID};
	CVideoBuffer* pTempBuffer[2] = {NULL, NULL};
	VideoBufferInfo destBufInfo = pDest->GetVideoBufferInfo();

	LPDIRECT3DVERTEXBUFFER9		pVB = NULL,pVBInstance = NULL;
	LPDIRECT3DINDEXBUFFER9		pIB = NULL;
	m_pMesh->GetMesh()->GetVertexBuffer(&pVB);
	ASSERT(pVB);
	m_pMesh->GetMesh()->GetIndexBuffer(&pIB);
	ASSERT(pIB);
	m_pInstanceMesh->GetMesh()->GetVertexBuffer(&pVBInstance);
	ASSERT(pVBInstance);
	IDirect3DVertexDeclaration9 * pVertexDecl = NULL;
	pVertexDecl =  m_pMesh->GetVertexDeclarationPtr();
	ASSERT(NULL != pVertexDecl);

	for(int i = 0; i < 2;i ++)
	{
		destBufInfo.eUsage = VideoBufferInfo::_IN_OUT;
		pTempBuffer[i] = pVM->CreateVideoBuffer(destBufInfo);	//or CreateRenderTargetBuffer
		ASSERT(pTempBuffer[i]);
		CVideoBuffer* pTempDef = pTempBuffer[i];
			
		if(i == 0)
		{
		}
		else
		{
			vAlpha.z = D3DCMP_GREATER;
			hr = m_pEffect->SetVector("g_vAlpha",&vAlpha);
			ASSERT(SUCCEEDED(hr));
		}
		bool bOK = m_pEngine->SetRenderTarget(pTempDef);
		ASSERT(bOK);
		hr = pDevice->SetViewport(&vPort);
		ASSERT(SUCCEEDED(hr));

		// Begin the scene
		if( SUCCEEDED( pDevice->BeginScene() ) )
		{
			hr = pDevice->SetStreamSource(0,pVB,0,sizeof(float) * 4);
			ASSERT(SUCCEEDED(hr));
			hr = pDevice->SetStreamSourceFreq(0,D3DSTREAMSOURCE_INDEXEDDATA|int(vShape.x * vShape.y));
			ASSERT(SUCCEEDED(hr));
			hr = pDevice->SetStreamSource(1,pVBInstance,0,sizeof(float));
			ASSERT(SUCCEEDED(hr));
			hr = pDevice->SetStreamSourceFreq(1,D3DSTREAMSOURCE_INSTANCEDATA|1ul);
			ASSERT(SUCCEEDED(hr));
			hr = pDevice->SetIndices(pIB);
			ASSERT(SUCCEEDED(hr));
			hr = pDevice->SetVertexDeclaration(pVertexDecl);
			ASSERT(SUCCEEDED(hr));

			hr = m_pEffect->Begin(&cPass,0);
			ASSERT(SUCCEEDED(hr));
			hr = m_pEffect->BeginPass(uPass);
			ASSERT(SUCCEEDED(hr));
			hr = pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,24,0,12);
			ASSERT(SUCCEEDED(hr));
			hr = m_pEffect->EndPass();
			ASSERT(SUCCEEDED(hr));
			hr = m_pEffect->End();
			ASSERT(SUCCEEDED(hr));
			hr = pDevice->SetStreamSourceFreq(0,1);
			ASSERT(SUCCEEDED(hr));
			hr = pDevice->SetStreamSourceFreq(1,1);
			ASSERT(SUCCEEDED(hr));
			// End the scene
			hr = pDevice->EndScene();
			ASSERT(SUCCEEDED(hr));
		}
	}
	SAFE_RELEASE(pVB);
	SAFE_RELEASE(pIB);
	SAFE_RELEASE(pVBInstance);
	pVM->ReleaseVideoBuffer(pMipMap);
#ifdef _3D_CUBE_TRANS
	pVM->ReleaseVideoBuffer(pMipMap1);
#endif

	//hr = D3DXSaveTextureToFile(_T("./Cube_Temp_0.bmp"), D3DXIFF_BMP, pTempBuffer[0]->GetTexture(), NULL);
	//ASSERT(SUCCEEDED(hr));
	//hr = D3DXSaveTextureToFile(_T("./Cube_Temp_1.bmp"), D3DXIFF_BMP, pTempBuffer[1]->GetTexture(), NULL);
	//ASSERT(SUCCEEDED(hr));



	//next
	bool bOK = m_pEngine->SetRenderTarget(pDest);
	ASSERT(bOK);
	{
		D3DXMATRIX *matView = NULL, *matProj= NULL;
		pResMgr->GetPerspectiveMatrix( &matView, &matProj);		//CGPURenderµÄ×ÓÀà
		matCombine = *matView * *matProj;
	}
	bOK = GenerateMatrix(pTempBuffer[0], &matTex, mat_Image );
	ASSERT(bOK);
	hr = m_pEffect->SetTechnique("Compose");
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetMatrix("g_matWorldViewProj",&matCombine);
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetMatrix("g_matTex",&matTex);
	ASSERT(SUCCEEDED(hr));
	//TP_VBufferDef * pTempDef = m_pResMan->GetBufferDef(hTemp[0]);
	hr = m_pEffect->SetTexture("g_txColor",pTempBuffer[0]->GetTexture());
	ASSERT(SUCCEEDED(hr));
	//pTempDef = m_pResMan->GetBufferDef(hTemp[1]);
	hr = m_pEffect->SetTexture("g_txColor1",pTempBuffer[1]->GetTexture());
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetVector("g_vBackColor",(D3DXVECTOR4*)(float*)crBack);
	ASSERT(SUCCEEDED(hr));

	if( SUCCEEDED( pDevice->BeginScene() ) )
	{
		UINT cPass;
		hr = m_pEffect->Begin(&cPass,0);
		ASSERT(SUCCEEDED(hr));
#ifdef _3D_CUBE_TRANS
		hr = m_pEffect->BeginPass(pParam->fBackgroundTransparency > 0.0f ? 1 : 0);
		ASSERT(SUCCEEDED(hr));
#else
		hr = m_pEffect->BeginPass(0);
		ASSERT(SUCCEEDED(hr));
#endif
		bool pDstDef_bNeedBlend = true;
		if ( pDstDef_bNeedBlend )
		{
			hr = pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			ASSERT(SUCCEEDED(hr));
			hr = pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			ASSERT(SUCCEEDED(hr));
			hr = pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			ASSERT(SUCCEEDED(hr));
			hr = pDevice->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, FALSE );
			ASSERT(SUCCEEDED(hr));
			//pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			//pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			hr = m_pEffect->CommitChanges();
			ASSERT(SUCCEEDED(hr));
		}

		bOK = m_pQuadMesh->DrawMeshFx();
		ASSERT(bOK);
		hr = m_pEffect->EndPass();
		ASSERT(SUCCEEDED(hr));
		hr = m_pEffect->End();
		ASSERT(SUCCEEDED(hr));
		hr = pDevice->EndScene();
		ASSERT(SUCCEEDED(hr));
	}
	//pDstDef->OffsetX = 0.0f;
	//pDstDef->OffsetY = 0.0f;
	//SetRect(&pDstDef->rcImage,0,0,pProfile->nEditWidth * (pSrcDef->IsYUV16Buffer()?0.5f:1.0f),pProfile->nEditHeight);
	hr = pDevice->SetRenderTarget(1,NULL);
	ASSERT(SUCCEEDED(hr));
	
	//pDstDef->fAlphaValue = 1.0f;
	for(int i = 0; i < 2;i ++)
	{
		//FreeRTBuffer(hTemp[i]);
		pVM->ReleaseVideoBuffer(pTempBuffer[i]);
	}
	//D3DXSaveTextureToFile(L"c:\\yuva.bmp",D3DXIFF_BMP,pYUVA->GetTexture(),NULL);
// 	D3DXSaveTextureToFile(L"c:\\yuvami.bmp",D3DXIFF_BMP,pMipMap->GetTexture(),NULL);
 	//D3DXSaveTextureToFile(L"c:\\src.bmp",D3DXIFF_BMP,((CBaseTexture*)pDstDef->pContainer)->GetTexture(),NULL);
	//m_pResMan->DumpResourceToFile(pDstDef->handle,L"c:\\3dcube.bmp",FALSE,FALSE,D3DXIFF_BMP);
	return true;
}

HRESULT CCubeTransRender::CreateMesh()
{
	HRESULT hr = E_FAIL;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	LPCTSTR pszMeshName = _T("CubeMesh");
	m_pMesh = pResMgr->FindMesh(pszMeshName);
	if(!m_pMesh)
	{
		D3DVERTEXELEMENT9 VertexElem[] = 
		{		
			{ 0, 0,     D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION,  0 },
			{ 0, 3 * 4, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,  0 },	
			{ 1, 0,     D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,  1 },	
			D3DDECL_END()
		};

		LPDIRECT3DVERTEXDECLARATION9 pVertexDecl = NULL;
		hr =  pDevice->CreateVertexDeclaration( VertexElem, &pVertexDecl );
		ASSERT(SUCCEEDED(hr));

		D3DVERTEXELEMENT9 VertexElem0[] = 
		{		
			{ 0, 0,     D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION,  0 },
			{ 0, 3 * 4, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,  0 },		
			D3DDECL_END()
		};
		struct CUSTOMVERTEX
		{		
			D3DXVECTOR3 position; // The position
			FLOAT       tu;   // The texture coordinates
		};
		LPD3DXMESH pMesh = NULL;
		hr = D3DXCreateMesh(12, 24, D3DXMESH_DYNAMIC | D3DXMESH_WRITEONLY, VertexElem0, pDevice, &pMesh);
		ASSERT(SUCCEEDED(hr));
		CUSTOMVERTEX *pVertices = NULL;
		DWORD* pAtt = NULL;
		WORD * pIB = NULL;

		pMesh->LockVertexBuffer(0,(void**)&pVertices);
		ASSERT(SUCCEEDED(hr));
		pMesh->LockAttributeBuffer(0,&pAtt);	
		ASSERT(SUCCEEDED(hr));
		pMesh->LockIndexBuffer(0,(void**)&pIB);
		ASSERT(SUCCEEDED(hr));

		const float LITTLEVALUE = 0;
		//Front Face 
		pVertices[0].position = D3DXVECTOR3(-0.5f,-0.5f,-0.50f-LITTLEVALUE);
		pVertices[0].tu = 0.0f;
		pVertices[1].position = D3DXVECTOR3(-0.5f,0.5f,-0.50f-LITTLEVALUE);
		pVertices[1].tu = 0.0f;
		pVertices[2].position = D3DXVECTOR3(0.5f,-0.5f,-0.50f-LITTLEVALUE);
		pVertices[2].tu = 0.0f;
		pVertices[3].position = D3DXVECTOR3(0.5f,0.5f,-0.50f-LITTLEVALUE);
		pVertices[3].tu = 0.0f;

		//Back Face
		pVertices[4].position = D3DXVECTOR3(-0.5f,-0.5f,0.5f+LITTLEVALUE);
		pVertices[4].tu = 1.0f;
		pVertices[5].position = D3DXVECTOR3(-0.5f,0.5f,0.5f+LITTLEVALUE);
		pVertices[5].tu = 1.0f;
		pVertices[6].position = D3DXVECTOR3(0.5f,-0.5f,0.5f+LITTLEVALUE);
		pVertices[6].tu = 1.0f;
		pVertices[7].position = D3DXVECTOR3(0.5f,0.5f,0.5f+LITTLEVALUE);
		pVertices[7].tu = 1.0f;

		//Top Face
		pVertices[8].position = D3DXVECTOR3(-0.5f,0.5f+LITTLEVALUE,-0.5f);
		pVertices[8].tu = 2.0f;
		pVertices[9].position = D3DXVECTOR3(-0.5f,0.5f+LITTLEVALUE,0.5f);
		pVertices[9].tu = 2.0f;
		pVertices[10].position = D3DXVECTOR3(0.5f,0.5f+LITTLEVALUE,-0.5f);
		pVertices[10].tu = 2.0f;
		pVertices[11].position = D3DXVECTOR3(0.5f,0.5f+LITTLEVALUE,0.5f);
		pVertices[11].tu = 2.0f;

		//Bottom Face
		pVertices[12].position = D3DXVECTOR3(-0.5f,-0.5f-LITTLEVALUE,-0.5f);
		pVertices[12].tu = 3.0f;
		pVertices[13].position = D3DXVECTOR3(-0.5f,-0.5f-LITTLEVALUE,0.5f);
		pVertices[13].tu = 3.0f;
		pVertices[14].position = D3DXVECTOR3(0.5f,-0.5f-LITTLEVALUE,-0.5f);
		pVertices[14].tu = 3.0f;
		pVertices[15].position = D3DXVECTOR3(0.5f,-0.5f-LITTLEVALUE,0.5f);
		pVertices[15].tu = 3.0f;

		//Left Face
		pVertices[16].position = D3DXVECTOR3(-0.5f-LITTLEVALUE,0.5f,-0.5f);
		pVertices[16].tu = 4.0f;
		pVertices[17].position = D3DXVECTOR3(-0.5f-LITTLEVALUE,0.5f,0.5f);
		pVertices[17].tu = 4.0f;
		pVertices[18].position = D3DXVECTOR3(-0.5f-LITTLEVALUE,-0.5f,-0.5f);
		pVertices[18].tu = 4.0f;
		pVertices[19].position = D3DXVECTOR3(-0.5f-LITTLEVALUE,-0.5f,0.5f);
		pVertices[19].tu = 4.0f;

		//Right Face
		pVertices[20].position = D3DXVECTOR3(0.5f+LITTLEVALUE,0.5f,-0.5f);
		pVertices[20].tu = 5.0f;
		pVertices[21].position = D3DXVECTOR3(0.5f+LITTLEVALUE,0.5f,0.5f);
		pVertices[21].tu = 5.0f;
		pVertices[22].position = D3DXVECTOR3(0.5f+LITTLEVALUE,-0.5f,-0.5f);
		pVertices[22].tu = 5.0f;
		pVertices[23].position = D3DXVECTOR3(0.5f+LITTLEVALUE,-0.5f,0.5f);
		pVertices[23].tu = 5.0f;


		for(int i = 0; i < 6;i ++)
		{
			pIB[i * 6 + 0] = i * 4 + 0;
			pIB[i * 6 + 1] = i * 4 + 1;
			pIB[i * 6 + 2] = i * 4 + 2;

			pIB[i * 6 + 3] = i * 4 + 3;
			pIB[i * 6 + 4] = i * 4 + 2;
			pIB[i * 6 + 5] = i * 4 + 1;

			pAtt[i] = 0;
		}

		pMesh->UnlockVertexBuffer();
		pMesh->UnlockAttributeBuffer();
		pMesh->UnlockIndexBuffer();

		m_pMesh = pResMgr->CreateMesh(pDevice, pMesh, pVertexDecl, pszMeshName);
		ASSERT(m_pMesh);
	}

	LPCTSTR pszInstanceMeshName = _T("CubeInstanceMesh");
	m_pInstanceMesh = pResMgr->FindMesh(pszInstanceMeshName);
	if(!m_pInstanceMesh)
	{
		D3DVERTEXELEMENT9 InstanceVertexElem[] = 
		{		
			{ 0, 0,     D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION,  0 },		
			D3DDECL_END()
		};
		LPD3DXMESH pMesh = NULL;
		hr = D3DXCreateMesh(400,400,D3DXMESH_DYNAMIC | D3DXMESH_WRITEONLY, InstanceVertexElem, pDevice, &pMesh);
		ASSERT(SUCCEEDED(hr));

		float *pVB1 = NULL;
		hr = pMesh->LockVertexBuffer(0,(void**)&pVB1);
		ASSERT(SUCCEEDED(hr));

		for(int i = 0;i<400;i ++)
			pVB1[i] = i;

		pMesh->UnlockVertexBuffer();

		m_pInstanceMesh = pResMgr->CreateMesh(pDevice, pMesh, NULL, pszInstanceMeshName);
		ASSERT(m_pInstanceMesh);
	}
	if(m_pMesh && m_pInstanceMesh)	hr = S_OK;
	return hr;
}
