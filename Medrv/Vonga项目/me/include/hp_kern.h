/**************************************************************************\
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :	HOPEN OS Kernel System
 *
 * Purpose :    API for applications
 *
 * Author  :	Hejia Li
 *
 *-------------------------------------------------------------------------
 *
 * $Archive:: /hopen_system/include/hp_kern.h                       $
 * $Workfile:: hp_kern.h                                            $
 * $Revision:: 8  $     $Date:: 03-06-17 2:19p                      $
 *
\**************************************************************************/
 
#ifndef __HPKERN_H_
#define __HPKERN_H_

/* Hopen kernel version is 2.0 */
#ifndef _HOPEN_VER
#define	_HOPEN_VER	200
#endif

#include <hpdef.h>

typedef void*					OSHANDLE;

/**************************************************************************/
/*                     System API                                         */
/**************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif


/*********
 ********* Task functions.
 *********/

typedef int tid_t;
typedef int pid_t;

extern tid_t OS_GetTaskID(void);
extern pid_t OS_GetProcID(void);
extern int OS_GetTaskPriority(tid_t tid);
extern void * OS_GetTaskData(tid_t taskid, int index);

extern int OS_SetTaskData(int index, void * pData);
extern int OS_SetProcID(pid_t procid);
extern int OS_SetTaskPriority(tid_t tid, int prio);

extern void * OS_GetStackTop(tid_t tid, int mode);
extern void * OS_GetStackBase(tid_t tid);
extern int OS_EnumTask(pid_t procid, tid_t taskid);

struct tag_taskcreate {
	char			name[8];		/* ��������� */
	unsigned short	prio;			/* ����������� */
	unsigned short	flags;			/* ��������� */
	void	 (*entry)(void *);		/* ���������� */
	void  *			param;			/* ����Ĳ��� */
	unsigned long	stksize;		/* ��ջ�ĳ��� */
	void  *			stack;			/* ��ջ�Ŀ�ʼ��ַ */
	void  *			d_base_1;		/* ���ݶλ�����ַ */
	void  *			d_base_2;		/* ���ݶλ�����ַ */
	void  *			unused[4];		/* δ�õı������� */
};

#define TF_REALTIME		0x0002		/* ʵʱ���� */

extern tid_t OS_CreateTaskEx(struct tag_taskcreate * pCreateData);
extern int OS_EndTask(int exitdata);

extern int OS_SuspendTask(tid_t taskid);
extern int OS_ResumeTask(tid_t taskid);

/* Task state */
#define TS_READY        1
#define TS_SLEEP        2
#define TS_SUSPEND      3

struct task_state {
	short	ts_state;		/* Task state */
	short	ts_unused_1;	/* Unused */
	short	ts_base_prio;	/* Task priority */
	short	ts_curr_prio;	/* Current priority */
	void  * ts_wait_addr;	/* Task wait address */
	long	ts_unused_2[5];	/* For future used */
};

extern int OS_GetTaskState(tid_t tid, struct task_state * pReturn);


/***********
 *********** Mutext functions
 ***********/

typedef struct m_mutex {
	int		m_lock;			/* �Ƿ������״̬ */
	int		m_wait;			/* �ȴ���� */
	short   m_taskid;       /* ӵ���ߵ��߳� ID�� */
	short	m_count;		/* Ƕ�׽��������ͳ�� */
	short	m_flag;			/* �������ı�� */
	short	m_unused;		/* ����δ�� */
} MUTEX, *PMUTEX;

#define	MUTEX_EMBEDDED	0x01	/* ������Ϊ��Ƕ��ʹ�õ� */
#define	MUTEX_OWNER		0x02	/* ����ӵ�иû����� */

extern int OS_CreateMutex(PMUTEX pMutex, int mode);
extern int OS_DestroyMutex(PMUTEX pMutex);
extern int OS_WaitMutex(PMUTEX pMutex);
extern int OS_ReleaseMutex(PMUTEX pMutex);
extern int OS_GetMutexEntryCount(PMUTEX pMutex);
extern int OS_GetMutexOwner(PMUTEX pMutex);
extern int OS_GetMutexWaitTask(PMUTEX pMutex, int index);


/***********
 *********** Sleep and wakeup functions
 ***********/

extern int OS_SleepOn(int * pVar, int value);
extern int OS_Wakeup(void * pVar, int count);
extern int OS_Sleep(int miniseconds);
extern int OS_CWait(int * pVar, int value, PMUTEX pMutex);
extern int OS_CSignal(int * pVar, int count);
extern int OS_GetCondvarWaitTask(int * pVar, int index);


/**********
 ********** Inter process communication object functions
 **********/

extern OSHANDLE OS_CreateEvent(char * pName);
extern OSHANDLE OS_OpenEvent(char * pName);
extern int OS_DestroyEvent(OSHANDLE handle);
extern int OS_WaitEvent(OSHANDLE handle);
extern int OS_SignalEvent(OSHANDLE handle, int event);

extern OSHANDLE OS_CreateSemaphore(char * pName, int value);
extern OSHANDLE OS_OpenSemaphore(char * pName);
extern int OS_DestroySemaphore(OSHANDLE handle);
extern int OS_WaitSemaphore(OSHANDLE handle);
extern int OS_CheckSemaphore(OSHANDLE handle);
extern int OS_ReleaseSemaphore(OSHANDLE handle, int count);

extern OSHANDLE OS_CreateMailbox(char * pName, int size);
extern OSHANDLE OS_OpenMailbox(char * pName);
extern int OS_DestroyMailbox(OSHANDLE handle);
extern int OS_ReceiveMail(OSHANDLE handle, void * pBuf);
extern int OS_SendMail(OSHANDLE handle, void * pBuf);

#define RQ_NOBLOCK      1       /* ������ʱ�����п�ʱ������ */
#define SQ_NOBLOCK      1       /* д����ʱ��������ʱ������ */

extern OSHANDLE OS_CreateQueue(char * pName, int szitem, int qlen);
extern OSHANDLE OS_OpenQueue(char * pName);
extern int OS_DestroyQueue(OSHANDLE handle);
extern int OS_ReceiveQueue(OSHANDLE handle, void * pBuf, int flags);
extern int OS_SendQueue(OSHANDLE handle, void * pBuf, int flags);


/*********
 ********* Other system functions
 *********/

typedef unsigned long sysclock_t;
extern sysclock_t OS_GetSysClock(void);
extern int OS_GetTicksPerSecond(void);
extern int OS_DisableSched(void);
extern int OS_EnableSched(int mode);

extern int OS_GetError(void);
extern int OS_SetError(int error);

extern int OS_SetTickProc(void (*func)(), int ticks);


/********
 ******** Debug support functions
 ********/

extern int OS_OutputDebugChar(int chr);
extern int OS_OutputDebugString(const char * pstr);
extern int OS_GetDebugInput(void);

extern int OS_SetSystemParam(int index, void * valuc);
extern void * OS_GetSystemParam (int index);


/**************************************************************************/
/*                     System extend library API                          */
/**************************************************************************/

/********
 ******** For memory allocation
 ********/

/* Local Memory Flags */
#define LMEM_FIXED          0x0000
#define LMEM_MOVEABLE       0x0002
#define LMEM_NOCOMPACT      0x0010
#define LMEM_NODISCARD      0x0020
#define LMEM_ZEROINIT       0x0040
#define LMEM_MODIFY         0x0080
#define LMEM_DISCARDABLE    0x0F00
#define LMEM_VALID_FLAGS    0x0F72
#define LMEM_INVALID_HANDLE 0x8000

#define GMEM_FIXED          0x0000
#define GMEM_MOVEABLE       0x0002
#define GMEM_NOCOMPACT      0x0010
#define GMEM_NODISCARD      0x0020
#define GMEM_ZEROINIT       0x0040
#define GMEM_MODIFY         0x0080
#define GMEM_DISCARDABLE    0x0100
#define GMEM_NOT_BANKED     0x1000
#define GMEM_SHARE          0x2000
#define GMEM_DDESHARE       0x2000
#define GMEM_NOTIFY         0x4000
#define GMEM_LOWER          GMEM_NOT_BANKED
#define GMEM_VALID_FLAGS    0x7F72
#define GMEM_INVALID_HANDLE 0x8000

typedef void * HGLOBAL;
typedef	void * HLOCAL;

/* Structure of heap info */
typedef struct
{
	unsigned long StartAddr;
	unsigned long initSize;
	unsigned long FreeSize;
} HeapInfo;

HGLOBAL GlobalAlloc(unsigned uFlags, unsigned nBytes);
HGLOBAL GlobalFree(HGLOBAL hMem);
HGLOBAL GlobalRealloc(HGLOBAL pMem, unsigned long newsize, unsigned int flags);
void *  GlobalLock(HGLOBAL hMem);
int     GlobalUnlock(HGLOBAL hMem);
unsigned	GlobalGetFreeSpace(void);
void		GetGlobalHeapInfo(HeapInfo *pInfo);

/* For create local heap flags */
#define	HEAP_NOMUTEX		0x01		/* Heap don't use mutex */
#define	HEAP_EXPANDABLE		0x10		/* Heap expandable */

int		LocalCreateHeap(unsigned size, int flags, unsigned maxsize);
HLOCAL  LocalAlloc(unsigned uFlags, unsigned uBytes);
HLOCAL  LocalFree(HLOCAL hMem);
HLOCAL	LocalRealloc(HLOCAL pMem, unsigned long newsize, unsigned int flags);
void *	LocalLock(HLOCAL hMem);
int     LocalUnlock(HLOCAL hMem);
unsigned	LocalGetFreeSpace (void);
void		GetLocalHeapInfo(HeapInfo *pInfo);


/********
 ******** system lib
 ********/

#define	TS_REALTIME		0x0200		/* �� 1.0 ���ݵĶ��� */

/* ��ʱ��ϵͳ���ã�Ŀǰʵ����ϵͳ���У��Ժ󽫱����� */
extern tid_t OS_CreateTask(char * pname, int prio, void (*entry)(void *),
			int stksize, void * para);

#ifdef __cplusplus
}	/* extern "C" */
#endif	/* __cplusplus	*/

#endif	/* __HPKERN_H_	*/