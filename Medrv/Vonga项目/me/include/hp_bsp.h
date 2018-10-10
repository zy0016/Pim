/**************************************************************************\
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :	HOPEN OS Kernel System
 *
 * Purpose :    API for driver and main control
 *
 * Author  :	Xingshe Wang
 *
 *-------------------------------------------------------------------------
 *
 * $Archive:: /hopen2.0/include/hp_bsp.h                            $
 * $Workfile:: hp_bsp.h                                             $
 * $Revision:: 2  $     $Date:: 03-02-24 5:08p                      $
 *
\**************************************************************************/
 
#ifndef __HPBSP_H_
#define __HPBSP_H_

#include <hp_kern.h>


/**************************************************************************/
/*                     System API                                         */
/**************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

typedef void (* KERNPROC)(void);


/*********
 ********* Kernel initialize functions.
 *********/

extern int OS_InitKernel(char * kmaddr, int kmsize, int max_tasks);
extern int OS_StartKernel();
extern int OS_ExitKernel();


/*********
 ********* Other system functions
 *********/

extern int OS_EnableInterrupt(void);
extern int OS_DisableInterrupt(void);

typedef struct {
	unsigned long blockStart;
	unsigned long blockSize;
} RAMBlock;

int GlobalCreateHeap( RAMBlock * pRAMBlock, int blocks );

typedef void * (* IntHandle)(int);
#define INTHANDLE IntHandle
extern INTHANDLE OS_SetIrqHandle(int irq, INTHANDLE pHandle);

#ifdef __cplusplus
}	/* extern "C" */
#endif	/* __cplusplus	*/

#endif	/* __HPBSP_H_	*/