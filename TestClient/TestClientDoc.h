
// TestClientDoc.h : CTestClientDoc ��Ľӿ�
//
#include <vector>
#include "../VideoBuffer.h"
#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/ResourceManager.h"

#pragma once


class CTestClientDoc : public CDocument
{
protected: // �������л�����
	CTestClientDoc();
	DECLARE_DYNCREATE(CTestClientDoc)

// ����
public:
	void SetImage(UINT level, LPCTSTR pszFilename);
	bool SetBackBufferSize(UINT w, UINT h);
	void SetEffect(LPCTSTR pszEffectName);

// ����
public:
	bool InitEffect(HWND hDeviceWnd, int nBackBufferWidth, int nBackBufferHeight);
	void UninitEffect();
	bool UpdateBuffer(UINT level);
	bool UpdateBuffer(UINT level, const BYTE* pBits, int w, int h, int pitch);
	bool Render();
	bool Draw(HWND hWnd);
	bool CopyBuffer(CVideoBuffer* pDest, CVideoBuffer* pSrc);

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CTestClientDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CRenderEngine* m_pRenderEngine;
	//CResourceManager* m_pResourceManager;
	CVideoBufferManager* m_pBufferMgr;
	CVideoBuffer* m_pDestImage;

	std::vector<CString> m_ImageFiles;
	std::vector<CVideoBuffer*> m_SrcImages;

	VideoBufferInfo m_DestVideoBufferInfo;

	CString m_strEffectName;

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnCloseDocument();
};


