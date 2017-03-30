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

#if _MSC_VER < 1300
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned long DWORD_PTR;
#endif

typedef signed char SINT8;
typedef signed short SINT16;
typedef signed long SINT32;

#define NOERR	(0)

#ifdef _WIN32
#define __far
#define __interrupt
#endif

#endif
