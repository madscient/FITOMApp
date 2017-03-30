// MidiCfgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FITOMcfg.h"
#include "MidiCfgDlg.h"
#include "afxdialogex.h"
#include <mmsystem.h>

// CMidiCfgDlg dialog

IMPLEMENT_DYNAMIC(CMidiCfgDlg, CDialogEx)

CMidiCfgDlg::CMidiCfgDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMidiCfgDlg::IDD, pParent)
	, selClock(0)
{

}

CMidiCfgDlg::~CMidiCfgDlg()
{
}

void CMidiCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MIDI1, cmbMidi[0]);
	DDX_Control(pDX, IDC_COMBO_MIDI2, cmbMidi[1]);
	DDX_Control(pDX, IDC_COMBO_MIDI3, cmbMidi[2]);
	DDX_Control(pDX, IDC_COMBO_MIDI4, cmbMidi[3]);
	DDX_Radio(pDX, IDC_RADIO_CLOCK1, selClock);
	DDV_MinMaxInt(pDX, selClock, 0, 3);
}


BEGIN_MESSAGE_MAP(CMidiCfgDlg, CDialogEx)
END_MESSAGE_MAP()


// CMidiCfgDlg message handlers


BOOL CMidiCfgDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO: ここに初期化を追加してください
	TCHAR  curdir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, curdir);
	CString profile;
	profile.Format(_T("%s\\FITOM.ini"), curdir);
	selClock = ::GetPrivateProfileInt(_T("MIDI"), _T("ClockRx"), 0, LPCTSTR(profile));
	CStringArray midiuse;
	for (int i = 0; i < MAX_MIDI; i++) {
		TCHAR tmp[256];
		CString keyname;
		keyname.Format(_T("MIDIIN%0i"), i + 1);
		::GetPrivateProfileString(_T("MIDI"), LPCTSTR(keyname), _T(""), tmp, 256, LPCTSTR(profile));
		midiuse.Add(tmp);
	}
	CStringArray midilist;
	MIDIINCAPS mic;
	int dev = -1;
	int numdevs = midiInGetNumDevs();
	for (int i = 0; i<numdevs; i++) {
		if (midiInGetDevCaps(i, &mic, sizeof(mic)) == MMSYSERR_NOERROR) {
			midilist.Add(mic.szPname);
		}
	}
	for (int i = 0; i < MAX_MIDI; i++){
		cmbMidi[i].ResetContent();
		cmbMidi[i].AddString(_T("(Nothing)"));
		cmbMidi[i].SetItemData(0, DWORD_PTR(-1));
		for (int j = 0; j < midilist.GetCount(); j++) {
			int k = cmbMidi[i].AddString(midilist[j]);
			if (midilist[j].Compare(midiuse[i]) == 0) {
				cmbMidi[i].SetCurSel(k);
			}
		}
		if (cmbMidi[i].GetCurSel() < 0 || midiuse[i].IsEmpty()) {
			cmbMidi[i].SetCurSel(0);
		}
	}
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


void CMidiCfgDlg::OnOK()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	UpdateData();
	TCHAR  curdir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, curdir);
	CString profile;
	profile.Format(_T("%s\\FITOM.ini"), curdir);
	::WritePrivateProfileSection(_T("MIDI"), NULL, LPCTSTR(profile));
	CString str;
	str.Format(_T("%i"), selClock);
	::WritePrivateProfileString(_T("MIDI"), _T("ClockRx"), LPCTSTR(str), LPCTSTR(profile));
	for (int i = 0; i < MAX_MIDI; i++) {
		int k = cmbMidi[i].GetCurSel();
		CString str;
		if (k > 0) {
			cmbMidi[i].GetWindowText(str);
		}
		else {
			str = _T("");
		}
		CString keyname;
		keyname.Format(_T("MIDIIN%0i"), i + 1);
		::WritePrivateProfileString(_T("MIDI"), LPCTSTR(keyname), LPCTSTR(str), LPCTSTR(profile));
	}
	CDialogEx::OnOK();
}
