// MidiChCtrlDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "FITOMdefine.h"
#include "SoundDev.h"
#include "ADPCM.h"
#include "MIDIDEV.h"
#include "MIDI.h"
#include "FITOMApp.h"
#include "MidiChCtrlDlg.h"
#include "afxdialogex.h"
#include "VoicePicker.h"
#include "VoiceEditDlg.h"
#include "DrumEditDlg.h"
//#include "SCCIWrapper.h"
#include "FITOMCfg.h"

// CMidiChCtrlDlg ダイアログ

CMidiChCtrlDlg::CMultiCtrl::CMultiCtrl() : value(-1), pGetter(0), pSetter(0)
{

}

CMidiChCtrlDlg::CMultiCtrl::CMultiCtrl
(CMidiChCtrlDlg* par, UINT idedt, UINT idspn, UINT idsld, int(CMidiChCtrlDlg::*pget)(), void(CMidiChCtrlDlg::*pset)(int), int min, int max) :
	pGetter(0), pSetter(0), pParent(0), isValid(0)
{
	isValid = Attach(par, idedt, idspn, idsld, pget, pset);
	SetRange(min, max);
}

BOOL CMidiChCtrlDlg::CMultiCtrl::Attach(CMidiChCtrlDlg* par, UINT idedt, UINT idspn, UINT idsld, int(CMidiChCtrlDlg::*pget)(), void(CMidiChCtrlDlg::*pset)(int))
{
	pParent = par;
	if (!Edit.SubclassDlgItem(idedt, pParent)) { return FALSE; }
	if (!Spin.SubclassDlgItem(idspn, pParent)) { return FALSE; }
	if (!Slider.SubclassDlgItem(idsld, pParent)) { return FALSE; }
	Spin.SetBase(10);
	Spin.SetBuddy(&Edit);
	pGetter = pget;
	pSetter = pset;
	return TRUE;
}

void CMidiChCtrlDlg::CMultiCtrl::SetValue()
{
	if (isValid && pGetter) {
		int val = (pParent->*pGetter)();
		if (value != val) {
			CString tmp;
			tmp.Format(_T("%i"), val);
			value = val;
			Spin.SetPos32(val);
			Slider.SetPos(val);
			Edit.SetWindowText(tmp);
		}
	}
}

void CMidiChCtrlDlg::CMultiCtrl::SetRange(int min, int max)
{
	if (isValid) {
		Spin.SetRange32(min, max);
		Slider.SetRange(min, max, TRUE);
	}
}

BOOL CMidiChCtrlDlg::CMultiCtrl::IsMember(int id)
{
	if (isValid) {
		if (id == Edit.GetDlgCtrlID() || id == Spin.GetDlgCtrlID() || id == Slider.GetDlgCtrlID()) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CMidiChCtrlDlg::CChecker::Attach(CMidiChCtrlDlg* par, UINT id, int(CMidiChCtrlDlg::*pget)(), void(CMidiChCtrlDlg::*pset)(int))
{
	if (!Btn.SubclassDlgItem(id, par)) { return FALSE; }
	pParent = par;
	pGetter = pget;
	pSetter = pset;
	return TRUE;
}

IMPLEMENT_DYNAMIC(CMidiChCtrlDlg, CDialogEx)

struct MultiCtrlInit {
	UINT editid;
	UINT spinid;
	UINT sliderid;
	int(CMidiChCtrlDlg::*pgetter)();
	void(CMidiChCtrlDlg::*psetter)(int);
	int min;
	int max;
};
MultiCtrlInit multictrinit[] = {
	{ IDC_EDIT_VOLUME, IDC_SPIN_VOLUME, IDC_SLIDER_VOLUME, &CMidiChCtrlDlg::GetVolume, &CMidiChCtrlDlg::SetVolume, 0, 127 },
	{ IDC_EDIT_EXPR, IDC_SPIN_EXPR, IDC_SLIDER_EXPR, &CMidiChCtrlDlg::GetExpr, &CMidiChCtrlDlg::SetExpr, 0, 127 },
	{ IDC_EDIT_MODUR, IDC_SPIN_MODUR, IDC_SLIDER_MODUR, &CMidiChCtrlDlg::GetModur, &CMidiChCtrlDlg::SetModur, 0, 127 },
	{ IDC_EDIT_FOOT, IDC_SPIN_FOOT, IDC_SLIDER_FOOT, &CMidiChCtrlDlg::GetFoot, &CMidiChCtrlDlg::SetFoot, 0, 127 },
	{ IDC_EDIT_PORTA, IDC_SPIN_PORTA, IDC_SLIDER_PORTA, &CMidiChCtrlDlg::GetPorta, &CMidiChCtrlDlg::SetPorta, 0, 127 },
	{ IDC_EDIT_RANGE, IDC_SPIN_RANGE, IDC_SLIDER_RANGE, &CMidiChCtrlDlg::GetRange, &CMidiChCtrlDlg::SetRange, 0, 127 },
	{ IDC_EDIT_BEND, IDC_SPIN_BEND, IDC_SLIDER_BEND, &CMidiChCtrlDlg::GetBend, &CMidiChCtrlDlg::SetBend, -8192, 8191 },
	{ IDC_EDIT_TUNE, IDC_SPIN_TUNE, IDC_SLIDER_TUNE, &CMidiChCtrlDlg::GetTune, &CMidiChCtrlDlg::SetTune, -8192, 8191 },
	{ IDC_EDIT_POLY, IDC_SPIN_POLY, IDC_SLIDER_POLY, &CMidiChCtrlDlg::GetPoly, &CMidiChCtrlDlg::SetPoly, 0, 127 },
	{ IDC_EDIT_PHYCH, IDC_SPIN_PHYCH, IDC_SLIDER_PHYCH, &CMidiChCtrlDlg::GetPhyCh, &CMidiChCtrlDlg::SetPhyCh, 0, 127 },
	{ IDC_EDIT_PAN, IDC_SPIN_PAN, IDC_SLIDER_PAN, &CMidiChCtrlDlg::GetPan, &CMidiChCtrlDlg::SetPan, 0, 127 },
	{ 0, 0, 0, 0, 0, 0, 0,},
};

struct CheckerCtrlInit {
	UINT checkboxid;
	int(CMidiChCtrlDlg::*pgetter)();
	void(CMidiChCtrlDlg::*psetter)(int);
};

CheckerCtrlInit checkerctrinit[] = {
	{ IDC_CHECK_LEGATO, &CMidiChCtrlDlg::GetLegato, &CMidiChCtrlDlg::SetLegato },
	{ IDC_CHECK_PORTA, &CMidiChCtrlDlg::GetPortOn, &CMidiChCtrlDlg::SetPortOn },
	{ IDC_CHECK_MONO, &CMidiChCtrlDlg::GetMono, &CMidiChCtrlDlg::SetMono },
	{ IDC_CHECK_SUSTAIN, &CMidiChCtrlDlg::GetSustain, &CMidiChCtrlDlg::SetSustain },
	{ IDC_CHECK_DAMP, &CMidiChCtrlDlg::GetDamp, &CMidiChCtrlDlg::SetDamp },
	{ IDC_CHECK_SOST, &CMidiChCtrlDlg::GetSoste, &CMidiChCtrlDlg::SetSoste },
	{ 0, 0, 0, },
};

CMidiChCtrlDlg::CMidiChCtrlDlg(CFITOM* pFitom, int ifid, int ch, CWnd* pParent /*=NULL*/)
: CDialogEx(CMidiChCtrlDlg::IDD, pParent), theFitom(pFitom), iIF(ifid), iCH(ch), theConfig(((CFITOMApp*)AfxGetApp())->GetConfig())
, prevDev(-1), prevBank(-1)
{
	theInst = theFitom->GetMidiInst(iIF);
	theCh = theInst->GetMidiCh(iCH);
	Create(IDD, pParent);
	for (int i = 0; multictrinit[i].editid != 0; i++) {
		pMulCtrl[i] = new CMultiCtrl(this, multictrinit[i].editid, multictrinit[i].spinid, multictrinit[i].sliderid,
			multictrinit[i].pgetter, multictrinit[i].psetter, multictrinit[i].min, multictrinit[i].max);
	}
	for (int i = 0; checkerctrinit[i].checkboxid != 0; i++) {
		pChecker[i] = new CChecker(this, checkerctrinit[i].checkboxid, checkerctrinit[i].pgetter, checkerctrinit[i].psetter);
	}
}

CMidiChCtrlDlg::~CMidiChCtrlDlg()
{
}

BOOL CMidiChCtrlDlg::CMultiCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL res = FALSE;
	int val = value;
	TCHAR tmp[32];
	switch (wParam) {
	case UDN_DELTAPOS:
		val = Spin.GetPos32();
		break;
	case EN_CHANGE:
		Edit.GetWindowTextA(tmp, 32);
		val = atoi(tmp);
		break;
	case WM_HSCROLL:
		val = Slider.GetPos();
		break;
	default:
		return FALSE;
	}
	if (value != val) {
		(pParent->*pSetter)(val);
		SetValue();
		return TRUE;
	}
	return FALSE;
}

void CMidiChCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TEXT_MIDICH, edtMidiIfCh);
	DDX_Control(pDX, IDC_COMBO_DEVICE, cmbDevice);
	DDX_Control(pDX, IDC_COMBO_BANK, cmbBank);
	DDX_Control(pDX, IDC_COMBO_PROG, cmbProg);
}


BEGIN_MESSAGE_MAP(CMidiChCtrlDlg, CDialogEx)
	ON_CBN_SELENDOK(IDC_COMBO_DEVICE, &CMidiChCtrlDlg::OnSelendokComboDevice)
	ON_CBN_SELENDOK(IDC_COMBO_BANK, &CMidiChCtrlDlg::OnSelendokComboBank)
	ON_CBN_SELENDOK(IDC_COMBO_PROG, &CMidiChCtrlDlg::OnSelendokComboProg)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_VEDIT, &CMidiChCtrlDlg::OnBnClickedBtnVedit)
	ON_BN_CLICKED(IDC_BUTTON_PICK, &CMidiChCtrlDlg::OnBnClickedButtonPick)
	ON_WM_HSCROLL()
	ON_CONTROL_RANGE(EN_CHANGE, IDC_EDIT_VOLUME, IDC_EDIT_PHYCH, &CMidiChCtrlDlg::OnEnChangeEdit)
	ON_NOTIFY_RANGE(UDN_DELTAPOS, IDC_SPIN_VOLUME, IDC_SPIN_PHYCH, &CMidiChCtrlDlg::OnDeltaposSpin)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_LEGATO, IDC_CHECK_SOST, &CMidiChCtrlDlg::OnBnClickedCheck)
	ON_BN_CLICKED(IDC_BTN_RESET, &CMidiChCtrlDlg::OnBnClickedBtnReset)
END_MESSAGE_MAP()


// CMidiChCtrlDlg メッセージ ハンドラー


void CMidiChCtrlDlg::OnCancel()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。

	OnOK();
}


void CMidiChCtrlDlg::OnOK()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	KillTimer(hTimer);
	Sleep(100);
	DestroyWindow();
}


BOOL CMidiChCtrlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	prevDev = -1;
	prevBank = -1;
	::SetWindowLong(cmbBank.GetSafeHwnd(), GWL_USERDATA, -1);
	// TODO: ここに初期化を追加してください
	if (theCh != 0) {
		InitDevice();
	}

	hTimer = SetTimer(0, 10, 0);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


void CMidiChCtrlDlg::PostNcDestroy()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	::Sleep(100);
	delete this;
}

void CMidiChCtrlDlg::InitDevice()
{
	CString tmp;
	tmp.Format(_T("CH%i - %i:%s"), iCH, iIF, theConfig->GetMidiIn(iIF)->GetDescriptor());
	edtMidiIfCh.SetWindowText(tmp);
	UINT32 curdev = theCh->GetDeviceID();
	cmbDevice.ResetContent();
	cmbDevice.AddString(_T("(NONE)"));
	for (int i = 0; i < theConfig->GetLogDevs(); i++) {
		CSoundDevice* pDev = theConfig->GetLogDeviceFromIndex(i);
		UINT32 physid = theConfig->GetDeviceUniqID(pDev);
		UINT32 devid = physid & 0xff;
		tmp.Format(_T("%02X:%s"), devid, CFITOM::GetDeviceNameFromID(devid));
		int n = cmbDevice.AddString(tmp);
		cmbDevice.SetItemData(n, physid);
		if (physid == curdev) {
			cmbDevice.SetCurSel(n);
		}
	}
	for (int i=0; i<theConfig->GetPcmDevs(); i++) {
		CAdPcmBase* pDev = theConfig->GetPCMDeviceFromIndex(i);
		UINT32 physid = theConfig->GetDeviceUniqID(pDev);
		UINT32 devid = physid & 0xff;
		TCHAR devname[64];
		theConfig->GetDeviceName(physid, devname, _countof(devname));
		tmp.Format(_T("%02X:%s"), devid, devname);
		int n = cmbDevice.AddString(tmp);
		cmbDevice.SetItemData(n, physid);
		if (curdev == physid) {
			cmbDevice.SetCurSel(n);
		}
	}
	{
		tmp.Format(_T("%02X:RHYTHM"), RHYTHM_BANK);
		int n = cmbDevice.AddString(tmp);
		cmbDevice.SetItemData(n, RHYTHM_BANK);
		if (curdev == RHYTHM_BANK) {
			cmbDevice.SetCurSel(n);
		}
	}
	RefreshBank();
	RefreshProg();
}

void CMidiChCtrlDlg::RefreshBank()
{
	if (!cmbBank.GetDroppedState()) {
		UINT32 curdev = theCh->GetDeviceID();
		UINT32 curbnk = theCh->GetBankNo();
		if (::GetWindowLong(cmbBank.GetSafeHwnd(), GWL_USERDATA) != curdev) {
			cmbBank.ResetContent();
			::SetWindowLong(cmbBank.GetSafeHwnd(), GWL_USERDATA, curdev);
			CString tmp;
			int maxbank = MAX_BANK;
			if (theConfig->isPcmDevice(curdev)) {
				if (curbnk > theConfig->GetPcmDevs()) {
					curbnk = 0;
				}
				maxbank = theConfig->GetPcmDevs();
			}
			if (curdev == RHYTHM_BANK) {
				maxbank = 0;
			}
			for (int i = 0; i < maxbank; i++) {
				const TCHAR* pBankName = theConfig->GetBankName(curdev, i);
				if (pBankName) {
					tmp.Format(_T("%03i:%s"), i, pBankName);
					int n = cmbBank.AddString(tmp);
					cmbBank.SetItemData(n, i);
					if (i == curbnk) {
						cmbBank.SetCurSel(n);
					}
				}
			}
		}
		else {
			if (cmbBank.GetCurSel() != curbnk) {
				cmbBank.SetCurSel(curbnk);
			}
		}
	}
}

void CMidiChCtrlDlg::RefreshProg()
{
	if (!cmbProg.GetDroppedState()) {
		UINT32 curdev = theCh->GetDeviceID();
		UINT32 curbnk = theCh->GetBankNo();
		UINT32 curprg = theCh->GetProgramNo();
		if (prevBank != curbnk || prevDev != curdev) {
			prevBank = curbnk;
			prevDev = curdev;
			cmbProg.ResetContent();
			for (int i = 0; i < 128; i++) {
				CString tmp;
				TCHAR vname[64];
				if (theConfig->GetVoiceName(curdev, curbnk, i, vname, _countof(vname))) {
					tmp.Format(_T("%03i:%s"), i, vname);
					int n = cmbProg.AddString(tmp);
					if (n == curprg) {
						cmbProg.SetCurSel(n);
					}
				}
			}
		}
		else {
			cmbProg.SetCurSel(curprg);
		}
	}
}

void CMidiChCtrlDlg::RefreshDevice()
{
	UINT32 dev = theCh->GetDeviceID();
	if (!cmbDevice.GetDroppedState()) {
		if (cmbDevice.GetItemData(cmbDevice.GetCurSel()) != dev) {
			for (int i = 0; i < cmbDevice.GetCount(); i++) {
				if (cmbDevice.GetItemData(i) == dev) {
					cmbDevice.SetCurSel(i);
				}
			}
		}
	}
	RefreshBank();
	RefreshProg();
}

void CMidiChCtrlDlg::OnSelendokComboDevice()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	UINT32 seldev = cmbDevice.GetItemData(cmbDevice.GetCurSel());
	if (theCh->IsInst() && seldev == RHYTHM_BANK) {
		theInst->Release(iCH);
		theInst->AssignRhythm(iCH);
		theCh = theInst->GetMidiCh(iCH);
	}
	else if (theCh->IsRhythm() && seldev != RHYTHM_BANK) {
		theInst->Release(iCH);
		theInst->AssignInst(iCH, theConfig->GetDeviceFromUniqID(seldev), 0);
		theCh = theInst->GetMidiCh(iCH);
	}
	else {
		theCh->BankSelMSB(theConfig->isPcmDevice(seldev) ? ADPCM_BANK : (seldev & 0xff));
		int bk = cmbBank.GetCurSel();
		if (theConfig->isPcmDevice(seldev)) {
			cmbBank.SetCurSel(bk);
			//bk = theConfig->GetPcmDeviceIndex(seldev);
		}
		else if ((seldev & 0xff) != RHYTHM_BANK) {
			while (bk && !theConfig->GetFMBank(CFITOM::GetDeviceVoiceGroupMask(seldev & 0xff), bk)) {
				bk--;
			}
		}
		theCh->BankSelLSB(bk);
		theCh->ProgChange(cmbProg.GetCurSel());
	}
	RefreshDevice();
}

void CMidiChCtrlDlg::OnSelendokComboBank()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	if (theCh->IsInst()) {
		theCh->BankSelLSB(cmbBank.GetCurSel());
		theCh->ProgChange(cmbProg.GetCurSel());
	}
	if (theCh->IsRhythm()) {
		theCh->BankSelLSB(0);
		theCh->ProgChange(cmbProg.GetCurSel());
	}
	RefreshBank();
	RefreshProg();
}

void CMidiChCtrlDlg::OnSelendokComboProg()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	theCh->ProgChange(cmbProg.GetCurSel());
	RefreshProg();
}

void CMidiChCtrlDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ここにメッセージ ハンドラー コードを追加するか、既定の処理を呼び出します。
	if (theCh) {
		RefreshDevice();
		for (int i = 0; i < numMultiCtrl; i++) {
			pMulCtrl[i] ? pMulCtrl[i]->SetValue() : 0;
		}
		for (int i = 0; i < numChecker; i++) {
			pChecker[i] ? pChecker[i]->UpdateState() : 0;
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CMidiChCtrlDlg::OnBnClickedBtnVedit()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	UINT32 devid = cmbDevice.GetItemData(cmbDevice.GetCurSel());
	if (theConfig->isPcmDevice(devid) || (devid & BUILTIN_RHYTHM)) {
		AfxMessageBox(IDS_NOT_SUPPORTED_EDIT, MB_OK);
		return;
	}
	if ((devid & 0xff) == RHYTHM_BANK) {
		CDrumEditDlg dlg(m_pParentWnd);
		dlg.SetProg(cmbProg.GetCurSel());
		if (dlg.DoModal() == IDOK) {

		}
	}
	else {
		CVoiceEditDlg dlg(m_pParentWnd);
		dlg.SetDevice(devid);
		dlg.SetBank(cmbBank.GetCurSel());
		dlg.SetProg(cmbProg.GetCurSel());
		if (dlg.DoModal() == IDOK) {

		}
	}
}

void CMidiChCtrlDlg::OnBnClickedButtonPick()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	CDlgVoicePicker dlg;
	dlg.SetDevice(cmbDevice.GetItemData(cmbDevice.GetCurSel()));
	dlg.SetBank(cmbBank.GetCurSel());
	dlg.SetProg(cmbProg.GetCurSel());
	if (dlg.DoModal() == IDOK) {
		UINT32 devid = dlg.GetDevice();
		theCh->BankSelMSB(theConfig->isPcmDevice(devid) ? ADPCM_BANK : (devid & 0xff));
		theCh->BankSelLSB(dlg.GetBank());
		theCh->ProgChange(dlg.Getprog());
		RefreshDevice();
	}
}

void CMidiChCtrlDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: ここにメッセージ ハンドラー コードを追加するか、既定の処理を呼び出します。
	for (int i = 0; i < numMultiCtrl; i++) {
		if (pMulCtrl[i] && pMulCtrl[i]->IsMember(pScrollBar->GetDlgCtrlID())) {
			switch (nSBCode) {
			case TB_ENDTRACK:
			case TB_TOP:
			case TB_BOTTOM:
			case TB_LINEUP:
			case TB_LINEDOWN:
			case TB_PAGEUP:
			case TB_PAGEDOWN:
				nPos = ((CSliderCtrl*)pScrollBar)->GetPos();
				break;
			case TB_THUMBPOSITION:
			case TB_THUMBTRACK:
				break;
			}
			NMUPDOWN nms;
			nms.hdr.hwndFrom = pScrollBar->GetSafeHwnd();
			nms.hdr.idFrom = pScrollBar->GetDlgCtrlID();
			nms.hdr.code = nSBCode;
			nms.iPos = nPos;
			nms.iDelta = 0;
			LRESULT res = 0;
			pMulCtrl[i]->OnNotify(WM_HSCROLL, LPARAM(&nms), &res);
			return;
		}
	}
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CMidiChCtrlDlg::OnEnChangeEdit(UINT id)
{
	for (int i = 0; i < numMultiCtrl; i++) {
		if (pMulCtrl[i] && pMulCtrl[i]->IsMember(id)) {
			NMHDR nms;
			nms.code = EN_CHANGE;
			nms.hwndFrom = GetDlgItem(id)->GetSafeHwnd();
			nms.idFrom = id;
			LRESULT res = 0;
			pMulCtrl[i]->OnNotify(EN_CHANGE, LPARAM(&nms), &res);
		}
	}
}

void CMidiChCtrlDlg::OnDeltaposSpin(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	for (int i = 0; i < numMultiCtrl; i++) {
		if (pMulCtrl[i] && pMulCtrl[i]->IsMember(id)) {
			pMulCtrl[i]->OnNotify(UDN_DELTAPOS, LPARAM(pNMHDR), pResult);
		}
	}
	*pResult = 0;
}

void CMidiChCtrlDlg::OnBnClickedCheck(UINT id)
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	for (int i = 0; i < numChecker; i++){
		if (pChecker[i] && pChecker[i]->IsMember(id)) {
			pChecker[i]->OnClicked();
		}
	}
}

BOOL CMidiChCtrlDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	if (WM_KEYDOWN == pMsg->message)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:
			return FALSE;
		case VK_ESCAPE:
			return FALSE;
		default:
			break;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CMidiChCtrlDlg::OnBnClickedBtnReset()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	if (theCh) {
		theCh->ResetAllCtrl();
	}
}
