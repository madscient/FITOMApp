#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CMidiMonitor �_�C�A���O

class CFITOMConfig;

class CMidiMonitor : public CDialogEx
{
	DECLARE_DYNAMIC(CMidiMonitor)

public:
	CMidiMonitor(CFITOM* pFitom, CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CMidiMonitor();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_MIDI_MONITOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void PostNcDestroy();
	virtual void OnCancel();
	struct chstat {
		uint32_t dev;
		int note;
		int pan;
		int vol;
		int bank;
		int prog;
	public:
		chstat() : dev(-1), note(-1), pan(-1), vol(-1), bank(-1), prog(-1) {};
	};
	chstat prevstat[16];
	UINT_PTR hTimer;
	void GetCurrentStat(int ifno, int ch, chstat* stat);
	void UpdateStat(int ch, chstat* stat);
	int CompareStat(chstat& stat1, chstat& stat2);
	CFITOM* theFitom;
	CFITOMConfig* theConfig;
public:
	virtual bool OnInitDialog();
	CComboBox cmbMidiIn;
	CListCtrl lstMidiMon;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDblclkListMidich(NMHDR *pNMHDR, LRESULT *pResult);
	virtual bool PreTranslateMessage(MSG* pMsg);
};
