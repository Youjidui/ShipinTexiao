
// TestClientDoc.cpp : CTestClientDoc 类的实现
//

#include "stdafx.h"
#include "TestClient.h"

#include "TestClientDoc.h"
//#include "CommonMessage.h"
#include "EffectName.h"
#include "FreeImage.h"
#include <d3dx9.h>
#include "../D3D9Render/VideoBuffer.h"
#include "../D3D9Render/VideoBufferManager.h"
#include "../EffNegative/NegativeRender.h"
#include "../EffColorKey/ColorKeyRender.h"
#include "../SonyBlur/SonyBlurRender.h"
#include "../EffAmoebaWipeTrans/AmoebaWipeRender.h"
<<<<<<< HEAD
#include "../EffPush/PushRender.h"
=======
#include "../BarmWipeTrans/BarmWipeRender.h"

>>>>>>> de3bfc145099d011f86b6de45ea2e0a0f52fbd2f
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestClientDoc

IMPLEMENT_DYNCREATE(CTestClientDoc, CDocument)

BEGIN_MESSAGE_MAP(CTestClientDoc, CDocument)
END_MESSAGE_MAP()


// CTestClientDoc 构造/析构

CTestClientDoc::CTestClientDoc()
: m_pRenderEngine(NULL)
//, m_pResourceManager(NULL)
, m_pBufferMgr(NULL)
, m_pDestImage(NULL)
, m_pEffectParam(NULL)
{
	memset(&m_DestVideoBufferInfo, 0, sizeof(VideoBufferInfo));
	m_DestVideoBufferInfo.format = D3DFMT_A8B8G8R8;
	m_DestVideoBufferInfo.eType = VideoBufferInfo::VIDEO_MEM;
	m_DestVideoBufferInfo.eUsage = VideoBufferInfo::_OUT;
	m_DestVideoBufferInfo.nWidth = 1920;
	m_DestVideoBufferInfo.nHeight = 1080;
}

CTestClientDoc::~CTestClientDoc()
{
}

BOOL CTestClientDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)
	BOOL bOK = FALSE;
	//AfxGetMainWnd()->
	POSITION pos = GetFirstViewPosition();
	if(pos)
	{
		CView* pView = GetNextView(pos);
		if(pView)
		{
			HWND hWnd = (pView->GetSafeHwnd());
			if(hWnd)
			{
				bOK = InitEffect(hWnd, 1920, 1080);
			}
		}
	}

	return bOK;
}




// CTestClientDoc 序列化

void CTestClientDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CTestClientDoc 诊断

#ifdef _DEBUG
void CTestClientDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTestClientDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}

void CTestClientDoc::SetImage( UINT level, LPCTSTR pszFilename )
{
	if(m_ImageFiles.size() <= level)
	{
		m_ImageFiles.resize(level + 1);
	}
	m_ImageFiles[level] = pszFilename;
}

bool CTestClientDoc::UpdateBuffer( UINT level )
{
	bool bOK = false;
	if(m_ImageFiles.size() > level)
	{
		USES_CONVERSION;
		LPCTSTR filename = CT2CW(m_ImageFiles[level]);
		FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
		fif = FreeImage_GetFileTypeU(filename, 0);
		if(fif == FIF_UNKNOWN) 
			fif = FreeImage_GetFIFFromFilenameU(filename);
		if(fif == FIF_UNKNOWN)
			return false;

		FIBITMAP* pBmp = NULL;
		if(FreeImage_FIFSupportsReading(fif))
			pBmp = FreeImage_LoadU(fif, filename);
		if(pBmp)
		{
			FIBITMAP* p32Bmp = FreeImage_ConvertTo32Bits(pBmp);
			if(p32Bmp)
			{
				int w = FreeImage_GetWidth(p32Bmp);
				int h = FreeImage_GetHeight(p32Bmp);
				int p = FreeImage_GetPitch(p32Bmp);
				BYTE* pBits = FreeImage_GetBits(p32Bmp);

				 bOK = UpdateBuffer(level, pBits, w, h, p);
			}
		}
	}
	return bOK;
}

bool CTestClientDoc::UpdateBuffer( UINT level, const BYTE* pBits, int w, int h, int pitch )
{
	bool bOK = false;
	if(m_SrcImages.size() <= level)
	{
		m_SrcImages.resize(level + 1);
	}
	CVideoBuffer*& pBuf = m_SrcImages[level];
	if(pBuf)
	{
		const VideoBufferInfo& bi = pBuf->GetVideoBufferInfo();
		if(bi.nHeight != h || bi.nWidth != w)
		{
			m_pBufferMgr->ReleaseVideoBuffer(pBuf);
			pBuf = NULL;
		}
	}
	if(!pBuf)
	{
		VideoBufferInfo bi = {D3DFMT_A8B8G8R8, VideoBufferInfo::SYSTEM_MEM, VideoBufferInfo::_IN, w, h, 0, 0};
		pBuf = m_pBufferMgr->CreateVideoBuffer(bi);
	}
	if(pBuf)
	{
		int vbpitch = 0;
		BYTE* pd = (BYTE*)pBuf->LockBuffer(vbpitch);
		const BYTE* ps = pBits;
		for(int i = 0; i < h; ++i)
		{
			int copysize = min(pitch, vbpitch);
			int zerosize = max(pitch, vbpitch) - copysize;
			memcpy(pd, ps, copysize);
			memset(pd + copysize, 0, zerosize);
			pd += vbpitch;
			ps += pitch;
		}
		pBuf->UnLockBuffer();
		bOK = true;
	}
	return bOK;
}

bool CTestClientDoc::InitEffect(HWND hDeviceWnd, int nBackBufferWidth, int nBackBufferHeight)
{
	m_pRenderEngine = InitEffectModule(hDeviceWnd, nBackBufferWidth, nBackBufferHeight);
	if(m_pRenderEngine)
	{
		m_pBufferMgr = CreateVideoBufferManager(m_pRenderEngine);
		//m_pRenderEngine->SetVideoBufferManager(m_pBufferMgr);
	}
	if(m_pBufferMgr)
	{
		SetBackBufferSize(nBackBufferWidth, nBackBufferHeight);
	}
	return !!m_pBufferMgr;
}

void CTestClientDoc::UninitEffect()
{
	ReleaseVideoBufferManager(m_pBufferMgr);
	m_pBufferMgr = NULL;
	m_pRenderEngine->SetVideoBufferManager(NULL);
	UninitEffectModule(m_pRenderEngine);
	m_pRenderEngine = NULL;
}

bool CTestClientDoc::SetBackBufferSize( UINT w, UINT h )
{
	if(m_pBufferMgr)
	{
		if(m_pDestImage)
		{
			m_pBufferMgr->ReleaseVideoBuffer(m_pDestImage);
			m_pDestImage = NULL;
		}
		m_DestVideoBufferInfo.nWidth = w;
		m_DestVideoBufferInfo.nHeight = h;
		m_pDestImage = m_pBufferMgr->CreateVideoBuffer(m_DestVideoBufferInfo);

		UpdateAllViews(NULL);
	}
	return !!m_pDestImage;
}

#endif //_DEBUG


// CTestClientDoc 命令

void CTestClientDoc::OnCloseDocument()
{
	UninitEffect();

	CDocument::OnCloseDocument();
}

bool CTestClientDoc::Render()
{
	bool bOK = false;
	if(m_pRenderEngine)
	{
		if(m_pDestImage)
		{
			if(!m_SrcImages.empty())
			{
				if(m_SrcImages[0])
				{
					CVideoBuffer* pSrc = m_SrcImages[0];
					CVideoBuffer* pDest = m_pDestImage;
					const VideoBufferInfo& destBufferInfo = pDest->GetVideoBufferInfo();
					const VideoBufferInfo& srcBufferInfo = pSrc->GetVideoBufferInfo();
					//CopyBuffer(m_pDestImage, m_SrcImages[0]);

					if(FX_NEGATIVE == m_strEffectName)
					{
						CNegativeRender eff;
						if(eff.Init(m_pRenderEngine))
						{
							bOK = eff.Render(pDest, pSrc, (NegativeFxParam*)m_pEffectParam);
						}
					}
					else if(FX_COLOR_KEY == m_strEffectName)
					{
						ColorKeyRender eff;
						if(eff.Init(m_pRenderEngine))
						{
							bOK = eff.Render(pDest, pSrc, (ColorKeyParam*)m_pEffectParam);
						}
					}
					else if(FX_SONY_BLUR == m_strEffectName)
					{
						CSonyBlurRender eff;
						if(eff.Init(m_pRenderEngine))
						{
							bOK = eff.Render(pDest, pSrc, (SonyBlurFxParam*)m_pEffectParam);
						}
					}
					else if(FX_AMOEBA_WIPE == m_strEffectName)
					{
						if(m_SrcImages.size() >= 2)
						{
							CVideoBuffer* pSrc2 = m_SrcImages[1];
							CAmoebaWipeRender eff;
							if(eff.Init(m_pRenderEngine))
							{
								bOK = eff.Render(pDest, pSrc, pSrc2, (AmoebaWipeFxParam*)m_pEffectParam);
							}
						}
					}
<<<<<<< HEAD
					else if(FX_PUSH == m_strEffectName)
=======
					else if(FX_BARM_WIPE == m_strEffectName)
>>>>>>> de3bfc145099d011f86b6de45ea2e0a0f52fbd2f
					{
						if(m_SrcImages.size() >= 2)
						{
							CVideoBuffer* pSrc2 = m_SrcImages[1];
<<<<<<< HEAD
							CPushRender eff;
							if(eff.Init(m_pRenderEngine))
							{
								bOK = eff.Render(pDest, pSrc, pSrc2, (PushFxParam*)m_pEffectParam);
=======
							CBarmWipeRender eff;
							if(eff.Init(m_pRenderEngine))
							{
								bOK = eff.Render(pDest, pSrc, pSrc2, (BarmWipeFxParam*)m_pEffectParam);
>>>>>>> de3bfc145099d011f86b6de45ea2e0a0f52fbd2f
							}
						}
					}
				}
			}
		}
	}
	return bOK;
}

bool CTestClientDoc::CopyBuffer( CVideoBuffer* pDest, CVideoBuffer* pSrc )
{
	if(pSrc && pDest)
	{
		const VideoBufferInfo& destBufferInfo = pDest->GetVideoBufferInfo();
		const VideoBufferInfo& srcBufferInfo = pSrc->GetVideoBufferInfo();
		int pitch = 0;
		const BYTE* ps = (const BYTE*)pSrc->LockBuffer(pitch);
		int vbpitch = 0;
		BYTE* pd = (BYTE*)pDest->LockBuffer(vbpitch);
		int h = min(destBufferInfo.nHeight, srcBufferInfo.nHeight);
		for(int i = 0; i < h; ++i)
		{
			int copysize = min(pitch, vbpitch);
			int zerosize = max(pitch, vbpitch) - copysize;
			memcpy(pd, ps, copysize);
			memset(pd + copysize, 0, zerosize);
			pd += vbpitch;
			ps += pitch;
		}
		pSrc->UnLockBuffer();
		pDest->UnLockBuffer();
		return true;
	}
	return false;
}

bool CTestClientDoc::Draw( HWND hWnd )
{
	bool bOK = false;
	if(m_pRenderEngine)
	{
		if(m_pDestImage)
		{
			IDirect3DDevice9 *pDevice = m_pRenderEngine->GetDevice();
			if(pDevice)
			{
				IDirect3DSurface9 *pBackSurface;
				pDevice->GetBackBuffer(0,
					0,
					D3DBACKBUFFER_TYPE_MONO,
					&pBackSurface
					);

				if(pBackSurface)
				{
					IDirect3DSurface9* pSrc = m_pDestImage->GetSurface();
					if(pSrc)
					{
						pDevice->StretchRect(pSrc,
							NULL,
							pBackSurface,
							NULL,
							D3DTEXF_POINT 
							);
						//D3DXSaveSurfaceToFile(_T("c:\\DisplaySurface-Result.dds"), D3DXIFF_DDS, m_pDisplaySurface, NULL, NULL);

						HRESULT hr = pDevice->Present(NULL,NULL,hWnd,NULL);
						//D3DXSaveSurfaceToFile(_T("c:\\BackSurface.dds"), D3DXIFF_DDS, pBackSurface, NULL, NULL);
						bOK = SUCCEEDED(hr);
					}

					pBackSurface->Release();
				}		
			}		
		}
	}
	return bOK;
}

void CTestClientDoc::SetEffect( LPCTSTR pszEffectName, FxParamBase* pParam )
{
	m_strEffectName = pszEffectName;
	m_pEffectParam = pParam;
}
