// SplashDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "FITOMApp.h"
#include "SplashDlg.h"
#include "afxdialogex.h"


// CSplashDlg ダイアログ

IMPLEMENT_DYNAMIC(CSplashDlg, CDialogEx)

CSplashDlg::CSplashDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSplashDlg::IDD, pParent)
{
	Create(IDD, NULL);
	HBITMAP hbmp = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SPLASH));
	stcLogo.SetBitmap(hbmp);
	SetWindowText(_T("Starting FITOM..."));
}

CSplashDlg::~CSplashDlg()
{
}

void CSplashDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOGO, stcLogo);
}


BEGIN_MESSAGE_MAP(CSplashDlg, CDialogEx)
END_MESSAGE_MAP()


// CSplashDlg メッセージ ハンドラー


void CSplashDlg::PostNcDestroy()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	delete this;
}

void CSplashDlg::SetStatus(LPCTSTR str)
{
	GetDlgItem(IDC_STATUS_MSG)->SetWindowText(str);
}

void CSplashDlg::SetFilename(LPCTSTR str)
{
	GetDlgItem(IDC_STATUS_FILENAME)->SetWindowText(str);
}


void CSplashDlg::OnOK()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。

	CDialogEx::DestroyWindow();
}


void CSplashDlg::OnCancel()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。

	CDialogEx::DestroyWindow();
}
