
// ExpTool_GUI.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CExpTool_GUIApp:
// See ExpTool_GUI.cpp for the implementation of this class
//

class CExpTool_GUIApp : public CWinApp
{
public:
	CExpTool_GUIApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CExpTool_GUIApp theApp;