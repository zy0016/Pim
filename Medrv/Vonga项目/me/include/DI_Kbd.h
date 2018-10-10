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
 * $Revision: 3 $     $Date: 00-03-24 9:21 $    
 *
 * $Log: /Hopen/include/ddi/DI_Kbd.h $
 * 
 * 3     00-03-24 9:21 Dling
 * bak for kernel 2.0
 * 
 * 2     00-01-27 10:38 Dling
 * bak for adding fat16
 * 
 * 1     99-11-08 14:20 Dling
 * 
 * 1     99-11-08 13:38 Dling
 * 
 ***************************************************************************
 */

#ifndef _DDI_KEYBOARD_H
#define _DDI_KEYBOARD_H

typedef struct KBDEVENT{
	BYTE	vkey;
	BYTE	ascii;
	BYTE	scancode;
	BYTE	shiftstate;
	DWORD	time;
}KBDEVENT;

/*
typedef struct KEYBOARD_Event
{
	unsigned short	slen;
	unsigned char type;
	unsigned char scancode;
	unsigned char vkey;
	unsigned char unused;
} KEYBOARD_Event; */

#define	KB_KEYDOWN		0x01
#define	KB_NUMPADKEY	0x10
#define	KB_EXTENDKEY	0x20

#define	LED_NUMLOCK		0x02
#define	LED_CAPSLOCK	0x04
#define	LED_SCROLLLOCK	0x01

/* Define mouse event */
#define	KBD_INPUTEVENT	1

#endif // _DDI_KEYBOARD_H