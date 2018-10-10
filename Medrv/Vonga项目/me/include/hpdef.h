/**************************************************************************\
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :	HOPEN OS Kernel System
 *
 * Purpose :    Basic Hopen OS Type Definitions.
 *
 * Author  :	Hejia Li
 *
 *-------------------------------------------------------------------------
 *
 * $Archive:: /hopen2.0/include/hpdef.h                             $
 * $Workfile:: hpdef.h                                              $
 * $Revision:: 7  $     $Date:: 03-02-24 5:08p                      $
 *
\**************************************************************************/

#ifndef __HPDEF_H_
#define __HPDEF_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************/
/*          Common definitions and typedefs                               */
/**************************************************************************/

#define VOID            void

#define FAR
#define NEAR
#ifndef CONST
#define CONST           const
#endif

/**************************************************************************/
/*          Simple types & common helper macros                           */
/**************************************************************************/

typedef int             BOOL;       /* b  */
#define FALSE           0
#define TRUE            1

typedef unsigned char   BYTE;       /* by   */
typedef unsigned short  WORD;       /* w    */
typedef signed   short  IWORD;      /* iw   */
typedef signed   short  SHORT;      /* sn   */
typedef int             INT;        /* n    */
typedef unsigned int    UINT;       /* n    */
typedef unsigned long   DWORD;      /* dw   */
typedef signed   long   LONG;       /* l    */
typedef float           FLOAT;      /* f    */

typedef char            int8;       /* 8-bit signed integer     */
typedef short           int16;      /* 16-bit signed integer    */
typedef long            int32;      /* 32-bit signed integer    */
typedef unsigned char   uint8;      /* 8-bit unsigned integer   */
typedef unsigned short  uint16;     /* 16-bit unsigned integer  */
typedef unsigned long   uint32;     /* 32-bit unsigned integer  */
typedef unsigned int    uint;       /* unsigned int             */

typedef unsigned char   u_char;     /* 8-bit unsigned integer   */
typedef unsigned short  u_short;    /* 16-bit unsigned integer  */
typedef unsigned long   u_long;     /* 32-bit unsigned integer  */
typedef unsigned int    u_int;      /* unsigned integer         */

#ifndef MAKELONG

#define LOBYTE(w)       ((BYTE)(w))
#define HIBYTE(w)       ((BYTE)((UINT)(w) >> 8))

#define LOWORD(l)       ((WORD)(l))
#define HIWORD(l)       ((WORD)((DWORD)(l) >> 16))

#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define MAKELONG(low, high) ((LONG)(((WORD)(low)) | \
                            (((DWORD)((WORD)(high))) << 16)))

#endif /* MAKELONG */

#if !defined(NOMINMAX) && !defined(__cplusplus)
#ifndef max
#define max(a,b)        (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)        (((a) < (b)) ? (a) : (b))
#endif
#endif  /* NOMINMAX */

/**************************************************************************/
/*                  Common pointer types                                  */
/**************************************************************************/

#ifndef NULL
#define NULL            0
#endif

typedef BOOL*           PBOOL;      /* pb   */
typedef BOOL FAR*       LPBOOL;     /* lpb  */
typedef char*           PSTR;       /* psz  */
typedef char FAR*       LPSTR;      /* lpsz */
typedef const char*     PCSTR;      /* psz  */
typedef const char FAR* LPCSTR;     /* lpsz */
typedef BYTE*           PBYTE;      /* pby  */
typedef BYTE FAR*       LPBYTE;     /* lpby */
typedef WORD*           PWORD;      /* pw   */
typedef WORD FAR*       LPWORD;     /* lpw  */
typedef IWORD*          PIWORD;     /* piw  */
typedef IWORD FAR*      LPIWORD;    /* lpiw */
typedef SHORT*          PSHORT;     /* ps   */
typedef SHORT FAR*      LPSHORT;    /* lps  */
typedef int*            PINT;       /* pn   */
typedef int FAR*        LPINT;      /* lpn  */
typedef UINT*           PUINT;      /* pn   */
typedef UINT FAR*       LPUINT;     /* lpn  */
typedef DWORD*          PDWORD;     /* pdw  */
typedef DWORD FAR*      LPDWORD;    /* lpdw */
typedef long*           PLONG;      /* pl   */
typedef long FAR*       LPLONG;     /* lpl  */
typedef float*          PFLOAT;     /* pf   */
typedef float FAR*      LPFLOAT;    /* lpf  */
typedef void*           PVOID;      /* pv   */
typedef void FAR*       LPVOID;     /* lpv  */

typedef LPSTR           PCTSTR;     /* lpsz */
typedef LPSTR           LPCTSTR;    /* lpsz */

#define FIELDOFFSET(type, field)    ((int)(&((type *)1)->field)-1)

/**************************************************************************/
/*                  Common handle types                                   */
/**************************************************************************/

typedef void*                   HANDLE;
#define DECLARE_HANDLE(name)    struct name##__ { int unused; }; \
                                typedef const struct name##__* name

typedef HANDLE*					PHANDLE;	/* ph   */

#ifdef __cplusplus
}
#endif

#define _DEFTYPE_

#endif /* __HPDEF_H_	*/
