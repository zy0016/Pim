/**************************************************************************\
*
*                      Hopen Software System
*
* Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
*                       All Rights Reserved
*
* Model   :		MEdrv
*
* FileName:		MEComn.h
*
* Purpose :		Provide internal interface
*  
* Author  :		Ding Yong
*
\**************************************************************************/
#ifndef _ME_COMMON_H_
#define _ME_COMMON_H_

#include "window.h"
#include "hpdef.h"

/**********************************************
*
*	常量定义
*
***********************************************/
#define _ME_DEBUG_
enum 
{	
	FUNC,
	ATCMD
};
#define SEND_CMD		1
#define SEND_CONTEXT	2

#define TIMER_AT		0
#define TIMER_DH		1

#define ME_SEND_AT		101

#define ATCMD_LEN		52
#define URC_LEN			12

#define RC_BUFLEN		(5*1024)
#define URC_BUFLEN		(512)

#define APPTABLE_COUNT		22
#define ME_SMS_CODE_LEN		650

#define STATE_WAITRC			1
#define STATE_WAITURC			2
/**********************************************
*
*	结构定义
*
***********************************************/
typedef UINT (*_CALLBACK)(LONG lUserData, DWORD dwEventMask, DWORD param1, LONG param2);
typedef struct UART_Control
{
	_CALLBACK	Event_CallBack;
	LONG		para;
	DWORD		EventCode;
}UART_CONTROL, *PUART_CONTROL;

typedef struct tagMEFuncNode
{
	DWORD at_type;
	HWND  hwnd;
	UINT  msg;
	void* at_info;
	unsigned int info_len;
	
}FuncNode, *PFuncNode;

typedef struct tagMEATNode
{
	char command[ATCMD_LEN];
	char* context;
	int   timeout;

}ATNode, *PATNode;

typedef struct tagMEAppNode
{
	HWND hwnd;
	UINT msg;
	char keyword[URC_LEN+4];
	BOOL idle;
}MEAppNode, *PMEAppNode;

typedef struct tagMEAppTable
{
	MEAppNode node[APPTABLE_COUNT];
	int count;
}MEAppTable, *PMEAppTable;

typedef struct tagSend_ATCmd
{
	HWND hCallWnd;
	UINT hCallMsg;
	char command[ATCMD_LEN];
	int timeout;
}Send_ATCmd, *PSend_ATCmd;

typedef struct tagME_MemBlock
{
	char*	p;
	int		len;
	int		offset;
	int		linebegin;

}ME_MemBlock, *PME_MemBlock;

/**********************************************
*
*	全局变量
*
***********************************************/
int		ME_UartHandle;
char	ME_URCBuf[URC_BUFLEN];
HWND	ME_hWnd;
static  int AT_Index = 0;
MEAppTable ME_AppTable;
/**********************************************
*
*	函数声明
*
***********************************************/
extern BOOL ME_QueueInitial( void );
extern void ME_QueueDestroy( void );
extern BOOL ME_FuncInQueue( DWORD, void*, unsigned int );
extern BOOL ME_FuncOutQueue( void );
extern BOOL ME_FuncGetQueueHead( FuncNode* );
extern BOOL ME_ATInQueue( char*, char*, int );
extern BOOL ME_ATOutQueue( void );
extern void ME_ATEmptyQueue( void );
extern BOOL ME_ATGetQueueHead( ATNode* );
extern BOOL ME_IsQueueEmpty( int );

extern void ME_MemInitial( void );
extern BOOL ME_Malloc( int len );
extern void ME_MemEmpty();
extern unsigned int ME_Strlen();
extern char* ME_MemGetPntr();
extern int  ME_MemGetSize();
extern int  ME_MemGetOffset();
extern char* ME_MemGetLine();
extern BOOL ME_IsBufFull();
extern void ME_MemAssign( char c );
extern void ME_MDelOneLine();
extern void ME_MNewLine();
extern int ME_MGetLineLen();

extern BOOL FunctionParser( DWORD, void*, unsigned int );
extern int ResultCodeParser( DWORD, void*, unsigned int );

extern LRESULT CALLBACK ME_WndProc( HWND, UINT, WPARAM, LPARAM );
extern UINT ME_EventProc(LONG, DWORD, DWORD, LONG );


#endif