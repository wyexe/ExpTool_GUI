
// ExpTool_GUIDlg.h : header file
//

#pragma once
#include "ExpTool.h"

// CExpTool_GUIDlg dialog
class CExpTool_GUIDlg : public CDialogEx
{
// Construction
public:
	CExpTool_GUIDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_EXPTOOL_GUI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
private:
	VOID ClearList();
	VOID AddList(_In_ cwstring& wsText);
};
