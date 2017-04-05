#pragma once
#include "afxwin.h"


// CLineInputDlg ダイアログ

class CLineInputDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLineInputDlg)

public:
	CLineInputDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CLineInputDlg();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MVOL_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	CComboBox cmbLineIn;
	virtual BOOL OnInitDialog();
	virtual void OnOK();
//	afx_msg void OnNcDestroy();
//	virtual void PostNcDestroy();
	afx_msg void OnClose();
};
