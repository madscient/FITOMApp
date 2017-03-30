
// FITOMcfgDlg.h : ヘッダー ファイル
//

#pragma once


// CFITOMcfgDlg ダイアログ
class CFITOMcfgDlg : public CDialogEx
{
// コンストラクション
public:
	CFITOMcfgDlg(CWnd* pParent = NULL);	// 標準コンストラクター

// ダイアログ データ
	enum { IDD = IDD_FITOMCFG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedButtonDev();
	afx_msg void OnClickedButtonMidi();
	afx_msg void OnBnClickedButtonBank();
	afx_msg void OnBnClickedButtonAdpcm();
	afx_msg void OnBnClickedButtonAdpcmBank();
	afx_msg void OnBnClickedButtonExport();
};
