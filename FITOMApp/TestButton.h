#pragma once


// CTestButton

class CTestButton : public CButton
{
	DECLARE_DYNAMIC(CTestButton)

public:
	CTestButton();
	virtual ~CTestButton();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};


