// StereoCaptureDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StereoCapture.h"
#include "StereoCaptureDlg.h"
#include <numeric>
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define APP_TIMEOUT 5000 // The time in ms after which a grab is considered as failed

// Save some typing...
using namespace DShowLib;


// CAboutDlg dialog used for App About
class CAboutDlg : public CDialog
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

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// StereoCaptureDlg dialog
StereoCaptureDlg::StereoCaptureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(StereoCaptureDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


StereoCaptureDlg::~StereoCaptureDlg()
{
}

void StereoCaptureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTONIMAGESETTINGS1, m_cButtonSettings[0]);
	DDX_Control(pDX, IDC_BUTTONIMAGESETTINGS2, m_cButtonSettings[1]);
	DDX_Control(pDX, IDC_BUTTONLIVEVIDEO1, m_cButtonLive[0]);
	DDX_Control(pDX, IDC_BUTTONLIVEVIDEO2, m_cButtonLive[1]);
	DDX_Control(pDX, IDC_STATICVIDEO1, m_cStaticVideoWindow[0]);
	DDX_Control(pDX, IDC_STATICVIDEO2, m_cStaticVideoWindow[1]);
	DDX_Control(pDX, IDC_BUTTONSNAPIMAGE, m_cButtonSnapImage);
	DDX_Control(pDX, IDC_FILENAME, m_cFilename);
	DDX_Control(pDX, IDC_NFRAMES, m_cNframes);
	DDX_Control(pDX, IDC_FPS, m_cFramerate);
	DDX_Control(pDX, IDC_FASTMODE, m_cFastmode);
}

// StereoCaptureDlg message handlers
BEGIN_MESSAGE_MAP(StereoCaptureDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTONDEVICE1, OnBnClickedButtondevice1)
	ON_BN_CLICKED(IDC_BUTTONDEVICE2, OnBnClickedButtondevice2)
	ON_BN_CLICKED(IDC_BUTTONIMAGESETTINGS1, OnBnClickedButtonimagesettings1)
	ON_BN_CLICKED(IDC_BUTTONIMAGESETTINGS2, OnBnClickedButtonimagesettings2)
	ON_BN_CLICKED(IDC_BUTTONLIVEVIDEO1, OnBnClickedButtonlivevideo1)
	ON_BN_CLICKED(IDC_BUTTONLIVEVIDEO2, OnBnClickedButtonlivevideo2)
	ON_BN_CLICKED(IDC_CPSETTINGS, OnBnClickedCpsettings)
	ON_BN_CLICKED(IDC_FASTMODE, OnBnClickedFastmode)
	ON_BN_CLICKED(IDC_BUTTONSNAPIMAGE, OnBnClickedButtonsnapimage)
END_MESSAGE_MAP()


// Execute when starting the dialog
BOOL StereoCaptureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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


	// Define default textbox values
	m_cFilename.SetWindowText("output/Case1");
	m_cNframes.SetWindowText("10");
	m_cFramerate.SetWindowText("1");



	// Assign this window to the Grabber objects for live video display.
	m_cGrabber[0].setHWND(m_cStaticVideoWindow[0].m_hWnd);
	m_cGrabber[1].setHWND(m_cStaticVideoWindow[1].m_hWnd);

	// Adjust the live video to the size of both windows.
	CRect rect[2];
	m_cStaticVideoWindow[0].GetClientRect(&rect[0]);
	m_cGrabber[0].setDefaultWindowPosition(false);
	m_cGrabber[0].setWindowSize(rect[0].Width(), rect[0].Height());
	m_cStaticVideoWindow[1].GetClientRect(&rect[1]);
	m_cGrabber[1].setDefaultWindowPosition(false);
	m_cGrabber[1].setWindowSize(rect[1].Width(), rect[1].Height());

	// Try to load the previously used video capture device.
	m_cGrabber[0].loadDeviceStateFromFile("device0.xml");
	m_cGrabber[1].loadDeviceStateFromFile("device1.xml");

	// Prepare the sink and its buffer
	PrepareSink(0);
	PrepareSink(1);

	// Update buttons
	SetButtonStates();

	return TRUE;  // return TRUE unless you set the focus to a control.
}


//////////////////////////////////////////////////////////////////////////
// Prepare/update the sink and its buffer.
//////////////////////////////////////////////////////////////////////////
void StereoCaptureDlg::PrepareSink(size_t nstream)
{
	bool bRestartLive = false;

	if (m_cGrabber[nstream].isLive())
	{
		m_cGrabber[nstream].stopLive();
		bRestartLive = true;
	}

	m_pSink[nstream] = NULL;

	// Search for defined frame type and update accordingly
	const GUID subtype = m_cGrabber[nstream].getVideoFormat().getSubtype();
	tColorformatEnum cf = eY800;
	if (subtype == MEDIASUBTYPE_Y16)    cf = eY16;
	else if (subtype == MEDIASUBTYPE_RGB32)  cf = eRGB32;
	else if (subtype == MEDIASUBTYPE_RGB24)  cf = eRGB32;
	else if (subtype == MEDIASUBTYPE_RGB565) cf = eRGB32;
	else if (subtype == MEDIASUBTYPE_RGB555) cf = eRGB32;
	else if (subtype == MEDIASUBTYPE_RGB8)   cf = eY800;
	else if (subtype == MEDIASUBTYPE_UYVY)   cf = eRGB32;
	else if (subtype == MEDIASUBTYPE_Y800)   cf = eY800;
	else if (subtype == MEDIASUBTYPE_BY8)    cf = eRGB32;
	else if (subtype == MEDIASUBTYPE_YGB0)   cf = eY16;
	else if (subtype == MEDIASUBTYPE_YGB1)   cf = eY16;

	CString nframes;
	m_cNframes.GetWindowText(nframes);

	m_pSink[nstream] = FrameHandlerSink::create(cf, atoi(nframes));

	m_pSink[nstream]->setSnapMode(true);


	m_cGrabber[nstream].setSinkType(m_pSink[nstream]);


	if (bRestartLive)
	{
		m_cGrabber[nstream].startLive();
	}

}


//////////////////////////////////////////////////////////////////////////
// This method sets the states of the dialog's buttons.
//////////////////////////////////////////////////////////////////////////
void StereoCaptureDlg::SetButtonStates(void)
{
	bool bDevValid[2];
	bool bIsLive[2];

	for (size_t i = 0; i < 2; i++){
		bDevValid[i] = m_cGrabber[i].isDevValid();
		bIsLive[i] = m_cGrabber[i].isLive();

		m_cButtonSettings[i].EnableWindow(bDevValid[i]);
		m_cButtonLive[i].EnableWindow(bDevValid[i]);

		if (!bDevValid[i])
		{
			m_cButtonLive[i].SetWindowText("Live Start");
		}


		if (bIsLive[i])
		{
			m_cButtonLive[i].SetWindowText("Live Stop");
		}
		else
		{
			m_cButtonLive[i].SetWindowText("Live Start");
		}
	}

	if (!bDevValid[0] || !bDevValid[1])
	{
		m_cButtonSnapImage.EnableWindow(false);
	}
	else
	{
		m_cButtonSnapImage.EnableWindow(true);
	}

}


void StereoCaptureDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

//////////////////////////////////////////////////////////////////////////
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon. For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
//////////////////////////////////////////////////////////////////////////
void StereoCaptureDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle.
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
		CDialog::OnPaint();
	}
}

//////////////////////////////////////////////////////////////////////////
// The system calls this function to obtain the cursor to display while
// the user drags the minimized window.
//////////////////////////////////////////////////////////////////////////
HCURSOR StereoCaptureDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//////////////////////////////////////////////////////////////////////////
// Show the device select dialog, if the "Device" button has been clicked.
// If live video is displayed, it has to be stopped.
//////////////////////////////////////////////////////////////////////////
void StereoCaptureDlg::OnBnClickedButtondevice(size_t nstream)
{

	// If live video is running, stop it.
	if (m_cGrabber[nstream].isDevValid() && m_cGrabber[nstream].isLive())
	{
		m_cGrabber[nstream].stopLive();
	}

	m_cGrabber[nstream].showDevicePage(this->m_hWnd);

	while (m_cGrabber[0].getDev().getUniqueName() == m_cGrabber[1].getDev().getUniqueName()) {
		::MessageBox(0, "This device has already been selected.", "Error", MB_OK | MB_ICONERROR);
		m_cGrabber[nstream].showDevicePage(this->m_hWnd);
	}

	// If we have selected a valid device, save it to the file "device.xml", so
	// the application can load it automatically when it is started the next time.
	if (m_cGrabber[nstream].isDevValid())
	{
		m_cGrabber[nstream].saveDeviceStateToFile({ "device" + std::to_string(nstream) + ".xml" });
	}

	SetButtonStates();
}

//////////////////////////////////////////////////////////////////////////
// Show the image settings dialog of IC Imaging Control. 
//////////////////////////////////////////////////////////////////////////
void StereoCaptureDlg::OnBnClickedButtonimagesettings(size_t nstream)
{
	if (m_cGrabber[nstream].isDevValid())
	{
		m_cGrabber[nstream].showVCDPropertyPage(this->m_hWnd);
		m_cGrabber[nstream].saveDeviceStateToFile({ "device" + std::to_string(nstream) + ".xml" });
	}
}


//////////////////////////////////////////////////////////////////////////
// This method starts and stops the live video.
//////////////////////////////////////////////////////////////////////////
void StereoCaptureDlg::OnBnClickedButtonlivevideo(size_t nstream)
{
	if (m_cGrabber[nstream].isDevValid())
	{
		if (m_cGrabber[nstream].isLive())
		{
			m_cGrabber[nstream].stopLive();
		}
		else
		{
			m_cGrabber[nstream].startLive();
		}
		SetButtonStates();
	}
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Close the app.
//////////////////////////////////////////////////////////////////////////
void StereoCaptureDlg::OnClose(size_t nstream)
{
	m_cGrabber[nstream].stopLive();

	CDialog::OnClose();
}


//////////////////////////////////////////////////////////////////////////
// Sort images by capture time.
//////////////////////////////////////////////////////////////////////////
std::vector<size_t> StereoCaptureDlg::SortByTime(size_t nstream, size_t nframes)
{
	std::vector<size_t> idxvec;
	std::vector<REFERENCE_TIME> timevec;

	idxvec.resize(nframes);
	timevec.resize(nframes);

	for (size_t i = 0; i < nframes; i++)
	{
		timevec[i] = m_pSink[nstream]->getMemBufferCollection()->getBuffer(i)->getSampleDesc().SampleEnd;
	}
	std::iota(idxvec.begin(), idxvec.end(), 0);
	std::sort(idxvec.begin(), idxvec.end(), [&timevec](size_t i1, size_t i2) {return timevec[i1] < timevec[i2]; });

	return idxvec;

}

//////////////////////////////////////////////////////////////////////////
// Snap and save an image.
//////////////////////////////////////////////////////////////////////////
void StereoCaptureDlg::OnBnClickedButtonsnapimage()
{
	bool bRestartLive[2]; // Whether the stream was live when the button was pressed
	bRestartLive[0] = false;
	bRestartLive[1] = false;

	std::vector<size_t> sorted1, sorted2; // Contains indices of frames sorted by capture time

	CString snframes, sfps, sfilename; // Contains strings from text boxes
	// Contains previous strings casted to correct types
	size_t nframes;
	double fps;
	std::string filename;

	// Timer information
	DWORD timer_start, timer_stop, dtime_ms, lasttime;

	// Errors
	Error e1, e2;

	

	// Check if fastmode is enabled
	bool fastmode = false;
	if (m_cFastmode.GetCheck() == BST_CHECKED) fastmode = true;

	// Update the sink and its buffer
	PrepareSink(0);
	PrepareSink(1);

	// Perform capture only if devices are valid
	if (m_cGrabber[0].isDevValid() && m_cGrabber[1].isDevValid())
	{

		// Add listener to know when a frame has been captured
		m_cGrabber[0].addListener(&m_cListener[0], GrabberListener::eFRAMEREADY);
		m_cGrabber[1].addListener(&m_cListener[1], GrabberListener::eFRAMEREADY);


		// Check if live video was on
		if (m_cGrabber[0].isLive())
		{
			m_cGrabber[0].stopLive();
			bRestartLive[0] = true;
		}
		if (m_cGrabber[1].stopLive())
		{
			m_cGrabber[1].startLive(false);
			bRestartLive[1] = true;
		}

		// Initialize the number of captured frames of the listener
		m_cListener[0].done = 0;
		m_cListener[1].done = 0;


		// Get the number of frames from textbox
		m_cNframes.GetWindowText(snframes);
		nframes = atoi(snframes);

		// Get the framerate from textbox
		m_cFramerate.GetWindowText(sfps);
		fps = atof(sfps);

		// Get the output file name from textbox
		m_cFilename.GetWindowText(sfilename);
		filename = (LPCTSTR)sfilename;



		// Start the stream without display
		m_cGrabber[0].startLive(false);
		m_cGrabber[1].startLive(false);


		// Two cases: 
		// 1. Fastmode is on: the capture is performed at the selected camera framerate; all frames are captured in the buffer and retained.
		// 2. Fastmode is off: the capture is performed one frame at a time with a timer.
		if (fastmode == true)
		{
			
			timer_start = ::GetTickCount();

			// Start asynchronous capture: queues up a snap job, thus copying a number of frames into the buffer. Returns immediately.
			e1 = m_pSink[0]->snapImagesAsync(nframes);
			e2 = m_pSink[1]->snapImagesAsync(nframes);

			if (e1.isError())
			{
				// Display an error.
				::MessageBox(0, e1.toString().c_str(), "Error", MB_OK | MB_ICONERROR);
			}
			else if (e2.isError())
			{
				// Display an error.
				::MessageBox(0, e2.toString().c_str(), "Error", MB_OK | MB_ICONERROR);
			}
			else
			{
				// Wait until all the frames have been captured
				while (m_cListener[0].done < nframes || m_cListener[1].done < nframes)
				{
					Sleep(10);

					// Check if error timeout has elapsed
					if ((::GetTickCount() - timer_start) >(APP_TIMEOUT + DWORD(1000.0*(float)nframes / fps)))
					{
						::MessageBox(0, "Timeout occured, not all frames have been grabbed.", "Error", MB_OK | MB_ICONERROR);
						break;
					}
				}


				timer_stop = ::GetTickCount();


				// Stop the stream
				m_cGrabber[0].stopLive();
				m_cGrabber[1].stopLive();

				// Sort the frames by capture time (sortedi contains the sorted indices)
				sorted1 = SortByTime(0, nframes);
				sorted2 = SortByTime(1, nframes);

				// Write buffers to files
				WriteToFile(0, nframes, sorted1, filename);
				WriteToFile(1, nframes, sorted2, filename);

				// Remove listeners
				m_cGrabber[0].removeListener(&m_cListener[0]);
				m_cGrabber[1].removeListener(&m_cListener[1]);

				while (m_cGrabber[0].isListenerRegistered(&m_cListener[0]) || m_cGrabber[1].isListenerRegistered(&m_cListener[1]))
				{
					Sleep(10); // Wait and give pending callbacks a chance to complete.  
				}
			}
		}
		else
		{

			// Compute the time between two frames
			dtime_ms = DWORD(1000.0 / fps);


			timer_start = ::GetTickCount();
			lasttime = timer_start;

			for (size_t i = 0; i < nframes; ++i)
			{
				// Sleep until it is time to snap a picture
				while ((::GetTickCount() - lasttime) < dtime_ms)
				{
					Sleep(1);
				}

				lasttime = ::GetTickCount();

				// Start asynchronous capture: queues up a snap job, thus copying a number of frames into the buffer. Returns immediately.
				e1 = m_pSink[0]->snapImagesAsync(1);
				e2 = m_pSink[1]->snapImagesAsync(1);

				if (e1.isError())
				{
					// Display an error.
					::MessageBox(0, e1.toString().c_str(), "Error", MB_OK | MB_ICONERROR);
				}
				else if (e2.isError())
				{
					// Display an error.
					::MessageBox(0, e2.toString().c_str(), "Error", MB_OK | MB_ICONERROR);
				}
				else
				{
					// Wait until frame has been grabbed
					while (m_cListener[0].done < 1 || m_cListener[1].done < 1)
					{
						Sleep(1);

						// Check if error timeout has elapsed
						if ((::GetTickCount() - lasttime) > APP_TIMEOUT)
						{
							::MessageBox(0, "Timeout occured, not all frames have been grabbed.", "Error", MB_OK | MB_ICONERROR);
							break;
						}
					}

					// Lock buffer once a frame has been captured so that it is not overwritten.
					m_pSink[0]->getLastAcqMemBuffer()->lock();
					m_pSink[1]->getLastAcqMemBuffer()->lock();
				}
				
			}

			timer_stop = ::GetTickCount();

			// Stop the stream
			m_cGrabber[0].stopLive();
			m_cGrabber[1].stopLive();


			// Sort the frames by capture time (sortedi contains the sorted indices)
			sorted1 = SortByTime(0, nframes);
			sorted2 = SortByTime(1, nframes);

			// Write buffers to files
			WriteToFile(0, nframes, sorted1, filename);
			WriteToFile(1, nframes, sorted2, filename);
			
			// Remove listeners
			m_cGrabber[0].removeListener(&m_cListener[0]);
			m_cGrabber[1].removeListener(&m_cListener[1]);

			while (m_cGrabber[0].isListenerRegistered(&m_cListener[0]) || m_cGrabber[1].isListenerRegistered(&m_cListener[1]))
			{
				Sleep(10); // Wait and give pending callbacks a chance to complete.  
			}


		}

	}

	// If needed, restart live streams
	if (bRestartLive[0])
	{
		m_cGrabber[0].startLive(true);
	}

	if (bRestartLive[1])
	{
		m_cGrabber[1].startLive(true);
	}

}

//////////////////////////////////////////////////////////////////////////
// Write buffer to files and display message box.
//////////////////////////////////////////////////////////////////////////
void StereoCaptureDlg::WriteToFile(size_t nstream, size_t nframes, std::vector<size_t> sorted, std::string filename)
{
	REFERENCE_TIME zerotime, timei, totaltime;
	char filenamestring[MAX_PATH], messagestring[MAX_PATH];

	// Get the capture time of the first frame
	zerotime = m_pSink[nstream]->getMemBufferCollection()->getBuffer(sorted[0])->getSampleDesc().SampleEnd;

	for (size_t i = 0; i < nframes; i++)
	{
		timei = m_pSink[nstream]->getMemBufferCollection()->getBuffer(sorted[i])->getSampleDesc().SampleEnd - zerotime;
		sprintf(filenamestring, "%s_cam%i_%02i_%i.bmp", filename.c_str(), nstream+1, i, long(double(timei) / 10000.0));
		m_pSink[nstream]->getMemBufferCollection()->getBuffer(sorted[i])->save(filenamestring);
	}

	totaltime = m_pSink[nstream]->getMemBufferCollection()->getBuffer(sorted[nframes - 1])->getSampleDesc().SampleEnd - zerotime;
	sprintf(messagestring, "Camera %i: %i images snapped in %i ms (average framerate of %f fps)", nstream+1, nframes, long(double(totaltime) / 10000.0), (double)(nframes - 1)*1000.0 / ((double(totaltime) / 10000.0)));

	::MessageBox(0, messagestring, "Finished", MB_OK | MB_ICONERROR);

}


//////////////////////////////////////////////////////////////////////////
// Copy settings from one camera to the other.
//////////////////////////////////////////////////////////////////////////
void StereoCaptureDlg::OnBnClickedCpsettings()
{
	std::string tocopy, tokeep;;
	size_t found1, found2;

	if (m_cGrabber[0].isDevValid() && m_cGrabber[1].isDevValid())
	{
		tocopy = m_cGrabber[0].saveDeviceState();
		tokeep = m_cGrabber[1].saveDeviceState();

		// We do not want to copy the first 2 lines (contains camera model and name)
		tocopy.erase(0, tocopy.find("\n") + 1);
		tocopy.erase(0, tocopy.find("\n") + 1);

		found1 = tokeep.find("\n") + 1;
		found2 = tokeep.find("\n", found1) + 1;

		tocopy.insert(0, tokeep.substr(0, found2));

		m_cGrabber[1].loadDeviceState(tocopy);

		m_cGrabber[1].saveDeviceStateToFile({ "device1.xml" });
	}

}

//////////////////////////////////////////////////////////////////////////
// Reacts to toggling fast mode.
//////////////////////////////////////////////////////////////////////////
void StereoCaptureDlg::OnBnClickedFastmode()
{
	// In fast mode, the framerate is the one from the camera settings
	if (m_cFastmode.GetCheck() == BST_CHECKED)
	{
		m_cFramerate.EnableWindow(false);
		CString strBuffer;
		strBuffer.Format("%f", m_cGrabber[0].getFPS());
		m_cFramerate.SetWindowText(strBuffer);
	}
	else
	{
		m_cFramerate.EnableWindow(true);
		m_cFramerate.SetWindowText("1");
	}

}



void StereoCaptureDlg::OnBnClickedButtondevice1() { OnBnClickedButtondevice(0); }
void StereoCaptureDlg::OnBnClickedButtondevice2() { OnBnClickedButtondevice(1); }

void StereoCaptureDlg::OnBnClickedButtonimagesettings1() { OnBnClickedButtonimagesettings(0); }
void StereoCaptureDlg::OnBnClickedButtonimagesettings2() { OnBnClickedButtonimagesettings(1); }

void StereoCaptureDlg::OnBnClickedButtonlivevideo1() { OnBnClickedButtonlivevideo(0); }
void StereoCaptureDlg::OnBnClickedButtonlivevideo2() { OnBnClickedButtonlivevideo(1); }

