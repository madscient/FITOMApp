#pragma once


// CDevModeDlg ダイアログ

class CDevModeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDevModeDlg)

public:
	CDevModeDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CDevModeDlg();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DEVMODE_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
};
