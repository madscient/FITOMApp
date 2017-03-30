
// FITOMcfg.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル

#include "scci.h"
#include "SCCIDefines.h"

// CFITOMcfgApp:
// このクラスの実装については、FITOMcfg.cpp を参照してください。
//

struct chipSymbols {
	SC_CHIP_TYPE type;
	LPCTSTR chipname;
	LPCTSTR chipcode;
};

class CFITOMcfgApp : public CWinApp
{
protected:
	HMODULE hScci;
	SoundInterfaceManager *pManager;

public:
	CFITOMcfgApp();
	BOOL InitSCCI();
	BOOL ExitSCCI();
	SoundInterfaceManager* GetManager() { return pManager; };
// オーバーライド
public:
	virtual BOOL InitInstance();
	SC_CHIP_TYPE GetChipTypeFromName(LPCTSTR sym);
	SC_CHIP_TYPE GetChipTypeFromCode(LPCTSTR sym);
	LPCTSTR GetChipName(SC_CHIP_TYPE type);
	// 実装
	DECLARE_MESSAGE_MAP()
};

extern CFITOMcfgApp theApp;