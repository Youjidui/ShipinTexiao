#include "StdAfx.h"
#include "SingleRevolvWipe.h"
#include <stdlib.h>
#include "../Utility/mathmacros.h"
#include "../Utility/SafeDelete.h"

#pragma warning(disable:4244)

CSingleRevolvWipe::CSingleRevolvWipe(void)
{
	m_pMesh = NULL;
}

CSingleRevolvWipe::~CSingleRevolvWipe(void)
{
}

HRESULT CSingleRevolvWipe::Init(CRenderEngine* pEngine)
{
	HRESULT hr = CWipeBase::Init(pEngine);
	ASSERT(SUCCEEDED(hr));
	ASSERT(m_pEngine);
	hr = InitMesh(pEngine);
	ASSERT(SUCCEEDED(hr));

	LPDIRECT3DDEVICE9 pDevice = pEngine->GetDevice();
	CResourceManager* pResMan = pEngine->GetResourceManager();
	m_pEffect = pResMan->CreateEffect(pDevice, _T("NewEffects/Revolv_Single_Mask.fx"));
	ASSERT(m_pEffect);

	return hr;
}

void CSingleRevolvWipe::Ready( CVideoBuffer* pMaskDef )
{
	CWipeBase::Ready(pMaskDef);
}

HRESULT CSingleRevolvWipe::Draw(CVideoBuffer* pMaskDef, BasicWipeFxParam* pParam)
{
	HRESULT hr = E_FAIL;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMan = m_pEngine->GetResourceManager();
	CVideoBufferManager* pVM = m_pEngine->GetVideoBufferManager();

	const VideoBufferInfo& biMask = pMaskDef->GetVideoBufferInfo();
	bool bOK = false;

	D3DXMATRIXA16 matPrevTrans[3],matRot,matTrans,matWorld[3],matType,matCombine[3];

	float fAxisOffset = (pParam->structPattern.fBorderWidth + pParam->structPattern.fSoftWidth) / 4.0f;
	float fBSWidth = fAxisOffset;
	fAxisOffset = fBSWidth * (pParam->structPattern.fOffset * 2.0f - 1.0f) ;	

	//fAxisOffset *= 1.0f;	
	D3DXMatrixTranslation(&matPrevTrans[0],fAxisOffset,0.0f,0.0f);

	fAxisOffset *= 2.0f;	
	D3DXMatrixTranslation(&matPrevTrans[1],fAxisOffset,0.0f,0.0f);
	D3DXMatrixIdentity(&matPrevTrans[2]);
	float s =  fabs(pParam->structPattern.fOffset * 2.0f - 1.0f);
	matPrevTrans[2]._41 = LERP(s,matPrevTrans[1]._41,matPrevTrans[0]._41);

	D3DXMatrixRotationZ(&matRot,- pParam->structPattern.fOffset * D3DX_PI * 2.0f);

	//if(pMaskDef->OffsetY_Other == 1 && pMaskDef->bIsCGBuffer)
	//	D3DXMatrixTranslation(&matTrans,pParam->structPattern.fCenterX,pParam->structPattern.fCenterY + 0.5f / pMaskDef->GetImageHeight() * (pParam->bOdd?1.0f:0.0f),0.0f);
	//else
	//	D3DXMatrixTranslation(&matTrans,pParam->structPattern.fCenterX,pParam->structPattern.fCenterY + 0.5f / pMaskDef->GetImageHeight() * (pParam->bOdd?0.0f:1.0f),0.0f);
	const bool pParam_bOdd = false;
	D3DXMatrixTranslation(&matTrans,pParam->structPattern.fCenterX,pParam->structPattern.fCenterY + 0.5f / biMask.nHeight * (pParam_bOdd?0.0f:1.0f),0.0f);
	matTrans._42 /= m_fAspect;

	//matPrevTrans[2]._41 = matPrevTrans[0]._41;

	D3DXMatrixIdentity(&matType);	

	switch(pParam->structPattern.nPattern)
	{
	case 1:D3DXMatrixRotationZ(&matType,D3DXToRadian(-90));break;
	case 2:D3DXMatrixRotationZ(&matType,D3DXToRadian(-180));break;
	case 3:D3DXMatrixRotationZ(&matType,D3DXToRadian(90));break;
	}
	matWorld[2] =   matRot * matPrevTrans[0] *   matType * matTrans;
	matCombine[2] = matWorld[2] * m_matView * m_matAspectProj;	

	matWorld[1] =   matRot * matPrevTrans[1] * matType * matTrans;
	matCombine[1] = matWorld[1] * m_matView * m_matAspectProj;	
	matWorld[0] =  matType * matTrans;
	matCombine[0] = matWorld[0] * m_matView * m_matAspectProj;	

	hr = m_pEffect->SetTechnique("Mask");
	ASSERT(SUCCEEDED(hr));

	hr = m_pEffect->SetMatrix("g_matWorldViewProjFix",&(matPrevTrans[0] * matCombine[0]));
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetMatrix("g_matWorldViewProj",&matCombine[2]);
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetMatrix("g_matWorldViewProjCenter",&(matPrevTrans[0] * matCombine[0]));
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetFloat("g_fBorderWidth",pParam->structPattern.fBorderWidth / 4.0f);
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetFloat("g_fSoftWidth",pParam->structPattern.fSoftWidth / 4.0f);
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetFloat("g_fOffset",pParam->structPattern.fOffset);
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

	CVideoBuffer* pTempDef[2] = {NULL,NULL};
	if(bDrawSoft)
	{
		VideoBufferInfo biTemp = {D3DFMT_A8R8G8B8, VideoBufferInfo::VIDEO_MEM, VideoBufferInfo::_IN_OUT, biMask.nWidth, biMask.nHeight, 0, 0};
		for(int i = 0; i< 2;i++)
		{
			//hTemp[i] = pParentRender->NewRTBuffer(0,0,pMaskDef->GetImageWidth() * (pMaskDef->IsYUV16Buffer() ? 2.0f:1.0f),pMaskDef->GetImageHeight(),pMaskDef->bIsCGBuffer);
			//assert(hTemp[i] > -1);
			//pTempDef[i] = m_pResMan->GetBufferDef(hTemp[i]);
			pTempDef[i] = pVM->CreateVideoBuffer(biTemp);
			ASSERT(pTempDef[i]);
		}
		aPass[0] = FALSE;
	}
	else
	{
		bool bOK = m_pEngine->SetRenderTarget(pMaskDef);
		ASSERT(bOK);
	}

	//m_pDevice->Clear(0,NULL,D3DCLEAR_STENCIL|D3DCLEAR_ZBUFFER,0x0,1.0f,0.0f);   

	float fOffsetX = bDrawSoft ?  (0.1f + 0.9f * pParam->structPattern.fSoftWidth / 5.0f): 0.0f;
	D3DXPLANE planebasic;
	UINT cPass , uPass;	
	for(int i = 0;i < 2; i ++)
	{
		hr = m_pEffect->Begin(&cPass,0);
		ASSERT(SUCCEEDED(hr));
		if(bDrawSoft)
		{
			bool bOK = m_pEngine->SetRenderTarget(pTempDef[i]);
			ASSERT(bOK);
		}

		for(uPass = 0;uPass < cPass; uPass ++)
		{
			if(!aPass[uPass])
				continue;			
			hr = m_pEffect->BeginPass(uPass);
			ASSERT(SUCCEEDED(hr));
			if(uPass == 0)
			{
				if(pParam->structPattern.fOffset < 0.125f)
					bOK = m_pMesh->DrawSubset(0);	
				else if(pParam->structPattern.fOffset < 0.375f)
					bOK = m_pMesh->DrawSubset(1);	
				else if(pParam->structPattern.fOffset < 0.625f)
					bOK = m_pMesh->DrawSubset(2);	
				else if(pParam->structPattern.fOffset < 0.875f)
					bOK = m_pMesh->DrawSubset(3);	
				else
					bOK = m_pMesh->DrawSubset(4);
				ASSERT(bOK);
			}
			else if( i == 0)
			{			
				//Active
				float fSign = pParam->structPattern.fOffset <= 0.5f ? -1.0f : 1.0f;

				D3DXMATRIXA16 matMove;
				D3DXMatrixTranslation(&matMove,fSign * (fBSWidth - fOffsetX / biMask.nWidth) ,0.0f,0.0f);
				D3DXPLANE plane(-fSign ,0.0f,0.0f,0.0f);

				D3DXMATRIXA16 matClip;
				matClip = matMove * matCombine[0];
				D3DXMatrixInverse(&matClip,NULL,&matClip);
				D3DXMatrixTranspose(&matClip,&matClip);
				D3DXPlaneTransform(&plane,&plane,&matClip);
				hr = pDevice->SetClipPlane(0,(float*)&plane);
				ASSERT(SUCCEEDED(hr));
				planebasic = plane;

				float bInvert = 1.0f;
				hr = m_pEffect->SetFloat("g_bInvert",bInvert);
				ASSERT(SUCCEEDED(hr));
				hr = m_pEffect->SetMatrix("g_matWorldViewProj",&matCombine[1]);
				ASSERT(SUCCEEDED(hr));
				hr = m_pEffect->SetMatrix("g_matWorld",&matWorld[1]);
				ASSERT(SUCCEEDED(hr));
				hr = m_pEffect->CommitChanges();
				ASSERT(SUCCEEDED(hr));
				hr = pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,D3DCLIPPLANE0);
				ASSERT(SUCCEEDED(hr));
				bOK = m_pMesh->DrawSubset(5);
				ASSERT(bOK);
			}
			else //if( i==1 )
			{
				//Fix
				float fSign = pParam->structPattern.fOffset <= 0.5f ? 1.0f : -1.0f;

				D3DXMATRIXA16 matMove;
				D3DXMatrixTranslation(&matMove,fSign * (fBSWidth - fOffsetX / biMask.nWidth) ,0.0f,0.0f);
				D3DXPLANE plane(-fSign ,0.0f,0.0f,0.0f);

				D3DXMATRIXA16 matClip;
				matClip = matMove * matCombine[1];
				D3DXMatrixInverse(&matClip,NULL,&matClip);
				D3DXMatrixTranspose(&matClip,&matClip);
				D3DXPlaneTransform(&plane,&plane,&matClip);
				hr = pDevice->SetClipPlane(0,(float*)&plane);
				ASSERT(SUCCEEDED(hr));

				float bInvert = 0.0f;
				hr = m_pEffect->SetFloat("g_bInvert",bInvert);
				ASSERT(SUCCEEDED(hr));
				hr = m_pEffect->SetMatrix("g_matWorldViewProj",&matCombine[0]);
				ASSERT(SUCCEEDED(hr));
				hr = m_pEffect->SetMatrix("g_matWorld",&matWorld[0]);
				ASSERT(SUCCEEDED(hr));
				hr = m_pEffect->CommitChanges();
				ASSERT(SUCCEEDED(hr));
				hr = pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,D3DCLIPPLANE0);
				ASSERT(SUCCEEDED(hr));
				bOK = m_pMesh->DrawSubset(5);
				ASSERT(bOK);
			}

			hr = m_pEffect->EndPass();
			ASSERT(SUCCEEDED(hr));

			//debug output
			//TCHAR szFilename[MAX_PATH];
			//_stprintf(szFilename, _T("SingleRevolvWipe_i=%d_pass=%d.dds"), i, uPass);
			//CVideoBuffer* pBuf = pMaskDef;
			//if(bDrawSoft)
			//	pBuf = (pTempDef[i]);
			//D3DXSaveSurfaceToFile(szFilename, D3DXIFF_DDS, pBuf->GetSurface(), NULL, NULL);
		}
		aPass[0] = FALSE;
		hr = m_pEffect->End();
		ASSERT(SUCCEEDED(hr));
	}

	//     	m_pResMan->DumpResourceToFile(hTemp[0],L"c:\\mask0.bmp",false,false,D3DXIFF_BMP);	
	//        m_pResMan->DumpResourceToFile(hTemp[1],L"c:\\mask1.bmp",false,false,D3DXIFF_BMP);	
	if(bDrawSoft)
	{
		bOK = m_pEngine->SetRenderTarget(pMaskDef);
		ASSERT(bOK);
		//Draw Basic
		hr = m_pEffect->SetMatrix("g_matWorldViewProjFix",&(matPrevTrans[0] * matCombine[0]));
		ASSERT(SUCCEEDED(hr));
		hr = m_pEffect->SetMatrix("g_matWorldViewProj",&matCombine[2]);
		ASSERT(SUCCEEDED(hr));
		hr = m_pEffect->SetMatrix("g_matWorldViewProjCenter",&(matPrevTrans[0] * matCombine[0]));
		ASSERT(SUCCEEDED(hr));

		if(pParam->structPattern.fOffset <= 0.125f)
		{
			hr = pDevice->SetClipPlane(0,(float*)&planebasic);
			ASSERT(SUCCEEDED(hr));
			hr = pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,D3DCLIPPLANE0);
			ASSERT(SUCCEEDED(hr));
		}
		else
		{
			hr = pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,0);
			ASSERT(SUCCEEDED(hr));
		}

		hr = m_pEffect->Begin(&cPass,0);
		ASSERT(SUCCEEDED(hr));
		hr = m_pEffect->BeginPass(0);
		ASSERT(SUCCEEDED(hr));

		if(pParam->structPattern.fOffset < 0.125f)
			bOK = m_pMesh->DrawSubset(0);	
		else if(pParam->structPattern.fOffset < 0.375f)
			bOK = m_pMesh->DrawSubset(1);	
		else if(pParam->structPattern.fOffset < 0.625f)
			bOK = m_pMesh->DrawSubset(2);	
		else if(pParam->structPattern.fOffset < 0.875f)
			bOK = m_pMesh->DrawSubset(3);
		else
			bOK = m_pMesh->DrawSubset(4);
		ASSERT(bOK);
		hr = m_pEffect->EndPass();
		ASSERT(SUCCEEDED(hr));
		hr =  m_pEffect->End();
		ASSERT(SUCCEEDED(hr));
		//  Compose			
		D3DXMATRIXA16 matMask,matCombine;
		matCombine = m_matView * m_matProj;

		hr = m_pEffect->SetTechnique("Compose");
		ASSERT(SUCCEEDED(hr));
		hr = m_pEffect->SetMatrix("g_matWorldViewProj",&matCombine);	
		ASSERT(SUCCEEDED(hr));
		UINT cPass,uPass = 0;
		if(pParam->structPattern.fOffset > 0.5f)
			uPass = 1;
		hr = m_pEffect->Begin(&cPass,0);
		ASSERT(SUCCEEDED(hr));
		hr = m_pEffect->BeginPass(uPass);
		ASSERT(SUCCEEDED(hr));
		for(int i = 0; i < 2;i ++)
		{
			bOK = ::GenerateMatrix(pTempDef[i],&matMask,mat_Image);
			ASSERT(bOK);
			hr = m_pEffect->SetMatrix("g_matMask",&matMask);
			ASSERT(SUCCEEDED(hr));
			hr = m_pEffect->SetTexture("g_txMask",pTempDef[i]->GetTexture());				
			ASSERT(SUCCEEDED(hr));
			hr = m_pEffect->CommitChanges();
			ASSERT(SUCCEEDED(hr));
			bOK = m_pQuadMesh->DrawMeshFx();
			ASSERT(bOK);
		}
		hr = m_pEffect->EndPass();
		ASSERT(SUCCEEDED(hr));
		hr = m_pEffect->End();
		ASSERT(SUCCEEDED(hr));
	}
	hr = pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,0);
	ASSERT(SUCCEEDED(hr));

	if(pTempDef[0])
	{
		pVM->ReleaseVideoBuffer(pTempDef[0]);
		pTempDef[0] = NULL;
	}
	if(pTempDef[1])
	{
		pVM->ReleaseVideoBuffer(pTempDef[1]);
		pTempDef[1] = NULL;
	}


	ASSERT(SUCCEEDED(hr));
	return hr;
}

HRESULT CSingleRevolvWipe::InitMesh(CRenderEngine* pEngine) 
{
	HRESULT hr = E_FAIL;
	LPCTSTR pszMeshName = _T("SingleRevolvWipeMesh");
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();

	m_pMesh = pResMgr->FindMesh(pszMeshName);
	if(!m_pMesh)
	{
		LPD3DXMESH pMesh = NULL;

		HRESULT hr = S_OK;
		int nNumVertex = 11;
		int nNumFace =  17;
		// Create the vertex buffer.
		if(FAILED(hr=D3DXCreateMesh(nNumFace,nNumVertex,TPXMESH_POOL  | D3DXMESH_WRITEONLY  ,declWipe,pDevice,&pMesh)))
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

		pVertices[0] = D3DXVECTOR3(0.0f,0.0f,1.0f);
		pVertices[1] = D3DXVECTOR3(0.0f,10.0f,0.0f);
		pVertices[2] = D3DXVECTOR3(10.0f,10.0f,0.0f);
		pVertices[3] = D3DXVECTOR3(10.0f,-10.0f,0.0f);	
		pVertices[4] = D3DXVECTOR3(-10.0f,-10.0f,0.0f);
		pVertices[5] = D3DXVECTOR3(-10.0f,10.0f,0.0f);
		pVertices[6] = D3DXVECTOR3(0.0f,10.0f,2.0f); //Rotating Point

		pVertices[7] = D3DXVECTOR3(0.0f,-10.0f,0.0f);		
		pVertices[8] = D3DXVECTOR3(0.0f,10.0f,0.0f);	
		pVertices[9] = D3DXVECTOR3(0.0f,-10.0f,1.0f);		
		pVertices[10] = D3DXVECTOR3(0.0f,10.0f,1.0f);	

		int nIndex = 0,nFace = 0;
		//Subset 0 
		pIB[nIndex ++] = 0;
		pIB[nIndex ++] = 1;
		pIB[nIndex ++] = 6;

		pAtt[nFace ++] = 0;
		//Subset 1
		pIB[nIndex ++] = 0;
		pIB[nIndex ++] = 1;
		pIB[nIndex ++] = 2;

		pIB[nIndex ++] = 0;
		pIB[nIndex ++] = 2;
		pIB[nIndex ++] = 6;

		pAtt[nFace ++] = 1;
		pAtt[nFace ++] = 1;
		//Subset 2
		pIB[nIndex ++] = 0;
		pIB[nIndex ++] = 1;
		pIB[nIndex ++] = 2;

		pIB[nIndex ++] = 0;
		pIB[nIndex ++] = 2;
		pIB[nIndex ++] = 3;

		pIB[nIndex ++] = 0;
		pIB[nIndex ++] = 3;
		pIB[nIndex ++] = 6;

		pAtt[nFace ++] = 2;
		pAtt[nFace ++] = 2;
		pAtt[nFace ++] = 2;
		//Subset 3
		pIB[nIndex ++] = 0;
		pIB[nIndex ++] = 1;
		pIB[nIndex ++] = 2;

		pIB[nIndex ++] = 0;
		pIB[nIndex ++] = 2;
		pIB[nIndex ++] = 3;

		pIB[nIndex ++] = 0;
		pIB[nIndex ++] = 3;
		pIB[nIndex ++] = 4;

		pIB[nIndex ++] = 0;
		pIB[nIndex ++] = 4;
		pIB[nIndex ++] = 6;

		pAtt[nFace ++] = 3;
		pAtt[nFace ++] = 3;
		pAtt[nFace ++] = 3;
		pAtt[nFace ++] = 3;
		//Subset 4
		pIB[nIndex ++] = 0;
		pIB[nIndex ++] = 1;
		pIB[nIndex ++] = 2;

		pIB[nIndex ++] = 0;
		pIB[nIndex ++] = 2;
		pIB[nIndex ++] = 3;

		pIB[nIndex ++] = 0;
		pIB[nIndex ++] = 3;
		pIB[nIndex ++] = 4;

		pIB[nIndex ++] = 0;
		pIB[nIndex ++] = 4;
		pIB[nIndex ++] = 5;

		pIB[nIndex ++] = 0;
		pIB[nIndex ++] = 5;
		pIB[nIndex ++] = 6;

		pAtt[nFace ++] = 4;
		pAtt[nFace ++] = 4;
		pAtt[nFace ++] = 4;
		pAtt[nFace ++] = 4;
		pAtt[nFace ++] = 4;
		// Subset 5
		pIB[nIndex ++] = 8;
		pIB[nIndex ++] = 9;
		pIB[nIndex ++] = 7;

		pIB[nIndex ++] = 8;
		pIB[nIndex ++] = 10;
		pIB[nIndex ++] = 9;

		pAtt[nFace ++] = 5;
		pAtt[nFace ++] = 5;

		pMesh->UnlockVertexBuffer();
		pMesh->UnlockIndexBuffer();
		pMesh->UnlockAttributeBuffer();

		m_pMesh = pResMgr->CreateMesh(pDevice, pMesh, pszMeshName);
		ASSERT(m_pMesh);
	}

	return S_OK;
}

