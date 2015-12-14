#include "StdAfx.h"
#include "MatrixWipe.h"
#include <MMSystem.h>
#pragma comment(lib, "winmm")
#include <stdlib.h>

#pragma warning(disable:4244)

typedef enum
{
	DIR_X_P = 0,
	DIR_X_N,
	DIR_Y_P,
	DIR_Y_N,
};

int CMatrixWipe::sm_nTitleSize = 0;
int CMatrixWipe::sm_nPattern = -1;
int CMatrixWipe::sm_nSequence = -1;

CMatrixWipe::CMatrixWipe()
: m_pTexture(NULL)
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXVECTOR3 vEyePt( 0.0f, 0.0f,-0.5f/tanf(D3DX_PI/8) );
	D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );	
	D3DXMatrixLookAtLH( &m_matView, &vEyePt, &vLookatPt, &vUpVec );

	D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4, 1.0f, 0.5f, 1000.0f );

	m_nTitleSize = 0;
	m_nPattern = -1;
	m_nOffset = 0;
	m_nSequence = -1;
	m_nLastFrame = 0;
}

CMatrixWipe::~CMatrixWipe(void)
{
	if(m_pTexture)
		m_pEngine->GetVideoBufferManager()->ReleaseVideoBuffer(m_pTexture);
}

HRESULT CMatrixWipe::Init(CRenderEngine* pEngine)
{
	HRESULT hr = CWipeBase::Init(pEngine);
	ASSERT(SUCCEEDED(hr));
	ASSERT(m_pEngine);

	LPDIRECT3DDEVICE9 pDevice = m_pEngine->GetDevice();
	CResourceManager* pResMan = m_pEngine->GetResourceManager();
	CVideoBufferManager* pVM = m_pEngine->GetVideoBufferManager();

	m_pEffect = pResMan->CreateEffect(pDevice, _T("NewEffects/GMatrix_Mask.fx"));
	ASSERT(m_pEffect);

	//m_pTexture = pResMan->CreateTexture(20,20,D3DUSAGE_DYNAMIC,D3DFMT_R32F,D3DPOOL_DEFAULT,&UUID_TEXTURE);
	VideoBufferInfo bi = {D3DFMT_R32F, VideoBufferInfo::VIDEO_MEM, VideoBufferInfo::_IN, 20, 20, 0, 0 };
	m_pTexture =  pVM->CreateVideoBuffer(bi);
	ASSERT(m_pTexture);

	return S_OK;
}

void CMatrixWipe::Ready( CVideoBuffer* pMaskDef )
{
	CWipeBase::Ready(pMaskDef);
}

//HRESULT CMatrixWipe::Draw( CVideoBuffer *pMaskDef, MatrixWipeFxParam* pParam )
HRESULT CMatrixWipe::Draw(CVideoBuffer* pMaskDef, BasicWipeFxParam* pParam)
{
	HRESULT hr = E_FAIL;

	GenerateSequence((MatrixWipeFxParam*)pParam);

	float fAxisOffset = pParam->structPattern.fOffset * 2.0f - 1.0f;	

	bool bOK = m_pEngine->SetRenderTarget(pMaskDef);
	ASSERT(bOK);

	D3DXMATRIXA16 matCombine; 
	matCombine = m_matView * m_matProj;
	hr = m_pEffect->SetTechnique("Mask");
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetMatrix("g_matWorldViewProj",&matCombine);
	ASSERT(SUCCEEDED(hr));

	D3DXVECTOR4 vMisc(m_nTitleSize,m_nOffset * pParam->structPattern.fOffset,0.0f,0.0f);

	float fBorderOffset = pParam->structPattern.fBorderWidth * m_nTitleSize * m_nTitleSize / 4.0f;	
	float fSoftOffset = pParam->structPattern.fSoftWidth * m_nTitleSize * m_nTitleSize / 4.0f;	
	float fCenterShift = fAxisOffset * (fBorderOffset + fSoftOffset);	
	float fCenter = m_nOffset * pParam->structPattern.fOffset + fCenterShift;

	float fBorderLow = fCenter  - (vMisc.y - floor(vMisc.y)) - fBorderOffset;
	float fBorderHigh = fCenter + (ceil(vMisc.y) - vMisc.y) + fBorderOffset;

	D3DXVECTOR4 vBorder(fBorderLow,fBorderHigh - 1.0f,fCenter,0.0f);
	hr = m_pEffect->SetVector("g_vBorder",&vBorder);
	ASSERT(SUCCEEDED(hr));

	float fSoftLow = fCenter - (vMisc.y - floor(vMisc.y))  - fSoftOffset;
	float fSoftHigh = fCenter + (ceil(vMisc.y) - vMisc.y) + fSoftOffset;

	D3DXVECTOR4 vSoft(fSoftLow,fSoftHigh,fSoftOffset,0.0f);	
	hr = m_pEffect->SetVector("g_vSoft",&vSoft);
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetVector("g_vMisc",&vMisc);
	ASSERT(SUCCEEDED(hr));
	hr = m_pEffect->SetTexture("g_txSequence",m_pTexture->GetTexture());
	ASSERT(SUCCEEDED(hr));

	BOOL bDrawBorder = pParam->structPattern.fBorderWidth > 0.0f;
	BOOL bDrawSoft = pParam->structPattern.fSoftWidth > 0.0f;

	BOOL aPass[5] = {FALSE};	
	if(bDrawBorder && bDrawSoft)
	{
		aPass[3] = TRUE;	
		if(pParam->structPattern.fSoftWidth > pParam->structPattern.fBorderWidth)
			aPass[4] = TRUE;
	}			
	else if(bDrawBorder)
	{
		aPass[1] = TRUE;		
	}
	else if(bDrawSoft)
	{
		aPass[2] = TRUE;

	}
	else
		aPass[0] = TRUE;

	UINT cPass;
	hr = m_pEffect->Begin(&cPass,0);
	ASSERT(SUCCEEDED(hr));
	for(UINT uPass = 0;uPass < cPass; uPass ++)
	{
		if(!aPass[uPass])
			continue;							
		hr = m_pEffect->BeginPass(uPass);
		ASSERT(SUCCEEDED(hr));
		bOK = m_pQuadMesh->DrawMeshFx();
		ASSERT(bOK);
		hr = m_pEffect->EndPass();
		ASSERT(SUCCEEDED(hr));
	}
	hr = m_pEffect->End();
	ASSERT(SUCCEEDED(hr));

	return hr;
}

void CMatrixWipe::GenerateSequence(MatrixWipeFxParam* pParam)
{
	int nTitleSize = ceil(10 / pow(10.0f,pParam->fTitleSize)) * 2;   
	//if(pParam->structPattern.nPattern == 13 ) //Noisy
	//{
	//	if((pParam->dwCurFrame - m_nLastFrame) > 1)
	//	{
	//		m_nLastFrame = pParam->dwCurFrame;
	//		m_nTitleSize = 0;
	//	}
	//}
	if(nTitleSize != m_nTitleSize || m_nPattern != pParam->structPattern.nPattern || m_nSequence != pParam->nSequence 
		|| sm_nTitleSize != nTitleSize || sm_nPattern != pParam->structPattern.nPattern || sm_nSequence != pParam->nSequence)
	{ 
		m_nOffset = nTitleSize * nTitleSize;

		m_nTitleSize = nTitleSize;
		m_nPattern = pParam->structPattern.nPattern;
		m_nSequence = pParam->nSequence;
		// wsp note:由于特技的所有实例公用一张纹理m_uResID_Texture,因此,当所有实例的参数(TitleSize或Pattern或nSequence)都没有改变,
		// 而在该特技的不同实例之间切换时,需要判断实例间的参数是否更改,进而决定是否变更唯一的纹理m_uResID_Texture
		sm_nTitleSize = nTitleSize;
		sm_nPattern = pParam->structPattern.nPattern;
		sm_nSequence = pParam->nSequence;

		switch(pParam->structPattern.nPattern) 
		{
		case 0://top horizontal
			TopHorizontal();
			break;
		case 1://Bottom horizontal
			BottomHorizontal();
			break;
		case 2:
			LeftVertical();
			break;
		case 3:
			RightVertical();
			break;
		case 4:
			DiagonalTopLeft();
			break;
		case 5:
			DiagonalTopLeft();
			break;
		case 6:
			TopLeftSpiral();
			break;
		case 7:
			BottomRightSpiral();
			break;
		case 8:
			TopLeftWaterFall();
			break;
		case 9:
			TopLeftWaterFall();
			break;
		case 10:
			LeftWaterFall();
			break;
		case 11:
			LeftWaterFall();
			break;
		case 12:
		case 13:
			Random();
			break;			
		default:ASSERT(0);break;
		}
	}
}

void CMatrixWipe::Random()
{
	srand(m_nPattern == 12 ? 777 : timeGetTime()); 
	CDWordArray array;
	switch(m_nSequence) 
	{
	case 0://normal	
		for(int i = 0;i < m_nTitleSize * m_nTitleSize ;i ++)
			array.Add(i);		
		break;
	case 1://Birdirection
		for(int i = 0;i < m_nTitleSize * m_nTitleSize / 2 ;i ++)
		{
			array.Add(i);		
			array.Add(i);		
		}
		m_nOffset /= 2;
		break;
	default:
		for(int i = 0;i < m_nTitleSize * m_nTitleSize ;i += 2)
		{
			array.Add(i + 1);	
			array.Add(i);	
		}
		break;
	}

	ASSERT(m_pTexture);
	LPDIRECT3DTEXTURE9 pTex = m_pTexture->GetTexture();
	ASSERT(pTex);
	D3DLOCKED_RECT lr;
	HRESULT hr = pTex->LockRect(0,&lr,NULL,0);
	ASSERT(SUCCEEDED(hr));
	float * pData = NULL;
	for(int y = 0; y < m_nTitleSize ; y ++)
	{
		pData = (float*)((byte*)lr.pBits + lr.Pitch * y);
		for(int x = 0; x < m_nTitleSize ; x ++)
		{
			UINT uRand;
			//rand_s(&uRand);
			uRand = rand();
			int nIndex = uRand % array.GetCount();
			pData[x] = array.GetAt(nIndex);						
			array.RemoveAt(nIndex);
		}
	}
	pTex->UnlockRect(0);
}


void CMatrixWipe::LeftWaterFall()
{
	ASSERT(m_pTexture);
	LPDIRECT3DTEXTURE9 pTex = m_pTexture->GetTexture();
	ASSERT(pTex);
	D3DLOCKED_RECT lr;
	HRESULT hr = pTex->LockRect(0,&lr,NULL,0);
	ASSERT(SUCCEEDED(hr));
	float * pData = NULL; 

	m_nOffset = m_nTitleSize + (m_nTitleSize - 1) * 2;
	int nSeq = 0;
	int x = 0;
	int nCount = 0;
	int nReverse = 0;
	int nStep = max(m_nTitleSize / 4,1);

	switch(m_nSequence) {
	case 0://normal
		while (nCount <  m_nTitleSize * m_nTitleSize)
		{
			int xx = x;
			for(int y = 0; y < m_nTitleSize ;y ++)
			{						
				if(xx < 0 )
					break;
				if(xx < m_nTitleSize)
				{
					pData = (float*)((byte*)lr.pBits + lr.Pitch * y);
					if(m_nPattern == 10)
						pData[xx] = nSeq;
					else ///11
						pData[m_nTitleSize - 1 - xx] = nSeq;
					nCount ++;
				}						
				xx -= nStep;
			}
			nSeq ++;
			x ++;
		}
		break;
	case 1://Bidirection	
		//Left_top
		while (nCount <  m_nTitleSize * m_nTitleSize / 2)
		{
			int xx = x;
			for(int y = 0; y < m_nTitleSize ; y ++)
			{						
				if(xx < 0 )
					break;
				if(xx < m_nTitleSize)
				{
					pData = (float*)((byte*)lr.pBits + lr.Pitch * y);
					if(m_nPattern == 10)
						pData[xx] = nSeq;
					else ///11
						pData[m_nTitleSize - 1 - xx] = nSeq;
					nCount ++;
				}						
				xx -= nStep;
			}
			nSeq ++;
			x ++;
		}
		//Right_up
		nSeq = 0;		
		x = m_nTitleSize - 1;
		nCount = 0;
		while (nCount <  m_nTitleSize * m_nTitleSize / 2)
		{
			int xx = x;
			for(int y = m_nTitleSize - 1; y > -1 ; y --)
			{						
				if(xx >= m_nTitleSize )
					break;
				if(xx > -1)
				{
					pData = (float*)((byte*)lr.pBits + lr.Pitch * y);
					if(m_nPattern == 10)
						pData[xx] = nSeq;
					else ///11
						pData[m_nTitleSize - 1 - xx] = nSeq;
					nCount ++;
				}						
				xx += nStep;
			}
			nSeq ++;
			x --;
		}		
		break;
	case 2://checker
	case 3://checker Reverse
		for( int i = 0;i < 2; i ++)
		{
			x = 0;			
			while (nCount <  m_nTitleSize * m_nTitleSize / 2)
			{
				BOOL bAdd = FALSE;
				int xx = x;
				for(int y = 0; y < m_nTitleSize ; y ++)
				{						
					if(xx < 0 )
						break;
					if(xx < m_nTitleSize 
						&& (i == 0 && (y + xx) % 2 == 0 || i == 1 && (y + xx) % 2 == 1) )
					{
						pData = (float*)((byte*)lr.pBits + lr.Pitch * y);
						if(m_nPattern == 10)
							pData[xx] = nSeq;
						else ///11
							pData[m_nTitleSize - 1 - xx] = nSeq;
						nCount ++;
						bAdd = TRUE;
					}						
					xx -= nStep;
				}
				if(bAdd)
				{					
					nSeq ++;

					if(i == 1 && m_nSequence == 3)
						nReverse ++;
				}
				x ++;
			}	
			nCount = 0;			
		}
		if(m_nSequence == 3)
		{
			for(int y = 0; y < m_nTitleSize; y ++)
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);
				for(int x = 0; x < m_nTitleSize; x ++)
				{
					if(pData[x] >= nSeq - nReverse)
					{
						pData[x] = nSeq - 1 - (pData[x] - (nSeq - nReverse));
					}
				}
			}
		}
		break;
	case 4://Bidirection	Checker
	case 5://Bidirection	Checker Reverse
		//Left_top
		for( int i = 0;i < 2; i ++)
		{
			nCount = 0;
			x = 0;	
			while (nCount <  m_nTitleSize * m_nTitleSize / 4)
			{
				BOOL bAdd = FALSE;
				int xx = x;
				for(int y = 0; y < m_nTitleSize && nCount <  m_nTitleSize * m_nTitleSize / 4; y ++)
				{						
					if(xx < 0 )
						break;
					if(xx < m_nTitleSize
						&& (i == 0 && (y + xx) % 2 == 0 || i == 1 && (y + xx) % 2 == 1))
					{
						pData = (float*)((byte*)lr.pBits + lr.Pitch * y);
						if(m_nPattern == 10)
							pData[xx] = nSeq;
						else ///11
							pData[m_nTitleSize - 1 - xx] = nSeq;
						nCount ++;
						bAdd = TRUE;
					}						
					xx -= nStep;
				}
				if(bAdd)
				{
					nSeq ++;
					if(i == 1 && m_nSequence == 5)
						nReverse ++;
				}
				x ++;
			}			
		}
		//Right_up
		nSeq = 0;	
		for( int i = 0;i < 2; i ++)
		{
			x = m_nTitleSize - 1;
			nCount = 0;
			while (nCount <  m_nTitleSize * m_nTitleSize / 4)
			{
				BOOL bAdd = FALSE;
				int xx = x;
				for(int y = m_nTitleSize - 1; y > -1 && nCount <  m_nTitleSize * m_nTitleSize / 4; y --)
				{						
					if(xx >= m_nTitleSize )
						break;
					if(xx > -1
						&& (i == 0 && (y + xx) % 2 == 0 || i == 1 && (y + xx) % 2 == 1))
					{
						pData = (float*)((byte*)lr.pBits + lr.Pitch * y);
						if(m_nPattern == 10)
							pData[xx] = nSeq;
						else ///11
							pData[m_nTitleSize - 1 - xx] = nSeq;
						nCount ++;
						bAdd = TRUE;
					}						
					xx += nStep;
				}
				if(bAdd)
					nSeq ++;
				x --;
			}	
		}
		if(m_nSequence == 5)
		{
			for(int y = 0; y < m_nTitleSize; y ++)
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);
				for(int x = 0; x < m_nTitleSize; x ++)
				{
					if(pData[x] >= nSeq - nReverse)
					{
						pData[x] = nSeq - 1 - (pData[x] - (nSeq - nReverse));
					}
				}
			}
		}

		break;
	}
	m_nOffset = nSeq;
	pTex->UnlockRect(0);
}

void CMatrixWipe::TopLeftWaterFall()
{
	ASSERT(m_pTexture);
	LPDIRECT3DTEXTURE9 pTex = m_pTexture->GetTexture();
	ASSERT(pTex);
	D3DLOCKED_RECT lr;
	HRESULT hr = pTex->LockRect(0,&lr,NULL,0);
	ASSERT(SUCCEEDED(hr));
	float * pData = NULL; 

	m_nOffset = m_nTitleSize + (m_nTitleSize - 1) * 2;
	int nSeq = 0;
	int y = 0;
	int nCount = 0;
	int nReverse = 0;
	int nStep = max(m_nTitleSize / 4,1);

	switch(m_nSequence) {
	case 0://normal
		while (nCount <  m_nTitleSize * m_nTitleSize)
		{
			int yy = y;
			for(int x = 0; x < m_nTitleSize ; x ++)
			{						
				if(yy < 0 )
					break;
				if(yy < m_nTitleSize)
				{
					pData = (float*)((byte*)lr.pBits + lr.Pitch * yy);
					if(m_nPattern == 8)
						pData[x] = nSeq;
					else ///9
						pData[m_nTitleSize - 1 - x] = nSeq;
					nCount ++;
				}						
				yy -= nStep;
			}
			nSeq ++;
			y ++;
		}
		break;
	case 1://Bidirection	
		//Left_top
		while (nCount <  m_nTitleSize * m_nTitleSize / 2)
		{
			int yy = y;
			for(int x = 0; x < m_nTitleSize ; x ++)
			{						
				if(yy < 0 )
					break;
				if(yy < m_nTitleSize)
				{
					pData = (float*)((byte*)lr.pBits + lr.Pitch * yy);
					if(m_nPattern == 8)
						pData[x] = nSeq;
					else ///9
						pData[m_nTitleSize - 1 - x] = nSeq;
					nCount ++;
				}						
				yy -= nStep;
			}
			nSeq ++;
			y ++;
		}
		//Right_up
		nSeq = 0;		
		y = m_nTitleSize - 1;
		nCount = 0;
		while (nCount <  m_nTitleSize * m_nTitleSize / 2)
		{
			int yy = y;
			for(int x = m_nTitleSize - 1; x > -1 ; x --)
			{						
				if(yy >= m_nTitleSize )
					break;
				if(yy > -1)
				{
					pData = (float*)((byte*)lr.pBits + lr.Pitch * yy);
					if(m_nPattern == 8)
						pData[x] = nSeq;
					else ///9
						pData[m_nTitleSize - 1 - x] = nSeq;
					nCount ++;
				}						
				yy += nStep;
			}
			nSeq ++;
			y --;
		}		
		break;
	case 2://checker
	case 3://checker Reverse
		for( int i = 0;i < 2; i ++)
		{
			y = 0;			
			while (nCount <  m_nTitleSize * m_nTitleSize / 2)
			{
				BOOL bAdd = FALSE;
				int yy = y;
				for(int x = 0; x < m_nTitleSize ; x ++)
				{						
					if(yy < 0 )
						break;
					if(yy < m_nTitleSize 
						&& (i == 0 && (x + yy) % 2 == 0 || i == 1 && (x + yy) % 2 == 1) )
					{
						pData = (float*)((byte*)lr.pBits + lr.Pitch * yy);
						if(m_nPattern == 8)
							pData[x] = nSeq;
						else ///9
							pData[m_nTitleSize - 1 - x] = nSeq;
						nCount ++;
						bAdd = TRUE;
					}						
					yy -= nStep;
				}
				if(bAdd)
				{					
					nSeq ++;

					if(i == 1 && m_nSequence == 3)
						nReverse ++;
				}
				y ++;
			}	
			nCount = 0;			
		}
		if(m_nSequence == 3)
		{
			for(int y = 0; y < m_nTitleSize; y ++)
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);
				for(int x = 0; x < m_nTitleSize; x ++)
				{
					if(pData[x] >= nSeq - nReverse)
					{
						pData[x] = nSeq - 1 - (pData[x] - (nSeq - nReverse));
					}
				}
			}
		}
		break;
	case 4://Bidirection	Checker
	case 5://Bidirection	Checker Reverse
		//Left_top
		for( int i = 0;i < 2; i ++)
		{
			nCount = 0;
			y = 0;	
			while (nCount <  m_nTitleSize * m_nTitleSize / 4)
			{
				BOOL bAdd = FALSE;
				int yy = y;
				for(int x = 0; x < m_nTitleSize && nCount <  m_nTitleSize * m_nTitleSize / 4; x ++)
				{						
					if(yy < 0 )
						break;
					if(yy < m_nTitleSize
						&& (i == 0 && (x + yy) % 2 == 0 || i == 1 && (x + yy) % 2 == 1))
					{
						pData = (float*)((byte*)lr.pBits + lr.Pitch * yy);
						if(m_nPattern == 8)
							pData[x] = nSeq;
						else ///9
							pData[m_nTitleSize - 1 - x] = nSeq;
						nCount ++;
						bAdd = TRUE;
					}						
					yy -= nStep;
				}
				if(bAdd)
				{
					nSeq ++;
					if(i == 1 && m_nSequence == 5)
						nReverse ++;
				}
				y ++;
			}			
		}
		//Right_up
		nSeq = 0;	
		for( int i = 0;i < 2; i ++)
		{
			y = m_nTitleSize - 1;
			nCount = 0;
			while (nCount <  m_nTitleSize * m_nTitleSize / 4)
			{
				BOOL bAdd = FALSE;
				int yy = y;
				for(int x = m_nTitleSize - 1; x > -1 && nCount <  m_nTitleSize * m_nTitleSize / 4; x --)
				{						
					if(yy >= m_nTitleSize )
						break;
					if(yy > -1
						&& (i == 0 && (x + yy) % 2 == 0 || i == 1 && (x + yy) % 2 == 1))
					{
						pData = (float*)((byte*)lr.pBits + lr.Pitch * yy);
						if(m_nPattern == 8)
							pData[x] = nSeq;
						else ///9
							pData[m_nTitleSize - 1 - x] = nSeq;
						nCount ++;
						bAdd = TRUE;
					}						
					yy += nStep;
				}
				if(bAdd)
					nSeq ++;
				y --;
			}	
		}
		if(m_nSequence == 5)
		{
			for(int y = 0; y < m_nTitleSize; y ++)
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);
				for(int x = 0; x < m_nTitleSize; x ++)
				{
					if(pData[x] >= nSeq - nReverse)
					{
						pData[x] = nSeq - 1 - (pData[x] - (nSeq - nReverse));
					}
				}
			}
		}

		break;
	}
	m_nOffset = nSeq;
	pTex->UnlockRect(0);
}


void CMatrixWipe::TopLeftSpiral()
{
	ASSERT(m_pTexture);
	LPDIRECT3DTEXTURE9 pTex = m_pTexture->GetTexture();
	ASSERT(pTex);
	D3DLOCKED_RECT lr;
	HRESULT hr = pTex->LockRect(0,&lr,NULL,0);
	ASSERT(SUCCEEDED(hr));
	float * pData = NULL; 

	int nSeq = 0;
	int nDir = DIR_X_P;
	int x = 0,y = 0;				
	int xMax = m_nTitleSize,yMax = m_nTitleSize,xMin = -1,yMin = 0;
	int nStep = 1;
	switch(m_nSequence)
	{
	case 0://Normal
	case 1://BirDirection				
		for(int i = 0; i < 2;i ++)
		{			
			while ( m_nSequence == 0 && nSeq < m_nTitleSize * m_nTitleSize
				|| m_nSequence == 1 && nSeq < m_nTitleSize * m_nTitleSize / 2 && nSeq > -1) 
			{	
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);	

				pData[x] = nSeq;

				nSeq += nStep;			

				switch(nDir) 
				{
				case DIR_X_P: //x +
					x ++;
					if(x + 1 >= xMax)
					{
						xMax --;
						nDir = DIR_Y_P;
					}
					break;
				case DIR_Y_P://y +
					y ++;
					if(y + 1 >= yMax)
					{
						yMax --;
						nDir = DIR_X_N;
					}
					break;
				case DIR_X_N:// x -
					x --;
					if(x - 1 <= xMin)
					{
						xMin ++;
						nDir = DIR_Y_N;
					}
					break;
				case DIR_Y_N://y -
					y --;
					if(y - 1 <= yMin)
					{
						yMin ++;
						nDir = DIR_X_P;
					}
					break;
				}
			}
			if(m_nSequence == 1)
			{
				nStep = -1;
				nSeq = m_nTitleSize * m_nTitleSize / 2 - 1;
				m_nOffset = m_nTitleSize * m_nTitleSize / 2;
			}
			else
				break;
		}
		break;	
	case 2://Checker	
	case 3://Checker Reverse
		while (nSeq < m_nTitleSize * m_nTitleSize) 
		{	
			pData = (float*)((byte*)lr.pBits + lr.Pitch * y);	

			pData[x] = nSeq / 2;

			if(nSeq % 2)
			{
				if(m_nSequence == 2)
					pData[x] += m_nTitleSize * m_nTitleSize / 2;											
				else //3
					pData[x] = m_nTitleSize * m_nTitleSize - 1 - pData[x];
			}

			nSeq += nStep;			

			switch(nDir) 
			{
			case DIR_X_P: //x +
				x ++;
				if(x + 1 >= xMax)
				{
					xMax --;
					nDir = DIR_Y_P;
				}
				break;
			case DIR_Y_P://y +
				y ++;
				if(y + 1 >= yMax)
				{
					yMax --;
					nDir = DIR_X_N;
				}
				break;
			case DIR_X_N:// x -
				x --;
				if(x - 1 <= xMin)
				{
					xMin ++;
					nDir = DIR_Y_N;
				}
				break;
			case DIR_Y_N://y -
				y --;
				if(y - 1 <= yMin)
				{
					yMin ++;
					nDir = DIR_X_P;
				}
				break;
			}
		}
		break;
	case 4://Bidir Checker
	case 5://Bidir Checker Rev
		//
		for( int i = 0; i < 2 ; i ++)
		{

			while (nSeq < m_nTitleSize * m_nTitleSize / 2  && nSeq > -1) 
			{	
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);	

				pData[x] = nSeq / 2;


				if((nSeq % 2) && i == 0 || (nSeq % 2) == 0 && i == 1)
				{
					if(m_nSequence == 4)
						pData[x] += m_nTitleSize * m_nTitleSize / 4;											
					else //5
						pData[x] = m_nTitleSize * m_nTitleSize / 2 - 1 - pData[x];
				}

				nSeq += nStep;			

				switch(nDir) 
				{
				case DIR_X_P: //x +
					x ++;
					if(x + 1 >= xMax)
					{
						xMax --;
						nDir = DIR_Y_P;
					}
					break;
				case DIR_Y_P://y +
					y ++;
					if(y + 1 >= yMax)
					{
						yMax --;
						nDir = DIR_X_N;
					}
					break;
				case DIR_X_N:// x -
					x --;
					if(x - 1 <= xMin)
					{
						xMin ++;
						nDir = DIR_Y_N;
					}
					break;
				case DIR_Y_N://y -
					y --;
					if(y - 1 <= yMin)
					{
						yMin ++;
						nDir = DIR_X_P;
					}
					break;
				}														
			}
			nStep = -1;
			nSeq = m_nTitleSize * m_nTitleSize / 2 - 1;
			m_nOffset = m_nTitleSize * m_nTitleSize / 2;
		}
		break;
	}

	pTex->UnlockRect(0);
}

void CMatrixWipe::BottomRightSpiral()
{
	ASSERT(m_pTexture);
	LPDIRECT3DTEXTURE9 pTex = m_pTexture->GetTexture();
	ASSERT(pTex);
	D3DLOCKED_RECT lr;
	HRESULT hr = pTex->LockRect(0,&lr,NULL,0);
	ASSERT(SUCCEEDED(hr));
	float * pData = NULL; 

	int nSeq = 0;
	int nDir = DIR_Y_N;
	int x = m_nTitleSize - 1,y = m_nTitleSize - 1;				
	int xMax = m_nTitleSize - 1,yMax = m_nTitleSize,xMin = -1,yMin = -1;
	int nStep = 1;
	switch(m_nSequence)
	{
	case 0://Normal
	case 1://BirDirection				
		for(int i = 0; i < 2;i ++)
		{			
			while ( m_nSequence == 0 && nSeq < m_nTitleSize * m_nTitleSize
				|| m_nSequence == 1 && nSeq < m_nTitleSize * m_nTitleSize / 2 && nSeq > -1) 
			{	
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);	

				pData[x] = nSeq;

				nSeq += nStep;	

				switch(nDir) 
				{
				case DIR_Y_N://y -
					y --;
					if(y - 1 <= yMin)
					{
						yMin ++;
						nDir = DIR_X_N;
					}
					break;
				case DIR_X_N:// x -
					x --;
					if(x - 1 <= xMin)
					{
						xMin ++;
						nDir = DIR_Y_P;
					}
					break;
				case DIR_Y_P://y +
					y++;
					if(y + 1 >= yMax)
					{
						yMax --;
						nDir = DIR_X_P;
					}
					break;		
				case DIR_X_P: //x +
					x ++;
					if(x + 1 >= xMax)
					{
						xMax --;
						nDir = DIR_Y_N;
					}
					break;
				}
			}
			if(m_nSequence == 1)
			{
				nStep = -1;
				nSeq = m_nTitleSize * m_nTitleSize / 2 - 1;
				m_nOffset = m_nTitleSize * m_nTitleSize / 2;
			}
			else
				break;
		}
		break;	
	case 2://Checker	
	case 3://Checker Reverse
		while (nSeq < m_nTitleSize * m_nTitleSize) 
		{	
			pData = (float*)((byte*)lr.pBits + lr.Pitch * y);	

			pData[x] = nSeq / 2;

			if(nSeq % 2)
			{
				if(m_nSequence == 2)
					pData[x] += m_nTitleSize * m_nTitleSize / 2;											
				else //3
					pData[x] = m_nTitleSize * m_nTitleSize - 1 - pData[x];
			}

			nSeq += nStep;			

			switch(nDir) 
			{
			case DIR_Y_N://y -
				y --;
				if(y - 1 <= yMin)
				{
					yMin ++;
					nDir = DIR_X_N;
				}
				break;
			case DIR_X_N:// x -
				x --;
				if(x - 1 <= xMin)
				{
					xMin ++;
					nDir = DIR_Y_P;
				}
				break;
			case DIR_Y_P://y +
				y++;
				if(y + 1 >= yMax)
				{
					yMax --;
					nDir = DIR_X_P;
				}
				break;		
			case DIR_X_P: //x +
				x ++;
				if(x + 1 >= xMax)
				{
					xMax --;
					nDir = DIR_Y_N;
				}
				break;
			}
		}
		break;
	case 4://Bidir Checker
	case 5://Bidir Checker Rev
		//
		for( int i = 0; i < 2 ; i ++)
		{

			while (nSeq < m_nTitleSize * m_nTitleSize / 2  && nSeq > -1) 
			{	
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);	

				pData[x] = nSeq / 2;


				if((nSeq % 2) && i == 0 || (nSeq % 2) == 0 && i == 1)
				{
					if(m_nSequence == 4)
						pData[x] += m_nTitleSize * m_nTitleSize / 4;											
					else //5
						pData[x] = m_nTitleSize * m_nTitleSize / 2 - 1 - pData[x];
				}

				nSeq += nStep;			

				switch(nDir) 
				{
				case DIR_Y_N://y -
					y --;
					if(y - 1 <= yMin)
					{
						yMin ++;
						nDir = DIR_X_N;
					}
					break;
				case DIR_X_N:// x -
					x --;
					if(x - 1 <= xMin)
					{
						xMin ++;
						nDir = DIR_Y_P;
					}
					break;
				case DIR_Y_P://y +
					y++;
					if(y + 1 >= yMax)
					{
						yMax --;
						nDir = DIR_X_P;
					}
					break;		
				case DIR_X_P: //x +
					x ++;
					if(x + 1 >= xMax)
					{
						xMax --;
						nDir = DIR_Y_N;
					}
					break;
				}													
			}
			nStep = -1;
			nSeq = m_nTitleSize * m_nTitleSize / 2 - 1;
			m_nOffset = m_nTitleSize * m_nTitleSize / 2;
		}
		break;
	}				


	pTex->UnlockRect(0);
}

void CMatrixWipe::DiagonalTopLeft()
{
	ASSERT(m_pTexture);
	LPDIRECT3DTEXTURE9 pTex = m_pTexture->GetTexture();
	ASSERT(pTex);
	D3DLOCKED_RECT lr;
	HRESULT hr = pTex->LockRect(0,&lr,NULL,0);
	ASSERT(SUCCEEDED(hr));
	float * pData = NULL;

	int nSeq = 0;
	int nDir = DIR_X_P;
	int x = 0 ,y =  0 ;					
	int xMax = m_nTitleSize - 1,yMax = m_nTitleSize,xMin = -1,yMin = -1;

	switch(m_nSequence) 
	{
	case 0://normal		
		while (nSeq < m_nTitleSize * m_nTitleSize) 
		{	
			pData = (float*)((byte*)lr.pBits + lr.Pitch * y);
			if(m_nPattern == 4)
				pData[x] = nSeq ++;
			else if(m_nPattern == 5)
				pData[m_nTitleSize - 1 - x] = nSeq ++;

			if(nDir == DIR_X_P)
			{
				x ++;
				y --;
			}
			else
			{
				x --;
				y ++;
			}
			if(x < 0 || x >= m_nTitleSize || y < 0 || y >= m_nTitleSize)
			{
				if(nDir == DIR_X_P)
				{
					if(x >= m_nTitleSize)
					{
						x --;
						y += 2;
					}
					else
						y ++;
				}
				else
				{
					if(y >= m_nTitleSize)
					{
						y --;
						x += 2;
					}
					else
						x ++;
				}
				nDir = (nDir == DIR_X_P ? DIR_Y_P :DIR_X_P);				
			}					
		}

		break;	
	case 1: //Bidirection
		m_nOffset = m_nTitleSize * m_nTitleSize / 2;
		//Left Top
		while (nSeq < m_nTitleSize * m_nTitleSize / 2) 
		{	
			pData = (float*)((byte*)lr.pBits + lr.Pitch * y);
			if(m_nPattern == 4)
				pData[x] = nSeq ++;
			else if(m_nPattern == 5)
				pData[m_nTitleSize - 1 - x] = nSeq ++;

			if(nDir == DIR_X_P)
			{
				x ++;
				y --;
			}
			else
			{
				x --;
				y ++;
			}
			if(x < 0 || x >= m_nTitleSize || y < 0 || y >= m_nTitleSize)
			{
				if(nDir == DIR_X_P)
				{
					if(x >= m_nTitleSize)
					{
						x --;
						y += 2;
					}
					else
						y ++;
				}
				else
				{
					if(y >= m_nTitleSize)
					{
						y --;
						x += 2;
					}
					else
						x ++;
				}
				nDir = (nDir == DIR_X_P ? DIR_Y_P :DIR_X_P);				
			}					
		}
		//Right Bottom
		nSeq = 0;
		x = m_nTitleSize - 1;
		y = m_nTitleSize - 1;
		nDir = DIR_X_N;
		while (nSeq < m_nTitleSize * m_nTitleSize / 2) 
		{	
			pData = (float*)((byte*)lr.pBits + lr.Pitch * y);
			if(m_nPattern == 4)
				pData[x] = nSeq ++;
			else if(m_nPattern == 5)
				pData[m_nTitleSize - 1 - x] = nSeq ++;

			if(nDir == DIR_X_N)
			{
				x --;
				y ++;
			}
			else
			{
				x ++;
				y --;
			}
			if(x < 0 || x >= m_nTitleSize || y < 0 || y >= m_nTitleSize)
			{
				if(nDir == DIR_X_N)
				{
					if(x < 0)
					{
						x ++;
						y -= 2;
					}
					else
						y --;
				}
				else
				{
					if(y < -1)
					{
						y ++;
						x -= 2;
					}
					else
						x --;
				}
				nDir = (nDir == DIR_X_N ? DIR_Y_N :DIR_X_N);				
			}					
		}
		break;
	case 2://Checker
		{			
			//方向 ： 右上
			int nOldX = 0,nOldY = 0;
			while (nSeq < m_nTitleSize * m_nTitleSize / 2) 
			{	
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);
				if(m_nPattern == 4)
					pData[x] = nSeq ++;
				else if(m_nPattern == 5)
					pData[m_nTitleSize - 1 - x] = nSeq ++;

				x ++;
				y --;

				if(x >= m_nTitleSize || y < 0)
				{						
					nOldY += 2;
					if(nOldY >= m_nTitleSize)
					{					
						nOldX += 2 + (nOldY == m_nTitleSize ? -1 : 0);
						nOldY = m_nTitleSize - 1;						
					}
					x = nOldX;
					y = nOldY;
				}	
			}	

			//方向 ： 左下
			nOldX = 1;
			nOldY = 0;
			x = nOldX;
			y = nOldY;
			while (nSeq < m_nTitleSize * m_nTitleSize) 
			{	
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);
				if(m_nPattern == 4)
					pData[x] = nSeq ++;
				else if(m_nPattern == 5)
					pData[m_nTitleSize - 1 - x] = nSeq ++;

				x --;
				y ++;

				if(y >= m_nTitleSize || x < 0)
				{						
					nOldX += 2;
					if(nOldX >= m_nTitleSize)
					{					
						nOldY += 2 + + (nOldX == m_nTitleSize ? -1 : 0);
						nOldX = m_nTitleSize - 1;						
					}
					x = nOldX;
					y = nOldY;
				}	
			}			
		}
		break;	
	case 3://Checker Reverse
		{			
			//方向 ： 左上
			int nOldX = 0,nOldY = 0;
			while (nSeq < m_nTitleSize * m_nTitleSize / 2) 
			{	
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);
				if(m_nPattern == 4)
					pData[x] = nSeq ++;
				else if(m_nPattern == 5)
					pData[m_nTitleSize - 1 - x] = nSeq ++;

				x ++;
				y --;

				if(x >= m_nTitleSize || y < 0)
				{						
					nOldY += 2;
					if(nOldY >= m_nTitleSize)
					{					
						nOldX += 2 + (nOldY == m_nTitleSize ? -1 : 0);
						nOldY = m_nTitleSize - 1;						
					}
					x = nOldX;
					y = nOldY;
				}	
			}	
			nOldX -= 2 + (nOldX == m_nTitleSize ? 0 : 1);
			x = nOldX;
			//方向 ： 右上			
			while (nSeq < m_nTitleSize * m_nTitleSize) 
			{	
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);
				if(m_nPattern == 4)
					pData[x] = nSeq ++;
				else if(m_nPattern == 5)
					pData[m_nTitleSize - 1 - x] = nSeq ++;

				x ++;
				y --;

				if(x >= m_nTitleSize || y < 0)
				{						
					nOldX -= 2;
					if(nOldX < 0)
					{					
						nOldY -= 2 + (nOldX == -1 ? -1 : 0);
						nOldX = 0;						
					}
					x = nOldX;
					y = nOldY;
				}	
			}			
		}
		break;	
	case 4://Bidir Checker
		{	
			m_nOffset = m_nTitleSize * m_nTitleSize / 2;
			//上三角
			//方向 ： 右上
			int nOldX = 0,nOldY = 0;
			while (nSeq < m_nTitleSize * m_nTitleSize / 4) 
			{	
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);
				if(m_nPattern == 4)
					pData[x] = nSeq ++;
				else if(m_nPattern == 5)
					pData[m_nTitleSize - 1 - x] = nSeq ++;

				x ++;
				y --;

				if(x >= m_nTitleSize || y < 0)
				{						
					nOldY += 2;
					if(nOldY >= m_nTitleSize)
					{					
						nOldX += 2 + (nOldY == m_nTitleSize ? -1 : 0);
						nOldY = m_nTitleSize - 1;						
					}
					x = nOldX;
					y = nOldY;
				}	
			}	

			//方向 ： 左下
			nOldX = 1;
			nOldY = 0;
			x = nOldX;
			y = nOldY;
			while (nSeq < m_nTitleSize * m_nTitleSize / 2) 
			{	
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);
				if(m_nPattern == 4)
					pData[x] = nSeq ++;
				else if(m_nPattern == 5)
					pData[m_nTitleSize - 1 - x] = nSeq ++;

				x --;
				y ++;

				if(y >= m_nTitleSize || x < 0)
				{						
					nOldX += 2;
					if(nOldX >= m_nTitleSize)
					{					
						nOldY += 2 + (nOldX == m_nTitleSize ? -1 : 0);
						nOldX = m_nTitleSize - 1;						
					}
					x = nOldX;
					y = nOldY;
				}	
			}	
			//下三角
			nOldX = m_nTitleSize - 1;
			nOldY = m_nTitleSize - 1;
			x = nOldX;
			y = nOldY;

			nSeq = 0;
			while (nSeq < m_nTitleSize * m_nTitleSize / 4) 
			{	
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);
				if(m_nPattern == 4)
					pData[x] = nSeq ++;
				else if(m_nPattern == 5)
					pData[m_nTitleSize - 1 - x] = nSeq ++;

				x --;
				y ++;

				if(y >= m_nTitleSize || x < 0)
				{						
					nOldY -= 2;
					if(nOldY < 0)
					{					
						nOldX -= 2 + (nOldY == -2 ? -1 : 0);
						nOldY = 0;						
					}
					x = nOldX;
					y = nOldY;
				}
			}
			nOldX = m_nTitleSize - 1 - 1;
			nOldY = m_nTitleSize - 1;
			x = nOldX;
			y = nOldY;
			while (nSeq < m_nTitleSize * m_nTitleSize / 2) 
			{	
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);
				if(m_nPattern == 4)
					pData[x] = nSeq ++;
				else if(m_nPattern == 5)
					pData[m_nTitleSize - 1 - x] = nSeq ++;

				x ++;
				y --;

				if(x >= m_nTitleSize || y < 0)
				{						
					nOldX -= 2;
					if(nOldX < 0)
					{					
						nOldY -= 2 + (nOldX == -2 ? -1 : 0);
						nOldX = 0;						
					}
					x = nOldX;
					y = nOldY;
				}	
			}	
		}
		break;
	case 5://Checker Reverse
		{	
			m_nOffset = m_nTitleSize * m_nTitleSize / 2;
			//上三角
			//方向 ： 左上
			int nOldX = 0,nOldY = 0;
			while (nSeq < m_nTitleSize * m_nTitleSize / 4) 
			{	
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);
				if(m_nPattern == 4)
					pData[x] = nSeq ++;
				else if(m_nPattern == 5)
					pData[m_nTitleSize - 1 - x] = nSeq ++;

				x ++;
				y --;

				if(x >= m_nTitleSize || y < 0)
				{						
					nOldY += 2;
					if(nOldY >= m_nTitleSize)
					{					
						nOldX += 2 + (nOldY == m_nTitleSize ? -1 : 0);
						nOldY = m_nTitleSize - 1;						
					}
					x = nOldX;
					y = nOldY;
				}	
			}	
			nOldX = 1;
			nOldY = 0;
			x = nOldX;
			y = nOldY;
			//方向 ： 右上			
			nSeq = m_nTitleSize * m_nTitleSize / 2 - 1;
			while (nSeq >=  m_nTitleSize * m_nTitleSize / 4) 
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);
				if(m_nPattern == 4)
					pData[x] = nSeq --;
				else if(m_nPattern == 5)
					pData[m_nTitleSize - 1 - x] = nSeq --;

				x --;
				y ++;

				if(y >= m_nTitleSize || x < 0)
				{						
					nOldX += 2;
					if(nOldX >= m_nTitleSize)
					{					
						nOldY += 2 + (nOldX == -2 ? -1 : 0);
						nOldX = m_nTitleSize - 1;						
					}
					x = nOldX;
					y = nOldY;
				}
			}	
			//下三角
			nOldX = m_nTitleSize - 1;
			nOldY = m_nTitleSize - 1;
			x = nOldX;
			y = nOldY;

			nSeq = 0;
			while (nSeq < m_nTitleSize * m_nTitleSize / 4) 
			{	
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);
				if(m_nPattern == 4)
					pData[x] = nSeq ++;
				else if(m_nPattern == 5)
					pData[m_nTitleSize - 1 - x] = nSeq ++;					

				x --;
				y ++;

				if(y >= m_nTitleSize || x < 0)
				{						
					nOldY -= 2;
					if(nOldY < 0)
					{					
						nOldX -= 2 + (nOldY == -2 ? -1 : 0);
						nOldY = 0;						
					}
					x = nOldX;
					y = nOldY;
				}
			}
			nOldX = m_nTitleSize - 1 - 1;
			nOldY = m_nTitleSize - 1;

			x = nOldX;
			y = nOldY;
			nSeq = m_nTitleSize * m_nTitleSize / 2 - 1;
			while (nSeq >=  m_nTitleSize * m_nTitleSize / 4) 
			{	
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);
				if(m_nPattern == 4)
					pData[x] = nSeq --;
				else if(m_nPattern == 5)
					pData[m_nTitleSize - 1 - x] = nSeq --;

				x ++;
				y --;

				if(x >= m_nTitleSize || y < 0)
				{						
					nOldX -= 2;
					if(nOldX < 0)
					{					
						nOldY -= 2 + (nOldX == -1 ? -1 : 0);
						nOldX = 0;						
					}
					x = nOldX;
					y = nOldY;
				}	
			}	
		}
		break;	
	}

	pTex->UnlockRect(0);
}

void CMatrixWipe::RightVertical()
{
	ASSERT(m_pTexture);
	LPDIRECT3DTEXTURE9 pTex = m_pTexture->GetTexture();
	ASSERT(pTex);
	D3DLOCKED_RECT lr;
	HRESULT hr = pTex->LockRect(0,&lr,NULL,0);
	ASSERT(SUCCEEDED(hr));
	float * pData = NULL; 

	int nSeq = 0;
	int nDir = DIR_Y_N;
	int x = m_nTitleSize -1 ,y =  m_nTitleSize -1 ;					
	int xMax = m_nTitleSize - 1,yMax = m_nTitleSize,xMin = -1,yMin = -1;

	switch(m_nSequence) 
	{
	case 0://normal
	case 1://BiDirection
		{
			while (x > -1) 
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
				pData[x] = nSeq ++;

				if(nDir == DIR_Y_P)
				{
					y ++;
					if(y >= m_nTitleSize)
					{
						nDir = DIR_Y_N;
						y --;
						x --;
					}
				}
				else //DIR_Y_N
				{
					y --;
					if(y <= -1)
					{
						nDir = DIR_Y_P;
						y ++;
						x --;
					}
				}
			}
			if(m_nSequence == 1)
			{
				m_nOffset = ceil(m_nTitleSize / 2.0f) * m_nTitleSize;

				for(int x = 0; x < m_nTitleSize / 2; x ++)
				{					
					for(int y = 0; y < m_nTitleSize ;y ++)
					{
						pData = (float*)((byte*)lr.pBits + lr.Pitch * y);
						pData[x] = pData[m_nTitleSize - 1 - x];
					}
				}
			}
		}
		break;
	case 2://Checker
		{
			for(int i = 0; i < 2;i ++)
			{
				y = m_nTitleSize - 1 - i;
				x = m_nTitleSize - 1;
				while (x > -1) 
				{
					pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
					pData[x] = nSeq ++;

					if(nDir == DIR_Y_P)
					{
						y += 2;
						if(y >= m_nTitleSize)
						{
							nDir = DIR_Y_N;
							y -= (y == m_nTitleSize ? 1 : 3);
							x --;
						}
					}
					else //DIR_Y_N
					{
						y -= 2;
						if(y <= -1)
						{
							nDir = DIR_Y_P;
							y += (y == -1 ? 1 : 3);
							x --;
						}
					}
				}
			}
		}
		break;
	case 3://Checker Reverse
		{						
			while (x > -1) 
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
				pData[x] = nSeq ++;

				if(nDir == DIR_Y_P)
				{
					y += 2;
					if(y >= m_nTitleSize)
					{
						nDir = DIR_Y_N;
						y -= (y == m_nTitleSize ? 1 : 3);
						x --;
					}
				}
				else //DIR_Y_N
				{
					y -= 2;
					if(y <= -1)
					{
						nDir = DIR_Y_P;
						y += (y == -1 ? 1 : 3);
						x --;
					}
				}
			}
			x ++;
			while (x < m_nTitleSize) 
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
				pData[x] = nSeq ++;

				if(nDir == DIR_Y_P)
				{
					y += 2;
					if(y >= m_nTitleSize)
					{
						nDir = DIR_Y_N;
						y -= (y == m_nTitleSize ? 1 : 3);
						x ++;
					}
				}
				else //DIR_Y_N
				{
					y -= 2;
					if(y <= -1)
					{
						nDir = DIR_Y_P;
						y += (y == -1 ? 1 : 3);
						x ++;
					}
				}
			}
		}
		break;
	case 4://Bidir Checker
		{
			m_nOffset = ceil(m_nTitleSize / 2.0f) * m_nTitleSize;

			for(int i = 0; i < 2;i ++)
			{
				y = m_nTitleSize - 1 - i;
				x = m_nTitleSize - 1;

				while (x >= m_nTitleSize / 2) 
				{
					pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
					pData[x] = nSeq ++;

					if(nDir == DIR_Y_P)
					{
						y += 2;
						if(y >= m_nTitleSize)
						{
							nDir = DIR_Y_N;
							y -= (y == m_nTitleSize ? 1 : 3);
							x --;
						}
					}
					else //DIR_Y_N
					{
						y -= 2;
						if(y <= -1)
						{
							nDir = DIR_Y_P;
							y += (y == -1 ? 1 : 3);
							x --;
						}
					}
				}
			}
			nSeq = 0;
			for(int i = 0; i < 2;i ++)
			{
				y = m_nTitleSize - 1 - (1- i);							
				x = 0 ;
				while (x < m_nTitleSize / 2) 
				{
					pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
					pData[x] = nSeq ++;

					if(nDir == DIR_Y_P)
					{
						y += 2;
						if(y >= m_nTitleSize)
						{
							nDir = DIR_Y_N;
							y -= (y == m_nTitleSize ? 1 : 3);
							x ++;
						}
					}
					else //DIR_Y_N
					{
						y -= 2;
						if(y <= -1)
						{
							nDir = DIR_Y_P;
							y += (y == -1 ? 1 : 3);
							x ++;
						}
					}
				}
			}
		}
		break;
	case 5://Bidir Checker Rev
		{
			m_nOffset = ceil(m_nTitleSize / 2.0f) * m_nTitleSize;
			/////////////////////////Down/////////////////////////////////////////////////						
			while (x >= m_nTitleSize / 2) 
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
				pData[x] = nSeq ++;

				if(nDir == DIR_Y_P)
				{
					y += 2;
					if(y >= m_nTitleSize)
					{
						nDir = DIR_Y_N;
						y -= (y == m_nTitleSize ? 1 : 3);
						x --;
					}
				}
				else //DIR_Y_N
				{
					y -= 2;
					if(y <= -1)
					{
						nDir = DIR_Y_P;
						y += (y == -1 ? 1 : 3);
						x --;
					}
				}
			}
			x ++;
			while (x < m_nTitleSize) 
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
				pData[x] = nSeq ++;

				if(nDir == DIR_Y_P)
				{
					y += 2;
					if(y >= m_nTitleSize)
					{
						nDir = DIR_Y_N;
						y -= (y == m_nTitleSize ? 1 : 3);
						x ++;
					}
				}
				else //DIR_Y_N
				{
					y -= 2;
					if(y <= -1)
					{
						nDir = DIR_Y_P;
						y += (y == -1 ? 1 : 3);
						x ++;
					}
				}
			}
			/////////////////////////Up/////////////////////////////////////////////////						
			nSeq = 0;

			y = m_nTitleSize - 1 - 1;							
			x = 0 ;
			while (x < m_nTitleSize / 2) 
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
				pData[x] = nSeq ++;

				if(nDir == DIR_Y_P)
				{
					y += 2;
					if(y >= m_nTitleSize)
					{
						nDir = DIR_Y_N;
						y -= (y == m_nTitleSize ? 1 : 3);
						x ++;
					}
				}
				else //DIR_Y_N
				{
					y -= 2;
					if(y <= -1)
					{
						nDir = DIR_Y_P;
						y += (y == -1 ? 1 : 3);
						x ++;
					}
				}
			}	
			x --;
			while (x > -1) 
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
				pData[x] = nSeq ++;

				if(nDir == DIR_Y_P)
				{
					y += 2;
					if(y >= m_nTitleSize)
					{
						nDir = DIR_Y_N;
						y -= (y == m_nTitleSize ? 1 : 3);
						x --;
					}
				}
				else //DIR_Y_N
				{
					y -= 2;
					if(y <= -1)
					{
						nDir = DIR_Y_P;
						y += (y == -1 ? 1 : 3);
						x --;
					}
				}
			}		
		}
		break;
	default:
		assert(-1);
		break;
	}
	pTex->UnlockRect(0);
}

void CMatrixWipe::LeftVertical()
{
	ASSERT(m_pTexture);
	LPDIRECT3DTEXTURE9 pTexture = m_pTexture->GetTexture();
	ASSERT(pTexture);
	D3DLOCKED_RECT lr;
	HRESULT hr = pTexture->LockRect(0,&lr,NULL,0);
	ASSERT(SUCCEEDED(hr));
	float * pData = NULL;

	int nSeq = 0;
	int nDir = DIR_Y_P;
	int x = 0,y = 0;					
	int xMax = m_nTitleSize - 1,yMax = m_nTitleSize,xMin = -1,yMin = -1;

	switch(m_nSequence) 
	{
	case 0://normal
	case 1://BiDirection
		{
			while (x < m_nTitleSize) 
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
				pData[x] = nSeq ++;

				if(nDir == DIR_Y_P)
				{
					y ++;
					if(y >= m_nTitleSize)
					{
						nDir = DIR_Y_N;
						y --;
						x ++;
					}
				}
				else //DIR_Y_N
				{
					y --;
					if(y <= -1)
					{
						nDir = DIR_Y_P;
						y ++;
						x ++;
					}
				}
			}
			if(m_nSequence == 1)
			{
				m_nOffset = ceil(m_nTitleSize / 2.0f) * m_nTitleSize;

				for(int x = 0; x < m_nTitleSize / 2; x ++)
				{					
					for(int y = 0; y < m_nTitleSize ;y ++)
					{
						pData = (float*)((byte*)lr.pBits + lr.Pitch * y);
						pData[m_nTitleSize - 1 - x] = pData[x];
					}
				}
			}
		}
		break;
	case 2://Checker
		{
			for(int i = 0; i < 2;i ++)
			{
				y = i;
				x = 0;
				while (x < m_nTitleSize) 
				{
					pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
					pData[x] = nSeq ++;

					if(nDir == DIR_Y_P)
					{
						y += 2;
						if(y >= m_nTitleSize)
						{
							nDir = DIR_Y_N;
							y -= (y == m_nTitleSize ? 1 : 3);
							x ++;
						}
					}
					else //DIR_Y_N
					{
						y -= 2;
						if(y <= -1)
						{
							nDir = DIR_Y_P;
							y += (y == -1 ? 1 : 3);
							x ++;
						}
					}
				}
			}
		}
		break;
	case 3://Checker Reverse
		{						
			while (x < m_nTitleSize) 
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
				pData[x] = nSeq ++;

				if(nDir == DIR_Y_P)
				{
					y += 2;
					if(y >= m_nTitleSize)
					{
						nDir = DIR_Y_N;
						y -= (y == m_nTitleSize ? 1 : 3);
						x ++;
					}
				}
				else //DIR_Y_N
				{
					y -= 2;
					if(y <= -1)
					{
						nDir = DIR_Y_P;
						y += (y == -1 ? 1 : 3);
						x ++;
					}
				}
			}
			x --;
			while (x > -1) 
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
				pData[x] = nSeq ++;

				if(nDir == DIR_Y_P)
				{
					y += 2;
					if(y >= m_nTitleSize)
					{
						nDir = DIR_Y_N;
						y -= (y == m_nTitleSize ? 1 : 3);
						x --;
					}
				}
				else //DIR_Y_N
				{
					y -= 2;
					if(y <= -1)
					{
						nDir = DIR_Y_P;
						y += (y == -1 ? 1 : 3);
						x --;
					}
				}
			}
		}
		break;
	case 4://Bidir Checker
		{
			m_nOffset = ceil(m_nTitleSize / 2.0f) * m_nTitleSize;

			for(int i = 0; i < 2;i ++)
			{
				y = i;
				x = 0;
				while (x < m_nTitleSize / 2) 
				{
					pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
					pData[x] = nSeq ++;

					if(nDir == DIR_Y_P)
					{
						y += 2;
						if(y >= m_nTitleSize)
						{
							nDir = DIR_Y_N;
							y -= (y == m_nTitleSize ? 1 : 3);
							x ++;
						}
					}
					else //DIR_Y_N
					{
						y -= 2;
						if(y <= -1)
						{
							nDir = DIR_Y_P;
							y += (y == -1 ? 1 : 3);
							x ++;
						}
					}
				}
			}
			nSeq = 0;
			for(int i = 0; i < 2;i ++)
			{
				y = 1- i;							
				x = m_nTitleSize - 1 ;
				while (x >= m_nTitleSize / 2) 
				{
					pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
					pData[x] = nSeq ++;

					if(nDir == DIR_Y_P)
					{
						y += 2;
						if(y >= m_nTitleSize)
						{
							nDir = DIR_Y_N;
							y -= (y == m_nTitleSize ? 1 : 3);
							x --;
						}
					}
					else //DIR_Y_N
					{
						y -= 2;
						if(y <= -1)
						{
							nDir = DIR_Y_P;
							y += (y == -1 ? 1 : 3);
							x --;
						}
					}
				}
			}
		}
		break;
	case 5://Bidir Checker Rev
		{
			m_nOffset = ceil(m_nTitleSize / 2.0f) * m_nTitleSize;

			while (x < m_nTitleSize / 2) 
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
				pData[x] = nSeq ++;

				if(nDir == DIR_Y_P)
				{
					y += 2;
					if(y >= m_nTitleSize)
					{
						nDir = DIR_Y_N;
						y -= (y == m_nTitleSize ? 1 : 3);
						x ++;
					}
				}
				else //DIR_Y_N
				{
					y -= 2;
					if(y <= -1)
					{
						nDir = DIR_Y_P;
						y += (y == -1 ? 1 : 3);
						x ++;
					}
				}
			}
			x --;
			while (x > -1) 
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
				pData[x] = nSeq ++;

				if(nDir == DIR_Y_P)
				{
					y += 2;
					if(y >= m_nTitleSize)
					{
						nDir = DIR_Y_N;
						y -= (y == m_nTitleSize ? 1 : 3);
						x --;
					}
				}
				else //DIR_Y_N
				{
					y -= 2;
					if(y <= -1)
					{
						nDir = DIR_Y_P;
						y += (y == -1 ? 1 : 3);
						x --;
					}
				}
			}
			/////////////////////////Down/////////////////////////////////////////////////						
			nSeq = 0;

			y = 1;							
			x = m_nTitleSize - 1 ;
			while (x >= m_nTitleSize / 2) 
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
				pData[x] = nSeq ++;

				if(nDir == DIR_Y_P)
				{
					y += 2;
					if(y >= m_nTitleSize)
					{
						nDir = DIR_Y_N;
						y -= (y == m_nTitleSize ? 1 : 3);
						x --;
					}
				}
				else //DIR_Y_N
				{
					y -= 2;
					if(y <= -1)
					{
						nDir = DIR_Y_P;
						y += (y == -1 ? 1 : 3);
						x --;
					}
				}
			}	
			x ++;
			while (x < m_nTitleSize) 
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
				pData[x] = nSeq ++;

				if(nDir == DIR_Y_P)
				{
					y += 2;
					if(y >= m_nTitleSize)
					{
						nDir = DIR_Y_N;
						y -= (y == m_nTitleSize ? 1 : 3);
						x ++;
					}
				}
				else //DIR_Y_N
				{
					y -= 2;
					if(y <= -1)
					{
						nDir = DIR_Y_P;
						y += (y == -1 ? 1 : 3);
						x ++;
					}
				}
			}		
		}
		break;
	default:
		assert(-1);
		break;
	}
	pTexture->UnlockRect(0);
}

void CMatrixWipe::BottomHorizontal()
{
	ASSERT(m_pTexture);
	LPDIRECT3DTEXTURE9 pTexture = m_pTexture->GetTexture();
	ASSERT(pTexture);
	D3DLOCKED_RECT lr;
	HRESULT hr = pTexture->LockRect(0,&lr,NULL,0);
	ASSERT(SUCCEEDED(hr));
	float * pData = NULL;

	int nSeq = 0;
	int nDir = DIR_X_N;
	int x = m_nTitleSize -1 ,y =  m_nTitleSize -1 ;					
	int xMax = m_nTitleSize - 1,yMax = m_nTitleSize,xMin = -1,yMin = -1;

	switch(m_nSequence) 
	{
	case 0://normal
	case 1://BiDirection
		{
			while (y > -1) 
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
				pData[x] = nSeq ++;

				if(nDir == DIR_X_P)
				{
					x ++;
					if(x >= m_nTitleSize)
					{
						nDir = DIR_X_N;
						x --;
						y --;
					}
				}
				else //DIR_X_N
				{
					x --;
					if(x <= -1)
					{
						nDir = DIR_X_P;
						x ++;
						y --;
					}
				}
			}
			if(m_nSequence == 1)
			{
				m_nOffset = ceil(m_nTitleSize / 2.0f) * m_nTitleSize;

				for(int y = 0; y < m_nTitleSize / 2; y ++)
					memcpy_s((byte*)lr.pBits + lr.Pitch * y,m_nTitleSize * sizeof(float),
					(byte*)lr.pBits + lr.Pitch * (m_nTitleSize - 1 - y),m_nTitleSize * sizeof(float));
			}
		}
		break;
	case 2://Checker
		{
			for(int i = 0; i < 2;i ++)
			{
				x = m_nTitleSize - 1 - i;
				y = m_nTitleSize - 1;
				while (y > -1) 
				{
					pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
					pData[x] = nSeq ++;

					if(nDir == DIR_X_P)
					{
						x += 2;
						if(x >= m_nTitleSize)
						{
							nDir = DIR_X_N;
							x -= (x == m_nTitleSize ? 1 : 3);
							y --;
						}
					}
					else //DIR_X_N
					{
						x -= 2;
						if(x <= -1)
						{
							nDir = DIR_X_P;
							x += (x == -1 ? 1 : 3);
							y --;
						}
					}
				}
			}
		}
		break;
	case 3://Checker Reverse
		{						
			while (y > -1) 
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
				pData[x] = nSeq ++;

				if(nDir == DIR_X_P)
				{
					x += 2;
					if(x >= m_nTitleSize)
					{
						nDir = DIR_X_N;
						x -= (x == m_nTitleSize ? 1 : 3);
						y --;
					}
				}
				else //DIR_X_N
				{
					x -= 2;
					if(x <= -1)
					{
						nDir = DIR_X_P;
						x += (x == -1 ? 1 : 3);
						y --;
					}
				}
			}
			y ++;
			while (y < m_nTitleSize) 
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
				pData[x] = nSeq ++;

				if(nDir == DIR_X_P)
				{
					x += 2;
					if(x >= m_nTitleSize)
					{
						nDir = DIR_X_N;
						x -= (x == m_nTitleSize ? 1 : 3);
						y ++;
					}
				}
				else //DIR_X_N
				{
					x -= 2;
					if(x <= -1)
					{
						nDir = DIR_X_P;
						x += (x == -1 ? 1 : 3);
						y ++;
					}
				}
			}
		}
		break;
	case 4://Bidir Checker
		{
			m_nOffset = ceil(m_nTitleSize / 2.0f) * m_nTitleSize;

			for(int i = 0; i < 2;i ++)
			{
				x = m_nTitleSize - 1 - i;
				y = m_nTitleSize - 1;

				while (y >= m_nTitleSize / 2) 
				{
					pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
					pData[x] = nSeq ++;

					if(nDir == DIR_X_P)
					{
						x += 2;
						if(x >= m_nTitleSize)
						{
							nDir = DIR_X_N;
							x -= (x == m_nTitleSize ? 1 : 3);
							y --;
						}
					}
					else //DIR_X_N
					{
						x -= 2;
						if(x <= -1)
						{
							nDir = DIR_X_P;
							x += (x == -1 ? 1 : 3);
							y --;
						}
					}
				}
			}
			nSeq = 0;
			for(int i = 0; i < 2;i ++)
			{
				x = m_nTitleSize - 1 - (1- i);							
				y = 0 ;
				while (y < m_nTitleSize / 2) 
				{
					pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
					pData[x] = nSeq ++;

					if(nDir == DIR_X_P)
					{
						x += 2;
						if(x >= m_nTitleSize)
						{
							nDir = DIR_X_N;
							x -= (x == m_nTitleSize ? 1 : 3);
							y ++;
						}
					}
					else //DIR_X_N
					{
						x -= 2;
						if(x <= -1)
						{
							nDir = DIR_X_P;
							x += (x == -1 ? 1 : 3);
							y ++;
						}
					}
				}
			}
		}
		break;
	case 5://Bidir Checker Rev
		{
			m_nOffset = ceil(m_nTitleSize / 2.0f) * m_nTitleSize;
			/////////////////////////Down/////////////////////////////////////////////////						
			while (y >= m_nTitleSize / 2) 
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
				pData[x] = nSeq ++;

				if(nDir == DIR_X_P)
				{
					x += 2;
					if(x >= m_nTitleSize)
					{
						nDir = DIR_X_N;
						x -= (x == m_nTitleSize ? 1 : 3);
						y --;
					}
				}
				else //DIR_X_N
				{
					x -= 2;
					if(x <= -1)
					{
						nDir = DIR_X_P;
						x += (x == -1 ? 1 : 3);
						y --;
					}
				}
			}
			y ++;
			while (y < m_nTitleSize) 
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
				pData[x] = nSeq ++;

				if(nDir == DIR_X_P)
				{
					x += 2;
					if(x >= m_nTitleSize)
					{
						nDir = DIR_X_N;
						x -= (x == m_nTitleSize ? 1 : 3);
						y ++;
					}
				}
				else //DIR_X_N
				{
					x -= 2;
					if(x <= -1)
					{
						nDir = DIR_X_P;
						x += (x == -1 ? 1 : 3);
						y ++;
					}
				}
			}
			/////////////////////////Up/////////////////////////////////////////////////						
			nSeq = 0;

			x = m_nTitleSize - 1 - 1;							
			y = 0 ;
			while (y < m_nTitleSize / 2) 
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
				pData[x] = nSeq ++;

				if(nDir == DIR_X_P)
				{
					x += 2;
					if(x >= m_nTitleSize)
					{
						nDir = DIR_X_N;
						x -= (x == m_nTitleSize ? 1 : 3);
						y ++;
					}
				}
				else //DIR_X_N
				{
					x -= 2;
					if(x <= -1)
					{
						nDir = DIR_X_P;
						x += (x == -1 ? 1 : 3);
						y ++;
					}
				}
			}	
			y --;
			while (y > -1) 
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
				pData[x] = nSeq ++;

				if(nDir == DIR_X_P)
				{
					x += 2;
					if(x >= m_nTitleSize)
					{
						nDir = DIR_X_N;
						x -= (x == m_nTitleSize ? 1 : 3);
						y --;
					}
				}
				else //DIR_X_N
				{
					x -= 2;
					if(x <= -1)
					{
						nDir = DIR_X_P;
						x += (x == -1 ? 1 : 3);
						y --;
					}
				}
			}		
		}
		break;
	default:
		assert(-1);
		break;
	}
	pTexture->UnlockRect(0);
}

void CMatrixWipe::TopHorizontal()
{
	ASSERT(m_pTexture);
	LPDIRECT3DTEXTURE9 pTexture = m_pTexture->GetTexture();
	ASSERT(pTexture);
	D3DLOCKED_RECT lr;
	HRESULT hr = pTexture->LockRect(0,&lr,NULL,0);
	ASSERT(SUCCEEDED(hr));
	float * pData = NULL;

	int nSeq = 0;
	int nDir = DIR_X_P;
	int x = 0,y = 0;					
	int xMax = m_nTitleSize - 1,yMax = m_nTitleSize,xMin = -1,yMin = -1;

	switch(m_nSequence) 
	{
	case 0://normal
	case 1://BiDirection
		{
			while (nSeq < m_nTitleSize * m_nTitleSize) 
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
				pData[x] = nSeq ++;

				if(nDir == DIR_X_P)
				{
					x ++;
					if(x >= m_nTitleSize)
					{
						nDir = DIR_X_N;
						x --;
						y ++;
					}
				}
				else //DIR_X_N
				{
					x --;
					if(x <= -1)
					{
						nDir = DIR_X_P;
						x ++;
						y ++;
					}
				}
			}
			if(m_nSequence == 1)
			{
				m_nOffset = ceil(m_nTitleSize / 2.0f) * m_nTitleSize;

				for(int y = 0; y < m_nTitleSize / 2; y ++)
					memcpy_s((byte*)lr.pBits + lr.Pitch * (m_nTitleSize - 1 - y),m_nTitleSize * sizeof(float),
					(byte*)lr.pBits + lr.Pitch * y,m_nTitleSize * sizeof(float));
			}
		}
		break;
	case 2://Checker
		{
			for(int i = 0; i < 2;i ++)
			{
				x = i;
				y = 0;
				while (y < m_nTitleSize) 
				{
					pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
					pData[x] = nSeq ++;

					if(nDir == DIR_X_P)
					{
						x += 2;
						if(x >= m_nTitleSize)
						{
							nDir = DIR_X_N;
							x -= (x == m_nTitleSize ? 1 : 3);
							y ++;
						}
					}
					else //DIR_X_N
					{
						x -= 2;
						if(x <= -1)
						{
							nDir = DIR_X_P;
							x += (x == -1 ? 1 : 3);
							y ++;
						}
					}
				}
			}
		}
		break;
	case 3://Checker Reverse
		{						
			while (y < m_nTitleSize) 
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
				pData[x] = nSeq ++;

				if(nDir == DIR_X_P)
				{
					x += 2;
					if(x >= m_nTitleSize)
					{
						nDir = DIR_X_N;
						x -= (x == m_nTitleSize ? 1 : 3);
						y ++;
					}
				}
				else //DIR_X_N
				{
					x -= 2;
					if(x <= -1)
					{
						nDir = DIR_X_P;
						x += (x == -1 ? 1 : 3);
						y ++;
					}
				}
			}
			y --;
			while (y > -1) 
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
				pData[x] = nSeq ++;

				if(nDir == DIR_X_P)
				{
					x += 2;
					if(x >= m_nTitleSize)
					{
						nDir = DIR_X_N;
						x -= (x == m_nTitleSize ? 1 : 3);
						y --;
					}
				}
				else //DIR_X_N
				{
					x -= 2;
					if(x <= -1)
					{
						nDir = DIR_X_P;
						x += (x == -1 ? 1 : 3);
						y --;
					}
				}
			}
		}
		break;
	case 4://Bidir Checker
		{
			m_nOffset = ceil(m_nTitleSize / 2.0f) * m_nTitleSize;

			for(int i = 0; i < 2;i ++)
			{
				x = i;
				y = 0;
				while (y < m_nTitleSize / 2) 
				{
					pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
					pData[x] = nSeq ++;

					if(nDir == DIR_X_P)
					{
						x += 2;
						if(x >= m_nTitleSize)
						{
							nDir = DIR_X_N;
							x -= (x == m_nTitleSize ? 1 : 3);
							y ++;
						}
					}
					else //DIR_X_N
					{
						x -= 2;
						if(x <= -1)
						{
							nDir = DIR_X_P;
							x += (x == -1 ? 1 : 3);
							y ++;
						}
					}
				}
			}
			nSeq = 0;
			for(int i = 0; i < 2;i ++)
			{
				x = 1- i;							
				y = m_nTitleSize - 1 ;
				while (y >= m_nTitleSize / 2) 
				{
					pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
					pData[x] = nSeq ++;

					if(nDir == DIR_X_P)
					{
						x += 2;
						if(x >= m_nTitleSize)
						{
							nDir = DIR_X_N;
							x -= (x == m_nTitleSize ? 1 : 3);
							y --;
						}
					}
					else //DIR_X_N
					{
						x -= 2;
						if(x <= -1)
						{
							nDir = DIR_X_P;
							x += (x == -1 ? 1 : 3);
							y --;
						}
					}
				}
			}
		}
		break;
	case 5://Bidir Checker Rev
		{
			m_nOffset = ceil(m_nTitleSize / 2.0f) * m_nTitleSize;

			while (y < m_nTitleSize / 2) 
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
				pData[x] = nSeq ++;

				if(nDir == DIR_X_P)
				{
					x += 2;
					if(x >= m_nTitleSize)
					{
						nDir = DIR_X_N;
						x -= (x == m_nTitleSize ? 1 : 3);
						y ++;
					}
				}
				else //DIR_X_N
				{
					x -= 2;
					if(x <= -1)
					{
						nDir = DIR_X_P;
						x += (x == -1 ? 1 : 3);
						y ++;
					}
				}
			}
			y --;
			while (y > -1) 
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
				pData[x] = nSeq ++;

				if(nDir == DIR_X_P)
				{
					x += 2;
					if(x >= m_nTitleSize)
					{
						nDir = DIR_X_N;
						x -= (x == m_nTitleSize ? 1 : 3);
						y --;
					}
				}
				else //DIR_X_N
				{
					x -= 2;
					if(x <= -1)
					{
						nDir = DIR_X_P;
						x += (x == -1 ? 1 : 3);
						y --;
					}
				}
			}
			/////////////////////////Down/////////////////////////////////////////////////						
			nSeq = 0;

			x = 1;							
			y = m_nTitleSize - 1 ;
			while (y >= m_nTitleSize / 2) 
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
				pData[x] = nSeq ++;

				if(nDir == DIR_X_P)
				{
					x += 2;
					if(x >= m_nTitleSize)
					{
						nDir = DIR_X_N;
						x -= (x == m_nTitleSize ? 1 : 3);
						y --;
					}
				}
				else //DIR_X_N
				{
					x -= 2;
					if(x <= -1)
					{
						nDir = DIR_X_P;
						x += (x == -1 ? 1 : 3);
						y --;
					}
				}
			}	
			y ++;
			while (y < m_nTitleSize) 
			{
				pData = (float*)((byte*)lr.pBits + lr.Pitch * y);					
				pData[x] = nSeq ++;

				if(nDir == DIR_X_P)
				{
					x += 2;
					if(x >= m_nTitleSize)
					{
						nDir = DIR_X_N;
						x -= (x == m_nTitleSize ? 1 : 3);
						y ++;
					}
				}
				else //DIR_X_N
				{
					x -= 2;
					if(x <= -1)
					{
						nDir = DIR_X_P;
						x += (x == -1 ? 1 : 3);
						y ++;
					}
				}
			}		
		}
		break;
	default:
		ASSERT(0);
		break;
	}
	pTexture->UnlockRect(0);
}


