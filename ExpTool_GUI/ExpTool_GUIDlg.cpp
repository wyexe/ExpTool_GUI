
// ExpTool_GUIDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ExpTool_GUI.h"
#include "ExpTool_GUIDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CExpTool_GUIDlg dialog



CExpTool_GUIDlg::CExpTool_GUIDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CExpTool_GUIDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CExpTool_GUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CExpTool_GUIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CExpTool_GUIDlg message handlers

BOOL CExpTool_GUIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CExpTool_GUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CExpTool_GUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CExpTool_GUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CExpTool_GUIDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialogEx::OnOK();
}


BOOL CExpTool_GUIDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_CHAR && pMsg->wParam == VK_RETURN)
	{
		if (pMsg->hwnd == reinterpret_cast<CEdit*>(this->GetDlgItem(IDC_EDIT1))->GetSafeHwnd())
		{
			ClearList();

			WCHAR wszText[1024] = { 0 };
			auto pEdit = reinterpret_cast<CEdit*>(this->GetDlgItem(IDC_EDIT1));
			pEdit->GetWindowTextW(wszText, _countof(wszText) - 1);

			if (!CExpTool::GetInstance().GetResult(cwstring(wszText)))
			{
				CONST auto& wsErrText = CExpTool::GetInstance().GetErrText();
				AddList(wsErrText);
			}
			else
			{
				CONST auto& Vec = CExpTool::GetInstance().GetCalcPrcoess();
				for (CONST auto& itm : Vec)
					AddList(itm.c_str());
			}
			
			pEdit->SetSel(-1);
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

VOID CExpTool_GUIDlg::ClearList()
{
	auto pListBox = reinterpret_cast<CListBox*>(GetDlgItem(IDC_LIST1));
	while (pListBox->GetCount() != 0)
		pListBox->DeleteString(0);
}

VOID CExpTool_GUIDlg::AddList(_In_ cwstring& wsText)
{
	auto pListBox = reinterpret_cast<CListBox*>(GetDlgItem(IDC_LIST1));
	pListBox->InsertString(pListBox->GetCount(), wsText.c_str());
}