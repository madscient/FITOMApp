
// FITOMcfg.cpp : アプリケーションのクラス動作を定義します。
//

#include "stdafx.h"
#include "FITOMcfg.h"
#include "FITOMcfgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFITOMcfgApp

BEGIN_MESSAGE_MAP(CFITOMcfgApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CFITOMcfgApp コンストラクション

CFITOMcfgApp::CFITOMcfgApp()
{
	// 再起動マネージャーをサポートします
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}


// 唯一の CFITOMcfgApp オブジェクトです。

CFITOMcfgApp theApp;

extern chipSymbols symbols[] = {
	{ SC_TYPE_NONE, _T("NONE"), _T("Nothing"), },
	{ SC_TYPE_YM2608, _T("OPNA"), _T("YM2608"), },
	{ SC_TYPE_YM2151, _T("OPM"), _T("YM2151"), },
	{ SC_TYPE_YM2610, _T("OPNB"), _T("YM2610"), },
	{ SC_TYPE_YM2203, _T("OPN"), _T("YM2203"), },
	{ SC_TYPE_YM2612, _T("OPN2"), _T("YM2612"), },
	{ SC_TYPE_AY8910, _T("PSG"), _T("AY3-8910"), },
	{ SC_TYPE_SN76489, _T("DCSG"), _T("SN76489"), },
	{ SC_TYPE_YM3812, _T("OPL2"), _T("YM3812"), },
	{ SC_TYPE_YMF262, _T("OPL3"), _T("YMF262"), },
	{ SC_TYPE_YM2413, _T("OPLL"), _T("YM2413"), },
	{ SC_TYPE_YM3526, _T("OPL"), _T("YM3526"), },
	{ SC_TYPE_YMF288, _T("OPN3L"), _T("YMF288"), },
	{ SC_TYPE_SCC, _T("SCC"), _T("2212P003"), },
	{ SC_TYPE_SCCS, _T("SCC+"), _T("2312P001"), },
	{ SC_TYPE_Y8950, _T("Y8950"), _T("YM3801"), },
	{ SC_TYPE_YM2164, _T("OPP"), _T("YM2164"), },
	{ SC_TYPE_YM2414, _T("OPZ"), _T("YM2414"), },
	{ SC_TYPE_AY8930, _T("EPSG"), _T("AY8930"), },
	{ SC_TYPE_YM2149, _T("SSG"), _T("YM2149"), },
	{ SC_TYPE_YMZ294, _T("SSGL"), _T("YMZ294"), },
	{ SC_TYPE_SN76496, _T("DCSG"), _T("SN76496"), },
	{ SC_TYPE_YM2420, _T("OPLL2"), _T("YM2420"), },
	{ SC_TYPE_YMF281, _T("OPLLP"), _T("YMF281"), },
	{ SC_TYPE_YMF276, _T("OPN2L"), _T("YMF276"), },
	{ SC_TYPE_YM2610B, _T("OPNBB"), _T("YM2610B"), },
	{ SC_TYPE_YMF286, _T("OPNBK"), _T("YMF286K"), },
	{ SC_TYPE_YM2602, _T("315-5124"), _T("YM2602"), },
	{ SC_TYPE_UM3567, _T("OPLL"), _T("UM3567"), },
	{ SC_TYPE_YMF274, _T("OPL4"), _T("YMF274"), },
	{ SC_TYPE_YM3806, _T("OPQ"), _T("YM3806"), },
	{ SC_TYPE_YM2163, _T("DSG"), _T("YM2163"), },
	{ SC_TYPE_YM7129, _T("OPK2"), _T("YM7129"), },
	{ SC_TYPE_YMZ280, _T("PCMD8"), _T("YMZ280"), },
	{ SC_TYPE_YMZ705, _T("SSGS"), _T("YMZ705"), },
	{ SC_TYPE_YMZ735, _T("FMS"), _T("YMZ735"), },
	{ SC_TYPE_YM2423, _T("OPLLX"), _T("YM2423"), },
	{ SC_TYPE_SPC700, _T("SPC700"), _T("SPC700"), },
	{ SC_TYPE_NBV4,	_T("NBV4") _T("NBV4"), },	// NBV4用
	{ SC_TYPE_AYB02, _T("AYB02"), _T("AYB02"), },		// AYB02用
	{ SC_TYPE_8253,	_T("i8253"), _T("i8253"), },	// i8253（及び互換チップ用）
	{ SC_TYPE_OTHER, _T("OTHER"), _T("OTHER"), },
	{ SC_TYPE_UNKNOWN, _T("UNKNOWN"), _T("UNKNOWN"), },
	{ SC_TYPE_MAX, 0, 0, },
};

// CFITOMcfgApp 初期化

BOOL CFITOMcfgApp::InitInstance()
{
	// アプリケーション マニフェストが visual スタイルを有効にするために、
	// ComCtl32.dll Version 6 以降の使用を指定する場合は、
	// Windows XP に InitCommonControlsEx() が必要です。さもなければ、ウィンドウ作成はすべて失敗します。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// アプリケーションで使用するすべてのコモン コントロール クラスを含めるには、
	// これを設定します。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// ダイアログにシェル ツリー ビューまたはシェル リスト ビュー コントロールが
	// 含まれている場合にシェル マネージャーを作成します。
	CShellManager *pShellManager = new CShellManager;

	// MFC コントロールでテーマを有効にするために、"Windows ネイティブ" のビジュアル マネージャーをアクティブ化
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 標準初期化
	// これらの機能を使わずに最終的な実行可能ファイルの
	// サイズを縮小したい場合は、以下から不要な初期化
	// ルーチンを削除してください。
	// 設定が格納されているレジストリ キーを変更します。
	// TODO: 会社名または組織名などの適切な文字列に
	// この文字列を変更してください。
	SetRegistryKey(_T("アプリケーション ウィザードで生成されたローカル アプリケーション"));

	CFITOMcfgDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ダイアログが <OK> で消された時のコードを
		//  記述してください。
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ダイアログが <キャンセル> で消された時のコードを
		//  記述してください。
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: ダイアログの作成に失敗しました。アプリケーションは予期せずに終了します。\n");
		TRACE(traceAppMsg, 0, "警告: ダイアログで MFC コントロールを使用している場合、#define _AFX_NO_MFC_CONTROLS_IN_DIALOGS を指定できません。\n");
	}

	// 上で作成されたシェル マネージャーを削除します。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ダイアログは閉じられました。アプリケーションのメッセージ ポンプを開始しないで
	//  アプリケーションを終了するために FALSE を返してください。
	return FALSE;
}

BOOL CFITOMcfgApp::InitSCCI()
{
	// scci.dllの読込み
	if (!(hScci = ::LoadLibrary(_T("scci.dll")))) {
		return -1;
	}

	// サウンドインターフェースマネージャー取得用関数アドレス取得
	SCCIFUNC getSoundInterfaceManager = (SCCIFUNC)(::GetProcAddress(hScci, _T("getSoundInterfaceManager")));
	if (getSoundInterfaceManager == NULL){
		return -1;
	}

	// サウンドインターフェースマネージャー取得
	if (!(pManager = getSoundInterfaceManager())) {
		return -1;
	}

	// サウンドインターフェースマネージャーインスタンス初期化
	pManager->initializeInstance();

	// リセットを行う
	pManager->init();
	pManager->reset();
	pManager->setLevelDisp(FALSE);

	//pManager->setDelay(0);
	return 0;
}

BOOL CFITOMcfgApp::ExitSCCI()
{
	if (pManager) {
		pManager->releaseAllSoundChip();
		pManager->releaseAllInterface();
		pManager->releaseInstance();
		pManager = 0;
	}
	if (hScci) {
		::FreeLibrary(hScci);
		hScci = 0;
	}
	return TRUE;
}

SC_CHIP_TYPE CFITOMcfgApp::GetChipTypeFromName(LPCTSTR sym)
{
	for (int i = 0; i < SC_TYPE_MAX; i++) {
		if (lstrcmp(symbols[i].chipname, sym) == 0) {
			return symbols[i].type;
		}
	}
	return SC_TYPE_NONE;
}

SC_CHIP_TYPE CFITOMcfgApp::GetChipTypeFromCode(LPCTSTR sym)
{
	for (int i = 0; i < SC_TYPE_MAX; i++) {
		if (lstrcmp(symbols[i].chipcode, sym) == 0) {
			return symbols[i].type;
		}
	}
	return SC_TYPE_NONE;
}

LPCTSTR CFITOMcfgApp::GetChipName(SC_CHIP_TYPE type)
{
	for (int i = 0; i < SC_TYPE_MAX; i++) {
		if (symbols[i].type == type) {
			return symbols[i].chipname;
		}
	}
	return _T("NONE");
}