#pragma once


// CStretchPicture

class CStretchPicture : public CStatic
{
	DECLARE_DYNAMIC(CStretchPicture)

public:
	CStretchPicture();
	virtual ~CStretchPicture();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};


