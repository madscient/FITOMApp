// TestButton.cpp : 実装ファイル
//

#include "stdafx.h"
#include "FITOMApp.h"
#include "TestButton.h"


// CTestButton

IMPLEMENT_DYNAMIC(CTestButton, CButton)

CTestButton::CTestButton()
{

}

CTestButton::~CTestButton()
{
}


BEGIN_MESSAGE_MAP(CTestButton, CButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CTestButton メッセージ ハンドラー

void CTestButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ここにメッセージ ハンドラー コードを追加するか、既定の処理を呼び出します。
	if (GetParent()) {
		NMHDR nms;
		nms.hwndFrom = GetSafeHwnd();
		nms.idFrom = GetDlgCtrlID();
		nms.code = WM_LBUTTONDOWN;
		GetParent()->SendMessage(WM_NOTIFY, WPARAM(GetDlgCtrlID()), LPARAM(&nms));
	}
	CButton::OnLButtonDown(nFlags, point);
}


void CTestButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: ここにメッセージ ハンドラー コードを追加するか、既定の処理を呼び出します。
	if (GetParent()) {
		NMHDR nms;
		nms.hwndFrom = GetSafeHwnd();
		nms.idFrom = GetDlgCtrlID();
		nms.code = WM_LBUTTONUP;
		LRESULT res = 0;
		GetParent()->SendMessage(WM_NOTIFY, WPARAM(GetDlgCtrlID()), LPARAM(&nms));
	}
	CButton::OnLButtonUp(nFlags, point);
}
