#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include <vector>

// CPcmBankDlg ダイアログ

class CPcmBankDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPcmBankDlg)

public:
	CPcmBankDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CPcmBankDlg();

// ダイアログ データ
	enum { IDD = IDD_PCM_BANK_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	void Refresh(int lastselection = -1);
	void Update();
	CString filename;
	CString bankname;
	struct WAVFILES {
		TCHAR filename[MAX_PATH];
		int rate;
	};
	std::vector<WAVFILES> wavfiles;
	BOOL bModified;
	DECLARE_MESSAGE_MAP()
public:
	LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam);
	CListCtrl lstPcmBank;
	CEdit edtBankName;
	CEdit edtFileName;
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBtnTest();
	afx_msg void OnBtnAdd();
	afx_msg void OnBtnUp();
	afx_msg void OnBtnDown();
	afx_msg void OnBtnDel();
	afx_msg void OnBtnOpen();
	afx_msg void OnBtnHiLo();
	afx_msg void OnUpdateBtnTest(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBtnUp(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBtnDown(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBtnDel(CCmdUI *pCmdUI);
	virtual void OnCancel();
};
