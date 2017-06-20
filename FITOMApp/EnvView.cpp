// EnvView.cpp : 実装ファイル
//

#include "stdafx.h"
#include "FITOMApp.h"
#include "EnvView.h"

#define BG_COL	RGB(255, 255, 255)
#define FG_COL	RGB(0, 0, 0)
#define OS_UNIT	128
#define OS_CX	(OS_UNIT*4)
#define OS_CY	OS_UNIT

// CEnvView

IMPLEMENT_DYNAMIC(CEnvView, CWnd)

CEnvView::CEnvView()
{
	hMemDC = CreateCompatibleDC(NULL);
	BITMAPINFO bmi;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biClrImportant = 0;
	bmi.bmiHeader.biClrUsed = 0;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biHeight = OS_CY;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bmi.bmiHeader.biSizeImage = 0;
	bmi.bmiHeader.biWidth = OS_CX;
	bmi.bmiHeader.biXPelsPerMeter = 0;
	bmi.bmiHeader.biYPelsPerMeter = 0;
	hBmp = CreateDIBSection(hMemDC, &bmi, DIB_RGB_COLORS, &pBmp, NULL, 0);
	hOld = (HBITMAP)SelectObject(hMemDC, (HGDIOBJ)hBmp);
	CDC memDC;
	memDC.Attach(hMemDC);
	memDC.FillSolidRect(0, 0, OS_CX, OS_CY, BG_COL);
	memDC.Detach();
}

CEnvView::~CEnvView()
{
	SelectObject(hMemDC, (HGDIOBJ)hOld);
	DeleteObject((HGDIOBJ)hBmp);
	DeleteDC(hMemDC);
}

void CEnvView::SetEnv(int tl, int egs, int ar, int dr, int sl, int sr, int rr)
{
	theTL = tl;
	theEGS = egs;
	theAR = ar;
	theDR = dr;
	theSL = sl;
	theSR = sr;
	theRR = rr;
	Update();
}


BEGIN_MESSAGE_MAP(CEnvView, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()

// CEnvView メッセージ ハンドラー

void CEnvView::Update()
{
	CDC memDC;
	memDC.Attach(hMemDC);
	memDC.FillSolidRect(0, 0, OS_CX, OS_CY, BG_COL);
	CBrush bs;
	bs.CreateSolidBrush(FG_COL);
	CBrush* oldbs = memDC.SelectObject(&bs);

	//Origin
	int x = 0;
	int y = OS_CY;
	memDC.MoveTo(x, y);
	//Attack
	x += (OS_UNIT - theAR);
	y -= OS_CY;
	memDC.LineTo(x, y);
	//Decay
	if (theDR && theSL) {
		double dx = OS_UNIT - theDR;
		double dy = ((dx / OS_UNIT) * double(theSL));
		dx = ((OS_UNIT / dx) * double(theSL));
		x += round(dx);
		y += round(dy);
		memDC.LineTo(x, y);
		y += (theSR / 2);
	}
	else if (theDR == 0) {
		//No decay and No sustain
	}
	else if (theSL == 0) {
		//No decay
		y += (theSR / 2);
	}
	else {
	}
	//Sustain
	memDC.LineTo(OS_CX - (OS_UNIT - theRR), y);
	//Release
	memDC.LineTo(OS_CX, OS_CY);

	memDC.SelectObject(oldbs);
	memDC.Detach();

}

void CEnvView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: ここにメッセージ ハンドラー コードを追加します。
					   // 描画メッセージで CWnd::OnPaint() を呼び出さないでください。
	CRect rc;
	this->GetWindowRect(&rc);
	Gdiplus::Graphics* gr = new Gdiplus::Graphics(dc);
	Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(hBmp, 0);
	gr->DrawImage(bmp, Gdiplus::Rect(0, 0, rc.Width(), rc.Height()));
}
