/**************************************************************************\
*
*                      Hopen Software System
*
* Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
*                       All Rights Reserved
*
* Model   :		mmsuser.h
*
* Purpose :		MMS ���������ݽṹ���弰��������
*  
* Author  :		hyzhang
*
*-------------------------------------------------------------------------
*
* $Archive::                                                       $
* $Workfile::                                                      $
* $Revision::    $     $Date::                                     $
* 
\**************************************************************************/

#ifndef MMS_USER_H_
#define MMS_USER_H_

/**************************************************************
 *		 MMS �ĺ�������ֵ, ��Ӧ�ó����͵���Ϣ��������
 **************************************************************/

/* �����������Ǻ�������ֵҲ��Ϊ��Ϣ�Ĵ����� */
#define MMS_SUCCESS		0	/* �ɹ�		*/
#define MMS_FAILURE		-1	/* ʧ��		*/
#define MMS_BLOCKED		-2	/* ����		*/

/* ��������� */
#define MMS_NOT_INIT	-6	/* û�е��ó�ʼ������	*/
#define MMS_NOT_PROXY	-7	/* û�����ô���			*/
#define MMS_NOT_CONF	-8	/* ���͵���Ϣû�еõ��Է���ȷ��	*/
#define MMS_NOT_HANDLE	-9	/* ������Ч�� MMS ����	*/
#define MMS_INPROGRESS	-10	/* �������ڽ�����		*/
#define MMS_VALUE_ERR	-11	/* ��������				*/
#define MMS_BUFFER_ERR	-12	/* ����������			*/
#define MMS_NOT_CONNECT	-13	/* û�����ӵ�����		*/
#define MMS_SYS_ERROR	-14	/* ����ϵͳ��������		*/
#define MMS_NOT_SUPPORT	-15	/* ��֧�ִ�ѡ��			*/

/* HIWORD(lParam), �ײ����ԭ��, Ϊ������WAP��ͻ, ��Ӧֵ�Ѹ��� */
#define MMS_Continue				0x10
#define MMS_ESwitching				0x11
#define MMS_EOK						0x20
#define MMS_ECreated				0x21
#define MMS_EAccepted				0x22
#define MMS_ENonAuthoritative		0x23
#define MMS_ENoContent				0x24
#define MMS_EReset					0x25
#define MMS_EPartialContent			0x26
#define MMS_EMultipleChoices		0x30
#define MMS_EMovedPermanently		0x31
#define MMS_EMovedtemporarily		0x32
#define MMS_ESeeOther				0x33
#define MMS_ENotmodified			0x34
#define MMS_EUseProxy				0x35
#define MMS_EBadRequest				0x40
#define MMS_EUnauthorized			0x41
#define MMS_EPaymentrequired		0x42
#define MMS_EForbidden				0x43
#define MMS_ENotFound				0x44
#define MMS_EMethodnotallowed		0x45
#define MMS_ENotAcceptable			0x46
#define MMS_EProxyAuthenticationrequired	0x47
#define MMS_ERequestTimeout			0x48
#define MMS_EConflict				0x49
#define MMS_EGone					0x4A
#define MMS_ELengthRequired			0x4B
#define MMS_EPreconditionfailed		0x4C
#define MMS_ERequestedentitytoolarge	0x4D
#define MMS_ERequestURItoolarge		0x4E
#define MMS_EUnsupportedmediatype	0x4F
#define MMS_EInternalServerError	0x60
#define MMS_ENotImplemented			0x61
#define MMS_EBadGateway				0x62
#define MMS_EServiceUnavailable		0x63
#define MMS_EGatewayTimeout			0x64
#define MMS_EHTTPversionnotsupported	0x65

#define MMS_EPROTOERR				0xe0	//Զ��Э�����	
#define MMS_EDISCONNECT				0xe1	//��������
#define MMS_ESUSPEND				0xe2	//�������
#define MMS_ERESUME					0xe3	//�ָ�����
#define MMS_ECONGESTION				0xe4	//Զ����Դ����
#define MMS_ECONNECTERR				0xe5	//�Ự��������
#define MMS_EMRUEXCEEDED			0xe6	//���ݰ���С����
#define MMS_EMOREXCEEDED			0xe7	//Э�̷���������������
#define MMS_EPEERREQ				0xe8	//Զ������
#define MMS_ENETERR					0xe9	//�������
#define MMS_EUSERREQ				0xea	//�û�����
#define MMS_EREDIRECT				0xeb	//�ض���
#define MMS_EREPLY					0xec

/* ��Ϣ����: LOWORD(lParam) */
#define MMS_CONNECT		1	/* ���м̴����������� */
#define MMS_SENDED		2	/* MMS ��Ϣ�����Ƿ�ɹ�	*/
#define MMS_ACCEPT		3	/* �� MMS ��Ϣ����		*/
#define MMS_READ		4	/* �����ݵ���			*/

#define MMS_SUSPEND		8	/* ������Ϣ				*/
#define MMS_RESUME		9	/* �ָ���Ϣ				*/
#define MMS_REDIRECT	10	/* �ض�����Ϣ			*/
#define MMS_DISCONNECT	11	/* �Է��Ͽ�����			*/

/**************************************************************
 *					 ���úͻ�ȡ MMS ѡ��ĳ�������
 **************************************************************/
/* ȫ��ѡ�� */
#define MMS_FIONBIO		1	/* ����MMS�����ӷ�ʽ */

/* ѡ��Option */
#define MMS_HEAD		2	/* ����MMS����Ϣͷ	*/
#define MMS_BODY		3	/* ����MMS����Ϣ��	*/

#define MMS_PROXYURI	4	/* ���ô����uri	*/
#define MMS_REPORT		5	/* �Ƿ����Ͷ�ݱ���	*/
#define MMS_TRANSID		6	/* �õ�Transaction-ID*/
#define MMS_RESSTATUS	7	/* �õ���Ӧ״̬		*/
#define MMS_MSGID		8	/* �õ�Message-ID*/

/**************************************************************
 *					 MMS �Ĵ򿪷�ʽ
 **************************************************************/
#define MMS_SEND		1
#define MMS_RECV		2

 /**************************************************************
 *					 ���ݽṹ����
 **************************************************************/
typedef struct tagMMSBody
{
	char* pHead;
	int headlen;
	char* pBody;
	int bodylen;
}MMSBody;

typedef struct tagMMSMultiBody
{
	int Bodynum;
	MMSBody body[1];
}MMSMultiBody;

/**************************************************************
 *				MMS �ṩ�Ĺ�����������
 **************************************************************/
int MMS_Startup(HWND hWnd, int Message);
int MMS_Connect(char *GateWay, unsigned short Port);
int MMS_Open(int Protocol, int Mode);
int MMS_Accept(int Handle, char *pBuf, int *BufLen, int *bDelivered);
int MMS_Send(int Handle, char *pHead, int HeadLen, char *pBody, int BodyLen);
int MMS_Recv(int Handle, char *pBuf, int BufLen, int bDelay);
int MMS_RecvDelay(int Handle, char *pBuf, int BufLen);
int MMS_GetData(int Handle, char *pHead, int *HeadLen, char *pBody, int *BodyLen);
int MMS_SetOption(int Handle, int Option, void *pValue, int Length);
int MMS_GetOption(int Handle, int Option, void *pValue, int Length);
int MMS_Disconnect(void);
int MMS_Close(int Handle);
int MMS_Destroy(void);
int MMS_GetLastError(void);

int MMS_PackHeadersInBody(char *pData, int DataLen, char *pBuf, int BufLen);
int MMS_PackMulitBody(MMSMultiBody *pBody, char *pData, int DataLen);
int MMS_LibParseMulitBody( char* pData, int datalen, MMSMultiBody* pBody );
int MMS_LibParseBodyHead(char *pData, int DataLen, char *pBuf, int BufLen);

#endif	/* MMS_USER_H_ */