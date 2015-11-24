#include "StdAfx.h"
#include "MipMapGenerator.h"

#pragma warning(disable: 4244 4018)

CMipMapGenerator::CMipMapGenerator(void)
: m_pEngine(NULL)
//, m_pMesh(NULL)
, m_pEffect(NULL)
{
	memset(m_pMesh, 0, sizeof(m_pMesh));
}

CMipMapGenerator::~CMipMapGenerator(void)
{
}

bool CMipMapGenerator::Init(CRenderEngine* pEngine)
{
	HRESULT hr = E_FAIL;
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = pEngine->GetDevice();
	CResourceManager* pResMan = pEngine->GetResourceManager();
	m_pEffect = pResMan->CreateEffect(pDevice, _T("NewEffects/fx_GDirectOut.fx"));
	ASSERT(m_pEffect);
	m_pMesh[0] = pResMan->CreateQuadWHMipmapMesh(pDevice);
	ASSERT(m_pMesh[0]);
	m_pMesh[1] = pResMan->CreateQuadWMipmapMesh(pDevice);
	ASSERT(m_pMesh[1]);
	m_pMesh[2] = pResMan->CreateQuadHMipmapMesh(pDevice);
	ASSERT(m_pMesh[2]);
	return true;
}

void CMipMapGenerator::Uninit()
{

}

bool CMipMapGenerator::Render(CVideoBuffer* pDest, CVideoBuffer *pSrc, FxParamBase* pParam)
{
	HRESULT hr = GenerateMipMap(pDest, pSrc);
	ASSERT(SUCCEEDED(hr));
	return SUCCEEDED(hr);
}

HRESULT CMipMapGenerator::GenerateMipMap(CVideoBuffer* pMipMap, CVideoBuffer* pSrcDef)
{
	HRESULT hr = E_FAIL;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMan = m_pEngine->GetResourceManager();

	RESET_RENDER_TARGET(m_pEngine);
	SET_DEPTH_STENCIL(m_pEngine);

	D3DSURFACE_DESC desc;
	LPDIRECT3DSURFACE9 pRT = pMipMap->GetSurface();
	hr = pDevice->SetRenderTarget( 0, pRT );
	ASSERT(SUCCEEDED(hr));
	hr = pRT->GetDesc(&desc);
	ASSERT(SUCCEEDED(hr));

	D3DVIEWPORT9 vPort,vOldPort;
	hr = pDevice->GetViewport(&vPort);
	ASSERT(SUCCEEDED(hr));
	vOldPort = vPort;
	//const Custom_Profile * pProfile = m_pEngine->GetCurProfile();
	int nEditWidth, nEditHeight;
	m_pEngine->GetTargetVideoSize(nEditWidth, nEditHeight);
	vPort.Width = min(nEditWidth + 64,vPort.Width);
	vPort.Height = min(nEditHeight + 64,vPort.Height);
	pDevice->SetViewport(&vPort);
	pDevice->Clear(0,NULL,D3DCLEAR_TARGET, 0, 1.0, 0L); 
	pDevice->SetViewport(&vOldPort);

	D3DXMATRIXA16 matCombine , matWorld, matTex;
	D3DXMatrixIdentity(&matWorld);

	const VideoBufferInfo& viSrc = pSrcDef->GetVideoBufferInfo();
	float pSrcDef_GetImageWidth = viSrc.nWidth;
	float pSrcDef_GetImageHeight = viSrc.nHeight;
	matWorld._11 = pSrcDef_GetImageWidth / (float)desc.Width;
	matWorld._22 = pSrcDef_GetImageHeight / (float)desc.Height;
	matWorld._41 = -0.5f + matWorld._11 / 2.0f;
	matWorld._42 = 0.5f  - matWorld._22 / 2.0f;
	//if(bLocation)
	//{
	//	matWorld._41 += pSrcDef->OffsetX / desc.Width;
	//	matWorld._42 -= pSrcDef->OffsetY /desc.Height;
	//}

	//matCombine = matWorld * m_matView * m_matProj;
	D3DXMATRIX *matView = NULL, *matProj= NULL;
	pResMan->GetPerspectiveMatrix( &matView, &matProj);
	matCombine = matWorld * *matView * *matProj;

	GenerateMatrix(pSrcDef, &matTex, mat_Image );
	float pSrcDef_BaseWidth = viSrc.nAllocWidth;
	float pSrcDef_BaseHeight = viSrc.nAllocHeight;
	matTex._41 =  0.5f / pSrcDef_BaseWidth;
	matTex._42 =  0.5f / pSrcDef_BaseHeight;

	m_pEffect->SetMatrix("g_matWorldViewProj",&matCombine);
	m_pEffect->SetMatrix("g_matTexture",&matTex);
	m_pEffect->SetTexture("g_txColor",pSrcDef->GetTexture());
	m_pEffect->SetTechnique("Picture");

	if(SUCCEEDED(pDevice->BeginScene()))
	{ 		
		UINT cPass;
		m_pEffect->Begin(&cPass,0);
		m_pEffect->BeginPass(0);
		//if(bLocation)
		//{
		//	if(pSrcDef->GetImageWidth() >= pProfile->nEditWidth && pSrcDef->GetImageHeight() >= pProfile->nEditHeight)
		//		m_pMesh[0]->DrawMeshFx();
		//	else if(pSrcDef->GetImageWidth() >= pProfile->nEditWidth )
		//		m_pMesh[1]->DrawMeshFx();
		//	else if(pSrcDef->GetImageHeight() >= pProfile->nEditHeight)
		//		m_pMesh[2]->DrawMeshFx();
		//	else
		//		m_pResMan->GetQuadMesh()->DrawMeshFx();
		//}
		//else
			bool bOK = m_pMesh[0]->DrawMeshFx();
		ASSERT(bOK);

		m_pEffect->EndPass();
		m_pEffect->End();	
		pDevice->EndScene();		
	}	

	//  	m_pResMan->DumpResourceToFile(pSrcDef->handle,L"c:\\src.dds");
	//  	D3DXSaveTextureToFile(L"c:\\mipmap.bmp",D3DXIFF_BMP,pMipMap->GetTexture(),NULL);
	return hr;
}
