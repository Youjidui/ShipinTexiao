#pragma once
#include "../FxParam.h"
#include "ParamBasicWipeDlg.h"

class CParamMatrixWipeDlg : public CParamBasicWipeDlg
{
	typedef CParamBasicWipeDlg _baseClass;
	DECLARE_DYNAMIC(CParamMatrixWipeDlg)

public:
	CParamMatrixWipeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParamMatrixWipeDlg();

	void SetParam(MatrixWipeFxParam* pParam) {m_pParam = pParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
