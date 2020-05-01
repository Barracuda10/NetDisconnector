#pragma once
#include "afxwin.h"


// Manual 对话框

class Manual : public CDialogEx
{
	DECLARE_DYNAMIC(Manual)

public:
	Manual(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~Manual();
private:
	Manual *pManual;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MANUAL_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_content;
	CFont m_font;
	virtual BOOL OnInitDialog();
	afx_msg void OnEnSetfocusContent();
	CEdit m_contentCtrl;
	afx_msg void OnClose();
};
