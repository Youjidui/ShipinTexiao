// TestClientDoc.h : CTestClientDoc 类的接口
//
#pragma once

#include <vector>
#include "../VideoBuffer.h"
#include "../D3D9Render/RenderEngine.h"
#include "../D3D9Render/ResourceManager.h"

class CBufferColorConvertor;

class CTestClientDoc : public CDocument
{
protected: // 仅从序列化创建
	CTestClientDoc();
	DECLARE_DYNCREATE(CTestClientDoc)

// 属性
public:
	void SetImage(UINT level, LPCTSTR pszFilename);
	bool SetBackBufferSize(UINT w, UINT h);
	void SetEffect(LPCTSTR pszEffectName, FxParamBase* pParam);

// 操作
public:
	bool InitEffect(HWND hDeviceWnd, int nBackBufferWidth, int nBackBufferHeight);
	void UninitEffect();
	bool UpdateBuffer(UINT level);
	bool Render();
	bool Draw(HWND hWnd);
	bool CopyBuffer(CVideoBuffer* pDest, CVideoBuffer* pSrc);

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

	bool UpdateBuffer(UINT level, const BYTE* pBits, int w, int h, int pitch);
	bool ColorConvert(CVideoBuffer* pDest, CVideoBuffer* pSrc, bool bIsRGB2YUV );
	bool EffectRender(CVideoBuffer* pDestImage, CVideoBuffer* pSrc, CVideoBuffer* pSrc2);

protected:
	CRenderEngine* m_pRenderEngine;
	//CResourceManager* m_pResourceManager;
	CVideoBufferManager* m_pBufferMgr;
	CVideoBuffer* m_pDestImage;

	std::vector<CString> m_ImageFiles;
	std::vector<CVideoBuffer*> m_SrcImages;

	CVideoBuffer* m_pBackImage;

	VideoBufferInfo m_DestVideoBufferInfo;

	CString m_strEffectName;
	FxParamBase* m_pEffectParam;

private:
	bool InitColorConvertor();
	void UninitColorConvertor();
	CBufferColorConvertor* m_pColorConvertor;

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnCloseDocument();
};


