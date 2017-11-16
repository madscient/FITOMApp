#ifndef __TYPES_H__
#define __TYPES_H__

typedef char CHAR;

#ifdef _DOS
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned long UINT32;
inline UINT8 max(UINT8 a, UINT8 b) { return (a>b) ? a : b; };
inline UINT8 min(UINT8 a, UINT8 b) { return (a<b) ? a : b; };
#define FALSE	(0)
#define TRUE	(!FALSE)
#endif

#if defined(_WIN32) && (_MSC_VER < 1300)
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned long DWORD_PTR;
#endif

typedef signed char SINT8;
typedef signed short SINT16;
typedef signed long SINT32;

#if defined(_UNICODE) && !defined(_WIN32)
typedef WCHAR TCHAR;
#define _T(x) L##x
#elif !defined(_WIN32)
typedef CHAR TCHAR;
#define _T(x) x
#endif

#ifndef _WIN32
typedef unsigned int UINT;
typedef uint32_t UINT32;
typedef wchar_t WCHAR;
typedef bool BOOL;
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned long DWORD_PTR;
typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef BYTE* LPBYTE;
typedef WORD* LPWORD;
typedef DWORD* LPDWORD;
typedef TCHAR TSTR;
typedef TSTR* PTSTR;
typedef PTSTR LPTSTR;
typedef const TCHAR* PCTSTR;
typedef PCTSTR LPCTSTR;
#define MAX_PATH	520
#define ZeroMemory(d,l)	memset(d,0,l)
#define FALSE	false
#define TRUE	true
#endif

#define NOERR	(0)

#ifdef _WIN32
#define __far
#define __interrupt
#endif

#include <string>
#include <iostream>

#ifdef _UNICODE
#define tstring wstring
#define terr wcerr
#define generic_tstring generic_wstring
#define tiptree wiptree
#define tcsncpy wcsncpy
#define tcslen wcslen
#define tifstream wifstream
#define tcscmpn wcsncmp
#define tcscmp wcscmp
#else
#define tstring string
#define terr cerr
#define generic_tstring generic_string
#define tiptree iptree
#define tcsncpy strncpy
#define tcslen strlen
#define tifstream ifstream
#define tcscmpn strncmp
#define tcscmp strcmp
#endif

#endif
