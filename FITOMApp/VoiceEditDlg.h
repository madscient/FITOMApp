#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "TestButton.h"
#include "SoundDev.h"
#include "MIDI.h"
#include "EnvView.h"

#define VOICE_PARAM_ENV		1
#define VOICE_PARAM_SLFO	2
#define VOICE_PARAM_WAVE	4
#define VOICE_PARAM_FREQ	8

// CVoiceEditDlg ダイアログ

class CFITOMConfig;

class CVoiceEditDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CVoiceEditDlg)

public:
	CVoiceEditDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CVoiceEditDlg();

	// ダイアログ データ
	enum { IDD = IDD_DLG_VEDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	CFITOMConfig* theConfig;
	BOOL bInit;
	BOOL bModified;
	UINT32 theDevice;
	UINT32 theBank;
	UINT32 theProg;
	FMVOICE theVoice;
	FMVOICE orgVoice;
	CInstCh* pICh;

	struct VoiceItem {
		LPCTSTR caption;
		DWORD edittype;
		int min;
		int max;
		DWORD mask;
		void (CVoiceEditDlg::*pSetter)(int vg, int op, int val);
		int (CVoiceEditDlg::*pGetter)(int vg, int op);
		void (CVoiceEditDlg::*pViewer)(int vg, int op);
	};
	struct ImgView {
		int vg;
		int bound;
		DWORD* imglst;
	};
	VoiceItem* editting_item[5];
	static VoiceItem commonItem[];
	static VoiceItem operatorItem[];
	static ImgView algoimg[];
	static ImgView waveimg[];
	CListCtrl* lstctls[5];

	void UpdateListCtrl(int op, BOOL bInit=FALSE);
	void UpdateVoiceView(FMVOICE* voice);
	void UpdateAlgoView(int vg, int op);
	void UpdateWaveView(int vg, int op);
	void UpdateEnvView(int vg, int op);
	void UpdateFreqView(int vg, int op);
	LRESULT OnClkListParam(LPNMITEMACTIVATE pNMLV, int op);
	void OnChangeSlider(int op, int nPos);

	//Getter
	int GetAL(int vg, int op) { return theVoice.AL; };
	int GetAL1(int vg, int op) { return theVoice.AL & 0x1; };
	int GetAL3(int vg, int op) { return theVoice.AL & 0x7; };
	int GetAL4(int vg, int op) { return theVoice.AL & 0xf; };
	int GetFB(int vg, int op) { return theVoice.FB; };
	int GetFB1(int vg, int op) { return theVoice.FB & 0x7; };
	int GetFB2(int vg, int op) { return theVoice.FB >> 3; };
	int GetNE(int vg, int op) { return theVoice.AL >> 3; };
	int GetNFreq(int vg, int op) { return theVoice.NFQ; };
	int GetNFreq2(int vg, int op) { return (theVoice.NFQ) | (theVoice.FB << 5); };
	int GetAMS(int vg, int op) { return theVoice.AMS; };
	int GetPMS(int vg, int op) { return theVoice.PMS; };
	int GetLFODepth(int vg, int op) { int ldep = ((theVoice.LDM << 7) | theVoice.LDL); return (ldep > 8191) ? (ldep - 16384) : ldep; };
	int GetLFOFreq(int vg, int op) { return theVoice.LFO; };
	int GetLFOWave(int vg, int op) { return theVoice.LWF; };
	int GetLFORate(int vg, int op) { return theVoice.LFR; };
	int GetLFODelay(int vg, int op) { return theVoice.LFD; };
	int GetAR5(int vg, int op) { return theVoice.op[op].AR >> 2; };
	int GetDR5(int vg, int op) { return theVoice.op[op].DR >> 2; };
	int GetSR5(int vg, int op) { return theVoice.op[op].SR >> 2; };
	int GetSL4(int vg, int op) { return theVoice.op[op].SL >> 3; };
	int GetRR4(int vg, int op) { return theVoice.op[op].RR >> 3; };
	int GetAR4(int vg, int op) { return theVoice.op[op].AR >> 3; };
	int GetDR4(int vg, int op) { return theVoice.op[op].DR >> 3; };
	int GetSR4(int vg, int op) { return theVoice.op[op].SR >> 3; };
	int GetRV4(int vg, int op) { return theVoice.op[op].REV >> 3; };
	int GetAR(int vg, int op) { return theVoice.op[op].AR; };
	int GetDR(int vg, int op) { return theVoice.op[op].DR; };
	int GetSR(int vg, int op) { return theVoice.op[op].SR; };
	int GetSL(int vg, int op) { return theVoice.op[op].SL; };
	int GetRR(int vg, int op) { return theVoice.op[op].RR; };
	int GetRV(int vg, int op) { return theVoice.op[op].REV; };
	int GetTL(int vg, int op) { return theVoice.op[op].TL; };
	int GetTL6(int vg, int op) { return (theVoice.op[op].TL > 63) ? 63 : (theVoice.op[op].TL & 63); };
	int GetTL5(int vg, int op) { return theVoice.op[op].TL >> 2; };
	int GetTL4(int vg, int op) { return ((theVoice.op[op].TL > 63) ? 63 : (theVoice.op[op].TL & 63)) >> 2; };
	int GetML(int vg, int op) { return theVoice.op[op].MUL; };
	int GetDT1(int vg, int op) { return theVoice.op[op].DT1; };
	int GetDT2(int vg, int op) { return theVoice.op[op].DT2; };
	int GetDT3(int vg, int op) { return theVoice.op[op].DT3; };
	int GetPDT(int vg, int op) { int pdt = ((GetDT1(vg, op) << 7) | (GetDT2(vg, op))) - 8192; return pdt; };
	int GetPDT2(int vg, int op) { int pdt = ((GetDT3(vg, op) << 7) | (GetDT2(vg, op))) - 8192; return pdt; };
	int GetAME(int vg, int op) { return theVoice.op[op].AM; };
	int GetVIB(int vg, int op) { return theVoice.op[op].VIB; };
	int GetFIX(int vg, int op) { return theVoice.op[op].DM0; };
	int GetEG(int vg, int op) { return theVoice.op[op].EGT; };
	int GetEGS(int vg, int op) { return theVoice.op[op].EGS; };
	int GetEGS5(int vg, int op) { return theVoice.op[op].EGS >> 2; };
	int GetKSL(int vg, int op) { return theVoice.op[op].KSL; };
	int GetKSR(int vg, int op) { return theVoice.op[op].KSR; };
	int GetWS(int vg, int op) { return theVoice.op[op].WS; };
	int GetFix(int vg, int op) { return (1 << theVoice.op[op].DT1) * ((theVoice.op[op].MUL << 4) | theVoice.op[op].DT3); };
	int GetOPLFOFreq(int vg, int op) { return theVoice.op[op].SLF; };
	int GetOPLFODepth(int vg, int op) { return (theVoice.op[op].SLD > 63) ? (theVoice.op[op].SLD - 128) : theVoice.op[op].SLD; };
	int GetOPLFOWave(int vg, int op) { return theVoice.op[op].SLW; };
	int GetOPLFODelay(int vg, int op) { return theVoice.op[op].SLY; };
	int GetOPLFORate(int vg, int op) { return theVoice.op[op].SLR; };
	int GetNAM(int vg, int op) { return ((theVoice.op[op].DT1 & 15) << 4) | (theVoice.op[op].DT2 & 15); };
	int GetNOM(int vg, int op) { return ((theVoice.op[op].DT3 & 15) << 4) | (theVoice.op[op].DT3 & 15); };
	//Setter
	void SetAL(int vg, int op, int val) { theVoice.AL = val; UpdateAlgoView(vg, theVoice.AL);  };
	void SetAL3(int vg, int op, int val) { theVoice.AL = val | (GetNE(vg, op) ? 0x8 : 0); UpdateAlgoView(vg, theVoice.AL); };
	void SetROMTN(int vg, int op, int val) { theVoice.AL = val ? (val | 64) : 0; };
	void SetFB(int vg, int op, int val) { theVoice.FB = val; };
	void SetFB1(int vg, int op, int val) { theVoice.FB = (theVoice.FB & 0x78) | (val & 0x7); };
	void SetFB2(int vg, int op, int val) { theVoice.FB = (theVoice.FB & 0x47) | ((val & 0x7) << 3); };
	void SetNE(int vg, int op, int val) { theVoice.AL = (theVoice.AL & 0x77) | (val ? 8 : 0); };
	void SetNFreq(int vg, int op, int val) { theVoice.NFQ = val; };
	void SetNFreq2(int vg, int op, int val) { theVoice.NFQ = (val & 31); theVoice.FB = (val >> 5); };
	void SetAMS(int vg, int op, int val) { theVoice.AMS = val; };
	void SetPMS(int vg, int op, int val) { theVoice.PMS = val; };
	void SetLFODepth(int vg, int op, int val) { theVoice.LDM = ((val >> 7) & 0x7f); theVoice.LDL = (val & 0x7f); };
	void SetLFOFreq(int vg, int op, int val) { theVoice.LFO = val; };
	void SetLFOWave(int vg, int op, int val) { theVoice.LWF = val; };
	void SetLFORate(int vg, int op, int val) { theVoice.LFR = val; };
	void SetLFODelay(int vg, int op, int val) { theVoice.LFD = val; };
	void SetAR(int vg, int op, int val) { theVoice.op[op].AR = val; };
	void SetDR(int vg, int op, int val) { theVoice.op[op].DR = val; };
	void SetSR(int vg, int op, int val) { theVoice.op[op].SR = val; };
	void SetRR(int vg, int op, int val) { theVoice.op[op].RR = val; };
	void SetSL(int vg, int op, int val) { theVoice.op[op].SL = val; };
	void SetRV(int vg, int op, int val) { theVoice.op[op].REV = val; };
	void SetAR5(int vg, int op, int val) { theVoice.op[op].AR = (val << 2) | (val >> 3); };
	void SetDR5(int vg, int op, int val) { theVoice.op[op].DR = (val << 2) | (val >> 3); };
	void SetSR5(int vg, int op, int val) { theVoice.op[op].SR = (val << 2) | (val >> 3); };
	void SetRR4(int vg, int op, int val) { theVoice.op[op].RR = (val << 3) | (val >> 1); };
	void SetSL4(int vg, int op, int val) { theVoice.op[op].SL = (val << 3) | (val >> 1); };
	void SetAR4(int vg, int op, int val) { theVoice.op[op].AR = (val << 3) | (val >> 1); };
	void SetDR4(int vg, int op, int val) { theVoice.op[op].DR = (val << 3) | (val >> 1); };
	void SetSR4(int vg, int op, int val) { theVoice.op[op].SR = (val << 3) | (val >> 1); };
	void SetRV4(int vg, int op, int val) { theVoice.op[op].REV = (val << 3) | (val >> 1); };
	void SetTL(int vg, int op, int val) { theVoice.op[op].TL = val; };
	void SetTL6(int vg, int op, int val) { theVoice.op[op].TL = (val > 63) ? 63 : val; };
	void SetTL5(int vg, int op, int val) { theVoice.op[op].TL = (val << 2) | (val >> 3); };
	void SetTL4(int vg, int op, int val) { theVoice.op[op].TL = (val > 15) ? 63 : (val << 2); };
	void SetML(int vg, int op, int val) { theVoice.op[op].MUL = val; UpdateFreqView(vg, op); };
	void SetDT1(int vg, int op, int val) { theVoice.op[op].DT1 = val; UpdateFreqView(vg, op); };
	void SetDT2(int vg, int op, int val) { theVoice.op[op].DT2 = val; UpdateFreqView(vg, op); };
	void SetDT3(int vg, int op, int val) { theVoice.op[op].DT3 = val; UpdateFreqView(vg, op); };
	void SetPDT(int vg, int op, int val) { val += 8192;  theVoice.op[op].DT1 = ((val >> 7) & 0x7f); theVoice.op[op].DT2 = (val & 0x7f); };
	void SetPDT2(int vg, int op, int val) { val += 8192;  theVoice.op[op].DT3 = ((val >> 7) & 0x7f); theVoice.op[op].DT2 = (val & 0x7f); };
	void SetAME(int vg, int op, int val) { theVoice.op[op].AM = val; };
	void SetVIB(int vg, int op, int val) { theVoice.op[op].VIB = val; };
	void SetFIX(int vg, int op, int val) { theVoice.op[op].DM0 = val; };
	void SetEG(int vg, int op, int val) { theVoice.op[op].EGT = val; };
	void SetEGS(int vg, int op, int val) { theVoice.op[op].EGS = val; };
	void SetEGS5(int vg, int op, int val) { theVoice.op[op].EGS = (val << 2) | (val >> 3); };
	void SetKSL(int vg, int op, int val) { theVoice.op[op].KSL = val; };
	void SetKSR(int vg, int op, int val) { theVoice.op[op].KSR = val; };
	void SetWS(int vg, int op, int val) { theVoice.op[op].WS = val; };
	void SetOPLFOFreq(int vg, int op, int val) { theVoice.op[op].SLF = val; };
	void SetOPLFODepth(int vg, int op, int val) { theVoice.op[op].SLD = (val & 0x7f); };
	void SetOPLFOWave(int vg, int op, int val) { theVoice.op[op].SLW = val; };
	void SetOPLFODelay(int vg, int op, int val) { theVoice.op[op].SLY = val; };
	void SetOPLFORate(int vg, int op, int val) { theVoice.op[op].SLR = val; };
	void SetNAM(int vg, int op, int val) { theVoice.op[op].DT1 = val >> 4; theVoice.op[op].DT2 = val & 15; };
	void SetNOM(int vg, int op, int val) { theVoice.op[op].DT3 = val >> 4; theVoice.op[op].DT3 = val & 15; };

	DECLARE_MESSAGE_MAP()
public:
	void SetDevice(UINT32 dev);
	void SetBank(UINT32 bank);
	void SetProg(UINT32 prog);
	CListCtrl lstCommon;
	CListCtrl lstOper1;
	CListCtrl lstOper2;
	CListCtrl lstOper3;
	CListCtrl lstOper4;
	afx_msg void OnTestButtonDown();
	afx_msg void OnTestButtonUp();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	CEdit edtName;
	afx_msg void OnBnClickedButtonPick();
	afx_msg void OnUpdateEditName();
	CEdit edtDevice;
	CEdit edtBank;
	CEdit edtProg;
	CTestButton btnTest;
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	CSpinButtonCtrl spnVelocity;
	CSpinButtonCtrl spnNote;
	LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClkListCparam(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClkListOpparam1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClkListOpparam2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClkListOpparam3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClkListOpparam4(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnClickedButtonBank();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnChangeEditName();
	afx_msg void OnVoiceCopy();
	afx_msg void OnVoicePaste();
	afx_msg void OnUpdateVoicePaste(CCmdUI *pCmdUI);
	afx_msg void OnUpdateRevert(CCmdUI *pCmdUI);
	afx_msg void OnBnClickedVoiceText();
	afx_msg void OnClickedBtnRevert();
	afx_msg void OnClickedBtnApply();
	CStatic picAL;
	CStatic picWS0;
	CStatic picWS1;
	CStatic picWS2;
	CStatic picWS3;
	CEnvView picEnv0;
	CEnvView picEnv1;
	CEnvView picEnv2;
	CEnvView picEnv3;
	CSliderCtrl sldCommon;
	CSliderCtrl sldOp1;
	CSliderCtrl sldOp2;
	CSliderCtrl sldOp3;
	CSliderCtrl sldOp4;
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
