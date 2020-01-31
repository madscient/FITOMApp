#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CdlgCoicePicker ダイアログ

class CFITOMConfig;

class CDlgVoicePicker : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgVoicePicker)

public:
	CDlgVoicePicker(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CDlgVoicePicker();

// ダイアログ データ
	enum { IDD = IDD_DLG_VPICK };

protected:
	CFITOMConfig* theConfig;
	CInstCh* pICh;
	bool bInit;
	uint32_t theDevice;
	uint32_t theBank;
	uint32_t theProg;
	void RefreshDevice();
	void RefreshBank();
	void RefreshProg();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	CString GetBankName(uint32_t device, uint32_t bank);
	CString GetProgName(uint32_t device, uint32_t bank, uint32_t prog);
	DECLARE_MESSAGE_MAP()
public:
	void SetDevice(uint32_t dev);
	void SetBank(uint32_t bank);
	void SetProg(uint32_t prog);
	uint32_t GetDevice() { return theDevice; };
	uint32_t GetBank() { return theBank; };
	uint32_t Getprog() { return theProg; };
	CComboBox cmbDevice;
	CComboBox cmbBank;
	CListCtrl lstProg;
	LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSelendokComboDevice();
	afx_msg void OnSelendokComboBank();
	virtual BOOL OnInitDialog();
//	afx_msg void OnOK(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDblclkListProgs(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void OnOK();
	afx_msg void OnClickListProgs(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI *pCmdUI);
	afx_msg void OnEditDelete();
	afx_msg void OnUpdateEditDelete(CCmdUI *pCmdUI);
	afx_msg void OnEditInsert();
	afx_msg void OnUpdateEditInsert(CCmdUI *pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
	afx_msg void OnRclickListProgs(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRdblclkListProgs(NMHDR *pNMHDR, LRESULT *pResult);
};
