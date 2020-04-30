#pragma once


// CLCDView

class CLCDView : public CStatic
{
	DECLARE_DYNAMIC(CLCDView)

public:
	CLCDView();
	virtual ~CLCDView();

protected:
	HBITMAP hBmp;
	HBITMAP hOld;
	HDC hMemDC;
	LPVOID pBmp;
	char caption[17];
	BYTE lcddot[16][16];
	void DrawChar(int x, int y, char ch);
	void DrawDot(int x, int y, bool dot);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	void SetDot(BYTE dot[16][16]);
	void SetCap(char* cap);
	afx_msg void OnShowWindow(bool bShow, UINT nStatus);
};


