// PcmBankDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "FITOMcfg.h"
#include "PcmBankDlg.h"
#include "afxdialogex.h"
#include <strsafe.h>
#include <mmsystem.h>


// CPcmBankDlg ダイアログ

IMPLEMENT_DYNAMIC(CPcmBankDlg, CDialogEx)

CPcmBankDlg::CPcmBankDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CPcmBankDlg::IDD, pParent), bModified(FALSE)
{

}

CPcmBankDlg::~CPcmBankDlg()
{
}

void CPcmBankDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PCM_BANK, lstPcmBank);
	DDX_Control(pDX, IDC_EDT_BANKNAME, edtBankName);
	DDX_Control(pDX, IDC_EDT_FILENAME, edtFileName);
}


BEGIN_MESSAGE_MAP(CPcmBankDlg, CDialogEx)
	ON_MESSAGE(WM_KICKIDLE, &CPcmBankDlg::OnKickIdle)
	ON_COMMAND(IDC_BTN_TEST, &CPcmBankDlg::OnBtnTest)
	ON_COMMAND(IDC_BTN_ADD, &CPcmBankDlg::OnBtnAdd)
	ON_COMMAND(IDC_BTN_UP, &CPcmBankDlg::OnBtnUp)
	ON_COMMAND(IDC_BTN_DOWN, &CPcmBankDlg::OnBtnDown)
	ON_COMMAND(IDC_BTN_DEL, &CPcmBankDlg::OnBtnDel)
	ON_COMMAND(IDC_BTN_OPEN, &CPcmBankDlg::OnBtnOpen)
	ON_COMMAND(IDC_BTN_HILO, &CPcmBankDlg::OnBtnHiLo)
	ON_UPDATE_COMMAND_UI(IDC_BTN_TEST, &CPcmBankDlg::OnUpdateBtnTest)
	ON_UPDATE_COMMAND_UI(IDC_BTN_UP, &CPcmBankDlg::OnUpdateBtnUp)
	ON_UPDATE_COMMAND_UI(IDC_BTN_DOWN, &CPcmBankDlg::OnUpdateBtnDown)
	ON_UPDATE_COMMAND_UI(IDC_BTN_DEL, &CPcmBankDlg::OnUpdateBtnDel)
	ON_UPDATE_COMMAND_UI(IDC_BTN_HILO, &CPcmBankDlg::OnUpdateBtnDel)
	ON_EN_CHANGE(IDC_EDT_BANKNAME, &CPcmBankDlg::OnEnChangeEdtBankname)
END_MESSAGE_MAP()

LRESULT CPcmBankDlg::OnKickIdle(WPARAM wParam, LPARAM lParam)
{
	UpdateDialogControls(this, FALSE);

	return FALSE;
}

void CPcmBankDlg::Refresh(int lastselection)
{
	edtFileName.SetWindowText(filename);
	edtBankName.SetWindowText(bankname);
	lstPcmBank.DeleteAllItems();
	for (int i = 0; i < wavfiles.size(); i++) {
		CString prog;
		prog.Format(_T("%i"), i);
		lstPcmBank.InsertItem(i, prog);
		lstPcmBank.SetItemText(i, 1, wavfiles[i].filename);
		lstPcmBank.SetItemText(i, 2, wavfiles[i].rate ? _T("32kHz") : _T("16kHz"));
	}
	if (lastselection >= 0) {
		lstPcmBank.SetItemState(lastselection, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	}
}

// CPcmBankDlg メッセージ ハンドラー

BOOL CPcmBankDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO: ここに初期化を追加してください
	lstPcmBank.DeleteAllItems();
	lstPcmBank.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	lstPcmBank.InsertColumn(0, _T("Prog No."), LVCFMT_LEFT, 24);
	lstPcmBank.InsertColumn(1, _T("Filename"), LVCFMT_LEFT, 480);
	lstPcmBank.InsertColumn(2, _T("Rate"), LVCFMT_LEFT, 64);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void CPcmBankDlg::OnOK()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	if (bModified) {
		::WritePrivateProfileSection(_T("Bank"), NULL, filename);
		::WritePrivateProfileString(_T("Header"), _T("BankName"), bankname, filename);
		::WritePrivateProfileString(_T("Header"), _T("Type"), _T("ADPCM"), filename);
		std::vector<WAVFILES>::iterator iter = wavfiles.begin();
		for (int i = 0; iter < wavfiles.end(); iter++, i++) {
			CString keyname;
			keyname.Format(_T("Prog%i"), i);
			CString param;
			param.Format(_T("%s,%i"), iter->filename, iter->rate);
			::WritePrivateProfileString(_T("Bank"), keyname, param, filename);
		}
	}
	CDialogEx::OnOK();
}

void CPcmBankDlg::OnBtnOpen()
{
	// TODO: ここにコマンド ハンドラー コードを追加します。
	CFileDialog dlg(TRUE, _T(".bnk"), 0, OFN_CREATEPROMPT | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_ENABLESIZING,
		_T("Bank file(*.bnk)|*.bnk|All files(*.*)|*.*||"), this);
	if (dlg.DoModal() == IDOK) {
		filename = dlg.GetPathName();
		if (::PathFileExists(filename)) {
			TCHAR buf[MAX_PATH];
			::GetPrivateProfileString(_T("Header"), _T("BankName"), _T("**NONE**"), buf, _countof(buf), filename);
			bankname = buf;
			wavfiles.clear();
			for (int i = 0; i < 128; i++) {
				TCHAR keyname[80];
				StringCchPrintf(keyname, _countof(keyname), _T("Prog%i"), i);
				::GetPrivateProfileString(_T("Bank"), keyname, _T(""), buf, _countof(buf), filename);
				if (lstrlen(buf)) {
					WAVFILES wf;
					sscanf_s(buf, _T("%[^,],%i"), wf.filename, _countof(wf.filename), &wf.rate);
					wavfiles.push_back(wf);
				}
				else {
					break;
				}
			}
		}
		else {
			wavfiles.clear();
			bModified = TRUE;
		}
		Refresh();
	}
}

void CPcmBankDlg::OnBtnTest()
{
	// TODO: ここにコマンド ハンドラー コードを追加します。
	int selidx = lstPcmBank.GetSelectionMark();
	::PlaySound(wavfiles[selidx].filename, 0, SND_FILENAME | SND_ASYNC);
}

void CPcmBankDlg::OnBtnAdd()
{
	// TODO: ここにコマンド ハンドラー コードを追加します。
	CFileDialog dlg(TRUE, _T(".wav"), 0, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_ENABLESIZING,
		_T("Wave file(*.wav)|*.wav|All files(*.*)|*.*||"), this);
	if (dlg.DoModal() == IDOK) {
		CString wfname = dlg.GetPathName();
		if (::PathFileExists(wfname)) {
			WAVFILES wf;
			StringCchCopy(wf.filename, _countof(wf.filename), wfname);
			wf.rate = 0;	// 16kHz as default
			int selidx = lstPcmBank.GetSelectionMark();
			if (selidx >= 0) {
				wavfiles.insert(wavfiles.begin() + selidx, wf);
			}
			else {
				wavfiles.push_back(wf);
			}
			Refresh(selidx);
		}
	}
}

void CPcmBankDlg::OnBtnUp()
{
	// TODO: ここにコマンド ハンドラー コードを追加します。
	int selidx = lstPcmBank.GetSelectionMark();
	std::swap(wavfiles[selidx], wavfiles[selidx - 1]);
	bModified = TRUE;
	Refresh(selidx-1);
}

void CPcmBankDlg::OnBtnDown()
{
	// TODO: ここにコマンド ハンドラー コードを追加します。
	int selidx = lstPcmBank.GetSelectionMark();
	std::swap(wavfiles[selidx], wavfiles[selidx + 1]);
	bModified = TRUE;
	Refresh(selidx + 1);
}

void CPcmBankDlg::OnBtnDel()
{
	// TODO: ここにコマンド ハンドラー コードを追加します。
	int selidx = lstPcmBank.GetSelectionMark();
	wavfiles.erase(wavfiles.begin() + selidx);
	bModified = TRUE;
	Refresh(selidx);
}

void CPcmBankDlg::OnBtnHiLo()
{
	// TODO: ここにコマンド ハンドラー コードを追加します。
	int selidx = lstPcmBank.GetSelectionMark();
	wavfiles[selidx].rate = ~(wavfiles[selidx].rate) & 1;
	bModified = TRUE;
	Refresh(selidx);
}

void CPcmBankDlg::OnUpdateBtnTest(CCmdUI *pCmdUI)
{
	// TODO:ここにコマンド更新 UI ハンドラー コードを追加します。
	int selidx = lstPcmBank.GetSelectedCount();
	pCmdUI->Enable((selidx > 0) && ::PathFileExists(wavfiles[selidx].filename));
}

void CPcmBankDlg::OnUpdateBtnUp(CCmdUI *pCmdUI)
{
	// TODO:ここにコマンド更新 UI ハンドラー コードを追加します。
	pCmdUI->Enable(lstPcmBank.GetSelectionMark() > 0);
}

void CPcmBankDlg::OnUpdateBtnDown(CCmdUI *pCmdUI)
{
	// TODO:ここにコマンド更新 UI ハンドラー コードを追加します。
	pCmdUI->Enable(lstPcmBank.GetSelectionMark() < (wavfiles.size() - 1));
}

void CPcmBankDlg::OnUpdateBtnDel(CCmdUI *pCmdUI)
{
	// TODO:ここにコマンド更新 UI ハンドラー コードを追加します。
	pCmdUI->Enable(lstPcmBank.GetSelectedCount() > 0);
}


void CPcmBankDlg::OnCancel()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	if (bModified) {
		if (::AfxMessageBox(IDS_STR_CONFIRM_DISCARD, MB_YESNO) == IDNO) {
			return;
		}
	}
	CDialogEx::OnCancel();
}


void CPcmBankDlg::OnEnChangeEdtBankname()
{
	// TODO: これが RICHEDIT コントロールの場合、このコントロールが
	// この通知を送信するには、CDialogEx::OnInitDialog() 関数をオーバーライドし、
	// CRichEditCtrl().SetEventMask() を
	// OR 状態の ENM_CHANGE フラグをマスクに入れて呼び出す必要があります。

	// TODO: ここにコントロール通知ハンドラー コードを追加してください。
	edtBankName.GetWindowText(bankname);
}
