/**************************************************************************\
**                                                                        **
**                      MiniTask Kernel System                            **
**                                                                        **
**  Copyright (C) 1998 by CASS Corptation & Software Engineering Center   **
**                       All Rights Reserved                              **
**                                                                        **
** The contents of this file are subject to the CASS License;             **
**                                                                        **
** Software distributed under this packet is distributed on an "AS IS"    **
** basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.        **
**                                                                        **
** Duplicate this file without CASS license is illegal.                   **
**                                                                        **
\**************************************************************************/

/***************************************************************************
 *
 * CPU program for intel X86
 *
 * $Revision: 1 $     $Date: 00-03-24 9:26 $    
 *
 * $Log: /Hopen/include/u_mem.h $
 * 
 * 1     00-03-24 9:26 Dling
 * bak for kernel 2.0
 * 
 ***************************************************************************
 */

#ifndef _UMEM_H
#define _UMEM_H

#include "hpdef.h"

// macro:
#ifndef LMEM_FIXED
#define LMEM_FIXED          0x0000
#define LMEM_MOVEABLE       0x0002
#define LMEM_DISCARDABLE    0x0F00
#endif // LMEM_FIXED

#ifndef LMEM_MODIFY
#define LMEM_MODIFY			0x0080
#endif

#ifndef LMEM_DISCARDED
#define LMEM_DISCARDED      0x4000
#define LMEM_LOCKCOUNT      0x00FF
#endif // LMEM_DISCARDED

#ifndef LMEM_VALID_FLAGS
#define LMEM_VALID_FLAGS    0x0F72
#define LMEM_INVALID_HANDLE 0x8000
#endif	//LMEM_VALID_FLAGS

// create memory heap
void * _UMEM_CreateHeap(void* start, UINT size, UINT handles, UINT granularity);
// alloc memory
void * _UMEM_Alloc(void* hMemHeap, UINT uFlags, UINT uBytes);
// free memory
void * _UMEM_Free(void * hMemHeap, void * hMem);
// realloc memory
void * _UMEM_ReAlloc(void* hMemHeap, void * hMem, UINT uBytes, UINT uFlags);
// lock memory
void * _UMEM_Lock(void * hMemHeap, void * hMem);
// unlock memory
int _UMEM_Unlock(void * hMemHeap, void * hMem);
// get free space:
UINT _UMEM_GetFreeSpace(void * hMemHeap);


#endif //_UMEM_H