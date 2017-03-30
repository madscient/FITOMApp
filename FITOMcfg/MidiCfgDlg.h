#pragma once
#include "afxwin.h"

#define MAX_MIDI	4

// CMidiCfgDlg dialog

class CMidiCfgDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMidiCfgDlg)

public:
	CMidiCfgDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMidiCfgDlg();

// Dialog Data
	enum { IDD = IDD_MIDI_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	CComboBox cmbMidi[MAX_MIDI];
	int selClock;
};
