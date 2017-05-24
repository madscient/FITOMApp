// TextVoiceEditDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "FITOMApp.h"
#include "FITOMCfg.h"
#include "afxdialogex.h"
#include "FITOM.h"
#include "TestButton.h"
#include "TextVoiceEditDlg.h"
#include "MIDI.h"
#include "VoiceEditDlg.h"

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

// CTextVoiceEditDlg ダイアログ

IMPLEMENT_DYNAMIC(CTextVoiceEditDlg, CDialogEx)

CTextVoiceEditDlg::CTextVoiceEditDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CTextVoiceEditDlg::IDD, pParent), typeidx(-1), theType(VOICE_GROUP_NONE), theDevice(0)
, pICh(new CInstCh(0, CFITOM::GetInstance())), theConfig(((CFITOMApp*)AfxGetApp())->GetConfig())
{
}

CTextVoiceEditDlg::~CTextVoiceEditDlg()
{
	delete pICh;
}

void CTextVoiceEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMB_TYPE, cmbType);
	DDX_Control(pDX, IDC_EDIT_NAME, edtName);
	DDX_Control(pDX, IDC_CAP_ALFB, capALFB);
	DDX_Control(pDX, IDC_EDIT_ALFB, edtALFB);
	DDX_Control(pDX, IDC_CAP_OP, capOP);
	DDX_Control(pDX, IDC_EDIT_OP, edtOP);
	DDX_Control(pDX, IDC_EDIT_LFO, edtLFO);
	DDX_Control(pDX, IDC_BTN_TEST, btnTest);
	DDX_Control(pDX, IDC_SPIN_NOTE, spnNote);
	DDX_Control(pDX, IDC_CAP_ADD, capAdd);
	DDX_Control(pDX, IDC_EDIT_ADD, edtAdd);
}

struct voicetypes {
	UINT32 type;
	int ops;
	LPCTSTR name;
	LPCTSTR alfb;
	LPCTSTR opv;
	LPCTSTR opa;
	int(CFITOMConfig::*parseFunc)(FMVOICE*, int, std::vector<int>&);
	int(CFITOMConfig::*buildFunc)(FMVOICE*, int, TCHAR*, size_t);
};

voicetypes VoiceTypeTable[] = {
	{ VOICE_GROUP_OPM, 4, _T("OPM"), _T("AL FB NE NF"), _T("AR DR SR RR SL TL KS ML DT1 DT2 AMS"), _T("FIX DT3 WS REV EGS"), &CFITOMConfig::ParseOPMVoice, &CFITOMConfig::BuildOPMVoice, },
	{ VOICE_GROUP_OPNA, 4, _T("OPNA"), _T("AL FB"), _T("AR DR SR RR SL TL KS ML DT1  EG AMS"), 0, &CFITOMConfig::ParseOPNVoice, &CFITOMConfig::BuildOPNVoice, },
	{ VOICE_GROUP_OPL2, 2, _T("OPL2"), _T("AL FB"), _T("AR DR SR RR SL TL KS ML PDT WF AM"), 0, &CFITOMConfig::ParseOPL2Voice, &CFITOMConfig::BuildOPL2Voice, },
	{ VOICE_GROUP_OPL3, 4, _T("OPL3"), _T("AL FB1 FB2"), _T("AR DR SR RR SL TL KS ML PDT WF AM"), 0, &CFITOMConfig::ParseOPL3Voice, &CFITOMConfig::BuildOPL3Voice, },
	{ VOICE_GROUP_OPLL, 2, _T("OPLL"), _T("AL FB"), _T("AR DR SR RR SL TL KS ML PDT WF AM"), 0, &CFITOMConfig::ParseOPLLVoice, &CFITOMConfig::BuildOPLLVoice, },
	{ VOICE_GROUP_PSG, 1, _T("PSG"), _T("AL FB NF"), _T("AR DR SR RR SL BS EG NAM NOM WF NS"), 0, &CFITOMConfig::ParsePSGVoice, &CFITOMConfig::BuildPSGVoice, },
	{ VOICE_GROUP_NONE, 0, 0, 0, 0, 0, 0, },
};

BEGIN_MESSAGE_MAP(CTextVoiceEditDlg, CDialogEx)
	ON_CBN_SELENDOK(IDC_CMB_TYPE, &CTextVoiceEditDlg::OnSelendokCmbType)
END_MESSAGE_MAP()

void CTextVoiceEditDlg::SetVoice(FMVOICE* voice)
{
	theVoice = *voice;
}

void CTextVoiceEditDlg::SetDevice(UINT32 dev)
{
	theDevice = dev;
	theType = CFITOM::GetDeviceVoiceGroupMask(dev);
	for (int i = 0; VoiceTypeTable[i].type != VOICE_GROUP_NONE; i++) {
		if (VoiceTypeTable[i].type == theType) {
			typeidx = i;
		}
	}
}

void CTextVoiceEditDlg::GetVoice(FMVOICE* voice)
{
	*voice = theVoice;
}

UINT32 CTextVoiceEditDlg::GetType()
{
	return theType;
}

// CTextVoiceEditDlg メッセージ ハンドラー


BOOL CTextVoiceEditDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO: ここに初期化を追加してください
	cmbType.ResetContent();
	for (int i = 0; VoiceTypeTable[i].type != VOICE_GROUP_NONE; i++) {
		int n = cmbType.AddString(VoiceTypeTable[i].name);
		cmbType.SetItemData(n, VoiceTypeTable[i].type);
		if (VoiceTypeTable[i].type == theType) {
			cmbType.SetCurSel(n);
		}
	}
	spnNote.SetRange(0, 127);
	spnNote.SetPos(60);
	Refresh();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


BOOL CTextVoiceEditDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	if (wParam == IDC_BTN_TEST) {
		NMHDR* pnms = (NMHDR*)lParam;
		switch (pnms->code) {
		case WM_LBUTTONDOWN:
			OnTestButtonDown();
			break;
		case WM_LBUTTONUP:
			OnTestButtonUp();
			break;
		}
	}
	return CDialogEx::OnNotify(wParam, lParam, pResult);
}

void CTextVoiceEditDlg::OnTestButtonDown()
{
	Update();

	pICh->SetVolume(100);
	pICh->SetExpress(127);
	pICh->SetPanpot(64);
	pICh->BankSelMSB(theDevice);
	pICh->SetVoiceData(&theVoice);
	pICh->NoteOn(spnNote.GetPos(), 127);
}

void CTextVoiceEditDlg::OnTestButtonUp()
{
	pICh->NoteOff(spnNote.GetPos());
}

void CTextVoiceEditDlg::OnOK()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	Update();
	CDialogEx::OnOK();
}


void CTextVoiceEditDlg::OnCancel()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。

	CDialogEx::OnCancel();
}


void CTextVoiceEditDlg::OnSelendokCmbType()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	theType = cmbType.GetItemData(cmbType.GetCurSel());
	Refresh();
}

void CTextVoiceEditDlg::Refresh()
{
	if (theType && typeidx >= 0) {
		capALFB.SetWindowText(VoiceTypeTable[typeidx].alfb);
		capOP.SetWindowText(VoiceTypeTable[typeidx].opv);
		if (VoiceTypeTable[typeidx].opa) {
			capAdd.SetWindowText(VoiceTypeTable[typeidx].opa);
			edtAdd.EnableWindow(TRUE);
		}
		else {
			capAdd.SetWindowText(_T(""));
			edtAdd.EnableWindow(FALSE);
		}
		char vname[17];
		lstrcpynA(vname, theVoice.name, 16);
		vname[16] = 0;
		edtName.SetWindowText(CA2T(vname));
		TCHAR param[80];
		CFITOMApp* pApp = (CFITOMApp*)AfxGetApp();
		(theConfig->*(VoiceTypeTable[typeidx].buildFunc))(&theVoice, 0, param, _countof(param));
		edtALFB.SetWindowText(param);
		CString str = _T("");
		for (int i = 1; i < (VoiceTypeTable[typeidx].ops + 1); i++) {
			int r = (theConfig->*(VoiceTypeTable[typeidx].buildFunc))(&theVoice, i, param, _countof(param));
			str = str + param + _T("\r\n");
		}
		edtOP.SetWindowText(str);
		str = _T("");
		for (int i = 0; i < (VoiceTypeTable[typeidx].ops + 1); i++) {
			theConfig->BuildLFOParam(&theVoice, i, param, _countof(param));
			str = str + param + _T("\r\n");
		}
		edtLFO.SetWindowText(str);
		if (edtAdd.IsWindowEnabled()) {
			for (int i = 0; i < (VoiceTypeTable[typeidx].ops); i++) {
				int r = (theConfig->*(VoiceTypeTable[typeidx].buildFunc))(&theVoice, i + VoiceTypeTable[typeidx].ops + 1, param, _countof(param));
				str = str + param + _T("\r\n");
			}
			edtAdd.SetWindowText(str);
		}
	}
}

void CTextVoiceEditDlg::Update()
{
	theType = cmbType.GetItemData(cmbType.GetCurSel());
	TCHAR vname[17];
	CString str;
	edtName.GetWindowText(str);
	lstrcpyn(vname, str.Left(16), 16);
	vname[16] = _T('\0');
	memcpy(theVoice.name, CT2A(vname), 16);
	edtALFB.GetWindowText(str);
	std::vector<int> param;
	std::vector<std::tstring> lstparam;
	boost::split(lstparam, boost::trim_copy(std::tstring(LPCTSTR(str))), boost::is_space());
	BOOST_FOREACH(std::tstring s, lstparam) {
		if (!s.empty()) {
			param.push_back(stoi(s));
		}
	}
	CFITOMApp* pApp = (CFITOMApp*)AfxGetApp();
	(theConfig->*(VoiceTypeTable[typeidx].parseFunc))(&theVoice, 0, param);
	edtOP.GetWindowText(str);
	for (int count = 0, lineidx = 0, nextidx = 0; nextidx != -1;  ) {
		nextidx = str.Find(_T("\n"), lineidx);
		CString line;
		if (nextidx > lineidx) {
			line = str.Mid(lineidx, nextidx - lineidx);
			lineidx = nextidx + 1;
		} else if (str.Mid(lineidx)) {
			line = str.Mid(lineidx);
		}
		if (line.GetLength()) {
			param.clear();
			lstparam.clear();
			boost::split(lstparam, boost::trim_copy(std::tstring(LPCTSTR(line))), boost::is_space());
			BOOST_FOREACH(std::tstring s, lstparam) {
				if (!s.empty()) {
					param.push_back(stoi(s));
				}
			}
			(theConfig->*(VoiceTypeTable[typeidx].parseFunc))(&theVoice, ++count, param);
		}
	}
	edtLFO.GetWindowText(str);
	for (int count = 0, lineidx = 0, nextidx = 0; nextidx != -1;) {
		nextidx = str.Find(_T("\n"), lineidx);
		CString line;
		if (nextidx > lineidx) {
			line = str.Mid(lineidx, nextidx - lineidx);
			lineidx = nextidx + 1;
		}
		else if (str.Mid(lineidx)) {
			line = str.Mid(lineidx);
		}
		if (line.GetLength()) {
			param.clear();
			lstparam.clear();
			boost::split(lstparam, boost::trim_copy(std::tstring(LPCTSTR(str))), boost::is_space());
			BOOST_FOREACH(std::tstring s, lstparam) {
				if (!s.empty()) {
					param.push_back(stoi(s));
				}
			}
			theConfig->ParseLFOParam(&theVoice, ++count, param);
		}
	}
	if (edtAdd.IsWindowEnabled()) {
		edtAdd.GetWindowText(str);
		for (int count = 0, lineidx = 0, nextidx = 0; nextidx != -1; ) {
			nextidx = str.Find(_T("\n"), lineidx);
			CString line;
			if (nextidx > lineidx) {
				line = str.Mid(lineidx, nextidx - lineidx);
				lineidx = nextidx + 1;
			}
			else if (str.Mid(lineidx)) {
				line = str.Mid(lineidx);
			}
			if (line.GetLength()) {
				param.clear();
				lstparam.clear();
				boost::split(lstparam, boost::trim_copy(std::tstring(LPCTSTR(line))), boost::is_space());
				BOOST_FOREACH(std::tstring s, lstparam) {
					if (!s.empty()) {
						param.push_back(stoi(s));
					}
				}
				(theConfig->*(VoiceTypeTable[typeidx].parseFunc))(&theVoice, (++count) + VoiceTypeTable[typeidx].ops, param);
			}
		}
	}
}