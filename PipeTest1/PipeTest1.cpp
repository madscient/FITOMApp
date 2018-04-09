// PipeTest1.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>

int main()
{
	HANDLE namedpipe = ::CreateFile(_T("\\\\.\\pipe\\FITOM"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (namedpipe != INVALID_HANDLE_VALUE) {
		TCHAR buf[1024];
		while (1) {
			fgets(buf, 1024, stdin);
			if (strcmp(_T("exit"), buf) == 0) {
				break;
			}
			DWORD len = 0;
			::WriteFile(namedpipe, buf, strlen(buf), &len, NULL);
		}
	}
	else {
		fprintf(stderr, _T("pipe can't open.\n"));
	}
	::CloseHandle(namedpipe);
	return 0;
}

