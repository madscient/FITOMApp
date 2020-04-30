#pragma once
#include "afxwin.h"
#include "SoundDev.h"
#include "afxcmn.h"

// CTextVoiceEditDlg �_�C�A���O

class CInstCh;
class CTextVoiceEditDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTextVoiceEditDlg)

public:
	CTextVoiceEditDlg(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CTextVoiceEditDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_DLG_TVED };

protected:
	CFITOMConfig* theConfig;
	FMVOICE theVoice;
	uint32_t theDevice;
	uint32_t theType;
	int typeidx;
	CInstCh* pICh;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
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
	void SetDevice(uint32_t dev);
	void GetVoice(FMVOICE* voice);
	uint32_t GetType();
	virtual bool OnInitDialog();
	virtual bool OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelendokCmbType();
	CSpinButtonCtrl spnNote;
	CStatic capAdd;
	CEdit edtAdd;
};
