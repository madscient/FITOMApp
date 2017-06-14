// DevModeDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "FITOMcfg.h"
#include "DevModeDlg.h"
#include "afxdialogex.h"


// CDevModeDlg ダイアログ

IMPLEMENT_DYNAMIC(CDevModeDlg, CDialogEx)

CDevModeDlg::CDevModeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DEVMODE_DLG, pParent)
{

}

CDevModeDlg::~CDevModeDlg()
{
}

void CDevModeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDevModeDlg, CDialogEx)
END_MESSAGE_MAP()


// CDevModeDlg メッセージ ハンドラー
