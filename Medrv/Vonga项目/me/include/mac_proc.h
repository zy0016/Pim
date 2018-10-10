
/**************************************************************************\
 *
 *                      Hopen TCP/IP System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 *
 * Filename		mac_proc.h
 *
 * Purpose      ����NDIS�����
 *  
 * Author       ZhouLinTao
 *
 *-------------------------------------------------------------------------
 *
 * $Archive:: /tcpipsec/mac/mac_proc.h                              $
 * $Revision:: 5  $     $Date:: 01-12-28 11:22                      $
 *
 * $History:: mac_proc.h                                            $
 * 
 * *****************  Version 5  *****************
 * User: Zhaozheng    Date: 01-12-28   Time: 11:22
 * Updated in $/tcpipsec/mac
 * 
 * *****************  Version 4  *****************
 * User: Zhaozheng    Date: 01-08-31   Time: 11:04
 * Updated in $/tcpipsec/mac
 * 
 * *****************  Version 2  *****************
 * User: Zhaozheng    Date: 01-03-20   Time: 11:50
 * Updated in $/tcpipsec/mac
 * 
 * *****************  Version 1  *****************
 * User: Zhaozheng    Date: 00-12-19   Time: 16:28
 * Created in $/tcpipsec/mac
 * 
 * *****************  Version 3  *****************
 * User: Zhaozheng    Date: 00-06-16   Time: 14:25
 * Updated in $/tcpipsec/dial
 * 
 * *****************  Version 2  *****************
 * User: Zhaozheng    Date: 00-06-01   Time: 8:49
 * Updated in $/tcpipsec/dial
 * 
\**************************************************************************/

/*************************************************************************\
*
*	��������: NDIS�������Ƴ���
*
*	�ļ�����: mac_proc.h
*
*	��������: ������
*
*	������: modem���������紫�䡢����ͨ��
*
\*************************************************************************/

#ifndef _MAC_PROC_H_
#define _MAC_PROC_H_

#include "DI_Uart.h"

/*************************************************************************\
*
*	��������
*
\*************************************************************************/

/* ������Ϣ���� */
#define EXPLAIN_END			1		/* �ű��������		*/
#define NDIS_ADAPTER		2		/* NDIS�豸����		*/
#define PPP_NEGOTIATE		3		/* PPPЭ�̽��		*/
#define LINE_EVENT			4		/* ��·�¼�			*/
#define MAC_CLOSED			5		/* MAC�豸�ɹ��ر�  */
#define SCR_CLOSED			(MAC_CLOSED)

/* ����NDIS�豸���� */
#define ADPTOR_OPEN			6		/* NDIS OpenAdpter	*/
#define ADPTOR_CLOSE		7		/* NDIS CloseAdpter	*/

/* ����PPPЭ�̽�� */
#define PPP_SUCCESS			8		/* PPPЭ�̳ɹ�		*/
#define PPP_FAILURE			9		/* PPPЭ��ʧ��		*/
#define PAP_FAILURE			10		/* PAPЭ��ʧ��		*/
#define PPP_RE_NEGO			16		/* PPP����Э��		*/

/* ������·�¼� */
#define LINE_UP				11		/* ��·��ͨ			*/
#define LINE_DOWN			12		/* ��·����			*/
#define LINE_DATAIN			13		/* �����ݵ���		*/
#define LINE_DATAOUT		14		/* ���Է�������		*/
#define LINE_STATE			15		/* ��·״̬			*/
#define LINE_POLLING		17		/* ��ѯ�����·		*/
#define LINE_RING			18		/* �����ź�			*/

/* ����ű�����Ŀ�� */
#define SCR_START			1		/* ��ʼ�ű�����		*/
#define SCR_STOP			2		/* ��ֹ�ű�����		*/

/* ��������������� */
#define DATA_ACTIVE			1		/* �����ݴ���		*/
#define DATA_SILENCE		0		/* �����ݴ���		*/

/* ����ѡ�� */
#define OPT_ISPNAME			2		/* ISP ����	*/
#define OPT_PHONENUM		3		/* �绰����	*/
#define OPT_USERNAME		4		/* �û��ʺ� */
#define OPT_PASSWORD		5		/* �û����� */
#define OPT_TRANSRATE		6		/* �������� */
#define OPT_CALLSTATE		7		/* �ű�����״̬ */
#define OPT_DIALMODE		8		/* �ű����ŷ�ʽ */
#define OPT_DNS1			9		/* ��������������ַ	*/
#define OPT_DNS2			10		/* ��������������ַ	*/

#ifndef _TCPIP_NOWIN_
#define OPT_CALLWND			11		/* ��Ϣ�������� */
#define OPT_CALLMSG			12		/* ����������Ϣ */
#else
#define OPT_CALLBACK		11		/* �ص�����     */
#endif

#define OPT_TESTDATA		13		/* ����������� */
#define OPT_SCRIPTNAME		14		/* �ű��ļ����� */
#define OPT_SELDIALTONE		15		/* ��鲦����   */
#define OPT_PHONEBOOK		16		/* �绰������   */
#define OPT_CONTEXT			17		/* SMS������Ϣ  */
#define OPT_INTEGERVALUE	18		/* ��������		*/
#define OPT_STRINGVALUE		19		/* �ִ�����		*/
#define OPT_COMCONFIG		20		/* ��������		*/
#define OPT_SWITCH_MODE		21		/* ������ģʽת��ΪATģʽ */


/* ���巵��ֵ */
#define	RTN_SUCCESS				0		/* �ɹ� */
#define RTN_FAILURE				(-1)	/* ʧ�� */
#define ERR_MAC_OCCUPIED		(-2)	/* û�п���MAC */
#define ERR_INVALID_COM_PORT	(-3)	/* ������Ч */
#define ERR_INVALID_NDIS_MAC	(-4)	/* NDIS��Ч */
#define ERR_INVALID_LINE		(-6)	/* ��·��Ч */
#define RTN_WOULDBLOCK			(-7)	/* ����		*/
#define ERR_LINE_CONNECTED		(-8)	/* ��·��Ȼ��ͨ */

#define MAC_MAXSTRINGLEN		52		/* �ַ�������󳤶�	*/
#define MAX_QUEUE_LEN			30		/* �¼����г��� */
#define MAX_DEVICE_NUM			1		/* �����豸���� */

#define SCR_USERSTOP			1000	/* �û���ֹ�ű�����ֵ */
#define SCR_ERRORCODE			SCR_USERSTOP
#define SCR_INVALID_VERSION		(SCR_USERSTOP + 1)		/* �汾��Ϣ���� */

/* ���廽��ԭ����� */
#define SCRIPT_TIMEOUT			1		/* �ű����ͳ�ʱ	*/
#define SCRIPT_DATAIN			2		/* �ű����ݵ��� */
#define SCRIPT_DATAOUT			3		/* �ű����ݿ�д */
#define SCRIPT_STOP				4		/* �ű�������ֹ */
#define SCRIPT_START			5		/* �ű��������� */

/* ����GSM AT Script Explain֧�ֵĲ������� */
#define CONTEXT_LEN				360
#define INT_VAR_NUM				5
#define STR_VAR_NUM				5

/* ����ѡ������� */
#define MAKEOPTION(opt, id)		((id << 16) | opt & 0xffff)

/*************************************************************************\
*
*	���ݽṹ���������Ͷ���
*
\*************************************************************************/

/* ����ͨ������ԭ��������� */
typedef int (*COM_ENTRY)(int, int, int, int, long);

/* �����ı����ݽṹ */
typedef struct _Textent
{
	int LineLength;
	int LineNumber;
	char** TextPaper;
}	TEXTENT;

/* ����ϵͳ��Ϣ�ṹ */
typedef struct _SysInfo
{	
	int			MAC_Task;			/* MAC�첽�����߳�  */
	int			SexpTask;			/* �ű������߳�ID��	*/
	OSHANDLE	MAC_Event;			/* MAC�ȴ��¼�		*/
	OSHANDLE	SexpEvent;			/* �ű������¼�		*/
	
#if (_HOPEN_VER >= 200)
	MUTEX*		MutexLock;			/* MAC������		*/
	MUTEX		tempMutexLock;
#else
	OSHANDLE	MutexLock;			/* MAC������		*/
#endif
	COM_ENTRY	Request;			/* ͨ������ԭ����� */

}	SYSINFO;

/* �����¼����нṹ */
typedef struct EventQueue
{
	char	item[MAX_QUEUE_LEN];	/* �¼����� */
	char	head;					/* ����ͷ�� */
	char	tail;					/* ����β�� */

}	EVENTQUEUE;

/* ���岦����Ϣ�ṹ */
typedef struct _MacInfo
{
#ifndef _TCPIP_NOWIN_
	HWND	call_wnd;		/* ��Ϣ�ص����� */
	int		call_msg;		/* ���ڻص���Ϣ */
#else
	int		(*callback)(short, long);
#endif
	char	state;			/* ״̬			*/
	char	linestate;		/* ����״̬		*/
	int		dev_handle;		/* ���ھ��		*/
	int		retval;			/* �ű�����ֵ	*/
	char	callstate;		/* ��������		*/
	char	bTone;			/* ��Ƶ����		*/
	char	CheckTone;		/* �Ƿ��Ⲧ���� */
	char	devicename	[MAC_MAXSTRINGLEN + 1];	/* �����豸���� */	
	char	scriptname	[MAC_MAXSTRINGLEN + 1];	/* �ű��ļ����� */
	char	ISPname		[MAC_MAXSTRINGLEN + 1];	/* ISP ����		*/
	char	phonenum	[MAC_MAXSTRINGLEN + 1];	/* �绰����		*/
	char	username	[MAC_MAXSTRINGLEN + 1];	/* �û���		*/
	char	password	[MAC_MAXSTRINGLEN + 1];	/* �û�����		*/
	char	tranrate	[MAC_MAXSTRINGLEN + 1];	/* ��������		*/
	char	dns1		[MAC_MAXSTRINGLEN + 1];	/* ������������	*/
	char	dns2		[MAC_MAXSTRINGLEN + 1];	/* ������������	*/

}	MACINFO;

/* ����ű������������ݽ����� */
typedef struct _DataInfo
{
	char* PhoneBook;
	int	  LineLength;
	int   LineNumber;
	char  Context  [CONTEXT_LEN + 1];
	short IntVarArr[INT_VAR_NUM];
	char  StrVarArr[STR_VAR_NUM][MAC_MAXSTRINGLEN + 1];
}	DATAINFO;

/* ����MAC���ƽṹ */
typedef struct _MacCtrlBlock
{
#ifndef _TCPIP_NOWIN_
	HWND		DialWnd;		/* ����Ӧ�ô��ھ�� */
	int			DialMsg;		/* ����Ӧ�ô�����Ϣ */
#else
	int			(*callback)(short, long);
#endif
	int			OpenAdaptorTag;	/* NDIS OpenAdpter��� */
	SYSINFO		SysInfo;		/* ϵͳ��Ϣ */
	MACINFO		MacInfo[MAX_DEVICE_NUM];	/* MAC���� */
	DATAINFO	DataInfo;
	EVENTQUEUE	UartQueue;		/* �ж��¼����� */
	EVENTQUEUE	MesgQueue;		/* �첽�¼����� */
		
}	MACCTRLBLOCK;


/*************************************************************************\
*
*	��������
*
\*************************************************************************/

/* ϵͳ�ӿں��� */
int MAC_Initial	(void);					/* for system task : socklib.c */
int MAC_Destroy	(void);					/* for system task */
#ifndef _TCPIP_NOWIN_
int MAC_Instance(HWND, int);			/* for dial application */
#else
int MAC_Instance(int (*)(short, long));	/* for dial application */
#endif
int MAC_AbortNetDial(void);				/* for dial application */

/* �ⲿ�ӿں��� */
int MAC_Open		(char*);
int MAC_Close		(int);
int MAC_SetOption	(int, unsigned, int, int);
int MAC_GetOption	(int, unsigned, char*, int);
int MAC_ScriptExplain(int, int);
int MAC_StopExplain	(void);
int MAC_StartNetwork(int);
int MAC_StopNetwork	(int);
int MAC_Read		(int, char*, int);
int MAC_Write		(int, char*, int);

/* �ű�����������ӿ� */
int SCR_Open(int devhandle);
int SCR_Close(int handle);
int SCR_ScriptExplain(int handle, int call);
int SCR_SetOption(int handle, unsigned OptionID, int value, int len);
int SCR_GetOption(int handle, unsigned OptionID, char* buf, int buflen);
int SCR_ScriptDatain(int handle);
int SCR_ScriptDataout(int handle);

/* �ڲ��ӿں��� */
int MAC_GetDns		(char*, char*, int);	/* socklib.c */
int MAC_AppendEvent	(int);

/************* End Of Head File ******************************************/
#endif	/* _MAC_PROC_H_ */
