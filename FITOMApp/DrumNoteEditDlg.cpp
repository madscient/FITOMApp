// DrumNoteEditDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "FITOMApp.h"
#include "FITOM.h"
#include "FITOMdefine.h"
#include "SoundDev.h"
#include "ADPCM.h"
#include "MIDI.h"
#include "DrumNoteEditDlg.h"
#include "afxdialogex.h"
#include "SCCIWrapper.h"
#include "VoicePicker.h"


// CDrumNoteEditDlg �_�C�A���O

IMPLEMENT_DYNAMIC(CDrumNoteEditDlg, CDialogEx)

CDrumNoteEditDlg::CDrumNoteEditDlg(CRhythmCh* pCH, CWnd* pParent /*=NULL*/)
: CDialogEx(CDrumNoteEditDlg::IDD, pParent), theNote(36), theProg(0), theCH(pCH),
bModified(FALSE), bInitialized(FALSE), bUpdating(FALSE), theConfig(((CFITOMApp*)AfxGetApp())->GetConfig())
{
}

CDrumNoteEditDlg::~CDrumNoteEditDlg()
{
}

void CDrumNoteEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_MAPNAME, edtMapName);
	DDX_Control(pDX, IDC_EDIT_MAPNUM, edtMapNote);
	DDX_Control(pDX, IDC_EDIT_NOTENAME, edtNoteName);
	DDX_Control(pDX, IDC_COMBO_DEV, cmbDevice);
	DDX_Control(pDX, IDC_EDIT_NOTE, edtNoteNum);
	DDX_Control(pDX, IDC_SPIN_NOTE, spnNoteNum);
	DDX_Control(pDX, IDC_SPIN_PAN, spnPanpot);
	DDX_Control(pDX, IDC_SPIN_GATE, spnGate);
	DDX_Control(pDX, IDC_EDIT_PAN, edtPanpot);
	DDX_Control(pDX, IDC_EDIT_GATE, edtGate);
	DDX_Control(pDX, IDC_EDIT_VOICE, edtVoice);
	DDX_Control(pDX, IDC_BTN_PICK, btnVoiceBrowse);
	DDX_Control(pDX, IDC_SPIN_TUNE, spnTune);
	DDX_Control(pDX, IDC_EDIT_TUNE, edtTune);
}


BEGIN_MESSAGE_MAP(CDrumNoteEditDlg, CDialogEx)
	ON_MESSAGE(WM_KICKIDLE, &CDrumNoteEditDlg::OnKickIdle)
	ON_COMMAND(IDC_BTN_TEST, &CDrumNoteEditDlg::OnBtnTest)
	ON_UPDATE_COMMAND_UI(IDC_BTN_TEST, &CDrumNoteEditDlg::OnUpdateBtnTest)
	ON_COMMAND(IDC_BTN_PASTE, &CDrumNoteEditDlg::OnBtnPaste)
	ON_UPDATE_COMMAND_UI(IDC_BTN_PASTE, &CDrumNoteEditDlg::OnUpdateBtnPaste)
	ON_COMMAND(IDC_BTN_COPY, &CDrumNoteEditDlg::OnBtnCopy)
	ON_CBN_SELENDOK(IDC_COMBO_DEV, &CDrumNoteEditDlg::OnModified)
	ON_CBN_SELENDOK(IDC_COMBO_TYPE, &CDrumNoteEditDlg::OnModified)
	ON_EN_CHANGE(IDC_EDIT_NOTENAME, &CDrumNoteEditDlg::OnModified)
	ON_EN_CHANGE(IDC_EDIT_FNUM, &CDrumNoteEditDlg::OnModified)
	ON_EN_CHANGE(IDC_EDIT_NOTE, &CDrumNoteEditDlg::OnModified)
	ON_EN_CHANGE(IDC_EDIT_PAN, &CDrumNoteEditDlg::OnModified)
	ON_EN_CHANGE(IDC_EDIT_GATE, &CDrumNoteEditDlg::OnModified)
	ON_COMMAND(IDC_BTN_PICK, &CDrumNoteEditDlg::OnBtnPick)
	ON_UPDATE_COMMAND_UI(IDC_BTN_PICK, &CDrumNoteEditDlg::OnUpdateBtnPick)
	ON_UPDATE_COMMAND_UI(IDC_COMBO_TYPE, &CDrumNoteEditDlg::OnUpdateCmbType)
END_MESSAGE_MAP()

// CDrumNoteEditDlg ���b�Z�[�W �n���h���[

void CDrumNoteEditDlg::OnBtnTest()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	Update();
	theCH->AllNoteOff();
	theCH->SetVolume(127);
	theCH->SetExpress(127);
	theCH->NoteOn(theNote, 127, &theDrum);
}

void CDrumNoteEditDlg::OnOK()
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊��N���X���Ăяo���Ă��������B
	Update();
	CDialogEx::OnOK();
}

BOOL CDrumNoteEditDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO: �����ɏ�������ǉ����Ă�������
	if (theConfig) {
		cmbDevice.ResetContent();
		CString str;
		cmbDevice.AddString(_T("(NONE)"));
		cmbDevice.SetItemData(0, DEVICE_NONE);
		for (int i = 0; i < theConfig->GetLogDevs(); i++) {
			uint32_t did = theConfig->GetDeviceUniqID(theConfig->GetLogDeviceFromIndex(i));
			str.Format(_T("%02X:%s"), did & 0xff, CFITOM::GetDeviceNameFromID(did));
			int k = cmbDevice.AddString(LPCTSTR(str));
			cmbDevice.SetItemData(k, did);
		}
		/*
		for (int i = 0; i < theConfig->GetPhyDevs(); i++) {
			uint32_t phyid = theConfig->GetDeviceUniqID(theConfig->GetPhysDeviceFromIndex(i), TRUE);
			bool regist = TRUE;
			for (int j = 0; j < cmbDevice.GetCount(); j++) {
				if (phyid == cmbDevice.GetItemData(j)) {
					regist = FALSE;
					break;
				}
			}
			if (regist) {
				DWORD did = phyid & 0xff;
				DWORD ifid = (phyid >> 16) & 0xff;
				DWORD slid = (phyid >> 8) & 0xff;
				CString str;
				str.Format(_T("%02X:%s (%02X:%02X)"), did, CFITOM::GetDeviceNameFromID(did), ifid, slid);
				int k = cmbDevice.AddString(str);
				cmbDevice.SetItemData(k, phyid);
			}
		}
		*/
		for (int i = 0; i < theConfig->GetPcmDevs(); i++) {
			CAdPcmBase* pdev = theConfig->GetPCMDeviceFromIndex(i);
			uint32_t phyid = theConfig->GetDeviceUniqID(pdev);
			DWORD ifid = (phyid >> 16) & 0xff;
			DWORD slid = (phyid >> 8) & 0xff;
			TCHAR devname[64];
			theConfig->GetDeviceName(phyid, devname, _countof(devname));
			CString str;
			str.Format(_T("%02X:%s (%02X:%02X)"), pdev->GetDevice(), devname, ifid, slid);
			int k = cmbDevice.AddString(str);
			cmbDevice.SetItemData(k, phyid);
		}
		spnNoteNum.SetRange(0, 255);
		spnPanpot.SetRange(-63, 63);
		spnGate.SetRange(0, 127);
		spnTune.SetRange(-8192, 8191);
		spnTune.SetBuddy(&edtTune);

		CDrumBank* theBank = theConfig->GetDrumBank(theProg);
		if (theBank) {
			str.Format(_T("%03i:%s"), theProg, theBank->GetBankName());
			edtMapName.SetWindowText(LPCTSTR(str));
			str.Format(_T("%03i"), theProg);
			edtMapNote.SetWindowText(LPCTSTR(str));

			theBank->GetVoice(theNote, &theDrum);
		}
	}
	Refresh();
	bInitialized = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}

void CDrumNoteEditDlg::Refresh()
{
	if (theConfig) {
		CWnd* wndFocused = GetFocus();
		CString str;
		str.Format(_T("%s"), theDrum.name);
		if (edtNoteName.GetSafeHwnd() != wndFocused->GetSafeHwnd()) {
			edtNoteName.SetWindowText(LPCTSTR(str));
		}
		DWORD devtype = theDrum.devID & 0xff;
		for (int i = 0; i < cmbDevice.GetCount(); i++) {
			if (cmbDevice.GetItemData(i) == theDrum.devID) {
				cmbDevice.SetCurSel(i);
				break;
			}
		}
		UINT devgrp = CFITOM::GetDeviceVoiceGroupMask(theDrum.devID & 0xff);
		//btnVoiceBrowse.EnableWindow();
		if (theDrum.bank == 255 && theDrum.prog == 255) { // ROM rhythm
			edtVoice.SetWindowText(_T("(255:255 Internal ROM)"));
		}
		else {
			TCHAR tmp[64];
			theConfig->GetVoiceName(theDrum.devID, theDrum.bank, theDrum.prog, tmp, _countof(tmp));
			str.Format(_T("%i:%i %s"), theDrum.bank, theDrum.prog, tmp);
			edtVoice.SetWindowText(LPCTSTR(str));
		}
		str.Format(_T("%04X"), theDrum.fnum);
		if (edtNoteNum.GetSafeHwnd() != wndFocused->GetSafeHwnd()) {
			spnNoteNum.SetPos(theDrum.num & 0x7f);
		}
		if (edtPanpot.GetSafeHwnd() != wndFocused->GetSafeHwnd()) {
			spnPanpot.SetPos(theDrum.pan);
		}
		if (edtGate.GetSafeHwnd() != wndFocused->GetSafeHwnd()) {
			spnGate.SetPos(theDrum.gate);
		}
		if (edtTune.GetSafeHwnd() != wndFocused->GetSafeHwnd()) {
			spnTune.SetPos((int16_t)theDrum.fnum);
		}
	}
}

void CDrumNoteEditDlg::Update()
{
	CString str;
	theDrum.fnum = (uint16_t)spnTune.GetPos();
	edtNoteName.GetWindowText(str);
	StringCchCopy(theDrum.name, _countof(theDrum.name), LPCTSTR(str));
	theDrum.devID = cmbDevice.GetItemData(cmbDevice.GetCurSel());
	theDrum.device = theConfig->GetDeviceFromUniqID(theDrum.devID);
	theDrum.num = spnNoteNum.GetPos32();
	if ((theDrum.num & 0x80) && (CFITOM::GetDeviceVoiceGroupMask(theDrum.devID & 0xff) != VOICE_GROUP_OPL2)) {
		theDrum.bank = 255;
		theDrum.prog = 255;
	}
	theDrum.gate = spnGate.GetPos32();
	theDrum.pan = spnPanpot.GetPos32();
}

void CDrumNoteEditDlg::OnBtnPaste()
{
	// TODO: Add your command handler code here
	((CFITOMApp*)AfxGetApp())->GetDrumFromClipBoard(&theDrum);
	Refresh();
}

void CDrumNoteEditDlg::OnUpdateBtnPaste(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(((CFITOMApp*)AfxGetApp())->IsDrumInClipBoard());
}

void CDrumNoteEditDlg::OnUpdateBtnTest(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable((theDrum.device != 0));
}

void CDrumNoteEditDlg::OnBtnCopy()
{
	// TODO: Add your command handler code here
	Update();
	((CFITOMApp*)AfxGetApp())->DrumCopy(&theDrum);
}

LRESULT CDrumNoteEditDlg::OnKickIdle(WPARAM wParam, LPARAM lParam)
{
	UpdateDialogControls(this, FALSE);

	return FALSE;
}

void CDrumNoteEditDlg::OnModified()
{
	// TODO: Add your control notification handler code here
	if (bInitialized && !bUpdating) {
		bModified = TRUE;
		bUpdating = TRUE;
		Update();
		Refresh();
		bUpdating = FALSE;
	}
}

void CDrumNoteEditDlg::OnBtnPick()
{
	// TODO: Add your control notification handler code here
	Update();
	CDlgVoicePicker dlg(this);
	dlg.SetBank(theDrum.bank);
	dlg.SetProg(theDrum.prog);
	dlg.SetDevice(cmbDevice.GetItemData(cmbDevice.GetCurSel()));
	if (dlg.DoModal() == IDOK) {
		theDrum.bank = dlg.GetBank();
		theDrum.prog = dlg.Getprog();
		theDrum.devID = dlg.GetDevice();
		theDrum.device = theConfig->GetDeviceFromUniqID(theDrum.devID);
		Refresh();
	}
}

void CDrumNoteEditDlg::OnUpdateBtnPick(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable((!(theDrum.num & 0x80)) && (theDrum.devID & 0xff));
}

void CDrumNoteEditDlg::OnUpdateCmbType(CCmdUI *pCmdUI)
{
	UINT devgrp = CFITOM::GetDeviceVoiceGroupMask(theDrum.devID & 0xff);
	switch (theDrum.devID & 0xff) {
	case DEVICE_OPNA:
	case DEVICE_OPN3L:
	case DEVICE_OPNB:
	case DEVICE_2610B:
	case DEVICE_F286:
	case DEVICE_OPLL:
	case DEVICE_OPLL2:
	case DEVICE_OPLLX:
	case DEVICE_OPLLP:
	case DEVICE_Y8950:
	case DEVICE_OPK:
	case DEVICE_OPK2:
	case DEVICE_DSG:
		// Enable internal rhythm
		pCmdUI->Enable(TRUE);
		break;
	default:
		pCmdUI->Enable(FALSE);
		break;
	}
}