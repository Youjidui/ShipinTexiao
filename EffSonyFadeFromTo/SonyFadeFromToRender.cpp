#include "StdAfx.h"
#include "SonyFadeFromToRender.h"
#include "../Utility/ColorConvertor.h"

#pragma warning(disable:4244)

CSonyFadeFromToRender::CSonyFadeFromToRender(void)
: m_pEngine(NULL)
, m_pQuadMesh(NULL)
, m_pEffect(NULL)
{
}

CSonyFadeFromToRender::~CSonyFadeFromToRender(void)
{
	Uninit();
}

bool CSonyFadeFromToRender::Init( CRenderEngine* pEngine )
{
	HRESULT hr = E_FAIL;
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = pEngine->GetDevice();
	CResourceManager* pResMan = pEngine->GetResourceManager();

	m_pQuadMesh = pResMan->CreateQuadMesh(pDevice);
	ASSERT(m_pQuadMesh);
	m_pEffect = pResMan->CreateEffect(pDevice, _T("NewEffects/SonyFadeFromTo.fx"));
	ASSERT(m_pEffect);

	//return SUCCEEDED(hr);
	return true;
}

void CSonyFadeFromToRender::Uninit()
{
}

bool CSonyFadeFromToRender::Render(CVideoBuffer* pDest, CVideoBuffer *pSrcA, FxParamBase* pParamRaw)
{
	SonyFadeFromToFxParam* pParam = (SonyFadeFromToFxParam*)pParamRaw;
	RESET_RENDER_TARGET(m_pEngine);
	SET_DEPTH_STENCIL(m_pEngine);
	CResourceManager* pResMan = m_pEngine->GetResourceManager();
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();

	float fTransition = pParam->fTransition;
	if(!pParam->bFromOrTo)
		fTransition = 1.f - pParam->fTransition;

	CVideoBuffer* pSrcDef = pSrcA;
	CVideoBuffer* pDstDef = pDest;
    m_pEngine->SetRenderTarget(pDstDef);
 
 	//D3DXMATRIX matCombined = m_matWorld*m_matView*m_matProj;     
	D3DXMATRIX *matView,*matPorj;
	pResMan->GetPerspectiveMatrix(&matView,&matPorj);
	D3DXMATRIX matCombined = (*matView)*(*matPorj);

 	D3DXMATRIX matTextureSrc;
	bool pDstDef_bIsCGBuffer = false;
	if( !pDstDef_bIsCGBuffer )
		GenerateMatrix(pSrcDef,&matTextureSrc,mat_Identity);
	else
		GenerateMatrix(pSrcDef,&matTextureSrc,mat_Image);
 
 	m_pEffect->SetMatrix("g_matWorldViewProj",&matCombined);
 	m_pEffect->SetMatrix("g_matTexture",&matTextureSrc);	
    
	m_pEffect->SetFloat("g_FadeScale",fTransition);
	
	D3DXCOLOR cFadeColor = pParam->cFadeColor;
	const Buffer_Color_Format pSrcDef_bufferFormat = FMT_RGBA32;
	ColorConvertor::RGBA2(pSrcDef_bufferFormat,&cFadeColor,&cFadeColor);

	D3DXVECTOR4 vecColor = D3DXVECTOR4(cFadeColor.r,cFadeColor.g,cFadeColor.b,cFadeColor.a);
	m_pEffect->SetVector("g_FadeColor",&vecColor);
 	// PS constant
 
 	// TEXTURE & stage state & sample state
 	m_pEffect->SetTexture("g_txColor",pSrcDef->GetTexture());

	D3DXCOLOR colorBlack(0.f,0.f,0.f,0.f);
	if(FMT_YUVA32 == pSrcDef_bufferFormat)
		ColorConvertor::RGBA2YUVA(&colorBlack,&colorBlack);
	m_pEffect->SetVector("g_colorBlack",(D3DXVECTOR4*)&colorBlack);
 
    // render state
	bool pDstDef_bNeedBlend = true;
    //setCurAlphaRenderState(pDstDef_bNeedBlend);
	if ( pDstDef_bNeedBlend)
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

	float pSrcDef_fAlphaValue = 1.0f;
    m_pEffect->SetFloat("g_fAlpha",pSrcDef_fAlphaValue );
    m_pEffect->SetTechnique("Render");

	UINT cPass,uPass;
    switch(pSrcDef_bufferFormat)
    {
    case FMT_RGBA32:
    case FMT_YUVA32:
        uPass = 0;
        break;
    case FMT_YUYV:
        uPass = 1;
        //if(pDstDef->pAlpha)
        //{
        //    if(pSrcDef->pAlpha)   
        //        uPass = 2;
        //    else
        //        uPass = 3;
        //}
        break;
    }
 
 	// begin&end scene
 	if ( SUCCEEDED(pDevice->BeginScene()))
    {
        
        m_pEffect->Begin(&cPass,0);

        m_pEffect->BeginPass(uPass);
        m_pQuadMesh-> DrawMeshFx();
        m_pEffect->EndPass();

        m_pEffect->End();

        pDevice->EndScene();
 	}
 
 	// »Ö¸´Éè±¸×´Ì¬
 	pDevice->SetVertexShader(NULL);
 	pDevice->SetPixelShader(NULL);
 	pDevice->SetRenderTarget(1,NULL); 	
 
	return true;
}
