
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
	// TODO: 在此添加一次性构造代码

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

	return TRUE;
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
		VideoBufferInfo* pbi = pBuf->GetVideoBufferInfo();
		if(pbi->nHeight != h || pbi->nWidth != w)
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

bool CTestClientDoc::InitEffect()
{
	m_pBufferMgr = CreateVideoBufferManager();
	return InitEffects(m_pEffect, m_pTransEffect);
}

void CTestClientDoc::UninitEffect()
{
	UninitEffects();
	ReleaseVideoBufferManager(m_pBufferMgr);
}

#endif //_DEBUG


// CTestClientDoc 命令
