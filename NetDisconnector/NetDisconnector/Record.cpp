// Record.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "NetDisconnector.h"
#include "Record.h"
#include "afxdialogex.h"


// Record �Ի���

IMPLEMENT_DYNAMIC(Record, CDialogEx)

Record::Record(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_RECORD_DIALOG, pParent)
	, input_vk_code(0)
	, input_modifiers(0)
	, m_RecordLabel(_T(""))
{

}

Record::~Record()
{
}

void Record::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HOTKEY1, m_Recorder);
	DDX_Text(pDX, IDC_EDIT1, m_RecordLabel);
}


BEGIN_MESSAGE_MAP(Record, CDialogEx)
	ON_BN_CLICKED(IDOK, &Record::OnBnClickedOk)
	ON_EN_SETFOCUS(IDC_EDIT1, &Record::OnEnSetfocusEdit1)
END_MESSAGE_MAP()


// Record ��Ϣ�������


void Record::OnBnClickedOk()
{
	//TODO�����һ���ǰ壬Ĭ�Ͻ����ȼ��ؼ�������ǰ�ǰ����أ��Ӵ������ȼ��ؼ�
	CHotKeyCtrl* hotkey = (CHotKeyCtrl*)GetDlgItem(IDC_HOTKEY1);
	WORD vk_Code, Modifiers;
	hotkey->GetHotKey(vk_Code, Modifiers);
	input_key = m_Recorder.GetHotKeyName();
	input_vk_code = vk_Code;
	input_modifiers = Modifiers;
	CDialogEx::OnOK();
}


BOOL Record::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 24;
	StrCpyW(lf.lfFaceName, L"Impact");
	m_Font.CreateFontIndirect(&lf);
	GetDlgItem(IDC_EDIT1)->SetFont(&m_Font);
	GetDlgItem(IDC_HOTKEY1)->SetFont(&m_Font);

	GetDlgItem(IDC_HOTKEY1)->EnableWindow(FALSE);
	GetDlgItem(IDC_HOTKEY1)->ShowWindow(SW_HIDE);
	UpdateData(true);
	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void Record::OnEnSetfocusEdit1()
{
	GetDlgItem(IDC_HOTKEY1)->EnableWindow(TRUE);
	GetDlgItem(IDC_HOTKEY1)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT1)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT1)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_HOTKEY1)->SetFocus();
	SetWindowText(_T("Recording..."));
}
