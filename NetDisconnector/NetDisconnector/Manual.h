#pragma once
#include "afxwin.h"


// Manual dialog

class Manual : public CDialogEx
{
	DECLARE_DYNAMIC(Manual)

public:
	Manual(CWnd* pParent = NULL);   // standard constructor
	virtual ~Manual();
private:
	Manual *pManual;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MANUAL_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_content;
	CFont m_font;
	virtual BOOL OnInitDialog();
	afx_msg void OnEnSetfocusContent();
	CEdit m_contentCtrl;
	afx_msg void OnClose();
};
