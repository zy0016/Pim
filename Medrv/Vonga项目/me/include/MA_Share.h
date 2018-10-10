#ifndef _MASHARE_H_
#define _MASHARE_H_


#include"SPShare.h"


/*接收数据的最大长度*/
#define MAXRECVDATALEN		40 * 1024

/*超时设置*/
#define GNS_TIME			10 * 60 * 1000		/*取新邮件头超时设为10分钟*/
#define DLM_TIME			10 * 60 * 1000		/*下载一封邮件超时设为10分钟*/
#define SM_TIME				10 * 60 * 1000		/*发送一封邮件超时设为10分钟*/

/*邮件操作成功发送消息*/
#define ON_MAILMESSAGE			WM_USER + 200	
#define	ON_MAILMESS				1				/*下载邮件成功*/
#define	ON_NEWSUMMARYLISTSIZE	2				/*取新邮件头列表大小成功*/
#define ON_SENDMAIL				3				/*发送邮件*/

/*邮件操作失败发送消息*/
#define Mail_Fail_MSG			WM_USER + 300   
#define ERR_MSG_USERPASS		1				/*用户名密码错*/	
#define ERR_MSG_HOSTNAME		2				/*服务器主机名错*/
#define ERR_MSG_TIMEOUT			3				/*超时错误*/
#define ERR_MSG_OTHER			4				/*其它错误*/

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
	int     nToLen;	         /*接收人地址长度*/
	int		nCcLen;	         /*抄送人地址长度*/
	int  	nSubjectLen;     /*主题长度*/
	int 	nTextLen;        /*正文长度*/
	int		nAttachNameLen;	 /*附件名长度*/
	int		nAttachLen;		 /*附件的长度*/	
	char    From[ADDR_LEN];  /*发送人的地址*/
	char    bUnUsed0;        /*填充字节*/
	char	data[1];  	     /*数据。数据顺序分别是: 接收人 抄送人 主题 正文 (中间没有任何分隔符号)附件名 附件*/

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

/*邮件代理对象接口函数*/
void GetMailAgentInterface(MAILAGENTINTERFACE* pAgentInterface);

#endif