#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CDevMonitor �_�C�A���O

class CFITOMConfig;

class CDevMonitor : public CDialogEx
{
	DECLARE_DYNAMIC(CDevMonitor)

public:
	CDevMonitor(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CDevMonitor();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_DEV_MONITOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	CFITOMConfig* theConfig;
	UINT_PTR hTimer;
	void UpdateList(bool binit=FALSE);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSelendokComboDevice();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void PostNcDestroy();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	CComboBox cmbDevice;
	CListCtrl lstDevice;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonReset();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
