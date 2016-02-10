#include "StdAfx.h"
#include "RingsRender.h"
#include "../Utility/SafeDelete.h"
#include "../Utility/mathmacros.h"
#include "../Utility/PathSettings.h"
#include "../FxParam.h"
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi")
#include "../Logger/Logging.h"


#define _TRANS

static 
VOID WINAPI ColorFill (D3DXVECTOR4* pOut, const D3DXVECTOR2* pTexCoord, 
					   const D3DXVECTOR2* pTexelSize, LPVOID pData)
{

	*pOut = D3DXVECTOR4((rand()%10001)/10001.0f, (rand()%10001)/10001.0f, (rand()%10001)/10001.0f,(rand()%10001)/10001.0f);
	if(pData)
	{
		//TPRenderEngine* pEngine = (TPRenderEngine*)pData;
		//D3DXCOLOR src = D3DXCOLOR(pOut->x,pOut->y,pOut->z,pOut->w),dest;
		//pEngine->ColorConv_RGBA_YUVA(&src,&dest);
		//*pOut = D3DXVECTOR4(dest.r,dest.g,dest.b,dest.a);
		D3DXCOLOR src = D3DXCOLOR(pOut->x,pOut->y,pOut->z,pOut->w),dest;
		ColorConvertor::RGBA2YUVA(&src,&dest);
		*pOut = D3DXVECTOR4(dest.r,dest.g,dest.b,dest.a);
	}

}


CRingsRender::CRingsRender(void)
: m_pEngine(NULL)
, m_pQuadMesh(NULL)
, m_pEffect(NULL)
, m_pShapeTexture(NULL)
{
}

CRingsRender::~CRingsRender(void)
{
}

bool CRingsRender::Init( CRenderEngine* pEngine)
{
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	m_pQuadMesh = pResMgr->CreateQuadMesh(pDevice);
	ASSERT(NULL != m_pQuadMesh);
	m_pEffect = pResMgr->CreateEffect(pDevice, _T("NewEffects/Rings.fx"));
	ASSERT(NULL != m_pEffect);
	//{
	//	D3DDeviceSettings  info = m_pEngine->GetD3DDeviceSettings();
	//	if( SUCCEEDED( m_pEngine->GetD3DObject()->CheckDeviceFormat( info.AdapterOrdinal, info.DeviceType, info.AdapterFormat, D3DUSAGE_QUERY_FILTER, D3DRTYPE_TEXTURE, D3DFMT_G32R32F ) ) )
	//		m_uResID_fx = pResMan->CreateShader(_T("GRings8.fxo"), RS_FX, &uuid_fx);
	//	else
	//		m_uResID_fx = pResMan->CreateShader(_T("GRings7.fxo"), RS_FX, &uuid_fx);

	//	m_pEffect = ((CBaseFx*)m_pResMan->GetResource(m_uResID_fx))->GetFxPtr();
	//	assert(m_pEffect);
	//}

	LPCTSTR pszShapeTexture = _T("ShapeWipe/ShapeTexture.dds");
	TCHAR szTexFilePath[MAX_PATH];
	PathSettings::BuildResourcePath(szTexFilePath, sizeof(szTexFilePath), pszShapeTexture);
	bool bOK = PathFileExists(szTexFilePath);
	ASSERT(bOK);
	m_pShapeTexture = pResMgr->CreateTexture(pDevice, szTexFilePath,1800,8,0,D3DFMT_G32R32F,D3DPOOL_MANAGED, pszShapeTexture);
	ASSERT(m_pShapeTexture);
	if(!m_pShapeTexture)
	{
		char buf[MAX_PATH * 2];
		wcstombs(buf, szTexFilePath, MAX_PATH);
		LOG_ERROR_FORMAT("%s:pResMgr->CreateTexture failed. The texture file name is %s", __FUNCTION__, buf);
	}

	srand(1000);
	//m_uResID_Noise = m_pResMan->CreateTexture(512,256,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&UUID_TEXTURE_NOISE);
	//m_pNoiseTexture = ((CBaseTexture*)m_pResMan->GetResource(m_uResID_Noise))->GetTexture();
	CVideoBufferManager* pBufMgr = m_pEngine->GetVideoBufferManager();
	VideoBufferInfo mediaBI = {D3DFMT_A8R8G8B8, VideoBufferInfo::VIDEO_MEM, VideoBufferInfo::_IN, 512, 256, 0, 0};		//the same as SonyPinpRender
	m_pNoiseTexture = pBufMgr->CreateVideoBuffer(mediaBI);
	ASSERT(m_pNoiseTexture);
	LPDIRECT3DTEXTURE9 pTex = m_pNoiseTexture->GetTexture();
	ASSERT(pTex);
	BOOL bRGBA2YUVA = false;
	if (FAILED (D3DXFillTexture (pTex, &ColorFill, (LPVOID)bRGBA2YUVA)))
	{
		return FALSE;
	}

	return true;
}

bool CRingsRender::Render( CVideoBuffer* pDstDef, CVideoBuffer* pSrcDef, CVideoBuffer* pSrcB, FxParamBase* pParamRaw)
{
	RESET_RENDER_TARGET(m_pEngine);
	SET_DEPTH_STENCIL(m_pEngine);

	const VideoBufferInfo& biSrc = pSrcDef->GetVideoBufferInfo();
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMan = m_pEngine->GetResourceManager();
	RingsFxParam* pParam = (RingsFxParam*)pParamRaw;	

	D3DXMATRIXA16 matWorld,matCombine,matTex;
	D3DXMatrixIdentity(&matWorld);

	D3DXMatrixIdentity(&matTex);
	//const Custom_Profile * pProfile = m_pEngine->GetCurProfile();
	int nEditWidth, nEditHeight;
	m_pEngine->GetTargetVideoSize(nEditWidth, nEditHeight);
	matTex._11 = nEditWidth / (float)biSrc.nAllocWidth;
	matTex._22 = nEditHeight / (float)biSrc.nAllocHeight;
	matTex._31 = 0.5f / biSrc.nAllocWidth;
	matTex._32 = 0.5f / biSrc.nAllocHeight;

#ifdef _TRANS
	if(pParam->nEffectNo == 0)
		m_pEffect->SetTechnique("RingsTrans");
	else
		m_pEffect->SetTechnique("BrokenRingsTrans");	
#else		
	m_pEngine->SetRenderTarget(0,pDstDef->handle,pDstDef->COLOR_BLACK(),0x0);
	if(pParam->nEffectNo == 0)
		m_pEffect->SetTechnique("Rings");
	else
		m_pEffect->SetTechnique("BrokenRings");
	
#endif
	//float fAspect = m_pResMan->GetAspect() * (pSrcDef->BaseWidth * (pSrcDef->IsYUV16Buffer() ? 2.0f : 1.0f))  / (float)(pSrcDef->BaseHeight  * m_pResMan->GetAspectVerifyCoef());		
	float	fAspect = nEditWidth*1.0f/nEditHeight;
	
	D3DXMATRIX *matView = NULL, *matProj= NULL;
	pResMan->GetOrthoMatrix(&matView, &matProj);
	matCombine = matWorld * *matView * *matProj;	
	m_pEffect->SetMatrix("g_matWVP",&matCombine);	
	
	m_pEffect->SetMatrix("g_matTex",&matTex);
	m_pEffect->SetTexture("g_txColor",pSrcDef->GetTexture());
	m_pEffect->SetTexture("g_txShape",m_pShapeTexture->GetTexture());
	m_pEffect->SetTexture("g_txNoise",m_pNoiseTexture->GetTexture());

	float pSrcDef_fAlphaValue = 1.0f;
	D3DXVECTOR4 vRings(pParam->fTranslate, pParam->fRandomTranslate, pSrcDef_fAlphaValue, 0.0f);
	m_pEffect->SetVector("g_vRings",&vRings); 

	int nPattern = pParam->nPattern;
	switch(pParam->nPattern) {
	case 0:nPattern = 1;break;
	case 1:nPattern = 0;break;
	case 3:nPattern = 4;break;
	case 4:nPattern = 3;break;
	}
	D3DXVECTOR4 vMisc((nPattern + 0.5f) / 8.0f,fAspect,pParam->fCenterX * fAspect / 2.0f,-pParam->fCenterY / 2.0f);	
	m_pEffect->SetVector("g_vMisc",&vMisc);
 
   
	D3DXVECTOR4 vBlock(pParam->fWidth + 1e-6f, pParam->fRandomWidth, D3DXToRadian( pParam->fSpiral ), pParam->fRandomPixel);
	vBlock.x /= 2.5f;
	m_pEffect->SetVector("g_vBlock",&vBlock);

	D3DXVECTOR2 vAspect(1.0f,1.0f);
	if(pParam->fAspect > 0.0f)
		vAspect.y = (1.0f + pParam->fAspect);
	else 
		vAspect.x = (1.0f + fabs(pParam->fAspect)); 	

	D3DXMATRIXA16 matRote,matAspect;
	D3DXMatrixScaling(&matAspect,vAspect.x,vAspect.y,1.0f);
	D3DXMatrixRotationZ(&matRote,D3DXToRadian( pParam->fRotate ));
	m_pEffect->SetMatrix("g_matTexRote",&(matRote * matAspect));		
	m_pEffect->SetMatrix("g_matTexRoteR",&matRote);

	if(SUCCEEDED(pDevice->BeginScene()))
	{
		UINT cPass;
		m_pEffect->Begin(&cPass,0);
		m_pEffect->BeginPass(0);
		m_pQuadMesh->DrawMeshFx();
		m_pEffect->EndPass();
		m_pEffect->End();
		pDevice->EndScene();
	}

	return true;
}