#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// Record dialog

class Record : public CDialogEx
{
	DECLARE_DYNAMIC(Record)

public:
	Record(CWnd* pParent = NULL);   // standard constructor
	virtual ~Record();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RECORD_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CHotKeyCtrl m_Recorder;
	CString input_key;
	int input_vk_code;
	int input_modifiers;
	virtual BOOL OnInitDialog();
	CString m_RecordLabel;
	afx_msg void OnEnSetfocusEdit1();
	CFont m_font;
	CFont m_font_2;
	afx_msg void OnEnSetfocusMessage();
	afx_msg void OnBnSetfocusOk();
	CEdit m_MessageCtrl;
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};
