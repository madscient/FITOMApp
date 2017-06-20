#pragma once


// CEnvView

class CEnvView : public CWnd
{
	DECLARE_DYNAMIC(CEnvView)

public:
	CEnvView();
	virtual ~CEnvView();
	void SetEnv(int tl, int egs, int ar, int dr, int sl, int sr, int rr);
	void Update();
protected:
	HBITMAP hBmp;
	HBITMAP hOld;
	HDC hMemDC;
	LPVOID pBmp;
	int theTL;
	int theEGS;
	int theAR;
	int theDR;
	int theSL;
	int theSR;
	int theRR;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};


