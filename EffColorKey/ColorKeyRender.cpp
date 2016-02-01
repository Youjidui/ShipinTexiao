#include "StdAfx.h"
#include "ColorKeyRender.h"
#include "../Utility/ColorConvertor.h"


#define WIDTH	0.06f

ColorKeyRender::ColorKeyRender(void)
: m_pEngine(NULL)
, m_pQuadMesh(NULL)
, m_matWorld(NULL)
, m_matPrj(NULL)
, m_matView(NULL)
, m_pBuffer(NULL)
{
	m_pBuffer = new NVVertex[12];
	memset(m_pBuffer, 0, sizeof(NVVertex[12]));
}

ColorKeyRender::~ColorKeyRender(void)
{
	delete[] m_pBuffer;
}

HRESULT ColorKeyRender::SetSourceTextureWithTrans( const DWORD dwSampler, CVideoBuffer* pTexDef, MAT_STYLE matStyle )
{
	HRESULT hr = E_FAIL;

	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();

	D3DXMATRIX matTex;
	bool bOK = GenerateMatrix(pTexDef, &matTex, matStyle);
	ASSERT(bOK);
	int registerOffset = 4 + dwSampler*4; // 0-4 registers was reserved for matWVP
	hr = pDevice->SetVertexShaderConstantF(registerOffset, (float*)&matTex, 4);
	//return SetSourceTexture(dwSampler, pTexDef->pContainer);
	hr = pDevice->SetTextureStageState( dwSampler, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	hr = pDevice->SetTextureStageState( dwSampler, D3DTSS_TEXCOORDINDEX,dwSampler );
	//LPDIRECT3DTEXTURE9 lpTex = CreateTexture(pDevice, pTexDef);
	LPDIRECT3DTEXTURE9 lpTex = pTexDef->GetTexture();
	if(lpTex)
		hr = pDevice->SetTexture( dwSampler,  lpTex);
	//lpTex->Release();

	return hr;
}


bool ColorKeyRender::Init(CRenderEngine* pEngine )
{
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = pEngine->GetDevice();
	CResourceManager* pResMan = pEngine->GetResourceManager();
	m_pQuadMesh = pResMan->CreateQuadMesh(pDevice);

	// 创建四个三角形的Mesh
	m_pBuffer[0]._diffuse = 0xFFFFFFFF;
	m_pBuffer[0]._Pos = D3DXVECTOR3(-0.5f, 0.5f, 0.0f);
	m_pBuffer[1]._diffuse = 0xFFFFFFFF;
	m_pBuffer[1]._Pos = D3DXVECTOR3(-0.5f+WIDTH, 0.5f, 0.0f);
	m_pBuffer[2]._diffuse = 0xFFFFFFFF;
	m_pBuffer[2]._Pos = D3DXVECTOR3(-0.5f, 0.5f-WIDTH, 0.0f);
	m_pBuffer[3]._diffuse = 0xFFFFFFFF;
	m_pBuffer[3]._Pos = D3DXVECTOR3(0.0f, 0.5f, 0.0f);
	m_pBuffer[4]._diffuse = 0xFFFFFFFF;
	m_pBuffer[4]._Pos = D3DXVECTOR3(0.0f, 0.5f-WIDTH, 0.0f);
	m_pBuffer[5]._diffuse = 0xFFFFFFFF;
	m_pBuffer[5]._Pos = D3DXVECTOR3(0.0f-WIDTH, 0.5f, 0.0f);
	m_pBuffer[6]._diffuse = 0xFFFFFFFF;
	m_pBuffer[6]._Pos = D3DXVECTOR3(0.0f, -0.5f, 0.0f);
	m_pBuffer[7]._diffuse = 0xFFFFFFFF;
	m_pBuffer[7]._Pos = D3DXVECTOR3(0.0f-WIDTH, -0.5f, 0.0f);
	m_pBuffer[8]._diffuse = 0xFFFFFFFF;
	m_pBuffer[8]._Pos = D3DXVECTOR3(0.0f, -0.5f+WIDTH, 0.0f);
	m_pBuffer[9]._diffuse = 0xFFFFFFFF;
	m_pBuffer[9]._Pos = D3DXVECTOR3(-0.5f, -0.5f, 0.0f);
	m_pBuffer[10]._diffuse = 0xFFFFFFFF;
	m_pBuffer[10]._Pos = D3DXVECTOR3(-0.5f, -0.5f+WIDTH, 0.0f);
	m_pBuffer[11]._diffuse = 0xFFFFFFFF;
	m_pBuffer[11]._Pos = D3DXVECTOR3(-0.5f+WIDTH, -0.5f, 0.0f);

	int nMacroCount = 2;
	//const char** pMacro_YUYV = new const char*[nMacroCount];
	//pMacro_YUYV[0] = "FMT_YUYV";
	//pMacro_YUYV[1] = NULL;
	//const char** pMacro_UYVY = new const char*[nMacroCount];
	//pMacro_UYVY[0] = "FMT_UYVY";
	//pMacro_UYVY[1] = NULL;

	m_PS_ColorKey_RGB32_NOMASK = pResMan->CreatePixelShader(pDevice, _T("NewEffects/PS_colorkey_PS3.PSH"));
	ASSERT(m_PS_ColorKey_RGB32_NOMASK);
	m_PS_ColorKey_RGB32_MASK = pResMan->CreatePixelShader(pDevice, _T("NewEffects/PS_colorkey_RGB32_MASK_v3.PSH"));
	ASSERT(m_PS_ColorKey_RGB32_MASK);

	//m_PS_ColorKey_YUV16_NA_YUYV_NOMASK = pResMan->CreatePixelShader(pDevice, _T("NewEffects/PS_colorkey_YUV16.PSH"), pMacro_YUYV, nMacroCount);
	//ASSERT(m_PS_ColorKey_YUV16_NA_YUYV_NOMASK);
	//m_PS_ColorKey_YUV16_NA_UYVY_NOMASK = pResMan->CreatePixelShader(pDevice, _T("NewEffects/PS_colorkey_YUV16.PSH"), pMacro_UYVY, nMacroCount);
	//ASSERT(m_PS_ColorKey_YUV16_NA_UYVY_NOMASK);
	//m_PS_ColorKey_YUV16_AP_YUYV_NOMASK = pResMan->CreatePixelShader(pDevice, _T("NewEffects/PS_colorkey_YUV16_AP.PSH"), pMacro_YUYV, nMacroCount);
	//ASSERT(m_PS_ColorKey_YUV16_AP_YUYV_NOMASK);
	//m_PS_ColorKey_YUV16_AP_UYVY_NOMASK = pResMan->CreatePixelShader(pDevice, _T("NewEffects/PS_colorkey_YUV16_AP.PSH"), pMacro_UYVY, nMacroCount);
	//ASSERT(m_PS_ColorKey_YUV16_AP_UYVY_NOMASK);

	//m_PS_ColorKey_YUV16_NA_YUYV_MASK = pResMan->CreatePixelShader(pDevice, _T("NewEffects/PS_colorkey_YUV16_MASK.PSH"), pMacro_YUYV, nMacroCount);
	//ASSERT(m_PS_ColorKey_YUV16_NA_YUYV_MASK);
	//m_PS_ColorKey_YUV16_NA_UYVY_MASK = pResMan->CreatePixelShader(pDevice, _T("NewEffects/PS_colorkey_YUV16_MASK.PSH"), pMacro_UYVY, nMacroCount);
	//ASSERT(m_PS_ColorKey_YUV16_NA_UYVY_MASK);
	//m_PS_ColorKey_YUV16_AP_YUYV_MASK = pResMan->CreatePixelShader(pDevice, _T("NewEffects/PS_colorkey_YUV16_AP_MASK.PSH"), pMacro_YUYV, nMacroCount);
	//ASSERT(m_PS_ColorKey_YUV16_AP_YUYV_MASK);
	//m_PS_ColorKey_YUV16_AP_UYVY_MASK = pResMan->CreatePixelShader(pDevice, _T("NewEffects/PS_colorkey_YUV16_AP_MASK.PSH"), pMacro_UYVY, nMacroCount);
	//ASSERT(m_PS_ColorKey_YUV16_AP_UYVY_MASK);

	m_PS_Blur = pResMan->CreatePixelShader(pDevice, _T("NewEffects/PS_Blur_PS3.psh"));
	ASSERT(m_PS_Blur);

	const char** pMacro_EXPAND = new const char*[nMacroCount];
	pMacro_EXPAND[0] = "EXPAND";
	pMacro_EXPAND[1] = NULL;
	const char** pMacro_SHRINK = new const char*[nMacroCount];
	pMacro_SHRINK[0] = "SHRINK";
	pMacro_SHRINK[1] = NULL;

	m_PS_FineTune_RGB32_EXPAND = pResMan->CreatePixelShader(pDevice, _T("NewEffects/PS_FineTune_RGB32.psh"), pMacro_EXPAND, nMacroCount);
	ASSERT(m_PS_FineTune_RGB32_EXPAND);
	//m_PS_FineTune_YUV16_EXPAND = pResMan->CreatePixelShader(pDevice, _T("NewEffects/PS_FineTune_YUV16.psh"), pMacro_EXPAND, nMacroCount);
	//ASSERT(m_PS_FineTune_YUV16_EXPAND);
	m_PS_FineTune_RGB32_SHRINK = pResMan->CreatePixelShader(pDevice, _T("NewEffects/PS_FineTune_RGB32.psh"), pMacro_SHRINK, nMacroCount);
	ASSERT(m_PS_FineTune_RGB32_SHRINK);
	//m_PS_FineTune_YUV16_SHRINK = pResMan->CreatePixelShader(pDevice, _T("NewEffects/PS_FineTune_YUV16.psh"), pMacro_SHRINK, nMacroCount);
	//ASSERT(m_PS_FineTune_YUV16_SHRINK);
	delete[] pMacro_EXPAND;
	delete[] pMacro_SHRINK;

	//m_PS_ColorKey_YUVA_NOMASK = pResMan->CreatePixelShader(pDevice, _T("NewEffects/PS_colorkey_YUVA.psh"));
	//ASSERT(m_PS_ColorKey_YUVA_NOMASK);

	//m_PS_ColorKey_YUVA_MASK = pResMan->CreatePixelShader(pDevice, _T("NewEffects/PS_colorkey_YUVA_MASK.psh"));
	//ASSERT(m_PS_ColorKey_YUVA_MASK);

	m_VS_DirectOut = pResMan->CreateVertexShader(pDevice, _T("Shaders/VS_DirectOut.VSH"));
	ASSERT(m_VS_DirectOut);
	m_VS_DirectOut_V3 = pResMan->CreateVertexShader(pDevice, _T("Shaders/VS_DirectOutV3.VSH"));
	ASSERT(m_VS_DirectOut_V3);

	return true;
}

void ColorKeyRender::Uninit()
{

}

bool _fine_tune_enabled()
{
	return false;
}

bool _blur_enabled()
{
	return false;
}

bool ColorKeyRender::Render( CVideoBuffer* pDest, CVideoBuffer* pSrc, ColorKeyParam* pParam )
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMan = m_pEngine->GetResourceManager();
	CVideoBufferManager* pBufMan = m_pEngine->GetVideoBufferManager();
	RESET_RENDER_TARGET(m_pEngine);

	// setup world/view/projection transformation
	D3DXMATRIX  matWorld, *pMatView = NULL, *pMatProj = NULL;
	D3DXMatrixIdentity(&matWorld);
	pResMan->GetOrthoMatrix(&pMatView, &pMatProj);
	D3DXMATRIX matWVP = (*pMatView)* (*pMatProj);
	pDevice->SetVertexShaderConstantF(0, (float*)matWVP, 4);

	const VideoBufferInfo& bi = pSrc->GetVideoBufferInfo();
	// we'll render 3 passes at most, 3 dest-RT and 1 src-RT, totally 4 RTs
	CVideoBuffer* pRTDef[4] = {NULL};
	pRTDef[0] = pSrc;
	m_render_pass[0] = &ColorKeyRender::_colorkey_pass;
	int pass_count = 1;

	// add fine tune pass to queue
	if ( _fine_tune_enabled() )
	{
		VideoBufferInfo mediaBI = {D3DFMT_A8R8G8B8, VideoBufferInfo::SYSTEM_MEM, VideoBufferInfo::_IN_OUT, bi.nWidth, bi.nWidth, 0, 0};
		CVideoBuffer* pMedia = pBufMan->CreateVideoBuffer(mediaBI);
		if(pMedia)
		{
			pRTDef[pass_count] = pMedia;
			m_render_pass[pass_count] = &ColorKeyRender::_fine_tune_pass;
			pass_count++;
		}
	}

	// add blur pass to queue
	if ( _blur_enabled() )
	{
		VideoBufferInfo mediaBI = {D3DFMT_A8R8G8B8, VideoBufferInfo::SYSTEM_MEM, VideoBufferInfo::_IN_OUT, bi.nWidth, bi.nWidth, 0, 0};
		CVideoBuffer* pMedia = pBufMan->CreateVideoBuffer(mediaBI);
		if(pMedia)
		{
			pRTDef[pass_count] = pMedia;
			m_render_pass[pass_count] = &ColorKeyRender::_blur_pass;
			pass_count++;
		}
	}

	// setup final RT
	pRTDef[pass_count] = pDest;
	pass_count++;

	// render all passes in queue
	for ( int i=0 ; i<pass_count-1 ; i++ )
	{
		(this->*m_render_pass[i])( pRTDef[i], pRTDef[i+1], pParam );
	}

	for ( int i=1 ; i<pass_count-1 ; i++ )
	{
		//m_pResMan->FreeRTBuffer(pRTDef[i]->handle);
		pBufMan->ReleaseVideoBuffer(pRTDef[i]);
		pRTDef[i] = NULL;
	}

	pDevice->SetRenderTarget(1, NULL);
	pDevice->SetPixelShader(NULL);
	pDevice->SetTexture(0, NULL);
	pDevice->SetTexture(1, NULL);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return true;
}

void ColorKeyRender::_colorkey_pass(CVideoBuffer* pSrcDef, CVideoBuffer* pDstDef, FxParamBase* pFxParam)
{
	// choose pixel shader
	const int RGB32 = 0;
	const int YUV16 = 1;
	const int NA = 0;
	const int AP = 1;
	const int UYVY = 0;
	const int YUYV = 1;
	const int NOMASK = 0;
	const int MASK = 1;
	CPixelShader* shader_array[2][2][2][2] = {NULL};
	shader_array[RGB32][NA][YUYV][NOMASK]   = m_PS_ColorKey_RGB32_NOMASK;
	shader_array[RGB32][NA][UYVY][NOMASK]   = m_PS_ColorKey_RGB32_NOMASK;
	shader_array[RGB32][AP][YUYV][NOMASK]   = m_PS_ColorKey_RGB32_NOMASK;
	shader_array[RGB32][AP][UYVY][NOMASK]   = m_PS_ColorKey_RGB32_NOMASK;
	shader_array[RGB32][NA][YUYV][MASK]     = m_PS_ColorKey_RGB32_MASK;
	shader_array[RGB32][NA][UYVY][MASK]     = m_PS_ColorKey_RGB32_MASK;
	shader_array[RGB32][AP][YUYV][MASK]     = m_PS_ColorKey_RGB32_MASK;
	shader_array[RGB32][AP][UYVY][MASK]     = m_PS_ColorKey_RGB32_MASK;
	//shader_array[YUV16][NA][YUYV][NOMASK]   = m_PS_ColorKey_YUV16_NA_YUYV_NOMASK;
	//shader_array[YUV16][NA][UYVY][NOMASK]   = m_PS_ColorKey_YUV16_NA_UYVY_NOMASK;
	//shader_array[YUV16][AP][YUYV][NOMASK]   = m_PS_ColorKey_YUV16_AP_YUYV_NOMASK;
	//shader_array[YUV16][AP][UYVY][NOMASK]   = m_PS_ColorKey_YUV16_AP_UYVY_NOMASK;
	//shader_array[YUV16][NA][YUYV][MASK]     = m_PS_ColorKey_YUV16_NA_YUYV_MASK;
	//shader_array[YUV16][NA][UYVY][MASK]     = m_PS_ColorKey_YUV16_NA_UYVY_MASK;
	//shader_array[YUV16][AP][YUYV][MASK]     = m_PS_ColorKey_YUV16_AP_YUYV_MASK;
	//shader_array[YUV16][AP][UYVY][MASK]     = m_PS_ColorKey_YUV16_AP_UYVY_MASK;

	BOOL shader_yuv16 = FALSE;
	BOOL shader_alpha = FALSE;
	BOOL shader_yuyv = FALSE;
	BOOL shader_mask = FALSE;
	CPixelShader* pixelshader = shader_array[shader_yuv16][shader_alpha][shader_yuyv][shader_mask];

	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMan = m_pEngine->GetResourceManager();

	pDevice->SetPixelShader(pixelshader->GetPixelShaderPtr());

	pDevice->SetRenderTarget(0, pDstDef->GetSurface());

	int cur_tex_stage = 0;

	// texture stage - 0, source image
	SetSourceTextureWithTrans(cur_tex_stage, pSrcDef, mat_Image);
	cur_tex_stage++;
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	// texture stage - 1, alpha
	//if ( pSrcDef->IsYUV16Buffer() && pSrcDef->HasSeparatedAlpha() )
	//{
	//	m_pEngine->SetSourceTexture(cur_tex_stage, pSrcDef->pAlpha);
	//	cur_tex_stage++;
	//}

	// texture stage - 2, mask
	//if ( shader_mask )
	//{
	//	TP_VBufferDef *pMaskDef = m_pResMan->GetBufferDef(pSrcDef->pMask[0]);
	//	m_pEngine->SetSourceTextureWithTrans(cur_tex_stage, pMaskDef, mat_Image);
	//	cur_tex_stage++;
	//}

	// choose vertex shader
	LPDIRECT3DVERTEXSHADER9 vertexshader = m_VS_DirectOut_V3->GetVertexShaderPtr();
	//if ( cur_tex_stage == 2 )
	//	vertexshader = m_pResMan->m_VSDirectOut_2stages_V3;
	//else if ( cur_tex_stage == 3 )
	//	vertexshader = m_pResMan->m_VSDirectOut_3stages_V3;

	ColorKeyParam* m_pParam = (ColorKeyParam*)pFxParam;
	// render
	if ( SUCCEEDED(pDevice->BeginScene()) )
	{
		// setup pixel shader env
		D3DXVECTOR4 ps_detector(
			m_pParam->fAngle1,  m_pParam->fAngle2,
			m_pParam->fLength1, m_pParam->fLength2 );
		pDevice->SetPixelShaderConstantF(0, (float*)ColorConvertor::GetMatrix_RGBA2YUVA(),  4);
		pDevice->SetPixelShaderConstantF(4, (float*)&ps_detector,   1);
		pDevice->SetPixelShaderConstantF(5, (float*)&m_pParam->fKeyColor, 1);
		float ang_soft = m_pParam->fSoftness/3.0f;
		float len_soft = m_pParam->fSoftness/9.0f;
		D3DXVECTOR4 lo_bound(ps_detector.x-ang_soft, ps_detector.y-ang_soft, ps_detector.z-len_soft, ps_detector.w-len_soft);
		D3DXVECTOR4 hi_bound(ps_detector.x+ang_soft, ps_detector.y+ang_soft, ps_detector.z+len_soft, ps_detector.w+len_soft);
		pDevice->SetPixelShaderConstantF(6, (float*)&lo_bound, 1);
		pDevice->SetPixelShaderConstantF(7, (float*)&hi_bound, 1);
		pDevice->SetPixelShaderConstantF(8, (float*)&D3DXVECTOR2(m_pParam->fGrayMin,m_pParam->fGrayMax), 1);
		//D3DXVECTOR2  fIsReverse=D3DXVECTOR2( m_pParam->bIsReverse*1.0f,pSrcDef->bIsCG_BlenedBuffer);
		D3DXVECTOR2  fIsReverse=D3DXVECTOR2( m_pParam->bIsReverse*1.0f, 0.f);
		pDevice->SetPixelShaderConstantF(9, (float*)&fIsReverse, 1);

		// draw
		//m_pEngine->ApplyCurRenderState();
		m_pQuadMesh->DrawMesh(0, vertexshader);
		pDevice->EndScene();
	}
	pDevice->SetPixelShader(NULL);
	pDevice->SetTexture(0, NULL);
	pDevice->SetTexture(1, NULL);
	pDevice->SetRenderTarget(1, NULL);

	// 	m_pResMan->DumpResourceToFile(pSrcDef->handle,L"C:/src.dds",true);
	// 	m_pResMan->DumpResourceToFile(pDstDef->handle,L"C:/dest.dds",true);
}


void ColorKeyRender::_fine_tune_pass(CVideoBuffer* pSrcDef, CVideoBuffer* pDstDef, FxParamBase* pFxParam)
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMan = m_pEngine->GetResourceManager();

	pDevice->SetRenderTarget(0, pDstDef->GetSurface());
	SetSourceTextureWithTrans(0, pSrcDef, mat_Image);

	ColorKeyParam* m_pParam = (ColorKeyParam*)pFxParam;
	float fFineTune = fabsf(m_pParam->fFineTune);
		CPixelShader* pixelshader = NULL;
		pixelshader = fFineTune>0.0f ? m_PS_FineTune_RGB32_EXPAND : m_PS_FineTune_RGB32_SHRINK;
	pDevice->SetPixelShader(pixelshader->GetPixelShaderPtr());
	const VideoBufferInfo& bi = pSrcDef->GetVideoBufferInfo();
	D3DXVECTOR2 vTexelSize(1.0f/(float)bi.nWidth, 1.0f/(float)bi.nHeight);
	pDevice->SetPixelShaderConstantF(0, (float*)&vTexelSize, 1);
	pDevice->SetPixelShaderConstantF(1, &fFineTune, 1);
	if ( SUCCEEDED(pDevice->BeginScene()) )
	{
		//m_pEngine->ApplyCurRenderState();
		//pResMan->GetQuadMesh()->DrawMesh(m_pResMan->m_VSDirectOut);
		m_pQuadMesh->DrawMesh(0, m_VS_DirectOut->GetVertexShaderPtr());
		pDevice->EndScene();
	}
	pDevice->SetTexture(0, NULL);
	pDevice->SetRenderTarget(1, NULL);
	//m_pResMan->DumpResourceToFile(pSrcDef->handle, L"c:\\finetune_src_alpha.dds", true);
	//m_pResMan->DumpResourceToFile(pDstDef->handle, L"c:\\finetune_dest_alpha.dds", true);
}


void ColorKeyRender::_blur_pass(CVideoBuffer* pSrcDef, CVideoBuffer* pDstDef, FxParamBase* pFxParam)
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMan = m_pEngine->GetResourceManager();

	pDevice->SetRenderTarget(0, pDstDef->GetSurface());
	SetSourceTextureWithTrans(0, pSrcDef, mat_Image);

	ColorKeyParam* m_pParam = (ColorKeyParam*)pFxParam;
	float fBlurness = m_pParam->fBlurness;
	if ( SUCCEEDED( pDevice->BeginScene() ) )
	{
		pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		pDevice->SetPixelShader(m_PS_Blur->GetPixelShaderPtr());
		const VideoBufferInfo& bi = pSrcDef->GetVideoBufferInfo();
		D3DXVECTOR2 vQTexSize( 0.25f * fBlurness / (float)bi.nWidth, 0.25f * fBlurness / bi.nHeight);
		pDevice->SetPixelShaderConstantF(0, (const float*)&vQTexSize, 1);

		//m_pEngine->ApplyCurRenderState();
		m_pQuadMesh->DrawMesh(0, m_VS_DirectOut->GetVertexShaderPtr());
		pDevice->EndScene();
	}
	//m_pResMan->DumpResourceToFile(hDestID, L"c:\\dest.dds");
	pDevice->SetRenderTarget(1, NULL);
	pDevice->SetPixelShader( NULL );
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetTexture(0, NULL);
}
