
// NetDisconnectorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NetDisconnector.h"
#include "NetDisconnectorDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//These header used to get adaptors
#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib, "IPHLPAPI.lib")
#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

//This header use to access Record dialog
#include "Record.h"

//These header used to play sound
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")



// CNetDisconnectorDlg dialog



CNetDisconnectorDlg::CNetDisconnectorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_NETDISCONNECTOR_DIALOG, pParent)
	, m_LogContent(_T(""))
	, get_adapter(_T(""))
	, get_hotkey(_T(""))
	, DcSoundState(0)
	, RcSoundState(0)
	, DcMethod(0)
	, get_vk_code(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
}

void CNetDisconnectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO, m_AdaptersList);
	DDX_Text(pDX, IDC_LOG, m_LogContent);
	DDX_Control(pDX, IDC_LOG, m_LogCtrl);
}

BEGIN_MESSAGE_MAP(CNetDisconnectorDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO, &CNetDisconnectorDlg::OnCbnSelchangeCombo)
	ON_COMMAND(ID_NETWORK_DISCONNECT, &CNetDisconnectorDlg::OnNetworkDisconnect)
	ON_COMMAND(ID_NETWORK_CONNECT, &CNetDisconnectorDlg::OnNetworkConnect)
	ON_COMMAND(ID_OPTION_CHANGEHOTKEY, &CNetDisconnectorDlg::OnOptionChangehotkey)
	ON_WM_HOTKEY()
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_HELPINFO()
	ON_COMMAND(ID_OPTION_DISCONNECTSOUND, &CNetDisconnectorDlg::OnOptionDisconnectsound)
	ON_COMMAND(ID_OPTION_RECONNECTSOUND, &CNetDisconnectorDlg::OnOptionReconnectsound)
	ON_COMMAND(ID_OPTION_DISABLEADAPTER, &CNetDisconnectorDlg::OnOptionDisableadapter)
END_MESSAGE_MAP()


// CNetDisconnectorDlg message handlers

BOOL CNetDisconnectorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here


	LOGFONT lf;//Set font
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 18;
	StrCpyW(lf.lfFaceName, L"Calibri");
	m_font.CreateFontIndirect(&lf);
	GetDlgItem(IDC_LOG)->SetFont(&m_font);


	//GetAdaptersInfo
	/*PIP_ADAPTER_INFO pAdapterInfo;
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
			CString temp;
			temp.Format(_T("%S"), pAdapter->Description);
			m_AdaptersList.AddString(temp);//Add adaptors to combobox
			pAdapter = pAdapter->Next;
		}
	}
	if (pAdapterInfo)//Release memory usage
		FREE(pAdapterInfo);*/



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
		struct AdaptersSet {
			CString AdapterName;
			CString ValueName;
			CString ConnectionName;
			CString ValuePnPInstanceId;
			CString PnPInstanceId;
		}AdaptersSet[50];
		DWORD Namesize = SubpathLen++;
		LPSTR Subpath = new char[Namesize];
		RegEnumKeyExA(phkResult, dwIndex, Subpath, &Namesize, NULL, NULL, NULL, NULL);
		if (!strcmp(Subpath, "Descriptions")) {
			continue;
		}
		AdaptersSet[dwIndex].AdapterName = Subpath;
		delete[] Subpath;//Release memory usage
		Subpath = NULL;
		CString newPath = _T("SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\") + AdaptersSet[dwIndex].AdapterName + "\\Connection";
		HKEY phkResult;
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, newPath, 0, KEY_READ | KEY_WOW64_64KEY, &phkResult)) {
			return 1;
		}
		DWORD ValueNum, wValueNameLen, bValueLen;
		if (RegQueryInfoKey(phkResult, NULL, NULL, 0, NULL, NULL, NULL, &ValueNum, &wValueNameLen, &bValueLen, NULL, NULL)) {//Get connection infos
			return 1;
		}
		for (DWORD dwIndex_2 = 0;dwIndex_2 < ValueNum;dwIndex_2++) {
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
				AdaptersSet[dwIndex].ValueName = ValueName;
				AdaptersSet[dwIndex].ConnectionName = Value;
			}
			else if ((CString)ValueName == "PnPInstanceId" || (CString)ValueName == "PnpInstanceID") {
				AdaptersSet[dwIndex].ValuePnPInstanceId = ValueName;
				AdaptersSet[dwIndex].PnPInstanceId = Value;
			}
			delete[]Value;//Release memory usage
			Value = NULL;
			free(ValueName);//Release memory usage
			ValueName = NULL;
		}
		if (AdaptersSet[dwIndex].PnPInstanceId.Left(4) == "PCI\\" || AdaptersSet[dwIndex].PnPInstanceId.Left(4) == "ROOT") {
			m_AdaptersList.AddString(AdaptersSet[dwIndex].ConnectionName);//Add Connections to combobox
		}
	}



	
	get_adapter = AfxGetApp()->GetProfileString(_T("Settings"), _T("Adapter"), NULL);//Get default adaptor
	int pos = m_AdaptersList.FindStringExact(0, get_adapter);
	if (pos == -1) {
		pos = m_AdaptersList.FindStringExact(0, _T("Local Area Connection"));//If pos eqaul to NULL, search for default adaptor
		if (pos == -1) {
			pos = m_AdaptersList.FindStringExact(0, _T("本地连接"));
			if (pos == -1) {
				pos = m_AdaptersList.FindStringExact(0, _T("WLAN"));
				if (pos == -1) {
					pos = 0;//If cant find default adapter, set it to first one
				}
			}
		}
	}
	m_AdaptersList.SetCurSel(pos);//Set selected adaptor
	m_AdaptersList.GetLBText(m_AdaptersList.GetCurSel(), get_adapter);//Save current select to value get_adapter


	UpdateData(true);
	SYSTEMTIME systime;
	CString time;

	get_vk_code = AfxGetApp()->GetProfileInt(_T("Settings"), _T("KeyVk"), VK_PAUSE);
	ATOM HotkeyID = GlobalAddAtom(_T("INITIAL_BIND")) - 0xc000;//Register hotkey
	if (FALSE == RegisterHotKey(GetSafeHwnd(), 1, AfxGetApp()->GetProfileInt(_T("Settings"), _T("KeyModifiers"), NULL) | MOD_NOREPEAT, get_vk_code)) {
		GetLocalTime(&systime);
		time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);
		
		m_LogContent += time + _T("Register hotkey failed!!!") + _T("\r\n");
	}
	get_hotkey = AfxGetApp()->GetProfileString(_T("Settings"), _T("Key"), _T("Pause"));//Get hotkey string

	GetLocalTime(&systime);
	time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

	if (get_vk_code >= 0x21 && get_vk_code <= 0x2D) {
		m_LogContent += time + _T("This key may not work!!!") + _T("\r\n");
	}
	m_LogContent += time + _T("Press ") + get_hotkey.MakeUpper() + _T(" to disconnect");
	UpdateData(false);
	m_LogCtrl.LineScroll(m_LogCtrl.GetLineCount(), 0);


	DcSoundState = AfxGetApp()->GetProfileInt(_T("Settings"), _T("DcSound"), MF_CHECKED);
	RcSoundState = AfxGetApp()->GetProfileInt(_T("Settings"), _T("RcSound"), MF_CHECKED);
	CMenu* menu = GetMenu()->GetSubMenu(1);
	menu->CheckMenuItem(ID_OPTION_DISCONNECTSOUND, MF_BYCOMMAND | DcSoundState);
	menu->CheckMenuItem(ID_OPTION_RECONNECTSOUND, MF_BYCOMMAND | RcSoundState);

	DcMethod = AfxGetApp()->GetProfileInt(_T("Settings"), _T("DcMethod"), MF_UNCHECKED);
	menu->CheckMenuItem(ID_OPTION_DISABLEADAPTER, MF_BYCOMMAND | DcMethod);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNetDisconnectorDlg::OnPaint()
{
	CRect rec;
	CPaintDC dc(this);
	GetDlgItem(IDC_LOG)->GetWindowRect(&rec);
	ScreenToClient(&rec);
	dc.FillSolidRect(rec, RGB(12, 12, 12));

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



void CNetDisconnectorDlg::OnCbnSelchangeCombo()
{
	UpdateData(true);

	m_AdaptersList.GetLBText(m_AdaptersList.GetCurSel(), get_adapter);//Get adaptor name

	SYSTEMTIME systime;
	CString time;
	GetLocalTime(&systime);
	time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

	m_LogContent += _T("\r\n") + time + _T("Disconnect target now set to ") + get_adapter + _T("");
	UpdateData(false);
	m_LogCtrl.LineScroll(m_LogCtrl.GetLineCount(), 0);
}


void CNetDisconnectorDlg::OnNetworkDisconnect()
{
	UpdateData(true);
	SYSTEMTIME systime;
	CString time;

	CString release;
	if (DcMethod == MF_UNCHECKED) {
		release = _T("cmd.exe /c ipconfig /release \"") + get_adapter + _T("\">nul");
	}
	else if (DcMethod == MF_CHECKED) {
		release = _T("cmd.exe /c netsh interface set interface name=\"") + get_adapter + _T("\" admin=disabled");
	}

	USES_CONVERSION;
	LPWSTR temp = new WCHAR[release.GetLength() + 1];
	wcscpy((LPTSTR)temp, T2W((LPTSTR)release.GetBuffer(NULL)));
	release.ReleaseBuffer();

	STARTUPINFO si = { sizeof(si) };
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));

	CreateProcess(NULL, temp, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

	GetLocalTime(&systime);
	time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

	m_LogContent += _T("\r\n") + time + _T("Disconnecting...");
	UpdateData(false);
	m_LogCtrl.LineScroll(m_LogCtrl.GetLineCount(), 0);

	if (WaitForSingleObject(pi.hProcess, 12000) == WAIT_TIMEOUT) {//Wait until process is complete then play notification sound
		WinExec("cmd.exe /c taskkill /im ipconfig.exe /t /f", SW_HIDE);//Terminate timeout process
		GetLocalTime(&systime);
		time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

		m_LogContent += _T("\r\n") + time + _T("Disconnecting timeout!!!");
		m_LogContent += _T("\r\n") + time + _T("Target adapter may have no internet connection or disabled DHCP");
		UpdateData(false);
		m_LogCtrl.LineScroll(m_LogCtrl.GetLineCount(), 0);
	}

	GetLocalTime(&systime);
	time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

	m_LogContent += _T("\r\n") + time + _T("Disconnected");
	UpdateData(false);
	m_LogCtrl.LineScroll(m_LogCtrl.GetLineCount(), 0);
}


void CNetDisconnectorDlg::OnNetworkConnect()
{
	UpdateData(true);
	SYSTEMTIME systime;
	CString time;

	CString renew;
	if (DcMethod == MF_UNCHECKED) {
		renew = _T("cmd.exe /c ipconfig /renew \"") + get_adapter + _T("\">nul");
	}
	else if (DcMethod == MF_CHECKED) {
		renew = _T("cmd.exe /c netsh interface set interface name=\"") + get_adapter + _T("\" admin=enabled");
	}

	USES_CONVERSION;
	LPWSTR temp = new WCHAR[renew.GetLength() + 1];
	wcscpy((LPTSTR)temp, T2W((LPTSTR)renew.GetBuffer(NULL)));
	renew.ReleaseBuffer();

	STARTUPINFO si = { sizeof(si) };
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));

	CreateProcess(NULL, temp, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

	GetLocalTime(&systime);
	time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

	m_LogContent += _T("\r\n") + time + _T("Connecting...");
	UpdateData(false);
	m_LogCtrl.LineScroll(m_LogCtrl.GetLineCount(), 0);

	if (WaitForSingleObject(pi.hProcess, 12000) == WAIT_TIMEOUT) {//Wait until process is complete then play notification sound
		WinExec("cmd.exe /c taskkill /im ipconfig.exe /t /f", SW_HIDE);//Terminate timeout process
		GetLocalTime(&systime);
		time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

		m_LogContent += _T("\r\n") + time + _T("Connecting timeout!!!");
		m_LogContent += _T("\r\n") + time + _T("Target adapter may have no internet connection or disabled DHCP");
		UpdateData(false);
		m_LogCtrl.LineScroll(m_LogCtrl.GetLineCount(), 0);
	}

	GetLocalTime(&systime);
	time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

	m_LogContent += _T("\r\n") + time + _T("Connected");
	UpdateData(false);
	m_LogCtrl.LineScroll(m_LogCtrl.GetLineCount(), 0);
}


void CNetDisconnectorDlg::OnOptionChangehotkey()
{
	UnregisterHotKey(GetSafeHwnd(), 1); //Unbind all hotkey

	INT_PTR result;
	Record RecDlg;
	RecDlg.m_RecordLabel = get_hotkey;
	result = RecDlg.DoModal();
	if (IDCANCEL == result) {  //If press cancel then quit
		ATOM HotkeyID = GlobalAddAtom(_T("INITIAL_REBIND")) - 0xc000; //Register hotkey
		RegisterHotKey(GetSafeHwnd(), 1, AfxGetApp()->GetProfileInt(_T("Settings"), _T("KeyModifiers"), NULL) | MOD_NOREPEAT, AfxGetApp()->GetProfileInt(_T("Settings"), _T("KeyVk"), VK_PAUSE));//Register hotkey
		return;
	}
	if (RecDlg.input_vk_code == 0x00) {  //If press nothing then quit
		ATOM HotkeyID = GlobalAddAtom(_T("INITIAL_REBIND")) - 0xc000; //Register hotkey
		RegisterHotKey(GetSafeHwnd(), 1, AfxGetApp()->GetProfileInt(_T("Settings"), _T("KeyModifiers"), NULL) | MOD_NOREPEAT, AfxGetApp()->GetProfileInt(_T("Settings"), _T("KeyVk"), VK_PAUSE));//Register hotkey
		return;
	}

	ATOM HotkeyID = GlobalAddAtom(_T("REBIND")) - 0xc000; //Register new hotkey
	BOOL done = RegisterHotKey(GetSafeHwnd(), 1, RecDlg.input_modifiers | MOD_NOREPEAT, RecDlg.input_vk_code);
	get_vk_code = RecDlg.input_vk_code;
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("KeyModifiers"), RecDlg.input_modifiers);
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("KeyVk"), get_vk_code);

	UpdateData(true);
	SYSTEMTIME systime;
	CString time;
	GetLocalTime(&systime);
	time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

	get_hotkey = RecDlg.input_key; //Add keys which have issue here
	if (get_vk_code == 0x13) {
		get_hotkey = "Pause";
	}
	if (get_vk_code >= 0x21 && get_vk_code <= 0x2D) {
		m_LogContent += _T("\r\n") + time + _T("This key may not work!!!");
	}

	AfxGetApp()->WriteProfileString(_T("Settings"), _T("Key"), get_hotkey);

	//TODO：解决下录制是空的时候的状态
	if (done) {
		m_LogContent += _T("\r\n") + time + _T("Press ") + get_hotkey.MakeUpper() + _T(" to disconnect");
	}
	else{
		m_LogContent += _T("\r\n") + time + _T("Register hotkey failed!!!");
	}
	UpdateData(false);
	m_LogCtrl.LineScroll(m_LogCtrl.GetLineCount(), 0);
}


void CNetDisconnectorDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	static bool flag = false;
	UpdateData(true);
	SYSTEMTIME systime;
	CString time;

	if (nHotKeyId) {//If match the HotKeyId then continue
		if (!flag) {
			CString release;
			if(DcMethod == MF_UNCHECKED) {
				release = _T("cmd.exe /c ipconfig /release \"") + get_adapter + _T("\">nul");
			}
			else if(DcMethod == MF_CHECKED) {
				release = _T("cmd.exe /c netsh interface set interface name=\"") + get_adapter + _T("\" admin=disabled");
			}

			USES_CONVERSION;
			LPWSTR temp = new WCHAR[release.GetLength() + 1];
			wcscpy((LPTSTR)temp, T2W((LPTSTR)release.GetBuffer(NULL)));
			release.ReleaseBuffer();

			STARTUPINFO si = {sizeof(si)};
			si.dwFlags = STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_HIDE;
			PROCESS_INFORMATION pi;
			ZeroMemory(&pi, sizeof(pi));

			CreateProcess(NULL, temp, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

			GetLocalTime(&systime);
			time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

			m_LogContent += _T("\r\n") + time + _T("Disconnecting...");
			UpdateData(false);
			m_LogCtrl.LineScroll(m_LogCtrl.GetLineCount(), 0);

			if (WaitForSingleObject(pi.hProcess, 12000) == WAIT_TIMEOUT) {//Wait until process is complete then play notification sound
				WinExec("cmd.exe /c taskkill /im ipconfig.exe /t /f", SW_HIDE);//Terminate timeout process
				GetLocalTime(&systime);
				time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

				m_LogContent += _T("\r\n") + time + _T("Disconnecting timeout!!!");
				m_LogContent += _T("\r\n") + time + _T("Target adapter may have no internet connection or disabled DHCP");
				UpdateData(false);
				m_LogCtrl.LineScroll(m_LogCtrl.GetLineCount(), 0);
			}
			
			if (DcSoundState == MF_CHECKED)
				PlaySound(MAKEINTRESOURCE(IDR_WAVE1), AfxGetResourceHandle(), SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);

			GetLocalTime(&systime);
			time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

			m_LogContent += _T("\r\n") + time + _T("Press ") + get_hotkey.MakeUpper() + _T(" again to reconnect");
			UpdateData(false);
			m_LogCtrl.LineScroll(m_LogCtrl.GetLineCount(), 0);

			flag = !flag;
		}
		else {
			CString renew;
			if (DcMethod == MF_UNCHECKED) {
				renew = _T("cmd.exe /c ipconfig /renew \"") + get_adapter + _T("\">nul");
			}
			else if (DcMethod == MF_CHECKED) {
				renew = _T("cmd.exe /c netsh interface set interface name=\"") + get_adapter + _T("\" admin=enabled");
			}

			USES_CONVERSION;
			LPWSTR temp = new WCHAR[renew.GetLength() + 1];
			wcscpy((LPTSTR)temp, T2W((LPTSTR)renew.GetBuffer(NULL)));
			renew.ReleaseBuffer();

			STARTUPINFO si = {sizeof(si)};
			si.dwFlags = STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_HIDE;
			PROCESS_INFORMATION pi;
			ZeroMemory(&pi, sizeof(pi));

			CreateProcess(NULL, temp, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

			GetLocalTime(&systime);
			time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

			m_LogContent += _T("\r\n") + time + _T("Connecting...");
			UpdateData(false);
			m_LogCtrl.LineScroll(m_LogCtrl.GetLineCount(), 0);

			if (WaitForSingleObject(pi.hProcess, 12000) == WAIT_TIMEOUT) {//Wait until process is complete then play notification sound
				WinExec("cmd.exe /c taskkill /im ipconfig.exe /t /f", SW_HIDE);//Terminate timeout process
				GetLocalTime(&systime);
				time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);
				m_LogContent += _T("\r\n") + time + _T("Connecting timeout!!!");
				m_LogContent += _T("\r\n") + time + _T("Target adapter may have no internet connection or disabled DHCP");
				UpdateData(false);
				m_LogCtrl.LineScroll(m_LogCtrl.GetLineCount(), 0);
			}

			if (RcSoundState == MF_CHECKED)
				PlaySound(MAKEINTRESOURCE(IDR_WAVE2), AfxGetResourceHandle(), SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);

			GetLocalTime(&systime);
			time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

			m_LogContent += _T("\r\n") + time + _T("Press ") + get_hotkey.MakeUpper() +_T(" to disconnect");
			UpdateData(false);
			m_LogCtrl.LineScroll(m_LogCtrl.GetLineCount(), 0);

			flag = !flag;
		}
	}
	CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}


HBRUSH CNetDisconnectorDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID() == IDC_LOG) {
		pDC->SetTextColor(RGB(204, 204, 204));
		pDC->SetBkColor(RGB(12, 12, 12));
		hbr = m_brush;
	}

	// TODO:  在此更改 DC 的任何特性

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


void CNetDisconnectorDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("DcSound"), DcSoundState);
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("RcSound"), RcSoundState);
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("DcMethod"), DcMethod);
	AfxGetApp()->WriteProfileString(_T("Settings"), _T("Adapter"), get_adapter);//Save current adapter
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
	wndpl.rcNormalPosition.left = AfxGetApp()->GetProfileInt(_T("Settings"), _T("WindowPosX"), 712);
	wndpl.rcNormalPosition.top = AfxGetApp()->GetProfileInt(_T("Settings"), _T("WindowPosY"), 328);
	if (wndpl.rcNormalPosition.left < 0)
		wndpl.rcNormalPosition.left = 0;
	if (wndpl.rcNormalPosition.top < 0)
		wndpl.rcNormalPosition.top = 0;
	RECT workarea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &workarea, 0);
	wndpl.rcNormalPosition.left = min(workarea.right - 506, wndpl.rcNormalPosition.left + workarea.left);
	wndpl.rcNormalPosition.top = min(workarea.bottom - 384, wndpl.rcNormalPosition.top + workarea.top);
	wndpl.rcNormalPosition.right = 506 + wndpl.rcNormalPosition.left;
	wndpl.rcNormalPosition.bottom = 384 + wndpl.rcNormalPosition.top;
	SetWindowPlacement(&wndpl);

	CDialogEx::PreInitDialog();
}


BOOL CNetDisconnectorDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	return TRUE;
}


void CNetDisconnectorDlg::OnOptionDisconnectsound()
{
	CMenu* menu = GetMenu()->GetSubMenu(1);
	UpdateData(true);
	SYSTEMTIME systime;
	CString time;

	if (menu->GetMenuState(ID_OPTION_DISCONNECTSOUND, MF_BYCOMMAND) == MF_CHECKED) {
		DcSoundState = MF_UNCHECKED;
		menu->CheckMenuItem(ID_OPTION_DISCONNECTSOUND, MF_BYCOMMAND | MF_UNCHECKED);

		GetLocalTime(&systime);
		time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

		m_LogContent += _T("\r\n") + time + _T("Notification sound disabled");
		UpdateData(false);
		m_LogCtrl.LineScroll(m_LogCtrl.GetLineCount(), 0);
	}
	else if (menu->GetMenuState(ID_OPTION_DISCONNECTSOUND, MF_BYCOMMAND) == MF_UNCHECKED) {
		DcSoundState = MF_CHECKED;
		menu->CheckMenuItem(ID_OPTION_DISCONNECTSOUND, MF_BYCOMMAND | MF_CHECKED);

		GetLocalTime(&systime);
		time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

		m_LogContent += _T("\r\n") + time + _T("Notification sound enabled");
		UpdateData(false);
		m_LogCtrl.LineScroll(m_LogCtrl.GetLineCount(), 0);
	}
}


void CNetDisconnectorDlg::OnOptionReconnectsound()
{
	CMenu* menu = GetMenu()->GetSubMenu(1);
	UpdateData(true);
	SYSTEMTIME systime;
	CString time;

	if (menu->GetMenuState(ID_OPTION_RECONNECTSOUND, MF_BYCOMMAND) == MF_CHECKED) {
		RcSoundState = MF_UNCHECKED;
		menu->CheckMenuItem(ID_OPTION_RECONNECTSOUND, MF_BYCOMMAND | MF_UNCHECKED);

		GetLocalTime(&systime);
		time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

		m_LogContent += _T("\r\n") + time + _T("Notification sound disabled");
		UpdateData(false);
		m_LogCtrl.LineScroll(m_LogCtrl.GetLineCount(), 0);
	}
	else if (menu->GetMenuState(ID_OPTION_RECONNECTSOUND, MF_BYCOMMAND) == MF_UNCHECKED) {
		RcSoundState = MF_CHECKED;
		menu->CheckMenuItem(ID_OPTION_RECONNECTSOUND, MF_BYCOMMAND | MF_CHECKED);

		GetLocalTime(&systime);
		time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

		m_LogContent += _T("\r\n") + time + _T("Notification sound enabled");
		UpdateData(false);
		m_LogCtrl.LineScroll(m_LogCtrl.GetLineCount(), 0);
	}
}


void CNetDisconnectorDlg::OnOptionDisableadapter()
{
	CMenu* menu = GetMenu()->GetSubMenu(1);
	UpdateData(true);
	SYSTEMTIME systime;
	CString time;

	if (menu->GetMenuState(ID_OPTION_DISABLEADAPTER, MF_BYCOMMAND) == MF_CHECKED) {
		DcMethod = MF_UNCHECKED;
		menu->CheckMenuItem(ID_OPTION_DISABLEADAPTER, MF_BYCOMMAND | MF_UNCHECKED);
		
		GetLocalTime(&systime);
		time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

		m_LogContent += _T("\r\n") + time + _T("Disconnect method change to release IP address");
		UpdateData(false);
		m_LogCtrl.LineScroll(m_LogCtrl.GetLineCount(), 0);
	}
	else if (menu->GetMenuState(ID_OPTION_DISABLEADAPTER, MF_BYCOMMAND) == MF_UNCHECKED) {
		DcMethod = MF_CHECKED;
		menu->CheckMenuItem(ID_OPTION_DISABLEADAPTER, MF_BYCOMMAND | MF_CHECKED);

		GetLocalTime(&systime);
		time.Format(_T("%02d:%02d:%02d "), systime.wHour, systime.wMinute, systime.wSecond);

		m_LogContent += _T("\r\n") + time + _T("Disconnect method change to disable adapter");
		UpdateData(false);
		m_LogCtrl.LineScroll(m_LogCtrl.GetLineCount(), 0);
	}
}
