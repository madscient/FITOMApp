// VoiceEditDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "FITOMdefine.h"
#include "FITOMApp.h"
#include "VoiceEditDlg.h"
#include "VoicePicker.h"
#include "afxdialogex.h"
#include "FITOMCfg.h"
#include "TextVoiceEditDlg.h"

// データテーブル
CVoiceEditDlg::VoiceItem CVoiceEditDlg::commonItem[] = {
	{ _T("Feedback"), 0, 0, 7, VOICE_GROUP_OPM | VOICE_GROUP_OPNA | VOICE_GROUP_OPL2 | VOICE_GROUP_OPLL, &CVoiceEditDlg::SetFB, &CVoiceEditDlg::GetFB, 0, },
	{ _T("Feedback1"), 0, 0, 7, VOICE_GROUP_OPL3 | VOICE_GROUP_MA3, &CVoiceEditDlg::SetFB1, &CVoiceEditDlg::GetFB1, 0, },
	{ _T("Feedback2"), 0, 0, 7, VOICE_GROUP_OPL3 | VOICE_GROUP_MA3, &CVoiceEditDlg::SetFB2, &CVoiceEditDlg::GetFB2, 0, },
	{ _T("Algorithm"), 0, 0, 7, VOICE_GROUP_OPM | VOICE_GROUP_MA3, &CVoiceEditDlg::SetAL3, &CVoiceEditDlg::GetAL3, &CVoiceEditDlg::UpdateAlgoView, },
	{ _T("Algorithm"), 0, 0, 15, VOICE_GROUP_OPNA, &CVoiceEditDlg::SetAL, &CVoiceEditDlg::GetAL4, &CVoiceEditDlg::UpdateAlgoView, },
	{ _T("Algorithm"), 0, 0, 1, VOICE_GROUP_OPL2, &CVoiceEditDlg::SetAL, &CVoiceEditDlg::GetAL1, &CVoiceEditDlg::UpdateAlgoView, },
	{ _T("Algorithm"), 0, 0, 11, VOICE_GROUP_OPL3, &CVoiceEditDlg::SetAL, &CVoiceEditDlg::GetAL4, &CVoiceEditDlg::UpdateAlgoView, },
	{ _T("ROM Tone"), 0, 0, 15, VOICE_GROUP_OPLL, &CVoiceEditDlg::SetROMTN, &CVoiceEditDlg::GetAL4, &CVoiceEditDlg::UpdateAlgoView, },
	{ _T("Tone/Noise"), 0, 0, 7, VOICE_GROUP_PSG, &CVoiceEditDlg::SetAL, &CVoiceEditDlg::GetAL, 0, },
	{ _T("Noise Enable"), 0, 0, 3, VOICE_GROUP_OPM, &CVoiceEditDlg::SetNE, &CVoiceEditDlg::GetNE, 0, },
	{ _T("Noise Freq"), 0, 0, 31, VOICE_GROUP_OPM, &CVoiceEditDlg::SetNFreq, &CVoiceEditDlg::GetNFreq, 0, },
	{ _T("Noise Freq(L)"), 0, 0, 31, VOICE_GROUP_PSG, &CVoiceEditDlg::SetNFreq, &CVoiceEditDlg::GetNFreq, 0, },
	{ _T("Noise Freq(H)"), 0, 0, 7, VOICE_GROUP_PSG, &CVoiceEditDlg::SetFB, &CVoiceEditDlg::GetFB, 0, },
	{ _T("H-LFO Freq"), 0, 0, 3, VOICE_GROUP_MA3, &CVoiceEditDlg::SetAMS, &CVoiceEditDlg::GetAMS, 0, },
	{ _T("Basic Octave"), 0, 0, 3, VOICE_GROUP_MA3, &CVoiceEditDlg::SetPMS, &CVoiceEditDlg::GetPMS, 0, },
	{ _T("AM Sense"), 0, 0, 3, VOICE_GROUP_OPM | VOICE_GROUP_OPNA, &CVoiceEditDlg::SetAMS, &CVoiceEditDlg::GetAMS, 0, },
	{ _T("PM Sense"), 0, 0, 7, VOICE_GROUP_OPM | VOICE_GROUP_OPNA, &CVoiceEditDlg::SetPMS, &CVoiceEditDlg::GetPMS, 0, },
	{ _T("P-LFO Wave"), 0, 0, 14, VOICE_GROUP_ALL, &CVoiceEditDlg::SetLFOWave, &CVoiceEditDlg::GetLFOWave, 0, },
	{ _T("P-LFO Depth"), 0, -8192, 8191, VOICE_GROUP_ALL, &CVoiceEditDlg::SetLFODepth, &CVoiceEditDlg::GetLFODepth, 0, },
	{ _T("P-LFO Freq"), 0, 0, 15, VOICE_GROUP_ALL, &CVoiceEditDlg::SetLFOFreq, &CVoiceEditDlg::GetLFOFreq, 0, },
	{ _T("P-LFO Delay"), 0, 0, 127, VOICE_GROUP_ALL, &CVoiceEditDlg::SetLFODelay, &CVoiceEditDlg::GetLFODelay, 0, },
	{ _T("P-LFO Rate"), 0, 0, 127, VOICE_GROUP_ALL, &CVoiceEditDlg::SetLFORate, &CVoiceEditDlg::GetLFORate, 0, },
	{ 0, 0, 0, 0, VOICE_GROUP_NONE, 0, 0, 0, },
};

CVoiceEditDlg::VoiceItem CVoiceEditDlg::operatorItem[] = {
	{ _T("Attack Rate"), 0, 0, 31, VOICE_GROUP_OPM | VOICE_GROUP_OPNA, &CVoiceEditDlg::SetAR5, &CVoiceEditDlg::GetAR5, &CVoiceEditDlg::UpdateEnvView, },
	{ _T("Decay Rate"), 0, 0, 31, VOICE_GROUP_OPM | VOICE_GROUP_OPNA, &CVoiceEditDlg::SetDR5, &CVoiceEditDlg::GetDR5, 0, },
	{ _T("Attack Rate"), 0, 0, 15, VOICE_GROUP_OPL2 | VOICE_GROUP_OPL3 | VOICE_GROUP_OPLL | VOICE_GROUP_MA3, &CVoiceEditDlg::SetAR4, &CVoiceEditDlg::GetAR4, &CVoiceEditDlg::UpdateEnvView, },
	{ _T("Decay Rate"), 0, 0, 15, VOICE_GROUP_OPL2 | VOICE_GROUP_OPL3 | VOICE_GROUP_OPLL | VOICE_GROUP_MA3, &CVoiceEditDlg::SetDR4, &CVoiceEditDlg::GetDR4, &CVoiceEditDlg::UpdateEnvView, },
	{ _T("Attack Rate"), 0, 0, 127, VOICE_GROUP_PSG, &CVoiceEditDlg::SetAR, &CVoiceEditDlg::GetAR, &CVoiceEditDlg::UpdateEnvView, },
	{ _T("Decay Rate"), 0, 0, 127, VOICE_GROUP_PSG, &CVoiceEditDlg::SetDR, &CVoiceEditDlg::GetDR, &CVoiceEditDlg::UpdateEnvView, },
	{ _T("Sustain Level"), 0, 0, 15, VOICE_GROUP_ALL & (~VOICE_GROUP_PSG), &CVoiceEditDlg::SetSL4, &CVoiceEditDlg::GetSL4, &CVoiceEditDlg::UpdateEnvView, },
	{ _T("Sustain Level"), 0, 0, 127, VOICE_GROUP_PSG, &CVoiceEditDlg::SetSL, &CVoiceEditDlg::GetSL, &CVoiceEditDlg::UpdateEnvView, },
	{ _T("Sustain Rate"), 0, 0, 31, VOICE_GROUP_OPM | VOICE_GROUP_OPNA, &CVoiceEditDlg::SetSR5, &CVoiceEditDlg::GetSR5, &CVoiceEditDlg::UpdateEnvView, },
	{ _T("Sustain Rate"), 0, 0, 15, VOICE_GROUP_OPL2 | VOICE_GROUP_OPL3 | VOICE_GROUP_OPLL | VOICE_GROUP_MA3, &CVoiceEditDlg::SetSR4, &CVoiceEditDlg::GetSR4, &CVoiceEditDlg::UpdateEnvView, },
	{ _T("Sustain Rate"), 0, 0, 127, VOICE_GROUP_PSG, &CVoiceEditDlg::SetSR, &CVoiceEditDlg::GetSR, &CVoiceEditDlg::UpdateEnvView, },
	{ _T("Release Rate"), 0, 0, 15, VOICE_GROUP_ALL & (~VOICE_GROUP_PSG), &CVoiceEditDlg::SetRR4, &CVoiceEditDlg::GetRR4, &CVoiceEditDlg::UpdateEnvView, },
	{ _T("Release Rate"), 0, 0, 127, VOICE_GROUP_PSG, &CVoiceEditDlg::SetRR, &CVoiceEditDlg::GetRR, &CVoiceEditDlg::UpdateEnvView, },
	{ _T("Reverbation"), 0, 0, 7, VOICE_GROUP_OPM, &CVoiceEditDlg::SetRV4, &CVoiceEditDlg::GetRV4, 0, },
	//{ _T("Reverbation"), 0, 0, 127, VOICE_GROUP_PSG, &CVoiceEditDlg::SetRV, &CVoiceEditDlg::GetRV, 0, },
	{ _T("Total Level"), 0, 0, 127, VOICE_GROUP_OPM | VOICE_GROUP_OPNA, &CVoiceEditDlg::SetTL, &CVoiceEditDlg::GetTL, 0, },
	{ _T("Total Level"), 0, 0, 63, VOICE_GROUP_OPL2 | VOICE_GROUP_OPL3 | VOICE_GROUP_OPLL | VOICE_GROUP_MA3, &CVoiceEditDlg::SetTL6, &CVoiceEditDlg::GetTL6, 0, },
	{ _T("EG-Bias"), 0, 0, 3, VOICE_GROUP_OPM, &CVoiceEditDlg::SetEGS5, &CVoiceEditDlg::GetEGS5, 0, },
	{ _T("EG-Bias"), 0, 0, 127, VOICE_GROUP_PSG, &CVoiceEditDlg::SetEGS, &CVoiceEditDlg::GetEGS, 0, },
	{ _T("KS-Level"), 0, 0, 3, VOICE_GROUP_OPL2 | VOICE_GROUP_OPL3 | VOICE_GROUP_OPLL | VOICE_GROUP_MA3, &CVoiceEditDlg::SetKSL, &CVoiceEditDlg::GetKSL, 0, },
	{ _T("KS-Rate"), 0, 0, 3, VOICE_GROUP_OPM | VOICE_GROUP_OPNA | VOICE_GROUP_OPL2 | VOICE_GROUP_OPL3 | VOICE_GROUP_OPLL | VOICE_GROUP_MA3, &CVoiceEditDlg::SetKSR, &CVoiceEditDlg::GetKSR, 0, },
	{ _T("SSG-EG"), 0, 0, 31, VOICE_GROUP_OPNA | VOICE_GROUP_PSG, &CVoiceEditDlg::SetEG, &CVoiceEditDlg::GetEG, 0, },
	{ _T("AM Enable"), 0, 0, 1, VOICE_GROUP_OPM | VOICE_GROUP_OPNA | VOICE_GROUP_OPL2 | VOICE_GROUP_OPL3 | VOICE_GROUP_OPLL, &CVoiceEditDlg::SetAME, &CVoiceEditDlg::GetAME, 0, },
	{ _T("AM Depth"), 0, 0, 7, VOICE_GROUP_MA3, &CVoiceEditDlg::SetAME, &CVoiceEditDlg::GetAME, 0, },
	{ _T("VIB Enable"), 0, 0, 1, VOICE_GROUP_OPL2 | VOICE_GROUP_OPL3 | VOICE_GROUP_OPLL | VOICE_GROUP_MA3, &CVoiceEditDlg::SetVIB, &CVoiceEditDlg::GetVIB, 0, },
	{ _T("VIB Depth"), 0, 0, 7, VOICE_GROUP_MA3, &CVoiceEditDlg::SetVIB, &CVoiceEditDlg::GetVIB, 0, },
	{ _T("Osc Fix"), 0, 0, 1, VOICE_GROUP_OPM, &CVoiceEditDlg::SetFIX, &CVoiceEditDlg::GetFIX, 0, },
	{ _T("Wave Select"), 0, 0, 1, VOICE_GROUP_OPLL, &CVoiceEditDlg::SetWS, &CVoiceEditDlg::GetWS, &CVoiceEditDlg::UpdateWaveView, },
	{ _T("Wave Select"), 0, 0, 7, VOICE_GROUP_OPM | VOICE_GROUP_OPL2 | VOICE_GROUP_OPL3, &CVoiceEditDlg::SetWS, &CVoiceEditDlg::GetWS, &CVoiceEditDlg::UpdateWaveView, },
	{ _T("Wave Select"), 0, 0, 31, VOICE_GROUP_MA3, &CVoiceEditDlg::SetWS, &CVoiceEditDlg::GetWS, &CVoiceEditDlg::UpdateWaveView, },
	{ _T("Wave Select"), 0, 0, 127, VOICE_GROUP_PSG, &CVoiceEditDlg::SetWS, &CVoiceEditDlg::GetWS, &CVoiceEditDlg::UpdateWaveView, },
	{ _T("Multiple"), 0, 0, 15, VOICE_GROUP_OPM | VOICE_GROUP_OPNA | VOICE_GROUP_OPL2 | VOICE_GROUP_OPL3 | VOICE_GROUP_OPLL, &CVoiceEditDlg::SetML, &CVoiceEditDlg::GetML, &CVoiceEditDlg::UpdateFreqView, },
	{ _T("Detune1"), 0, 0, 7, VOICE_GROUP_OPM | VOICE_GROUP_OPNA | VOICE_GROUP_MA3, &CVoiceEditDlg::SetDT1, &CVoiceEditDlg::GetDT1, &CVoiceEditDlg::UpdateFreqView, },
	{ _T("Detune2"), 0, 0, 3, VOICE_GROUP_OPM, &CVoiceEditDlg::SetDT2, &CVoiceEditDlg::GetDT2, &CVoiceEditDlg::UpdateFreqView, },
	{ _T("Fine freq"), 0, 0, 15, VOICE_GROUP_OPM, &CVoiceEditDlg::SetDT3, &CVoiceEditDlg::GetDT3, &CVoiceEditDlg::UpdateFreqView, },
	{ _T("Pseudo Detune"), 0, -8192, 8191, VOICE_GROUP_OPL3, &CVoiceEditDlg::SetPDT, &CVoiceEditDlg::GetPDT, &CVoiceEditDlg::UpdateFreqView, },
	{ _T("Pseudo Detune"), 0, -8192, 8191, VOICE_GROUP_OPNA, &CVoiceEditDlg::SetPDT2, &CVoiceEditDlg::GetPDT2, 0, },
	{ _T("Noise AND mask"), 0, 0, 255, VOICE_GROUP_PSG, &CVoiceEditDlg::SetNAM, &CVoiceEditDlg::GetNAM, 0, },
	{ _T("Noise OR mask"), 0, 0, 255, VOICE_GROUP_PSG, &CVoiceEditDlg::SetNOM, &CVoiceEditDlg::GetNOM, 0, },
	{ _T("TL-LFO Wave"), 0, 0, 6, VOICE_GROUP_ALL, &CVoiceEditDlg::SetOPLFOWave, &CVoiceEditDlg::GetOPLFOWave, 0, },
	{ _T("TL-LFO Depth"), 0, -64, 63, VOICE_GROUP_ALL, &CVoiceEditDlg::SetOPLFODepth, &CVoiceEditDlg::GetOPLFODepth, 0, },
	{ _T("TL-LFO Freq"), 0, 0, 15, VOICE_GROUP_ALL, &CVoiceEditDlg::SetOPLFOFreq, &CVoiceEditDlg::GetOPLFOFreq, 0, },
	{ _T("TL-LFO Delay"), 0, 0, 127, VOICE_GROUP_ALL, &CVoiceEditDlg::SetOPLFODelay, &CVoiceEditDlg::GetOPLFODelay, 0, },
	{ _T("TL-LFO Rate"), 0, 0, 127, VOICE_GROUP_ALL, &CVoiceEditDlg::SetOPLFORate, &CVoiceEditDlg::GetOPLFORate, 0, },
	{ 0, 0, 0, 0, VOICE_GROUP_NONE, 0, 0, 0, },
};

DWORD opnal[] = {
	IDB_BMP_OPNAL0, IDB_BMP_OPNAL1, IDB_BMP_OPNAL2, IDB_BMP_OPNAL3, IDB_BMP_OPNAL4, IDB_BMP_OPNAL5, IDB_BMP_OPNAL6, IDB_BMP_OPNAL7,
	IDB_BMP_OPNAL8, IDB_BMP_OPNAL9, IDB_BMP_OPNALA, IDB_BMP_OPNALB, IDB_BMP_OPNALC, IDB_BMP_OPNALD, IDB_BMP_OPNALE, IDB_BMP_OPNALF,
	0,
};

DWORD opmal[] = {
	IDB_BMP_OPNAL0, IDB_BMP_OPNAL1, IDB_BMP_OPNAL2, IDB_BMP_OPNAL3, IDB_BMP_OPNAL4, IDB_BMP_OPNAL5, IDB_BMP_OPNAL6, IDB_BMP_OPNAL7,
	IDB_BMP_OPMAL8, IDB_BMP_OPMAL9, IDB_BMP_OPMALA, IDB_BMP_OPMALB, IDB_BMP_OPMALC, IDB_BMP_OPMALD, IDB_BMP_OPMALE, IDB_BMP_OPMALF,
	0,
};

DWORD oplal[] = {
	IDB_BMP_OPLAL0, IDB_BMP_OPLAL1, IDB_BMP_OPLAL2, IDB_BMP_OPLAL3, IDB_BMP_OPLAL4, IDB_BMP_OPLAL5, IDB_BMP_OPLAL6, IDB_BMP_OPLAL7,
	IDB_BMP_OPLAL8, IDB_BMP_OPLAL9, IDB_BMP_OPLALA, IDB_BMP_OPLALB,
	0,
};

DWORD ma3al[] = {
	IDB_BMP_OPLAL0, IDB_BMP_OPLAL1, IDB_BMP_OPLALB, IDB_BMP_OPNAL2, IDB_BMP_OPLAL4, IDB_BMP_OPLAL8, IDB_BMP_OPLAL6, IDB_BMP_OPLAL7,
};

DWORD psgal[] = {
	IDB_BMP_PSG_AL0,
	0,
};

DWORD oplws[] = {
	IDB_BMP_OPLWS0, IDB_BMP_OPLWS1, IDB_BMP_OPLWS2, IDB_BMP_OPLWS3, IDB_BMP_OPLWS4, IDB_BMP_OPLWS5, IDB_BMP_OPLWS6, IDB_BMP_OPLWS7,
	0,
};

DWORD opzws[] = {
	IDB_BMP_OPZWS0, IDB_BMP_OPZWS1, IDB_BMP_OPZWS2, IDB_BMP_OPZWS3, IDB_BMP_OPZWS4, IDB_BMP_OPZWS5, IDB_BMP_OPZWS6, IDB_BMP_OPZWS7,
	0,
};

DWORD ma3ws[] = {
	IDB_BMP_OPLWS0, IDB_BMP_OPLWS1, IDB_BMP_OPLWS2, IDB_BMP_OPLWS3, IDB_BMP_OPLWS4, IDB_BMP_OPLWS5, IDB_BMP_OPLWS6, IDB_BMP_OPLWS7,
	IDB_BMP_MA3WS8, IDB_BMP_MA3WS9, IDB_BMP_MA3WSA, IDB_BMP_MA3WSB, IDB_BMP_MA3WSC, IDB_BMP_MA3WSD, IDB_BMP_MA3WSE, IDB_BMP_NONE_WS,
	IDB_BMP_OPZWS1, IDB_BMP_OPZWS5, IDB_BMP_MA3WSI, IDB_BMP_MA3WSJ, IDB_BMP_OPZWS3, IDB_BMP_OPZWS7, IDB_BMP_MA3WSM, IDB_BMP_NONE_WS,
	IDB_BMP_MA3WSO, IDB_BMP_MA3WSP, IDB_BMP_MA3WSQ, IDB_BMP_MA3WSR, IDB_BMP_MA3WSS, IDB_BMP_MA3WST, IDB_BMP_MA3WSU, IDB_BMP_NONE_WS,
	0,
};

DWORD sinws[] = {
	IDB_BMP_OPLWS0, 0,
};

DWORD freqind[] = {
	IDC_OP_FREQ1, IDC_OP_FREQ2, IDC_OP_FREQ3, IDC_OP_FREQ4, 0,
};

DWORD sliders[] = {
	IDC_SLIDER1, IDC_SLIDER2, IDC_SLIDER3, IDC_SLIDER4, IDC_SLIDER5, 0,
};

CVoiceEditDlg::ImgView CVoiceEditDlg::algoimg[] = {
	{ VOICE_GROUP_PSG, 0, 0, },
	{ VOICE_GROUP_OPM, 15, opmal, },
	{ VOICE_GROUP_OPNA, 15, opnal, },
	{ VOICE_GROUP_OPL2, 7, oplal, },
	{ VOICE_GROUP_OPL3, 11, oplal, },
	{ VOICE_GROUP_OPLL, 0, 0, },
	{ VOICE_GROUP_MA3, 7, ma3al, },
	{ VOICE_GROUP_NONE, 0, 0, },
};

CVoiceEditDlg::ImgView CVoiceEditDlg::waveimg[] = {
	{ VOICE_GROUP_PSG, 0, 0, },
	{ VOICE_GROUP_OPM, 7, opzws, },
	{ VOICE_GROUP_OPNA, 0, sinws, },
	{ VOICE_GROUP_OPL2, 7, oplws, },
	{ VOICE_GROUP_OPL3, 7, oplws, },
	{ VOICE_GROUP_OPLL, 1, oplws, },
	{ VOICE_GROUP_MA3, 31, ma3ws, },
	{ VOICE_GROUP_NONE, 0, 0, },
};

// CVoiceEditDlg ダイアログ

IMPLEMENT_DYNAMIC(CVoiceEditDlg, CDialogEx)

CVoiceEditDlg::CVoiceEditDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVoiceEditDlg::IDD, pParent)
	, theDevice(-1), theBank(-1), theProg(-1)
	, theConfig(((CFITOMApp*)AfxGetApp())->GetConfig())
	, bInit(FALSE), bModified(FALSE), pICh(0)
{
	lstctls[0] = &lstCommon;
	lstctls[1] = &lstOper1;
	lstctls[2] = &lstOper2;
	lstctls[3] = &lstOper3;
	lstctls[4] = &lstOper4;
}

CVoiceEditDlg::~CVoiceEditDlg()
{
	delete pICh;
}

void CVoiceEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CPARAM, lstCommon);
	DDX_Control(pDX, IDC_LIST_OPPARAM1, lstOper1);
	DDX_Control(pDX, IDC_LIST_OPPARAM2, lstOper2);
	DDX_Control(pDX, IDC_LIST_OPPARAM3, lstOper3);
	DDX_Control(pDX, IDC_LIST_OPPARAM4, lstOper4);
	DDX_Control(pDX, IDC_EDIT_NAME, edtName);
	DDX_Control(pDX, IDC_EDIT_DEVICE, edtDevice);
	DDX_Control(pDX, IDC_EDIT_BANK, edtBank);
	DDX_Control(pDX, IDC_EDIT_PROG_NO, edtProg);
	DDX_Control(pDX, IDC_BTN_TEST, btnTest);
	DDX_Control(pDX, IDC_SPIN_VELO, spnVelocity);
	DDX_Control(pDX, IDC_SPIN_NOTE, spnNote);
	DDX_Control(pDX, IDC_PIC_AL, picAL);
	DDX_Control(pDX, IDC_PIC_WS0, picWS0);
	DDX_Control(pDX, IDC_PIC_WS1, picWS1);
	DDX_Control(pDX, IDC_PIC_WS2, picWS2);
	DDX_Control(pDX, IDC_PIC_WS3, picWS3);
	DDX_Control(pDX, IDC_PIC_ENV0, picEnv0);
	DDX_Control(pDX, IDC_PIC_ENV1, picEnv1);
	DDX_Control(pDX, IDC_PIC_ENV2, picEnv2);
	DDX_Control(pDX, IDC_PIC_ENV3, picEnv3);
	DDX_Control(pDX, IDC_SLIDER1, sldCommon);
	DDX_Control(pDX, IDC_SLIDER2, sldOp1);
	DDX_Control(pDX, IDC_SLIDER3, sldOp2);
	DDX_Control(pDX, IDC_SLIDER4, sldOp3);
	DDX_Control(pDX, IDC_SLIDER5, sldOp4);
}


BEGIN_MESSAGE_MAP(CVoiceEditDlg, CDialogEx)
	ON_MESSAGE(WM_KICKIDLE, &CVoiceEditDlg::OnKickIdle)
	ON_COMMAND(IDC_BUTTON_PICK, &CVoiceEditDlg::OnBnClickedButtonPick)
	ON_EN_UPDATE(IDC_EDIT_NAME, &CVoiceEditDlg::OnUpdateEditName)
	ON_NOTIFY(NM_CLICK, IDC_LIST_CPARAM, &CVoiceEditDlg::OnClkListCparam)
	ON_NOTIFY(NM_CLICK, IDC_LIST_OPPARAM1, &CVoiceEditDlg::OnClkListOpparam1)
	ON_NOTIFY(NM_CLICK, IDC_LIST_OPPARAM2, &CVoiceEditDlg::OnClkListOpparam2)
	ON_NOTIFY(NM_CLICK, IDC_LIST_OPPARAM3, &CVoiceEditDlg::OnClkListOpparam3)
	ON_NOTIFY(NM_CLICK, IDC_LIST_OPPARAM4, &CVoiceEditDlg::OnClkListOpparam4)
	ON_COMMAND(IDCANCEL, &CVoiceEditDlg::OnBnClickedCancel)
	ON_EN_CHANGE(IDC_EDIT_NAME, &CVoiceEditDlg::OnChangeEditName)
	ON_COMMAND(IDC_VOICE_COPY, &CVoiceEditDlg::OnVoiceCopy)
	ON_COMMAND(IDC_VOICE_PASTE, &CVoiceEditDlg::OnVoicePaste)
	ON_UPDATE_COMMAND_UI(IDC_VOICE_PASTE, &CVoiceEditDlg::OnUpdateVoicePaste)
	ON_COMMAND(IDC_VOICE_IMPORT, &CVoiceEditDlg::OnBnClickedVoiceText)
	ON_COMMAND(IDC_BTN_REVERT, &CVoiceEditDlg::OnClickedBtnRevert)
	ON_UPDATE_COMMAND_UI(IDC_BTN_REVERT, &CVoiceEditDlg::OnUpdateRevert)
	ON_WM_VSCROLL()
END_MESSAGE_MAP()

void CVoiceEditDlg::SetDevice(UINT32 dev)
{
	theDevice = dev;
	if (bInit && theDevice > 0) {
		CString tmp;
		UINT8 did = theDevice & 0xff;
		TCHAR devname[64];
		theConfig->GetDeviceName(theDevice, devname, _countof(devname));
		tmp.Format(_T("%02X:%s"), did, devname);
		edtDevice.SetWindowText(tmp);
	}
}

void CVoiceEditDlg::SetBank(UINT32 bank)
{
	theBank = bank;
	if (bInit && theDevice > 0) {
		CString tmp;
		tmp.Format(_T("%03i:%s"), theBank, theConfig->GetBankName(theDevice, theBank));
		edtBank.SetWindowText(tmp);
	}
}

void CVoiceEditDlg::SetProg(UINT32 prog)
{
	theProg = prog;

	UINT32 vg = CFITOM::GetDeviceVoiceGroupMask(theDevice);
	if (bInit && theBank >= 0 && theDevice > 0) {
		TCHAR tmp[64];
		theConfig->GetVoiceName(theDevice, theBank, theProg, tmp, _countof(tmp));
		CString str;
		str.Format(_T("%i"), theProg);
		edtProg.SetWindowText(str);
		edtName.SetWindowText(tmp);
	}
}

void CVoiceEditDlg::UpdateListCtrl(int op, BOOL bInit)
{
	if (op <= 4 && op >= 0) {
		int i = 0;
		if (bInit) {
			lstctls[op]->DeleteAllItems();
		}
		for (VoiceItem* items = (op == 0) ? commonItem : operatorItem; items->caption; items++) {
			int vg = CFITOM::GetDeviceVoiceGroupMask(theDevice);
			if (items->mask & vg) {
				if (bInit) {
					lstctls[op]->InsertItem(i, items->caption);
					lstctls[op]->SetItemData(i, DWORD_PTR(items));
				}
				lstctls[op]->SetItemData(i, DWORD_PTR(items));
				CString tmp;
				if (items->pGetter) {
					tmp.Format(_T("%i"), (this->*(items->pGetter))(vg, op-1));
				}
				else if (items->pViewer) {
					//(this->*(items->pViewer))(vg, op-1);
				}
				else {
					tmp = _T("-");
				}
				lstctls[op]->SetItemText(i, 1, tmp);
				i++;
			}
		}
		if (lstctls[op]->GetSelectionMark() < 0) {
			lstctls[op]->SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
			NMITEMACTIVATE ia;
			ia.iItem = 0;
			OnClkListParam(&ia, op);
		}
	}
}

// CVoiceEditDlg メッセージ ハンドラー

BOOL CVoiceEditDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO: ここに初期化を追加してください
	for (int op = 0; op < 5; op++) {
		lstctls[op]->SetExtendedStyle(LVS_EX_FULLROWSELECT);
		lstctls[op]->InsertColumn(0, _T("Parameter"), LVCFMT_LEFT, 84);
		lstctls[op]->InsertColumn(1, _T("Value"), LVCFMT_RIGHT, 46);
	}

	UpdateVoiceView(NULL);
	spnNote.SetRange(0, 127);
	spnVelocity.SetRange(0, 127);
	spnNote.SetPos(60);
	spnVelocity.SetPos(127);

	pICh = new CInstCh(0, CFITOM::GetInstance());
	pICh->BankSelMSB(theDevice);
	pICh->BankSelLSB(theBank);
	pICh->SetVolume(100);
	pICh->SetExpress(127);
	pICh->SetPanpot(64);
	spnVelocity.SetRange32(0, 127);
	spnVelocity.SetPos32(127);
	spnNote.SetRange32(0, 127);
	spnNote.SetPos32(60);
	bModified = FALSE;
	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void CVoiceEditDlg::UpdateVoiceView(FMVOICE* voice)
{
	CString tmp;
	int vg = CFITOM::GetDeviceVoiceGroupMask(theDevice);
	CFMBank* pbank = theConfig->GetFMBank(vg, theBank);
	if (pbank) {
		voice ? (theVoice = *voice) : pbank->GetVoice(theProg, &theVoice);
		memcpy(&orgVoice, &theVoice, sizeof(FMVOICE));
		TCHAR devname[64];
		theConfig->GetDeviceName(theDevice, devname, _countof(devname));
		tmp.Format(_T("%02X:%s"), theDevice & 0xff, devname);
		edtDevice.SetWindowText(tmp);
		tmp.Format(_T("%03i:%s"), theBank, pbank->GetBankName());
		edtBank.SetWindowText(tmp);
		tmp.Format(_T("%3i"), theProg);
		edtProg.SetWindowText(tmp);
		char tmpname[17];
		memcpy(tmpname, theVoice.name, 16);
		tmpname[16] = 0;
		edtName.SetWindowText(CA2T(tmpname));
		for (int op = 0; op < 5; op++) {
			UpdateListCtrl(op, TRUE);
			if (op) {
				UpdateWaveView(vg, op - 1);
				UpdateEnvView(vg, op - 1);
				UpdateFreqView(vg, op - 1);
			}
		}
		UpdateAlgoView(vg, theVoice.AL);
	}
}

void CVoiceEditDlg::UpdateAlgoView(int vg, int op)
{
	int al = GetAL(vg, op);
	for (int i = 0; algoimg[i].vg != VOICE_GROUP_NONE; i++) {
		if (algoimg[i].vg == vg) {
			al = min(al, algoimg[i].bound);
			if (algoimg[i].imglst) {
				picAL.ShowWindow(SW_SHOW);
				picAL.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(algoimg[i].imglst[al])));
			}
			else {
				picAL.ShowWindow(SW_HIDE);
			}
		}
	}
}

void CVoiceEditDlg::UpdateWaveView(int vg, int op)
{
	--op;
	CStretchPicture* picwave[] = { &picWS0, &picWS1, &picWS2, &picWS3, 0, };
	int ws = GetWS(vg, op);
	for (int i = 0; waveimg[i].vg != VOICE_GROUP_NONE; i++) {
		if (waveimg[i].vg == vg) {
			ws = min(ws, waveimg[i].bound);
			if (waveimg[i].imglst) {
				picwave[op & 3]->ShowWindow(SW_SHOW);
				picwave[op & 3]->SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(waveimg[i].imglst[ws])));
			}
			else {
				picwave[op & 3]->ShowWindow(SW_HIDE);
			}
		}
	}
}

void CVoiceEditDlg::UpdateFreqView(int vg, int op)
{
	CString str;
	if (GetFIX(vg, op)) {
		str.Format(_T("Fixed\t%iHz"), (1 << GetDT1(vg, op)) * (GetML(vg, op) * 16 + GetDT3(vg, op)));
	}
	else {
		double oplml[] = { 0.5, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 10.0, 12.0, 12.0, 15.0, 15.0, };
		double opmml[] = { 0.5, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, };
		double opmdt2[] = { 1.0, 1.4142135623730950488016887242097, 1.5703015887805285931588871794778, 1.731073122012286053390184437556, };
		double ML = ((vg & (VOICE_GROUP_OPL2 | VOICE_GROUP_OPL3 | VOICE_GROUP_OPLL | VOICE_GROUP_MA3)) ? oplml : opmml)[GetML(vg, op)];
		double DT2 = (vg & VOICE_GROUP_OPM) ? opmdt2[GetDT2(vg, op) & 3] : double(1.0);
		str.Format(_T("Ratio\tx%2.2f"), ML * DT2);
	}
	SetDlgItemText(freqind[op], str);
}

void CVoiceEditDlg::UpdateEnvView(int vg, int op)
{
	CEnvView* picenv[] = { &picEnv0, &picEnv1, &picEnv2, &picEnv3, 0, };
	FMOP* env = &(theVoice.op[op]);
	if (0 <= op && op < 4) {
		picenv[op]->SetEnv(env->TL, env->EGS, env->AR, env->DR, env->SL, env->SR, env->RR);
		picenv[op]->Invalidate();
	}
}

void CVoiceEditDlg::OnOK()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	if (bModified) {
		int vg = CFITOM::GetDeviceVoiceGroupMask(theDevice);
		CFMBank* pbank = theConfig->GetFMBank(vg, theBank);
		if (pbank) {
			pbank->SetVoice(theProg, &theVoice);
			((CFITOMApp*)AfxGetApp())->SaveVoice(vg, theBank, theProg);
			CFITOM::GetInstance()->ReloadVoice(&theVoice, theDevice, theBank, theProg);
		}
		bModified = FALSE;
	}
}

void CVoiceEditDlg::OnBnClickedButtonPick()
{
	if (!bModified || ::AfxMessageBox(IDS_CONFIRM_VOICE_DISCARD, MB_YESNO) == IDYES) {
		CDlgVoicePicker dlg;
		dlg.SetDevice(theDevice);
		dlg.SetBank(theBank);
		dlg.SetProg(theProg);
		if (dlg.DoModal() == IDOK) {
			SetDevice(dlg.GetDevice());
			SetBank(dlg.GetBank());
			SetProg(dlg.Getprog());
			UpdateVoiceView(NULL);
			bModified = FALSE;
		}
	}
}


void CVoiceEditDlg::OnUpdateEditName()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加してください。
}

void CVoiceEditDlg::OnTestButtonDown()
{
	pICh->BankSelMSB(theDevice);
	pICh->SetVoiceData(&theVoice);
	pICh->NoteOn(spnNote.GetPos(), spnVelocity.GetPos());
}

void CVoiceEditDlg::OnTestButtonUp()
{
	pICh->NoteOff(spnNote.GetPos());
}

BOOL CVoiceEditDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	if (wParam == IDC_BTN_TEST) {
		NMHDR* pnms = (NMHDR*)lParam;
		switch (pnms->code) {
		case WM_LBUTTONDOWN:
			OnTestButtonDown();
			break;
		case WM_LBUTTONUP:
			OnTestButtonUp();
			break;
		}
	}
	return CDialogEx::OnNotify(wParam, lParam, pResult);
}


void CVoiceEditDlg::OnClkListCparam(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	*pResult = OnClkListParam(pNMItemActivate, 0);
}


void CVoiceEditDlg::OnClkListOpparam1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	*pResult = OnClkListParam(pNMItemActivate, 1);
}


void CVoiceEditDlg::OnClkListOpparam2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	*pResult = OnClkListParam(pNMItemActivate, 2);
}


void CVoiceEditDlg::OnClkListOpparam3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	*pResult = OnClkListParam(pNMItemActivate, 3);
}


void CVoiceEditDlg::OnClkListOpparam4(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	*pResult = OnClkListParam(pNMItemActivate, 4);
}

LRESULT CVoiceEditDlg::OnClkListParam(LPNMITEMACTIVATE pNMLV, int op)
{
	CListCtrl* plst = lstctls[op];
	if (pNMLV->iItem < 0) {
		return -1;
	}

	VoiceItem* pvi = (VoiceItem*)(plst->GetItemData(pNMLV->iItem));
	if (pvi->pSetter && pvi->pGetter) {
		CRect ColumnRect;
		int vg = CFITOM::GetDeviceVoiceGroupMask(theDevice);
		plst->GetSubItemRect(pNMLV->iItem, 1, LVIR_BOUNDS, ColumnRect);
		plst->ClientToScreen(ColumnRect);
		this->ScreenToClient(ColumnRect);
		int cxedge = ::GetSystemMetrics(SM_CXEDGE);
		int cyedge = ::GetSystemMetrics(SM_CYEDGE);
		editting_item[op] = pvi;
		CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(sliders[op]);
		pSlider->EnableWindow();
		pSlider->SetRange(0, (pvi->max - pvi->min));
		pSlider->SetPos((pvi->max - pvi->min) - ((this->*(pvi->pGetter))(vg, op - 1) - pvi->min));
	}
	return 0;
}


void CVoiceEditDlg::OnChangeSlider(int op, int nPos)
{
	// TODO: ここにコントロール通知ハンドラー コードを追加してください。
	if (0 <= op && op < 5 && editting_item[op] != 0) {
		//edtInplace.SetSel(0, -1);
		if (editting_item[op]->pSetter) {
			int vg = CFITOM::GetDeviceVoiceGroupMask(theDevice);
			int value = (editting_item[op]->max - editting_item[op]->min) - (nPos - editting_item[op]->min);
			value = min(value, editting_item[op]->max);
			value = max(value, editting_item[op]->min);
			(this->*(editting_item[op]->pSetter))(vg, op - 1, value);
			pICh->SetVoiceData(&theVoice);
			if (editting_item[op]->pViewer) {
				(this->*(editting_item[op]->pViewer))(vg, op);
			}
			UpdateListCtrl(op, FALSE);
			OnClickedBtnApply();
		}
		bModified = TRUE;
	}
	else {
		int nothing = 0;
	}
}

void CVoiceEditDlg::OnClickedButtonBank()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
}

BOOL CVoiceEditDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	if (WM_KEYDOWN == pMsg->message)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:
			return FALSE;
		case VK_ESCAPE:
			return FALSE;
		default:
			break;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CVoiceEditDlg::OnBnClickedCancel()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	if (!bModified || ::AfxMessageBox(IDS_CONFIRM_VOICE_DISCARD, MB_YESNO) == IDYES) {
		UpdateVoiceView(&orgVoice);
		bModified = FALSE;
		OnClickedBtnApply();
		CDialogEx::OnCancel();
	}
}

void CVoiceEditDlg::OnChangeEditName()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加してください。
	TCHAR tmp[17];
	edtName.GetWindowText(tmp, 17);
	tmp[16] = _T(0);
	memcpy(theVoice.name, CT2A(tmp), 16);
	bModified = TRUE;
}

void CVoiceEditDlg::OnVoiceCopy()
{
	// TODO: ここにコマンド ハンドラー コードを追加します。
	((CFITOMApp*)AfxGetApp())->VoiceCopy(&theVoice);
}

void CVoiceEditDlg::OnVoicePaste()
{
	// TODO: ここにコマンド ハンドラー コードを追加します。
	CString str;
	TCHAR name[17];
	FMVOICE clip;
	if (((CFITOMApp*)AfxGetApp())->GetVoiceFromClipBoard(&clip)) {
		memcpy(name, clip.name, 16);
		name[16] = 0;
		str.Format(IDS_CONFIRM_VOICE_PASTE, theVoice.ID, LPCTSTR(name));
		if (AfxMessageBox(str, MB_YESNO) == IDYES) {
			UpdateVoiceView(&clip);
			bModified = TRUE;
		}
	}
}

void CVoiceEditDlg::OnUpdateVoicePaste(CCmdUI *pCmdUI)
{
	// TODO:ここにコマンド更新 UI ハンドラー コードを追加します。
	pCmdUI->Enable(((CFITOMApp*)AfxGetApp())->IsVoiceInClipBoard());
}

LRESULT CVoiceEditDlg::OnKickIdle(WPARAM wParam, LPARAM lParam)
{
	UpdateDialogControls(this, FALSE);

	return FALSE;
}


void CVoiceEditDlg::OnBnClickedVoiceText()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	CTextVoiceEditDlg dlg(this);
	dlg.SetVoice(&theVoice);
	dlg.SetDevice(theDevice);
	if (dlg.DoModal() == IDOK) {
		FMVOICE voice;
		dlg.GetVoice(&voice);
		UpdateVoiceView(&voice);
		bModified = TRUE;
	}
}

void CVoiceEditDlg::OnUpdateRevert(CCmdUI *pCmdUI)
{
	// TODO:ここにコマンド更新 UI ハンドラー コードを追加します。
	pCmdUI->Enable(bModified);
}


void CVoiceEditDlg::OnClickedBtnRevert()
{
	// TODO: ここにコマンド ハンドラー コードを追加します。
	if (AfxMessageBox(IDS_CONFIRM_VOICE_DISCARD, MB_YESNO) == IDYES) {
		UpdateVoiceView(&orgVoice);
		bModified = FALSE;
		OnClickedBtnApply();
	}
}


void CVoiceEditDlg::OnClickedBtnApply()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	int vg = CFITOM::GetDeviceVoiceGroupMask(theDevice);
	CFMBank* pbank = theConfig->GetFMBank(vg, theBank);
	if (pbank) {
		pbank->SetVoice(theProg, &theVoice);
		CFITOM::GetInstance()->ReloadVoice(&theVoice, theDevice, theBank, theProg);
	}
}


void CVoiceEditDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: ここにメッセージ ハンドラー コードを追加するか、既定の処理を呼び出します。
	if ((nSBCode == SB_THUMBTRACK || nSBCode == SB_THUMBPOSITION ||
		nSBCode == SB_LINEUP || nSBCode == SB_LINEDOWN ||
		nSBCode == SB_PAGEUP || nSBCode == SB_PAGEDOWN) && pScrollBar) {
		for (int i = 0; i < 5; i++) {
			UINT32 sender = pScrollBar->GetDlgCtrlID();
			if (sender == sliders[i]) {
				if (nSBCode == SB_LINEUP || nSBCode == SB_LINEDOWN ||
					nSBCode == SB_PAGEUP || nSBCode == SB_PAGEDOWN) {
					nPos += pScrollBar->GetScrollPos();
				}
				OnChangeSlider(i, (int)nPos);
			}
		}
	}
	else {
		CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
	}
}
