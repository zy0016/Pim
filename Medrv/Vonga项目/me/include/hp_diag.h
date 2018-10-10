/**************************************************************************\
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :	HOPEN OS Kernel System
 *
 * Purpose :    Define the assert macro.
 *  
 * Author  :	Hejia Li
 *
 *-------------------------------------------------------------------------
 *
 * $Archive:: /hopen2.0/include/hp_diag.h                           $
 * $Workfile:: hp_diag.h                                            $
 * $Revision:: 4  $     $Date:: 03-06-06 10:26a                     $
 *
\**************************************************************************/

#ifndef __HPDIAG_H_
#define __HPDIAG_H_

#ifdef _DEBUG

#ifdef _EMULATE_ /* For Emulator */

#define _CRT_ASSERT     2
int __cdecl _CrtDbgReport(
        int,
        const char *,
        int,
        const char *,
        const char *,
        ...);

#define ASSERT(expr) \
  do {    \
    if (!(expr) && \
        (1 == _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, #expr))) \
        __asm { int 3 }; \
  } while (0)

#else   /* _EMULATE_    */

#if	defined( _M68328_ )	/* legend pda, lingdi */

int _FS_DbgReport(const char *, int, const char *);

#define ASSERT(expr) \
	if( ! (expr) ) { \
		_FS_DbgReport(__FILE__, __LINE__, #expr); \
	}

#else	/* #if defined( _M68328_ ) */

#if (_HOPEN_VER >= 200)

void _assert(const char* prompt, const char* pFile, int nLine);

#define ASSERT(expr) \
  do {    \
    if (!(expr)) \
        _assert(#expr, __FILE__, __LINE__); \
  } while (0)

#else   /* _HOPEN_VER >= 200 */

#define ASSERT(f)          ((void)0)

#endif  /* _HOPEN_VER >= 200 */

#endif	/* #if defined( _M68328_ ) */

#endif  /* _EMULATE_    */

#else   /* _DEBUG		*/

#define ASSERT(f)          ((void)0)

#endif	/* !_DEBUG		*/

#endif	/* __HPDIAG_H_	*/
