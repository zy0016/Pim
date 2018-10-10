/**************************************************************************\
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :	HOPEN LIB
 *
 * Purpose      Standard library header.
 *
 * Author  :	WeiZhong
 *
 *-------------------------------------------------------------------------
 *
 * $Archive:: /hopen_system/include/malloc.h                        $
 * $Workfile:: malloc.h                                             $
 * $Revision:: 2  $     $Date:: 03-04-10 4:04p                      $
 *
\**************************************************************************/

#ifndef __MALLOC_H
#define __MALLOC_H

#if __cplusplus
extern "C" {
#endif

#ifndef __SIZE_T
#define __SIZE_T
    typedef unsigned int	size_t;
#endif

#ifndef NULL
#define NULL 0
#endif

#if (_C166)
void *os_malloc(long size);
void os_free(_huge void *p);
void *os_realloc(_huge void *p, long size);
#else   // _C166
void *os_malloc(long size);
void os_free(void *p);
void *os_realloc(void *p, long size);
#endif  // _C166

// Redefine the malloc and free function name to avoid conflict with C run
// time library
#define malloc  os_malloc
#define free    os_free
#define realloc os_realloc

//void    free(void* memblock);
//void*   malloc(size_t size);

#if __cplusplus
}
#endif

#endif
