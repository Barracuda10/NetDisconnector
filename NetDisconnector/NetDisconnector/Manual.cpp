// Manual.cpp : implementation file
//

#include "stdafx.h"
#include "NetDisconnector.h"
#include "NetDisconnectorDlg.h"
#include "Manual.h"
#include "afxdialogex.h"


// Manual dialog

IMPLEMENT_DYNAMIC(Manual, CDialogEx)

Manual::Manual(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MANUAL_DIALOG, pParent)
	, m_content(_T(""))
{
	//Register Class
	WNDCLASS wc;
	::GetClassInfo(AfxGetInstanceHandle(), _T("#32770"), &wc);
	wc.lpszClassName = _T("NetDisconnectorUsage");
	AfxRegisterClass(&wc);
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


// Manual message handlers


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
	m_content = "    Spin the Lucky Wheel\r\n";
	m_content += "        -  Start GTA5 with VPN or proxy enabled\r\n";
	m_content += "        -  Wait game completing Initializing Social Club\r\n";
	m_content += "        -  Join a Invite Only Session and go to Lucky Wheel\r\n";
	m_content += "        -  Wait for 4s/4.65s to press S after the game show Use S to Spin prompt\r\n";
	m_content += "        -  Press hotkey (default PAUSE key) to disconnect internet if it's not car\r\n";
	m_content += "\r\n    Make solo Public Session\r\n";
	m_content += "        -  Press hotkey (default PAUSE key) to disconnect\r\n";
	m_content += "        -  Offline around 12s (count the reconnecting time)\r\n";
	m_content += "\r\n    Make solo Public Session with UDP Block\r\n";
	m_content += "        -  Change to Firewall Block UDP method\r\n";
	m_content += "        -  Press hotkey (default PAUSE key) to disconnect\r\n";
	m_content += "        -  Offline around 10s (game will not sign out)\r\n";
	m_content += "\r\n    Disconnect by Firewall\r\n";
	m_content += "        -  Firewall Block UDP mode can make solo Public Session\r\n";
	m_content += "        -  It can disconnect all players in the session\r\n";
	m_content += "               and block anyone from joining this session\r\n";
	m_content += "        -  Firewall Block TCP mode can disconnect all connections to Game Server\r\n";
	m_content += "        -  Firewall Block ALL mode can disconnect both of them\r\n";
	m_content += "        -  Game will not force sign out by using these methods\r\n";
	m_content += "        -  Notice all of these methods require system firewall on\r\n";
	m_content += "\r\n    Other Things\r\n";
	m_content += "        -  Disconnect the internet to prevent game saving unwanted progress\r\n";
	m_content += "        -  Game only check region when Initializing Social Club, \r\n";
	m_content += "               after that casino contents is unlocked until game quit, \r\n"; 
	m_content += "               no need VPN or proxy to do casino related stuff\r\n";
	m_content += "        -  This program work when using a VPN or proxy\r\n";//\r\n


	/*WINDOWPLACEMENT wndpl, wndpl_parent;
	GetWindowPlacement(&wndpl_parent);
	GetWindowPlacement(&wndpl);

	CString temp;
	temp.Format(_T("%d"), wndpl.rcNormalPosition.bottom - wndpl_parent.rcNormalPosition.top);
	m_content += temp + _T(" ");
	temp.Format(_T("%d"), wndpl.rcNormalPosition.right - wndpl_parent.rcNormalPosition.left);
	m_content += temp;*/

	UpdateData(false);
	m_contentCtrl.LineScroll(-1 * m_contentCtrl.GetLineCount(), 0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void Manual::OnEnSetfocusContent()
{
	m_contentCtrl.SetSel(-1, -1);
	m_contentCtrl.LineScroll(-1 * m_contentCtrl.GetLineCount(), 0);
	::HideCaret(GetDlgItem(IDC_CONTENT)->GetSafeHwnd());
}


void Manual::OnClose()
{
	DestroyWindow();

	CDialogEx::OnClose();
}
