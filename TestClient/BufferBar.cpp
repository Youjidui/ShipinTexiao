// BufferBar.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "BufferBar.h"
#include "CommonMessage.h"


#define PROFILE_SECTION _T("VideoEffectModules")
#define FIRST_LEVEL		_T("FirstLevelFileName")
#define SECOND_LEVEL	_T("SecondLevelFileName")

#define UM_UPDATE_IMAGE_FILE	(WM_USER + 1999)

// CBufferBar 对话框

IMPLEMENT_DYNAMIC(CBufferBar, CDialog)

CBufferBar::CBufferBar(CWnd* pParent /*=NULL*/)
	: CDialog(CBufferBar::IDD, pParent)
	, m_strFirstLevelFileName(_T(""))
	, m_strSecondLevelFileName(_T(""))
	, m_uWidth(1920)
	, m_uHeight(1080)
{

}

CBufferBar::~CBufferBar()
{
}

void CBufferBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_IMAGE1, m_strFirstLevelFileName);
	DDX_Text(pDX, IDC_IMAGE2, m_strSecondLevelFileName);
	DDX_Text(pDX, IDC_WIDTH, m_uWidth);
	DDX_Text(pDX, IDC_HEIGHT, m_uHeight);
}


BEGIN_MESSAGE_MAP(CBufferBar, CDialog)
	ON_BN_CLICKED(IDC_SELECT_IMAGE1, &CBufferBar::OnBnClickedSelectImage1)
	ON_BN_CLICKED(IDC_SELECT_IMAGE2, &CBufferBar::OnBnClickedSelectImage2)
	ON_EN_KILLFOCUS(IDC_HEIGHT, &CBufferBar::OnEnKillfocusHeight)
	ON_EN_KILLFOCUS(IDC_WIDTH, &CBufferBar::OnEnKillfocusWidth)
	ON_MESSAGE(UM_UPDATE_IMAGE_FILE, &CBufferBar::OnUpdateImageFile)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CBufferBar 消息处理程序

void CBufferBar::OnBnClickedSelectImage1()
{
	m_strFirstLevelFileName = GetFilePath();
	AfxGetApp()->WriteProfileString(PROFILE_SECTION, FIRST_LEVEL, m_strFirstLevelFileName);
	UpdateData(FALSE);
	AfxGetMainWnd()->PostMessage(UM_SET_IMAGE, 0, (LPARAM)(LPCTSTR)m_strFirstLevelFileName);
	//TODO: UM_SET_IMAGE
	//UM_SET_IMAGE把图像消息通知给Document类，然后再把图像提取成Buffer传递给VideoEffect
	//再从VideoEffect取得Buffer，显示到View中
}

void CBufferBar::OnBnClickedSelectImage2()
{
	m_strSecondLevelFileName = GetFilePath();
	AfxGetApp()->WriteProfileString(PROFILE_SECTION, SECOND_LEVEL, m_strSecondLevelFileName);
	UpdateData(FALSE);
	AfxGetMainWnd()->PostMessage(UM_SET_IMAGE, 1, (LPARAM)(LPCTSTR)m_strSecondLevelFileName);
	//过渡特技需要2层图像
}

LPCTSTR CBufferBar::GetFilePath()
{
	static CString strPath=_T("");
	CFileDialog Dlg(TRUE,_T("*.bmp"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_T("图像Files(*.bmp *.tga *.png *.tif *.jpg *.jpeg *.gif)|*.bmp;*.tga;*.png;*.tif;*.jpg;*.jpeg;*gif;||"),NULL);
	Dlg.m_ofn.lpstrInitialDir=strPath;
	if(Dlg.DoModal()==IDOK)
	{
		strPath=Dlg.GetPathName();
	}
	return strPath;
}


void CBufferBar::OnEnKillfocusHeight()
{
	UpdateData(TRUE);
	AfxGetMainWnd()->PostMessage(UM_SET_TARGET_BUFFER_SIZE, m_uWidth, m_uHeight);
}

void CBufferBar::OnEnKillfocusWidth()
{
	UpdateData(TRUE);
	AfxGetMainWnd()->PostMessage(UM_SET_TARGET_BUFFER_SIZE, m_uWidth, m_uHeight);
}

void CBufferBar::OnDestroy()
{
	CDialog::OnDestroy();
}

BOOL CBufferBar::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_strFirstLevelFileName = AfxGetApp()->GetProfileString(PROFILE_SECTION, FIRST_LEVEL);
	m_strSecondLevelFileName = AfxGetApp()->GetProfileString(PROFILE_SECTION, SECOND_LEVEL);

	PostMessage(UM_UPDATE_IMAGE_FILE, 0, 0);

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

LRESULT CBufferBar::OnUpdateImageFile( WPARAM w, LPARAM l )
{
	//UpdateData(FALSE);
	if(!m_strFirstLevelFileName.IsEmpty())
		AfxGetMainWnd()->PostMessage(UM_SET_IMAGE, 0, (LPARAM)(LPCTSTR)m_strFirstLevelFileName);
	if(!m_strSecondLevelFileName.IsEmpty())
		AfxGetMainWnd()->PostMessage(UM_SET_IMAGE, 1, (LPARAM)(LPCTSTR)m_strSecondLevelFileName);
	return 0;
}
