#pragma once
#include "afxwin.h"


// AutoReonnect 对话框

class AutoReonnect : public CDialogEx
{
	DECLARE_DYNAMIC(AutoReonnect)

public:
	AutoReonnect(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~AutoReonnect();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AUTORECONNECT_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
};
