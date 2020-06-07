#pragma once
#include "afxwin.h"


// AutoReonnect �Ի���

class AutoReonnect : public CDialogEx
{
	DECLARE_DYNAMIC(AutoReonnect)

public:
	AutoReonnect(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~AutoReonnect();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AUTORECONNECT_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
};
