#pragma once


// CSysCtlDlg ダイアログ

class CSysCtlDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSysCtlDlg)

public:
	CSysCtlDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CSysCtlDlg();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYSCTL_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnDevinit();
	afx_msg void OnBnClickedBtnRestart();
	afx_msg void OnBnClickedBtnReload();
};
