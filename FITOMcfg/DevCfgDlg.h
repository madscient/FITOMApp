#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "scci.h"
#include "SCCIDefines.h"
#include <vector>
#include <string>

// CDevCfgDlg dialog

class CDevCfgDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDevCfgDlg)

public:
	CDevCfgDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDevCfgDlg();

// Dialog Data
	enum { IDD = IDD_DEVICE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	struct scciDeviceProperty {
		DWORD ifid;
		DWORD slid;
		SCCI_SOUND_CHIP_INFO* pssci;
		SoundChip* psc;
	};
	struct scciInterface {
		SoundInterface* psi;
		SCCI_INTERFACE_INFO* psii;
		std::vector<scciDeviceProperty*> sdps;
	};
	std::vector<scciInterface*> sis;
	std::vector<CString> devuse;
	CString BuildConfigString(scciDeviceProperty* sdp);
	void RefreshUseList();
	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_treeSCCI;
	CListBox m_lstDevice;
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDel();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonAddAll();
	afx_msg void OnBnClickedButtonDelAll();
};
