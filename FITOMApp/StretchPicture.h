#pragma once


// CStretchPicture

class CStretchPicture : public CStatic
{
	DECLARE_DYNAMIC(CStretchPicture)

public:
	CStretchPicture();
	virtual ~CStretchPicture();

protected:
	HBITMAP m_hBmp;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	HBITMAP SetBitmap(HBITMAP bmp);
	HBITMAP GetBitmap() const { return m_hBmp; };
};


