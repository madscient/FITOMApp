#pragma once
#include "StretchPicture.h"


// CSplashDlg ダイアログ

class CSplashDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSplashDlg)

public:
	CSplashDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CSplashDlg();

// ダイアログ データ
	enum { IDD = IDD_SPLASH };
	void SetStatus(LPCTSTR str);
	void SetFilename(LPCTSTR str);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
	virtual void OnOK();
	virtual void OnCancel();
public:
	CStretchPicture stcLogo;
};
