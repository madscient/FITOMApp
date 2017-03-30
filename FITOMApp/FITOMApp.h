
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
	void SaveOPMVoice(CFMBank* bank, CProgressDlg* pDlg = 0);
	void SaveOPNVoice(CFMBank* bank, CProgressDlg* pDlg = 0);
	void SaveOPL2Voice(CFMBank* bank, CProgressDlg* pDlg = 0);
	void SaveOPL3Voice(CFMBank* bank, CProgressDlg* pDlg = 0);
	void SaveOPLLVoice(CFMBank* bank, CProgressDlg* pDlg = 0);
	void SavePSGVoice(CFMBank* bank, CProgressDlg* pDlg = 0);
	void SaveDrumMap(CDrumBank* bank, CProgressDlg* pDlg = 0);
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

	void DoEvents();
	void SplashMessage(LPCTSTR str);
	void SplashStatus(LPCTSTR str);
	void SaveVoice(int group, int bank);
	//スタティック
	static void CALLBACK TimerProc(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dummy1, DWORD dummy2);
	static unsigned int CALLBACK PollingProc(void* params);

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
};

extern CFITOMApp theApp;
