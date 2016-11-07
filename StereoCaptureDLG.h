// StereoCaptureDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "listener.h"

// StereoCaptureDlg dialog
class StereoCaptureDlg : public CDialog
{
	// Construction
	public:
		StereoCaptureDlg(CWnd* pParent = NULL);	// standard constructor
		~StereoCaptureDlg();

	// Dialog Data
		enum { IDD = IDD_StereoCapture_DIALOG };



	protected:
		virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

		afx_msg void OnBnClickedButtondevice1();
		afx_msg void OnBnClickedButtonimagesettings1();
		afx_msg void OnBnClickedButtonlivevideo1();

		afx_msg void OnBnClickedButtondevice2();
		afx_msg void OnBnClickedButtonimagesettings2();
		afx_msg void OnBnClickedButtonlivevideo2();

		void SetButtonStates(void);

		void OnBnClickedButtondevice(size_t nstream);
		void OnBnClickedButtonimagesettings(size_t nstream);
		void OnBnClickedButtonlivevideo(size_t nstream);

		void PrepareSink(size_t nstream);

		std::vector<size_t> StereoCaptureDlg::SortByTime(size_t nstream, size_t nframes);

		void StereoCaptureDlg::WriteToFile(size_t nstream, size_t nframes, std::vector<size_t> sorted, std::string filename);

		HICON m_hIcon;

		// Generated message map functions
		virtual BOOL OnInitDialog();
		afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
		afx_msg void OnPaint();
		afx_msg HCURSOR OnQueryDragIcon();
		afx_msg void OnClose(size_t i);
		afx_msg void OnBnClickedButtonsnapimage();
		afx_msg void OnBnClickedCpsettings();
		afx_msg void OnBnClickedFastmode();
		DECLARE_MESSAGE_MAP()


	private:
		CButton m_cButtonSettings[2];
		CButton m_cButtonLive[2];
		CStatic m_cStaticVideoWindow[2];
		CButton m_cButtonSnapImage;
		CEdit m_cFilename;
		CEdit m_cNframes;
		CEdit m_cFramerate;
		CButton m_cFastmode;

		// The instances of the Grabber class.
		DShowLib::Grabber		m_cGrabber[2]; 

		// Listeners associated to the grabbers
		CListener m_cListener[2];

		// The sinks for snap capture.
		DShowLib::tFrameHandlerSinkPtr m_pSink[2];

};
