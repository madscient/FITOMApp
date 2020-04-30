
// FITOMApp.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ɑ΂��Ă��̃t�@�C�����C���N���[�h����O�� 'stdafx.h' ���C���N���[�h���Ă�������"
#endif

#include "resource.h"		// ���C�� �V���{��


// CFITOMApp:
// ���̃N���X�̎����ɂ��ẮAFITOMApp.cpp ���Q�Ƃ��Ă��������B
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
	volatile uint32_t lltickcount;
	volatile bool bRunning;
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
	bool IsVoiceInClipBoard();
	bool IsDrumInClipBoard();
	bool GetVoiceFromClipBoard(FMVOICE* voice);
	bool GetDrumFromClipBoard(DRUMMAP* voice);
	void ResetInterface();
	void InitDevice();

	void DoEvents();
	void SplashMessage(LPCTSTR str);
	void SplashStatus(LPCTSTR str);
	void SaveVoice(int group, int bank, int prog=-1);
	//�X�^�e�B�b�N
	static void CALLBACK TimerProc(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dummy1, DWORD dummy2);
	static unsigned int CALLBACK PollingProc(void* params);
	static unsigned int CALLBACK CommandProc(void* params);
// �I�[�o�[���C�h
public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
};

extern CFITOMApp theApp;
