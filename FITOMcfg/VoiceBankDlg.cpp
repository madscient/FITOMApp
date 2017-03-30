// VoiceBankDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FITOMcfg.h"
#include "VoiceBankDlg.h"
#include "afxdialogex.h"
#include <strsafe.h>

// CVoiceBankDlg dialog

IMPLEMENT_DYNAMIC(CVoiceBankDlg, CDialogEx)

CVoiceBankDlg::CVoiceBankDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CVoiceBankDlg::IDD, pParent), bModified(FALSE)
{

}

CVoiceBankDlg::~CVoiceBankDlg()
{
}

void CVoiceBankDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_VOICE_GROUP, tabGroup);
}


BEGIN_MESSAGE_MAP(CVoiceBankDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_PICK1, &CVoiceBankDlg::OnClickedBtnPick1)
	ON_BN_CLICKED(IDC_BTN_PICK2, &CVoiceBankDlg::OnClickedBtnPick2)
	ON_BN_CLICKED(IDC_BTN_PICK3, &CVoiceBankDlg::OnClickedBtnPick3)
	ON_BN_CLICKED(IDC_BTN_PICK4, &CVoiceBankDlg::OnClickedBtnPick4)
	ON_BN_CLICKED(IDC_BTN_PICK5, &CVoiceBankDlg::OnClickedBtnPick5)
	ON_BN_CLICKED(IDC_BTN_PICK6, &CVoiceBankDlg::OnClickedBtnPick6)
	ON_BN_CLICKED(IDC_BTN_PICK7, &CVoiceBankDlg::OnClickedBtnPick7)
	ON_BN_CLICKED(IDC_BTN_PICK8, &CVoiceBankDlg::OnClickedBtnPick8)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_VOICE_GROUP, &CVoiceBankDlg::OnTcnSelchangeTabVoiceGroup)
	ON_BN_CLICKED(IDC_BTN_DEL1, &CVoiceBankDlg::OnClickedBtnDel1)
	ON_BN_CLICKED(IDC_BTN_DEL2, &CVoiceBankDlg::OnClickedBtnDel2)
	ON_BN_CLICKED(IDC_BTN_DEL3, &CVoiceBankDlg::OnClickedBtnDel3)
	ON_BN_CLICKED(IDC_BTN_DEL4, &CVoiceBankDlg::OnClickedBtnDel4)
	ON_BN_CLICKED(IDC_BTN_DEL5, &CVoiceBankDlg::OnClickedBtnDel5)
	ON_BN_CLICKED(IDC_BTN_DEL6, &CVoiceBankDlg::OnClickedBtnDel6)
	ON_BN_CLICKED(IDC_BTN_DEL7, &CVoiceBankDlg::OnClickedBtnDel7)
	ON_BN_CLICKED(IDC_BTN_DEL8, &CVoiceBankDlg::OnClickedBtnDel8)
END_MESSAGE_MAP()

#define VOICE_GROUP_NONE 0x00
#define VOICE_GROUP_OPM  0x01
#define VOICE_GROUP_OPNA 0x02
#define VOICE_GROUP_OPL2 0x04
#define VOICE_GROUP_OPL3 0x08
#define VOICE_GROUP_OPLL 0x10
#define VOICE_GROUP_PSG  0x20
#define VOICE_GROUP_RHYTHM 0x8000

#define MAX_BANK	8

struct EDITITEMS {
	UINT nameid;
	UINT fileid;
};
EDITITEMS edititems[] = {
	{ IDC_EDIT_BANK_NAME1, IDC_EDIT_FILE_NAME1, },
	{ IDC_EDIT_BANK_NAME2, IDC_EDIT_FILE_NAME2, },
	{ IDC_EDIT_BANK_NAME3, IDC_EDIT_FILE_NAME3, },
	{ IDC_EDIT_BANK_NAME4, IDC_EDIT_FILE_NAME4, },
	{ IDC_EDIT_BANK_NAME5, IDC_EDIT_FILE_NAME5, },
	{ IDC_EDIT_BANK_NAME6, IDC_EDIT_FILE_NAME6, },
	{ IDC_EDIT_BANK_NAME7, IDC_EDIT_FILE_NAME7, },
	{ IDC_EDIT_BANK_NAME8, IDC_EDIT_FILE_NAME8, },
};

struct TABSETTING {
	TCHAR* tabname;
	DWORD voicegroup;
};
TABSETTING tabset[] = {
	{ _T("OPM"), VOICE_GROUP_OPM, },
	{ _T("OPNA"), VOICE_GROUP_OPNA, },
	{ _T("OPL2"), VOICE_GROUP_OPL2, },
	{ _T("OPL3"), VOICE_GROUP_OPL3, },
	{ _T("OPLL"), VOICE_GROUP_OPLL, },
	{ _T("PSG"), VOICE_GROUP_PSG, },
	{ _T("DRUM"), VOICE_GROUP_RHYTHM, },
	{ 0, VOICE_GROUP_NONE, },
};

// CVoiceBankDlg message handlers

BOOL CVoiceBankDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	tabGroup.SetItemExtra(4);
	tabGroup.DeleteAllItems();
	for (int i = 0; tabset[i].tabname; i++) {
		TCITEM tcitem;
		tcitem.pszText = tabset[i].tabname;
		tcitem.lParam = tabset[i].voicegroup;
		tcitem.mask = TCIF_TEXT | TCIF_PARAM;
		tabGroup.InsertItem(i, &tcitem);
	}
	tabGroup.SetCurSel(0);
	tabGroup.SetCurFocus(0);

	for (int i = 0; tabset[i].tabname != 0; i++) {
		TCHAR* keybase = _T("bank");
		if (tabset[i].voicegroup == VOICE_GROUP_RHYTHM) {
			keybase = _T("prog");
		}
		bankfiles[i].RemoveAll();
		banknames[i].RemoveAll();
		for (int j = 0; j < MAX_BANK; j++) {
			TCHAR filename[MAX_PATH];
			CString keyname;
			keyname.Format(_T("%s%i"), keybase, j);
			::GetPrivateProfileString(tabset[i].tabname, keyname, _T(""), filename, _countof(filename), _T(".\\FITOM.ini"));
			if (::PathFileExists(filename)) {
				bankfiles[i].Add(CString(filename));
				TCHAR bankname[80] = { _T('\0') };
				::GetPrivateProfileString(_T("Header"), _T("BankName"), _T(""), bankname, _countof(bankname), filename);
				banknames[i].Add(CString(bankname));
			}
			else {
				bankfiles[i].Add(CString(_T("")));
				banknames[i].Add(CString(_T("")));
			}
		}
	}

	Refresh();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CVoiceBankDlg::Refresh()
{
	int tabindex = tabGroup.GetCurSel();
	for (int i = 0; i < MAX_BANK; i++) {
		SetDlgItemText(edititems[i].nameid, banknames[tabindex].ElementAt(i));
		SetDlgItemText(edititems[i].fileid, bankfiles[tabindex].ElementAt(i));
	}
}

void CVoiceBankDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	for (int i = 0; tabset[i].tabname != 0; i++) {
		TCHAR* keybase = _T("bank");
		if (tabset[i].voicegroup == VOICE_GROUP_RHYTHM) {
			keybase = _T("prog");
		}
		::WritePrivateProfileString(tabset[i].tabname, 0, 0, _T(".\\FITOM.ini"));
		for (int j = 0; j < MAX_BANK; j++) {
			CString keyname;
			keyname.Format(_T("%s%i"), keybase, j);
			::WritePrivateProfileString(tabset[i].tabname, keyname, bankfiles[i].ElementAt(j), _T(".\\FITOM.ini"));
		}
	}
	CDialogEx::OnOK();
}

void CVoiceBankDlg::OnTcnSelchangeTabVoiceGroup(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	Refresh();
}

void CVoiceBankDlg::OnPick(int index)
{
	int tabindex = tabGroup.GetCurSel();
	TCHAR initdir[MAX_PATH];
	::GetFullPathName(bankfiles[tabindex].ElementAt(index), _countof(initdir), initdir, 0);
	CFileDialog dlg(TRUE, _T(".bnk"), initdir, OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_ENABLESIZING | OFN_FILEMUSTEXIST,
		_T("Bank file(*.bnk)|*.bnk|All files(*.*)|*.*||"), this);
	if (dlg.DoModal() == IDOK) {
		bModified = TRUE;
		bankfiles[tabindex].ElementAt(index) = dlg.GetPathName();
		//SetDlgItemText(edititems[index].fileid, bankfiles[tabindex].ElementAt(index));
		TCHAR bankname[80] = { _T('\0') };
		::GetPrivateProfileString(_T("Header"), _T("BankName"), _T(""), bankname, _countof(bankname), bankfiles[tabindex].ElementAt(index));
		banknames[tabindex].ElementAt(index) = bankname;
		//SetDlgItemText(edititems[index].nameid, bankname);
		Refresh();
	}
}

void CVoiceBankDlg::OnDelete(int index)
{
	int tabindex = tabGroup.GetCurSel();
	bankfiles[tabindex].ElementAt(index) = _T("");
	banknames[tabindex].ElementAt(index) = _T("");
	SetDlgItemText(edititems[index].nameid, _T(""));
	SetDlgItemText(edititems[index].fileid, _T(""));
}