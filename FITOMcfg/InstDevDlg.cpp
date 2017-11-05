// InstDevDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "FITOMcfg.h"
#include "InstDevDlg.h"
#include "afxdialogex.h"


// CInstDevDlg ダイアログ

IMPLEMENT_DYNAMIC(CInstDevDlg, CDialogEx)

CInstDevDlg::CInstDevDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_INST_DLG, pParent)
{

}

CInstDevDlg::~CInstDevDlg()
{
}

void CInstDevDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_INST, lstDevice);
	DDX_Control(pDX, IDC_COMBO_DEVICE, cmbModule);
	DDX_Control(pDX, IDC_COMBO_MODE, cmbMode);
	DDX_Control(pDX, IDC_COMBO_TYPE, cmbType);
}


BEGIN_MESSAGE_MAP(CInstDevDlg, CDialogEx)
END_MESSAGE_MAP()


// CInstDevDlg メッセージ ハンドラー
LPCTSTR instruments[] = {
	_T("OPNA/B FM"),
	_T("OPNA/B ADPCM B"),
	_T("OPNA Rhythm"),
	_T("OPNB ADPCM A"),
	_T("SSG"),
	_T("PSG"),
	_T("APSG"),
	_T("DCSG"),
	_T("SCC"),
	_T("SCC+"),
	_T("SAA"),
	_T("OPM FM"),
	_T("OPL/Y8950 FM"),
	_T("Y8950 ADPCM"),
	_T("OPL2 FM"),
	_T("OPL3 FM 2OP"),
	_T("OPL3 FM 4OP"),
	_T("OPN FM"),
	_T("OPN2 FM"),
	_T("OPN2C FM"),
	_T("OPN2L/3L FM"),
	_T("OPN3L Rhythm"),
	_T("OPP FM"),
	_T("OPZ FM"),
	_T("OPLL/OPLL2 FM"),
	_T("OPLLP FM"),
	_T("OPLLX FM"),
	_T("OPLL/2/P/X Rhythm"),
	_T("OPK/2 FM"),
	_T("OPK/2 Rhythm"),
};