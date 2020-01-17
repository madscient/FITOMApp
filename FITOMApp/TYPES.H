#ifndef __TYPES_H__
#define __TYPES_H__

typedef char CHAR;

#ifdef _DOS
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
inline uint8_t max(uint8_t a, uint8_t b) { return (a>b) ? a : b; };
inline uint8_t min(uint8_t a, uint8_t b) { return (a<b) ? a : b; };
#define FALSE	(0)
#define TRUE	(!FALSE)
#endif

#if defined(_WIN32) && (_MSC_VER < 1300)
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long DWORD_PTR;
#endif

#if defined(_UNICODE) && !defined(_WIN32)
typedef WCHAR TCHAR;
#define _T(x) L##x
#elif !defined(_WIN32)
typedef CHAR TCHAR;
#define _T(x) x
#endif

#if !defined(_WIN32)
typedef unsigned int UINT;
typedef uint32_t uint32_t;
typedef wchar_t WCHAR;
#ifdef _UNICODE
typedef WCHAR TCHAR;
#else
typedef CHAR TCHAR;
#endif
typedef bool BOOL;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
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
#define tcscpy strcpy
#define tcslen strlen
#define tifstream ifstream
#define tcscmpn strncmp
#define tcscmp strcmp
#endif

#endif
