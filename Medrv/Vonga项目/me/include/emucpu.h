/**************************************************************************\
**                                                                        **
**                      NetOS Kernel System                               **
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

/**************************************************************************\
 *
 *  $Revision: 1 $	$Date: 99-06-18 9:45 $
 *
 *  $Log: /os/include/emucpu.h $
 * 
 * 1     99-06-18 9:45 Dling
 * 
 * 2     99-05-10 9:15a Xswang
 * 
 * 1     99-04-02 3:08p Xswang
 * 
\**************************************************************************/
#ifndef _EMUCPU_H
#define _EMUCPU_H

#ifndef _TYPEDEF_
#define _TYPEDEF_

#ifndef NULL
#define NULL    (void *)0
#endif

typedef signed long		int32;
typedef unsigned long	uint32;
typedef signed short	int16;
typedef unsigned short	uint16;
typedef signed char		int8;
typedef unsigned char	uint8;
typedef unsigned int	uint;

#endif

#define CONST   const

#define EMBEDDED        1

#define OBJ_HANDLE_BASE 1025
#define	OBJ_TASKID_BASE	4097

#define DEFINTSTATE
#define	ENTER_CRITICAL			_CPU_EnterCritical( )
#define	LEAVE_CRITICAL			_CPU_LeaveCritical( )

#define MAX_SERVICE             32
#define MAX_TICKPROC            32
#define DEF_QUEUELEN            16
/* 32 interrupts sources in 68328 */
#define	MAX_INTERRUPTS			32
#define	MAX_POSTINTS			16
/* Ticks per second is 20 */
#define	TICKS_PERSECOND			20

#define	TASK_TIMESLICE			5

#define	PRIO_TIMER		200		/* 时钟任务的优先级 */
#define	PRIO_LOWERST	255		/* 最低的任务优先数 */

#define MAX_CACHES      128
#define CACHE_SIZE      1024
#define	MEM_HANDLE_BASE	1024

struct tag_context {
	uint32	regs[14];
};

#define	SETERROR( e )	pRunningTask->error = e
#define	GETERROR		pRunningTask->error


struct emu_keydata {
	int		vkey;
	uint	keydata;
};

struct emu_mousedata {
	int		x;
	int		y;
	int		flags;
};

struct emu_pendata {
	int		x;
	int		y;
	int		flags;
};

/* Interrupt vector for emulate device */
#define	IRQ_KEYBOARD	1
#define	IRQ_MOUSE		2
#define	IRQ_PEN			3
#define	IRQ_CHARDEV		6
#define	IRQ_COMM		7
#define IRQ_INFRARED	8

/* For emulate pen action */
#define BTN_PENDOWN 0x0010
#define	BTN_PENMOVE	0x0040
#define BTN_PENUP   0x0020

#ifndef	_IN_EMUCPU_

#ifdef	__cplusplus
extern "C" {
#endif

int  _EMU_CreateTask(struct tag_tcb * pTask, struct tag_taskcreate * pData);
void _EMU_EndTask( struct tag_tcb * pTask );
void _EMU_TaskSleep( struct tag_tcb * pTask );
void _EMU_TaskWakeup( struct tag_tcb * pTask );

struct tag_tcb * _EMU_GetRunningTask( void );
void _EMU_SetRunningTask( struct tag_tcb * pTask );

void _EMU_KeyboardInt( int vkey, uint keydata );
void _EMU_PenInt( int x, int y, int flags );
void _EMU_MouseInt( int x, int y, int flags );
void _EMU_ChardevInt( int flags );
void _EMU_CommInt(int mask);
void _EMU_InfraInt(int event);

/* Get Communication mode setting */
int    Emu_GetCommMode( void );
void * Emu_GetCommDCB( void );

#ifdef	__cplusplus
}
#endif

#endif

#endif // _EMUCPU_H