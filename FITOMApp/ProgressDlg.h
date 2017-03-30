#pragma once
#include "afxcmn.h"


// CProgressDlg ダイアログ

class CProgressDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProgressDlg)

public:
	CProgressDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CProgressDlg();

// ダイアログ データ
	enum { IDD = IDD_PROGRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
	virtual void OnOK();
	virtual void OnCancel();
public:
	void SetRange(int lower, int upper);
	int SetPos(int pos);
	void SetStatus(LPCTSTR str);
	void SetTarget(LPCTSTR str);
	CProgressCtrl progBar;
};
