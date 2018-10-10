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
*	常量定义
*
\*************************************************************************/

/* 定义WAP编程接口返回值 */
#define WAPI_SUCCESS		0		/* 成功 */
#define WAPI_FAILURE		1		/* 失败 */
#define WAPI_BLOCKED		2		/* 阻塞 */
#define WAPI_PARAMETER		3
#define WAPI_RESOURCE		4
#define WAPI_INVALID		5
#define WAPI_DATA			6
#define WAPI_NOTCONNECTED	7
#define WAPI_NOTSUSPENDED	8
#define WAPI_NOTINIT		9
#define WAPI_CONNECTED		10

/* 消息定义LOWORD(lParam) */
#define MSG_CONNECTED		1		/* 连接 */
#define MSG_DISCONNECT		2		/* 断开 */
#define MSG_REDIRECT		3		/* 转向 */
#define MSG_SUSPEND			4		/* 挂起 */
#define MSG_RESUME			5		/* 恢复 */
#define MSG_PUSHDATA		6		/* PUSH */
#define MSG_RECV_URL		7		/* 接收URL数据 */

/* 错误原因 */
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

#define WAPEPROTOERR			0xe0	//远方协议错误	
#define WAPEDISCONNECT			0xe1	//断联错误
#define WAPESUSPEND				0xe2	//挂起错误
#define WAPERESUME				0xe3	//恢复错误
#define WAPECONGESTION			0xe4	//远方资源不足
#define WAPECONNECTERR			0xe5	//会话创建错误
#define WAPEMRUEXCEEDED			0xe6	//数据包大小错误
#define WAPEMOREXCEEDED			0xe7	//协商方法或推数量错误
#define WAPEPEERREQ				0xe8	//远方错误
#define WAPENETERR				0xe9	//网络错误
#define WAPEUSERREQ				0xea	//用户错误
#define WAPEREDIRECT			0xeb	//重定向
#define WAPEREPLY				0xec

/* 定义承载网络标号和地址长度 */
#define WAP_IPv4					0x00
#define WAP_IPv6					0x01
#define WAP_GSM_USSD				0x02
#define WAP_GSM_SMS					0x03
#define WAP_IS_136_R_Data			0x04
#define WAP_IS_637_CDMA_SMS			0x05

/* WAP默认端口 */
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
*	数据结构及数据类型定义
*
\*************************************************************************/
/* 重定向地址最大长度 */
#define RDADDR_MAXADDRLEN	32		/* 重定向地址最大长度 */		
/* 重定向原因 */
#define WAP_PERREDIRECT			0x80	//永久重定向
#define WAP_REUSESECURITY		0x40	//重用安全会话

/* 数据结构 */
typedef struct tagWapData
{
	char* pData;
	int len;
}WapData;

/* 重定向结构 */
typedef struct tagWapReDirect
{
	int rdReason;		//重定向原因
	int ifBearNet;		//承载网络类型是否有效。
	int ifRDPort;		//是否重定向端口。
	char addrlen;		//地址长度
	char BearNet;		//承载网络类型
	unsigned short Port;	//重定向端口号。
	char RDAddr[RDADDR_MAXADDRLEN];//重定向地址。	
}WapReDirect;

/* 说明：本结构定义了一个WSP能力记录结构 */
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

/* WSP能力协商结构 */
typedef struct tagWspCapability
{
	void** Aliases;						//别名（地址结构）。
	int CLientSDU;						//客户端最大数据长度。
	int ExMethod[MAX_EXMTNUM];			//扩展方法。
	int HCPage[MAX_CODEPAGENUM];		//头部代码页。
	int Momr;							//最大方法数量。
	int Mopr;							//最大推数量。
	int ProtoOption[MAX_PROOPTIONNUM];	//协议选项。
	int ServerSDU;						//服务端最大数据长度。
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

/* WAPI的选项设置 */
#define WAPI_FIONBIO	1

/* 连接时的优选级 */
#define CONNECT_LOW		1
#define CONNECT_NORMAL	2
#define CONNECT_HIGHT	3

/*************************************************************************\
*
*	函数声明
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
//面向连接的URL请求
int WAPI_RequestURL(char* URL, char method, 
					char* head, int headlen,
					char* body, int bodylen);	/* 返回非0句柄 */

//无连接的URL请求
int WAPI_RequestURLto(char* gateway, unsigned short port, 
					char* URL, char method, 
					char* head, int headlen,
					char* body, int bodylen);	/* 返回非0句柄 */

int WAPI_GetURLHeader(int URL_handle, char* buf, int buflen);
int WAPI_GetURLHeaderSize(int URL_handle);
int WAPI_GetURLBody(int URL_handle, char* buf, int buflen);
int WAPI_GetURLBodySize(int URL_handle);
int WAPI_CloseURLRequest(int URL_handle);
int WAPI_PushResister( HWND hWnd, int Msg, char* AppID );
int WAPI_PushUnResister( HWND hWnd, char* AppID );

/* 连接时的优选级 */
#define CONNECT_LOW		1
#define CONNECT_NORMAL	2
#define CONNECT_HIGHT	3

/* 公开函数声明 */
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