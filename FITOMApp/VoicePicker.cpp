// CDlgVoicePicker.cpp : �����t�@�C��
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

// CdlgCoicePicker �_�C�A���O

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


// CDlgVoicePicker ���b�Z�[�W �n���h���[


void CDlgVoicePicker::OnSelendokComboDevice()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	//SetDevice(cmbDevice.GetItemData(cmbDevice.GetCurSel()));
	theDevice = cmbDevice.GetItemData(cmbDevice.GetCurSel());
	SetBank(0);
	RefreshBank();
	RefreshProg();
}


void CDlgVoicePicker::OnSelendokComboBank()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	SetBank(cmbBank.GetItemData(cmbBank.GetCurSel()));
	RefreshProg();
}

void CDlgVoicePicker::SetDevice(uint32_t dev)
{
	theDevice = dev;
	if (bInit) {
		RefreshDevice();
	}
}

void CDlgVoicePicker::SetBank(uint32_t bank)
{
	theBank = bank;
	if (bInit) {
		RefreshBank();
	}
}

void CDlgVoicePicker::SetProg(uint32_t prog)
{
	theProg = prog;
	if (bInit) {
		RefreshProg();
	}
}

bool CDlgVoicePicker::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	bInit = TRUE;
	// TODO: �����ɏ�������ǉ����Ă�������
	lstProg.SetExtendedStyle(LVS_EX_AUTOSIZECOLUMNS | LVS_EX_LABELTIP | LVS_EX_JUSTIFYCOLUMNS | LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT);
	RefreshDevice();
	RefreshBank();
	RefreshProg();
	pICh = new CInstCh(0, CFITOM::GetInstance());

	return TRUE;  // return TRUE unless you set the focus to a control
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}

CString CDlgVoicePicker::GetBankName(uint32_t device, uint32_t bank)
{
	return CString(theConfig->GetBankName(device, bank));
}

CString CDlgVoicePicker::GetProgName(uint32_t device, uint32_t bank, uint32_t prog)
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
			uint32_t devid = pDev->GetDevice();
			uint32_t phyid = theConfig->GetDeviceUniqID(pDev);
			TCHAR devname[64];
			theConfig->GetDeviceName(phyid, devname, _countof(devname));
			tmp.Format(_T("%02X:%s"), devid, devname);
			int k = cmbDevice.AddString(tmp);
			cmbDevice.SetItemData(k, phyid);
			if (uint8_t(phyid) == uint8_t(theDevice)) {
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
			uint32_t devid = pDev->GetDevice();
			uint32_t phyid = theConfig->GetDeviceUniqID(pDev);
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
	bool isPcm = theConfig->isPcmDevice(theDevice);
	UINT vg = CFITOM::GetDeviceVoiceGroupMask(theDevice & 0xff);
	if (!(vg & VOICE_GROUP_RHYTHM)) {
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
	else {
		int k = cmbBank.AddString(_T("0:Built-In"));
		cmbBank.SetItemData(k, 0);
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
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	*pResult = 0;
	OnOK();
}


void CDlgVoicePicker::OnOK()
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊��N���X���Ăяo���Ă��������B
	theProg = lstProg.GetSelectionMark();
	CDialogEx::OnOK();
}


void CDlgVoicePicker::OnClickListProgs(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	*pResult = 0;
}


void CDlgVoicePicker::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	// TODO:�����ɃR�}���h�X�V UI �n���h���[ �R�[�h��ǉ����܂��B
}


void CDlgVoicePicker::OnEditCopy()
{
	// TODO: �����ɃR�}���h �n���h���[ �R�[�h��ǉ����܂��B
}


void CDlgVoicePicker::OnEditCut()
{
	// TODO: �����ɃR�}���h �n���h���[ �R�[�h��ǉ����܂��B
}


void CDlgVoicePicker::OnUpdateEditCut(CCmdUI *pCmdUI)
{
	// TODO:�����ɃR�}���h�X�V UI �n���h���[ �R�[�h��ǉ����܂��B
}


void CDlgVoicePicker::OnEditDelete()
{
	// TODO: �����ɃR�}���h �n���h���[ �R�[�h��ǉ����܂��B
}


void CDlgVoicePicker::OnUpdateEditDelete(CCmdUI *pCmdUI)
{
	// TODO:�����ɃR�}���h�X�V UI �n���h���[ �R�[�h��ǉ����܂��B
}


void CDlgVoicePicker::OnEditInsert()
{
	// TODO: �����ɃR�}���h �n���h���[ �R�[�h��ǉ����܂��B
}


void CDlgVoicePicker::OnUpdateEditInsert(CCmdUI *pCmdUI)
{
	// TODO:�����ɃR�}���h�X�V UI �n���h���[ �R�[�h��ǉ����܂��B
}


void CDlgVoicePicker::OnEditPaste()
{
	// TODO: �����ɃR�}���h �n���h���[ �R�[�h��ǉ����܂��B
}


void CDlgVoicePicker::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
	// TODO:�����ɃR�}���h�X�V UI �n���h���[ �R�[�h��ǉ����܂��B
}


void CDlgVoicePicker::OnRclickListProgs(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	*pResult = 0;
}


void CDlgVoicePicker::OnRdblclkListProgs(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	theProg = lstProg.GetSelectionMark();
	BeginWaitCursor();
	uint8_t devm = theDevice & 0xff;
	bool isPcm = theConfig->isPcmDevice(theDevice);
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
