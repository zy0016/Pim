/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :	wap.h
 *
 * Purpose :	implement wap program interface
 *  
 * Author  :	ltzhou
 *
 *-------------------------------------------------------------------------
 *
 * $Archive::                                                       $
 * $Workfile::                                                      $
 * $Revision::    $     $Date::                                     $
 * 
\**************************************************************************/

#ifndef _WAPI_H_
#define _WAPI_H_

/*************************************************************************\
*
*	��������
*
\*************************************************************************/

/* ����WAP��̽ӿڷ���ֵ */
#define WAPI_SUCCESS		0		/* �ɹ� */
#define WAPI_FAILURE		1		/* ʧ�� */
#define WAPI_BLOCKED		2		/* ���� */
#define WAPI_PARAMETER		3
#define WAPI_RESOURCE		4
#define WAPI_INVALID		5
#define WAPI_DATA			6
#define WAPI_NOTCONNECTED	7
#define WAPI_NOTSUSPENDED	8
#define WAPI_NOTINIT		9
#define WAPI_CONNECTED		10

/* ��Ϣ����LOWORD(lParam) */
#define MSG_CONNECTED		1		/* ���� */
#define MSG_DISCONNECT		2		/* �Ͽ� */
#define MSG_REDIRECT		3		/* ת�� */
#define MSG_SUSPEND			4		/* ���� */
#define MSG_RESUME			5		/* �ָ� */
#define MSG_PUSHDATA		6		/* PUSH */
#define MSG_RECV_URL		7		/* ����URL���� */

/* ����ԭ�� */
#define WAPI_Continue				0x10
#define WAPESwitching				0x11
#define WAPEOK						0x20
#define WAPECreated					0x21
#define WAPEAccepted				0x22
#define WAPENonAuthoritative		0x23
#define WAPENoContent				0x24
#define WAPEReset					0x25
#define WAPEPartialContent			0x26
#define WAPEMultipleChoices			0x30
#define WAPEMovedPermanently		0x31
#define WAPEMovedtemporarily		0x32
#define WAPESeeOther				0x33
#define WAPENotmodified				0x34
#define WAPEUseProxy				0x35
#define WAPEBadRequest				0x40
#define WAPEUnauthorized			0x41
#define WAPEPaymentrequired			0x42
#define WAPEForbidden				0x43
#define WAPENotFound				0x44
#define WAPEMethodnotallowed		0x45
#define WAPENotAcceptable			0x46
#define WAPEProxyAuthenticationrequired	0x47
#define WAPERequestTimeout			0x48
#define WAPEConflict				0x49
#define WAPEGone					0x4A
#define WAPELengthRequired			0x4B
#define WAPEPreconditionfailed		0x4C
#define WAPERequestedentitytoolarge	0x4D
#define WAPERequestURItoolarge		0x4E
#define WAPEUnsupportedmediatype	0x4F
#define WAPEInternalServerError		0x60
#define WAPENotImplemented			0x61
#define WAPEBadGateway				0x62
#define WAPEServiceUnavailable		0x63
#define WAPEGatewayTimeout			0x64
#define WAPEHTTPversionnotsupported	0x65

#define WAPEPROTOERR			0xe0	//Զ��Э�����	
#define WAPEDISCONNECT			0xe1	//��������
#define WAPESUSPEND				0xe2	//�������
#define WAPERESUME				0xe3	//�ָ�����
#define WAPECONGESTION			0xe4	//Զ����Դ����
#define WAPECONNECTERR			0xe5	//�Ự��������
#define WAPEMRUEXCEEDED			0xe6	//���ݰ���С����
#define WAPEMOREXCEEDED			0xe7	//Э�̷���������������
#define WAPEPEERREQ				0xe8	//Զ������
#define WAPENETERR				0xe9	//�������
#define WAPEUSERREQ				0xea	//�û�����
#define WAPEREDIRECT			0xeb	//�ض���
#define WAPEREPLY				0xec

/* ������������ź͵�ַ���� */
#define WAP_IPv4					0x00
#define WAP_IPv6					0x01
#define WAP_GSM_USSD				0x02
#define WAP_GSM_SMS					0x03
#define WAP_IS_136_R_Data			0x04
#define WAP_IS_637_CDMA_SMS			0x05

/* WAPĬ�϶˿� */
#define WSP_Datagram_PORT				9200	//WAP connectionless session service 
#define WSP_WTLS_Datagram_PORT			9202	//WAP secure connectionless session service
#define WSP_WTP_Datagram_PORT			9201	//WAP session service
#define WSP_WTP_WTLS_Datagram_PORT		9203	//WAP secure session service 
#define vCard_Datagram_PORT				9204	//WAP vCard 
#define vCard_WTLS_Datagram_PORT		9206	//WAP vCard Secure 
#define vCalendar_Datagram_PORT			9205	//WAP vCal 
#define vCalendar_WTLS_Datagram_PORT	9207	//WAP vCal Secure 

/* WAP Method define */
#define WAP_METHOD_GET        1
#define WAP_METHOD_POST       2

/*************************************************************************\
*
*	���ݽṹ���������Ͷ���
*
\*************************************************************************/
/* �ض����ַ��󳤶� */
#define RDADDR_MAXADDRLEN	32		/* �ض����ַ��󳤶� */		
/* �ض���ԭ�� */
#define WAP_PERREDIRECT			0x80	//�����ض���
#define WAP_REUSESECURITY		0x40	//���ð�ȫ�Ự

/* ���ݽṹ */
typedef struct tagWapData
{
	char* pData;
	int len;
}WapData;

/* �ض���ṹ */
typedef struct tagWapReDirect
{
	int rdReason;		//�ض���ԭ��
	int ifBearNet;		//�������������Ƿ���Ч��
	int ifRDPort;		//�Ƿ��ض���˿ڡ�
	char addrlen;		//��ַ����
	char BearNet;		//������������
	unsigned short Port;	//�ض���˿ںš�
	char RDAddr[RDADDR_MAXADDRLEN];//�ض����ַ��	
}WapReDirect;

/* ˵�������ṹ������һ��WSP������¼�ṹ */
#define MAX_EXMTNUM			10
#define MAX_CODEPAGENUM		10
#define WSP_DEFCSDU			2 * 1024		//byte	
#define WSP_DEFMAXMNUM		10
#define WSP_DEFMAXPNUM		10
#define WSP_DEFSSDU			2 * 1024		//byte
#define MAX_PROOPTIONNUM	10

#define WSP_MAXCAPALEN		1024			//byte

#define WSP_CAPACSUD		0x00			//capability Client SDU Size
#define WSP_CAPASSUD		0x01			//capability Server SDU Size
#define WSP_CAPAPOPTION		0x02			//capability Protocol Option
#define WSP_CAPAMMOR		0x03			//capability Method MOR
#define WSP_CAPAPMOR		0x04			//capability Push MOR
#define WSP_CAPAEMETHOD		0x05			//capability Extended Method
#define WSP_CAPAHCODEPAGE	0x06			//capability Header Code Page
#define WSP_CAPAALIASES		0x07			//capability Aliases

/* WSP����Э�̽ṹ */
typedef struct tagWspCapability
{
	void** Aliases;						//��������ַ�ṹ����
	int CLientSDU;						//�ͻ���������ݳ��ȡ�
	int ExMethod[MAX_EXMTNUM];			//��չ������
	int HCPage[MAX_CODEPAGENUM];		//ͷ������ҳ��
	int Momr;							//��󷽷�������
	int Mopr;							//�����������
	int ProtoOption[MAX_PROOPTIONNUM];	//Э��ѡ�
	int ServerSDU;						//�����������ݳ��ȡ�
}WspCapability;

#define WAPI_PUSHIDANYAPP	"x-wap-application:*"			//Any Application 
#define WAPI_PUSHIDSIAAPP	"x-wap-application:push.sia"	//WAP Push SIA 
#define WAPI_PUSHIDWMLAPP	"x-wap-application:wml.ua"		//WML User Agent 
#define WAPI_PUSHIDWTAAPP	"x-wap-application:wta.ua"		//WTA User Agent 
//This ID will used for application dispatching to MMS User 
//Agent in the handling of MMS notfication using WAP Push. 
//See WAP-206-MMSCTR for more detail. 
#define WAPI_PUSHIDMMSAPP	"x-wap-application:mms.ua"		
//SyncML PUSH Application ID: used to push a SyncML Alert 
//from a SyncML server side. The SyncML Alert is an indication 
//for starting a SyncML session e.g., for data synchronization. 
//Requested by the WAP WAG Synchronisation Drafting Committee. 
#define WAPI_PUSHIDSYNCMLAPP	"x-wap-application:push.syncml"		
//This ID is used for application dispatching to Location User 
//Agent in the handling of Location Invocation document. 
//See WAP-257-LOCPROT for details. Requested by the WAP WAG Location 
//Drafting Committee.  
#define WAPI_PUSHIDLOCAPP	"x-wap-application:loc.ua"
//This ID is used for SyncML Device Management. Requested by the SyncML 
//Device Management Expert Group.  
#define WAPI_PUSHIDSYNCDMAPP "x-wap-application:syncml.dm"
//This ID is used for DRM User Agent. Requested by the WAP WAG Download DC.  
#define WAPI_PUSHIDDRMAPP	"x-wap-application:drm.ua"
//This ID is used for Email Notification (EMN) User Agent. 
//Requested by the WAP WAG PUSH DC.  
#define WAPI_PUSHIDEMNAPP	"x-wap-application:emn.ua"
//This ID is used for Wireless Village (EMN) User Agent. 
//Requested by Wireless Village.  
#define WAPI_PUSHIDWVAPP	"x-wap-application:wv.ua"

/* WAPI��ѡ������ */
#define WAPI_FIONBIO	1

/* ����ʱ����ѡ�� */
#define CONNECT_LOW		1
#define CONNECT_NORMAL	2
#define CONNECT_HIGHT	3

/*************************************************************************\
*
*	��������
*
\*************************************************************************/
/* WAP API */
int WAPI_Instance(HWND hwnd, int mesg);
int WAPI_Connect(char* gateway, unsigned short port, int SDUSize);
int WAPI_Connect_Open(char* gateway, unsigned short port, WapData* pHeader, WapData* pCapa, int ifSec );
int WAPI_Disconnect(void);
int WAPI_Suspend(void);
int WAPI_Resume(void);

int WAPI_GetPUSHHeader(int push_handle, char* buf, int buflen);
int WAPI_GetPUSHHeaderSize(int push_handle);
int WAPI_GetPUSHBody(int push_handle, char* buf, int buflen);
int WAPI_GetPUSHBodySize(int push_handle);
int WAPI_ClosePush(int Push_handle);
/* WSP/B API */
//�������ӵ�URL����
int WAPI_RequestURL(char* URL, char method, 
					char* head, int headlen,
					char* body, int bodylen);	/* ���ط�0��� */

//�����ӵ�URL����
int WAPI_RequestURLto(char* gateway, unsigned short port, 
					char* URL, char method, 
					char* head, int headlen,
					char* body, int bodylen);	/* ���ط�0��� */

int WAPI_GetURLHeader(int URL_handle, char* buf, int buflen);
int WAPI_GetURLHeaderSize(int URL_handle);
int WAPI_GetURLBody(int URL_handle, char* buf, int buflen);
int WAPI_GetURLBodySize(int URL_handle);
int WAPI_CloseURLRequest(int URL_handle);
int WAPI_PushResister( HWND hWnd, int Msg, char* AppID );
int WAPI_PushUnResister( HWND hWnd, char* AppID );

/* ����ʱ����ѡ�� */
#define CONNECT_LOW		1
#define CONNECT_NORMAL	2
#define CONNECT_HIGHT	3

/* ������������ */
int WAPA_Startup(void);
int WAPA_Connect(char *Gateway, unsigned short Port, int SDUSize, 
				  HWND hWnd, int Msg, int Priority);
int WAPA_Connect_Open(char *Gateway, unsigned short Port, WapData *pHeader, 
					  WapData *pCapa, int ifSec, HWND hWnd, int Msg, int Priority);
int WAPA_Disconnect(HWND hWnd, int Msg);
int WAPA_Suspend(void);
int WAPA_Resume(char *gateway, unsigned short port);

int WAPA_RequestURL(char *URL, char method, char *head, int headlen,
					char *body, int bodylen, HWND hWnd, int Msg);
int WAPA_RequestURLto(char *gateway, unsigned short port, char *URL,
					 char method, char *head, int headlen, char *body,
					 int bodylen, HWND hWnd, int Msg);
int WAPA_GetURLHeader(int URL_handle, char* buf, int buflen);
int WAPA_GetURLHeaderSize(int URL_handle);
int WAPA_GetURLBody(int URL_handle, char* buf, int buflen);
int WAPA_GetURLBodySize(int URL_handle);
int WAPA_CloseURLRequest(int URL_handle);

int WAPA_PushResister( HWND hWnd, int Msg, char* AppID );
int WAPA_PushUnResister( HWND hWnd, char* AppID );
int WAPA_GetPUSHHeader(int push_handle, char* buf, int buflen);
int WAPA_GetPUSHHeaderSize(int push_handle);
int WAPA_GetPUSHBody(int push_handle, char* buf, int buflen);
int WAPA_GetPUSHBodySize(int push_handle);
int WAPA_ClosePush(int Push_handle);
/************* End Of Head File ******************************************/
#endif	/* _WAPI_H_ */