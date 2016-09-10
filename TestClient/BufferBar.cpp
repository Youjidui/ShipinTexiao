// BufferBar.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TestClient.h"
#include "BufferBar.h"
#include "CommonMessage.h"


#define PROFILE_SECTION _T("VideoEffectModules")
#define FIRST_LEVEL		_T("FirstLevelFileName")
#define SECOND_LEVEL	_T("SecondLevelFileName")

#define UM_UPDATE_IMAGE_FILE	(WM_USER + 1999)

// CBufferBar �Ի���

IMPLEMENT_DYNAMIC(CBufferBar, CDialog)

CBufferBar::CBufferBar(CWnd* pParent /*=NULL*/)
	: CDialog(CBufferBar::IDD, pParent)
	, m_strFirstLevelFileName(_T(""))
	, m_strSecondLevelFileName(_T(""))
	, m_uWidth(DEFAULT_TARGET_BUFFER_WIDTH)
	, m_uHeight(DEFAULT_TARGET_BUFFER_HEIGHT)
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
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, &CBufferBar::OnToolTipNotify )
END_MESSAGE_MAP()


// CBufferBar ��Ϣ�������

void CBufferBar::OnBnClickedSelectImage1()
{
	CString str = GetFilePath(m_strFirstLevelFileName);
	if(str != m_strFirstLevelFileName)
	{
		m_strFirstLevelFileName = str;
		AfxGetApp()->WriteProfileString(PROFILE_SECTION, FIRST_LEVEL, m_strFirstLevelFileName);
		UpdateData(FALSE);
		AfxGetMainWnd()->PostMessage(UM_SET_IMAGE, 0, (LPARAM)(LPCTSTR)m_strFirstLevelFileName);
		//TODO: UM_SET_IMAGE
		//UM_SET_IMAGE��ͼ����Ϣ֪ͨ��Document�࣬Ȼ���ٰ�ͼ����ȡ��Buffer���ݸ�VideoEffect
		//�ٴ�VideoEffectȡ��Buffer����ʾ��View��
	}
}

void CBufferBar::OnBnClickedSelectImage2()
{
	CString str = GetFilePath(m_strSecondLevelFileName);
	if(str != m_strSecondLevelFileName)
	{
		m_strSecondLevelFileName = str;
		AfxGetApp()->WriteProfileString(PROFILE_SECTION, SECOND_LEVEL, m_strSecondLevelFileName);
		UpdateData(FALSE);
		AfxGetMainWnd()->PostMessage(UM_SET_IMAGE, 1, (LPARAM)(LPCTSTR)m_strSecondLevelFileName);
		//�����ؼ���Ҫ2��ͼ��
	}
}

LPCTSTR CBufferBar::GetFilePath(CString strInitPath)
{
	static CString strPath;
	strPath = strInitPath;
	CFileDialog Dlg(TRUE,_T("*.bmp"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_T("ͼ��Files(*.bmp *.tga *.png *.tif *.jpg *.jpeg *.gif)|*.bmp;*.tga;*.png;*.tif;*.jpg;*.jpeg;*gif;||"),NULL);
	Dlg.m_ofn.lpstrInitialDir = strPath;
	if(Dlg.DoModal()==IDOK)
	{
		strPath = Dlg.GetPathName();
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
	EnableToolTips(FALSE);
	CDialog::OnDestroy();
}

BOOL CBufferBar::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_strFirstLevelFileName = AfxGetApp()->GetProfileString(PROFILE_SECTION, FIRST_LEVEL);
	m_strSecondLevelFileName = AfxGetApp()->GetProfileString(PROFILE_SECTION, SECOND_LEVEL);

	EnableToolTips(TRUE);
	//m_tooltip.Create(this);
	//m_tooltip.AddTool(GetDlgItem(IDC_IMAGE1));
	//m_tooltip.AddTool(GetDlgItem(IDC_IMAGE2));
	//m_tooltip.Activate(TRUE);

	PostMessage(UM_UPDATE_IMAGE_FILE, 0, 0);

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
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

BOOL CBufferBar::OnToolTipNotify( UINT id, NMHDR * pTTTStruct, LRESULT * pResult )
{
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct;    
	UINT nID =pTTTStruct->idFrom; //�õ���Ӧ����ID���п�����HWND
	if (pTTT->uFlags & TTF_IDISHWND)    //����nID�Ƿ�ΪHWND
	{
		nID = ::GetDlgCtrlID((HWND)nID);//��HWND�õ�IDֵ����Ȼ��Ҳ����ͨ��HWNDֵ���ж�
		switch(nID)
		{
		case(IDC_IMAGE1):
			pTTT->lpszText = (LPTSTR)(LPCTSTR)m_strFirstLevelFileName;
			pTTT->hinst = NULL;
			break;
		case(IDC_IMAGE2):
			pTTT->lpszText = (LPTSTR)(LPCTSTR)m_strSecondLevelFileName;
			pTTT->hinst = NULL;
			break;
		}
		//HWND hWnd = (HWND)pTTTStruct->idFrom;
		//::GetWindowText(hWnd, pTTT->szText, sizeof(pTTT->szText));//����
	}
	return(FALSE);
}

