#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDrumEditDlg ダイアログ

class CFITOMConfig;

class CDrumEditDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDrumEditDlg)

public:
	CDrumEditDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CDrumEditDlg();

// ダイアログ データ
	enum { IDD = IDD_DLG_DEDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	CFITOMConfig* theConfig;
	CRhythmCh* pCH;
	int theProg;
	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	void Refresh();
	bool bModified;
public:
	void SetProg(int prog) { theProg = prog; };
	int GetProg() { return theProg; };
	virtual bool OnInitDialog();
	CListCtrl lstDrumNote;
	LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedBtnPick();
	CEdit edtDrumMapName;
	afx_msg void OnBtnTest();
	afx_msg void OnBtnCopy();
	afx_msg void OnBtnPaste();
	afx_msg void OnUpdateBtnSelection(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBtnPaste(CCmdUI *pCmdUI);
	afx_msg void OnBtnSave();
	afx_msg void OnUpdateBtnSave(CCmdUI *pCmdUI);
	afx_msg void OnNMDblclkListDrumnote(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void OnCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
