// AdPcmDevDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "FITOMcfg.h"
#include "AdPcmDevDlg.h"
#include "afxdialogex.h"
#include <strsafe.h>

// CAdPcmDevDlg ダイアログ

IMPLEMENT_DYNAMIC(CAdPcmDevDlg, CDialogEx)

CAdPcmDevDlg::CAdPcmDevDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAdPcmDevDlg::IDD, pParent)
{

}

CAdPcmDevDlg::~CAdPcmDevDlg()
{
}

void CAdPcmDevDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ADPCM, lstAdpcmDevice);
}


BEGIN_MESSAGE_MAP(CAdPcmDevDlg, CDialogEx)
	ON_MESSAGE(WM_KICKIDLE, &CAdPcmDevDlg::OnKickIdle)
	ON_COMMAND(IDC_BTN_UP, &CAdPcmDevDlg::OnBtnUp)
	ON_COMMAND(IDC_BTN_DOWN, &CAdPcmDevDlg::OnBtnDown)
	ON_COMMAND(IDC_BTN_DEL, &CAdPcmDevDlg::OnBtnDel)
	ON_UPDATE_COMMAND_UI(IDC_BTN_UP, &CAdPcmDevDlg::OnUpdateBtn)
	ON_UPDATE_COMMAND_UI(IDC_BTN_DOWN, &CAdPcmDevDlg::OnUpdateBtn)
	ON_UPDATE_COMMAND_UI(IDC_BTN_DEL, &CAdPcmDevDlg::OnUpdateBtn)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ADPCM, &CAdPcmDevDlg::OnDblclkListAdpcm)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CAdPcmDevDlg メッセージ ハンドラー

LRESULT CAdPcmDevDlg::OnKickIdle(WPARAM wParam, LPARAM lParam)
{
	UpdateDialogControls(this, FALSE);

	return FALSE;
}

void CAdPcmDevDlg::OnOK()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	int count = 0;
	TCHAR  curdir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, curdir);
	CString profile;
	profile.Format(_T("%s\\FITOM.ini"), curdir);

	CString str;
	::WritePrivateProfileSection(_T("ADPCM"), NULL, profile);
	for (int i = 0; i < devuse.size(); i++) {
		if (lstrlen(devuse[i].bnkfile)) {
			CString keyname;
			keyname.Format(_T("device%i"), i);
			LPCTSTR devkey = _T("ADPCM");
			if (devuse[i].type == SC_TYPE_YM2610 || devuse[i].type == SC_TYPE_YM2610B || devuse[i].type == SC_TYPE_YMF286) {
				devkey = devuse[i].subtype ? _T("ADPCM_B") : _T("ADPCM_A");
			}
			str.Format(_T("%s:%s,SCCI:%i:%i,%s"), devkey, ((CFITOMcfgApp*)AfxGetApp())->GetChipName(devuse[i].type), devuse[i].ifid, devuse[i].slid, devuse[i].bnkfile);
			::WritePrivateProfileString(_T("ADPCM"), keyname, str, profile);
			count++;
		}
	}
	str.Format(_T("%i"), count);
	::WritePrivateProfileString(_T("ADPCM"), _T("count"), str, profile);
	CDialogEx::OnOK();
}


BOOL CAdPcmDevDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO: ここに初期化を追加してください
	if (((CFITOMcfgApp*)AfxGetApp())->InitSCCI() == 0) {
		sis.clear();
		devuse.clear();
		SoundInterfaceManager* pManager = ((CFITOMcfgApp*)AfxGetApp())->GetManager();
		int ifcount = pManager->getInterfaceCount();
		for (int i = 0; i < ifcount; i++) {
			scciInterface* psi = new scciInterface();
			psi->psi = pManager->getInterface(i);
			psi->psii = pManager->getInterfaceInfo(i);
			psi->sdps.clear();
			if (psi->psi && psi->psii) {
				for (int j = 0; j < psi->psi->getSoundChipCount(); j++) {
					scciDeviceProperty* sdp = new scciDeviceProperty();
					sdp->psc = psi->psi->getSoundChip(j);
					sdp->pssci = sdp->psc->getSoundChipInfo();
					if (sdp->pssci->iSoundChip == SC_TYPE_Y8950 ||
						sdp->pssci->iSoundChip == SC_TYPE_YM2608 ||
						sdp->pssci->iSoundChip == SC_TYPE_YM2610 ||
						sdp->pssci->iSoundChip == SC_TYPE_YM2610B ||
						sdp->pssci->iSoundChip == SC_TYPE_YMF286) {
						sdp->ifid = i;
						sdp->slid = j;
						psi->sdps.push_back(sdp);
						adpcmDevCfg adc;
						adc.type = (SC_CHIP_TYPE)sdp->pssci->iSoundChip;
						adc.subtype = 0;
						adc.ifid = i;
						adc.slid = j;
						ZeroMemory(adc.bnkfile, sizeof(adc.bnkfile));
						devuse.push_back(adc);
						if (sdp->pssci->iSoundChip == SC_TYPE_YM2610 ||
							sdp->pssci->iSoundChip == SC_TYPE_YM2610B ||
							sdp->pssci->iSoundChip == SC_TYPE_YMF286) {
							adc.subtype = 1;
							devuse.push_back(adc);
						}
					}
				}
				sis.push_back(psi);
			}
		}
		TCHAR  curdir[MAX_PATH];
		::GetCurrentDirectory(MAX_PATH, curdir);
		CString profile;
		profile.Format(_T("%s\\FITOM.ini"), curdir);
		int devcount = ::GetPrivateProfileInt(_T("ADPCM"), _T("count"), 0, LPCTSTR(profile));
		for (int i = 0; i < devcount; i++) {
			CString keyname;
			keyname.Format(_T("device%i"), i);
			TCHAR devstr[256];
			::GetPrivateProfileString(_T("ADPCM"), LPCTSTR(keyname), _T(""), devstr, 256, LPCTSTR(profile));
			TCHAR devstr2[32], adpcmtype[16], devname[16], ifstr[16], bnkfile[MAX_PATH];
			sscanf_s(devstr, _T("%[^,], %[^,], %[^,]"), devstr2, _countof(devstr2), ifstr, _countof(ifstr), bnkfile, _countof(bnkfile));
			sscanf_s(devstr2, _T("%[^:]:%s"), adpcmtype, _countof(adpcmtype), devname, _countof(devname));
			TCHAR ifname[16];
			int mbno, slno;
			sscanf_s(ifstr, _T("%[^:]:%i:%i"), ifname, _countof(ifname), &mbno, &slno);
			SC_CHIP_TYPE ct = ((CFITOMcfgApp*)AfxGetApp())->GetChipTypeFromName(devname);
			for (int j = 0; j < devuse.size(); j++) {
				if (devuse[j].ifid == mbno && devuse[j].slid == slno && devuse[j].type == ct) {
					StringCchCopy(devuse[j].bnkfile, _countof(devuse[j].bnkfile), bnkfile);
				}
			}
		}
	}
	lstAdpcmDevice.DeleteAllItems();
	lstAdpcmDevice.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	lstAdpcmDevice.InsertColumn(0, _T("No."), LVCFMT_LEFT, 16);
	lstAdpcmDevice.InsertColumn(1, _T("Chip"), LVCFMT_LEFT, 48);
	lstAdpcmDevice.InsertColumn(2, _T("I/F"), LVCFMT_LEFT, 64);
	lstAdpcmDevice.InsertColumn(3, _T("Bank File"), LVCFMT_LEFT, 256);
	Refresh();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void CAdPcmDevDlg::Refresh()
{
	int rows = 0;
	lstAdpcmDevice.DeleteAllItems();
	for (int i = 0; i < devuse.size(); i++) {
		CString str;
		str.Format(_T("%i"), rows);
		lstAdpcmDevice.InsertItem(rows, str);
		str = ((CFITOMcfgApp*)AfxGetApp())->GetChipName(devuse[i].type);
		if (devuse[i].type == SC_TYPE_YM2610 || devuse[i].type == SC_TYPE_YM2610B || devuse[i].type == SC_TYPE_YMF286) {
			str.Format(_T("%s(%s)"), str, devuse[i].subtype ? _T("B") : _T("A"));
		}
		lstAdpcmDevice.SetItemText(rows, 1, str);
		str.Format(_T("SCCI:%X:%X"), devuse[i].ifid, devuse[i].slid);
		lstAdpcmDevice.SetItemText(rows, 2, str);
		lstAdpcmDevice.SetItemText(rows, 3, devuse[i].bnkfile);
		rows++;
	}
}

void CAdPcmDevDlg::OnBtnUp()
{
	// TODO: ここにコマンド ハンドラー コードを追加します。
	int selidx = lstAdpcmDevice.GetSelectionMark();
	if (selidx > 0) {
		std::swap(devuse[selidx-1], devuse[selidx]);
		Refresh();
	}
}


void CAdPcmDevDlg::OnBtnDown()
{
	// TODO: ここにコマンド ハンドラー コードを追加します。
	int selidx = lstAdpcmDevice.GetSelectionMark();
	if (selidx < devuse.size()) {
		std::swap(devuse[selidx], devuse[selidx+1]);
		Refresh();
	}
}


void CAdPcmDevDlg::OnBtnDel()
{
	// TODO: ここにコマンド ハンドラー コードを追加します。
	int selidx = lstAdpcmDevice.GetSelectionMark();
	if (selidx >= 0) {
		devuse[selidx].bnkfile[0] = _T('\0');
		Refresh();
	}
}

void CAdPcmDevDlg::OnUpdateBtn(CCmdUI *pCmdUI)
{
	// TODO:ここにコマンド更新 UI ハンドラー コードを追加します。
	pCmdUI->Enable(lstAdpcmDevice.GetSelectedCount());
}


void CAdPcmDevDlg::OnDblclkListAdpcm(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	*pResult = 0;
	int selidx = lstAdpcmDevice.GetSelectionMark();
	if (selidx >= 0) {
		TCHAR initdir[MAX_PATH];
		if (lstrlen(devuse[selidx].bnkfile) == 0) {
			::GetCurrentDirectory(_countof(initdir), initdir);
		}
		else {
			::GetFullPathName(devuse[selidx].bnkfile, _countof(initdir), initdir, 0);
		}
		CFileDialog dlg(TRUE, _T(".bnk"), initdir, OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_ENABLESIZING | OFN_FILEMUSTEXIST,
			_T("Bank file(*.bnk)|*.bnk|All files(*.*)|*.*||"), this);
		if (dlg.DoModal() == IDOK) {
			StringCchCopy(devuse[selidx].bnkfile, _countof(devuse[selidx].bnkfile), dlg.GetPathName());
			Refresh();
		}
	}
}


void CAdPcmDevDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	for (int i = 0; i < sis.size(); i++) {
		for (int j = 0; j < sis[i]->sdps.size(); j++) {
			delete sis[i]->sdps[j];
		}
		sis[i]->sdps.clear();
		delete sis[i];
	}
	sis.clear();
	((CFITOMcfgApp*)AfxGetApp())->ExitSCCI();
	// TODO: ここにメッセージ ハンドラー コードを追加します。
}
