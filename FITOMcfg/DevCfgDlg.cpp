// DevCfgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FITOMcfg.h"
#include "DevCfgDlg.h"
#include "DevModeDlg.h"
#include "afxdialogex.h"
#include <algorithm>

// CDevCfgDlg dialog

IMPLEMENT_DYNAMIC(CDevCfgDlg, CDialogEx)

extern chipSymbols symbols[];

CDevCfgDlg::CDevCfgDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDevCfgDlg::IDD, pParent)
{

}

CDevCfgDlg::~CDevCfgDlg()
{
}

void CDevCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_SCCI, m_treeSCCI);
	DDX_Control(pDX, IDC_LIST_USING, m_lstDevice);
}


BEGIN_MESSAGE_MAP(CDevCfgDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CDevCfgDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &CDevCfgDlg::OnBnClickedButtonDel)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_ADD_ALL, &CDevCfgDlg::OnBnClickedButtonAddAll)
	ON_BN_CLICKED(IDC_BUTTON_DEL_ALL, &CDevCfgDlg::OnBnClickedButtonDelAll)
	ON_LBN_DBLCLK(IDC_LIST_USING, &CDevCfgDlg::OnDblclkListUsing)
END_MESSAGE_MAP()

CString CDevCfgDlg::BuildConfigString(scciDeviceProperty* sdp)
{
	CString ret = _T("");
	if (sdp && sdp->pssci->iSoundChip != SC_TYPE_NONE &&
		sdp->pssci->iSoundChip != SC_TYPE_OTHER &&
		sdp->pssci->iSoundChip != SC_TYPE_UNKNOWN &&
		sdp->pssci->iSoundChip != SC_TYPE_MAX) {
		ret.Format(_T("%s:0, SCCI:%i:%i"), symbols[sdp->pssci->iSoundChip].chipname, sdp->ifid, sdp->slid);
	}
	return ret;
}

// CDevCfgDlg message handlers

void CDevCfgDlg::RefreshUseList()
{
	std::sort(devuse.begin(), devuse.end());
	devuse.erase(std::unique(devuse.begin(), devuse.end()), devuse.end());
	m_lstDevice.ResetContent();
	for (auto it = devuse.begin(); it != devuse.end(); ++it) {
		m_lstDevice.AddString(*it);
	}
}

void CDevCfgDlg::OnBnClickedButtonAdd()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	TVITEM tvi;
	TCHAR pszText[64];
	tvi.mask = TVIF_HANDLE | TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN;
	tvi.hItem = m_treeSCCI.GetSelectedItem();
	tvi.cchTextMax = 64;
	tvi.pszText = pszText;
	if (tvi.hItem) {
		m_treeSCCI.GetItem(&tvi);
		if (tvi.cChildren) {
			scciInterface* psi = (scciInterface*)tvi.lParam;
			for (int i = 0; psi && i < psi->sdps.size(); i++) {
				CString str = BuildConfigString(psi->sdps[i]);
				if (!str.IsEmpty()) {
					devuse.push_back(str);
				}
			}
		}
		else {
			scciDeviceProperty* psdp = (scciDeviceProperty*)tvi.lParam;
			CString str = BuildConfigString(psdp);
			if (!str.IsEmpty()) {
				devuse.push_back(str);
			}
		}
		RefreshUseList();
	}
}


void CDevCfgDlg::OnBnClickedButtonDel()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	int sel = m_lstDevice.GetCurSel();
	if (sel > -1) {
		devuse.erase(devuse.begin() + sel);
	}
	RefreshUseList();
}

BOOL CDevCfgDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO: ここに初期化を追加してください
	if (((CFITOMcfgApp*)AfxGetApp())->InitSCCI() == 0) {
		sis.clear();
		m_treeSCCI.DeleteAllItems();
		SoundInterfaceManager* pManager = ((CFITOMcfgApp*)AfxGetApp())->GetManager();
		int ifcount = pManager->getInterfaceCount();
		for (int i = 0; i < ifcount; i++) {
			scciInterface* psi = new scciInterface();
			psi->psi = pManager->getInterface(i);
			psi->psii = pManager->getInterfaceInfo(i);
			psi->sdps.clear();
			if (psi->psi && psi->psii) {
				HTREEITEM hi = m_treeSCCI.InsertItem(LPCTSTR(psi->psii->cInterfaceName));
				TVITEM tvi;
				tvi.hItem = hi;
				tvi.lParam = DWORD_PTR(psi);
				tvi.state = TVIS_EXPANDED;
				tvi.stateMask = TVIS_EXPANDED;
				tvi.mask = TVIF_PARAM | TVIF_STATE | TVIF_HANDLE;
				m_treeSCCI.SetItem(&tvi);
				for (int j = 0; j < psi->psi->getSoundChipCount(); j++) {
					scciDeviceProperty* sdp = new scciDeviceProperty();
					sdp->psc = psi->psi->getSoundChip(j);
					if (sdp->psc) {
						sdp->pssci = sdp->psc->getSoundChipInfo();
						sdp->ifid = i;
						sdp->slid = j;
						psi->sdps.push_back(sdp);
						CString devname;
						devname.Format(_T("%s(%s)@%iHz"),
							symbols[sdp->pssci->iSoundChip].chipname,
							symbols[sdp->pssci->iSoundChip].chipcode,
							sdp->pssci->dClock
							);
						HTREEITEM hsi = m_treeSCCI.InsertItem(LPCTSTR(devname), hi);
						m_treeSCCI.SetItemData(hsi, DWORD_PTR(sdp));
					}
				}
				sis.push_back(psi);
			}
		}
		TCHAR  curdir[MAX_PATH];
		::GetCurrentDirectory(MAX_PATH, curdir);
		CString profile;
		profile.Format(_T("%s\\FITOM.ini"), curdir);
		int devcount = ::GetPrivateProfileInt(_T("Device"), _T("count"), 0, LPCTSTR(profile));
		for (int i = 0; i < devcount; i++) {
			CString keyname;
			keyname.Format(_T("device%i"), i);
			TCHAR devstr[256];
			::GetPrivateProfileString(_T("Device"), LPCTSTR(keyname), _T(""), devstr, 256, LPCTSTR(profile));
			devuse.push_back(CString(devstr));
		}
		RefreshUseList();
	}
	else {
		// failed to init SCCI
		AfxMessageBox(_T("scci.dllの初期化に失敗しました。"), MB_OK);
		OnCancel();
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


void CDevCfgDlg::OnOK()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	TCHAR  curdir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, curdir);
	CString profile;
	profile.Format(_T("%s\\FITOM.ini"), curdir);
	CString count;
	count.Format(_T("%i"), devuse.size());
	::WritePrivateProfileSection(_T("Device"), NULL, LPCTSTR(profile));
	::WritePrivateProfileString(_T("Device"), _T("count"), LPCTSTR(count), LPCTSTR(profile));
	for (int i = 0; i<devuse.size(); i++) {
		CString keyname;
		keyname.Format(_T("device%i"), i);
		::WritePrivateProfileString(_T("Device"), LPCTSTR(keyname), LPCTSTR(devuse[i]), LPCTSTR(profile));
	}

	CDialogEx::OnOK();
}

void CDevCfgDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: Add your message handler code here
	for (int i = 0; i < sis.size(); i++) {
		for (int j = 0; j < sis[i]->sdps.size(); j++) {
			delete sis[i]->sdps[j];
		}
		sis[i]->sdps.clear();
		delete sis[i];
	}
	sis.clear();
	((CFITOMcfgApp*)AfxGetApp())->ExitSCCI();
}

void CDevCfgDlg::OnBnClickedButtonAddAll()
{
	// TODO: Add your control notification handler code here
	TVITEM tvi;
	TCHAR pszText[64];
	tvi.mask = TVIF_HANDLE | TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN;
	tvi.hItem = m_treeSCCI.GetRootItem();
	tvi.cchTextMax = 64;
	tvi.pszText = pszText;
	while (tvi.hItem) {
		m_treeSCCI.GetItem(&tvi);
		if (tvi.cChildren) {
			scciInterface* psi = (scciInterface*)tvi.lParam;
			for (int i = 0; psi && i < psi->sdps.size(); i++) {
				CString str = BuildConfigString(psi->sdps[i]);
				if (!str.IsEmpty()) {
					devuse.push_back(str);
				}
			}
		}
		tvi.hItem = m_treeSCCI.GetNextItem(tvi.hItem, TVGN_NEXT);
	}
	RefreshUseList();
}

void CDevCfgDlg::OnBnClickedButtonDelAll()
{
	// TODO: Add your control notification handler code here
	devuse.clear();
	RefreshUseList();
}


void CDevCfgDlg::OnDblclkListUsing()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	CDevModeDlg dlg(this);
	if (dlg.DoModal() == IDOK) {

	}
}
