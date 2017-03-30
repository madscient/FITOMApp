// CDlgVoicePicker.cpp : 実装ファイル
//

#include "stdafx.h"
#include "FITOMApp.h"
#include "FITOM.h"
#include "SoundDev.h"
#include "ADPCM.h"
#include "MIDIDEV.h"
#include "MIDI.h"
#include "FITOMApp.h"
#include "VoicePicker.h"
#include "afxdialogex.h"
#include "SCCIWrapper.h"
#include "FITOMCfg.h"

// CdlgCoicePicker ダイアログ

IMPLEMENT_DYNAMIC(CDlgVoicePicker, CDialogEx)

CDlgVoicePicker::CDlgVoicePicker(CWnd* pParent /*=NULL*/)
: CDialogEx(CDlgVoicePicker::IDD, pParent), theConfig(((CFITOMApp*)AfxGetApp())->GetConfig()),
theDevice(-1), theBank(-1), theProg(-1), bInit(FALSE), pICh(0)
{

}

CDlgVoicePicker::~CDlgVoicePicker()
{
	delete pICh;
}

void CDlgVoicePicker::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DEVICE, cmbDevice);
	DDX_Control(pDX, IDC_COMBO_BANK, cmbBank);
	DDX_Control(pDX, IDC_LIST_PROGS, lstProg);
}


BEGIN_MESSAGE_MAP(CDlgVoicePicker, CDialogEx)
	ON_MESSAGE(WM_KICKIDLE, &CDlgVoicePicker::OnKickIdle)
	ON_CBN_SELENDOK(IDC_COMBO_DEVICE, &CDlgVoicePicker::OnSelendokComboDevice)
	ON_CBN_SELENDOK(IDC_COMBO_BANK, &CDlgVoicePicker::OnSelendokComboBank)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PROGS, &CDlgVoicePicker::OnDblclkListProgs)
	ON_NOTIFY(NM_CLICK, IDC_LIST_PROGS, &CDlgVoicePicker::OnClickListProgs)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CDlgVoicePicker::OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_COPY, &CDlgVoicePicker::OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, &CDlgVoicePicker::OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &CDlgVoicePicker::OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_DELETE, &CDlgVoicePicker::OnEditDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, &CDlgVoicePicker::OnUpdateEditDelete)
	ON_COMMAND(ID_EDIT_INSERT, &CDlgVoicePicker::OnEditInsert)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INSERT, &CDlgVoicePicker::OnUpdateEditInsert)
	ON_COMMAND(ID_EDIT_PASTE, &CDlgVoicePicker::OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CDlgVoicePicker::OnUpdateEditPaste)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_PROGS, &CDlgVoicePicker::OnRclickListProgs)
	ON_NOTIFY(NM_RDBLCLK, IDC_LIST_PROGS, &CDlgVoicePicker::OnRdblclkListProgs)
END_MESSAGE_MAP()


// CDlgVoicePicker メッセージ ハンドラー


void CDlgVoicePicker::OnSelendokComboDevice()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	//SetDevice(cmbDevice.GetItemData(cmbDevice.GetCurSel()));
	theDevice = cmbDevice.GetItemData(cmbDevice.GetCurSel());
	SetBank(0);
	RefreshBank();
	RefreshProg();
}


void CDlgVoicePicker::OnSelendokComboBank()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	SetBank(cmbBank.GetItemData(cmbBank.GetCurSel()));
	RefreshProg();
}

void CDlgVoicePicker::SetDevice(UINT32 dev)
{
	theDevice = dev;
	if (bInit) {
		RefreshDevice();
	}
}

void CDlgVoicePicker::SetBank(UINT32 bank)
{
	theBank = bank;
	if (bInit) {
		RefreshBank();
	}
}

void CDlgVoicePicker::SetProg(UINT32 prog)
{
	theProg = prog;
	if (bInit) {
		RefreshProg();
	}
}

BOOL CDlgVoicePicker::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	bInit = TRUE;
	// TODO: ここに初期化を追加してください
	lstProg.SetExtendedStyle(LVS_EX_AUTOSIZECOLUMNS | LVS_EX_LABELTIP | LVS_EX_JUSTIFYCOLUMNS | LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT);
	RefreshDevice();
	RefreshBank();
	RefreshProg();
	pICh = new CInstCh(0, CFITOM::GetInstance());

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

CString CDlgVoicePicker::GetBankName(UINT32 device, UINT32 bank)
{
	return CString(theConfig->GetBankName(device, bank));
}

CString CDlgVoicePicker::GetProgName(UINT32 device, UINT32 bank, UINT32 prog)
{
	CString ret;
	TCHAR tmp[64];
	theConfig->GetVoiceName(device, bank, prog, tmp, _countof(tmp));
	ret = tmp;
	return ret;
}

void CDlgVoicePicker::RefreshDevice()
{
	cmbDevice.ResetContent();
	for (int i = 0; i < theConfig->GetLogDevs(); i++) {
		CString tmp;
		CSoundDevice* pDev = theConfig->GetLogDeviceFromIndex(i);
		if (pDev) {
			UINT32 devid = pDev->GetDevice();
			UINT32 phyid = theConfig->GetDeviceUniqID(pDev);
			TCHAR devname[64];
			theConfig->GetDeviceName(phyid, devname, _countof(devname));
			tmp.Format(_T("%02X:%s"), devid, devname);
			int k = cmbDevice.AddString(tmp);
			cmbDevice.SetItemData(k, phyid);
			if (UINT8(phyid) == UINT8(theDevice)) {
				cmbDevice.SetCurSel(k);
			}
		}
		else {
			int oh_my_god = 1;
		}
	}
	for (int i = 0; i < theConfig->GetPcmDevs(); i++) {
		CString tmp;
		CAdPcmBase* pDev = theConfig->GetPCMDeviceFromIndex(i);
		if (pDev) {
			UINT32 devid = pDev->GetDevice();
			UINT32 phyid = theConfig->GetDeviceUniqID(pDev);
			TCHAR devname[64];
			theConfig->GetDeviceName(phyid, devname, _countof(devname));
			tmp.Format(_T("%02X:%s"), devid, devname);
			int k = cmbDevice.AddString(tmp);
			cmbDevice.SetItemData(k, phyid);
			if (phyid == theDevice) {
				cmbDevice.SetCurSel(k);
			}
		}
		else {
			int oh_my_god = 1;
		}
	}


}

void CDlgVoicePicker::RefreshBank()
{
	cmbBank.ResetContent();
	CString tmp;
	BOOL isPcm = theConfig->isPcmDevice(theDevice);
	UINT vg = CFITOM::GetDeviceVoiceGroupMask(theDevice & 0xff);
	for (int i = 0; i < MAX_BANK; i++) {
		if (isPcm ? (i < theConfig->GetPcmDevs()) : (theConfig->GetFMBank(vg, i) != 0)) {
			CString pBankName = GetBankName(theDevice, i);
			tmp.Format(_T("%03i:%s"), i, pBankName);
			int n = cmbBank.AddString(tmp);
			cmbBank.SetItemData(n, i);
			if (i == theBank) {
				cmbBank.SetCurSel(n);
			}
		}
	}
	if (theBank >= cmbBank.GetCount()) {
		theBank = 0;
		cmbBank.SetCurSel(0);
	}
}

void CDlgVoicePicker::RefreshProg()
{
	lstProg.DeleteAllItems();
	for (int i = 0; i < 128; i++) {
		CString tmp;
		CString vname = GetProgName(theDevice, theBank, i);
		tmp.Format(_T("%03i:%s"), i, vname);
		lstProg.InsertItem(i, tmp);
		lstProg.EnsureVisible(i, TRUE);
		if (i == theProg) {
			lstProg.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
			lstProg.SetSelectionMark(i);
		}
	}
}

void CDlgVoicePicker::OnDblclkListProgs(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	*pResult = 0;
	OnOK();
}


void CDlgVoicePicker::OnOK()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	theProg = lstProg.GetSelectionMark();
	CDialogEx::OnOK();
}


void CDlgVoicePicker::OnClickListProgs(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	*pResult = 0;
}


void CDlgVoicePicker::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	// TODO:ここにコマンド更新 UI ハンドラー コードを追加します。
}


void CDlgVoicePicker::OnEditCopy()
{
	// TODO: ここにコマンド ハンドラー コードを追加します。
}


void CDlgVoicePicker::OnEditCut()
{
	// TODO: ここにコマンド ハンドラー コードを追加します。
}


void CDlgVoicePicker::OnUpdateEditCut(CCmdUI *pCmdUI)
{
	// TODO:ここにコマンド更新 UI ハンドラー コードを追加します。
}


void CDlgVoicePicker::OnEditDelete()
{
	// TODO: ここにコマンド ハンドラー コードを追加します。
}


void CDlgVoicePicker::OnUpdateEditDelete(CCmdUI *pCmdUI)
{
	// TODO:ここにコマンド更新 UI ハンドラー コードを追加します。
}


void CDlgVoicePicker::OnEditInsert()
{
	// TODO: ここにコマンド ハンドラー コードを追加します。
}


void CDlgVoicePicker::OnUpdateEditInsert(CCmdUI *pCmdUI)
{
	// TODO:ここにコマンド更新 UI ハンドラー コードを追加します。
}


void CDlgVoicePicker::OnEditPaste()
{
	// TODO: ここにコマンド ハンドラー コードを追加します。
}


void CDlgVoicePicker::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
	// TODO:ここにコマンド更新 UI ハンドラー コードを追加します。
}


void CDlgVoicePicker::OnRclickListProgs(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	*pResult = 0;
}


void CDlgVoicePicker::OnRdblclkListProgs(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	theProg = lstProg.GetSelectionMark();
	BeginWaitCursor();
	UINT8 devm = theDevice & 0xff;
	BOOL isPcm = theConfig->isPcmDevice(theDevice);
	pICh->BankSelMSB(devm);
	pICh->BankSelLSB(theBank);
	pICh->ProgChange(theProg);
	pICh->SetVolume(100);
	pICh->SetExpress(127);
	pICh->SetPanpot(64);
	pICh->NoteOn(64, 127);
	::Sleep(1000);
	pICh->NoteOff(64);
	EndWaitCursor();
	*pResult = 0;
}

LRESULT CDlgVoicePicker::OnKickIdle(WPARAM wParam, LPARAM lParam)
{
	UpdateDialogControls(this, FALSE);

	return FALSE;
}
