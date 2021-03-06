
// TestClientDoc.cpp : CTestClientDoc 类的实现
//

#include "stdafx.h"
#include "TestClient.h"

#include "TestClientDoc.h"
#include "EffectName.h"
#include "FreeImage.h"
#include <d3dx9.h>
#include "../D3D9Render/VideoBuffer.h"
#include "../D3D9Render/VideoBufferManager.h"

#include "../BufferColorConvertor/BufferColorConvertor.h"

#include "../EffNegative/NegativeRender.h"
#include "../EffColorKey/ColorKeyRender.h"
#include "../SonyBlur/SonyBlurRender.h"
#include "../EffAmoebaWipeTrans/AmoebaWipeRender.h"
#include "../EffPush/PushRender.h"
#include "../SonySlide/SonySlideRender.h"
#include "../EffBarmWipeTrans/BarmWipeRender.h"
#include "../EffFanWipe/FanWipeRender.h"
#include "../EffMatrixWipeTrans/MatrixWipeRender.h"
#include "../EffMultiAxisRotaryWipe/MultiAxisRotaryWipeRender.h"
#include "../EffRevolvingWipe/RevolvingWipeRender.h"
#include "../EffRippleWipe/RippleWipeRender.h"
#include "../EffChromaKey/ChromaKeyRender.h"
#include "../EffPageRoll/PageRollRender.h"
#include "../SonyDME3DTransform/SonyDME3DTransformRender.h"
#include "../EffQuadPageRollTrans/DuoPageRollRender.h"
#include "../EffQuadPageRollTrans/QuadPageRollRender.h"
#include "../Eff3DCubeTrans/CubeTransRender.h"
#include "../EffSonyPinP/SonyPinpRender.h"
#include "../EffSonyBarnSlide/SonyBarnSlideRender.h"
#include "../EffBrokenGlass/BrokenGlassRender.h"
#include "../EffRings/RingsRender.h"
#include "../EffDissolve/DissolveRender.h"
#include "../EffSonyFadeFromTo/SonyFadeFromToRender.h"
#include "../EffPageRotation/PageRotationRender.h"
#include "../Sony3DBrokenGlass/Sony3DBrokenGlassRender.h"
#include "../EffDipToColor/DipToColorRender.h"

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
, m_pBackImage(NULL)
, m_pEffectParam(NULL)
, m_pColorConvertor(NULL)
{
	memset(&m_DestVideoBufferInfo, 0, sizeof(VideoBufferInfo));
	m_DestVideoBufferInfo.format = D3DFMT_A8R8G8B8;
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
				bOK = InitEffect(hWnd, DEFAULT_TARGET_BUFFER_WIDTH, DEFAULT_TARGET_BUFFER_HEIGHT);
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
#endif	//_DEBUG


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
		{
			return false;
		}

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
		VideoBufferInfo bi = {D3DFMT_A8R8G8B8, VideoBufferInfo::SYSTEM_MEM, VideoBufferInfo::_IN, w, h, 0, 0};
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

	//if(bOK)
	//{
	//	VideoBufferInfo bi = {D3DFMT_A8R8G8B8, VideoBufferInfo::VIDEO_MEM, VideoBufferInfo::_IN_OUT, w, h, 0, 0};
	//	CVideoBuffer* pTemp = m_pBufferMgr->CreateVideoBuffer(bi);
	//	ASSERT(pTemp);
	//	if(ColorConvert(pTemp, pBuf, true))
	//	{
	//		m_pBufferMgr->ReleaseVideoBuffer(pBuf);
	//		pBuf = pTemp;
	//	}
	//	else
	//	{
	//		m_pBufferMgr->ReleaseVideoBuffer(pTemp);
	//		ASSERT(false);
	//	}
	//}

	return bOK;
}

bool CTestClientDoc::InitEffect(HWND hDeviceWnd, int nBackBufferWidth, int nBackBufferHeight)
{
	m_pRenderEngine = InitEffectModule(hDeviceWnd, nBackBufferWidth, nBackBufferHeight);
	ASSERT(m_pRenderEngine);

	if(m_pRenderEngine)
	{
		m_pBufferMgr = CreateVideoBufferManager(m_pRenderEngine);
		ASSERT(m_pBufferMgr);
	}

	if(m_pBufferMgr)
	{
		SetBackBufferSize(nBackBufferWidth, nBackBufferHeight);
	}

	bool bOK = InitColorConvertor();
	ASSERT(bOK);

	return !!m_pBufferMgr;
}

void CTestClientDoc::UninitEffect()
{
	UninitColorConvertor();

	if(m_pBufferMgr)
	{
	for(size_t i = 0; i < m_SrcImages.size(); ++i)
	{
		m_pBufferMgr->ReleaseVideoBuffer(m_SrcImages[i]);
	}
	m_SrcImages.clear();
	m_pBufferMgr->ReleaseVideoBuffer(m_pBackImage);
	m_pBufferMgr->ReleaseVideoBuffer(m_pDestImage);

	ReleaseVideoBufferManager(m_pBufferMgr);
	m_pBufferMgr = NULL;
	}

	if(m_pRenderEngine)
	{
	m_pRenderEngine->SetVideoBufferManager(NULL);
	UninitEffectModule(m_pRenderEngine);
	m_pRenderEngine = NULL;
	}
}

bool CTestClientDoc::SetBackBufferSize( UINT w, UINT h )
{
	if(m_pRenderEngine)
		m_pRenderEngine->SetTargetVideoSize(w,h);
	if(m_pBufferMgr)
	{
		//target buffer
		if(m_pDestImage)
		{
			m_pBufferMgr->ReleaseVideoBuffer(m_pDestImage);
			m_pDestImage = NULL;
		}
		m_DestVideoBufferInfo.nWidth = w;
		m_DestVideoBufferInfo.nHeight = h;
		m_pDestImage = m_pBufferMgr->CreateVideoBuffer(m_DestVideoBufferInfo);

		//background buffer
		if(m_pBackImage)
		{
			m_pBufferMgr->ReleaseVideoBuffer(m_pBackImage);
			m_pBackImage = NULL;
		}
		if(!m_pBackImage)
		{
			VideoBufferInfo bi = {D3DFMT_A8R8G8B8, VideoBufferInfo::SYSTEM_MEM, VideoBufferInfo::_IN, w, h, 0, 0};
			m_pBackImage = m_pBufferMgr->CreateVideoBuffer(bi);
		}
		if(m_pBackImage)
		{
			DWORD* psLine[2] = {new DWORD[w], new DWORD[w]};
			int pitch = sizeof(DWORD)*w;
			memset(psLine[0], 0, pitch);
			memset(psLine[1], 0xff, pitch);

			int vbpitch = 0;
			BYTE* pd = (BYTE*)m_pBackImage->LockBuffer(vbpitch);
			for(UINT i = 0; i < h; ++i)
			{
				const DWORD* ps = psLine[i%2];
				int copysize = min(pitch, vbpitch);
				int zerosize = max(pitch, vbpitch) - copysize;
				memcpy(pd, ps, copysize);
				memset(pd + copysize, 0, zerosize);
				pd += vbpitch;
				ps += pitch;
			}
			m_pBackImage->UnLockBuffer();
			delete[] psLine[0];
			delete[] psLine[1];
		}

		//refresh
		UpdateAllViews(NULL);
	}
	return !!m_pDestImage;
}


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
			size_t sz = m_SrcImages.size();
			if(sz == 2)
			{
				if(m_SrcImages[0] && m_SrcImages[1])
				{
					CVideoBuffer* pSrc = m_SrcImages[0];
					CVideoBuffer* pSrc2 = m_SrcImages[1];
					CVideoBuffer* pDest = m_pDestImage;

					VideoBufferInfo tempBI = pDest->GetVideoBufferInfo();
					tempBI.eUsage = VideoBufferInfo::_IN_OUT;
					//alpha
#ifdef BLEND_COMPOSE
					CVideoBuffer* pTemp = m_pBufferMgr->CreateVideoBuffer(tempBI);
					ASSERT(pTemp);
					pDest = pTemp;
					ASSERT(pDest == pTemp);
#endif	//BLEND_COMPOSE

//#define YUV_COLOR_SPACE
#ifdef YUV_COLOR_SPACE
					CVideoBuffer* pRGBDest = pDest;
					CVideoBuffer* pRGBSrc1 = pSrc;
					CVideoBuffer* pRGBSrc2 = pSrc2;
					//src buffer for YUV2RGB
					CVideoBuffer* pYUV = m_pBufferMgr->CreateVideoBuffer(tempBI);
					ASSERT(pYUV);
					VideoBufferInfo biYUVSrc = pRGBSrc1->GetVideoBufferInfo();
					biYUVSrc.eUsage = VideoBufferInfo::_IN_OUT;
					CVideoBuffer* pYUVSrc1 = m_pBufferMgr->CreateVideoBuffer(biYUVSrc);
					ASSERT(pYUVSrc1);
					bOK = ColorConvert(pYUVSrc1, pRGBSrc1, true);
					ASSERT(bOK);
					//D3DXSaveSurfaceToFile(_T("./yuv_src_1.dds"), D3DXIFF_DDS, pYUVSrc1->GetSurface(), NULL, NULL);
					biYUVSrc = pRGBSrc2->GetVideoBufferInfo();
					biYUVSrc.eUsage = VideoBufferInfo::_IN_OUT;
					CVideoBuffer* pYUVSrc2 = m_pBufferMgr->CreateVideoBuffer(biYUVSrc);
					ASSERT(pYUVSrc2);
					bOK = ColorConvert(pYUVSrc2, pRGBSrc2, true);
					ASSERT(bOK);
					//D3DXSaveSurfaceToFile(_T("./yuv_src_2.dds"), D3DXIFF_DDS, pYUVSrc2->GetSurface(), NULL, NULL);
					pDest = pYUV; pSrc = pYUVSrc1; pSrc2 = pYUVSrc2;
#endif	//YUV_COLOR_SPACE

					//effect render
					bOK = EffectRender(pDest, pSrc, pSrc2);
					ASSERT(bOK);
					//D3DXSaveSurfaceToFile(_T("./yuv_dest.dds"), D3DXIFF_DDS, pYUV->GetSurface(), NULL, NULL);

#ifdef YUV_COLOR_SPACE
					////YUV to RGB
					bOK = ColorConvert(pRGBDest, pYUV, false);
					ASSERT(bOK);
					m_pBufferMgr->ReleaseVideoBuffer(pYUV);
					m_pBufferMgr->ReleaseVideoBuffer(pYUVSrc1);
					m_pBufferMgr->ReleaseVideoBuffer(pYUVSrc2);
					//D3DXSaveSurfaceToFile(_T("./rgb_dest.dds"), D3DXIFF_DDS, pDest->GetSurface(), NULL, NULL);
#endif	//YUV_COLOR_SPACE

#ifdef BLEND_COMPOSE
					pDest = m_pDestImage;
					ASSERT(pDest != pTemp);
					ASSERT(m_pBackImage);
					bOK = m_pRenderEngine->BlendCompose(pDest, m_pBackImage, pTemp);
					ASSERT(bOK);
					//D3DXSaveSurfaceToFile(_T("./Blend_src_a.dds"), D3DXIFF_DDS, pTemp->GetSurface(), NULL, NULL);
					//D3DXSaveSurfaceToFile(_T("./Blend_src_B.dds"), D3DXIFF_DDS, pSrc2->GetSurface(), NULL, NULL);
					//D3DXSaveSurfaceToFile(_T("./Blend.dds"), D3DXIFF_DDS, pDest->GetSurface(), NULL, NULL);
					m_pBufferMgr->ReleaseVideoBuffer(pTemp);
#endif	//BLEND_COMPOSE
				}
				else
				{
					CString str;
					bool bSecond = (m_SrcImages[1] == NULL);
					str.Format(_T("发现第 %d 层的图像文件加载失败，请检查"), bSecond ? 2 : 1);
					AfxMessageBox(str);
				}
			}
			else
			{
				//AfxMessageBox(_T("请确保 2 个图层的图像文件都正确指定了"));
			}
		}
	}
	return bOK;
}

bool CTestClientDoc::EffectRender(CVideoBuffer* pDest, CVideoBuffer* pSrc, CVideoBuffer* pSrc2)
{
	bool bOK = false;
	if(FX_NONE == m_strEffectName)
	{
		bOK = m_pRenderEngine->EffectVideoCopy(pDest, pSrc);
	}
	else if(FX_NEGATIVE == m_strEffectName)
	{
		CNegativeRender eff;
		if(eff.Init(m_pRenderEngine))
		{
			bOK = eff.Render(pDest, pSrc, m_pEffectParam);
		}
	}
	//else if(FX_COLOR_KEY == m_strEffectName)
	//{
	//	ColorKeyRender eff;
	//	if(eff.Init(m_pRenderEngine))
	//	{
	//		bOK = eff.Render(pDest, pSrc, (ColorKeyParam*)m_pEffectParam);
	//	}
	//}
	else if(FX_CHROMA_KEY== m_strEffectName)
	{
		ChromaKeyRender eff;
		if(eff.Init(m_pRenderEngine))
		{
			//bOK = eff.Render(pDest, pSrc, (ChromaKeyFxParam*)m_pEffectParam);
			bOK = eff.Render(pDest, pSrc, pSrc2, (ChromaKeyFxParam*)m_pEffectParam);
		}
	}
	else if(FX_AMOEBA_WIPE == m_strEffectName)
	{
		CAmoebaWipeRender eff;
		if(eff.Init(m_pRenderEngine))
		{
			bOK = eff.Render(pDest, pSrc, pSrc2, (AmoebaWipeFxParam*)m_pEffectParam);
		}
	}
	else if(FX_PUSH == m_strEffectName)
	{
		CPushRender eff;
		if(eff.Init(m_pRenderEngine))
		{
			bOK = eff.Render(pDest, pSrc, pSrc2, (PushFxParam*)m_pEffectParam);
		}
	}
	else if(FX_RIPPLE_WIPE == m_strEffectName)
	{
		CRippleWipeRender eff;
		if(eff.Init(m_pRenderEngine))
		{
			bOK = eff.Render(pDest, pSrc, pSrc2, m_pEffectParam);
		}
	}
	else if(FX_BROKEN_GLASS == m_strEffectName)
	{
		CBrokenGlassRender eff;
		if(eff.Init(m_pRenderEngine))
		{
			bOK = eff.Render(pDest, pSrc, pSrc2, m_pEffectParam);
		}
	}
	else if(FX_RINGS == m_strEffectName)
	{
		CRingsRender eff;
		if(eff.Init(m_pRenderEngine))
		{
			bOK = eff.Render(pDest, pSrc, pSrc2, m_pEffectParam);
		}
	}
	else if(FX_DISSOLVE == m_strEffectName)
	{
		CDissolveRender eff;
		if(eff.Init(m_pRenderEngine))
		{
			bOK = eff.Render(pDest, pSrc, pSrc2, m_pEffectParam);
		}
	}
	else if(FX_PAGE_ROTATION == m_strEffectName)
	{
		CPageRotationRender eff;
		if(eff.Init(m_pRenderEngine))
		{
			bOK = eff.Render(pDest, pSrc, pSrc2, m_pEffectParam);
		}
	}
	else if(FX_FADE_FROM == m_strEffectName)
	{
		CSonyFadeFromRender eff;
		if(eff.Init(m_pRenderEngine))
		{
			bOK = eff.Render(pDest, pSrc, pSrc2, m_pEffectParam);
		}
	}
	else if(FX_FADE_TO == m_strEffectName)
	{
		CSonyFadeToRender eff;
		if(eff.Init(m_pRenderEngine))
		{
			bOK = eff.Render(pDest, pSrc, pSrc2, m_pEffectParam);
		}
	}
	else if(FX_SONY_SLIDE == m_strEffectName)
	{
		CSonySlideRender eff;
		if(eff.Init(m_pRenderEngine))
		{
			bOK = eff.Render(pDest, pSrc, pSrc2, (SonySlideFxParam*)m_pEffectParam);
		}
	}
	else if(FX_SONY_BARN_SLIDE == m_strEffectName)
	{
		CSonyBarnSlideRender eff;
		if(eff.Init(m_pRenderEngine))
		{
			bOK = eff.Render(pDest, pSrc, pSrc2, m_pEffectParam);
		}
	}
	else if(FX_BARM_WIPE == m_strEffectName)
	{
		CBarmWipeRender eff;
		if(eff.Init(m_pRenderEngine))
		{
			bOK = eff.Render(pDest, pSrc, pSrc2, (BarmWipeFxParam*)m_pEffectParam);
		}
	}
	else if(FX_FAN_WIPE == m_strEffectName)
	{
		CFanWipeRender eff;
		if(eff.Init(m_pRenderEngine))
		{
			bOK = eff.Render(pDest, pSrc, pSrc2, (BasicWipeFxParam*)m_pEffectParam);
		}
	}
	else if(FX_MULTI_AXIS_ROTARY_WIPE == m_strEffectName)
	{
		CMultiAxisRotaryWipeRender eff;
		if(eff.Init(m_pRenderEngine))
		{
			bOK = eff.Render(pDest, pSrc, pSrc2, (BasicWipeFxParam*)m_pEffectParam);
		}
	}
	else if(FX_MATRIX_WIPE == m_strEffectName)
	{
		CMatrixWipeRender eff;
		if(eff.Init(m_pRenderEngine))
		{
			bOK = eff.Render(pDest, pSrc, pSrc2, (MatrixWipeFxParam*)m_pEffectParam);
		}
	}
	else if(FX_REVOLVING_WIPE == m_strEffectName)
	{
		CRevolvingWipeRender eff;
		if(eff.Init(m_pRenderEngine))
		{
			bOK = eff.Render(pDest, pSrc, pSrc2, (BasicWipeFxParam*)m_pEffectParam);
		}
	}
	else if(FX_PAGE_ROLL == m_strEffectName)
	{
		CPageRollRender eff;
		if(eff.Init(m_pRenderEngine))
		{
			bOK = eff.Render(pDest, pSrc, pSrc2, m_pEffectParam);
		}
	}
	else if(FX_DUO_PAGE_ROLL == m_strEffectName)
	{
		CDuoPageRollRender eff;
		if(eff.Init(m_pRenderEngine))
		{
			bOK = eff.Render(pDest, pSrc, pSrc2, m_pEffectParam);
		}
	}
	else if(FX_QUAD_PAGE_ROLL == m_strEffectName)
	{
		CQuadPageRollRender eff;
		if(eff.Init(m_pRenderEngine))
		{
			bOK = eff.Render(pDest, pSrc, pSrc2, m_pEffectParam);
		}
	}
	else if(FX_CUBE_TRANS == m_strEffectName)
	{
		CCubeTransRender eff;
		if(eff.Init(m_pRenderEngine))
		{
			bOK = eff.Render(pDest, pSrc, pSrc2, m_pEffectParam);
		}
	}
	else if(FX_SONY_DME_3D_TRANSFORM == m_strEffectName)
	{
		CSonyDME3DTransformRender eff;
		if(eff.Init(m_pRenderEngine))
		{
			bOK = eff.Render(pDest, pSrc, pSrc2, m_pEffectParam);
		}
	}
	else if(FX_SONY_PINP == m_strEffectName)
	{
		CSonyPinpRender eff;
		if(eff.Init(m_pRenderEngine))
		{
			bOK = eff.Render(pDest, pSrc, pSrc2, m_pEffectParam);
		}
	}
	else if(FX_SONY_3D_BROKEN_GLASS == m_strEffectName)
	{
		Sony3DBrokenGlassRender eff;
		if(eff.Init(m_pRenderEngine))
		{
			bOK = eff.Render(pDest, pSrc, pSrc2, (Sony3DBrokenGlassFxParam*)m_pEffectParam);
		}

	}
	else if(FX_DIP_TO_COLOR == m_strEffectName)
	{
		CDipToColorRender eff;
		if(eff.Init(m_pRenderEngine))
		{
			bOK = eff.Render(pDest, pSrc, pSrc2, (DipToColorTransFxParam*)m_pEffectParam);
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

bool CTestClientDoc::ColorConvert(CVideoBuffer* pDest, CVideoBuffer* pSrc, bool bIsRGB2YUV )
{
	bool bOK = false;
	ASSERT(pSrc);
	ASSERT(pDest);
	ColorConvertFxParam param;
	if(bIsRGB2YUV)
		param.convert_dir = ColorConvertFxParam::RGBA2YUVA;
	else
		param.convert_dir = ColorConvertFxParam::YUVA2RGBA;

	if(m_pColorConvertor)
	{
		bOK = m_pColorConvertor->Render(pDest, pSrc, &param);
	}
	return bOK;
}

bool CTestClientDoc::InitColorConvertor()
{
	bool bOK = false;
	if(!m_pColorConvertor)
		m_pColorConvertor = new CBufferColorConvertor;
	else
		return true;

	if(m_pColorConvertor)
	{
		bOK = m_pColorConvertor->Init(m_pRenderEngine);
		if(!bOK)
		{
			delete m_pColorConvertor;
			m_pColorConvertor = NULL;
		}
	}
	return bOK;
}

void CTestClientDoc::UninitColorConvertor()
{
	if(m_pColorConvertor)
	{
		m_pColorConvertor->Uninit();
		delete m_pColorConvertor;
		m_pColorConvertor = NULL;
	}
}




