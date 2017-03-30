// DrumMapPicker.cpp : 実装ファイル
//

#include "stdafx.h"
#include "FITOMApp.h"
#include "DrumMapPicker.h"
#include "afxdialogex.h"
#include "FITOMCfg.h"

// CDrumMapPicker ダイアログ

IMPLEMENT_DYNAMIC(CDrumMapPicker, CDialogEx)

CDrumMapPicker::CDrumMapPicker(CWnd* pParent /*=NULL*/)
: CDialogEx(CDrumMapPicker::IDD, pParent), theConfig(((CFITOMApp*)AfxGetApp())->GetConfig()), theProg(0)
{

}

CDrumMapPicker::~CDrumMapPicker()
{
}

void CDrumMapPicker::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DRUMMAP_LIST, lstDrumMap);
}


BEGIN_MESSAGE_MAP(CDrumMapPicker, CDialogEx)
	ON_NOTIFY(NM_DBLCLK, IDC_DRUMMAP_LIST, &CDrumMapPicker::OnDblclkDrummapList)
END_MESSAGE_MAP()


// CDrumMapPicker メッセージ ハンドラー


void CDrumMapPicker::OnDblclkDrummapList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	*pResult = 0;
	theProg = lstDrumMap.GetItemData(lstDrumMap.GetSelectionMark());
	OnOK();
}


BOOL CDrumMapPicker::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO: ここに初期化を追加してください
	lstDrumMap.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	lstDrumMap.InsertColumn(0, _T("Prog."), LVCFMT_RIGHT, 40);
	lstDrumMap.InsertColumn(1, _T("Name"), LVCFMT_LEFT, 160);
	lstDrumMap.InsertColumn(2, _T("Path"), LVCFMT_LEFT, 200);

	if (theConfig) {
		int rows = 0;
		CString tmp;
		for (int i = 0; i < 128; i++) {
			CDrumBank* pbank = theConfig->GetDrumBank(i);
			if (pbank) {
				tmp.Format(_T("%i"), i);
				lstDrumMap.InsertItem(rows, tmp);
				lstDrumMap.SetItemText(rows, 1, pbank->GetBankName());
				lstDrumMap.SetItemText(rows, 2, pbank->GetFileName());
				lstDrumMap.SetItemData(rows, DWORD_PTR(i));
				if (i == theProg) {
					lstDrumMap.SetSelectionMark(rows);
				}
				rows++;
			}
		}
		if (lstDrumMap.GetSelectionMark() < 0) {
			lstDrumMap.SetSelectionMark(0);
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


void CDrumMapPicker::OnOK()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	theProg = lstDrumMap.GetItemData(lstDrumMap.GetSelectionMark());
	CDialogEx::OnOK();
}
