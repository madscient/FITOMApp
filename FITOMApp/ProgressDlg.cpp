// ProgressDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "FITOMApp.h"
#include "ProgressDlg.h"
#include "afxdialogex.h"


// CProgressDlg ダイアログ

IMPLEMENT_DYNAMIC(CProgressDlg, CDialogEx)

CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CProgressDlg::IDD, pParent)
{
	Create(IDD, pParent);
}

CProgressDlg::~CProgressDlg()
{
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, progBar);
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialogEx)
END_MESSAGE_MAP()


// CProgressDlg メッセージ ハンドラー


void CProgressDlg::PostNcDestroy()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。

	delete this;
}


void CProgressDlg::OnOK()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。

	CDialogEx::DestroyWindow();
}


void CProgressDlg::OnCancel()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。

	CDialogEx::DestroyWindow();
}


void CProgressDlg::SetRange(int lower, int upper)
{
	progBar.SetRange32(lower, upper);
}


int CProgressDlg::SetPos(int pos)
{
	return progBar.SetPos(pos);
}


void CProgressDlg::SetStatus(LPCTSTR str)
{
	GetDlgItem(IDC_STATUS_MSG)->SetWindowText(str);
}


void CProgressDlg::SetTarget(LPCTSTR str)
{
	GetDlgItem(IDC_STATUS_TGT)->SetWindowText(str);
}
