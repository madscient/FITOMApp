#pragma once
#include "afxcmn.h"
#include <vector>

// CAdPcmDevDlg ダイアログ

class CAdPcmDevDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAdPcmDevDlg)

public:
	CAdPcmDevDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CAdPcmDevDlg();

// ダイアログ データ
	enum { IDD = IDD_ADPCM_DEV_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
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
	struct adpcmDevCfg {
		SC_CHIP_TYPE type;
		DWORD subtype;
		DWORD ifid;
		DWORD slid;
		TCHAR bnkfile[MAX_PATH];
	};
	std::vector<adpcmDevCfg> devuse;
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl lstAdpcmDevice;
	LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	void Refresh();
	afx_msg void OnBtnUp();
	afx_msg void OnBtnDown();
	afx_msg void OnBtnDel();
	afx_msg void OnUpdateBtn(CCmdUI *pCmdUI);
	afx_msg void OnDblclkListAdpcm(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDestroy();
};
