// LineInputDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "FITOMcfg.h"
#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <Functiondiscoverykeys_devpkey.h>
#include "LineInputDlg.h"
#include "afxdialogex.h"

#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

// CLineInputDlg ダイアログ

IMPLEMENT_DYNAMIC(CLineInputDlg, CDialogEx)

CLineInputDlg::CLineInputDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MVOL_DLG, pParent)
{

}

CLineInputDlg::~CLineInputDlg()
{
}

void CLineInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMB_LINEIN, cmbLineIn);
}


BEGIN_MESSAGE_MAP(CLineInputDlg, CDialogEx)
//	ON_WM_NCDESTROY()
ON_WM_CLOSE()
END_MESSAGE_MAP()


// CLineInputDlg メッセージ ハンドラー


BOOL CLineInputDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO: ここに初期化を追加してください
	TCHAR lineindev[256];
	::GetPrivateProfileString(_T("LINEIN"), _T("device"), _T(""), lineindev, _countof(lineindev), _T(".\\FITOM.ini"));
	cmbLineIn.ResetContent();
	int n = cmbLineIn.AddString(_T("(None)"));
	cmbLineIn.SetItemData(n, 0);
	IMMDeviceEnumerator* pmmenum = 0;
	HRESULT hres = ::CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pmmenum));
	if (SUCCEEDED(hres)) {
		IMMDeviceCollection* pmmcol = 0;
		hres = pmmenum->EnumAudioEndpoints(EDataFlow::eCapture, DEVICE_STATE_ACTIVE | DEVICE_STATE_UNPLUGGED, &pmmcol);
		if (SUCCEEDED(hres)) {
			UINT count = 0;
			hres = pmmcol->GetCount(&count);
			for (UINT i = 0; i < count; i++) {
				IMMDevice* pmmdev = 0;
				hres = pmmcol->Item(i, &pmmdev);
				if (SUCCEEDED(hres) && pmmdev) {
					IPropertyStore* pps = 0;
					hres = pmmdev->OpenPropertyStore(STGM_READ, &pps);
					if (SUCCEEDED(hres)) {
						PROPVARIANT varName;
						PropVariantInit(&varName);
						LPWSTR devid;
						hres = pmmdev->GetId(&devid);
						if (SUCCEEDED(hres) && SUCCEEDED(pps->GetValue(PKEY_Device_FriendlyName, &varName))) {
							n = cmbLineIn.AddString(CW2T(varName.pwszVal));
							cmbLineIn.SetItemData(n, (DWORD_PTR)devid);
							if (lstrcmp(lineindev, CW2T(devid)) == 0) {
								cmbLineIn.SetCurSel(n);
							}
						}
						devid = 0;
					}
					SAFE_RELEASE(pps);
				}
				SAFE_RELEASE(pmmdev);
			}
		}
		SAFE_RELEASE(pmmcol);
	}
	SAFE_RELEASE(pmmenum);
	if (cmbLineIn.GetCurSel() < 0) {
		cmbLineIn.SetCurSel(0);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


void CLineInputDlg::OnOK()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	::WritePrivateProfileString(_T("LINEIN"), _T("device"), CW2T((LPWSTR)cmbLineIn.GetItemData(cmbLineIn.GetCurSel())), _T(".\\FITOM.ini"));
	CDialogEx::OnOK();
}

void CLineInputDlg::OnClose()
{
	// TODO: ここにメッセージ ハンドラー コードを追加するか、既定の処理を呼び出します。
	for (int i = 0; i < cmbLineIn.GetCount(); i++) {
		LPWSTR devid = (LPWSTR)cmbLineIn.GetItemData(i);
		if (devid) {
			CoTaskMemFree(devid);
		}
	}
	CDialogEx::OnClose();
}
