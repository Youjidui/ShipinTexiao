#include "stdafx.h"
#include ".\BarmWipe.h"
#include "../D3D9Render/BaseFx.h"
#include "BarmWipeRender.h"


#define SIGN(x) ((x) > 0.0f ?  1.0f : ((x) < 0.0f ? - 1.0f : 0.0f)  )


CBarmWipe::CBarmWipe()
: m_pBarmMesh(NULL)
{

}

CBarmWipe::~CBarmWipe()
{

}
BOOL LineIntersection(D3DXVECTOR3 Line0[],D3DXVECTOR3 Line1[],D3DXVECTOR3& vPt)//1 钝角 2 平行
{
	/*已知四个点V1 V2，V3 V4，想临两点组成一条直线，求两条直线的交点
	*交点处的坐标必定都相等，利用插值公式
	*							y1+(y2-y1)s1=y4+(y3-y4)s2,
	*							x1+(x2-x1)s1=x4+(x3-x4)s2,
	*								a         b        c
	*							(y2-y1)s1-(y3-y4)s2+y1-y4=0;
	*								d          e        f
	*						   (x2-x1)s1-(x3-x4)s2+x1-x4=0;
	*		  b*f-c*e	
	*	s1=----------
	*	      a*e-b*d
	*	
	*		   b*f-c*e	
	*	s2=----------
	*	       a*e-b*d
	*/	
	float a=Line0[1].y-Line0[0].y;
	float b=Line1[0].y-Line1[1].y;
	float c=Line0[0].y-Line1[1].y;

	float d=Line0[1].x-Line0[0].x;
	float e=Line1[0].x-Line1[1].x;
	float f=Line0[0].x-Line1[1].x;

	if(fabs(a*e-b*d)>1e-6)
	{
		float s1=(b*f-c*e)/(a*e-b*d);
		float s2=(a*f-c*d)/(a*e-b*d);

		D3DXVec3Lerp(&vPt,&Line0[0],&Line0[1],s1);


		//return ((s1>=1&&s2>=1)||(s1<1&&s2<1&&s1>=0&&s2>=0));
		return TRUE;
	}
	else
	{
		vPt=Line0[1];
		//nRelation=LINE_PARALLEL;
		return TRUE;
	}
}

HRESULT CBarmWipe::Draw(CVideoBuffer* pMaskDef, void* pParamRaw)
{
	BarmWipeFxParam* pParam = (BarmWipeFxParam*)pParamRaw;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	CVideoBufferManager* pVM = m_pEngine->GetVideoBufferManager();
	const VideoBufferInfo& biMask = pMaskDef->GetVideoBufferInfo();

	HRESULT hr = E_FAIL;
	float fOffset = 1.0f;	

	switch(pParam->structPattern.nPattern) {
	case 0:
	case 1:
	case 2:
	case 3:		
		fOffset = CalcOffset(pParamRaw,pParam->structPattern.nPattern);
		hr = Draw(pMaskDef,pParamRaw, pParam->structPattern.nPattern,fOffset);
		ASSERT(SUCCEEDED(hr));
		break;
	case 4:
	case 5:
		{
			//handle_tpr hMask0 = ((GBarmWipeRender*)pParent)->NewRTBuffer(0,0,pMaskDef->GetImageWidth(),pMaskDef->GetImageHeight());
			//handle_tpr hMask1 = ((GBarmWipeRender*)pParent)->NewRTBuffer(0,0,pMaskDef->GetImageWidth(),pMaskDef->GetImageHeight());
			CVideoBuffer* pMask0 = pVM->CreateVideoBuffer(biMask);
			ASSERT(pMask0);
			CVideoBuffer* pMask1 = pVM->CreateVideoBuffer(biMask);
			ASSERT(pMask1);
			//TP_VBufferDef* pMaskDef0 = m_pResMan->GetBufferDef(hMask0);
			//TP_VBufferDef* pMaskDef1 = m_pResMan->GetBufferDef(hMask1);
			
			int nAdd = pParam->structPattern.nPattern == 4 ? 0 : 2;

			float fOff0 = CalcOffset(pParamRaw, 0 + nAdd);
			float fOff1 = CalcOffset(pParamRaw, 1 + nAdd);
			fOffset = min(fOff0, fOff1);
			hr = Draw(pMask0, pParamRaw, 0 + nAdd, fOffset);
			ASSERT(SUCCEEDED(hr));
			hr = Draw(pMask1, pParamRaw, 1 + nAdd, fOffset);
			ASSERT(SUCCEEDED(hr));

			bool bOK = m_pEngine->SetRenderTarget(pMaskDef);
			ASSERT(bOK);

			hr = m_pEffect->SetTechnique("Compose");
			ASSERT(SUCCEEDED(hr));
			D3DXMATRIXA16 matCombine,matMask;
			LPD3DXMATRIX matView = NULL, matProj= NULL;
			pResMgr->GetPerspectiveMatrix(&matView, &matProj);
			matCombine = *matView * *matProj;
			hr = m_pEffect->SetMatrix("g_matWorldViewProj",&matCombine);
			ASSERT(SUCCEEDED(hr));

			::GenerateMatrix(pMask0,&matMask,mat_Image);
			hr = m_pEffect->SetMatrix("g_matMask",&matMask);
			ASSERT(SUCCEEDED(hr));

			hr = m_pEffect->SetTexture("g_txMask0",pMask0->GetTexture());
			ASSERT(SUCCEEDED(hr));
			hr = m_pEffect->SetTexture("g_txMask1",pMask1->GetTexture());
			ASSERT(SUCCEEDED(hr));

			UINT cPass;
			hr = m_pEffect->Begin(&cPass,0);
			ASSERT(SUCCEEDED(hr));
			hr = m_pEffect->BeginPass(0);
			ASSERT(SUCCEEDED(hr));
			bOK = m_pQuadMesh->DrawMeshFx();
			ASSERT(bOK);
			hr = m_pEffect->EndPass();
			ASSERT(SUCCEEDED(hr));
			hr = m_pEffect->End();
			ASSERT(SUCCEEDED(hr));

			//D3DXSaveSurfaceToFile(_T("./BarmWipe_Mask_0.bmp"), D3DXIFF_BMP, pMask0->GetSurface(), NULL, NULL);
			//D3DXSaveSurfaceToFile(_T("./BarmWipe_Mask_1.bmp"), D3DXIFF_BMP, pMask1->GetSurface(), NULL, NULL);
			//D3DXSaveSurfaceToFile(_T("./BarmWipe_Mask.bmp"), D3DXIFF_BMP, pMaskDef->GetSurface(), NULL, NULL);

			pVM->ReleaseVideoBuffer(pMask0);
			pVM->ReleaseVideoBuffer(pMask1);
		}
		break;
	}
	return S_OK;
}
float	CBarmWipe::CalcOffset(void* pParamRaw,int nPattern)
{
	BarmWipeFxParam* pParam = (BarmWipeFxParam*)pParamRaw;	

	D3DXMATRIXA16 matType,matRot;

	D3DXMatrixIdentity(&matType);	
	D3DXMatrixRotationZ(&matRot,D3DXToRadian(pParam->fSlant));

	float fOffset = 1.0f; 
	float xMax = 0.5f,yMax = 0.5f   / m_fAspect;

	switch(nPattern)
	{
	case 0:		
		break;
	case 1:
		D3DXMatrixRotationZ(&matType,D3DXToRadian(90));		
		break;
	case 2:
		D3DXMatrixRotationZ(&matType,- (D3DX_PI  / 2.0f - atan2(yMax,xMax)));			
		break;
	case 3:
		D3DXMatrixRotationZ(&matType,(D3DX_PI  / 2.0f - atan2(yMax,xMax)));			
		break;
	}	

	D3DXVECTOR3 vLine0[] = 
	{
		D3DXVECTOR3(0.0f,0.0f,0.0f),
		D3DXVECTOR3(xMax,0.0f,0.0f),
	};
	D3DXVec3TransformCoord(&vLine0[1],&vLine0[1],&matType);
	D3DXVec3TransformCoord(&vLine0[1],&vLine0[1],&matRot);


	D3DXVECTOR3 vNormal = D3DXVECTOR3(vLine0[1].y,-vLine0[1].x,0.0f); 

	D3DXVECTOR3 vNormal1 = D3DXVECTOR3(xMax * SIGN(vLine0[1].x),yMax * SIGN(vLine0[1].y),0.0f);
	D3DXVECTOR3 vLine1[] = 
	{
		vNormal1,
		vNormal1 + vNormal,
	};

	D3DXVECTOR3 vPtIntersection;

	if(LineIntersection(vLine0,vLine1,vPtIntersection))
	{
		fOffset = D3DXVec3Length(&vPtIntersection);						
	}	
	return fOffset;
}

HRESULT CBarmWipe::Draw(CVideoBuffer* pMaskDef, void* pParamRaw, int nPattern,float fOffset)
{
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMan = m_pEngine->GetResourceManager();
	BarmWipeFxParam* pParam = (BarmWipeFxParam*)pParamRaw;		
	fOffset *= pParam->structPattern.fOffset;
	const VideoBufferInfo& biMask = pMaskDef->GetVideoBufferInfo();
	
	HRESULT hr = E_FAIL;

	D3DXMATRIXA16 matRot,matTrans,matWorld,matType,matCombine;	

	float fAxisOffset = (pParam->structPattern.fBorderWidth + pParam->structPattern.fSoftWidth) / 4.0f ;
	fAxisOffset *= -(pParam->structPattern.fOffset * 2.0f - 1.0f);	

	bool bOdd = false;
	float MaskDef_OffsetY_Other = 1.0f;
	//if(pMaskDef->OffsetY_Other == 1 && pMaskDef->bIsCGBuffer)
	//	D3DXMatrixTranslation(&matTrans,pParam->structPattern.fCenterX,pParam->structPattern.fCenterY + 0.5f / pMaskDef->GetImageHeight() * (pParam->bOdd?1.0f:0.0f),0.0f);
	//else
		D3DXMatrixTranslation(&matTrans,pParam->structPattern.fCenterX,pParam->structPattern.fCenterY + 0.5f / biMask.nHeight * (bOdd?0.0f:1.0f),0.0f);

	matTrans._42 /= m_fAspect;

	D3DXMatrixIdentity(&matType);	
	D3DXMatrixRotationZ(&matRot,D3DXToRadian(pParam->fSlant));

	float xMax = 0.5f,yMax = 0.5f / m_fAspect;

 	switch(nPattern)
	{
	case 0:		
		break;
	case 1:
		D3DXMatrixRotationZ(&matType,D3DXToRadian(90));		
		break;
	case 2:
		D3DXMatrixRotationZ(&matType,- (D3DX_PI  / 2.0f - atan2(yMax,xMax)));			
		break;
	case 3:
		D3DXMatrixRotationZ(&matType,(D3DX_PI  / 2.0f - atan2(yMax,xMax)));			
		break;
	}	
	matWorld = matType *  matRot * matTrans;
	LPD3DXMATRIX matView = NULL, matProj= NULL;
	pResMan->GetPerspectiveMatrix(&matView, &matProj);
	matCombine = matWorld * *matView * m_matAspectProj;

	bool bOK = m_pEngine->SetRenderTarget(pMaskDef);
	ASSERT(bOK);
	BOOL bDrawBorder = pParam->structPattern.fBorderWidth > 0.0f;
	BOOL bDrawSoft = pParam->structPattern.fSoftWidth > 0.0f;

	if(bDrawBorder || bDrawSoft)
	{		
		D3DXPLANE plane(-1.0f ,0.0f,0.0f,0.0f);

		D3DXMATRIXA16 matClip;	
		matClip = matType * matRot * *matView * m_matAspectProj;
		D3DXMatrixInverse(&matClip,NULL,&matClip);
		D3DXMatrixTranspose(&matClip,&matClip);                        
		D3DXPlaneTransform(&plane,&plane,&matClip);
		
		D3DXVECTOR4 vCenter(0.0f,0.0f,0.0f,1.0f);		
		D3DXVec4Transform(&vCenter,&vCenter,&matCombine);
		vCenter /= vCenter.w;
		
		D3DXPlaneFromPointNormal(&plane,&D3DXVECTOR3(vCenter.x,vCenter.y,vCenter.z),&D3DXVECTOR3(plane.a,plane.b,plane.c));

		hr = pDevice->SetClipPlane(0,(float*)&plane); 
		ASSERT(SUCCEEDED(hr));
		D3DXMATRIXA16 matProj = *matView * m_matAspectProj;
  
		hr = m_pEffect->SetTechnique("ZMask");
		ASSERT(SUCCEEDED(hr));
		hr = m_pEffect->SetMatrix("g_matWorldViewProj",&matProj);
		ASSERT(SUCCEEDED(hr));

		UINT cPass;
		hr = m_pEffect->Begin(&cPass,0);
		ASSERT(SUCCEEDED(hr));
		hr = m_pEffect->BeginPass(0);
		ASSERT(SUCCEEDED(hr));
		bOK = m_pQuadMesh->DrawMeshFx();
		ASSERT(bOK);
		hr = m_pEffect->EndPass();
		ASSERT(SUCCEEDED(hr));
		hr = m_pEffect->End();
		ASSERT(SUCCEEDED(hr));

		D3DXVECTOR4 vMisc1(D3DCMP_GREATER,0.0f,0.0f,0.0f);
		hr = m_pEffect->SetVector("g_vMisc1",&vMisc1);

		//D3DXSaveTextureToFile(L"C:\\mask.bmp",D3DXIFF_BMP,((CBaseTexture*)pMaskDef->pContainer)->GetTexture(),NULL);		
	}		

	hr = m_pEffect->SetTechnique("Mask");
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetMatrix("g_matWorldViewProj",&matCombine);
	ASSERT(SUCCEEDED(hr));
	D3DXVECTOR4 vMisc(pParam->structPattern.fBorderWidth / 4.0f,pParam->structPattern.fSoftWidth / 4.0f,
		fOffset,
		fAxisOffset);
	hr = m_pEffect->SetVector("g_vMisc",&vMisc);
	ASSERT(SUCCEEDED(hr));

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

	for(int i = 0; i < 2;i ++)
	{

		UINT cPass;
		hr = m_pEffect->Begin(&cPass,0);
		ASSERT(SUCCEEDED(hr));
		for(UINT uPass = 0;uPass < cPass; uPass ++)
		{
			if(!aPass[uPass])
				continue;			
			hr = m_pEffect->BeginPass(uPass);
			ASSERT(SUCCEEDED(hr));
			bOK = m_pBarmMesh->DrawSubset(0);
			ASSERT(bOK);
			hr = m_pEffect->EndPass();
			ASSERT(SUCCEEDED(hr));
		}
		hr = m_pEffect->End();
		ASSERT(SUCCEEDED(hr));

		aPass[0] = FALSE; 

		D3DXMATRIXA16 matMirror;
		D3DXMatrixIdentity(&matMirror);
		matMirror._11 *= -1;

		matWorld = matMirror * matType *  matRot * matTrans;
		matCombine = matWorld * *matView * m_matAspectProj;
		hr = m_pEffect->SetMatrix("g_matWorldViewProj",&matCombine);
		ASSERT(SUCCEEDED(hr));

		D3DXVECTOR4 vMisc(D3DCMP_LESS,0.0f,0.0f,0.0f);	
		hr = m_pEffect->SetVector("g_vMisc1",&vMisc);
		ASSERT(SUCCEEDED(hr));
	}
	//D3DXSaveTextureToFile(L"C:\\mask.bmp",D3DXIFF_BMP,((CBaseTexture*)pMaskDef->pContainer)->GetTexture(),NULL);
	return S_OK;
}

HRESULT CBarmWipe::InitMesh(CRenderEngine* pEngine)
{
	m_pEngine = pEngine;
	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();

	CBaseWipe::InitMesh(pEngine);

	m_pEffect = pResMgr->CreateEffect(pDevice, _T("NewEffects/Barm_Mask.fx"));
	ASSERT(m_pEffect);

	m_pBarmMesh = pResMgr->FindMesh(_T("BarmMesh"));
	if(!m_pBarmMesh)
	{
		LPD3DXMESH pMesh = NULL;

		HRESULT hr = S_OK;
		int nNumVertex = 4;
		int nNumFace =  2;
		// Create the vertex buffer.
		if(FAILED(hr=D3DXCreateMesh(nNumFace,nNumVertex, D3DXMESH_MANAGED | D3DXMESH_WRITEONLY, declWipe, pDevice, &pMesh)))
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

		pVertices[0] = D3DXVECTOR3(-1.0f,-10.5f,0.0f);
		pVertices[1] = D3DXVECTOR3(-1.0f,10.5f,0.0f);

		pVertices[2] = D3DXVECTOR3(1.0f,-10.5f,1.0f);
		pVertices[3] = D3DXVECTOR3(1.0f,10.5f,1.0f);

		pIB[0] = 0;
		pIB[1] = 1;
		pIB[2] = 2;

		pIB[3] = 1;
		pIB[4] = 3;
		pIB[5] = 2;

		pAtt[0] = 0;
		pAtt[1] = 0;

		pMesh->UnlockVertexBuffer();
		pMesh->UnlockIndexBuffer();
		pMesh->UnlockAttributeBuffer();

		m_pBarmMesh = pResMgr->CreateMesh(pDevice, pMesh, _T("BarmMesh"));
		ASSERT(m_pBarmMesh);
	}	

	return S_OK;
}

