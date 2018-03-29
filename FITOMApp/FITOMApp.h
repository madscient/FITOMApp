
// FITOMApp.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CFITOMApp:
// このクラスの実装については、FITOMApp.cpp を参照してください。
//

class CSplashDlg;
class CProgressDlg;
class CFITOMApp : public CWinApp
{
public:
	struct ERROR_MAP {
		int FITOM_ERR;
		int ERR_IDS;
	};
protected:
	UINT hTimer;
	HANDLE hPolling;
	HANDLE hCommand;
	volatile UINT32 lltickcount;
	volatile BOOL bRunning;
	CFITOM* theFitom;
	CFITOMConfigWin32* theConfig;
	static ERROR_MAP errmap[];
	FMVOICE* clipvoice;
	DRUMMAP* clipdrum;
	CSplashDlg* pSplash;
	void WriteLFOParam(FMVOICE* voice, LPCTSTR secname, LPCTSTR filename);
	CFITOMConfigWin32* LoadConfig();
	void SaveOPMVoice(CFMBank* bank, int num);
	void SaveOPNVoice(CFMBank* bank, int num);
	void SaveOPL2Voice(CFMBank* bank, int num);
	void SaveOPL3Voice(CFMBank* bank, int num);
	void SaveMA3Voice(CFMBank* bank, int num);
	void SaveOPLLVoice(CFMBank* bank, int num);
	void SavePSGVoice(CFMBank* bank, int num);
	void SaveDrumMap(CDrumBank* bank, int num);
	void SaveVoiceBank(void(CFITOMApp::*saveFunc)(CFMBank*, int), CFMBank* bank, LPCTSTR vtype, CProgressDlg* pDlg=0);
	void SaveOPMVoiceBank(CFMBank* bank, CProgressDlg* pDlg = 0);
	void SaveOPNVoiceBank(CFMBank* bank, CProgressDlg* pDlg = 0);
	void SaveOPL2VoiceBank(CFMBank* bank, CProgressDlg* pDlg = 0);
	void SaveOPL3VoiceBank(CFMBank* bank, CProgressDlg* pDlg = 0);
	void SaveMA3VoiceBank(CFMBank* bank, CProgressDlg* pDlg = 0);
	void SaveOPLLVoiceBank(CFMBank* bank, CProgressDlg* pDlg = 0);
	void SavePSGVoiceBank(CFMBank* bank, CProgressDlg* pDlg = 0);
	void SaveDrumMapBank(CDrumBank* bank, CProgressDlg* pDlg = 0);
public:
	CFITOMApp();
	int FITOMStart();
	int FITOMStop();
	CFITOM* GetFitom() { return theFitom; };
	CFITOMConfig* GetConfig() { return theConfig; };
	void VoiceCopy(FMVOICE* voice);
	void DrumCopy(DRUMMAP* drumnote);
	BOOL IsVoiceInClipBoard();
	BOOL IsDrumInClipBoard();
	BOOL GetVoiceFromClipBoard(FMVOICE* voice);
	BOOL GetDrumFromClipBoard(DRUMMAP* voice);
	void ResetInterface();
	void InitDevice();

	void DoEvents();
	void SplashMessage(LPCTSTR str);
	void SplashStatus(LPCTSTR str);
	void SaveVoice(int group, int bank, int prog=-1);
	//スタティック
	static void CALLBACK TimerProc(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dummy1, DWORD dummy2);
	static unsigned int CALLBACK PollingProc(void* params);
	static unsigned int CALLBACK CommandProc(void* params);
// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
};

extern CFITOMApp theApp;
