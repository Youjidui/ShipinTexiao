// EffectBar.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TestClient.h"
#include "EffectBar.h"
#include "EffectName.h"
#include "CommonMessage.h"

// CEffectBar �Ի���

IMPLEMENT_DYNAMIC(CEffectBar, CDialog)

CEffectBar::CEffectBar(CWnd* pParent /*=NULL*/)
	: CDialog(CEffectBar::IDD, pParent)
{

}

CEffectBar::~CEffectBar()
{
}

void CEffectBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EFFECTS, m_ctrlEffects);
	DDX_Control(pDX, IDC_PROGRESS, m_ctrlProgress);
}


BEGIN_MESSAGE_MAP(CEffectBar, CDialog)
	ON_BN_CLICKED(IDC_PARAMETERS, &CEffectBar::OnBnClickedParameters)
	ON_CBN_SELCHANGE(IDC_EFFECTS, &CEffectBar::OnCbnSelchangeEffects)
END_MESSAGE_MAP()


// CEffectBar ��Ϣ�������

void CEffectBar::OnBnClickedParameters()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//����ѡ�����Ч������������Ի���
}

BOOL CEffectBar::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	//m_ctrlEffects.AddString(_T("none"));
	m_ctrlEffects.AddString(FX_NEGATIVE);
	m_ctrlEffects.AddString(FX_COLOR_KEY);

	m_ctrlEffects.SetCurSel(0);

	//ON_CONTROL(CBN_SELCHANGE, IDC_EFFECTS
	PostMessage(WM_COMMAND, MAKELONG(IDC_EFFECTS, CBN_SELCHANGE), (LPARAM)m_ctrlEffects.GetSafeHwnd());

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CEffectBar::OnCbnSelchangeEffects()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nSel = m_ctrlEffects.GetCurSel();
	if(LB_ERR != nSel)
	{
		CString str;
		m_ctrlEffects.GetLBText(nSel, str);
		AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, nSel, (LPARAM)(LPCTSTR)str);
	}
}
