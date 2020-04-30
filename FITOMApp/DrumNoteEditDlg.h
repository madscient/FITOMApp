#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "FITOMCfg.h"

// CDrumNoteEditDlg �_�C�A���O

class CDrumNoteEditDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDrumNoteEditDlg)

public:
	CDrumNoteEditDlg(CRhythmCh* pCH, CWnd* pParent=NULL);   // �W���R���X�g���N�^�[
	virtual ~CDrumNoteEditDlg();
	void SetProg(int prog) { theProg = prog; };
	int GetProg() { return theProg; };
	void SetNote(int note) { theNote = note; };
	int GetNote() { return theNote; };
	void GetDrumNote(DRUMMAP* drumnote) { *drumnote = theDrum; };

// �_�C�A���O �f�[�^
	enum { IDD = IDD_DLG_DNOTE };

protected:
	CFITOMConfig* theConfig;
	CRhythmCh* theCH;
	DRUMMAP theDrum;
	int theProg;
	int theNote;
	bool bModified;
	bool bInitialized;
	bool bUpdating;
	void Refresh();
	void Update();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	CEdit edtMapName;
	CEdit edtMapNote;
	CEdit edtNoteName;
	CComboBox cmbDevice;
	CEdit edtNoteNum;
	CSpinButtonCtrl spnNoteNum;
	CListCtrl lstOption;
	afx_msg void OnBtnTest();
	afx_msg void OnUpdateBtnTest(CCmdUI *pCmdUI);
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam);
	CSpinButtonCtrl spnPanpot;
	CSpinButtonCtrl spnGate;
	CEdit edtPanpot;
	CEdit edtGate;
	CEdit edtVoice;
	CButton btnVoiceBrowse;
	afx_msg void OnBtnPaste();
	afx_msg void OnUpdateBtnPaste(CCmdUI *pCmdUI);
	afx_msg void OnBtnCopy();
	afx_msg void OnModified();
	afx_msg void OnBtnPick();
	afx_msg void OnUpdateBtnPick(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCmbType(CCmdUI *pCmdUI);
	CSpinButtonCtrl spnTune;
	CEdit edtTune;
};
