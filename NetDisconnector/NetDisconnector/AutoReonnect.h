#pragma once
#include "afxwin.h"


// AutoReonnect dialog

class AutoReonnect : public CDialogEx
{
	DECLARE_DYNAMIC(AutoReonnect)

public:
	AutoReonnect(CWnd* pParent = NULL);   // standard constructor
	virtual ~AutoReonnect();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AUTORECONNECT_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CButton m_autoreconnect;
	afx_msg void OnBnClickedCheck1();
	CString m_delay;
	UINT input_autoReconnect;
	CFont m_font;
	CFont m_font_2;
	afx_msg void OnEnChangeDelay();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	CEdit m_delayCtrl;
	afx_msg void OnBnClickedCheck2();
	CButton m_applytofirewall;
	UINT input_applytofirewall;
};
