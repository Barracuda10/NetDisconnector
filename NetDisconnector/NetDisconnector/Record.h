#pragma once
#include "afxcmn.h"


// Record �Ի���

class Record : public CDialogEx
{
	DECLARE_DYNAMIC(Record)

public:
	Record(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~Record();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RECORD_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
	CFont m_Font;
};
