
/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 2000-2001 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :
 *
 * Filename:	gsminit.h
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

#ifndef _GSMINIT_H_
#define _GSMINIT_H_



#define GSMINIT_RESULT_SUCCESS			0

#define GSMINIT_RESULT_SIMVERIFY		1
#define GSMINIT_RESULT_PINVERIFY		2
	#define GSMINIT_PIN			0
	#define GSMINIT_PUK			1
	#define GSMINIT_PIN2		2
	#define GSMINIT_PUK2		3

#define GSMINIT_RESULT_SIMNOTINSERT		3
#define GSMINIT_RESULT_SIMFAILURE		4
#define GSMINIT_RESULT_NORESPONSE		5
#define GSMINIT_RESULT_FAILURE			6


// External Function
int GsmInit_Initialize( HWND hWnd, UINT Msg );
int GsmInit_Continue( void );
int GsmInit_End( int result );
int GsmInit_GetInfo( void * DestMem, int * MsgLen, int *Count );



#endif //_GSMINIT_H_


