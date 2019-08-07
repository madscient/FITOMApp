#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CMidiChCtrlDlg ダイアログ

class CFITOMConfig;

class CMidiChCtrlDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMidiChCtrlDlg)

public:
	CMidiChCtrlDlg(CFITOM* pFitom, int ifid, int ch, CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CMidiChCtrlDlg();

// ダイアログ データ
	enum { IDD = IDD_MIDI_CH_MONITOR };
protected:
	static const int numMultiCtrl = 11;
	static const int numChecker = 6;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
	virtual void OnCancel();
	virtual void OnOK();
	UINT_PTR hTimer;
	int iIF;
	int iCH;
	CFITOM* theFitom;
	CFITOMConfig* theConfig;
	CMidiInst* theInst;
	CMidiCh* theCh;
	void InitDevice();
	void RefreshDevice();
	void RefreshBank();
	void RefreshProg();
	uint32_t prevDev;
	uint32_t prevBank;
public:
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	afx_msg void OnSelendokComboDevice();
	afx_msg void OnSelendokComboBank();
	afx_msg void OnSelendokComboProg();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void SetVolume(int val) { theCh->SetVolume(val); };
	void SetExpr(int val) { theCh->SetExpress(val); };
	void SetModur(int val) { theCh->SetModulation(val); };
	void SetFoot(int val) { theCh->SetFootCtrl(val); };
	void SetPorta(int val) { theCh->SetPortTime(val); };
	void SetRange(int val) { theCh->SetBendRange(val); };
	void SetBend(int val) { theCh->SetPitchBend(val+8192); };
	void SetTune(int val) { theCh->SetFineTune(val + 8192); };
	void SetPoly(int val) { theCh->SetPoly(val); };
	void SetPhyCh(int val) { theCh->SetPhyCh(val); };
	void SetPan(int val) { theCh->SetPanpot(val); };
	void SetLegato(int val) { theCh->SetLegato(val ? 64 : 0); };
	void SetPortOn(int val) { theCh->SetPortamento(val ? 64 : 0); };
	void SetMono(int val) { theCh->SetMono(val ? 1 : 0); };
	void SetSustain(int val) { theCh->SetSustain(val ? 64 : 0); };
	void SetDamp(int val) { theCh->SetForceDamp(val ? 64 : 0); };
	void SetSoste(int val) { theCh->SetSostenuto(val ? 64 : 0); };
	int GetVolume() { return theCh->GetTrackVolume(); };
	int GetExpr() { return theCh->GetExpress(); };
	int GetModur() { return theCh->GetModulation(); };
	int GetFoot() { return theCh->GetFootCtrl(); };
	int GetPorta() { return theCh->GetPortTime(); };
	int GetRange() { return theCh->GetBendRange(); };
	int GetBend() { return theCh->GetPitchBend()-8192; };
	int GetTune() { return theCh->GetFineTune()-8192; };
	int GetPoly() { return theCh->GetPoly(); };
	int GetPhyCh() { return theCh->GetPhyCh(); };
	int GetPan() { return theCh->GetPanpot(); };
	int GetLegato() { return theCh->GetLegato() != 0; };
	int GetPortOn() { return theCh->GetPortamento() != 0; };
	int GetMono() { return theCh->GetPoly() <= 1 ; };
	int GetSustain() { return theCh->GetSustain() != 0; };
	int GetDamp() { return theCh->GetForceDamp() != 0; };
	int GetSoste() { return theCh->GetSostenuto() != 0; };
	CEdit edtMidiIfCh;
	CComboBox cmbDevice;
	CComboBox cmbBank;
	CComboBox cmbProg;
	class CChecker {
	protected:
		BOOL isValid;
		CButton Btn;
		CMidiChCtrlDlg* pParent;
		int (CMidiChCtrlDlg::*pGetter)();
		void (CMidiChCtrlDlg::*pSetter)(int val);
	public:
		CChecker() : pParent(0), pGetter(0), pSetter(0){};
		CChecker(CMidiChCtrlDlg* par, UINT id, int(CMidiChCtrlDlg::*pget)(), void(CMidiChCtrlDlg::*pset)(int)) { isValid = Attach(par, id, pget, pset); };
		BOOL Attach(CMidiChCtrlDlg* par, UINT id, int(CMidiChCtrlDlg::*pget)(), void(CMidiChCtrlDlg::*pset)(int));
		BOOL IsMember(int id) { return (isValid && id == Btn.GetDlgCtrlID()); };
		void OnClicked() { (pParent->*pSetter)(Checked()); };
		BOOL Checked() { return isValid && Btn.GetCheck(); };
		void UpdateState() { isValid ? Btn.SetCheck((pParent->*pGetter)()!=0) : void(); };
	};
	class CMultiCtrl {
	protected:
		int value;
		BOOL isValid;
		CMidiChCtrlDlg* pParent;
		int (CMidiChCtrlDlg::*pGetter)();
		void (CMidiChCtrlDlg::*pSetter)(int val);
	public:
		CEdit Edit;
		CSpinButtonCtrl Spin;
		CSliderCtrl Slider;
		BOOL IsMember(int id);
		void SetValue();
		int GetValue() { return value; };
		void SetRange(int min, int max);
		BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
		CMultiCtrl();
		CMultiCtrl(CMidiChCtrlDlg* par, UINT idedt, UINT idspn, UINT idsld, int(CMidiChCtrlDlg::*pget)(), void(CMidiChCtrlDlg::*pset)(int), int min, int max);
		BOOL Attach(CMidiChCtrlDlg* par, UINT idedt, UINT idspn, UINT idsld, int(CMidiChCtrlDlg::*pget)(), void(CMidiChCtrlDlg::*pset)(int));
	};
	CMultiCtrl* pMulCtrl[numMultiCtrl];
	CChecker* pChecker[numChecker];
	afx_msg void OnBnClickedBtnVedit();
	afx_msg void OnBnClickedButtonPick();
	afx_msg void OnBnClickedCheckLegato();
	CSpinButtonCtrl spnPoly;
	CSpinButtonCtrl spnPhyCh;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnChangeEdit(UINT id);
	afx_msg void OnDeltaposSpin(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheck(UINT id);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnReset();
};
