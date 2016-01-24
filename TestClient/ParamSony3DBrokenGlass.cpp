// ParamSony3DBrokenGlass.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "ParamSony3DBrokenGlass.h"
#include "EffectName.h"
#include "CommonMessage.h"

#pragma warning(disable:4244)
#pragma warning(disable:4800)

// CParamSony3DBrokenGlass 对话框

IMPLEMENT_DYNAMIC(CParamSony3DBrokenGlass, CDialog)

CParamSony3DBrokenGlass::CParamSony3DBrokenGlass(CWnd* pParent /*=NULL*/)
: CDialog(CParamSony3DBrokenGlass::IDD, pParent)
{

}

CParamSony3DBrokenGlass::~CParamSony3DBrokenGlass()
{
}

void CParamSony3DBrokenGlass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParamSony3DBrokenGlass, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_REVERSE, &CParamSony3DBrokenGlass::OnBnClickedReverse)
	ON_BN_CLICKED(IDC_ENABLED, &CParamSony3DBrokenGlass::OnBnClickedEnabled)
END_MESSAGE_MAP()


// CParamSony3DBrokenGlass 消息处理程序
BOOL CParamSony3DBrokenGlass::OnInitDialog()
{
	CDialog::OnInitDialog();

	CSliderCtrl* pCtrl = NULL;
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_PROGRESS);
	pCtrl->SetRange(0, 10000, TRUE);
	pCtrl->SetPos(m_pParam->progress * 10000);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_DIVIDE_X);
	pCtrl->SetRange(1, 40, TRUE);
	pCtrl->SetPos(m_pParam->divideX);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_DIVIDE_Y);
	pCtrl->SetRange(1, 40, TRUE);
	pCtrl->SetPos(m_pParam->divideY);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_FALLING_DIRECTION);
	pCtrl->SetRange(1, 4, TRUE);
	pCtrl->SetPos(m_pParam->fallingDirection);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_CENTER_X);
	pCtrl->SetRange(-10000, 10000, TRUE);
	pCtrl->SetPos(m_pParam->centerX * 1000.f);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_CENTER_Y);
	pCtrl->SetRange(-10000, 10000, TRUE);
	pCtrl->SetPos(m_pParam->centerY * 1000.f);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_LOCAL_TRANSLATE_X);
	pCtrl->SetRange(-10000, 10000, TRUE);
	pCtrl->SetPos(m_pParam->transformPackParam.fLocalTranslateX * 1000.f);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_LOCAL_TRANSLATE_Y);
	pCtrl->SetRange(-10000, 10000, TRUE);
	pCtrl->SetPos(m_pParam->transformPackParam.fLocalTranslateY * 1000.f);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_LOCAL_TRANSLATE_Z);
	pCtrl->SetRange(-10000, 10000, TRUE);
	pCtrl->SetPos(m_pParam->transformPackParam.fLocalTranslateZ * 1000.f);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_LOCAL_RETATE_X);
	pCtrl->SetRange(-10000, 10000, TRUE);
	pCtrl->SetPos(m_pParam->transformPackParam.fLocalRotateX * 1000.f);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_LOCAL_ROTATE_Y);
	pCtrl->SetRange(-10000, 10000, TRUE);
	pCtrl->SetPos(m_pParam->transformPackParam.fLocalRotateY * 1000.f);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_LOCAL_ROTATE_Z);
	pCtrl->SetRange(-10000, 10000, TRUE);
	pCtrl->SetPos(m_pParam->transformPackParam.fLocalRotateZ * 1000.f);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SCALE_X);
	pCtrl->SetRange(-10000, 10000, TRUE);
	pCtrl->SetPos(m_pParam->transformPackParam.fScaleX * 1000.f);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SCALE_Y);
	pCtrl->SetRange(-10000, 10000, TRUE);
	pCtrl->SetPos(m_pParam->transformPackParam.fScaleY * 1000.f);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SCALE_Z);
	pCtrl->SetRange(-10000, 10000, TRUE);
	pCtrl->SetPos(m_pParam->transformPackParam.fScaleZ * 1000.f);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_WORLD_TRANSLATE_X);
	pCtrl->SetRange(-10000, 10000, TRUE);
	pCtrl->SetPos(m_pParam->transformPackParam.fWorldTranslateX * 1000.f);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_WORLD_TRANSLATE_Y);
	pCtrl->SetRange(-10000, 10000, TRUE);
	pCtrl->SetPos(m_pParam->transformPackParam.fWorldTranslateY * 1000.f);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_WORLD_TRANSLATE_Z);
	pCtrl->SetRange(-10000, 10000, TRUE);
	pCtrl->SetPos(m_pParam->transformPackParam.fWorldTranslateZ * 1000.f);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_WORLD_ROTATE_X);
	pCtrl->SetRange(-10000, 10000, TRUE);
	pCtrl->SetPos(m_pParam->transformPackParam.fWorldRotateX * 1000.f);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_WORLD_ROTATE_Y);
	pCtrl->SetRange(-10000, 10000, TRUE);
	pCtrl->SetPos(m_pParam->transformPackParam.fWorldRotateY * 1000.f);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_WORLD_ROTATE_Z);
	pCtrl->SetRange(-10000, 10000, TRUE);
	pCtrl->SetPos(m_pParam->transformPackParam.fWorldRotateZ * 1000.f);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_LIGHT_X_DEGREE);
	pCtrl->SetRange(-10000, 10000, TRUE);
	pCtrl->SetPos(m_pParam->lightingPackParam.lightXDegree * 1000.f);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_LIGHT_Y_DEGREE);
	pCtrl->SetRange(-10000, 10000, TRUE);
	pCtrl->SetPos(m_pParam->lightingPackParam.lightYDegree * 1000.f);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_DIFFUSE_2);
	pCtrl->SetRange(-10000, 10000, TRUE);
	pCtrl->SetPos(m_pParam->lightingPackParam.diffuse2 * 1000.f);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_AMBIENT_1);
	pCtrl->SetRange(-10000, 10000, TRUE);
	pCtrl->SetPos(m_pParam->lightingPackParam.ambient1 * 1000.f);

	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_FOV_DEGREE);
	pCtrl->SetRange(-10000, 10000, TRUE);
	pCtrl->SetPos(m_pParam->perspPackParam.fovDegree * 1000.f);

	return TRUE;
}

void CParamSony3DBrokenGlass::OnHScroll(UINT nSBCode, UINT uPos, CScrollBar* pScrollBar)
{
	CDialog::OnHScroll(nSBCode, uPos, pScrollBar);

	CSliderCtrl* pCtrl = (CSliderCtrl*)pScrollBar;
	int nPos = uPos;
	if(pCtrl)
		nPos = pCtrl->GetPos();

	switch(pScrollBar->GetDlgCtrlID())
	{
	case IDC_SLIDER_PROGRESS:
		m_pParam->progress = nPos / 10000.f;
		break;
	case IDC_DIVIDE_X:
		m_pParam->divideX = nPos;
		break;
	case IDC_DIVIDE_Y:
		m_pParam->divideY = nPos;
		break;
	case IDC_FALLING_DIRECTION:
		m_pParam->fallingDirection = nPos;
		break;
	case IDC_CENTER_X:
		m_pParam->centerX = nPos / 1000.f;
		break;
	case IDC_CENTER_Y:
		m_pParam->centerY = nPos / 1000.f;
		break;
	case IDC_LOCAL_TRANSLATE_X:
		m_pParam->transformPackParam.fLocalTranslateX = nPos / 1000.f;
		break;
	case IDC_LOCAL_TRANSLATE_Y:
		m_pParam->transformPackParam.fLocalTranslateY = nPos / 1000.f;
		break;
	case IDC_LOCAL_TRANSLATE_Z:
		m_pParam->transformPackParam.fLocalTranslateZ = nPos / 1000.f;
		break;
	case IDC_LOCAL_RETATE_X:
		m_pParam->transformPackParam.fLocalRotateX = nPos / 1000.f;
		break;
	case IDC_LOCAL_ROTATE_Y:
		m_pParam->transformPackParam.fLocalRotateY = nPos / 1000.f;
		break;
	case IDC_LOCAL_ROTATE_Z:
		m_pParam->transformPackParam.fLocalRotateZ = nPos / 1000.f;
		break;
	case IDC_SCALE_X:
		m_pParam->transformPackParam.fScaleX = nPos / 1000.f;
		break;
	case IDC_SCALE_Y:
		m_pParam->transformPackParam.fScaleY = nPos / 1000.f;
		break;
	case IDC_SCALE_Z:
		m_pParam->transformPackParam.fScaleZ = nPos / 1000.f;
		break;
	case IDC_WORLD_TRANSLATE_X:
		m_pParam->transformPackParam.fWorldTranslateX = nPos / 1000.f;
		break;
	case IDC_WORLD_TRANSLATE_Y:
		m_pParam->transformPackParam.fWorldTranslateY = nPos / 1000.f;
		break;
	case IDC_WORLD_TRANSLATE_Z:
		m_pParam->transformPackParam.fWorldTranslateZ = nPos / 1000.f;
		break;
	case IDC_WORLD_ROTATE_X:
		m_pParam->transformPackParam.fWorldRotateX = nPos / 1000.f;
		break;
	case IDC_WORLD_ROTATE_Y:
		m_pParam->transformPackParam.fWorldRotateY = nPos / 1000.f;
		break;
	case IDC_WORLD_ROTATE_Z:
		m_pParam->transformPackParam.fWorldRotateZ = nPos / 1000.f;
		break;
	case IDC_LIGHT_X_DEGREE:
		m_pParam->lightingPackParam.lightXDegree = nPos / 1000.f;
		break;
	case IDC_LIGHT_Y_DEGREE:
		m_pParam->lightingPackParam.lightYDegree = nPos / 1000.f;
		break;
	case IDC_DIFFUSE_2:
		m_pParam->lightingPackParam.diffuse2 = nPos / 1000.f;
		break;
	case IDC_AMBIENT_1:
		m_pParam->lightingPackParam.ambient1 = nPos / 1000.f;
		break;
	case IDC_FOV_DEGREE:
		m_pParam->perspPackParam.fovDegree = nPos / 1000.f;
		break;
	}

	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_SONY_3D_BROKEN_GLASS, (LPARAM)m_pParam);
}
void CParamSony3DBrokenGlass::OnBnClickedReverse()
{
	// TODO: 在此添加控件通知处理程序代码
	CButton* pCheckBox = (CButton*)GetDlgItem(IDC_REVERSE);
	int lStatus = pCheckBox->GetCheck();
	m_pParam->bReverse = (lStatus == BST_CHECKED);
	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_SONY_3D_BROKEN_GLASS, (LPARAM)m_pParam);
}

void CParamSony3DBrokenGlass::OnBnClickedEnabled()
{
	// TODO: 在此添加控件通知处理程序代码
	CButton* pCheckBox = (CButton*)GetDlgItem(IDC_ENABLED);
	int lStatus = pCheckBox->GetCheck();
	m_pParam->lightingPackParam.bEnabled = (lStatus == BST_CHECKED);
	AfxGetMainWnd()->SendMessage(UM_SELECT_EFFECT, (WPARAM)FX_SONY_3D_BROKEN_GLASS, (LPARAM)m_pParam);
}
