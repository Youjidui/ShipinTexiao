// ParamCubeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "ParamCubeDlg.h"
#include "EffectName.h"
#include "CommonMessage.h"

#pragma warning(disable:4244)

const float MaxValue = 256.0f;


// CParamCubeDlg 对话框

IMPLEMENT_DYNAMIC(CParamCubeDlg, CDialog)

CParamCubeDlg::CParamCubeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParamCubeDlg::IDD, pParent)
{

}

CParamCubeDlg::~CParamCubeDlg()
{
}

void CParamCubeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParamCubeDlg, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_Light, &CParamCubeDlg::OnBnClickedCheckLight)
	ON_BN_CLICKED(IDC_CHECK_bReverse, &CParamCubeDlg::OnBnClickedCheckbreverse)
END_MESSAGE_MAP()


// CParamCubeDlg 消息处理程序

BOOL CParamCubeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	float decimal_point = 1000.f;
	CSliderCtrl* pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fLocalTranslateX);
	pCtrl->SetRange(-10*decimal_point, 10*decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->trans.fLocalTranslateX * decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fLocalTranslateY);
	pCtrl->SetRange(-10*decimal_point, 10*decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->trans.fLocalTranslateY * decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fLocalTranslateZ);
	pCtrl->SetRange(-5*100.f, 300*100.f, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->trans.fLocalTranslateZ * 100.f);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fLocalRotateX);
	pCtrl->SetRange(-5*10000.f, 5*10000.f, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->trans.fLocalRotateX*10000.f);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fLocalRotateY);
	pCtrl->SetRange(-5*10000.f, 5*10000.f, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->trans.fLocalRotateY*10000.f);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fLocalRotateZ);
	pCtrl->SetRange(-5*10000.f, 5*10000.f, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->trans.fLocalRotateZ*10000.f);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fScaleX);
	pCtrl->SetRange(0, 10*decimal_point);
	if(m_pParam)	pCtrl->SetPos(m_pParam->trans.fScaleX*decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fScaleY);
	pCtrl->SetRange(0, 10*decimal_point);
	if(m_pParam)	pCtrl->SetPos(m_pParam->trans.fScaleY*decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fScaleZ);
	pCtrl->SetRange(0, 10*decimal_point);
	if(m_pParam)	pCtrl->SetPos(m_pParam->trans.fScaleZ*decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fWorldTranslateX);
	pCtrl->SetRange(-10*decimal_point, 10*decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->trans.fWorldTranslateX * decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fWorldTranslateY);
	pCtrl->SetRange(-10*decimal_point, 10*decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->trans.fWorldTranslateY * decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fWorldTranslateZ);
	pCtrl->SetRange(-5*100.f, 300*100.f, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->trans.fWorldTranslateZ * 100.f);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fWorldRotateX);
	pCtrl->SetRange(-5*10000.f, 5*10000.f, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->trans.fWorldRotateX*10000.f);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fWorldRotateY);
	pCtrl->SetRange(-5*10000.f, 5*10000.f, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->trans.fWorldRotateY*10000.f);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fWorldRotateZ);
	pCtrl->SetRange(-5*10000.f, 5*10000.f, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->trans.fWorldRotateZ*10000.f);

	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_Light);
	pBtn->SetCheck(m_pParam->light.bEnabled);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fDirectionX);
	pCtrl->SetRange(-90*decimal_point, 90*decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->light.fDirectionX*decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fDirectionY);
	pCtrl->SetRange(-90*decimal_point, 90*decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->light.fDirectionY*decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fDiffuse);
	pCtrl->SetRange(0, 2*10000.f);
	if(m_pParam)	pCtrl->SetPos(m_pParam->light.fDiffuse*10000.f);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fAmbient);
	pCtrl->SetRange(0, 1*10000.f);
	if(m_pParam)	pCtrl->SetPos(m_pParam->light.fAmbient*10000.f);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_nDiveX);
	pCtrl->SetRange(1, 20, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->shape.nDiveX);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_nDiveY);
	pCtrl->SetRange(1, 20, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->shape.nDiveY);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fIntervalX);
	pCtrl->SetRange(1*decimal_point, 10*decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->shape.fIntervalX*decimal_point);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fIntervalY);
	pCtrl->SetRange(1*decimal_point, 10*decimal_point, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->shape.fIntervalY*decimal_point);

	char vColor[4];
	memcpy(&vColor, &m_pParam->shape.crSlideColor, sizeof(vColor));
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_crSlideColor_R);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(vColor[2]);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_crSlideColor_G);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(vColor[1]);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_crSlideColor_B);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(vColor[0]);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fSlideTransparency);
	pCtrl->SetRange(0, 1*10000.f);
	if(m_pParam)	pCtrl->SetPos(m_pParam->shape.fSlideTransparency*10000.f);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_nDirecttion);
	pCtrl->SetRange(0, 3);
	if(m_pParam)	pCtrl->SetPos(m_pParam->shape.nDirecttion);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_nRotate);
	pCtrl->SetRange(1, 2, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->shape.nRotate);

	pBtn = (CButton*)GetDlgItem(IDC_CHECK_bReverse);
	pBtn->SetCheck(m_pParam->bReverse);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fPerspective);
	pCtrl->SetRange(10*100.f, 120*100.f, TRUE);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fPerspective*100.f);

	memcpy(&vColor, &m_pParam->crBackgroundColor, sizeof(vColor));
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_BackgroundColor_R);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(vColor[2]);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_BackgroundColor_G);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(vColor[1]);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_BackgroundColor_B);
	pCtrl->SetRange(0, 255);
	if(m_pParam)	pCtrl->SetPos(vColor[0]);


	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fBackgroundTransparency);
	pCtrl->SetRange(0, 1*10000.f);
	if(m_pParam)	pCtrl->SetPos(m_pParam->fBackgroundTransparency*10000.f);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_fTransition);
	pCtrl->SetRange(0, 10000);	//1.f, 0, 0.0001f
	if(m_pParam)	pCtrl->SetPos(m_pParam->fTransition*10000.f);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CParamCubeDlg::OnHScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
{
	CDialog::OnHScroll(nSBCode, uPos, pScrollBar);

	CSliderCtrl* pCtrl = (CSliderCtrl*)pScrollBar;
	int nPos = uPos;
	if(pCtrl)
		nPos = pCtrl->GetPos();

	float decimal_point = 1000.f;
	char vColor[4];
	switch(pScrollBar->GetDlgCtrlID())
	{
	case IDC_SLIDER_fTransition:		//Progress
		m_pParam->fTransition = nPos / 10000.f;
		break;
	case IDC_SLIDER_fBackgroundTransparency:
		m_pParam->fBackgroundTransparency = nPos / 10000.f;
		break;
	case IDC_SLIDER_fPerspective:
		m_pParam->fPerspective = nPos / 100.f;
		break;
	case IDC_SLIDER_nRotate:
		m_pParam->shape.nRotate = nPos ;
		break; 
	case IDC_SLIDER_nDirecttion:
		m_pParam->shape.nDirecttion = nPos;
		break;
	case IDC_SLIDER_fSlideTransparency:
		m_pParam->shape.fSlideTransparency = nPos / 10000.f;
		break;
	case IDC_SLIDER_fIntervalY:
		m_pParam->shape.fIntervalY = nPos / decimal_point;
		break;
	case IDC_SLIDER_fIntervalX:
		m_pParam->shape.fIntervalX = nPos / decimal_point;
		break;
	case IDC_SLIDER_nDiveY:
		m_pParam->shape.nDiveY = nPos;
		break;
	case IDC_SLIDER_nDiveX:
		m_pParam->shape.nDiveX = nPos ;
		break;
	case IDC_SLIDER_fAmbient:
		m_pParam->light.fAmbient = nPos / 10000.f;
		break;
	case IDC_SLIDER_fDiffuse:
		m_pParam->light.fDiffuse = nPos / 10000.f;
		break; 
	case IDC_SLIDER_fDirectionY:
		m_pParam->light.fDirectionY = nPos / decimal_point;
		break;
	case IDC_SLIDER_fDirectionX:
		m_pParam->light.fDirectionX = nPos / decimal_point;
		break;
	case IDC_SLIDER_fWorldRotateZ:
		m_pParam->trans.fWorldRotateZ = nPos / 10000.f;
		break;
	case IDC_SLIDER_fWorldRotateY:
		m_pParam->trans.fWorldRotateY = nPos / 10000.f;
		break;
	case IDC_SLIDER_fWorldRotateX:
		m_pParam->trans.fWorldRotateX = nPos / 10000.f;
		break;
	case IDC_SLIDER_fWorldTranslateZ:
		m_pParam->trans.fWorldTranslateZ = nPos / 100.f;
		break;
	case IDC_SLIDER_fWorldTranslateY:
		m_pParam->trans.fWorldTranslateY = nPos / decimal_point;
		break;
	case IDC_SLIDER_fWorldTranslateX:
		m_pParam->trans.fWorldTranslateX = nPos / decimal_point;
		break; 
	case IDC_SLIDER_fScaleZ:
		m_pParam->trans.fScaleZ = nPos / decimal_point;
		break;
	case IDC_SLIDER_fScaleY:
		m_pParam->trans.fScaleY = nPos / decimal_point;
		break;
	case IDC_SLIDER_fScaleX:
		m_pParam->trans.fScaleX = nPos / decimal_point;
		break;
	case IDC_SLIDER_fLocalRotateZ:
		m_pParam->trans.fLocalRotateZ = nPos / 10000.f;
		break;
	case IDC_SLIDER_fLocalRotateY:
		m_pParam->trans.fLocalRotateY = nPos / 10000.f;
		break;
	case IDC_SLIDER_fLocalRotateX:
		m_pParam->trans.fLocalRotateX = nPos / 10000.f;
		break;
	case IDC_SLIDER_fLocalTranslateZ:
		m_pParam->trans.fLocalTranslateZ = nPos / 100.f;
		break;
	case IDC_SLIDER_fLocalTranslateY:
		m_pParam->trans.fLocalTranslateY = nPos / decimal_point;
		break; 
	case IDC_SLIDER_fLocalTranslateX:
		m_pParam->trans.fLocalTranslateX = nPos / decimal_point;
		break;
	case IDC_SLIDER_BackgroundColor_R:
		memcpy(vColor, &m_pParam->crBackgroundColor, sizeof(vColor));
		vColor[2] = nPos;
		memcpy(&m_pParam->crBackgroundColor, vColor, sizeof(vColor));
		break;
	case IDC_SLIDER_BackgroundColor_G:
		memcpy(vColor, &m_pParam->crBackgroundColor, sizeof(vColor));
		vColor[1] = nPos;
		memcpy(&m_pParam->crBackgroundColor, vColor, sizeof(vColor));
		break;
	case IDC_SLIDER_BackgroundColor_B:
		memcpy(vColor, &m_pParam->crBackgroundColor, sizeof(vColor));
		vColor[0] = nPos;
		memcpy(&m_pParam->crBackgroundColor, vColor, sizeof(vColor));
		break;
	case IDC_SLIDER_crSlideColor_R:
		memcpy(vColor, &m_pParam->shape.crSlideColor, sizeof(vColor));
		vColor[2] = nPos;
		memcpy(&m_pParam->shape.crSlideColor, vColor, sizeof(vColor));
		break;
	case IDC_SLIDER_crSlideColor_G:
		memcpy(vColor, &m_pParam->shape.crSlideColor, sizeof(vColor));
		vColor[1] = nPos;
		memcpy(&m_pParam->shape.crSlideColor, vColor, sizeof(vColor));
		break;
	case IDC_SLIDER_crSlideColor_B:
		memcpy(vColor, &m_pParam->shape.crSlideColor, sizeof(vColor));
		vColor[0] = nPos;
		memcpy(&m_pParam->shape.crSlideColor, vColor, sizeof(vColor));
		break;
	}

	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_CUBE_TRANS, (LPARAM)m_pParam);
}

void CParamCubeDlg::OnBnClickedCheckLight()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_Light);
	m_pParam->light.bEnabled = pBtn->GetCheck();
}

void CParamCubeDlg::OnBnClickedCheckbreverse()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_bReverse);
	m_pParam->bReverse = pBtn->GetCheck();
}
