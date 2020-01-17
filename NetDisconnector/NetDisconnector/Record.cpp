// Record.cpp : 实现文件
//

#include "stdafx.h"
#include "NetDisconnector.h"
#include "Record.h"
#include "afxdialogex.h"


// Record 对话框

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


// Record 消息处理程序


void Record::OnBnClickedOk()
{
	//TODO：添加一个盖板，默认禁用热键控件，点击盖板盖板隐藏，接触禁用热键控件
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
				  // 异常: OCX 属性页应返回 FALSE
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
