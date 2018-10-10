
/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :	闹钟报警
 *
 * Filename:    ClkAlm.h
 *
 * Purpose :    闹钟报警维护模块外部接口声明
 *  
 * Author  :    JQiao
 *
 *-------------------------------------------------------------------------
 *
 * $Archive:: /RWP_SRC_Emu/include/ClkAlm.h                         $
 * $Revision:: 8  $     $Date:: 01-10-26 16:48                      $
 *
 * $History:: ClkAlm.h                                              $
 * 
 * *****************  Version 8  *****************
 * User: Jqiao        Date: 01-10-26   Time: 16:48
 * Updated in $/RWP_SRC_Emu/include
 * 
 * *****************  Version 7  *****************
 * User: Jqiao        Date: 01-10-13   Time: 14:23
 * Updated in $/RWP_SRC_Emu/include
 * 
 * *****************  Version 6  *****************
 * User: Jqiao        Date: 01-06-14   Time: 14:48
 * Updated in $/RWP_SRC_Emu/include
 * 提交Alpha5版本测试
 * 
 * *****************  Version 5  *****************
 * User: Jqiao        Date: 01-05-30   Time: 15:44
 * Updated in $/RWP_SRC_Emu/include
 * 1.加入闹钟报警自定义消息
 * 2 增加ALM_NODE结构Msg域的数组大小
 * 
 * *****************  Version 3  *****************
 * User: Jqiao        Date: 01-05-09   Time: 17:24
 * Updated in $/RWP_SRC_Emu/Pda_app/ClockAlm
 * 1.修改链表中字段bOrigHour和bOrigMinute的名称为bBegHour和
 *   bBegMinute
 * 
 * *****************  Version 2  *****************
 * User: Jqiao        Date: 01-05-09   Time: 17:16
 * Updated in $/RWP_SRC_Emu/Pda_app/ClockAlm
 * 1.在链表结构中增加了结点(即记录)原有开始小时和开始分钟2个
 *   字段
 * 
 * *****************  Version 1  *****************
 * User: Jqiao        Date: 01-05-08   Time: 15:38
 * Created in $/RWP_SRC_Emu/Pda_app/ClockAlm
 * 闹钟报警维护模块外部接口声明
 * 
\**************************************************************************/

#ifndef _ALM_CLKALM_H
#define _ALM_CLKALM_H


// 宏定义
#define MAX_LEN_ALMMSG	32	// 报警结点中报警信息最大长度


// 自定义时间结构
typedef struct tagDate
{
	BYTE   bMonth;		// 月
	BYTE   bDay;		// 日
	BYTE   bHour;		// 时
	BYTE   bMinute;		// 分
	WORD   wYear;		// 年
	BYTE   bSecond;		// 秒
	WORD   wUnused;
}DATE;
 
typedef struct tagAppParam
{
	WPARAM wParam;
	LPARAM lParam;
}APPPARAM, * PAPPPARAM;


// 链表结点结构
typedef struct tagAlmNode
{
	BYTE       bFrom;				// 报警记录结点来自的模块
	BOOL       bIsPIMApp;			// 本结点是否属于PIM程序

	APPPARAM   AppParam;			// 需要激活的App参数
	
	WORD	   wCaldYear;			// 日程时间(年)
	BYTE       bCaldMonth;			// 日程时间(月)
	BYTE       bCaldDay;			// 日程时间(日)

	BYTE       bBegHour;			// 记录原始开始时间(时)
	BYTE       bBegMinute;			// 记录原始开始时间(分)

	BYTE       bMonth;				// 报警时间(月)
	BYTE       bDay;				// 报警时间(日)
	BYTE       bHour;				// 报警时间(时)

	BYTE       bMinute;				// 报警时间(分)
	BYTE       bRepCyc;				// 报警重复周期
	BYTE       bRepInterval;		// 报警重复间隔
	BYTE       bRepMethod;			// 报警重复方法(仅对重复周期为"每月"有效)

	BYTE       bRepWeekday;			// 报警重复方法(仅对重复周期为"每周"有效)
	BYTE	   bMask;				// 报警重复结束日期是否有效	

	DWORD      dwDBIndex;			// 报警记录在PIM数据库中的索引
	struct	   tagAlmNode * pNext;	// 下一结点 

	DWORD      dwUnunsed;
	WORD       wYear;				// 报警时间(年)

	DATE	   RepEndDate;			// 报警重复结束日期
  
	char	   Msg[50];				// 报警内容

}ALM_NODE;


// 链表头结点结构
typedef struct tagHead
{
	UINT	   nNodeNum;	// 链表结点总数(不包括头结点)
	ALM_NODE * pNext;		// 指向链表第一个结点
}HEAD, * PHEAD;



// 自定义消息
#define  WM_SHOWMSG	    (WM_USER + 200)		// 显示报警信息
#define  WM_APPENDMSG   (WM_USER + 201)		// 追加报警信息
#define  WM_CHGSYSTIME  (WM_USER + 203)		// 系统时间有所更改信息


// 闹钟报警外部接口函数
extern DWORD ClockAlm_AppControl();
extern BOOL  ALM_InitAlarm(void);
extern BOOL  ALM_RegisterAlarm(ALM_NODE *);
extern BOOL  ALM_UpdateAlarm(BYTE, DWORD, ALM_NODE *);
extern BOOL  ALM_DeleteAlarm(BYTE, DWORD);
extern void  ALM_FreeList(HEAD *);


#endif _ALM_CLKALM_H	// _ALM_CLKALM_H