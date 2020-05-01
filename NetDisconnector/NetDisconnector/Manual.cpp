// Manual.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "NetDisconnector.h"
#include "NetDisconnectorDlg.h"
#include "Manual.h"
#include "afxdialogex.h"


// Manual �Ի���

IMPLEMENT_DYNAMIC(Manual, CDialogEx)

Manual::Manual(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MANUAL_DIALOG, pParent)
	, m_content(_T(""))
{

}

Manual::~Manual()
{
}

void Manual::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CONTENT, m_content);
	DDX_Control(pDX, IDC_CONTENT, m_contentCtrl);
}


BEGIN_MESSAGE_MAP(Manual, CDialogEx)
	ON_EN_SETFOCUS(IDC_CONTENT, &Manual::OnEnSetfocusContent)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// Manual ��Ϣ��������


BOOL Manual::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	LOGFONT lf;//Set font
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 15;
	StrCpyW(lf.lfFaceName, L"Tahoma");
	m_font.CreateFontIndirect(&lf);
	GetDlgItem(IDC_CONTENT)->SetFont(&m_font);

	UpdateData(true);
	m_content = "    How to spin the Lucky Wheel\r\n";
	m_content += "        -  Start GTA5 with VPN or proxy\r\n";
	m_content += "        -  Wait game complete Initializing Social Club\r\n";
	m_content += "           Casino contents now unlocked until game quit\r\n";
	m_content += "           Feel free to turn of VPN or proxy\r\n";
	m_content += "        -  Join a Invite Only Session and go to Lucky Wheel\r\n";
	m_content += "        -  Wait 4 seconds to press S after game show Use S to Spin message\r\n";
	m_content += "        -  Press hotkey (default PAUSE key) to disconnect if it's not car\r\n";
	m_content += "\r\n    How to make solo Public Session\r\n";
	m_content += "        -  Press hotkey (default PAUSE key) to disconnect\r\n";
	m_content += "        -  Offline around 12s (Wait 12 minus connecting time to Reconnect)\r\n";
	m_content += "\r\n    Other Things\r\n";
	m_content += "        -  Game only check region when Initializing Social Club\r\n";
	m_content += "        -  This program working when using VPN or proxy";//\r\n


	/*WINDOWPLACEMENT wndpl, wndpl_parent;
	GetWindowPlacement(&wndpl_parent);
	GetWindowPlacement(&wndpl);

	CString temp;
	temp.Format(_T("%d"), wndpl.rcNormalPosition.bottom - wndpl_parent.rcNormalPosition.top);
	m_content += temp + _T(" ");
	temp.Format(_T("%d"), wndpl.rcNormalPosition.right - wndpl_parent.rcNormalPosition.left);
	m_content += temp;*/

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void Manual::OnEnSetfocusContent()
{
	m_contentCtrl.SetSel(-1, -1);
	::HideCaret(GetDlgItem(IDC_CONTENT)->GetSafeHwnd());
}


void Manual::OnClose()
{
	DestroyWindow();

	CDialogEx::OnClose();
}