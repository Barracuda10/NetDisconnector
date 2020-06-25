// Record.cpp : implementation file
//

#include "stdafx.h"
#include "NetDisconnector.h"
#include "Record.h"
#include "afxdialogex.h"


// Record dialog

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
	DDX_Text(pDX, IDC_COVER, m_RecordLabel);
	DDX_Control(pDX, IDC_MESSAGE, m_MessageCtrl);
}


BEGIN_MESSAGE_MAP(Record, CDialogEx)
	ON_BN_CLICKED(IDOK, &Record::OnBnClickedOk)
	ON_EN_SETFOCUS(IDC_COVER, &Record::OnEnSetfocusEdit1)
	ON_EN_SETFOCUS(IDC_MESSAGE, &Record::OnEnSetfocusMessage)
	ON_BN_SETFOCUS(IDOK, &Record::OnBnSetfocusOk)
	ON_WM_HELPINFO()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()


// Record message handlers


void Record::OnBnClickedOk()
{
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
	m_font.CreateFontIndirect(&lf);
	GetDlgItem(IDC_COVER)->SetFont(&m_font);
	GetDlgItem(IDC_HOTKEY1)->SetFont(&m_font);

	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 17;
	m_font_2.CreateFontIndirect(&lf);
	GetDlgItem(IDC_MESSAGE)->SetFont(&m_font_2);

	GetDlgItem(IDC_MESSAGE)->SetWindowText(_T("Click to change key"));
	m_MessageCtrl.SetSel(-1, -1);

	GetDlgItem(IDC_HOTKEY1)->EnableWindow(FALSE);
	GetDlgItem(IDC_HOTKEY1)->ShowWindow(SW_HIDE);
	UpdateData(true);
	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void Record::OnEnSetfocusEdit1()
{
	GetDlgItem(IDC_HOTKEY1)->EnableWindow(TRUE);
	GetDlgItem(IDC_HOTKEY1)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_COVER)->EnableWindow(FALSE);
	GetDlgItem(IDC_COVER)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_HOTKEY1)->SetFocus();
	GetDlgItem(IDC_MESSAGE)->SetWindowText(_T("Recording..."));
	m_MessageCtrl.SetSel(-1, -1);
}


void Record::OnEnSetfocusMessage()
{
	m_MessageCtrl.SetSel(-1, -1);
	::HideCaret(GetDlgItem(IDC_MESSAGE)->GetSafeHwnd());

	if (GetDlgItem(IDC_HOTKEY1)->IsWindowEnabled()) {
		CHotKeyCtrl* hotkey = (CHotKeyCtrl*)GetDlgItem(IDC_HOTKEY1);
		WORD vk_Code, Modifiers;
		hotkey->GetHotKey(vk_Code, Modifiers);
		CString display_key = m_Recorder.GetHotKeyName();
		GetDlgItem(IDC_HOTKEY1)->EnableWindow(FALSE);
		GetDlgItem(IDC_HOTKEY1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COVER)->EnableWindow(TRUE);

		if (vk_Code == 0x00) {
			GetDlgItem(IDC_MESSAGE)->SetWindowText(_T("Click to change key"));
			GetDlgItem(IDC_HOTKEY1)->EnableWindow(FALSE);
			GetDlgItem(IDC_HOTKEY1)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_COVER)->EnableWindow(TRUE);
			GetDlgItem(IDC_COVER)->ShowWindow(SW_SHOW);
			return;
		}
		if (vk_Code == 0x13) {
			display_key = "Pause";
		}
		UpdateData(true);
		m_RecordLabel = display_key;
		
		/*
		available
		get_modifiers == 1 Shift + *
		get_modifiers == 3 Shift + Ctrl + *
		get_modifiers == 4 Alt + *
		get_modifiers == 6 Ctrl + Alt + *

		Unavailable
		get_modifiers == 2 Ctrl + *
		get_modifiers == 5 Shift + Alt + *
		get_modifiers == 7 Ctrl + Shift + Alt + *
		*/
		if (vk_Code >= 0x21 && vk_Code <= 0x2D || Modifiers == 1 || Modifiers == 3 || Modifiers == 4 || Modifiers == 6) {
			GetDlgItem(IDC_MESSAGE)->SetWindowText(_T("This key may not work!!!"));
		}
		else {
			GetDlgItem(IDC_MESSAGE)->SetWindowText(_T("Click to change key"));
		}
		UpdateData(false);

		GetDlgItem(IDC_COVER)->ShowWindow(SW_SHOW);
	}
}


void Record::OnBnSetfocusOk()
{
	Record::OnEnSetfocusMessage();
}


BOOL Record::OnHelpInfo(HELPINFO* pHelpInfo)
{
	return TRUE;
}


BOOL Record::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (pWnd == GetDlgItem(IDOK)) {
		SetCursor(LoadCursor(NULL, IDC_HAND));
		return true;
	}
	if (pWnd == GetDlgItem(IDC_COVER)) {
		SetCursor(LoadCursor(NULL, IDC_HAND));
		return true;
	}
	if (pWnd == GetDlgItem(IDC_MESSAGE)) {
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		return true;
	}

	return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}
