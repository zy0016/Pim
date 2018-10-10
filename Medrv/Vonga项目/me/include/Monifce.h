
/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 2000-2001 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :
 *
 * Filename:	Monifce.h
 *
 * Purpose :
 *
 * Author  :	Jingjing Xu
 *-------------------------------------------------------------------------
 *
 * $Archive::                                                       $
 * $Revision::    $     $Date::                                     $
 *
 * $History::                                                       $
 *
\**************************************************************************/

#ifndef _MONITOR_IFCE_H_
#define _MONITOR_IFCE_H_

/*----- define Battery Charge Status -------*/

//1,2,3,4: Battery Charge

#define		MON_BCS_CLOSEDOWN	0
#define		MON_BCS_CHARGEEND	5	//	charging has finished


/*---------- define PM Notice Code ---------------*/

#define	MON_URC_CRING		1
		#define MON_CRING_REL		1
		#define MON_CRING_FAX		2
		#define MON_CRING_VOICE		3

#define	MON_URC_TELENUMBER	2

#define MON_URC_SMSINDI_SM	3
#define MON_URC_SMSINDI_ME	4
#define MON_URC_SMSINDI_MT	5

#define MON_URC_SSN			6
		#define	MON_SSN_WAITING_CALL		1
		#define	MON_SSN_FORWARDED_CALL		2
		#define	MON_SSN_HELDCALL_TERMINATED	3

#define	MON_URC_SMSTATUS	7
		#define MON_SMSTATUS_SPACEAVAILABLE	1
		#define MON_SMSTATUS_BUFFERFULL		2
		#define MON_SMSTATUS_BUFFEROVERFLOW	3

#define	MON_URC_CTMALARM	8
		#define MON_CTMALARM_NORMAL			1
		#define MON_CTMALARM_CRITICAL		2
		#define MON_CTMALARM_SWITCHOFF		3

#define MON_NOCARRIER		9


//--------------extern function declare---------------

// Define Function Result Code
#define MON_FAILURE		-1
#define MON_SUCCESS		0

extern int Mon_Initialize (void);
extern int Mon_GSMQuery_Stop (void);
extern int Mon_DataMonitor_Stop (void);
extern int Mon_RLSD_Register (HWND Hwnd, unsigned int Msg);
extern int Mon_RLSD_UNRegister (HWND hWnd, unsigned int Msg);


#endif //_MONITOR_IFCE_H_

