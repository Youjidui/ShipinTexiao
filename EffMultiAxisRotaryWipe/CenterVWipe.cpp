#include "StdAfx.h"
#include "CenterVWipe.h"
#include <stdlib.h>
#include "../Utility/mathmacros.h"
#include "../Utility/SafeDelete.h"

#pragma warning(disable:4244)

CCenterVWipe::CCenterVWipe(void)
{
	m_pMesh = NULL;
}

CCenterVWipe::~CCenterVWipe(void)
{
}

HRESULT CCenterVWipe::Init(CRenderEngine* pEngine)
{
	HRESULT hr = CWipeBase::Init(pEngine);
	ASSERT(SUCCEEDED(hr));
	ASSERT(m_pEngine);
	hr = InitMesh(pEngine);
	ASSERT(SUCCEEDED(hr));

	LPDIRECT3DDEVICE9 pDevice = pEngine->GetDevice();
	CResourceManager* pResMan = pEngine->GetResourceManager();
	m_pEffect = pResMan->CreateEffect(pDevice, _T("NewEffects/GMARW_CenterV_Mask.fx"));
	ASSERT(m_pEffect);

	return hr;
}

void CCenterVWipe::Ready( CVideoBuffer* pMaskDef )
{
	CWipeBase::Ready(pMaskDef);
}

HRESULT CCenterVWipe::Draw(CVideoBuffer* pMaskDef, BasicWipeFxParam* pParam)
{
	HRESULT hr = E_FAIL;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMan = m_pEngine->GetResourceManager();

	bool bOK = m_pEngine->SetRenderTarget(pMaskDef);
	ASSERT(bOK);

	const VideoBufferInfo& biMask = pMaskDef->GetVideoBufferInfo();

	D3DXMATRIXA16 matPrevTrans,matRot,matTrans,matMirror,matWorld,matCombine[4];

	float fAxisOffset = (pParam->structPattern.fBorderWidth + pParam->structPattern.fSoftWidth) / 4.0f  + 1.0f  / biMask.nHeight;
	fAxisOffset *= pParam->structPattern.fOffset * 2.0f - 1.0f;
	fAxisOffset *= 1.01f;
	D3DXMatrixTranslation(&matPrevTrans,-fAxisOffset,0.0f,0.0f);

	D3DXMatrixRotationZ(&matRot,pParam->structPattern.fOffset * D3DX_PI / 2.0f);

	const bool pParam_bOdd = false;
	//if(pMaskDef->OffsetY_Other == 1 && pMaskDef->bIsCGBuffer)
	//	D3DXMatrixTranslation(&matTrans,pParam->structPattern.fCenterX,pParam->structPattern.fCenterY + 0.5f / pMaskDef->GetImageHeight() * (pParam->bOdd?0.0f:1.0f),0.0f);
	//else
		D3DXMatrixTranslation(&matTrans,pParam->structPattern.fCenterX,pParam->structPattern.fCenterY + 0.5f / biMask.nHeight * (pParam_bOdd?1.0f:0.0f),0.0f);
	matTrans._42 /= m_fAspect;
	D3DXMatrixIdentity(&matMirror);	

	float fWidthScale = CLAMP(0.5f + pParam->structPattern.fCenterX,0.0f,1.0f),fHeightScale = CLAMP(0.5f - pParam->structPattern.fCenterY,0.0f,1.0f);
	RECT rcScissort[4];
	SetRect(&rcScissort[0],0,0, biMask.nWidth * fWidthScale, biMask.nHeight * fHeightScale);
	SetRect(&rcScissort[1], biMask.nWidth * fWidthScale,0, biMask.nWidth, biMask.nHeight * fHeightScale);
	SetRect(&rcScissort[2],0, biMask.nHeight * fHeightScale, biMask.nWidth * fWidthScale, biMask.nHeight);
	SetRect(&rcScissort[3], biMask.nWidth * fWidthScale, biMask.nHeight * fHeightScale, biMask.nWidth, biMask.nHeight);

	matWorld = matPrevTrans * matRot * matMirror  * matTrans;
	matCombine[0] = matWorld * m_matView * m_matProj;

	matMirror._11 *= -1;
	matWorld = matPrevTrans * matRot * matMirror  * matTrans;
	matCombine[1] = matWorld * m_matView * m_matProj;

	matMirror._22 *= -1;
	matWorld = matPrevTrans * matRot * matMirror  * matTrans;
	matCombine[3] = matWorld * m_matView * m_matProj;

	matMirror._11 *= -1;	
	matWorld = matPrevTrans * matRot * matMirror  * matTrans;
	matCombine[2] = matWorld * m_matView * m_matProj;

	hr = m_pEffect->SetTechnique("Mask");
	ASSERT(SUCCEEDED(hr));

	hr = m_pEffect->SetFloat("g_fBorderWidth",pParam->structPattern.fBorderWidth / 4.0f);
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetFloat("g_fSoftWidth",pParam->structPattern.fSoftWidth / 4.0f);
	ASSERT(SUCCEEDED(hr));
	BOOL bDrawBorder = pParam->structPattern.fBorderWidth > 0.0f;
	BOOL bDrawSoft = pParam->structPattern.fSoftWidth > 0.0f;

	BOOL aPass[7] = {FALSE};
	aPass[0] = TRUE;
	if(bDrawBorder )
	{
		if(pParam->structPattern.fBorderWidth >= pParam->structPattern.fSoftWidth)
		{
			aPass[1] = TRUE;
			if( bDrawSoft)		
				aPass[3] = aPass[4] = TRUE;
		}
		else
		{
			aPass[3] = aPass[4] = aPass[5] = aPass[6] = TRUE;			
		}
	}			
	else if(bDrawSoft)
		aPass[2] = TRUE;

	UINT cPass;
	hr = m_pEffect->Begin(&cPass,0);
	ASSERT(SUCCEEDED(hr));
	for(UINT uPass = 0;uPass < cPass; uPass ++)
	{
		if(!aPass[uPass])
			continue;			
		hr = m_pEffect->BeginPass(uPass);
		ASSERT(SUCCEEDED(hr));

		for(int i = 0;i < 4;i ++)
		{
			hr = pDevice->SetScissorRect(&rcScissort[i]);
			ASSERT(SUCCEEDED(hr));
			hr = m_pEffect->SetMatrix("g_matWorldViewProj",&matCombine[i]);
			ASSERT(SUCCEEDED(hr));

			hr = m_pEffect->CommitChanges();
			ASSERT(SUCCEEDED(hr));
			hr = m_pMesh->DrawSubset(CLAMP(uPass,0,1));
			ASSERT(SUCCEEDED(hr));
		}
		hr = m_pEffect->EndPass();
		ASSERT(SUCCEEDED(hr));
	}
	hr = m_pEffect->End();
	ASSERT(SUCCEEDED(hr));
	return hr;
}

HRESULT CCenterVWipe::InitMesh(CRenderEngine* pEngine) 
{
	HRESULT hr = E_FAIL;
	LPCTSTR pszMeshName = _T("CenterVWipeMesh");
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();

	m_pMesh = pResMgr->FindMesh(pszMeshName);
	if(!m_pMesh)
	{
		LPD3DXMESH pMesh = NULL;

		HRESULT hr = S_OK;
		int nNumVertex = 5;
		int nNumFace =  3;
		// Create the vertex buffer.
		if(FAILED(hr=D3DXCreateMesh(nNumFace,nNumVertex,TPXMESH_POOL  | D3DXMESH_WRITEONLY  ,declWipe, pDevice,&pMesh)))
			return hr;
		D3DXVECTOR3* pVertices = NULL;
		WORD*			pIB=NULL;
		DWORD*			pAtt=NULL;
		if(FAILED(hr=pMesh->LockVertexBuffer(0,(LPVOID*)&pVertices)))
			return hr;
		if(FAILED(hr=pMesh->LockIndexBuffer(0,(LPVOID*)&pIB)))
			return hr;
		if(FAILED(hr=pMesh->LockAttributeBuffer(0,&pAtt)))
			return hr;

		ZeroMemory(pVertices,sizeof(D3DXVECTOR3) * nNumVertex);

		pVertices[0] = D3DXVECTOR3(0.0f,-10.0f,0.0f);
		pVertices[1] = D3DXVECTOR3(-10.0f,0.0f,0.0f);
		pVertices[2] = D3DXVECTOR3(0.0f,10.0f,0.0f);	

		pVertices[3] = D3DXVECTOR3(0.0f,-10.0f,1.0f);		
		pVertices[4] = D3DXVECTOR3(0.0f,10.0f,1.0f);	

		pIB[0] = 0;
		pIB[1] = 1;
		pIB[2] = 2;

		pAtt[0] = 0;
		//
		pIB[3] = 0;
		pIB[4] = 2;
		pIB[5] = 3;

		pAtt[1] = 1;

		pIB[6] = 2;
		pIB[7] = 4;
		pIB[8] = 3;

		pAtt[2] = 1;

		pMesh->UnlockVertexBuffer();
		pMesh->UnlockIndexBuffer();
		pMesh->UnlockAttributeBuffer();

		m_pMesh = pResMgr->CreateMesh(pDevice, pMesh, pszMeshName);
		ASSERT(m_pMesh);
	}

	return S_OK;
}

