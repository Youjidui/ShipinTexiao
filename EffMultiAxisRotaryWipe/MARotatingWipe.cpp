#include "StdAfx.h"
#include "MARotatingWipe.h"
#include <stdlib.h>
#include "../Utility/mathmacros.h"
#include "../Utility/SafeDelete.h"

#pragma warning(disable:4244)

CMARotatingWipe::CMARotatingWipe(void)
{
	m_pMesh = NULL;
}

CMARotatingWipe::~CMARotatingWipe(void)
{
}

HRESULT CMARotatingWipe::Init(CRenderEngine* pEngine)
{
	HRESULT hr = CWipeBase::Init(pEngine);
	ASSERT(SUCCEEDED(hr));
	ASSERT(m_pEngine);
	hr = InitMesh(pEngine);
	ASSERT(SUCCEEDED(hr));

	LPDIRECT3DDEVICE9 pDevice = pEngine->GetDevice();
	CResourceManager* pResMan = pEngine->GetResourceManager();
	m_pEffect = pResMan->CreateEffect(pDevice, _T("NewEffects/GMARW_Rotating_Mask.fx"));
	ASSERT(m_pEffect);

	return hr;
}

void CMARotatingWipe::Ready( CVideoBuffer* pMaskDef )
{
	CWipeBase::Ready(pMaskDef);
}

HRESULT CMARotatingWipe::Draw(CVideoBuffer* pMaskDef, BasicWipeFxParam* pParam)
{
	HRESULT hr = E_FAIL;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMan = m_pEngine->GetResourceManager();

	bool bOK = m_pEngine->SetRenderTarget(pMaskDef);
	ASSERT(bOK);
	//hr = pDevice->ColorFill(pMaskDef->GetSurface(), NULL, 0xffffffff);
	hr = pDevice->ColorFill(pMaskDef->GetSurface(), NULL, 0xff000000);
	//hr = pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0xff,0xff,0xff,0xff), 0, 0);
	ASSERT(SUCCEEDED(hr));

	const VideoBufferInfo& biMask = pMaskDef->GetVideoBufferInfo();

	D3DXMATRIXA16 matPrevTrans,matAxisTrans,matRot,matAxisInvTrans,matTrans,matMirror,matWorld,matCombine,matMirrorCombine,matType,matShift,matAdd;

	float fAxisOffset = (pParam->structPattern.fBorderWidth + pParam->structPattern.fSoftWidth) / 4.0f + 1.0f  / biMask.nHeight;
	fAxisOffset *= pParam->structPattern.fOffset * 2.0f - 1.0f;		//[-1, 1]

	D3DXMatrixTranslation(&matPrevTrans,fAxisOffset,0.0f,0.0f);
	
	D3DXMatrixRotationZ(&matRot,pParam->structPattern.fOffset * D3DX_PI / 2.0f);
	
	const bool pParam_bOdd = false;
	//if(pMaskDef->OffsetY_Other == 1 && pMaskDef->bIsCGBuffer)
	//	D3DXMatrixTranslation(&matTrans,pParam->structPattern.fCenterX,pParam->structPattern.fCenterY + 0.5f  / pMaskDef->GetImageHeight() * (pParam->bOdd?1.0f:0.0f),0.0f);
	//else
		D3DXMatrixTranslation(&matTrans,pParam->structPattern.fCenterX,pParam->structPattern.fCenterY + 0.5f / biMask.nHeight * (pParam_bOdd?0.0f:1.0f),0.0f);
	matTrans._42 /= m_fAspect;

	D3DXMatrixIdentity(&matMirror);
	D3DXMatrixIdentity(&matType);
	D3DXMatrixIdentity(&matShift);
    D3DXMatrixIdentity(&matAdd);

	/*/////////////////////////////////////////////////////////////////////////
	0 1           1       1     2   1111111     3        1
	  1 1               1 1          11111              111
	  1 1 1			  1 1 1           111              11111
	  1 1               1 1            1              1111111
	  1                   1
	*////////////////////////////////////////////////////////////////////////
//由于rcimage 是作闭右开，所以右 底 要移动一个像素
	D3DXVECTOR2 vCenter,vNewCenter,vMirror;
	RECT rcScissort,rcMirrorScissort;
	switch(pParam->structPattern.nPattern)
	{
	case 0:
		{
			//float m = biMask.nHeight * CLAMP(0.5f - pParam->structPattern.fCenterY,0.0f,1.0f);
			float m = biMask.nHeight * (0.5f - pParam->structPattern.fCenterY/4);
		SetRect(&rcScissort,0,0, biMask.nWidth, m);
		SetRect(&rcMirrorScissort,0, m, biMask.nWidth, biMask.nHeight);
		vCenter = D3DXVECTOR2(-0.5f,0.5f);
		vMirror = D3DXVECTOR2(1.0f,-1.0f);
        matAdd._42 = 1.0f / m_fAspect / biMask.nHeight;
		}
		break;
	case 1:
		{
			//float m = biMask.nHeight * CLAMP(0.5f - pParam->structPattern.fCenterY,0.0f,1.0f);
			float m = biMask.nHeight * (0.5f - pParam->structPattern.fCenterY/4);
		SetRect(&rcMirrorScissort,0,0, biMask.nWidth, m );
		SetRect(&rcScissort,0, m, biMask.nWidth, biMask.nHeight);
		vCenter = D3DXVECTOR2(0.5f,-0.5f);
		vMirror = D3DXVECTOR2(1.0f,-1.0f);
		D3DXMatrixRotationZ(&matType,D3DX_PI );
        matAdd._42 = 1.0f / m_fAspect / biMask.nHeight;
		}
		break;	
	case 2:
		{
			//float m = biMask.nWidth * CLAMP(0.5f + pParam->structPattern.fCenterX,0.0f,1.0f);
			float m = biMask.nWidth * (0.5f + pParam->structPattern.fCenterX/4);
		SetRect(&rcMirrorScissort,0,0, m, biMask.nHeight);
		SetRect(&rcScissort, m,0, biMask.nWidth, biMask.nHeight);
		vCenter = D3DXVECTOR2(0.5f,0.5f);		
		vMirror = D3DXVECTOR2(-1.0f,1.0f);
		D3DXMatrixRotationZ(&matType, - D3DX_PI /2.0f );
        matAdd._41 = - 1.0f / biMask.nWidth;
		}
		break;
	case 3:
		{
			//float m = biMask.nWidth * CLAMP(0.5f + pParam->structPattern.fCenterX,0.0f,1.0f);
			float m = biMask.nWidth * (0.5f + pParam->structPattern.fCenterX/4);
		SetRect(&rcScissort,0,0, m, biMask.nHeight);
		SetRect(&rcMirrorScissort, m,0, biMask.nWidth, biMask.nHeight);
		vCenter = D3DXVECTOR2(-0.5f,-0.5f);
		vMirror = D3DXVECTOR2(-1.0f,1.0f);
		D3DXMatrixRotationZ(&matType, D3DX_PI /2.0f );
        matAdd._41 -= 1.0f / biMask.nWidth;
		}
		break;
	}
	//vNewCenter = D3DXVECTOR2(vCenter.x,ceil(vCenter.y / m_fAspect * biMask.nHeight) / (float)biMask.nHeight);
	vNewCenter = D3DXVECTOR2(vCenter.x,vCenter.y );
	D3DXMatrixTranslation(&matAxisTrans,-vNewCenter.x,-vNewCenter.y,0.0f);
	D3DXMatrixTranslation(&matAxisInvTrans,vNewCenter.x,vNewCenter.y,0.0f);
	D3DXMatrixTranslation(&matShift,vNewCenter.x - vCenter.x,vNewCenter.y - vCenter.y,0.0f);
    
	matWorld = matPrevTrans * matType * matShift * matAxisTrans * matRot * matAxisInvTrans * matMirror *  matTrans;
	matCombine = matWorld * m_matView * m_matProj;
    
	matMirror._11 *= vMirror.x;
	matMirror._22 *= vMirror.y;
	matWorld = matPrevTrans * matType * matShift * matAxisTrans * matRot * matAxisInvTrans * matMirror * matTrans * matAdd;
	matMirrorCombine = matWorld * m_matView * m_matProj;

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

		hr = pDevice->SetScissorRect(&rcScissort);
		ASSERT(SUCCEEDED(hr));
		hr = m_pEffect->SetMatrix("g_matWorldViewProj",&matCombine);
		ASSERT(SUCCEEDED(hr));
		hr = m_pEffect->BeginPass(uPass);
		ASSERT(SUCCEEDED(hr));
		bOK = m_pMesh->DrawSubset(CLAMP(uPass,0,1));
		ASSERT(bOK);

		//Mirror
		hr = pDevice->SetScissorRect(&rcMirrorScissort);
		ASSERT(SUCCEEDED(hr));
		hr = m_pEffect->SetMatrix("g_matWorldViewProj",&matMirrorCombine);
		ASSERT(SUCCEEDED(hr));
		hr = m_pEffect->CommitChanges();
		ASSERT(SUCCEEDED(hr));
		bOK = m_pMesh->DrawSubset(CLAMP(uPass,0,1));
		ASSERT(bOK);

		hr = m_pEffect->EndPass();
		ASSERT(SUCCEEDED(hr));
	}
	hr = m_pEffect->End();
	ASSERT(SUCCEEDED(hr));
	return hr;
}

HRESULT CMARotatingWipe::InitMesh(CRenderEngine* pEngine) 
{
	HRESULT hr = E_FAIL;
	LPCTSTR pszMeshName = _T("MARotatingWipeMesh");
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
		if(FAILED(hr=D3DXCreateMesh(nNumFace,nNumVertex,TPXMESH_POOL  | D3DXMESH_WRITEONLY  ,declWipe, pDevice, &pMesh)))
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

		pVertices[0] = D3DXVECTOR3(-0.5f,-10.0f,0.0f);
		pVertices[1] = D3DXVECTOR3(-10.5f,0.5f,0.0f);
		pVertices[2] = D3DXVECTOR3(-0.5f,10.5f,0.0f);	

		pVertices[3] = D3DXVECTOR3(-0.5f,-10.0f,1.0f);		
		pVertices[4] = D3DXVECTOR3(-0.5f,10.5f,1.0f);	

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

