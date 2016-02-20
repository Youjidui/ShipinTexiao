#pragma once


// CBufferBar 对话框

class CBufferBar : public CDialog
{
	DECLARE_DYNAMIC(CBufferBar)

public:
	CBufferBar(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBufferBar();

// 对话框数据
	enum { IDD = IDD_DIALOGBAR_IMAGE };

protected:
	LPCTSTR GetFilePath(CString strInitPath);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strFirstLevelFileName;
	CString m_strSecondLevelFileName;
	UINT m_uWidth;
	UINT m_uHeight;
	//CToolTipCtrl m_tooltip;
	afx_msg void OnBnClickedSelectImage1();
	afx_msg void OnBnClickedSelectImage2();
	afx_msg void OnEnKillfocusHeight();
	afx_msg void OnEnKillfocusWidth();
	afx_msg void OnDestroy();
	afx_msg LRESULT OnUpdateImageFile( WPARAM w, LPARAM l );
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnToolTipNotify( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
};
