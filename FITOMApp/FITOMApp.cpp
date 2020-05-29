
// FITOMApp.cpp : アプリケーションのクラス動作を定義します。
//

#include "stdafx.h"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <gdiplus.h>
#include <WinSock2.h>

#include "FITOM.h"
#include "FITOMdefine.h"
#include "SoundDev.h"
#include "FITOMApp.h"
#include "FITOMAppDlg.h"
#include "FITOMCfg.h"
#include "SCCIWrapper.h"
#include "MIDIDEV.h"
#include "MIDI.h"
#include "codec.h"
#include "ADPCM.h"
#include "SplashDlg.h"
#include "ProgressDlg.h"
#include "FTSPI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CFITOMApp

BEGIN_MESSAGE_MAP(CFITOMApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CFITOMApp::ERROR_MAP CFITOMApp::errmap[] = {
	{ ERROR_MIDI_IN, IDS_ERROR_MIDI, },
	{ ERROR_FM_DEV, IDS_ERROR_DEV, },
	{ ERROR_FM_BANK, IDS_ERROR_VOICE, },
	{ ERROR_DRUM_MAP, IDS_ERROR_DRUM, },
	{ ERROR_SCCI, IDS_ERROR_SCCI, },
	{ -98, IDS_ERROR_PROFILE, },
	{ -99, IDS_ERROR_INI, },
	{ 0, 0, },
};

// CFITOMApp コンストラクション

CFITOMApp::CFITOMApp() : hTimer(0), hPolling(0), theFitom(0), bRunning(FALSE), clipvoice(0), clipdrum(0)
{
	// 再起動マネージャーをサポートします
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}


// 唯一の CFITOMApp オブジェクトです。

CFITOMApp theApp;

void SplashStatus(LPCTSTR str)
{
	theApp.SplashStatus(str);
}

void SplashMessage(LPCTSTR str)
{
	theApp.SplashMessage(str);
}

// CFITOMApp 初期化

BOOL CFITOMApp::InitInstance()
{
	// アプリケーション マニフェストが visual スタイルを有効にするために、
	// ComCtl32.dll Version 6 以降の使用を指定する場合は、
	// Windows XP に InitCommonControlsEx() が必要です。さもなければ、ウィンドウ作成はすべて失敗します。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// アプリケーションで使用するすべてのコモン コントロール クラスを含めるには、
	// これを設定します。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	Gdiplus::GdiplusStartupInput gpSI;
	ULONG_PTR lpToken;
	Gdiplus::GdiplusStartup(&lpToken, &gpSI, NULL);

	CWinApp::InitInstance();
	pSplash = new CSplashDlg();
	pSplash->ShowWindow(SW_SHOW);

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// ダイアログにシェル ツリー ビューまたはシェル リスト ビュー コントロールが
	// 含まれている場合にシェル マネージャーを作成します。
	CShellManager *pShellManager = new CShellManager;

	// MFC コントロールでテーマを有効にするために、"Windows ネイティブ" のビジュアル マネージャーをアクティブ化
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 標準初期化
	// これらの機能を使わずに最終的な実行可能ファイルの
	// サイズを縮小したい場合は、以下から不要な初期化
	// ルーチンを削除してください。
	// 設定が格納されているレジストリ キーを変更します。
	// TODO: 会社名または組織名などの適切な文字列に
	// この文字列を変更してください。
	SetRegistryKey(_T("FITOM"));
#if defined(_DEBUG) || defined(_CONSOLE)
	::AllocConsole();
	::freopen("CONOUT$", "w", stdout);
	::freopen("CONOUT$", "w", stderr);
#endif
	int res = FITOMStart();
	pSplash->ShowWindow(SW_HIDE);
	pSplash->EndDialog(0);
	if (!res) {
		CFITOMAppDlg dlg;
		m_pMainWnd = &dlg;
		INT_PTR nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			// TODO: ダイアログが <OK> で消された時のコードを
			//  記述してください。
		}
		else if (nResponse == IDCANCEL)
		{
			// TODO: ダイアログが <キャンセル> で消された時のコードを
			//  記述してください。
		}
		else if (nResponse == -1)
		{
			TRACE(traceAppMsg, 0, "警告: ダイアログの作成に失敗しました。アプリケーションは予期せずに終了します。\n");
			TRACE(traceAppMsg, 0, "警告: ダイアログで MFC コントロールを使用している場合、#define _AFX_NO_MFC_CONTROLS_IN_DIALOGS を指定できません。\n");
		}
		FITOMStop();
	}
	else {
		int ids = IDS_ERROR_OTHER;
		for (int i = 0; errmap[i].FITOM_ERR != 0; i++) {
			if (errmap[i].FITOM_ERR == res) { 
				ids = errmap[i].ERR_IDS;
				break;
			}
		}
		AfxMessageBox(ids);
	}

#if defined(_DEBUG) || defined(_CONSOLE)
	::FreeConsole();
#endif
	// 上で作成されたシェル マネージャーを削除します。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	/* GDI+終了 */
	Gdiplus::GdiplusShutdown(lpToken);

	// ダイアログは閉じられました。アプリケーションのメッセージ ポンプを開始しないで
	//  アプリケーションを終了するために FALSE を返してください。
	return FALSE;
}

void CFITOMApp::DoEvents()
{
	MSG msg;
	while (::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
	{
		if (!this->PumpMessage()) {
			return;
		}
	}
}

void CFITOMApp::SplashStatus(LPCTSTR str)
{
	DoEvents();
	pSplash->SetStatus(str);
}

void CFITOMApp::SplashMessage(LPCTSTR str)
{
	DoEvents();
	pSplash->SetFilename(str);
}

int CFITOMApp::FITOMStart()
{
	int res = 0;
	if (!PathFileExists(_T(".\\FITOM.ini"))) { return -99; }
	theConfig = LoadConfig();

	if (theConfig) {
		if (theConfig->GetScci() == 0) { return ERROR_SCCI; }
		if (theConfig->GetLogDevs() == 0) { return ERROR_FM_DEV; }
		if (theConfig->GetMidiInputs() == 0) { return ERROR_MIDI_IN; }
		theFitom = CFITOM::GetInstance();
		int res = theFitom->InitInstance(theConfig);
		if (res) { return res; }
		::timeBeginPeriod(1);
		hTimer = timeSetEvent(10 - 1, 0, TimerProc, (DWORD)this, TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
		hPolling = (HANDLE)_beginthreadex(0, 0, PollingProc, (void*)this, 0, 0);
		hCommand = (HANDLE)_beginthreadex(0, 0, CommandProc, (void*)this, 0, 0);
		bRunning = TRUE;
	}
	return res;
}

int CFITOMApp::FITOMStop()
{
	bRunning = FALSE;
	timeKillEvent(hTimer);
	timeEndPeriod(1);
	::Sleep(100);
	::TerminateThread(hPolling, 0);
	::Sleep(100);
	::TerminateThread(hCommand, 0);
	::Sleep(100);
	theFitom->ExitInstance();
	delete theConfig;
	return 0;
}

void CFITOMApp::ResetInterface()
{
	if (theConfig) {
		theConfig->GetScci()->InitialClear();
		theConfig->GetFTspi()->InitialClear();
	}
}

void CFITOMApp::InitDevice()
{
	if (theConfig) {
		for (int i = 0; i < theConfig->GetPhyDevs(); i++) {
			theConfig->GetPhysDeviceFromIndex(uint8_t(i))->Init();
		}
	}
}

void CALLBACK CFITOMApp::TimerProc(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dummy1, DWORD dummy2)
{
	CFITOMApp* theFitomApp = (CFITOMApp*)dwUser;
	if (theFitomApp->bRunning && theFitomApp->theFitom) {
		theFitomApp->theFitom->TimerCallBack(theFitomApp->lltickcount);
		theFitomApp->lltickcount++;
	}
}

unsigned int CFITOMApp::PollingProc(void* params)
{
	CFITOMApp* theFitomApp = (CFITOMApp*)params;
	while (theFitomApp->bRunning && theFitomApp->theFitom) {
		theFitomApp->theFitom->PollingCallBack();
		Sleep(0);
	}
	return 0;
}

unsigned int CFITOMApp::CommandProc(void* params)
{
	CFITOMApp* theFitomApp = (CFITOMApp*)params;
	WSADATA wsaData;
	SOCKET sock0;
	struct sockaddr_in addr;
	struct sockaddr_in client;
	int len;
	SOCKET sock;
	bool yes = 1;

	char buf[65536];
	char inbuf[65536];

	WSAStartup(MAKEWORD(2, 0), &wsaData);

	sock0 = socket(AF_INET, SOCK_STREAM, 0);
	if (sock0 == INVALID_SOCKET) {
		fprintf(stderr, "socket : %d\n", WSAGetLastError());
		return 1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(12345);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;

	setsockopt(sock0,
		SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));

	if (::bind(sock0, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
		fprintf(stderr, "bind : %d\n", WSAGetLastError());
		return 1;
	}

	if (listen(sock0, 5) != 0) {
		fprintf(stderr, "listen : %d\n", WSAGetLastError());
		return 1;
	}

	while (1) {
		len = sizeof(client);
		sock = accept(sock0, (struct sockaddr *)&client, &len);
		if (sock == INVALID_SOCKET) {
			fprintf(stderr, "accept : %d\n", WSAGetLastError());
			break;
		}

		memset(inbuf, 0, sizeof(inbuf));
		recv(sock, inbuf, sizeof(inbuf), 0);
		fprintf(stderr, "%s", inbuf);
		if (!theFitomApp->theFitom->CmdProc(inbuf, buf, sizeof(buf))) {
			sprintf_s(buf, sizeof(buf), "ERR.\r\n");
		}
		send(sock, buf, (int)strlen(buf), 0);

		closesocket(sock);
	}

	WSACleanup();

	return 0;
}

void CFITOMApp::VoiceCopy(FMVOICE* voice)
{
	if (!clipvoice) {
		clipvoice = new FMVOICE();
	}
	*clipvoice = *voice;
}

bool CFITOMApp::IsVoiceInClipBoard()
{
	return (clipvoice != 0);
}

bool CFITOMApp::GetVoiceFromClipBoard(FMVOICE* voice)
{
	if (clipvoice) {
		*voice = *clipvoice;
		delete clipvoice;
		clipvoice = 0;
		return TRUE;
	}
	return FALSE;
}

void CFITOMApp::DrumCopy(DRUMMAP* drumnote)
{
	if (!clipdrum) {
		clipdrum = new DRUMMAP();
	}
	*clipdrum = *drumnote;
}

bool CFITOMApp::IsDrumInClipBoard()
{
	return (clipdrum != 0);
}

bool CFITOMApp::GetDrumFromClipBoard(DRUMMAP* Drum)
{
	if (clipdrum) {
		*Drum = *clipdrum;
		delete clipdrum;
		clipdrum = 0;
		return TRUE;
	}
	return FALSE;
}

CFITOMConfigWin32* CFITOMApp::LoadConfig()
{
	CFITOMConfigWin32* ret = new CFITOMConfigWin32(_T(".\\FITOM.ini"));
	ret->SetProgressMsgFunc(::SplashStatus);
	ret->SetProgressFileFunc(::SplashMessage);
	ret->LoadConfig();

#ifdef _DEBUG
	fprintf_s(stderr, _T("[Logical Devices]\n"));
	for (int i = 0; i < ret->GetLogDevs(); i++) {
		CSoundDevice* pdev = ret->GetLogDeviceFromIndex(i);
		TCHAR portdesc[80];
		TCHAR devdesc[80];
		pdev->GetDevPort()->GetDesc(portdesc, _countof(portdesc));
		pdev->GetDescriptor(devdesc, _countof(devdesc));
		fprintf_s(stderr, _T("%02i %02X:%s (%s)\n"), i, pdev->GetDevice(), devdesc, portdesc);
	}
	fprintf_s(stderr, _T("[Physical Devices]\n"));
	for (int i = 0; i < ret->GetPhyDevs(); i++) {
		CSoundDevice* pdev = ret->GetPhysDeviceFromIndex(i);
		TCHAR portdesc[80];
		TCHAR devdesc[80];
		pdev->GetDevPort()->GetDesc(portdesc, _countof(portdesc));
		pdev->GetDescriptor(devdesc, _countof(devdesc));
		fprintf_s(stderr, _T("%02i %02X:%s (%s)\n"), i, pdev->GetDevice(), devdesc, portdesc);
	}
#endif
	return ret;
}


void CFITOMApp::SaveVoice(int group, int bank, int prog)
{
	struct {
		int vgroup;
		LPCTSTR vtype;
		void(CFITOMApp::*saveFunc)(CFMBank*, int);
	} functable[] = {
		{ VOICE_GROUP_OPM, _T("OPM"), &CFITOMApp::SaveOPMVoice, },
		{ VOICE_GROUP_OPNA, _T("OPN"), &CFITOMApp::SaveOPNVoice, },
		{ VOICE_GROUP_OPL3, _T("OPL3"), &CFITOMApp::SaveOPL3Voice, },
		{ VOICE_GROUP_OPL2, _T("OPL2"), &CFITOMApp::SaveOPL2Voice, },
		{ VOICE_GROUP_OPLL, _T("OPLL"), &CFITOMApp::SaveOPLLVoice, },
		{ VOICE_GROUP_PSG, _T("PSG"), &CFITOMApp::SavePSGVoice, },
		{ VOICE_GROUP_MA3, _T("MA3"), &CFITOMApp::SaveMA3Voice, },
		{ VOICE_GROUP_NONE, 0, 0, },
	};

	int func = -1;
	for (int i = 0; functable[i].vgroup != VOICE_GROUP_NONE; i++) {
		if (group == functable[i].vgroup) {
			func = i;
			break;
		}
	}

	CFMBank* pbank = 0;
	CDrumBank* dbank = 0;
	if (group == VOICE_GROUP_RHYTHM) {
		dbank = theConfig->GetDrumBank(bank);
	}
	else {
		pbank = theConfig->GetFMBank(group, bank);
	}
	if ((pbank && func >= 0) || dbank) {
		CProgressDlg* pDlg = new CProgressDlg(m_pMainWnd);
		pDlg->ShowWindow(SW_SHOW);
		pDlg->SetRange(0, 129);
		LPCTSTR filename = pbank ? pbank->GetFileName() : (dbank ? dbank->GetFileName() : _T(""));
		pDlg->SetStatus(filename);
		pDlg->SetTarget(_T("Header"));
		pDlg->SetPos(0);
		if (prog >= 0) {
			if (group == VOICE_GROUP_RHYTHM) {
				SaveDrumMap(dbank, prog);
			}
			else {
				(this->*functable[func].saveFunc)(pbank, prog);
			}
		}
		else {
			if (group == VOICE_GROUP_RHYTHM) {
				SaveDrumMapBank(dbank, pDlg);
			}
			else {
				SaveVoiceBank(functable[func].saveFunc, pbank, functable[func].vtype, pDlg);
			}
		}
		pDlg->EndDialog(0);
	}
}

void CFITOMApp::WriteLFOParam(FMVOICE* voice, LPCTSTR secname, LPCTSTR filename)
{
	TCHAR param[80];
	theConfig->BuildLFOParam(voice, 0, param, _countof(param));
	::WritePrivateProfileString(secname, _T("LFO0"), param, filename);
	for (int k = 0; k < 4; k++) {
		TCHAR keyname[16];
		StringCchPrintf(keyname, _countof(keyname), _T("LFO%i"), k + 1);
		theConfig->BuildLFOParam(voice, k + 1, param, _countof(param));
		::WritePrivateProfileString(secname, keyname, param, filename);
	}
}

void CFITOMApp::SaveVoiceBank(void(CFITOMApp::*saveFunc)(CFMBank*, int), CFMBank* bank, LPCTSTR vtype, CProgressDlg* pDlg)
{
	LPCTSTR filename = bank->GetFileName();
	if (::PathFileExists(filename)) {
		::WritePrivateProfileSection(_T("Header"), NULL, filename);
		::WritePrivateProfileSection(_T("Bank"), NULL, filename);
		::WritePrivateProfileString(_T("Header"), _T("Type"), vtype, filename);
		::WritePrivateProfileString(_T("Header"), _T("BankName"), bank->GetBankName(), filename);
		pDlg ? pDlg->SetPos(1) : 0;
		for (int i = 0; i < 128; i++) {
			(this->*saveFunc)(bank, i);
			pDlg ? pDlg->SetPos(i + 2) : 0;
		}
	}
}

void CFITOMApp::SaveOPMVoiceBank(CFMBank* bank, CProgressDlg* pDlg) {
	LPCTSTR filename = bank->GetFileName();
	if (::PathFileExists(filename)) {
		::WritePrivateProfileSection(_T("Header"), NULL, filename);
		::WritePrivateProfileSection(_T("Bank"), NULL, filename);
		::WritePrivateProfileString(_T("Header"), _T("Type"), _T("OPM"), filename);
		::WritePrivateProfileString(_T("Header"), _T("BankName"), bank->GetBankName(), filename);
		pDlg ? pDlg->SetPos(1) : 0;
		for (int i = 0; i < 128; i++) {
			SaveOPMVoice(bank, i);
			pDlg ? pDlg->SetPos(i + 2) : 0;
		}
	}
}

void CFITOMApp::SaveOPMVoice(CFMBank* bank, int num)
{
	LPCTSTR filename = bank->GetFileName();
	FMVOICE voice;
	bank->GetVoice(num, &voice);
	char name[17];
	memcpy(name, voice.name, 16);
	name[16] = 0;
	TCHAR secname[16];
	TCHAR param[80];
	StringCchPrintf(secname, _countof(secname), _T("Prog%i"), num);
	::WritePrivateProfileSection(secname, NULL, filename);
	WriteLFOParam(&voice, secname, filename);
	::WritePrivateProfileString(secname, _T("Name"), CA2T(name), filename);
	theConfig->BuildOPMVoice(&voice, 0, param, _countof(param));
	::WritePrivateProfileString(secname, _T("ALFB"), param, filename);
	for (int k = 0; k < 4; k++) {
		TCHAR keyname[16];
		StringCchPrintf(keyname, _countof(keyname), _T("OP%i"), k + 1);
		theConfig->BuildOPMVoice(&voice, k + 1, param, _countof(param));
		::WritePrivateProfileString(secname, keyname, param, filename);
	}
	for (int k = 0; k < 4; k++) {
		TCHAR keyname[16];
		StringCchPrintf(keyname, _countof(keyname), _T("ADD%i"), k + 1);
		theConfig->BuildOPMVoice(&voice, k + 5, param, _countof(param));
		::WritePrivateProfileString(secname, keyname, param, filename);
	}

}

void CFITOMApp::SaveOPNVoiceBank(CFMBank* bank, CProgressDlg* pDlg) {
	LPCTSTR filename = bank->GetFileName();
	if (::PathFileExists(filename)) {
		::WritePrivateProfileSection(_T("Header"), NULL, filename);
		::WritePrivateProfileSection(_T("Bank"), NULL, filename);
		::WritePrivateProfileString(_T("Header"), _T("Type"), _T("OPN"), filename);
		::WritePrivateProfileString(_T("Header"), _T("BankName"), bank->GetBankName(), filename);
		pDlg ? pDlg->SetPos(1) : 0;
		for (int i = 0; i < 128; i++) {
			SaveOPNVoice(bank, i);
			pDlg ? pDlg->SetPos(i + 2) : 0;
		}
	}
}

void CFITOMApp::SaveOPNVoice(CFMBank* bank, int num)
{
	LPCTSTR filename = bank->GetFileName();
	TCHAR secname[16];
	TCHAR param[80];
	FMVOICE voice;
	bank->GetVoice(num, &voice);
	char name[17];
	memcpy(name, voice.name, 16);
	name[16] = 0;

	StringCchPrintf(secname, _countof(secname), _T("Prog%i"), num);
	::WritePrivateProfileSection(secname, NULL, filename);
	WriteLFOParam(&voice, secname, filename);
	::WritePrivateProfileString(secname, _T("Name"), CA2T(name), filename);
	theConfig->BuildOPNVoice(&voice, 0, param, _countof(param));
	::WritePrivateProfileString(secname, _T("ALFB"), param, filename);
	for (int k = 0; k < 4; k++) {
		TCHAR keyname[16];
		StringCchPrintf(keyname, _countof(keyname), _T("OP%i"), k + 1);
		theConfig->BuildOPNVoice(&voice, k + 1, param, _countof(param));
		::WritePrivateProfileString(secname, keyname, param, filename);
	}
}

void CFITOMApp::SaveOPL3VoiceBank(CFMBank* bank, CProgressDlg* pDlg) {
	LPCTSTR filename = bank->GetFileName();
	if (::PathFileExists(filename)) {
		::WritePrivateProfileSection(_T("Header"), NULL, filename);
		::WritePrivateProfileSection(_T("Bank"), NULL, filename);
		::WritePrivateProfileString(_T("Header"), _T("Type"), _T("OPL3"), filename);
		::WritePrivateProfileString(_T("Header"), _T("BankName"), bank->GetBankName(), filename);
		pDlg ? pDlg->SetPos(1) : 0;
		for (int i = 0; i < 128; i++) {
			SaveOPL3Voice(bank, i);
			pDlg ? pDlg->SetPos(i + 2) : 0;
		}
	}
}

void CFITOMApp::SaveMA3VoiceBank(CFMBank* bank, CProgressDlg* pDlg) {
	LPCTSTR filename = bank->GetFileName();
	if (::PathFileExists(filename)) {
		::WritePrivateProfileSection(_T("Header"), NULL, filename);
		::WritePrivateProfileSection(_T("Bank"), NULL, filename);
		::WritePrivateProfileString(_T("Header"), _T("Type"), _T("MA3"), filename);
		::WritePrivateProfileString(_T("Header"), _T("BankName"), bank->GetBankName(), filename);
		pDlg ? pDlg->SetPos(1) : 0;
		for (int i = 0; i < 128; i++) {
			SaveMA3Voice(bank, i);
			pDlg ? pDlg->SetPos(i + 2) : 0;
		}
	}
}

void CFITOMApp::SaveOPL3Voice(CFMBank* bank, int num)
{
	LPCTSTR filename = bank->GetFileName();
	TCHAR secname[16];
	TCHAR param[80];
	FMVOICE voice;
	bank->GetVoice(num, &voice);
	char name[17];
	memcpy(name, voice.name, 16);
	name[16] = 0;

	StringCchPrintf(secname, _countof(secname), _T("Prog%i"), num);
	::WritePrivateProfileSection(secname, NULL, filename);
	WriteLFOParam(&voice, secname, filename);
	::WritePrivateProfileString(secname, _T("Name"), CA2T(name), filename);
	theConfig->BuildOPL3Voice(&voice, 0, param, _countof(param));
	::WritePrivateProfileString(secname, _T("ALFB"), param, filename);
	for (int k = 0; k < 4; k++) {
		TCHAR keyname[16];
		StringCchPrintf(keyname, _countof(keyname), _T("OP%i"), k + 1);
		theConfig->BuildOPL3Voice(&voice, k + 1, param, _countof(param));
		::WritePrivateProfileString(secname, keyname, param, filename);
	}
}

void CFITOMApp::SaveMA3Voice(CFMBank* bank, int num)
{
	LPCTSTR filename = bank->GetFileName();
	TCHAR secname[16];
	TCHAR param[80];
	FMVOICE voice;
	bank->GetVoice(num, &voice);
	char name[17];
	memcpy(name, voice.name, 16);
	name[16] = 0;

	StringCchPrintf(secname, _countof(secname), _T("Prog%i"), num);
	::WritePrivateProfileSection(secname, NULL, filename);
	WriteLFOParam(&voice, secname, filename);
	::WritePrivateProfileString(secname, _T("Name"), CA2T(name), filename);
	theConfig->BuildMA3Voice(&voice, 0, param, _countof(param));
	::WritePrivateProfileString(secname, _T("ALFB"), param, filename);
	for (int k = 0; k < 4; k++) {
		TCHAR keyname[16];
		StringCchPrintf(keyname, _countof(keyname), _T("OP%i"), k + 1);
		theConfig->BuildMA3Voice(&voice, k + 1, param, _countof(param));
		::WritePrivateProfileString(secname, keyname, param, filename);
	}
}

void CFITOMApp::SaveOPL2VoiceBank(CFMBank* bank, CProgressDlg* pDlg) {
	LPCTSTR filename = bank->GetFileName();
	if (::PathFileExists(filename)) {
		::WritePrivateProfileSection(_T("Header"), NULL, filename);
		::WritePrivateProfileSection(_T("Bank"), NULL, filename);
		::WritePrivateProfileString(_T("Header"), _T("Type"), _T("OPL2"), filename);
		::WritePrivateProfileString(_T("Header"), _T("BankName"), bank->GetBankName(), filename);
		pDlg ? pDlg->SetPos(1) : 0;
		for (int i = 0; i < 128; i++) {
			SaveOPL2Voice(bank, i);
			pDlg ? pDlg->SetPos(i + 2) : 0;
		}
	}
}

void CFITOMApp::SaveOPL2Voice(CFMBank* bank, int num)
{
	LPCTSTR filename = bank->GetFileName();
	TCHAR secname[16];
	TCHAR param[80];
	FMVOICE voice;
	bank->GetVoice(num, &voice);
	char name[17];
	memcpy(name, voice.name, 16);
	name[16] = 0;
	StringCchPrintf(secname, _countof(secname), _T("Prog%i"), num);
	::WritePrivateProfileSection(secname, NULL, filename);
	WriteLFOParam(&voice, secname, filename);
	::WritePrivateProfileString(secname, _T("Name"), CA2T(name), filename);
	theConfig->BuildOPL2Voice(&voice, 0, param, _countof(param));
	::WritePrivateProfileString(secname, _T("ALFB"), param, filename);
	for (int k = 0; k < 2; k++) {
		TCHAR keyname[16];
		StringCchPrintf(keyname, _countof(keyname), _T("OP%i"), k + 1);
		theConfig->BuildOPL2Voice(&voice, k + 1, param, _countof(param));
		::WritePrivateProfileString(secname, keyname, param, filename);
	}
}

void CFITOMApp::SaveOPLLVoiceBank(CFMBank* bank, CProgressDlg* pDlg) {
	LPCTSTR filename = bank->GetFileName();
	if (::PathFileExists(filename)) {
		::WritePrivateProfileSection(_T("Header"), NULL, filename);
		::WritePrivateProfileSection(_T("Bank"), NULL, filename);
		::WritePrivateProfileString(_T("Header"), _T("Type"), _T("OPLL"), filename);
		::WritePrivateProfileString(_T("Header"), _T("BankName"), bank->GetBankName(), filename);
		pDlg ? pDlg->SetPos(1) : 0;
		for (int i = 0; i < 128; i++) {
			SaveOPLLVoice(bank, i);
			pDlg ? pDlg->SetPos(i + 2) : 0;
		}
	}
}

void CFITOMApp::SaveOPLLVoice(CFMBank* bank, int num)
{
	LPCTSTR filename = bank->GetFileName();
	TCHAR secname[16];
	TCHAR param[80];
	FMVOICE voice;
	bank->GetVoice(num, &voice);
	char name[17];
	memcpy(name, voice.name, 16);
	name[16] = 0;
	StringCchPrintf(secname, _countof(secname), _T("Prog%i"), num);
	::WritePrivateProfileSection(secname, NULL, filename);
	WriteLFOParam(&voice, secname, filename);
	::WritePrivateProfileString(secname, _T("Name"), CA2T(name), filename);
	theConfig->BuildOPLLVoice(&voice, 0, param, _countof(param));
	::WritePrivateProfileString(secname, _T("ALFB"), param, filename);
	for (int k = 0; k < 2 && !(voice.AL & 0x40); k++) {
		TCHAR keyname[16];
		StringCchPrintf(keyname, _countof(keyname), _T("OP%i"), k + 1);
		theConfig->BuildOPLLVoice(&voice, k + 1, param, _countof(param));
		::WritePrivateProfileString(secname, keyname, param, filename);
	}
}

void CFITOMApp::SavePSGVoiceBank(CFMBank* bank, CProgressDlg* pDlg) {
	LPCTSTR filename = bank->GetFileName();
	if (::PathFileExists(filename)) {
		::WritePrivateProfileSection(_T("Header"), NULL, filename);
		::WritePrivateProfileSection(_T("Bank"), NULL, filename);
		::WritePrivateProfileString(_T("Header"), _T("Type"), _T("PSG"), filename);
		::WritePrivateProfileString(_T("Header"), _T("BankName"), bank->GetBankName(), filename);
		pDlg ? pDlg->SetPos(1) : 0;
		for (int i = 0; i < 128; i++) {
			SavePSGVoice(bank, i);
			pDlg ? pDlg->SetPos(i + 2) : 0;
		}
	}
}

void CFITOMApp::SavePSGVoice(CFMBank* bank, int num)
{
	LPCTSTR filename = bank->GetFileName();
	TCHAR param[80];
	TCHAR secname[16];
	FMVOICE voice;
	bank->GetVoice(num, &voice);
	char name[17];
	memcpy(name, voice.name, 16);
	name[16] = 0;
	StringCchPrintf(secname, _countof(secname), _T("Prog%i"), num);
	::WritePrivateProfileSection(secname, NULL, filename);
	WriteLFOParam(&voice, secname, filename);
	::WritePrivateProfileString(secname, _T("Name"), CA2T(name), filename);
	theConfig->BuildPSGVoice(&voice, 0, param, _countof(param));
	::WritePrivateProfileString(secname, _T("ALFB"), param, filename);
	for (int k = 0; k < 1; k++) {
		TCHAR keyname[16];
		StringCchPrintf(keyname, _countof(keyname), _T("OP%i"), k + 1);
		theConfig->BuildPSGVoice(&voice, k + 1, param, _countof(param));
		::WritePrivateProfileString(secname, keyname, param, filename);
	}
}

void CFITOMApp::SaveDrumMapBank(CDrumBank* bank, CProgressDlg* pDlg)
{
	LPCTSTR filename = bank->GetFileName();
	if (::PathFileExists(filename)) {
		::WritePrivateProfileSection(_T("Header"), NULL, filename);
		::WritePrivateProfileSection(_T("Bank"), NULL, filename);
		::WritePrivateProfileString(_T("Header"), _T("Type"), _T("RHYTHM"), filename);
		::WritePrivateProfileString(_T("Header"), _T("BankName"), bank->GetBankName(), filename);
		pDlg ? pDlg->SetPos(1) : 0;
		for (int i = 0; i < 128; i++) {
			SaveDrumMap(bank, i);
			pDlg ? pDlg->SetPos(i + 2) : 0;
		}
	}
}

void CFITOMApp::SaveDrumMap(CDrumBank* bank, int num)
{
	LPCTSTR filename = bank->GetFileName();
	TCHAR param[128];
	DRUMMAP drumnote;
	bank->GetVoice(num, &drumnote);
	if (drumnote.device != 0) {
		TCHAR note[20];
		if (drumnote.num & 0x80) {
			if (drumnote.fnum) {
				StringCchPrintf(note, _countof(note), _T("#%i:%04X"), drumnote.num & 0x7f, drumnote.fnum);
			}
			else {
				StringCchPrintf(note, _countof(note), _T("#%i"), drumnote.num & 0x7f);
			}
		}
		else {
			if (drumnote.fnum) {
				StringCchPrintf(note, _countof(note), _T("%i"), drumnote.num);
			}
			else {
				StringCchPrintf(note, _countof(note), _T("%i:%i"), drumnote.num, drumnote.fnum);
			}
		}
		TCHAR devparam[80];
		uint32_t phyid;
		phyid = drumnote.device->GetDevPort()->GetPhysicalId();
		if (drumnote.devID & 0x80000000) {	//logical device
			StringCchPrintf(devparam, _countof(devparam), _T("%s"), CFITOM::GetDeviceNameFromID(drumnote.devID & 0xff));
		}
		else if (theConfig->isPcmDevice(drumnote.devID)) {
			StringCchPrintf(devparam, _countof(devparam), _T("%s:%i"), CFITOM::GetDeviceNameFromID(drumnote.devID & 0xff),
				theConfig->GetPcmDeviceIndex((CAdPcmBase*)drumnote.device));
		}
		else
		{ //physical device
			StringCchPrintf(devparam, _countof(devparam), _T("%s:%i:%i"), CFITOM::GetDeviceNameFromID(drumnote.devID & 0xff),
				(phyid >> 16) & 0xff, (phyid >> 8) & 0xff);
		}
		StringCchPrintf(param, _countof(param), _T("%s,%s,%i,%i,%s,%i,%i"),
			drumnote.name, devparam, drumnote.bank, drumnote.prog, note, drumnote.pan, drumnote.gate);
		TCHAR keyname[20];
		StringCchPrintf(keyname, _countof(keyname), _T("Note%i"), num);
		::WritePrivateProfileString(_T("Bank"), keyname, param, filename);
	}
}

BOOL CFITOMApp::OnIdle(LONG lCount)
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	//if (theFitom && bRunning) {
	//	theFitom->PollingCallBack();
	//}
	return CWinApp::OnIdle(lCount);
}
