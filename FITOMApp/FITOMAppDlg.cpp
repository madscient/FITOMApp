
// FITOMAppDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "FITOMApp.h"
#include "FITOMAppDlg.h"
#include "afxdialogex.h"
#include "MidiMonitor.h"
#include "DevMonitor.h"
#include "SysCtlDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// �A�v���P�[�V�����̃o�[�W�������Ɏg���� CAboutDlg �_�C�A���O

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

// ����
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFITOMAppDlg �_�C�A���O



CFITOMAppDlg::CFITOMAppDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFITOMAppDlg::IDD, pParent)
	, bMidMon(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFITOMAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_MIDIMON, btnMidiMon);
	DDX_Check(pDX, IDC_CHECK_MIDIMON, bMidMon);
	DDX_Control(pDX, IDC_MASVOL, sldMasterVol);
	DDX_Control(pDX, IDC_LOGO, stcLogo);
}

BEGIN_MESSAGE_MAP(CFITOMAppDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_COMMAND(IDC_CHECK_MIDIMON, OnMidiMon)
	ON_COMMAND(IDC_BUTTON_DEVMON, OnDevMon)
	ON_COMMAND(IDC_BUTTON_SYSCTL, OnBnClickedButtonSysctl)
	ON_COMMAND(IDC_BUTTON_VEDIT, OnVEdit)
	ON_COMMAND(IDC_BUTTON_DEDIT, OnBnClickedButtonDedit)
	ON_WM_DRAWITEM()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CFITOMAppDlg ���b�Z�[�W �n���h���[

BOOL CFITOMAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// "�o�[�W�������..." ���j���[���V�X�e�� ���j���[�ɒǉ����܂��B

	// IDM_ABOUTBOX �́A�V�X�e�� �R�}���h�͈͓̔��ɂȂ���΂Ȃ�܂���B
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		bool bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B
	stcLogo.SetBitmap(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP1)));
	theFitom = ((CFITOMApp*)AfxGetApp())->GetFitom();
	dlgMidi = new CMidiMonitor(theFitom, this);
	SetTimer(IDD, 15, NULL);
	sldMasterVol.SetRange(0, 127);
	sldMasterVol.SetPos(theFitom->GetMasterVolume());
	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
}

void CFITOMAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void CFITOMAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �`��̃f�o�C�X �R���e�L�X�g

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �N���C�A���g�̎l�p�`�̈���̒���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �A�C�R���̕`��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ���[�U�[���ŏ��������E�B���h�E���h���b�O���Ă���Ƃ��ɕ\������J�[�\�����擾���邽�߂ɁA
//  �V�X�e�������̊֐����Ăяo���܂��B
HCURSOR CFITOMAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFITOMAppDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h���[ �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B
	sldMasterVol.SetPos(theFitom->GetMasterVolume());
	CDialogEx::OnTimer(nIDEvent);
}

void CFITOMAppDlg::OnOK()
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊��N���X���Ăяo���Ă��������B
	dlgMidi->CloseWindow();

	CDialogEx::OnOK();
}

void CFITOMAppDlg::OnDevMon()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	CDevMonitor* pdlg = new CDevMonitor(this);
	pdlg->ShowWindow(SW_SHOW);
}


void CFITOMAppDlg::OnConfig()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
}


void CFITOMAppDlg::OnMidiMon()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	UpdateData(1);
	dlgMidi->ShowWindow(bMidMon ? SW_SHOW : SW_HIDE);
}

void CFITOMAppDlg::OnVEdit()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	//�{�C�X�G�f�B�^�N��
	/*
	CVoiceEditDlg dlg(this);
	dlg.SetDevice(theFitom->GetLogicalDeviceFromIndex(0)->GetDevice());
	dlg.SetBank(0);
	dlg.SetProg(0);
	if (dlg.DoModal() == IDOK) {

	}
	*/
}


BOOL CFITOMAppDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊��N���X���Ăяo���Ă��������B
	return CDialogEx::OnCommand(wParam, lParam);
}


void CFITOMAppDlg::OnBnClickedButtonDedit()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	//�h�����}�b�v�G�f�B�^�N��
	/*
	CDrumMapPicker mdlg(this);
	mdlg.SetProg(0);
	if (mdlg.DoModal() == IDOK) {
		CDrumEditDlg dlg(this);
		dlg.SetProg(mdlg.GetProg());
		if (dlg.DoModal() == IDOK) {

		}
	}
	*/
}

void CFITOMAppDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h���[ �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B
	if (pScrollBar->GetDlgCtrlID() == IDC_MASVOL) {
		theFitom->SetMasterVolume((uint8_t)sldMasterVol.GetPos());
	}
	else {
		CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
	}
}


void CFITOMAppDlg::OnBnClickedButtonSysctl()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	CSysCtlDlg dlg(this);
	dlg.DoModal();
}
