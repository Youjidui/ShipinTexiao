#include "StdAfx.h"
#include "DipToColorRender.h"
#include "../Utility/ColorConvertor.h"

#pragma warning(disable:4244)


void SetAlphaRenderState(LPDIRECT3DDEVICE9 pDevice, bool bNeedBlend)
{
	if ( bNeedBlend )
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
}


CDipToColorRender::CDipToColorRender(void)
: m_pEngine(NULL)
, m_pQuadMesh(NULL)
, m_pEffect(NULL)
, m_VS_DirectOut_V3(NULL)
{
}

CDipToColorRender::~CDipToColorRender(void)
{
	Uninit();
}

bool CDipToColorRender::Init( CRenderEngine* pEngine )
{
	HRESULT hr = E_FAIL;
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = pEngine->GetDevice();
	CResourceManager* pResMan = pEngine->GetResourceManager();

	m_pQuadMesh = pResMan->CreateQuadMesh(pDevice);
	ASSERT(m_pQuadMesh);
	m_pEffect = pResMan->CreatePixelShader(pDevice, _T("NewEffects/DipToColorTrans.psh"));
	ASSERT(m_pEffect);
	m_VS_DirectOut_V3 = pResMan->CreateVertexShader(pDevice, _T("Shaders/VS_DirectOutV3.VSH"));
	ASSERT(m_VS_DirectOut_V3);

	return true;
}

void CDipToColorRender::Uninit()
{
}

bool CDipToColorRender::Render( CVideoBuffer* pDest, CVideoBuffer *pSrcA, CVideoBuffer *pSrcB, FxParamBase* pParamRaw )
{
	DipToColorTransFxParam* pParam = (DipToColorTransFxParam*)pParamRaw;
	RESET_RENDER_TARGET(m_pEngine);
	SET_DEPTH_STENCIL(m_pEngine);
	CResourceManager* pResMan = m_pEngine->GetResourceManager();
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();

	if( pParam->bReverse )
		std::swap(pSrcA, pSrcB);

	CVideoBuffer* pSrcFullDef0 = pSrcA;
	CVideoBuffer* pSrcFullDef1 = pSrcB;

	// loading和计算工作

	bool  bUseTex1 = false;
	float Scale = pParam->fProcess*2;
	if( Scale > 1.f )	
	{
		Scale = 2-Scale;
		bUseTex1 = true;
	}


	CVideoBuffer* pSrcDef = bUseTex1?(pSrcFullDef1):(pSrcFullDef0);
	bool bYUYV = false;//pSrcDef->IsYUV16Buffer();
	bool bAlpha = false;//pSrcDef->HasSeparatedAlpha();
	bool bYUYVA = bYUYV && bAlpha;
	Buffer_Color_Format pSrcDef_bufferFormat = FMT_RGBA32;
	bool bYUVA = (pSrcDef_bufferFormat==FMT_YUVA32);
	float pSrcDef_fAlphaValue = 1.0f;

	// 渲染目标标志位设定

	// setRT
	CVideoBuffer* pDstDef = pDest;
	bool bOK = m_pEngine->SetRenderTarget(pDstDef);
	ASSERT(bOK);

	// VS constant
	D3DXMATRIX	*matView,*matPorj;
	pResMan->GetOrthoMatrix(&matView,&matPorj);
	D3DXMATRIX matCombined = (*matView)*(*matPorj);

	D3DXMATRIX matTextureSrc;
	//if( pDstDef->bIsCGBuffer )
	//	GenerateMatrix(m_pResMan,pSrcDef->handle,&matTextureSrc,mat_Image);
	//else
	//	GenerateMatrix(m_pResMan,pSrcDef->handle,&matTextureSrc,mat_Identity);
	bOK = GenerateMatrix(pSrcDef,&matTextureSrc,mat_Identity);
	ASSERT(bOK);

	pDevice->SetVertexShaderConstantF(0, (float*)&matCombined, 4);
	pDevice->SetVertexShaderConstantF(4, (float*)&matTextureSrc, 4);

	// PS constant
	D3DXCOLOR ColorDip(pParam->fColor[0], pParam->fColor[1], pParam->fColor[2], pParam->fColor[3]);	
	if( bYUVA)
		ColorConvertor::RGBA2(pSrcDef_bufferFormat, &ColorDip, &ColorDip);
	D3DXVECTOR2 ScaleAndAlpha(Scale, pSrcDef_fAlphaValue);
	pDevice->SetPixelShaderConstantF(0, ScaleAndAlpha, 1);
	pDevice->SetPixelShaderConstantF(1, (float*)&ColorDip,  1);

	D3DXCOLOR colorBlack(0.f,0.f,0.f,0.f);
	if( bYUVA)
		ColorConvertor::RGBA2(pSrcDef_bufferFormat, &colorBlack,&colorBlack);
	pDevice->SetPixelShaderConstantF(2,(float*)&colorBlack,4);

	// TEXTURE & stage state & sample state
	pDevice->SetTexture(0,pSrcDef->GetTexture());
	pDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_POINT);
	pDevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_POINT);

	//if( bYUYVA )
	//{
	//	pDevice->SetTexture(1,pSrcDef->pAlpha);
	//	pDevice->SetSamplerState(1,D3DSAMP_MAGFILTER,D3DTEXF_POINT);
	//	pDevice->SetSamplerState(1,D3DSAMP_MINFILTER,D3DTEXF_POINT);
	//}

	SetAlphaRenderState(pDevice, false);
	pDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);
	pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);

	//if( bYUYVA )
	//	m_pEngine->SetPixelShader(m_ResID_DIP_PS_YUYVA);
	//else
		pDevice->SetPixelShader(m_pEffect->GetPixelShaderPtr());

		LPDIRECT3DVERTEXSHADER9 vertexshader = m_VS_DirectOut_V3->GetVertexShaderPtr();

	if ( SUCCEEDED(pDevice->BeginScene()) )
	{
		m_pQuadMesh->DrawMesh(0, vertexshader);
		pDevice->EndScene();
	}

	// dest标志位设定

	return true;
}

