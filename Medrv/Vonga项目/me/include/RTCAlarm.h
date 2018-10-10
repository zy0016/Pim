/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   : Driver
 *
 * Filename: RTCAlarm.h    
 *
 * Purpose : For M68328 RTC    
 *  
 * Author  :     
 *
 *-------------------------------------------------------------------------
 *
 * $Archive:: /RWP_SRC_Emu/include/RTCAlarm.h                        $
 * $Revision:: 1   $     $Date:: 01-05-11 13:41                      $
 *
 * $History:: RTCAlarm.h                                             $
 * 
 * *****************  Version 1  *****************
 * User: Jqiao        Date: 01-05-11   Time: 13:41
 * Created in $/RWP_SRC_Emu/include
 * 1.闹钟报警模块需要使用的头文件
 * 
\**************************************************************************/
#include "hpdef.h" 

typedef struct tagAlarmTime
{
	WORD wYear;
	WORD wMonth;
	WORD wDay;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
	WORD wSound;
}ALARMTIME, *PALARMTIME;