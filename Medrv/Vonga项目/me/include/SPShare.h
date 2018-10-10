#ifndef _SMTPSHR_H_
#define _SMTPSHR_H_

#include"window.h"

#define	SUBJECT_LEN		50				/*subject长度*/
#define ADDR_LEN		50				/*To,From长度*/
#define HOST_LEN		50				/*服务器主机名长度*/
#define PASSWORD_LEN	50				/*用户密码长度*/
#define USERID_LEN		50				/*用户ID的最大长度(邮箱)*/
#define ID_LEN			50				/*邮件的messageID长度*/
#define	DATE_LEN		50				/*时间长度*/


/**********************************POP3*******************************************/
/**********************************POP3*******************************************/
/**********************************POP3*******************************************/


/*POP3代理对象向上层程序发送的消息*/
#define	ONCONNECTED		WM_USER + 1		/*CONNECT成功*/
#define	ONTOP			WM_USER + 2		/*TOP命令完成*/
#define	ONLISTNUM		WM_USER + 3		/*LIST NUM命令完成*/
#define	ONLIST			WM_USER + 4		/*LIST命令完成*/
#define	ONSTAT			WM_USER + 5		/*STAT命令完成*/
#define	ONRETR			WM_USER + 6		/*RETR命令完成*/	
#define	ONDELETE		WM_USER + 7		/*DELE命令完成*/
#define	ONUIDL			WM_USER + 8		/*UIDL命令完成*/	
#define	ONNOOP			WM_USER + 10	/*NOOP命令完成*/	
#define	ONRESET			WM_USER + 11	/*REST命令完成*/	
#define	ONQUIT			WM_USER + 12	/*QUIT命令完成*/

/*POP3对象通知上层程序的错误消息*/
#define	POP_ERROR_MSG	 WM_USER + 13
#define POP3_ERR_TIMEOUT			1	/*超时错误*/
#define POP3_ERR_PROTOCOL			2	/*协议错误*/
#define	POP3_ERR_USER				3	/*用户名错*/
#define POP3_ERR_PASS				4	/*密码错*/
#define POP3_ERR_MEMORY				5	/*内存不足*/
#define POP3_ERR_HOSTNAME			6	/*主机名错*/
#define POP3_ERR_OTHER				7	/*其它错误*/

typedef void* PAGN;

typedef struct POPInfo
{
	int				Port;
	char			Host[HOST_LEN];
	char			Password[PASSWORD_LEN];	
	char			UserID[USERID_LEN];
	char			UnUser0;
	char			UnUser1;
	
}POPINFO;

typedef struct POP3AgentInterface
{
	int			(*CreatePOP3AgentObj)(PAGN pAgent,HWND hwnd,  POPINFO	*pPOPInfo);
	int			(*POP3_Connect)(PAGN pAgent);
	int 		(*POP3_Noop)(PAGN pAgent);
	int			(*POP3_Delete)(PAGN pAgent,	int	MailNumber);
	int			(*POP3_Reset)(PAGN pAgent);
	int			(*POP3_Quit)(PAGN pAgent);						
	int			(*POP3_Stat)(PAGN pAgent);
	int			(*POP3_ListNum)(PAGN pAgent,	int	Num);			
	int			(*POP3_List)(PAGN pAgent,	char *buf,	int buflen);
	int			(*POP3_UniqueID)(PAGN pAgent,	char *buf,	int buflen);
	int			(*POP3_Retr)(PAGN pAgent,	int	MailNumber,	char *Buf,	int	Buflen);
	int			(*POP3_Top)(PAGN pAgent,	int	MailNumber,	int	NumOfLines,	char	*buf,	int	buflen);
	void		(*DestoryPOP3AgentObj)(PAGN pAgent);
	
}	POP3AGENTINTERFACE;

void GetPOPAgentInterface(POP3AGENTINTERFACE** pAgentInterface);



/**********************************SMTP*******************************************/
/**********************************SMTP*******************************************/
/**********************************SMTP*******************************************/


/*SMTP对象结构对上层透明*/
typedef void* SAGN;

#define ONSEND					WM_USER + 100		/*邮件发送成功消息*/
#define SMTP_MSG_ERROR			WM_USER + 101		/*邮件发送失败消息*/
#define SMTP_ERR_USERPASS		1					/*用户名密码错*/
#define SMTP_ERR_HOSTNAME		2					/*服务器主机名错*/
#define SMTP_ERR_MEMORY			3					/*内存不足*/
#define SMTP_ERR_PROTOCOL		4					/*协议错*/
#define SMTP_ERR_TIMEOUT		5					/*超时错*/
#define SMTP_ERR_OTHER			6					/*其它错误*/

typedef struct MailInfo
{
	int		nToCcCount;					/*抄送人地址长度*/
	int		nToCcLen;					/*接收人地址长度*/		
	int		nTextLen;					/*邮件正文的长度*/			
	int		nPort;						/*协议端口号*/
	char	UserID[USERID_LEN];			/*用户名*/
	char	Password[PASSWORD_LEN];		/*密码*/
	char	From[ADDR_LEN];				/*发送人*/
	char	SmtpServer[HOST_LEN];		/*服务器主机名	*/
	char	*Data;						/*按照To,Cc,邮件，顺序存放数据的不定长度的缓冲区*/

}	MAILINFO;

/*SMTP对象接口*/
typedef struct SMTPAgentInterface
{
	int			(*CreateSMTPAgent)(SAGN pAgent,HWND hwnd,MAILINFO *SMailInfo);		
	int			(*SMTP_Sent)(SAGN pAgent);	    
	void		(*DestorySMTPAgent)(SAGN pAgent);

}	SMTPAGENTINTERFACE;

/*得到SMTP接口函数*/
void GetSMTPAgentInterface(SMTPAGENTINTERFACE** pAgentInterface);

#endif //_SPSHR_H_
