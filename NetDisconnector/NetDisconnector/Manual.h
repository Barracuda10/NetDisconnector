#pragma once
#include "afxwin.h"


// Manual �Ի���

class Manual : public CDialogEx
{
	DECLARE_DYNAMIC(Manual)

public:
	Manual(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~Manual();
private:
	Manual *pManual;

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MANUAL_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_content;
	CFont m_font;
	virtual BOOL OnInitDialog();
	afx_msg void OnEnSetfocusContent();
	CEdit m_contentCtrl;
	afx_msg void OnClose();
};
