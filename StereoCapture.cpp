// StereoCapture.cpp : Implements the classes for the application.
//

#include "stdafx.h"
#include "StereoCapture.h"
#include "StereoCaptureDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// StereoCaptureApp

BEGIN_MESSAGE_MAP(StereoCaptureApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// StereoCaptureApp construction

StereoCaptureApp::StereoCaptureApp()
{
}


// The one and only StereoCaptureApp object

StereoCaptureApp theApp;


// StereoCaptureApp initialization

BOOL StereoCaptureApp::InitInstance()
{
	DShowLib::InitLibrary();

	// At the end of the program, the IC Imaging Control Class Library must be cleaned up
	// by a call to ExitLibrary().
	atexit(DShowLib::ExitLibrary );

	// InitCommonControls() is required on Windows XP, if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("StereoCapture"));

	StereoCaptureDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();


	// Since the dialog has been closed, return FALSE so that we exit the
	// application, rather than start the application's message pump.
	return FALSE;
}
