#include "StdAfx.h"
#include "DiagonalWipe.h"
#include <stdlib.h>
#include "../Utility/mathmacros.h"
#include "../Utility/SafeDelete.h"

#pragma warning(disable:4244)

CDiagonalWipe::CDiagonalWipe(void)
{
	m_pMesh = NULL;
}

CDiagonalWipe::~CDiagonalWipe(void)
{
}

HRESULT CDiagonalWipe::Init(CRenderEngine* pEngine)
{
	HRESULT hr = CWipeBase::Init(pEngine);
	ASSERT(SUCCEEDED(hr));
	ASSERT(m_pEngine);
	hr = InitMesh(pEngine);
	ASSERT(SUCCEEDED(hr));

	LPDIRECT3DDEVICE9 pDevice = pEngine->GetDevice();
	CResourceManager* pResMan = pEngine->GetResourceManager();
	m_pEffect = pResMan->CreateEffect(pDevice, _T("NewEffects/GMARW_Diagonal_Mask.fx"));
	ASSERT(m_pEffect);

	return hr;
}

void CDiagonalWipe::Ready( CVideoBuffer* pMaskDef )
{
	CWipeBase::Ready(pMaskDef);
}

HRESULT CDiagonalWipe::Draw(CVideoBuffer* pMaskDef, BasicWipeFxParam* pParam)
{
	HRESULT hr = E_FAIL;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMan = m_pEngine->GetResourceManager();

	bool bOK = m_pEngine->SetRenderTarget(pMaskDef);
	ASSERT(bOK);

	D3DXMATRIXA16 matPrevTrans,matAxisTrans,matRot,matAxisInvTrans,matTrans,matMirror,
		matWorld,matMirrorWorld,matCombine,matMirrorCombine,matType,matShift,matAdd;

	const VideoBufferInfo& biMask = pMaskDef->GetVideoBufferInfo();
	float fAxisOffset = (pParam->structPattern.fBorderWidth + pParam->structPattern.fSoftWidth) / 4.0f  +  1.0f  / biMask.nHeight;
	fAxisOffset *= pParam->structPattern.fOffset * 2.0f - 1.0f;
	
	D3DXMatrixTranslation(&matPrevTrans,0.0f,-fAxisOffset,0.0f);


	D3DXMatrixRotationZ(&matRot,- pParam->structPattern.fOffset * D3DX_PI / 2.0f);

	const bool pParam_bOdd = false;
	//if(pMaskDef->OffsetY_Other == 1 && pMaskDef->bIsCGBuffer)
	//	D3DXMatrixTranslation(&matTrans,pParam->structPattern.fCenterX,pParam->structPattern.fCenterY + 0.5f / pMaskDef->GetImageHeight() * (pParam->bOdd?1.0f:0.0f),0.0f);
	//else
		D3DXMatrixTranslation(&matTrans,pParam->structPattern.fCenterX,pParam->structPattern.fCenterY + 0.5f / biMask.nHeight * (pParam_bOdd?0.0f:1.0f),0.0f);
	matTrans._42 /= m_fAspect;
	D3DXMatrixIdentity(&matMirror);
	D3DXMatrixIdentity(&matType);	
    D3DXMatrixIdentity(&matAdd);	
	
	/*/////////////////////////////////////////////////////////////////////////
	
	*////////////////////////////////////////////////////////////////////////	

	D3DXVECTOR2 vCenter,vNewCenter;
	switch(pParam->structPattern.nPattern - 4)
	{
	case 0:		
        matType._22 = -1;
        vCenter = D3DXVECTOR2(-0.5f,0.0f);
        D3DXMatrixRotationZ(&matRot,pParam->structPattern.fOffset * D3DX_PI / 2.0f);
        matAdd._41 = -1.0f  / biMask.nWidth;
		break;
	case 1:		
            vCenter = D3DXVECTOR2(-0.5f,0.0f);
            matAdd._41 = -1.0f  / biMask.nWidth;
		break;	
	case 2:			
		D3DXMatrixRotationZ(&matType, - D3DX_PI /2.0f );
		vCenter = D3DXVECTOR2(0.0f,0.5f);
        matAdd._42 = 1.0f / m_fAspect / biMask.nHeight;
		break;
	case 3:			
		{
			D3DXMatrixRotationZ(&matType, -D3DX_PI /2.0f );
			D3DXMATRIXA16 matsymmetry;
			D3DXMatrixIdentity(&matsymmetry);
			matsymmetry._22 = -1.0f;
			matType =  matType * matsymmetry  ;
			vCenter = D3DXVECTOR2(0.0f,-0.5f);
			D3DXMatrixRotationZ(&matRot,pParam->structPattern.fOffset * D3DX_PI / 2.0f);
            matAdd._42 = 1.0f / m_fAspect / biMask.nHeight;
		}
		break;
	}
	vNewCenter = D3DXVECTOR2(vCenter.x,ceil(vCenter.y / m_fAspect * biMask.nHeight) / (float)biMask.nHeight);
	D3DXMatrixTranslation(&matAxisTrans,-vNewCenter.x,-vNewCenter.y,0.0f);
	D3DXMatrixTranslation(&matAxisInvTrans,vNewCenter.x,vNewCenter.y,0.0f);
	D3DXMatrixTranslation(&matShift,vNewCenter.x - vCenter.x,vNewCenter.y - vCenter.y,0.0f);

	matWorld = matPrevTrans * matType * matAxisTrans * matRot * matAxisInvTrans * matMirror *  matTrans;
	matCombine = matWorld * m_matView * m_matProj;


	D3DXMatrixRotationZ(&matMirror,D3DX_PI);
	matMirrorWorld = matPrevTrans * matType * matAxisTrans * matRot * matAxisInvTrans * matMirror *  matTrans * matAdd;
	matMirrorCombine = matMirrorWorld * m_matView * m_matProj;

	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 0.5f, 1000.0f );

	D3DXPLANE vClipPlane= D3DXPLANE(0.0f,-1.0f,0.0f,0.0f);
	
	D3DXMATRIXA16 matClip;    
	matClip = matType * matRot * matTrans * m_matView * m_matProj;
    D3DXMatrixInverse(&matClip,NULL,&matClip);
    D3DXMatrixTranspose(&matClip,&matClip);
    D3DXPlaneTransform(&vClipPlane,&vClipPlane,&matClip);
    hr = pDevice->SetClipPlane(0,(float*)vClipPlane);
    ASSERT(SUCCEEDED(hr));
	
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

    hr = pDevice->Clear(0,NULL,D3DCLEAR_STENCIL,0x0,1.0f,0);
	ASSERT(SUCCEEDED(hr));

    hr = m_pEffect->SetMatrix("g_matWorldViewProj",&(m_matView * m_matProj));
	ASSERT(SUCCEEDED(hr));

	UINT cPass,uPass;
	hr = m_pEffect->Begin(&cPass,0);
	ASSERT(SUCCEEDED(hr));
    
    //Stencil
    hr = m_pEffect->BeginPass(7);
	ASSERT(SUCCEEDED(hr));
    bOK = m_pQuadMesh->DrawMeshFx();
	ASSERT(bOK);
    hr = m_pEffect->EndPass();
	ASSERT(SUCCEEDED(hr));
    
    D3DXVECTOR4 vStencil;
	for(uPass = 0;uPass < cPass; uPass ++)
	{
		if(!aPass[uPass])
			continue;
        
        vStencil = D3DXVECTOR4(D3DCMP_LESS,0,0.0f,0.0f);
		hr = m_pEffect->SetMatrix("g_matWorldViewProj",&matCombine);
		ASSERT(SUCCEEDED(hr));
        hr = m_pEffect->SetVector("g_vStencil",&vStencil);
		ASSERT(SUCCEEDED(hr));
		hr = m_pEffect->BeginPass(uPass);
		ASSERT(SUCCEEDED(hr));

		bOK = m_pMesh->DrawSubset(CLAMP(uPass,0,1));
		ASSERT(bOK);
        
		//Mirror	
        vStencil = D3DXVECTOR4(D3DCMP_GREATER,1,0.0f,0.0f);
		hr = m_pEffect->SetMatrix("g_matWorldViewProj",&matMirrorCombine);
		ASSERT(SUCCEEDED(hr));
        hr = m_pEffect->SetVector("g_vStencil",&vStencil);
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

HRESULT CDiagonalWipe::InitMesh(CRenderEngine* pEngine) 
{
	HRESULT hr = E_FAIL;
	LPCTSTR pszMeshName = _T("DiagonalWipeMesh");
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
		if(FAILED(hr=D3DXCreateMesh(nNumFace, nNumVertex, D3DXMESH_MANAGED | D3DXMESH_WRITEONLY  ,declWipe, pDevice, &pMesh)))
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

		pVertices[0] = D3DXVECTOR3(-10.5f,0.0f,0.0f);
		pVertices[1] = D3DXVECTOR3(9.5f,0.0f,0.0f);
		pVertices[2] = D3DXVECTOR3(-0.5f,-10.0f,0.0f);	

		pVertices[3] = D3DXVECTOR3(-10.5f,0.0f,1.0f);		
		pVertices[4] = D3DXVECTOR3(9.5f,0.0f,1.0f);		

		pIB[0] = 0;
		pIB[1] = 1;
		pIB[2] = 2;

		pAtt[0] = 0;
		//
		pIB[3] = 0;
		pIB[4] = 4;
		pIB[5] = 1;

		pAtt[1] = 1;

		pIB[6] = 3;
		pIB[7] = 4;
		pIB[8] = 0;

		pAtt[2] = 1;

		pMesh->UnlockVertexBuffer();
		pMesh->UnlockIndexBuffer();
		pMesh->UnlockAttributeBuffer();

		m_pMesh = pResMgr->CreateMesh(pDevice, pMesh, pszMeshName);
		ASSERT(m_pMesh);
	}

	return S_OK;
}
