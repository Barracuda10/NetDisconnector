// AutoReonnect.cpp : implementation file
//

#include "stdafx.h"
#include "NetDisconnector.h"
#include "AutoReonnect.h"
#include "afxdialogex.h"
#include "NetDisconnectorDlg.h"

// AutoReonnect dialog

IMPLEMENT_DYNAMIC(AutoReonnect, CDialogEx)

AutoReonnect::AutoReonnect(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_AUTORECONNECT_DIALOG, pParent)
	, m_delay(_T(""))
	, input_autoReconnect(0)
{

}

AutoReonnect::~AutoReonnect()
{
}

void AutoReonnect::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK1, m_autoreconnect);
	DDX_Text(pDX, IDC_DELAY, m_delay);
	DDX_Control(pDX, IDC_DELAY, m_delayCtrl);
}


BEGIN_MESSAGE_MAP(AutoReonnect, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK1, &AutoReonnect::OnBnClickedCheck1)
	ON_EN_CHANGE(IDC_DELAY, &AutoReonnect::OnEnChangeDelay)
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()


// AutoReonnect message handlers


BOOL AutoReonnect::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	//lf.lfHeight = 18;
	lf.lfHeight = 28;
	StrCpyW(lf.lfFaceName, L"System");
	m_font.CreateFontIndirect(&lf);
	GetDlgItem(IDC_CHECK1)->SetFont(&m_font);

	m_autoreconnect.SetCheck(input_autoReconnect);
	if (m_autoreconnect.GetCheck() == BST_CHECKED) {
		GetDlgItem(IDC_DELAY)->EnableWindow(TRUE);
		GetDlgItem(IDC_STATIC)->EnableWindow(TRUE);
		GetDlgItem(IDC_DELAY)->SetFocus();
	}
	else{
		GetDlgItem(IDC_DELAY)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK1)->SetFocus();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void AutoReonnect::OnBnClickedCheck1()
{
	if (m_autoreconnect.GetCheck() == BST_CHECKED) {
		input_autoReconnect = m_autoreconnect.GetCheck();
		GetDlgItem(IDC_DELAY)->EnableWindow(TRUE);
		GetDlgItem(IDC_STATIC)->EnableWindow(TRUE);
		GetDlgItem(IDC_DELAY)->SetFocus();
		m_delayCtrl.SetSel(0, -1);
	}
	else {
		input_autoReconnect = m_autoreconnect.GetCheck();
		GetDlgItem(IDC_DELAY)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK1)->SetFocus();
	}
}


void AutoReonnect::OnEnChangeDelay()
{
	UpdateData(true);
	UpdateData(false);
}


BOOL AutoReonnect::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (pWnd == GetDlgItem(IDOK)) {
		SetCursor(LoadCursor(NULL, IDC_HAND));
		return true;
	}
	if (pWnd == GetDlgItem(IDC_CHECK1)) {
		SetCursor(LoadCursor(NULL, IDC_HAND));
		return true;
	}

	return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}
