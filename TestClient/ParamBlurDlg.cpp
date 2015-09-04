// ParamBlurDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TestClient.h"
#include "ParamBlurDlg.h"
#include <tchar.h>
#include "EffectName.h"
#include "CommonMessage.h"


// CParamBlurDlg �Ի���

IMPLEMENT_DYNAMIC(CParamBlurDlg, CDialog)

CParamBlurDlg::CParamBlurDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParamBlurDlg::IDD, pParent)
{

}

CParamBlurDlg::~CParamBlurDlg()
{
}

void CParamBlurDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParamBlurDlg, CDialog)
	ON_EN_CHANGE(IDC_EDIT_BLUR_X, &CParamBlurDlg::OnEnChangeEditBlurX)
	ON_EN_CHANGE(IDC_EDIT_BLUR_Y, &CParamBlurDlg::OnEnChangeEditBlurY)
END_MESSAGE_MAP()


// CParamBlurDlg ��Ϣ�������

void CParamBlurDlg::OnEnChangeEditBlurX()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString str;
	//GetDlgItem(IDC_EDIT_BLUR_X, );
	GetDlgItemText(IDC_EDIT_BLUR_X, str);
	_stscanf(str, _T("%f"), &m_pParam->blurX);
	//m_pParam->blurX = _ttoi(str);
	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_SONY_BLUR, (LPARAM)m_pParam);
}

//UM_EFFECT_PARAMETER_CHANGE
void CParamBlurDlg::OnEnChangeEditBlurY()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	CString str;
	GetDlgItemText(IDC_EDIT_BLUR_Y, str);
	_stscanf(str, _T("%f"), &m_pParam->blurY);
	//m_pParam->blurY = _ttoi(str);
	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_SONY_BLUR, (LPARAM)m_pParam);
}

BOOL CParamBlurDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	TCHAR szText[16];
	_stprintf(szText, _T("%f"), m_pParam->blurX);
	SetDlgItemText(IDC_EDIT_BLUR_X, szText);
	_stprintf(szText, _T("%f"), m_pParam->blurY);
	SetDlgItemText(IDC_EDIT_BLUR_Y, szText);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
