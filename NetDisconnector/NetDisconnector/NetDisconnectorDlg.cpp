
// NetDisconnectorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NetDisconnector.h"
#include "NetDisconnectorDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//These headers use to get adapters
#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib, "IPHLPAPI.lib")
#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

//These header use to access other dialogs
#include "Record.h"
#include "AutoReonnect.h"

//These headers use to play sound
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")

//These headers use to operate system firewall
#include<netfw.h>





// CNetDisconnectorDlg dialog



CNetDisconnectorDlg::CNetDisconnectorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_NETDISCONNECTOR_DIALOG, pParent)
	, m_log(_T(""))
	, get_adapter(_T(""))
	, get_hotkey(_T(""))
	, DcSoundState(0)
	, RcSoundState(0)
	, DcMethod(0)
	, get_vk_code(0)
	, get_modifiers(0)
	, AutoReconnectState(0)
	, ARApplytoFirewallState(0)
	, reconnectDelay(0)
	, logWidthDiff(0)
	, logHeightDiff(0)
	, comboWidthDiff(0)
	, AdapterCurrent(0)
	, BlockudpState(0)
	, BlocktcpState(0)
	, BlockallState(0)
	, fwOn(0)
	//, connection(true)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
}

void CNetDisconnectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO, m_adaptersList);
	DDX_Text(pDX, IDC_LOG, m_log);
	DDX_Control(pDX, IDC_LOG, m_logCtrl);
}

BEGIN_MESSAGE_MAP(CNetDisconnectorDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO, &CNetDisconnectorDlg::OnCbnSelchangeCombo)
	ON_COMMAND(ID_NETWORK_DISCONNECT, &CNetDisconnectorDlg::OnNetworkDisconnect)
	ON_COMMAND(ID_NETWORK_CONNECT, &CNetDisconnectorDlg::OnNetworkConnect)
	ON_COMMAND(ID_OPTIONS_CHANGEHOTKEY, &CNetDisconnectorDlg::OnOptionsChangehotkey)
	ON_WM_HOTKEY()
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_HELPINFO()
	ON_COMMAND(ID_OPTIONS_DISCONNECTSOUND, &CNetDisconnectorDlg::OnOptionsDisconnectsound)
	ON_COMMAND(ID_OPTIONS_RECONNECTSOUND, &CNetDisconnectorDlg::OnOptionsReconnectsound)
	ON_COMMAND(ID_OPTIONS_DISABLEADAPTER, &CNetDisconnectorDlg::OnOptionsDisableadapter)
	ON_CBN_CLOSEUP(IDC_COMBO, &CNetDisconnectorDlg::OnCbnCloseupCombo)
	ON_EN_SETFOCUS(IDC_LOG, &CNetDisconnectorDlg::OnEnSetfocusLog)
	ON_COMMAND(ID_OPTIONS_RELEASEIPADDRESS, &CNetDisconnectorDlg::OnOptionsReleaseipaddress)
	ON_WM_TIMER()
	ON_COMMAND(ID_HELP_MANUAL, &CNetDisconnectorDlg::OnHelpManual)
	ON_COMMAND(ID_OPTIONS_AUTORECONNECT, &CNetDisconnectorDlg::OnOptionsAutoreconnect)
	ON_WM_SIZE()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_OPTIONS_BLOCKUDP, &CNetDisconnectorDlg::OnOptionsFirewallblockudp)
	ON_COMMAND(ID_OPTIONS_BLOCKTCP, &CNetDisconnectorDlg::OnOptionsFirewallblocktcp)
	ON_COMMAND(ID_OPTIONS_FIREWALLBLOCKALL, &CNetDisconnectorDlg::OnOptionsFirewallblockall)
	ON_COMMAND(ID_HELP_OPENFIREWALLSETTINGS, &CNetDisconnectorDlg::OnHelpOpenfirewallsettings)
	ON_COMMAND(ID_HELP_VIEWFIREWALLRULES, &CNetDisconnectorDlg::OnHelpViewfirewallrules)
END_MESSAGE_MAP()


struct AdaptersSet {
	CString AdapterNameToMatch;
	CString Description;
	CString IpAddress;
	CString Gateway;
	//CString CConnection = _T("Connected");
	BOOL Connection = true;

	CString AdapterName;
	CString ValueName;
	CString ConnectionName;
	CString ValuePnPInstanceId;
	CString PnPInstanceId;

	LPWSTR cmdline_disconnect[2];
	LPWSTR cmdline_connect[2];
	/*LPWSTR cmdline_disconnect_2;
	LPWSTR cmdline_connect_2;*/
}AdaptersSet[32];
int aIndex = 0;


struct RulesSet {
	CString RuleName;
	BOOL enable = FALSE;

	LPCSTR cmdline_enable[2];
	LPCSTR cmdline_disable[2];
}RulesSet[4];
int rIndex = 0;


CString modeSet[8] = { _T("Disconnect Adapter"), _T("Disable Adapter"), _T("Firewall Block UDP"), _T("Firewall Block TCP"), _T("Firewall Block All") };
int idSet[8] = { ID_OPTIONS_RELEASEIPADDRESS, ID_OPTIONS_DISABLEADAPTER, ID_OPTIONS_BLOCKUDP, ID_OPTIONS_BLOCKTCP, ID_OPTIONS_FIREWALLBLOCKALL };


// CNetDisconnectorDlg message handlers

BOOL CNetDisconnectorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here


	m_logCtrl.SetBackgroundColor(false, RGB(12, 12, 12));
	CHARFORMAT2 cf;
	ZeroMemory(&cf, sizeof(cf));
	cf.dwMask = CFM_COLOR | CFM_FACE | CFM_SIZE;
	cf.dwEffects = CFE_BOLD;
	cf.crTextColor = RGB(204, 204, 204);
	cf.yHeight = 220;
	StrCpyW(cf.szFaceName, _T("Calibri"));
	m_logCtrl.SetDefaultCharFormat(cf);
	m_logCtrl.SetTextMode(TM_PLAINTEXT);
	m_logCtrl.SetReadOnly(TRUE);
	m_logCtrl.HideCaret();
	::HideCaret(GetDlgItem(IDC_LOG)->GetSafeHwnd());


	LOGFONT lf;//Set font
	/*memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 18;
	StrCpyW(lf.lfFaceName, L"Calibri");
	m_font.CreateFontIndirect(&lf);
	GetDlgItem(IDC_LOG)->SetFont(&m_font);*/


	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 15;
	m_font_2.CreateFontIndirect(&lf);
	GetDlgItem(IDC_COMBO)->SetFont(&m_font_2);


	//struct AdaptersSet {
	//	CString AdapterNameToMatch;
	//	CString Description;
	//	CString IpAddress;
	//	CString Gateway;
	//	//CString CConnection = _T("Connected");
	//	BOOL Connection = true;

	//	CString AdapterName;
	//	CString ValueName;
	//	CString ConnectionName;
	//	CString ValuePnPInstanceId;
	//	CString PnPInstanceId;
	//}AdaptersSet[32];
	//int aIndex = 0;


	UpdateData(true);
	SYSTEMTIME systime;
	CString time;



	GetLocalTime(&systime);
	time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

	AutoReconnectState = AfxGetApp()->GetProfileInt(_T("Settings"), _T("AutoReconnect"), BST_UNCHECKED);
	ARApplytoFirewallState = AfxGetApp()->GetProfileInt(_T("Settings"), _T("AutoReconnectFirewall"), BST_CHECKED);
	reconnectDelay = AfxGetApp()->GetProfileInt(_T("Settings"), _T("ReconnectDelay"), 15000);

	DcSoundState = AfxGetApp()->GetProfileInt(_T("Settings"), _T("DcSound"), MF_CHECKED);
	RcSoundState = AfxGetApp()->GetProfileInt(_T("Settings"), _T("RcSound"), MF_CHECKED);
	CMenu* menu = GetMenu()->GetSubMenu(1);
	menu->CheckMenuItem(ID_OPTIONS_DISCONNECTSOUND, MF_BYCOMMAND | DcSoundState);
	menu->CheckMenuItem(ID_OPTIONS_RECONNECTSOUND, MF_BYCOMMAND | RcSoundState);

	DcMethod = AfxGetApp()->GetProfileInt(_T("Settings"), _T("DcMethod"), 0);
	if (DcMethod == MF_CHECKED)
		DcMethod = 1;
	else if (DcMethod == MF_UNCHECKED)
		DcMethod = 0;

	menu->CheckMenuRadioItem(ID_OPTIONS_RELEASEIPADDRESS, ID_OPTIONS_FIREWALLBLOCKALL, idSet[DcMethod], MF_BYCOMMAND);
	/*if (DcMethod == 0)
		menu->CheckMenuRadioItem(ID_OPTIONS_RELEASEIPADDRESS, ID_OPTIONS_FIREWALLBLOCKALL, ID_OPTIONS_RELEASEIPADDRESS, MF_BYCOMMAND);
	else if (DcMethod == 1)
		menu->CheckMenuRadioItem(ID_OPTIONS_RELEASEIPADDRESS, ID_OPTIONS_FIREWALLBLOCKALL, ID_OPTIONS_DISABLEADAPTER, MF_BYCOMMAND);
	else if (DcMethod == 2)
		menu->CheckMenuRadioItem(ID_OPTIONS_RELEASEIPADDRESS, ID_OPTIONS_FIREWALLBLOCKALL, ID_OPTIONS_BLOCKUDP, MF_BYCOMMAND);
	else if (DcMethod == 3)
		menu->CheckMenuRadioItem(ID_OPTIONS_RELEASEIPADDRESS, ID_OPTIONS_FIREWALLBLOCKALL, ID_OPTIONS_BLOCKTCP, MF_BYCOMMAND);
	else if (DcMethod == 4)
		menu->CheckMenuRadioItem(ID_OPTIONS_RELEASEIPADDRESS, ID_OPTIONS_FIREWALLBLOCKALL, ID_OPTIONS_FIREWALLBLOCKALL, MF_BYCOMMAND);*/

	AfxGetMainWnd()->SetWindowTextW(modeSet[DcMethod] + _T(" - NetDisconnector"));




	RulesSet[0].enable = AfxGetApp()->GetProfileInt(_T("Settings"), _T("BlockUDPState"), 0);
	RulesSet[1].enable = AfxGetApp()->GetProfileInt(_T("Settings"), _T("BlockTCPState"), 0);
	if (RulesSet[0].enable && RulesSet[1].enable) {
		RulesSet[2].enable = true;
	}
	RulesSet[0].RuleName = _T("udp");
	RulesSet[1].RuleName = _T("tcp");
	RulesSet[2].RuleName = _T("all");
	RulesSet[0].cmdline_enable[0] = "netsh advfirewall firewall set rule name=\"NetDisconnector (UDP-In)\" new enable=yes";
	RulesSet[0].cmdline_enable[1] = "netsh advfirewall firewall set rule name=\"NetDisconnector (UDP-Out)\" new enable=yes";
	RulesSet[1].cmdline_enable[0] = "netsh advfirewall firewall set rule name=\"NetDisconnector (TCP-In)\" new enable=yes";
	RulesSet[1].cmdline_enable[1] = "netsh advfirewall firewall set rule name=\"NetDisconnector (TCP-Out)\" new enable=yes";
	RulesSet[0].cmdline_disable[0] = "netsh advfirewall firewall set rule name=\"NetDisconnector (UDP-In)\" new enable=no";
	RulesSet[0].cmdline_disable[1] = "netsh advfirewall firewall set rule name=\"NetDisconnector (UDP-Out)\" new enable=no";
	RulesSet[1].cmdline_disable[0] = "netsh advfirewall firewall set rule name=\"NetDisconnector (TCP-In)\" new enable=no";
	RulesSet[1].cmdline_disable[1] = "netsh advfirewall firewall set rule name=\"NetDisconnector (TCP-Out)\" new enable=no";




	//GetAdaptersInfo
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;
	UINT i;
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL)
		return 1;
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
		FREE(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(ulOutBufLen);
		if (pAdapterInfo == NULL)
			return 1;
	}
	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
		pAdapter = pAdapterInfo;
		while (pAdapter) {
			AdaptersSet[aIndex].AdapterNameToMatch = pAdapter->AdapterName;
			AdaptersSet[aIndex].Description = pAdapter->Description;
			AdaptersSet[aIndex].IpAddress = pAdapter->IpAddressList.IpAddress.String;
			AdaptersSet[aIndex].Gateway = pAdapter->GatewayList.IpAddress.String;
			if (AdaptersSet[aIndex].Gateway == "0.0.0.0") {
				AdaptersSet[aIndex].Connection = false;
				//AdaptersSet[aIndex].CConnection = "Disconnected";
			}
			aIndex++;

			pAdapter = pAdapter->Next;
		}
	}
	if (pAdapterInfo)//Release memory usage
		FREE(pAdapterInfo);



	//GetConnectionName
	HKEY phkResult;
	LPCWSTR path = TEXT("SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}");
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, path, 0, KEY_READ | KEY_WOW64_64KEY, &phkResult)) {
		return 1;
	}
	DWORD SubpathNum, SubpathLen;
	if (RegQueryInfoKey(phkResult, NULL, NULL, 0, &SubpathNum, &SubpathLen, NULL, NULL, NULL, NULL, NULL, NULL)) {
		return 1;
	}
	for (DWORD dwIndex = 0;dwIndex < SubpathNum; dwIndex++) {
		/*struct AdaptersSet {
			CString AdapterName;
			CString ValueName;
			CString ConnectionName;
			CString ValuePnPInstanceId;
			CString PnPInstanceId;
		}AdaptersSet[50];*/
		DWORD Namesize = SubpathLen++;
		LPSTR Subpath = new char[Namesize];
		RegEnumKeyExA(phkResult, dwIndex, Subpath, &Namesize, NULL, NULL, NULL, NULL);
		if (!strcmp(Subpath, "Descriptions")) {
			continue;
		}
		for (int matchIndex = 0; matchIndex < aIndex; matchIndex++) {
			AdaptersSet[matchIndex].AdapterName = Subpath;
			if (AdaptersSet[matchIndex].AdapterName == AdaptersSet[matchIndex].AdapterNameToMatch) {
				CString newPath = _T("SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\") + AdaptersSet[matchIndex].AdapterName + "\\Connection";
				HKEY phkResult;
				if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, newPath, 0, KEY_READ | KEY_WOW64_64KEY, &phkResult)) {
					return 1;
				}
				DWORD ValueNum, wValueNameLen, bValueLen;
				if (RegQueryInfoKey(phkResult, NULL, NULL, 0, NULL, NULL, NULL, &ValueNum, &wValueNameLen, &bValueLen, NULL, NULL)) {//Get connection infos
					return 1;
				}
				for (DWORD dwIndex_2 = 0; dwIndex_2 < ValueNum; dwIndex_2++) {
					DWORD ValueNameLen = ++wValueNameLen;
					DWORD ValueLen = ++bValueLen;
					LPSTR ValueName = (LPSTR)malloc(ValueNameLen);
					LPBYTE bValue = (LPBYTE)malloc(ValueLen);
					if (RegEnumValueA(phkResult, dwIndex_2, ValueName, &ValueNameLen, NULL, NULL, bValue, &ValueLen)) {//Enum connection names
						return 1;
					}
					size_t BufferSize = MultiByteToWideChar(CP_ACP, 0, (char*)bValue, strlen((char*)bValue), NULL, 0);
					WCHAR* Value = new WCHAR[BufferSize + 1];
					MultiByteToWideChar(CP_ACP, 0, (char*)bValue, strlen((char*)bValue), Value, BufferSize);
					Value[BufferSize] = '\0';
					free(bValue);//Release memory usage
					bValue = NULL;
					if ((CString)ValueName == "Name") {
						AdaptersSet[matchIndex].ValueName = ValueName;
						AdaptersSet[matchIndex].ConnectionName = Value;
					}
					else if ((CString)ValueName == "PnPInstanceId" || (CString)ValueName == "PnpInstanceID") {
						AdaptersSet[matchIndex].ValuePnPInstanceId = ValueName;
						AdaptersSet[matchIndex].PnPInstanceId = Value;
					}
					delete[] Value;//Release memory usage
					Value = NULL;
					free(ValueName);//Release memory usage
					ValueName = NULL;
				}
				if (AdaptersSet[matchIndex].PnPInstanceId.Left(4) == "PCI\\" || AdaptersSet[matchIndex].PnPInstanceId.Left(4) == "ROOT") {
					//m_adaptersList.AddString(AdaptersSet[matchIndex].ConnectionName + _T(" | ") + AdaptersSet[matchIndex].Description + _T(" | ") + AdaptersSet[matchIndex].Gateway + _T(" | ") + AdaptersSet[matchIndex].CConnection);//Add Connections to combobox
					m_adaptersList.AddString(AdaptersSet[matchIndex].ConnectionName);
				}
				matchIndex = aIndex;
			}
		}
		delete[] Subpath;//Release memory usage
		Subpath = NULL;

		//AdaptersSet[dwIndex].AdapterName = Subpath;
		//delete[] Subpath;//Release memory usage
		//Subpath = NULL;

		//CString newPath = _T("SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\") + AdaptersSet[dwIndex].AdapterName + "\\Connection";
		//HKEY phkResult;
		//if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, newPath, 0, KEY_READ | KEY_WOW64_64KEY, &phkResult)) {
		//	return 1;
		//}
		//DWORD ValueNum, wValueNameLen, bValueLen;
		//if (RegQueryInfoKey(phkResult, NULL, NULL, 0, NULL, NULL, NULL, &ValueNum, &wValueNameLen, &bValueLen, NULL, NULL)) {//Get connection infos
		//	return 1;
		//}
		//for (DWORD dwIndex_2 = 0;dwIndex_2 < ValueNum;dwIndex_2++) {
		//	DWORD ValueNameLen = ++wValueNameLen;
		//	DWORD ValueLen = ++bValueLen;
		//	LPSTR ValueName = (LPSTR)malloc(ValueNameLen);
		//	LPBYTE bValue = (LPBYTE)malloc(ValueLen);
		//	if (RegEnumValueA(phkResult, dwIndex_2, ValueName, &ValueNameLen, NULL, NULL, bValue, &ValueLen)) {//Enum connection names
		//		return 1;
		//	}
		//	size_t BufferSize = MultiByteToWideChar(CP_ACP, 0, (char*)bValue, strlen((char*)bValue), NULL, 0);
		//	WCHAR* Value = new WCHAR[BufferSize + 1];
		//	MultiByteToWideChar(CP_ACP, 0, (char*)bValue, strlen((char*)bValue), Value, BufferSize);
		//	Value[BufferSize] = '\0';
		//	free(bValue);//Release memory usage
		//	bValue = NULL;
		//	if ((CString)ValueName == "Name") {
		//		AdaptersSet[dwIndex].ValueName = ValueName;
		//		AdaptersSet[dwIndex].ConnectionName = Value;
		//	}
		//	else if ((CString)ValueName == "PnPInstanceId" || (CString)ValueName == "PnpInstanceID") {
		//		AdaptersSet[dwIndex].ValuePnPInstanceId = ValueName;
		//		AdaptersSet[dwIndex].PnPInstanceId = Value;
		//	}
		//	delete[] Value;//Release memory usage
		//	Value = NULL;
		//	free(ValueName);//Release memory usage
		//	ValueName = NULL;
		//}
		//if (AdaptersSet[dwIndex].PnPInstanceId.Left(4) == "PCI\\" || AdaptersSet[dwIndex].PnPInstanceId.Left(4) == "ROOT") {
		//	m_adaptersList.AddString(AdaptersSet[dwIndex].AdapterName);//Add AdapterName to combobox
		//	m_adaptersList.AddString(AdaptersSet[dwIndex].ConnectionName);//Add Connections to combobox
		//}
	}



	
	get_adapter = AfxGetApp()->GetProfileString(_T("Settings"), _T("Adapter"), NULL);//Get default adaptor
	int pos = m_adaptersList.FindStringExact(0, get_adapter);
	if (pos == -1) {
		for (int matchIndex = 0; matchIndex < aIndex; matchIndex++) {
			if (AdaptersSet[matchIndex].Connection) {
				pos = m_adaptersList.FindStringExact(0, AdaptersSet[matchIndex].ConnectionName);
				break;
			}
		}
		//pos = m_adaptersList.FindStringExact(0, _T("Local Area Connection"));//If pos eqaul to NULL, search for default adaptor
		//if (pos == -1) {
		//	pos = m_adaptersList.FindStringExact(0, _T("BDLJ"));
		//	if (pos == -1) {
		//		pos = m_adaptersList.FindStringExact(0, _T("Wi-Fi"));
		//		if (pos == -1) {
		//			pos = m_adaptersList.FindStringExact(0, _T("WLAN"));
		//			if (pos == -1) {
		//				pos = 0;//If cant find default adapter, set it to first one
		//			}
		//		}
		//	}
		//}
	}
	m_adaptersList.SetCurSel(pos);//Set selected adaptor
	m_adaptersList.GetLBText(m_adaptersList.GetCurSel(), get_adapter);//Save current select to value get_adapter




	get_vk_code = AfxGetApp()->GetProfileInt(_T("Settings"), _T("KeyVk"), VK_PAUSE);
	get_modifiers = AfxGetApp()->GetProfileInt(_T("Settings"), _T("KeyModifiers"), NULL);

	if (get_vk_code >= 0x21 && get_vk_code <= 0x2D || get_modifiers == 1 || get_modifiers == 3 || get_modifiers == 4 || get_modifiers == 6) {
		m_log += time + _T("This key may not work!!!") + _T("\r\n");
	}

	ATOM HotkeyID = GlobalAddAtom(_T("INITIAL_BIND")) - 0xc000;//Register hotkey
	if (FALSE == RegisterHotKey(GetSafeHwnd(), 1, get_modifiers | MOD_NOREPEAT, get_vk_code)) {
		m_log += time + _T("Register hotkey failed!!!");
	}
	else {
		get_hotkey = AfxGetApp()->GetProfileString(_T("Settings"), _T("Key"), _T("Pause"));//Get hotkey string
		for (int matchIndex = 0; matchIndex < aIndex; matchIndex++) {
			if (get_adapter == AdaptersSet[matchIndex].ConnectionName) {
				AdapterCurrent = matchIndex;
				break;
			}
		}
		if (DcMethod < 2) {
			if (AdaptersSet[AdapterCurrent].Connection) {
				m_log += time + _T("Press ") + get_hotkey.MakeUpper() + _T(" to disconnect");
			}
			else {
				m_log += time + _T("This adapter is disconnected... Press ") + get_hotkey.MakeUpper() + _T(" to connect");
			}
		}
		else if (DcMethod >= 2) {
			//Check system firewall
			if (!CheckSystemFirewall()) {
				m_log += time + _T("Failed to check firewall") + _T("\r\n");
			}
			/*HRESULT hr = S_OK;
			INetFwMgr* fwMgr = NULL;
			INetFwPolicy* fwPolicy = NULL;
			INetFwProfile* fwProfile;
			hr = CoCreateInstance(
				__uuidof(NetFwMgr),
				NULL,
				CLSCTX_INPROC_SERVER,
				__uuidof(INetFwMgr),
				(void**)&fwMgr
			);
			hr = fwMgr->get_LocalPolicy(&fwPolicy);
			if (FAILED(hr)) {
				m_log += time + _T("Failed to check firewall") + _T("\r\n");
			}
			hr = fwPolicy->get_CurrentProfile(&fwProfile);
			if (FAILED(hr)) {
				m_log += time + _T("Failed to check firewall") + _T("\r\n");
			}
			VARIANT_BOOL fwEnabled = NULL;
			hr = fwProfile->get_FirewallEnabled(&fwEnabled);
			if (FAILED(hr)) {
				m_log += time + _T("Failed to check firewall") + _T("\r\n");
			}
			if (fwEnabled != VARIANT_FALSE) {
				fwOn = TRUE;
			}
			else {
				fwOn = FALSE;
			}*/


			if (fwOn) {
				if (!RulesSet[DcMethod - 2].enable) {
					m_log += time + _T("Press ") + get_hotkey.MakeUpper() + _T(" to disconnect");
				}
				else {
					m_log += time + _T("Firewall rules activated... Press ") + get_hotkey.MakeUpper() + _T(" to connect");
				}
			}
			else {
				m_log += time + _T("Firewall is off") + _T("\r\n");
				m_log += time + _T("This mode can not work without firewall on");
			}
		}
	}
	



	//Generate command line
	for (int matchIndex = 0; matchIndex < aIndex; matchIndex++) {
		CString cmdline;
		LPWSTR temp;

		// Use this macro for Initializing the conversion.
		// Actually, some member variable initialization deep inside.
		USES_CONVERSION;
		
		cmdline = _T("cmd.exe /c ipconfig /release \"") + AdaptersSet[matchIndex].ConnectionName + _T("\">nul");
		temp = new WCHAR[cmdline.GetLength() + 1];
		wcscpy((LPTSTR)temp, T2W((LPTSTR)cmdline.GetBuffer(NULL)));
		cmdline.ReleaseBuffer();
		AdaptersSet[matchIndex].cmdline_disconnect[0] = temp;

		cmdline = _T("cmd.exe /c netsh interface set interface name=\"") + AdaptersSet[matchIndex].ConnectionName + _T("\" admin=disabled");
		temp = new WCHAR[cmdline.GetLength() + 1];
		wcscpy((LPTSTR)temp, T2W((LPTSTR)cmdline.GetBuffer(NULL)));
		cmdline.ReleaseBuffer();
		AdaptersSet[matchIndex].cmdline_disconnect[1] = temp;

		cmdline = _T("cmd.exe /c ipconfig /renew \"") + get_adapter + _T("\">nul");
		temp = new WCHAR[cmdline.GetLength() + 1];
		wcscpy((LPTSTR)temp, T2W((LPTSTR)cmdline.GetBuffer(NULL)));
		cmdline.ReleaseBuffer();
		AdaptersSet[matchIndex].cmdline_connect[0] = temp;

		cmdline = _T("cmd.exe /c netsh interface set interface name=\"") + get_adapter + _T("\" admin=enabled");
		temp = new WCHAR[cmdline.GetLength() + 1];
		wcscpy((LPTSTR)temp, T2W((LPTSTR)cmdline.GetBuffer(NULL)));
		cmdline.ReleaseBuffer();
		AdaptersSet[matchIndex].cmdline_connect[1] = temp;
	}




	BOOL ruledAdded = AfxGetApp()->GetProfileInt(_T("Settings"), _T("RuleAdded"), 0);
	if (ruledAdded == 0) {
		WinExec("netsh advfirewall firewall add rule name=\"NetDisconnector (UDP-In)\" dir=in action=block protocol=udp localport=6672 enable=no", SW_HIDE);
		WinExec("netsh advfirewall firewall add rule name=\"NetDisconnector (UDP-Out)\" dir=out action=block protocol=udp localport=6672 enable=no", SW_HIDE);
		WinExec("netsh advfirewall firewall add rule name=\"NetDisconnector (TCP-In)\" dir=in action=block protocol=tcp enable=no", SW_HIDE);
		WinExec("netsh advfirewall firewall add rule name=\"NetDisconnector (TCP-Out)\" dir=out action=block protocol=tcp enable=no", SW_HIDE);
		ruledAdded = true;
	}
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("RuleAdded"), ruledAdded);
	



	UpdateData(false);
	m_logCtrl.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);


	GetDlgItem(IDC_LOG)->SetFocus();
	

	CRect rcParent;//Get control size differences
	GetClientRect(&rcParent);

	WINDOWPLACEMENT wndpl;

	m_logCtrl.GetWindowPlacement(&wndpl);
	logWidthDiff = rcParent.Width() - wndpl.rcNormalPosition.right;
	logHeightDiff = rcParent.Height() - wndpl.rcNormalPosition.bottom;
	
	m_adaptersList.GetWindowPlacement(&wndpl);
	comboWidthDiff = rcParent.Width() - wndpl.rcNormalPosition.right;

	return FALSE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNetDisconnectorDlg::OnPaint()
{
	/*CRect rec;
	CPaintDC dc(this);
	GetDlgItem(IDC_LOG)->GetWindowRect(&rec);
	ScreenToClient(&rec);
	dc.FillSolidRect(rec, RGB(12, 12, 12));*/

	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CNetDisconnectorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//static bool connection = true;
static bool timeout = false;
static bool waiting = false;
static bool reconnectCountdown = false;
int AdapterWaitingforchange = -1;


void CNetDisconnectorDlg::OnCbnSelchangeCombo()
{
	UpdateData(true);

	if (reconnectCountdown) {
		CString adapterTemp;
		m_adaptersList.GetLBText(m_adaptersList.GetCurSel(), adapterTemp);
		//connection = true;
		for (int matchIndex = 0; matchIndex < aIndex; matchIndex++) {
			if (adapterTemp == AdaptersSet[matchIndex].ConnectionName) {
				AdapterWaitingforchange = matchIndex;
				break;
			}
		}
		return;
	}

	m_adaptersList.GetLBText(m_adaptersList.GetCurSel(), get_adapter);//Get adaptor name
	//connection = true;
	for (int matchIndex = 0; matchIndex < aIndex; matchIndex++) {
		if (get_adapter == AdaptersSet[matchIndex].ConnectionName) {
			AdapterCurrent = matchIndex;
			break;
		}
	}

	AddLog(_T("Target adapter set to ") + get_adapter);


	if (DcMethod < 2) {
		if (AdaptersSet[AdapterCurrent].Connection) {
			AddLog(_T("Press ") + get_hotkey.MakeUpper() + _T(" to disconnect"));
		}
		else {
			AddLog(_T("This adapter is disconnected... Press ") + get_hotkey.MakeUpper() + _T(" to connect"));
		}
	}
}


void CNetDisconnectorDlg::OnCbnCloseupCombo()
{
	GetDlgItem(IDC_OLDLOG)->SetFocus();
}


void CNetDisconnectorDlg::OnEnSetfocusLog()
{
	MSG msg;
	PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);

	m_logCtrl.SetSel(-1, -1);
	m_logCtrl.HideCaret();
	GetDlgItem(IDC_OLDLOG)->SetFocus();
}


void CNetDisconnectorDlg::OnOptionsChangehotkey()
{
	UnregisterHotKey(GetSafeHwnd(), 1); //Unbind all hotkey

	INT_PTR result;
	Record RecDlg;
	RecDlg.m_RecordLabel = get_hotkey;
	result = RecDlg.DoModal();
	if (IDCANCEL == result || RecDlg.input_vk_code == 0x00) {  //If press cancel then quit or If press nothing then quit
		ATOM HotkeyID = GlobalAddAtom(_T("INITIAL_REBIND")) - 0xc000; //Register hotkey
		RegisterHotKey(GetSafeHwnd(), 1, AfxGetApp()->GetProfileInt(_T("Settings"), _T("KeyModifiers"), NULL) | MOD_NOREPEAT, AfxGetApp()->GetProfileInt(_T("Settings"), _T("KeyVk"), VK_PAUSE));//Register hotkey
		return;
	}

	ATOM HotkeyID = GlobalAddAtom(_T("REBIND")) - 0xc000; //Register new hotkey
	BOOL done = RegisterHotKey(GetSafeHwnd(), 1, RecDlg.input_modifiers | MOD_NOREPEAT, RecDlg.input_vk_code);
	get_modifiers = RecDlg.input_modifiers;
	get_vk_code = RecDlg.input_vk_code;
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("KeyModifiers"), get_modifiers);
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("KeyVk"), get_vk_code);

	UpdateData(true);

	get_hotkey = RecDlg.input_key; //Add keys which have issue here
	if (get_vk_code == 0x13) {
		get_hotkey = "Pause";
	}
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
	if (get_vk_code >= 0x21 && get_vk_code <= 0x2D || get_modifiers == 1 || get_modifiers == 3 || get_modifiers == 4 || get_modifiers == 6) {
		AddLog(_T("This key may not work!!!"));
	}

	AfxGetApp()->WriteProfileString(_T("Settings"), _T("Key"), get_hotkey);

	if (done) {
		for (int matchIndex = 0; matchIndex < aIndex; matchIndex++) {
			if (get_adapter == AdaptersSet[matchIndex].ConnectionName) {
				AdapterCurrent = matchIndex;
				break;
			}
		}
		if (DcMethod < 2) {
			if (AdaptersSet[AdapterCurrent].Connection) {
				AddLog(_T("Press ") + get_hotkey.MakeUpper() + _T(" to disconnect"));
			}
			else {
				AddLog(_T("Press ") + get_hotkey.MakeUpper() + _T(" again to reconnect"));
			}
		}
		else {
			if (fwOn) {
				if (!RulesSet[DcMethod - 2].enable) {
					AddLog(_T("Press ") + get_hotkey.MakeUpper() + _T(" to disconnect"));
				}
				else {
					AddLog(_T("Firewall rules activated... Press ") + get_hotkey.MakeUpper() + _T(" to connect"));
				}
			}
			else {
				AddLog(_T("Firewall is off"));
				AddLog(_T("This mode can not work without firewall on"));
			}
		}
	}
	else{
		AddLog(_T("Register hotkey failed!!!"));
	}
}


void CNetDisconnectorDlg::OnOptionsAutoreconnect()
{
	//IDD_AUTORECONNECT_DIALOG
	INT_PTR result;
	AutoReonnect AutDlg;
	CString output_delay;
	UINT AutoReconnectStateOld = AutoReconnectState;
	UINT ARApplytoFirewallStateOld = ARApplytoFirewallState;
	AutDlg.input_autoReconnect = AutoReconnectState;
	AutDlg.input_applytofirewall = ARApplytoFirewallState;
	output_delay.Format(_T("%d"), reconnectDelay);
	AutDlg.m_delay = output_delay;
	AutDlg.DoModal();
	AutoReconnectState = AutDlg.input_autoReconnect;
	ARApplytoFirewallState = AutDlg.input_applytofirewall;
	reconnectDelay = _ttoi(AutDlg.m_delay);

	UpdateData(true);

	if (AutoReconnectState != AutoReconnectStateOld) {
		if (AutoReconnectState == BST_UNCHECKED) {
			KillTimer(2);

			AddLog(_T("Auto reconnect disabled"));
		}
		else if (AutoReconnectState == BST_CHECKED) {
			AddLog(_T("Auto reconnect enabled"));
		}
	}
	if (ARApplytoFirewallState != ARApplytoFirewallStateOld) {
		if (ARApplytoFirewallState == BST_UNCHECKED) {
			KillTimer(2);

			AddLog(_T("Auto reconnect for Firewall Methods disabled"));
		}
		else if (ARApplytoFirewallState == BST_CHECKED) {
			AddLog(_T("Auto reconnect for Firewall Methods enabled"));
		}
	}
}


//DWORD WINAPI ThreadProc(_In_ LPVOID lpParameter) {
//	CNetDisconnectorDlg *pDlg = (CNetDisconnectorDlg*)lpParameter;//pDlg->
//	
//	pDlg->UpdateData(true);
//	SYSTEMTIME systime;
//	CString time;
//	
//	if (connection) {
//		CString release;
//		if (pDlg->DcMethod == 0) {
//			release = _T("cmd.exe /c ipconfig /release \"") + pDlg->get_adapter + _T("\">nul");
//		}
//		else if (pDlg->DcMethod == 1) {
//			release = _T("cmd.exe /c netsh interface set interface name=\"") + pDlg->get_adapter + _T("\" admin=disabled");
//		}
//
//		USES_CONVERSION;
//		LPWSTR temp = new WCHAR[release.GetLength() + 1];
//		wcscpy((LPTSTR)temp, T2W((LPTSTR)release.GetBuffer(NULL)));
//		release.ReleaseBuffer();
//
//		STARTUPINFO si = { sizeof(si) };
//		si.dwFlags = STARTF_USESHOWWINDOW;
//		si.wShowWindow = SW_HIDE;
//		PROCESS_INFORMATION pi;
//		ZeroMemory(&pi, sizeof(pi));
//
//		CreateProcess(NULL, temp, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
//
//		GetLocalTime(&systime);
//		time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);
//		
//		pDlg->m_log += _T("\r\n") + time + _T("Disconnecting...");
//		pDlg->UpdateData(false);
//		pDlg->m_logCtrl.LineScroll(pDlg->m_logCtrl.GetLineCount(), 0);
//
//		if (WaitForSingleObject(pi.hProcess, 12000) == WAIT_TIMEOUT) {//Wait until process is complete then play notification sound
//			WinExec("cmd.exe /c taskkill /im ipconfig.exe /t /f", SW_HIDE);//Terminate timeout process
//			GetLocalTime(&systime);
//			time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);
//
//			pDlg->m_log += _T("\r\n") + time + _T("Disconnecting timeout!!!");
//			pDlg->m_log += _T("\r\n") + time + _T("Target adapter may have no internet connection or disabled DHCP");
//			pDlg->UpdateData(false);
//			pDlg->m_logCtrl.LineScroll(pDlg->m_logCtrl.GetLineCount(), 0);
//		}
//	}
//	else {
//		CString renew;
//		if (pDlg->DcMethod == 0) {
//			renew = _T("cmd.exe /c ipconfig /renew \"") + pDlg->get_adapter + _T("\">nul");
//		}
//		else if (pDlg->DcMethod == 1) {
//			renew = _T("cmd.exe /c netsh interface set interface name=\"") + pDlg->get_adapter + _T("\" admin=enabled");
//		}
//
//		USES_CONVERSION;
//		LPWSTR temp = new WCHAR[renew.GetLength() + 1];
//		wcscpy((LPTSTR)temp, T2W((LPTSTR)renew.GetBuffer(NULL)));
//		renew.ReleaseBuffer();
//
//		STARTUPINFO si = { sizeof(si) };
//		si.dwFlags = STARTF_USESHOWWINDOW;
//		si.wShowWindow = SW_HIDE;
//		PROCESS_INFORMATION pi;
//		ZeroMemory(&pi, sizeof(pi));
//
//		CreateProcess(NULL, temp, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
//
//		GetLocalTime(&systime);
//		time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);
//
//		pDlg->m_log += _T("\r\n") + time + _T("Connecting...");
//		pDlg->UpdateData(false);
//		pDlg->m_logCtrl.LineScroll(pDlg->m_logCtrl.GetLineCount(), 0);
//
//		if (WaitForSingleObject(pi.hProcess, 12000) == WAIT_TIMEOUT) {//Wait until process is complete then play notification sound
//			WinExec("cmd.exe /c taskkill /im ipconfig.exe /t /f", SW_HIDE);//Terminate timeout process
//			GetLocalTime(&systime);
//			time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);
//
//			pDlg->m_log += _T("\r\n") + time + _T("Connecting timeout!!!");
//			pDlg->m_log += _T("\r\n") + time + _T("Target adapter may have no internet connection or disabled DHCP");
//			pDlg->UpdateData(false);
//			pDlg->m_logCtrl.LineScroll(pDlg->m_logCtrl.GetLineCount(), 0);
//		}
//	}
//	return true;
//}


void CNetDisconnectorDlg::OnNetworkDisconnect()
{
	UpdateData(true);

	if (DcMethod >= 2) {
		if (DcMethod - 2 != 2) {
			if (RulesSet[DcMethod - 2].enable == 0) {
				WinExec(RulesSet[DcMethod - 2].cmdline_enable[0], SW_HIDE);
				WinExec(RulesSet[DcMethod - 2].cmdline_enable[1], SW_HIDE);
				RulesSet[DcMethod - 2].enable = TRUE;
			}
		}
		else {
			if (RulesSet[0].enable == 0) {
				WinExec(RulesSet[0].cmdline_enable[0], SW_HIDE);
				WinExec(RulesSet[0].cmdline_enable[1], SW_HIDE);
				RulesSet[0].enable = TRUE;
			}
			if (RulesSet[1].enable == 0) {
				WinExec(RulesSet[1].cmdline_enable[0], SW_HIDE);
				WinExec(RulesSet[1].cmdline_enable[1], SW_HIDE);
				RulesSet[1].enable = TRUE;
			}
		}

		if (RulesSet[0].enable == TRUE && RulesSet[1].enable == TRUE) {
			RulesSet[2].enable = TRUE;
		}
		else {
			RulesSet[2].enable = FALSE;
		}
		AddLog(_T("Firewall rules activated ") + RulesSet[DcMethod - 2].RuleName.MakeUpper() + _T(" blocked"));
		return;
	}

	if (AdaptersSet[AdapterCurrent].Connection) {
		STARTUPINFO si = { sizeof(si) };
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
		PROCESS_INFORMATION pi;
		ZeroMemory(&pi, sizeof(pi));

		CreateProcess(NULL, AdaptersSet[AdapterCurrent].cmdline_disconnect[DcMethod], NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

		//GetLocalTime(&systime);
		//time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

		//m_log += _T("\r\n") + time + _T("Disconnecting...");
		AddLog(_T("Disconnecting..."));
		//UpdateData(false);
		//m_logCtrl.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

		SetTimer(1, 15000, NULL);
		waiting = !waiting;
		MSG msg;
		while (true) {
			if (MsgWaitForMultipleObjects(1, &pi.hThread, FALSE, 15000, QS_ALLEVENTS) == WAIT_OBJECT_0) {
				//GetLocalTime(&systime);
				//time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

				//m_log += _T("\r\n") + time + _T("Disconnected");
				AddLog(_T("Disconnected"));
				//UpdateData(false);
				//m_logCtrl.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
				break;
			}
			else if (timeout) {
				WinExec("cmd.exe /c taskkill /im ipconfig.exe /t /f", SW_HIDE);//Terminate timeout process
				//GetLocalTime(&systime);
				//time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

				//m_log += _T("\r\n") + time + _T("Disconnecting timeout!!!");
				//m_log += _T("\r\n") + time + _T("Adapter may have no internet connection or disabled DHCP");
				AddLog(_T("Disconnecting timeout!!!"));
				AddLog(_T("Adapter may have no internet connection or disabled DHCP"));
				//UpdateData(false);
				//m_logCtrl.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

				timeout = !timeout;
				break;
			}
			else {
				PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
				DispatchMessage(&msg);
			}
			Sleep(1);
		}
		KillTimer(1);
		waiting = !waiting;

		//if (WaitForSingleObject(pi.hProcess, 12000) == WAIT_TIMEOUT) {//Wait until process is complete then play notification sound
		//	WinExec("cmd.exe /c taskkill /im ipconfig.exe /t /f", SW_HIDE);//Terminate timeout process
		//	GetLocalTime(&systime);
		//	time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

		//	m_log += _T("\r\n") + time + _T("Disconnecting timeout!!!");
		//	m_log += _T("\r\n") + time + _T("Target adapter may have no internet connection or disabled DHCP");
		//	UpdateData(false);
		//	m_logCtrl.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		//}

		AdaptersSet[AdapterCurrent].Connection = !AdaptersSet[AdapterCurrent].Connection;
	}
	else {
		//GetLocalTime(&systime);
		//time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

		//m_log += _T("\r\n") + time + _T("Disconnected");
		AddLog(_T("Disconnected"));
		//UpdateData(false);
		//m_logCtrl.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}
}


void CNetDisconnectorDlg::OnNetworkConnect()
{
	UpdateData(true);

	if (DcMethod >= 2) {
		if (DcMethod - 2 != 2) {
			if (RulesSet[DcMethod - 2].enable == 1) {
				WinExec(RulesSet[DcMethod - 2].cmdline_disable[0], SW_HIDE);
				WinExec(RulesSet[DcMethod - 2].cmdline_disable[1], SW_HIDE);
				RulesSet[DcMethod - 2].enable = FALSE;
			}
		}
		else {
			if (RulesSet[0].enable == 1) {
				WinExec(RulesSet[0].cmdline_disable[0], SW_HIDE);
				WinExec(RulesSet[0].cmdline_disable[1], SW_HIDE);
				RulesSet[0].enable = FALSE;
			}
			if (RulesSet[1].enable == 1) {
				WinExec(RulesSet[1].cmdline_disable[0], SW_HIDE);
				WinExec(RulesSet[1].cmdline_disable[1], SW_HIDE);
				RulesSet[1].enable = FALSE;
			}
		}

		if (RulesSet[0].enable == TRUE && RulesSet[1].enable == TRUE) {
			RulesSet[2].enable = TRUE;
		}
		else {
			RulesSet[2].enable = FALSE;
		}
		AddLog(_T("Firewall rules deactivated ") + RulesSet[DcMethod - 2].RuleName.MakeUpper() + _T(" unblocked"));
		return;
	}

	if (!AdaptersSet[AdapterCurrent].Connection) {
		STARTUPINFO si = { sizeof(si) };
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
		PROCESS_INFORMATION pi;
		ZeroMemory(&pi, sizeof(pi));

		CreateProcess(NULL, AdaptersSet[AdapterCurrent].cmdline_connect[DcMethod], NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

		//GetLocalTime(&systime);
		//time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

		//m_log += _T("\r\n") + time + _T("Connecting...");
		AddLog(_T("Connecting..."));
		//UpdateData(false);
		//m_logCtrl.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

		SetTimer(1, 15000, NULL);
		waiting = !waiting;
		MSG msg;
		while (true) {
			if (MsgWaitForMultipleObjects(1, &pi.hThread, FALSE, 15000, QS_ALLEVENTS) == WAIT_OBJECT_0) {
				//GetLocalTime(&systime);
				//time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

				//m_log += _T("\r\n") + time + _T("Connected");
				AddLog(_T("Connected"));
				//UpdateData(false);
				//m_logCtrl.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
				break;
			}
			else if (timeout) {
				WinExec("cmd.exe /c taskkill /im ipconfig.exe /t /f", SW_HIDE);//Terminate timeout process
				//GetLocalTime(&systime);
				//time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

				//m_log += _T("\r\n") + time + _T("Connecting timeout!!!");
				//m_log += _T("\r\n") + time + _T("Adapter may have no internet connection or disabled DHCP");
				AddLog(_T("Connecting timeout!!!"));
				AddLog(_T("Adapter may have no internet connection or disabled DHCP"));
				//UpdateData(false);
				//m_logCtrl.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

				timeout = !timeout;
				break;
			}
			else {
				PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
				DispatchMessage(&msg);
			}
			Sleep(1);
		}
		KillTimer(1);
		waiting = !waiting;

		//if (WaitForSingleObject(pi.hProcess, 12000) == WAIT_TIMEOUT) {//Wait until process is complete then play notification sound
		//	WinExec("cmd.exe /c taskkill /im ipconfig.exe /t /f", SW_HIDE);//Terminate timeout process
		//	GetLocalTime(&systime);
		//	time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

		//	m_log += _T("\r\n") + time + _T("Connecting timeout!!!");
		//	m_log += _T("\r\n") + time + _T("Target adapter may have no internet connection or disabled DHCP");
		//	UpdateData(false);
		//	m_logCtrl.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		//}

		AdaptersSet[AdapterCurrent].Connection = !AdaptersSet[AdapterCurrent].Connection;
	}
	else {
		//GetLocalTime(&systime);
		//time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

		//m_log += _T("\r\n") + time + _T("Connected");
		AddLog(_T("Connected"));
		//UpdateData(false);
		//m_logCtrl.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}
}


void CNetDisconnectorDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	if (nHotKeyId && !waiting) {//If match the HotKeyId then continue
		KillTimer(2);
		UpdateData(true);
		//SYSTEMTIME systime;
		//CString time;


		/*DWORD dwID;//Running in another thread
		HANDLE hThread;
		hThread = CreateThread(0, 0, ThreadProc, this, 0, &dwID);

		DWORD result;
		MSG msg;
		while (true)
		{
			if (MsgWaitForMultipleObjects(1, &hThread, FALSE, INFINITE, QS_ALLEVENTS) == WAIT_OBJECT_0) {
				break;
			}
			else {
				PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
				DispatchMessage(&msg);
				
			}
			Sleep(1);
		}*/

		if (DcMethod >= 2) {
			if (RulesSet[DcMethod - 2].enable == 0) {
				if (DcMethod - 2 != 2) {
					if (RulesSet[DcMethod - 2].enable == 0) {
						WinExec(RulesSet[DcMethod - 2].cmdline_enable[0], SW_HIDE);
						WinExec(RulesSet[DcMethod - 2].cmdline_enable[1], SW_HIDE);
						RulesSet[DcMethod - 2].enable = TRUE;
					}
				}
				else {
					WinExec(RulesSet[0].cmdline_enable[0], SW_HIDE);
					WinExec(RulesSet[0].cmdline_enable[1], SW_HIDE);
					RulesSet[0].enable = TRUE;
					WinExec(RulesSet[1].cmdline_enable[0], SW_HIDE);
					WinExec(RulesSet[1].cmdline_enable[1], SW_HIDE);
					RulesSet[1].enable = TRUE;
					RulesSet[2].enable = TRUE;
				}

				if (DcSoundState == MF_CHECKED)
					PlaySound(MAKEINTRESOURCE(IDR_WAVE1), AfxGetResourceHandle(), SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);

				AddLog(_T("Firewall rules activated ") + RulesSet[DcMethod - 2].RuleName.MakeUpper() + _T(" blocked"));

				if (AutoReconnectState == BST_CHECKED && ARApplytoFirewallState == BST_CHECKED) {
					CString temp;
					temp.Format(_T("%d"), reconnectDelay / 1000);
					AddLog(_T("Auto reconnect after ") + temp + _T("s... Press ") + get_hotkey.MakeUpper() + _T(" again to connect now"));
					
					SetTimer(2, reconnectDelay, NULL);
					reconnectCountdown = true;
				}
				else {
					AddLog(_T("Press ") + get_hotkey.MakeUpper() + _T(" again to reconnect"));
				}
			}
			else if (RulesSet[DcMethod - 2].enable == 1) {
				if (DcMethod - 2 != 2) {
					WinExec(RulesSet[DcMethod - 2].cmdline_disable[0], SW_HIDE);
					WinExec(RulesSet[DcMethod - 2].cmdline_disable[1], SW_HIDE);
					RulesSet[DcMethod - 2].enable = FALSE;
				}
				else {
					WinExec(RulesSet[0].cmdline_disable[0], SW_HIDE);
					WinExec(RulesSet[0].cmdline_disable[1], SW_HIDE);
					RulesSet[0].enable = FALSE;
					WinExec(RulesSet[1].cmdline_disable[0], SW_HIDE);
					WinExec(RulesSet[1].cmdline_disable[1], SW_HIDE);
					RulesSet[1].enable = FALSE;
					RulesSet[2].enable = FALSE;
				}

				if (RcSoundState == MF_CHECKED)
					PlaySound(MAKEINTRESOURCE(IDR_WAVE2), AfxGetResourceHandle(), SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);

				AddLog(_T("Firewall rules deactivated ") + RulesSet[DcMethod - 2].RuleName.MakeUpper() + _T(" unblocked"));
				AddLog(_T("Press ") + get_hotkey.MakeUpper() + _T(" to disconnect"));
			}

			if (RulesSet[0].enable == TRUE && RulesSet[1].enable == TRUE) {
				RulesSet[2].enable = TRUE;
			}
			else {
				RulesSet[2].enable = FALSE;
			}
			return;
		}

		if (AdaptersSet[AdapterCurrent].Connection) {
			AdaptersSet[AdapterCurrent].Connection = !AdaptersSet[AdapterCurrent].Connection;

			STARTUPINFO si = {sizeof(si)};
			si.dwFlags = STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_HIDE;
			PROCESS_INFORMATION pi;
			ZeroMemory(&pi, sizeof(pi));

			CreateProcess(NULL, AdaptersSet[AdapterCurrent].cmdline_disconnect[DcMethod], NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

			//GetLocalTime(&systime);
			//time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

			//m_log += _T("\r\n") + time + _T("Disconnecting...");
			AddLog(_T("Disconnecting..."));
			//UpdateData(false);
			//m_logCtrl.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

			SetTimer(1, 15000, NULL);
			waiting = !waiting;
			MSG msg;
			while (true) {
				if (MsgWaitForMultipleObjects(1, &pi.hThread, FALSE, 15000, QS_ALLEVENTS) == WAIT_OBJECT_0) {
					if (DcSoundState == MF_CHECKED)
						PlaySound(MAKEINTRESOURCE(IDR_WAVE1), AfxGetResourceHandle(), SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);
					break;
				}
				else if (timeout) {
					WinExec("cmd.exe /c taskkill /im ipconfig.exe /t /f", SW_HIDE);//Terminate timeout process
					//GetLocalTime(&systime);
					//time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

					//m_log += _T("\r\n") + time + _T("Disconnecting timeout!!!");
					
					//m_log += _T("\r\n") + time + _T("Adapter may have no internet connection or disabled DHCP");
					AddLog(_T("Disconnecting timeout!!!"));
					AddLog(_T("Adapter may have no internet connection or disabled DHCP"));
					//UpdateData(false);
					//m_logCtrl.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

					timeout = !timeout;
					break;
				}
				else {
					PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
					DispatchMessage(&msg);
				}
				Sleep(1);
			}
			KillTimer(1);
			waiting = !waiting;

			//if (WaitForSingleObject(pi.hProcess, 12000) == WAIT_TIMEOUT) {//Wait until process is complete then play notification sound
			//	WinExec("cmd.exe /c taskkill /im ipconfig.exe /t /f", SW_HIDE);//Terminate timeout process
			//	GetLocalTime(&systime);
			//	time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

			//	m_log += _T("\r\n") + time + _T("Disconnecting timeout!!!");
			//	m_log += _T("\r\n") + time + _T("Target adapter may have no internet connection or disabled DHCP");
			//	UpdateData(false);
			//	m_logCtrl.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
			//}

			if (AutoReconnectState == BST_CHECKED) {
				CString temp;
				temp.Format(_T("%d"), reconnectDelay / 1000);
				AddLog(_T("Auto reconnect after ") + temp + _T("s... Press ") + get_hotkey.MakeUpper() + _T(" again to connect now"));

				SetTimer(2, reconnectDelay, NULL);
				reconnectCountdown = true;
			}
			else {
				AddLog(_T("Press ") + get_hotkey.MakeUpper() + _T(" again to reconnect"));
			}
		}
		else {
			AdaptersSet[AdapterCurrent].Connection = !AdaptersSet[AdapterCurrent].Connection;

			STARTUPINFO si = {sizeof(si)};
			si.dwFlags = STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_HIDE;
			PROCESS_INFORMATION pi;
			ZeroMemory(&pi, sizeof(pi));

			CreateProcess(NULL, AdaptersSet[AdapterCurrent].cmdline_connect[DcMethod], NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

			reconnectCountdown = false;

			//GetLocalTime(&systime);
			//time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

			//m_log += _T("\r\n") + time + _T("Connecting...");
			AddLog(_T("Connecting..."));
			//UpdateData(false);
			//m_logCtrl.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

			SetTimer(1, 15000, NULL);
			waiting = !waiting;
			MSG msg;
			while (true){
				if (MsgWaitForMultipleObjects(1, &pi.hThread, FALSE, 15000, QS_ALLEVENTS) == WAIT_OBJECT_0) {
					if (RcSoundState == MF_CHECKED)
						PlaySound(MAKEINTRESOURCE(IDR_WAVE2), AfxGetResourceHandle(), SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);
					break;
				}
				else if (timeout) {
					WinExec("cmd.exe /c taskkill /im ipconfig.exe /t /f", SW_HIDE);//Terminate timeout process
					//GetLocalTime(&systime);
					//time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

					//m_log += _T("\r\n") + time + _T("Connecting timeout!!!");
					//m_log += _T("\r\n") + time + _T("Adapter may have no internet connection or disabled DHCP");
					AddLog(_T("Connecting timeout!!!"));
					AddLog(_T("Adapter may have no internet connection or disabled DHCP"));
					//UpdateData(false);
					//m_logCtrl.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

					timeout = !timeout;

					break;
				}
				else {
					PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);

					/*GetLocalTime(&systime);
					time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);
					CString countdown;
					CString temp;
					countdown.Format(_T("%d"), i);
					if (msg.message == WM_TIMER)
						temp = _T("WM_TIMER");
					else if (msg.message == WM_MOUSEMOVE)
						temp = _T("WM_MOUSEMOVE");
					else if (msg.message == WM_KEYUP)
						temp = _T("WM_KEYUP");
					else
						temp.Format(_T("%d "), msg.message);
					m_log += _T("\r\n") + time + countdown + temp;
					UpdateData(false);
					m_logCtrl.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);*/

					DispatchMessage(&msg);
				}
				Sleep(1);
			}
			KillTimer(1);
			waiting = !waiting;

			//if (WaitForSingleObject(pi.hProcess, 12000) == WAIT_TIMEOUT) {//Wait until process is complete then play notification sound
			//	WinExec("cmd.exe /c taskkill /im ipconfig.exe /t /f", SW_HIDE);//Terminate timeout process
			//	GetLocalTime(&systime);
			//	time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

			//	m_log += _T("\r\n") + time + _T("Connecting timeout!!!");
			//	m_log += _T("\r\n") + time + _T("Target adapter may have no internet connection or disabled DHCP");
			//	UpdateData(false);
			//	m_logCtrl.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
			//}

			if (AdapterWaitingforchange != -1) {
				m_adaptersList.GetLBText(m_adaptersList.GetCurSel(), get_adapter);//Get adaptor name
				AdapterCurrent = AdapterWaitingforchange;
				AdapterWaitingforchange = -1;

				//SYSTEMTIME systime;
				//CString time;
				//GetLocalTime(&systime);
				//time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

				//m_log += _T("\r\n") + time + _T("Target adapter set to ") + get_adapter;
				AddLog(_T("Target adapter set to ") + get_adapter);

				if (AdaptersSet[AdapterCurrent].Connection) {
					//m_log += _T("\r\n") + time + _T("Press ") + get_hotkey.MakeUpper() + _T(" to disconnect");
					AddLog(_T("Press ") + get_hotkey.MakeUpper() + _T(" to disconnect"));
				}
				else {
					//m_log += _T("\r\n") + time + _T("This adapter is disconnected... Press ") + get_hotkey.MakeUpper() + _T(" to connect");
					AddLog(_T("This adapter is disconnected... Press ") + get_hotkey.MakeUpper() + _T(" to connect"));
				}

				//UpdateData(false);
				//m_logCtrl.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

				return;
			}
			//GetLocalTime(&systime);
			//time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

			//m_log += _T("\r\n") + time + _T("Press ") + get_hotkey.MakeUpper() +_T(" to disconnect");
			AddLog(_T("Press ") + get_hotkey.MakeUpper() + _T(" to disconnect"));
			//UpdateData(false);
			//m_logCtrl.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		}
	}
	CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}


void CNetDisconnectorDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case 1:
		KillTimer(1);
		timeout = !timeout;
	case 2:
		KillTimer(2);
		if (DcMethod < 2) {
			if (!AdaptersSet[AdapterCurrent].Connection) {
				if (!waiting) {
					OnHotKey(1, 0, 0);
				}
			}
		}
		else if (DcMethod >= 2) {
			if (RulesSet[DcMethod - 2].enable == 1) {
				if (!waiting) {
					OnHotKey(1, 0, 0);
				}
			}
		}
	default:
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CNetDisconnectorDlg::AddLog(CString message)
{
	SYSTEMTIME systime;
	CString time;
	GetLocalTime(&systime);
	time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

	m_log += _T("\r\n") + time + message;

	m_logCtrl.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	UpdateData(false); //Update variable to control
}


BOOL CNetDisconnectorDlg::CheckSystemFirewall()
{
	HRESULT hr = S_OK;
	INetFwMgr* fwMgr = NULL;
	INetFwPolicy* fwPolicy = NULL;
	INetFwProfile* fwProfile;
	hr = CoCreateInstance(__uuidof(NetFwMgr), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwMgr), (void**)&fwMgr);
	BOOL error = false;
	hr = fwMgr->get_LocalPolicy(&fwPolicy);
	if (FAILED(hr))
		error = true;
	hr = fwPolicy->get_CurrentProfile(&fwProfile);
	if (FAILED(hr))
		error = true;
	VARIANT_BOOL fwEnabled = NULL;
	hr = fwProfile->get_FirewallEnabled(&fwEnabled);
	if (FAILED(hr))
		error = true;

	if (fwEnabled != VARIANT_FALSE)
		fwOn = TRUE;
	else
		fwOn = FALSE;

	if (fwPolicy != NULL)
		fwPolicy->Release();
	if (fwMgr != NULL)
		fwMgr->Release();
	if (fwProfile != NULL)
		fwProfile->Release();

	if (error)
		return FALSE;
	else
		return TRUE;
}


HBRUSH CNetDisconnectorDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	/*if (pWnd->GetDlgCtrlID() == IDC_LOG) {
		pDC->SetTextColor(RGB(204, 204, 204));
		pDC->SetBkColor(RGB(12, 12, 12));
		hbr = m_brush;
	}*/

	// TODO:  Change any attributes of the DC here

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}


void CNetDisconnectorDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("DcSound"), DcSoundState);
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("RcSound"), RcSoundState);
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("DcMethod"), DcMethod);
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("AutoReconnect"), AutoReconnectState);
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("AutoReconnectFirewall"), ARApplytoFirewallState);
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("ReconnectDelay"), reconnectDelay);
	AfxGetApp()->WriteProfileString(_T("Settings"), _T("Adapter"), get_adapter);//Save current adapter
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("BlockUDPState"), RulesSet[0].enable);
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("BlockTCPState"), RulesSet[1].enable);
	UnregisterHotKey(GetSafeHwnd(), 1);
}


void CNetDisconnectorDlg::OnClose()
{
	WINDOWPLACEMENT wndpl;//Save last window position
	GetWindowPlacement(&wndpl);
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("WindowPosX"), wndpl.rcNormalPosition.left);
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("WindowPosY"), wndpl.rcNormalPosition.top);
	wndpl.rcNormalPosition.right = 506 + wndpl.rcNormalPosition.left;
	
	CDialogEx::OnClose();
}


void CNetDisconnectorDlg::PreInitDialog()
{
	WINDOWPLACEMENT wndpl;//Restore last window position
	GetWindowPlacement(&wndpl);
	RECT workarea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &workarea, 0);
	CRect dlgrect;
	GetWindowRect(dlgrect);//Dialog Original Size 506*384

	wndpl.rcNormalPosition.left = AfxGetApp()->GetProfileInt(_T("Settings"), _T("WindowPosX"), (workarea.right - dlgrect.Width()) / 2);
	wndpl.rcNormalPosition.top = AfxGetApp()->GetProfileInt(_T("Settings"), _T("WindowPosY"), (workarea.bottom - dlgrect.Height()) / 2);
	if (wndpl.rcNormalPosition.left < 0)
		wndpl.rcNormalPosition.left = 0;
	if (wndpl.rcNormalPosition.top < 0)
		wndpl.rcNormalPosition.top = 0;
	wndpl.rcNormalPosition.left = min(workarea.right - dlgrect.Width(), wndpl.rcNormalPosition.left + workarea.left);
	wndpl.rcNormalPosition.top = min(workarea.bottom - dlgrect.Height(), wndpl.rcNormalPosition.top + workarea.top);

	wndpl.rcNormalPosition.right = dlgrect.Width() + wndpl.rcNormalPosition.left;
	wndpl.rcNormalPosition.bottom = dlgrect.Height() + wndpl.rcNormalPosition.top;
	SetWindowPlacement(&wndpl);

	CDialogEx::PreInitDialog();
}


BOOL CNetDisconnectorDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	return TRUE;
}


void CNetDisconnectorDlg::OnOptionsDisconnectsound()
{
	CMenu* menu = GetMenu()->GetSubMenu(1);
	UpdateData(true);

	if (menu->GetMenuState(ID_OPTIONS_DISCONNECTSOUND, MF_BYCOMMAND) == MF_CHECKED) {
		DcSoundState = MF_UNCHECKED;
		menu->CheckMenuItem(ID_OPTIONS_DISCONNECTSOUND, MF_BYCOMMAND | MF_UNCHECKED);

		AddLog(_T("Notification sound turned off"));
	}
	else if (menu->GetMenuState(ID_OPTIONS_DISCONNECTSOUND, MF_BYCOMMAND) == MF_UNCHECKED) {
		DcSoundState = MF_CHECKED;
		menu->CheckMenuItem(ID_OPTIONS_DISCONNECTSOUND, MF_BYCOMMAND | MF_CHECKED);

		AddLog(_T("Notification sound turned on"));
	}
}


void CNetDisconnectorDlg::OnOptionsReconnectsound()
{
	CMenu* menu = GetMenu()->GetSubMenu(1);
	UpdateData(true);

	if (menu->GetMenuState(ID_OPTIONS_RECONNECTSOUND, MF_BYCOMMAND) == MF_CHECKED) {
		RcSoundState = MF_UNCHECKED;
		menu->CheckMenuItem(ID_OPTIONS_RECONNECTSOUND, MF_BYCOMMAND | MF_UNCHECKED);

		AddLog(_T("Notification sound turned off"));
	}
	else if (menu->GetMenuState(ID_OPTIONS_RECONNECTSOUND, MF_BYCOMMAND) == MF_UNCHECKED) {
		RcSoundState = MF_CHECKED;
		menu->CheckMenuItem(ID_OPTIONS_RECONNECTSOUND, MF_BYCOMMAND | MF_CHECKED);

		AddLog(_T("Notification sound turned on"));
	}
}


void CNetDisconnectorDlg::OnOptionsReleaseipaddress()
{
	KillTimer(2);
	CMenu* menu = GetMenu()->GetSubMenu(1);
	UpdateData(true);

	if (DcMethod != 0) {
		//AddLog(_T("Disconnect method switch to Release IP address"));
		AddLog(_T("Disconnect method switch to Disconnect adapter"));

		for (int matchIndex = 0; matchIndex < aIndex; matchIndex++) {
			if (get_adapter == AdaptersSet[matchIndex].ConnectionName) {
				AdapterCurrent = matchIndex;
				break;
			}
		}
		if (AdaptersSet[AdapterCurrent].Connection) {
			AddLog(_T("Press ") + get_hotkey.MakeUpper() + _T(" to disconnect"));
		}
		else {
			AddLog(_T("Press ") + get_hotkey.MakeUpper() + _T(" again to reconnect"));
		}
	}

	//menu->CheckMenuRadioItem(ID_OPTIONS_RELEASEIPADDRESS, ID_OPTIONS_FIREWALLBLOCKALL, ID_OPTIONS_RELEASEIPADDRESS, MF_BYCOMMAND);
	//menu->CheckMenuRadioItem(6, 10, 6, MF_BYPOSITION);
	DcMethod = 0;
	menu->CheckMenuRadioItem(idSet[0], idSet[4], idSet[DcMethod], MF_BYCOMMAND);
	AfxGetMainWnd()->SetWindowTextW(modeSet[DcMethod] + _T(" - NetDisconnector"));
}


void CNetDisconnectorDlg::OnOptionsDisableadapter()
{
	KillTimer(2);
	CMenu* menu = GetMenu()->GetSubMenu(1);
	UpdateData(true);

	if (DcMethod != 1) {
		AddLog(_T("Disconnect method switch to Disable adapter"));

		for (int matchIndex = 0; matchIndex < aIndex; matchIndex++) {
			if (get_adapter == AdaptersSet[matchIndex].ConnectionName) {
				AdapterCurrent = matchIndex;
				break;
			}
		}
		if (AdaptersSet[AdapterCurrent].Connection) {
			AddLog(_T("Press ") + get_hotkey.MakeUpper() + _T(" to disconnect"));
		}
		else {
			AddLog(_T("Press ") + get_hotkey.MakeUpper() + _T(" again to reconnect"));
		}
	}

	//menu->CheckMenuRadioItem(ID_OPTIONS_RELEASEIPADDRESS, ID_OPTIONS_FIREWALLBLOCKALL, ID_OPTIONS_DISABLEADAPTER, MF_BYCOMMAND);
	DcMethod = 1;
	menu->CheckMenuRadioItem(idSet[0], idSet[4], idSet[DcMethod], MF_BYCOMMAND);
	AfxGetMainWnd()->SetWindowTextW(modeSet[DcMethod] + _T(" - NetDisconnector"));
}


void CNetDisconnectorDlg::OnOptionsFirewallblockudp()
{
	KillTimer(2);
	CMenu* menu = GetMenu()->GetSubMenu(1);
	UpdateData(true);

	if (DcMethod != 2) {
		AddLog(_T("Disconnect method switch to Firewall Block UDP"));
	}

	//Check system firewall
	if (!CheckSystemFirewall()) {
		AddLog(_T("Failed to check firewall"));
	}
	//HRESULT hr = S_OK;
	//INetFwMgr* fwMgr = NULL;
	//INetFwPolicy* fwPolicy = NULL;
	//INetFwProfile* fwProfile;
	//hr = CoCreateInstance(
	//	__uuidof(NetFwMgr),
	//	NULL,
	//	CLSCTX_INPROC_SERVER,
	//	__uuidof(INetFwMgr),
	//	(void**)&fwMgr
	//);
	//hr = fwMgr->get_LocalPolicy(&fwPolicy);
	//if (FAILED(hr)) {
	//	m_LogContent += _T("\r\n") + time + _T("Failed to check firewall");
	//}
	//hr = fwPolicy->get_CurrentProfile(&fwProfile);
	//if (FAILED(hr)) {
	//	m_LogContent += _T("\r\n") + time + _T("Failed to check firewall");
	//}
	//VARIANT_BOOL fwEnabled = NULL;
	//hr = fwProfile->get_FirewallEnabled(&fwEnabled);
	//if (FAILED(hr)) {
	//	m_LogContent += _T("\r\n") + time + _T("Failed to check firewall");
	//}
	//if (fwEnabled != VARIANT_FALSE) {
	//	fwOn = TRUE;
	//}
	//else {
	//	fwOn = FALSE;
	//}


	if (fwOn) {
		if (RulesSet[0].enable == 0) {
			AddLog(_T("Press ") + get_hotkey.MakeUpper() + _T(" to disconnect"));
		}
		else {
			AddLog(_T("Firewall rules activated... Press ") + get_hotkey.MakeUpper() + _T(" to connect"));
		}
	}
	else {
		AddLog(_T("Firewall is off"));
		AddLog(_T("This mode can not work without firewall on"));
	}


	//menu->CheckMenuRadioItem(ID_OPTIONS_RELEASEIPADDRESS, ID_OPTIONS_FIREWALLBLOCKALL, ID_OPTIONS_BLOCKUDP, MF_BYCOMMAND);
	DcMethod = 2;
	menu->CheckMenuRadioItem(idSet[0], idSet[4], idSet[DcMethod], MF_BYCOMMAND);
	AfxGetMainWnd()->SetWindowTextW(modeSet[DcMethod] + _T(" - NetDisconnector"));
}


void CNetDisconnectorDlg::OnOptionsFirewallblocktcp()
{
	KillTimer(2);
	CMenu* menu = GetMenu()->GetSubMenu(1);
	UpdateData(true);

	if (DcMethod != 3) {
		AddLog(_T("Disconnect method switch to Firewall Block TCP"));
	}


	//Check system firewall
	if (!CheckSystemFirewall()) {
		AddLog(_T("Failed to check firewall"));
	}

	if (fwOn) {
		if (RulesSet[1].enable == 0) {
			AddLog(_T("Press ") + get_hotkey.MakeUpper() + _T(" to disconnect"));
		}
		else {
			AddLog(_T("Firewall rules activated... Press ") + get_hotkey.MakeUpper() + _T(" to connect"));
		}
	}
	else {
		AddLog(_T("Firewall is off"));
		AddLog(_T("This mode can not work without firewall on"));
	}


	//menu->CheckMenuRadioItem(ID_OPTIONS_RELEASEIPADDRESS, ID_OPTIONS_FIREWALLBLOCKALL, ID_OPTIONS_BLOCKTCP, MF_BYCOMMAND);
	DcMethod = 3;
	menu->CheckMenuRadioItem(idSet[0], idSet[4], idSet[DcMethod], MF_BYCOMMAND);
	AfxGetMainWnd()->SetWindowTextW(modeSet[DcMethod] + _T(" - NetDisconnector"));
}


void CNetDisconnectorDlg::OnOptionsFirewallblockall()
{
	KillTimer(2);
	CMenu* menu = GetMenu()->GetSubMenu(1);
	UpdateData(true);

	if (DcMethod != 4) {
		AddLog(_T("Disconnect method switch to Firewall Block ALL"));
		//AddLog(_T("Notice this method require system firewall enabled"));
	}


	//Check system firewall
	if (!CheckSystemFirewall()) {
		AddLog(_T("Failed to check firewall"));
	}

	if (fwOn) {
		if (RulesSet[2].enable == 0) {
			AddLog(_T("Press ") + get_hotkey.MakeUpper() + _T(" to disconnect"));
		}
		else {
			AddLog(_T("Firewall rules activated... Press ") + get_hotkey.MakeUpper() + _T(" to connect"));
		}
	}
	else {
		AddLog(_T("Firewall is off"));
		AddLog(_T("This mode can not work without firewall on"));
	}


	//menu->CheckMenuRadioItem(ID_OPTIONS_RELEASEIPADDRESS, ID_OPTIONS_FIREWALLBLOCKALL, ID_OPTIONS_FIREWALLBLOCKALL, MF_BYCOMMAND);
	DcMethod = 4;
	menu->CheckMenuRadioItem(idSet[0], idSet[4], idSet[DcMethod], MF_BYCOMMAND);
	AfxGetMainWnd()->SetWindowTextW(modeSet[DcMethod] + _T(" - NetDisconnector"));
}


void CNetDisconnectorDlg::OnHelpManual()
{
	CWnd* pWnd = FindWindow(_T("NetDisconnectorUsage"), _T("Usage"));
	if (!pWnd) {
		pManual = new Manual();
		pManual->Create(IDD_MANUAL_DIALOG, GetDesktopWindow());
		WINDOWPLACEMENT wndpl, wndpl_parent;
		GetWindowPlacement(&wndpl_parent);
		pManual->GetWindowPlacement(&wndpl);
		CRect dlgrect_parent;
		GetWindowRect(dlgrect_parent);//Parent Dialog Original Size 506*384
		CRect dlgrect;
		pManual->GetWindowRect(dlgrect);//Manual Dialog Original Size 474*312

		wndpl.rcNormalPosition.left = wndpl_parent.rcNormalPosition.left + fabs((dlgrect_parent.Width() - dlgrect.Width()) / 2);
		wndpl.rcNormalPosition.top = wndpl_parent.rcNormalPosition.top + fabs((dlgrect_parent.Height() - dlgrect.Height()) / 2);
		wndpl.rcNormalPosition.right = wndpl.rcNormalPosition.left + dlgrect.Width();
		wndpl.rcNormalPosition.bottom = wndpl.rcNormalPosition.top + dlgrect.Height();

		pManual->SetWindowPlacement(&wndpl);
		pManual->ShowWindow(SW_SHOW);
		pManual->SetFocus();
	}
	else {
		pWnd->BringWindowToTop();
		pWnd->ShowWindow(SW_NORMAL);
	}
	//if (pManual != NULL) {
	//	delete pManual;
	//	pManual = NULL;
	//}
	///*if (pManual != NULL) {
	//	pManual->ShowWindow(SW_SHOW);
	//}*/

	//if (pManual == NULL) {
	//	pManual = new Manual();
	//	pManual->Create(IDD_MANUAL_DIALOG, GetDesktopWindow());
	//}

	/*WINDOWPLACEMENT wndpl, wndpl_parent;
	GetWindowPlacement(&wndpl_parent);
	pManual->GetWindowPlacement(&wndpl);
	wndpl.rcNormalPosition.left = wndpl_parent.rcNormalPosition.left + 16;
	wndpl.rcNormalPosition.top = wndpl_parent.rcNormalPosition.top + 36;
	wndpl.rcNormalPosition.right = wndpl.rcNormalPosition.left + 474;
	wndpl.rcNormalPosition.bottom = wndpl.rcNormalPosition.top + 312;

	pManual->SetWindowPlacement(&wndpl);
	pManual->ShowWindow(SW_SHOW);
	pManual->SetFocus();*/
}


void CNetDisconnectorDlg::OnHelpOpenfirewallsettings()
{
	WinExec("cmd.exe /c start Firewall.cpl", SW_HIDE);
}


void CNetDisconnectorDlg::OnHelpViewfirewallrules()
{
	WinExec("cmd.exe /c start WF.msc", SW_HIDE);
}


void CNetDisconnectorDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_logCtrl.m_hWnd != NULL) {
		WINDOWPLACEMENT wndpl;
		m_logCtrl.GetWindowPlacement(&wndpl);

		wndpl.rcNormalPosition.right = cx - logWidthDiff;
		wndpl.rcNormalPosition.bottom = cy - logHeightDiff;

		m_logCtrl.SetWindowPlacement(&wndpl);
		m_logCtrl.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}
	if (m_adaptersList.m_hWnd != NULL) {
		WINDOWPLACEMENT wndpl;
		m_adaptersList.GetWindowPlacement(&wndpl);

		wndpl.rcNormalPosition.right = cx - comboWidthDiff;

		m_adaptersList.SetWindowPlacement(&wndpl);
	}
}


BOOL CNetDisconnectorDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	m_logCtrl.HideCaret();

	return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}