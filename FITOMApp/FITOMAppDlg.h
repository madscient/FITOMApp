
// FITOMAppDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once

#include "afxwin.h"
#include "afxcmn.h"

#include "LCDView.h"
#include "StretchPicture.h"

class CMidiMonitor;
class CFITOM;

// CFITOMAppDlg �_�C�A���O
class CFITOMAppDlg : public CDialogEx
{
// �R���X�g���N�V����
public:
	CFITOMAppDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^�[

// �_�C�A���O �f�[�^
	enum { IDD = IDD_FITOMAPP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �T�|�[�g

// ����
protected:
	HICON m_hIcon;
	CFITOM* theFitom;
	CMidiMonitor* dlgMidi;
	CButton btnStart;
	CButton btnMidiMon;
	CButton btnDevMon;
	BOOL bMidMon;
	// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void OnOK();
	virtual void OnCancel() { OnOK(); };
	afx_msg void OnDevMon();
	afx_msg void OnConfig();
	afx_msg void OnMidiMon();
	afx_msg void OnVEdit();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonDedit();
	CSliderCtrl sldMasterVol;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedButtonSysctl();
	CStretchPicture stcLogo;
};
