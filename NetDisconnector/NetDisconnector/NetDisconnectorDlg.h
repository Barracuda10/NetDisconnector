
// NetDisconnectorDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "Manual.h"


// CNetDisconnectorDlg dialog
class CNetDisconnectorDlg : public CDialogEx
{
// Construction
public:
	CNetDisconnectorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NETDISCONNECTOR_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	void OnCbnSelchangeCombo();
	DECLARE_MESSAGE_MAP()
private:
	Manual *pManual;
public:
	CComboBox m_AdaptersList;
	CString m_LogContent;
	CEdit m_LogCtrl;
	afx_msg void OnNetworkDisconnect();
	afx_msg void OnNetworkConnect();
	afx_msg void OnOptionsChangehotkey();
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	CString get_adapter;
	CString get_hotkey;
	CFont m_font;
	CFont m_font_2;
	CBrush m_brush;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	virtual void PreInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnOptionsDisconnectsound();
	afx_msg void OnOptionsReconnectsound();
	UINT DcSoundState;
	UINT RcSoundState;
	UINT DcMethod;
	afx_msg void OnOptionsDisableadapter();
	int get_vk_code;
	afx_msg void OnCbnCloseupCombo();
	afx_msg void OnEnSetfocusLog();
	afx_msg void OnOptionsReleaseipaddress();
	int get_modifiers;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnHelpManual();
	afx_msg void OnOptionsAutoreconnect();
	UINT AutoReconnectState;
	int reconnectDelay;
};
