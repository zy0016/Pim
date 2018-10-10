
#include "hp_modcfg.h"


#ifdef SIEMENS_TC35

/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 2000-2001 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :
 *
 * Filename:	dialer.h
 *
 * Purpose :	SmartPhone Version_NOWIN
 *
 * Author  :	Jingjing Xu
 *
 * Date    :	2002/6/17
 *-------------------------------------------------------------------------
 *
 * $Archive::                                                       $
 * $Revision::    $     $Date::                                     $
 *
 * $History::                                                       $
 *
\**************************************************************************/


#ifndef _DIAL_DIALER_H_
#define _DIAL_DIALER_H_


/*--- int DIALER_SelConnect( HWND, UINT, int SelectType, void *pDialInfo ); ---*/

// SelectType:
#define		DIALDEF_WAP			1
#define		DIALDEF_MAIL		2
#define		DIALDEF_SELFGPRS	3
#define		DIALDEF_SELFDATA	4

#define DIAL_MAXSTRLEN		31		// ISP���ֵ���󳤶�,��Ҫ��Ӧsetting.h

// when SelectType = DIALDEF_SELFGPRS
typedef struct TAGDIALER_GPRSINFO{
	char	APN[DIAL_MAXSTRLEN +1];			// APN
	char	UserID[DIAL_MAXSTRLEN +1];		// �û�ID
	char	PassWord[DIAL_MAXSTRLEN +1];	// ����

}DIALER_GPRSINFO, *PDIALER_GPRSINFO;

// when SelectType = DIALDEF_SELFDATA
typedef struct TAGDIALER_DATAINFO{
	char	PhoneNum[DIAL_MAXSTRLEN +1];	// �绰����
	char	UserName[DIAL_MAXSTRLEN +1];	// �û���
	char	PassWord[DIAL_MAXSTRLEN +1];	// ����

}DIALER_DATAINFO, *PDIALER_DATAINFO;



/*---------------- DIALER_GetLineState() return value ------------------*/

#define DIALER_INVALIDSTAGE		0		// ����״̬
#define DIALER_CONNECTSTAGE		1		// ����״̬
#define DIALER_PPPSTAGE			2		// PPPЭ��
#define DIALER_NETWORKSTAGE		3		// ����״̬
#define DIALER_HANGUPSTAGE		4		// �Ҷ�״̬



/*------------ �����������֪ͨ��Ϣ PostMessage() -------------*/

// ���ŷ�ʽ wParam
#define DIALER_RUNMODE_DATA		0		//���ݲ���
#define DIALER_RUNMODE_GPRS		1		//GPRS��ʽ

//lParam
enum Dialer_Run_MsgNotice {

// Function Return Value, not message
	DIALER_REQ_ACCEPT =0	,		// ���ܵ���
	DIALER_REQ_EXIST		,		// �Ѵ���
	DIALER_REQ_REFUSE		,		// �ܾ�����
	DIALER_MCH_FAILURE		,		// ��ͨ���������ʧ��
	DIALER_INVALID_ISP		,		// ��ЧISP

// Normal Stage Message
	DIALER_RUN_DIALING		,		// Dial Stage
	DIALER_RUN_BUSYRETRY	,		// ReDial when Busy
	DIALER_RUN_LOGINING		,		// Login Stage
	DIALER_RUN_NEGOTIATING	,		// Negotiate Stage
	DIALER_RUN_HANGUPING	,		// Hangup Stage

// Result Message
	DIALER_RUN_NETCONNECT	,
	DIALER_RUN_NETBROKEN	,
	DIALER_RUN_NET_ABNORMAL	,
	DIALER_RUN_NETPAUSE		,
	DIALER_RUN_NETRESUME	,

// Error Code, Use Function:Dialer_GetError()
	DIALER_RUN_DIALFAIL		,		// ����ʧ��
	DIALER_RUN_MODEMDUMB	,		// ���ƽ�����޷�Ӧ
	DIALER_RUN_BUSYFAIL		,		// ռ��
	DIALER_RUN_NODIALTONE	,		// �޲�����
	DIALER_RUN_NOCARRIER	,		// ���ز��ź�

	DIALER_RUN_LOGINFAIL	,		// ��¼ʧ��
	DIALER_RUN_PPPFAIL		,		// PPPЭ��ʧ��
	DIALER_RUN_PAPFAIL		,		// PAPЭ��ʧ��
	DIALER_RUN_HANGUPFAIL	,		// ��·�Ҷ�ʧ��

	DIALER_RUN_SCREXP_ERROR	,		// �ű�����������
	DIALER_RUN_SIGNAL_ERROR	,		// ��·�ź��쳣
	DIALER_RUN_GENERAL_ERROR,		// ģ�������쳣����

	DIALER_RUN_NO_ERROR,

};


/*--------------- �ӿں������� ------------------------*/

int DIALER_Initialize( void );
int DIALER_SelConnect( HWND hWnd, UINT msg, int SelectType, void *pDialInfo );
int DIALER_HangUp( void );
int DIALER_DialCancel( void );
int DIALER_GetLineState( void );
int Dialer_GetError( void );


int DIALER_NotifyRegister( HWND hWnd, int msg );
int DIALER_NotifyUNRegister( HWND hWnd, int msg );
int DIALER_CompareInfo( int SelectType, void *pSrcInfo );


#endif /* _DIAL_DIALER_H_ */

#elif defined WAVECOM_2C


/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 2000-2001 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :
 *
 * Filename:	dialer.h
 *
 * Purpose :	SmartPhone Version_NOWIN
 *
 * Author  :	Jingjing Xu
 *
 * Date    :	2002/8/22
 *-------------------------------------------------------------------------
 *
 * $Archive::                                                       $
 * $Revision::    $     $Date::                                     $
 *
 * $History::                                                       $
 *
\**************************************************************************/


#ifndef _DIAL_DIALER_H_
#define _DIAL_DIALER_H_


/*--- int DIALER_SelConnect( HWND, UINT, int SelectType, void *pDialInfo ); ---*/

// SelectType:
#define		DIALDEF_WAP			1
#define		DIALDEF_MAIL		2
#define		DIALDEF_SELFGPRS	3
#define		DIALDEF_SELFDATA	4

#define DIAL_MAXSTRLEN		31		// ISP���ֵ���󳤶�,��Ҫ��Ӧsetting.h

// when SelectType = DIALDEF_SELFGPRS
typedef struct TAGDIALER_GPRSINFO{
	char	APN[DIAL_MAXSTRLEN +1];			// APN
	char	UserID[DIAL_MAXSTRLEN +1];		// �û�ID
	char	PassWord[DIAL_MAXSTRLEN +1];	// ����

}DIALER_GPRSINFO, *PDIALER_GPRSINFO;

// when SelectType = DIALDEF_SELFDATA
typedef struct TAGDIALER_DATAINFO{
	char	PhoneNum[DIAL_MAXSTRLEN +1];	// �绰����
	char	UserName[DIAL_MAXSTRLEN +1];	// �û���
	char	PassWord[DIAL_MAXSTRLEN +1];	// ����

}DIALER_DATAINFO, *PDIALER_DATAINFO;



/*---------------- DIALER_GetLineState() return value ------------------*/

#define DIALER_INVALIDSTAGE		0		// ����״̬
#define DIALER_CONNECTSTAGE		1		// ����״̬
#define DIALER_PPPSTAGE			2		// PPPЭ��
#define DIALER_NETWORKSTAGE		3		// ����״̬
#define DIALER_HANGUPSTAGE		4		// �Ҷ�״̬



/*------------ �����������֪ͨ��Ϣ PostMessage() -------------*/

// ���ŷ�ʽ wParam
#define DIALER_RUNMODE_DATA		0		//���ݲ���
#define DIALER_RUNMODE_GPRS		1		//GPRS��ʽ

//lParam
enum Dialer_Run_MsgNotice {

// Function Return Value, not message
	DIALER_REQ_ACCEPT =0	,		// ���ܵ���
	DIALER_REQ_EXIST		,		// �Ѵ���
	DIALER_REQ_REFUSE		,		// �ܾ�����
	DIALER_MCH_FAILURE		,		// ��ͨ���������ʧ��
	DIALER_INVALID_ISP		,		// ��ЧISP

// Normal Stage Message
	DIALER_RUN_DIALING		,		// Dial Stage
	DIALER_RUN_BUSYRETRY	,		// ReDial when Busy
	//DIALER_RUN_LOGINING	,		 Login Stage
	DIALER_RUN_NEGOTIATING	,		// Negotiate Stage
	DIALER_RUN_HANGUPING	,		// Hangup Stage
	DIALER_RUN_CANCELING	,		// Try to Stop Dialing

// Result Message
	DIALER_RUN_NETCONNECT	,
	DIALER_RUN_NETBROKEN	,
	DIALER_RUN_NET_ABNORMAL	,
	DIALER_RUN_NETPAUSE		,
	DIALER_RUN_NETRESUME	,

// Error Code, Use Function:Dialer_GetError()
	DIALER_RUN_DIALFAIL		,		// ����ʧ��
	DIALER_RUN_MODEMDUMB	,		// ���ƽ�����޷�Ӧ
	DIALER_RUN_BUSYFAIL		,		// ռ��
	DIALER_RUN_NODIALTONE	,		// �޲�����
	DIALER_RUN_NOCARRIER	,		// ���ز��ź�

	//DIALER_RUN_LOGINFAIL	,		 ��¼ʧ��
	DIALER_RUN_PPPFAIL		,		// PPPЭ��ʧ��
	DIALER_RUN_PAPFAIL		,		// PAPЭ��ʧ��
	DIALER_RUN_HANGUPFAIL	,		// ��·�Ҷ�ʧ��

	DIALER_RUN_USERCANCEL	,		// �û�ȡ������
	DIALER_RUN_SCREXP_ERROR	,		// �ű�����������
	DIALER_RUN_SIGNAL_ERROR	,		// ��·�ź��쳣
	DIALER_RUN_GENERAL_ERROR,		// ģ�������쳣����

	DIALER_RUN_NO_ERROR,

};


/*--------------- �ӿں������� ------------------------*/

int DIALER_Initialize( void );
int DIALER_SelConnect( HWND hWnd, UINT msg, int SelectType, void *pDialInfo );
int DIALER_HangUp( void );
int DIALER_DialCancel( void );
int DIALER_GetLineState( void );
int Dialer_GetError( void );


int DIALER_NotifyRegister( HWND hWnd, int msg );
int DIALER_NotifyUNRegister( HWND hWnd, int msg );
int DIALER_CompareInfo( int SelectType, void *pSrcInfo );

/* For ColorMMS
int MMS_DialerStart( HWND hWnd, UINT msg,  int SelectType, void *pSrcInfo );
int MMS_DialerRecvMessage( HWND hWnd, UINT dialMsg, WPARAM wParam, LPARAM lParam );
*/


#endif /* _DIAL_DIALER_H_ */


#endif