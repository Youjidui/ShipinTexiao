
// TestClientDoc.cpp : CTestClientDoc 类的实现
//

#include "stdafx.h"
#include "TestClient.h"

#include "TestClientDoc.h"
//#include "CommonMessage.h"
#include "../FreeImage/FreeImage.h"
#include <d3dx9.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestClientDoc

IMPLEMENT_DYNCREATE(CTestClientDoc, CDocument)

BEGIN_MESSAGE_MAP(CTestClientDoc, CDocument)
END_MESSAGE_MAP()


// CTestClientDoc 构造/析构

CTestClientDoc::CTestClientDoc()
{
	memset(&m_DestVideoBufferInfo, 0, sizeof(VideoBufferInfo));
	m_DestVideoBufferInfo.eType = VBT_D3D9_MEM;
	m_DestVideoBufferInfo.format = D3DFMT_A8B8G8R8;
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
	if(m_ImageFiles.size() > level)
	{
		FIBITMAP* pBmp = FreeImage_LoadU(FIF_BMP, CT2CW(m_ImageFiles[level]));
		if(pBmp)
		{
			FIBITMAP* p32Bmp = FreeImage_ConvertTo32Bits(pBmp);
			if(p32Bmp)
			{
				int w = FreeImage_GetWidth(p32Bmp);
				int h = FreeImage_GetHeight(p32Bmp);
				int p = FreeImage_GetPitch(p32Bmp);
				BYTE* pBits = FreeImage_GetBits(p32Bmp);

				return UpdateBuffer(level, pBits, w, h, p);
			}
		}
	}
	return false;
}

bool CTestClientDoc::UpdateBuffer( UINT level, const BYTE* pBits, int w, int h, int pitch )
{
	if(m_SrcImages.size() <= level)
	{
		m_SrcImages.resize(level + 1);
	}
	IVideoBuffer*& pBuf = m_SrcImages[level];
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
		VideoBufferInfo bi = {VBT_D3D9_MEM, w, h, D3DFMT_A8B8G8R8};
		pBuf = m_pBufferMgr->CreateVideoBuffer(bi);
	}
	if(pBuf)
	{
		int vbpitch = 0;
		BYTE* pd = (BYTE*)pBuf->LockBuffer(vbpitch);
		const BYTE* ps = pBits;
		for(int i = 0; i < h; ++i)
		{
			memcpy(pd, ps, vbpitch);
			pd += vbpitch;
			ps += pitch;
		}
		pBuf->UnLockBuffer();
		return true;
	}
	return false;
}

bool CTestClientDoc::InitEffect(HWND hDeviceWnd, int nBackBufferWidth, int nBackBufferHeight)
{
	bool bOK = InitEffectModule(hDeviceWnd, nBackBufferWidth, nBackBufferHeight);
	if(bOK)
		m_pBufferMgr = CreateVideoBufferManager();
	return !!m_pBufferMgr;
}

void CTestClientDoc::UninitEffect()
{
	ReleaseVideoBufferManager(m_pBufferMgr);
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
