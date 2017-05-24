#pragma once
#include "afxwin.h"
#include "SoundDev.h"
#include "afxcmn.h"

// CTextVoiceEditDlg ダイアログ

class CInstCh;
class CTextVoiceEditDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTextVoiceEditDlg)

public:
	CTextVoiceEditDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CTextVoiceEditDlg();

// ダイアログ データ
	enum { IDD = IDD_DLG_TVED };

protected:
	CFITOMConfig* theConfig;
	FMVOICE theVoice;
	UINT32 theDevice;
	UINT32 theType;
	int typeidx;
	CInstCh* pICh;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	void OnTestButtonDown();
	void OnTestButtonUp();
	void Refresh();
	void Update();
	DECLARE_MESSAGE_MAP()
public:
	CTestButton btnTest;
	CComboBox cmbType;
	CEdit edtName;
	CStatic capALFB;
	CEdit edtALFB;
	CStatic capOP;
	CEdit edtOP;
	CEdit edtLFO;
	void SetVoice(FMVOICE* voice);
	void SetDevice(UINT32 dev);
	void GetVoice(FMVOICE* voice);
	UINT32 GetType();
	virtual BOOL OnInitDialog();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelendokCmbType();
	CSpinButtonCtrl spnNote;
	CStatic capAdd;
	CEdit edtAdd;
};
