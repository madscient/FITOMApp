#pragma once
#include "afxcmn.h"


// CVoiceBankDlg dialog

class CVoiceBankDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CVoiceBankDlg)

public:
	CVoiceBankDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVoiceBankDlg();

// Dialog Data
	enum { IDD = IDD_BANK_SETTING };

protected:
	BOOL bModified;
	CStringArray bankfiles[7];
	CStringArray banknames[7];
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void Refresh();
	void OnPick(int index);
	void OnDelete(int index);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnClickedBtnPick1() { OnPick(0); };
	afx_msg void OnClickedBtnPick2() { OnPick(1); };
	afx_msg void OnClickedBtnPick3() { OnPick(2); };
	afx_msg void OnClickedBtnPick4() { OnPick(3); };
	afx_msg void OnClickedBtnPick5() { OnPick(4); };
	afx_msg void OnClickedBtnPick6() { OnPick(5); };
	afx_msg void OnClickedBtnPick7() { OnPick(6); };
	afx_msg void OnClickedBtnPick8() { OnPick(7); };
	CTabCtrl tabGroup;
	afx_msg void OnTcnSelchangeTabVoiceGroup(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickedBtnDel1() { OnDelete(0); };
	afx_msg void OnClickedBtnDel2() { OnDelete(1); };
	afx_msg void OnClickedBtnDel3() { OnDelete(2); };
	afx_msg void OnClickedBtnDel4() { OnDelete(3); };
	afx_msg void OnClickedBtnDel5() { OnDelete(4); };
	afx_msg void OnClickedBtnDel6() { OnDelete(5); };
	afx_msg void OnClickedBtnDel7() { OnDelete(6); };
	afx_msg void OnClickedBtnDel8() { OnDelete(7); };
};
