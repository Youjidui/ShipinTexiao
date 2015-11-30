#include "StdAfx.h"
#include "QuadPageRollRender.h"
#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/VideoBuffer.h"
#include "../FxParam.h"
#include "../Utility/mathmacros.h"
#include "../Utility/SafeDelete.h"


#pragma warning(disable: 4244)


CQuadPageRollRender::CQuadPageRollRender(void)
{
}

CQuadPageRollRender::~CQuadPageRollRender(void)
{
}

bool CQuadPageRollRender::Draw(PageRollFxParam* pParamBase, D3DXMATRIX*	matTex)
{
	QuadPageRollFxParam* pParam = (QuadPageRollFxParam*)pParamBase;

	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMgr = m_pEngine->GetResourceManager();
	CVideoBufferManager* pVM = m_pEngine->GetVideoBufferManager();

	D3DXMATRIXA16 matWorld,matTexFront,matTexRear,matTrans,matPrevRotate,matAspect,matPrevScale,matPrevTrans;

	int nEditWidth, nEditHeight;
	m_pEngine->GetTargetVideoSize(nEditWidth, nEditHeight);
	float fAspect = nEditWidth * 1.0f / nEditHeight;

	D3DXVECTOR3 vPoint(0.0f,0.0f,0.0f),vNormal[2];
	// Begin the scene
	if( SUCCEEDED( pDevice->BeginScene() ) )
	{	
		for(int nPart = 0;nPart < 4;nPart ++)
		{
			float fAngle = D3DXToRadian(180 - pParam->fAngle[nPart]);
			D3DXMatrixRotationZ( &matWorld, fAngle );
			//         D3DXMatrixRotationZ(&matPrevRotate,D3DXToRadian(90 + pParam->structGeometry.fAngle)); 
			//         m_pEffect->SetMatrix("g_matPrevRotate",&matPrevRotate);       
			D3DXVECTOR4 vCorner[4] = 
			{
				D3DXVECTOR4(-0.5f * fAspect,-0.50f,0.0f,1.0f),
				D3DXVECTOR4(-0.5f * fAspect,0.50f,0.0f,1.0f),
				D3DXVECTOR4(0.5f * fAspect,-0.50f,0.0f,1.0f),
				D3DXVECTOR4(0.5f * fAspect,0.50f,0.0f,1.0f),
			};

			D3DXMatrixIdentity(&matTrans);
			//if(!pParam->bOdd)
			//	D3DXMatrixTranslation(&matTrans,0.0f,0.5f / pProfile->nEditHeight,0.0f);
			D3DXVec4TransformArray(vCorner,sizeof(D3DXVECTOR4),vCorner,sizeof(D3DXVECTOR4),&matWorld,4);
			float fMin = 10.0f,fMax = -10.0f;
			for(int i = 0; i < 4;i ++)
			{
				fMin = min(fMin,vCorner[i].x);
				fMax = max(fMax,vCorner[i].x);
			}
			fMax += pParam->structGeometry.fRadius / fAspect;
			float fProcess = fMin + (fMax - fMin) * pParam->structTrans.fTransition;

			matTexRear = matTexFront = *matTex;
			D3DXMatrixIdentity(&matPrevTrans);
			D3DXMatrixScaling(&matPrevScale,0.5f,0.5f,1.0f);
			matTexFront._11 *= 0.5f;
			matTexFront._22 *= 0.5f;
			switch(nPart)
			{
			case 0://Left Bottom								
				matPrevTrans._41 = -0.25f;
				matPrevTrans._42 = -0.25f;
				matTexFront._32 += (*matTex)._22 * 0.5f;	

				vNormal[0] = D3DXVECTOR3(-1.0f,0.0f,0.0f);
				vNormal[1] = D3DXVECTOR3(0.0f,-1.0f,0.0f);
				break;			
			case 1://Left Top								
				matPrevTrans._41 = -0.25f;
				matPrevTrans._42 = 0.25f;

				vNormal[0] = D3DXVECTOR3(-1.0f,0.0f,0.0f);
				vNormal[1] = D3DXVECTOR3(0.0f,1.0f,0.0f);
				break;			
			case 2://Right Bottom								
				matPrevTrans._41 = 0.25f;
				matPrevTrans._42 = -0.25f;
				matTexFront._31 += (*matTex)._11 * 0.5f;				
				matTexFront._32 += (*matTex)._22 * 0.5f;	

				vNormal[0] = D3DXVECTOR3(1.0f,0.0f,0.0f);
				vNormal[1] = D3DXVECTOR3(0.0f,-1.0f,0.0f);
				break;		
			case 3://Right Top								
				matPrevTrans._41 = 0.25f;
				matPrevTrans._42 = 0.25f;
				matTexFront._31 += (*matTex)._11 * 0.5f;	

				vNormal[0] = D3DXVECTOR3(1.0f,0.0f,0.0f);
				vNormal[1] = D3DXVECTOR3(0.0f,1.0f,0.0f);
				break;	
			}
			matPrevTrans._41 *= fAspect;
			matTexRear = matTexFront;
			switch(pParam->nGroupPattern)
			{
			case 0: //Nothing
				switch(pParam->nStepPattern) 
				{
				case 0:	break;
				case 1: // one by one				
					fProcess = fMin + (fMax - fMin) * CLAMP(0,1,(pParam->structTrans.fTransition - 0.25f * nPart) * 4.0f);
					break;
				case 2://gradient				
					fProcess = fMin + (fMax - fMin) * CLAMP(0,1,(pParam->structTrans.fTransition  - 0.125f * nPart) / 0.5f);				
					break;
				}
				break;
			case 1://Gap 1
				switch(pParam->nStepPattern) 
				{
				case 0:	break;
				case 1: // one by one				
					fProcess = fMin + (fMax - fMin) * CLAMP(0,1,(pParam->structTrans.fTransition - 0.5f * (nPart % 2)) * 2.0f);
					break;
				case 2://gradient				
					fProcess = fMin + (fMax - fMin) * CLAMP(0,1,(pParam->structTrans.fTransition  - 0.25f * (nPart % 2)) / 0.75f);				
					break;
				}
				break;
			case 2://Gap 2
				switch(pParam->nStepPattern) 
				{
				case 0:	break;
				case 1: // one by one				
					fProcess = fMin + (fMax - fMin) * CLAMP(0,1,(pParam->structTrans.fTransition - 0.5f * (nPart == 1 || nPart == 2)) * 2.0f);
					break;
				case 2://gradient				
					fProcess = fMin + (fMax - fMin) * CLAMP(0,1,(pParam->structTrans.fTransition  - 0.25f * (nPart == 1 || nPart == 2)) / 0.75f);				
					break;
				}
				break;
			}
			fProcess *= 1.1f;

			D3DXPLANE planeClip0,planeClip1;
			D3DXPlaneFromPointNormal(&planeClip0,&vPoint,&vNormal[0]);
			D3DXPlaneFromPointNormal(&planeClip1,&vPoint,&vNormal[1]);
			pDevice->SetClipPlane(0,(float*)&planeClip0);
			pDevice->SetClipPlane(1,(float*)&planeClip1);
			pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,D3DCLIPPLANE0|D3DCLIPPLANE1);


			D3DXVECTOR4 vMisc = D3DXVECTOR4(fProcess,pParam->structGeometry.fRadius / (pParam->bPageRoll ? 2.0f : 1.0f) ,pParam->structRear.fMatteRatio,pParam->structRear.bUseForeGround);
			m_pEffect->SetVector("g_vMisc",&vMisc);

			D3DXMatrixScaling(&matAspect, fAspect,1.0f,1.0f);
			m_pEffect->SetMatrix("g_matRotate",&(matAspect * matWorld));
			D3DXMatrixRotationZ( &matWorld, -fAngle );
			m_pEffect->SetMatrix("g_matWorld",&matWorld);
			D3DXMATRIX *matView = NULL, *matProj= NULL;
			pResMgr->GetPerspectiveMatrix( &matView, &matProj);		//CGPURenderµÄ×ÓÀà
			m_pEffect->SetMatrix("g_matWVP",&(matWorld * matPrevScale * matPrevTrans * matTrans * *matView * m_matProject));	

			D3DXVECTOR4 vLightDir = D3DXVECTOR4(cosf(D3DXToRadian( pParam->structLight.fAnglePhi)),0.0f,sinf(D3DXToRadian( pParam->structLight.fAnglePhi)),0.0f);
			D3DXVECTOR4 vEyeDir = D3DXVECTOR4(cosf(D3DXToRadian( pParam->structLight.fAngleTheta)),0.0f,sinf(D3DXToRadian( pParam->structLight.fAngleTheta)),0.0f);
			m_pEffect->SetVector("g_vLightDir",&vLightDir);
			m_pEffect->SetVector("g_vEyeDir",&vEyeDir);		

			m_pEffect->SetMatrix("g_matTexFront",&matTexFront);
			m_pEffect->SetMatrix("g_matTexRear",&matTexRear);		

			LPDIRECT3DVERTEXBUFFER9 pVB = NULL;	
			LPDIRECT3DINDEXBUFFER9	pIB = NULL;
			IDirect3DVertexDeclaration9 * pVertexDecl = NULL;	
			m_pMesh->GetMesh()->GetVertexBuffer(&pVB);
			m_pMesh->GetMesh()->GetIndexBuffer(&pIB);
			//pVertexDecl =  m_pVertexDecl->GetVertexDeclarationPtr();
			pVertexDecl =  m_pMesh->GetVertexDeclarationPtr();
			ASSERT(NULL != pVertexDecl);

			pDevice->SetStreamSource(0,pVB,0,sizeof(D3DXVECTOR3));		
			pDevice->SetIndices(pIB);		
			pDevice->SetVertexDeclaration(pVertexDecl);
			SAFE_RELEASE(pVB);
			SAFE_RELEASE(pIB);

			if(pParam->bPageRoll)
				m_pEffect->SetTechnique("PageRoll");
			else
				m_pEffect->SetTechnique("PageTurn");

			//UINT cPass,uPass = 0; pass 0 is for yuv
			UINT cPass,uPass = 1;	//pass 1 is for rgb
			m_pEffect->Begin(&cPass,0);

			m_pEffect->BeginPass(uPass);				
			pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,NumGrid * NumGrid,0,(NumGrid - 1) * (NumGrid - 1) * 2);
			m_pEffect->EndPass();

			m_pEffect->End();

			pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,0);
		}
		// End the scene
		pDevice->EndScene();
	}	
	return TRUE;
}