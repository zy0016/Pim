/**************************************************************************\
**                                                                        **
**                  Hopen Device Driver Interface           		      **
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
 * Hopen Device Driver Interface
 *
 * $Revision: 1 $     $Date: 99-11-08 14:21 $    
 *
 * $Log: /Hopen/include/ddi/DI_Mouse.h $
 * 
 * 1     99-11-08 14:21 Dling
 * 
 * 1     99-11-08 13:38 Dling
 * 
 ***************************************************************************
 */

#ifndef _DDI_MOUSE_H
#define _DDI_MOUSE_H

/*
typedef struct MOUSE_Mode {
	unsigned short slen;		// 该结构的长度
	unsigned short unused_1;	// 未用
	unsigned short samplerate;	// 每秒钟采样的次数
	unsigned short resolution;	// 移动的分辨率
} MOUSE_Mode;
*/

/* Device state structure */
/*
typedef struct MOUSE_Event {
	unsigned short	slen;
	unsigned short	btnstate;
	short	dx;
	short	dy;
} MOUSE_Event;*/

typedef struct MOUSEEVENT {
	short	dx;
	short	dy;
	WORD	btn_state;
	WORD	time;
}MOUSEEVENT;

/* Define mouse event */
#define	MOUSE_INPUTEVENT	1

#endif // _DDI_MOUSE_H