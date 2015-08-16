
// TestClientDoc.h : CTestClientDoc 类的接口
//
#include <vector>
#include "../VideoBuffer.h"

#pragma once


class CTestClientDoc : public CDocument
{
protected: // 仅从序列化创建
	CTestClientDoc();
	DECLARE_DYNCREATE(CTestClientDoc)

// 属性
public:
	void SetImage(UINT level, LPCTSTR pszFilename);

// 操作
public:
	bool InitEffect(HWND hDeviceWnd, int nBackBufferWidth, int nBackBufferHeight);
	void UninitEffect();
	bool UpdateBuffer(UINT level);
	bool UpdateBuffer(UINT level, const BYTE* pBits, int w, int h, int pitch);
	bool SetBackBufferSize(UINT w, UINT h);

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 实现
public:
	virtual ~CTestClientDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	std::vector<CString> m_ImageFiles;
	std::vector<IVideoBuffer*> m_SrcImages;
	IVideoBuffer* m_pDestImage;

	IVideoBufferManager* m_pBufferMgr;

	VideoBufferInfo m_DestVideoBufferInfo;

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnCloseDocument();
};


