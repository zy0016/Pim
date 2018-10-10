#ifndef _MASHARE_H_
#define _MASHARE_H_


#include"SPShare.h"


/*�������ݵ���󳤶�*/
#define MAXRECVDATALEN		40 * 1024

/*��ʱ����*/
#define GNS_TIME			10 * 60 * 1000		/*ȡ���ʼ�ͷ��ʱ��Ϊ10����*/
#define DLM_TIME			10 * 60 * 1000		/*����һ���ʼ���ʱ��Ϊ10����*/
#define SM_TIME				10 * 60 * 1000		/*����һ���ʼ���ʱ��Ϊ10����*/

/*�ʼ������ɹ�������Ϣ*/
#define ON_MAILMESSAGE			WM_USER + 200	
#define	ON_MAILMESS				1				/*�����ʼ��ɹ�*/
#define	ON_NEWSUMMARYLISTSIZE	2				/*ȡ���ʼ�ͷ�б��С�ɹ�*/
#define ON_SENDMAIL				3				/*�����ʼ�*/

/*�ʼ�����ʧ�ܷ�����Ϣ*/
#define Mail_Fail_MSG			WM_USER + 300   
#define ERR_MSG_USERPASS		1				/*�û��������*/	
#define ERR_MSG_HOSTNAME		2				/*��������������*/
#define ERR_MSG_TIMEOUT			3				/*��ʱ����*/
#define ERR_MSG_OTHER			4				/*��������*/

typedef	void*	PMAIL;

typedef struct UserSettingInfo 
{
	char PopHost[HOST_LEN];
	char Password[PASSWORD_LEN];	
	char UserID[USERID_LEN];	//bbb@263.net
	char SmtpHost[HOST_LEN];
	int	 PopPort;
	int  SmtpPort;

}	USERSETTINGINFO;

typedef	struct Summary
{
	char From[ADDR_LEN];
	char Subject[SUBJECT_LEN];	
	char MessageID[ID_LEN];
	char Date[DATE_LEN];	
	int	 Size;

}	SUMMARY;

typedef struct SummaryList
{
	int		 MailCount;	
	SUMMARY SumHeader[1];

}	SUMMARYLIST;

typedef struct SendMailInfo
{
	int     nToLen;	         /*�����˵�ַ����*/
	int		nCcLen;	         /*�����˵�ַ����*/
	int  	nSubjectLen;     /*���ⳤ��*/
	int 	nTextLen;        /*���ĳ���*/
	int		nAttachNameLen;	 /*����������*/
	int		nAttachLen;		 /*�����ĳ���*/	
	char    From[ADDR_LEN];  /*�����˵ĵ�ַ*/
	char    bUnUsed0;        /*����ֽ�*/
	char	data[1];  	     /*���ݡ�����˳��ֱ���: ������ ������ ���� ���� (�м�û���κηָ�����)������ ����*/

}	SENDMAILINFO;

typedef struct Text_Attach
{
	int		AttachFlag;
	int		nTextLen;
	int		nAttachNameLen;
	int		nAttachLen;
	int		nDataBufLen;
	char	*DataBuf;
}	TEXT_ATTACH;

typedef	struct MailAgentInterface
{
	int		(*CreateMailAgentObj)(PMAIL	pMAgent,	HWND	hwnd,	USERSETTINGINFO	*pUAccInfo);			
	int 	(*GetNewSummaryListSize)(PMAIL	pMAgent, char *OldIdList);
	int 	(*GetNewSummaryList)(PMAIL	pMAgent,	SUMMARYLIST		*MailSumList);		
	int		(*DownLoadMail)(PMAIL	pMAgent,  TEXT_ATTACH	*MailBuf,	char*	MessageID);			
	int 	(*SendMail)(PMAIL	pMAgent,	SENDMAILINFO	*SendMailInfo);	
	void	(*ExceptionDeal)(PMAIL	pMAgent);
	void	(*DestoryMailAgentObj)(PMAIL	pMAgent);

}	MAILAGENTINTERFACE;

/*�ʼ��������ӿں���*/
void GetMailAgentInterface(MAILAGENTINTERFACE* pAgentInterface);

#endif