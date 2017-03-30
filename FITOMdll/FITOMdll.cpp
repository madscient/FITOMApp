// FITOMdll.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "stdafx.h"
#include "FITOMdll.h"


// これは、エクスポートされた変数の例です。
FITOMDLL_API int nFITOMdll=0;

// これは、エクスポートされた関数の例です。
FITOMDLL_API int fnFITOMdll(void)
{
	return 42;
}

// これは、エクスポートされたクラスのコンストラクターです。
// クラス定義に関しては FITOMdll.h を参照してください。
CFITOMdll::CFITOMdll()
{
	return;
}
