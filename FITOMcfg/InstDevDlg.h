#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CInstDevDlg ダイアログ

class CInstDevDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CInstDevDlg)

public:
	CInstDevDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CInstDevDlg();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INST_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl lstDevice;
	CComboBox cmbModule;
	CComboBox cmbMode;
	CComboBox cmbType;
};
