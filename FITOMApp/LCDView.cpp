// LCDView.cpp : 実装ファイル
//

#include "stdafx.h"
#include "FITOMApp.h"
#include "LCDView.h"


// CLCDView

IMPLEMENT_DYNAMIC(CLCDView, CStatic)

CLCDView::CLCDView()
{
	hMemDC = CreateCompatibleDC(NULL);
	BITMAPINFO bmi;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biClrImportant = 0;
	bmi.bmiHeader.biClrUsed = 0;
	bmi.bmiHeader.biCompression = 0;
	bmi.bmiHeader.biHeight = 108;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bmi.bmiHeader.biSizeImage = 0;
	bmi.bmiHeader.biWidth = 80;
	bmi.bmiHeader.biXPelsPerMeter = 0;
	bmi.bmiHeader.biYPelsPerMeter = 0;
	hBmp = CreateDIBSection(hMemDC, &bmi, DIB_RGB_COLORS, &pBmp, NULL, 0);
	hOld = (HBITMAP)SelectObject(hMemDC, (HGDIOBJ)hBmp);
}

CLCDView::~CLCDView()
{
	SelectObject(hMemDC, (HGDIOBJ)hOld);
	DeleteObject((HGDIOBJ)hBmp);
	DeleteDC(hMemDC);
}


BEGIN_MESSAGE_MAP(CLCDView, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CLCDView メッセージ ハンドラー

void CLCDView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: ここにメッセージ ハンドラー コードを追加します。
					   // 描画メッセージで CStatic::OnPaint() を呼び出さないでください。
}

void CLCDView::SetCap(char* cap)
{
	if (lstrcmp(caption, cap)) {
		lstrcpyn(caption, cap, 16);
	}
}

void CLCDView::SetDot(BYTE dot[16][16])
{
	if (memcmp(dot, lcddot, 16 * 16)) {
		memcpy(lcddot, dot, 16 * 16);
	}
}