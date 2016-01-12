#include "StdAfx.h"
#include "RotatingWipe.h"
#include <stdlib.h>
#include "../Utility/mathmacros.h"
#include "../Utility/SafeDelete.h"

#pragma warning(disable:4244)

CRotatingWipe::CRotatingWipe(void)
{
	m_pMesh = NULL;
}

CRotatingWipe::~CRotatingWipe(void)
{
}

HRESULT CRotatingWipe::Init(CRenderEngine* pEngine)
{
	HRESULT hr = CWipeBase::Init(pEngine);
	ASSERT(SUCCEEDED(hr));
	ASSERT(m_pEngine);
	hr = InitMesh(pEngine);
	ASSERT(SUCCEEDED(hr));

	LPDIRECT3DDEVICE9 pDevice = pEngine->GetDevice();
	CResourceManager* pResMan = pEngine->GetResourceManager();
	m_pEffect = pResMan->CreateEffect(pDevice, _T("NewEffects/Fan_Wipe_Mask.fx"));
	ASSERT(m_pEffect);

	return hr;
}

void CRotatingWipe::Ready( CVideoBuffer* pMaskDef )
{
	CWipeBase::Ready(pMaskDef);
}

HRESULT CRotatingWipe::Draw(CVideoBuffer* pMaskDef, BasicWipeFxParam* pParam)
{
	HRESULT hr = E_FAIL;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMan = m_pEngine->GetResourceManager();

	bool bOK = m_pEngine->SetRenderTarget(pMaskDef);
	ASSERT(bOK);

	D3DXMATRIXA16 matAxisTrans,matRot,matTrans,matWorld,matCombine,matType;

	float fAxisOffset = (pParam->structPattern.fBorderWidth + pParam->structPattern.fSoftWidth) / 4.0f;
	fAxisOffset *= pParam->structPattern.fOffset * 2.0f - 1.0f;	

	D3DXMatrixRotationZ(&matRot,pParam->structPattern.fOffset * D3DX_PI / 2.0f);

	const VideoBufferInfo& biMask = pMaskDef->GetVideoBufferInfo();
	bool pParam_bOdd = false;
	//if(pMaskDef->OffsetY_Other == 1 && pMaskDef->bIsCGBuffer)
	//	D3DXMatrixTranslation(&matTrans,pParam->structPattern.fCenterX,pParam->structPattern.fCenterY + 0.5f / pMaskDef->GetImageHeight() * (pParam->bOdd?1.0f:0.0f),0.0f);
	//else
		D3DXMatrixTranslation(&matTrans,pParam->structPattern.fCenterX,pParam->structPattern.fCenterY + 0.5f / biMask.nHeight * (pParam_bOdd?0.0f:1.0f),0.0f);

	matTrans._42 /= m_fAspect;

	D3DXMatrixIdentity(&matType);

	float xMax = 0.5f,yMax = 0.5f / m_fAspect;
	D3DXVECTOR2 vCenter(-xMax,-yMax);

	switch(pParam->structPattern.nPattern) { 
	case 0:
		vCenter = D3DXVECTOR2(-xMax,-yMax);
		D3DXMatrixRotationZ(&matRot,-D3DX_PI / 2.0f * pParam->structPattern.fOffset);
		break;
	case 1:
		D3DXMatrixRotationZ(&matType,-D3DX_PI / 2.0f);
		vCenter = D3DXVECTOR2(-xMax,yMax);
		D3DXMatrixRotationZ(&matRot,-D3DX_PI / 2.0f * pParam->structPattern.fOffset);
		break;
	case 2:
		D3DXMatrixRotationZ(&matType,-D3DX_PI / 2.0f);
		vCenter = D3DXVECTOR2(xMax,yMax);
		D3DXMatrixRotationZ(&matRot,D3DX_PI / 2.0f * pParam->structPattern.fOffset);
		break;
	case 3:
		D3DXMatrixScaling(&matType,-1.0f,1.0f,1.0f);
		vCenter = D3DXVECTOR2(xMax,-yMax);
		D3DXMatrixRotationZ(&matRot,D3DX_PI / 2.0f * pParam->structPattern.fOffset);
		break;
	case 4:
		vCenter = D3DXVECTOR2(-xMax,0.0f);
		D3DXMatrixRotationZ(&matRot,-D3DX_PI * pParam->structPattern.fOffset);
		break;
	case 5:
		D3DXMatrixScaling(&matType,-1.0f,1.0f,1.0f);
		vCenter = D3DXVECTOR2(xMax,0.0f);
		D3DXMatrixRotationZ(&matRot,D3DX_PI * pParam->structPattern.fOffset);
		break;
	case 6:
		D3DXMatrixRotationZ(&matType,-D3DX_PI / 2.0f);
		vCenter = D3DXVECTOR2(0,yMax);
		D3DXMatrixRotationZ(&matRot,D3DX_PI * pParam->structPattern.fOffset);
		break;
	case 7:
		D3DXMatrixRotationZ(&matType,D3DX_PI / 2.0f);
		vCenter = D3DXVECTOR2(0,-yMax);
		D3DXMatrixRotationZ(&matRot,-D3DX_PI * pParam->structPattern.fOffset);
		break;
	}  

	D3DXMatrixTranslation(&matAxisTrans,vCenter.x,vCenter.y,0.0f);	

	matWorld = matType * matRot * matAxisTrans * matTrans;
	LPD3DXMATRIX matView = NULL, matProj= NULL;
	pResMan->GetPerspectiveMatrix(&matView, &matProj);
	matCombine = matWorld * (*matView) * m_matAspectProj;//*matProj;

	hr = m_pEffect->SetTechnique("Mask");
	ASSERT(SUCCEEDED(hr));

	D3DXVECTOR4 vMisc(pParam->structPattern.fBorderWidth / 4.0f,pParam->structPattern.fSoftWidth / 4.0f,fAxisOffset,0.0f);
	hr = m_pEffect->SetVector("g_vMisc",&vMisc);
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

		hr = m_pEffect->SetMatrix("g_matWorldViewProj",&matCombine);
		ASSERT(SUCCEEDED(hr));
		hr = m_pEffect->BeginPass(uPass);
		ASSERT(SUCCEEDED(hr));
		hr = m_pMesh->DrawSubset(CLAMP(uPass,0,1));
		ASSERT(SUCCEEDED(hr));
		hr = m_pEffect->EndPass();
		ASSERT(SUCCEEDED(hr));
	}
	hr = m_pEffect->End();
	ASSERT(SUCCEEDED(hr));
	return hr;
}

HRESULT CRotatingWipe::InitMesh(CRenderEngine* pEngine) 
{
	HRESULT hr = E_FAIL;
	LPCTSTR pszMeshName = _T("RotatingMesh");
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();

	m_pMesh = pResMgr->FindMesh(pszMeshName);
	if(m_pMesh)
	{
		const D3DVERTEXELEMENT9 declWipe[] =
		{
			{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0}, 		
			D3DDECL_END()
		};

		LPD3DXMESH pMesh = NULL;
		HRESULT hr = S_OK;
		int nNumVertex = 5;
		int nNumFace =  3;
		// Create the vertex buffer.
		if(FAILED(hr=D3DXCreateMesh(nNumFace, nNumVertex, D3DXMESH_MANAGED | D3DXMESH_WRITEONLY, declWipe, pDevice, &pMesh)))
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
		pVertices[1] = D3DXVECTOR3(0.0f,10.0f,0.0f);
		pVertices[2] = D3DXVECTOR3(-10.0f,0.0f,0.0f);	

		pVertices[3] = D3DXVECTOR3(0.0f,-10.0f,1.0f);		
		pVertices[4] = D3DXVECTOR3(0.0f,10.0f,1.0f);	

		pIB[0] = 0;
		pIB[1] = 2;
		pIB[2] = 1;

		pAtt[0] = 0;
		//
		pIB[3] = 1;
		pIB[4] = 3;
		pIB[5] = 0;

		pAtt[1] = 1;

		pIB[6] = 1;
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
