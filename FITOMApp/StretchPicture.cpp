// StretchPicture.cpp : 実装ファイル
//

#include "stdafx.h"
#include "FITOMApp.h"
#include "StretchPicture.h"


// CStretchPicture

IMPLEMENT_DYNAMIC(CStretchPicture, CStatic)

CStretchPicture::CStretchPicture() : m_hBmp(NULL)
{
}

CStretchPicture::~CStretchPicture()
{
}

HBITMAP CStretchPicture::SetBitmap(HBITMAP hbmp)
{
	HBITMAP ret = m_hBmp;
	m_hBmp = hbmp;
	Invalidate(FALSE);
	return ret;
}

BEGIN_MESSAGE_MAP(CStretchPicture, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CStretchPicture メッセージ ハンドラー


void CStretchPicture::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: ここにメッセージ ハンドラー コードを追加します。
					   // 描画メッセージで CStatic::OnPaint() を呼び出さないでください。
	HBITMAP hbmp = GetBitmap();
	if (hbmp) {
		CRect rc;
		this->GetWindowRect(&rc);
		Gdiplus::Graphics* gr = new Gdiplus::Graphics(dc);
		Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(hbmp, 0);
		gr->DrawImage(bmp, Gdiplus::Rect(0, 0, rc.Width(), rc.Height()));
	}
}
