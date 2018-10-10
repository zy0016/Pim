#ifndef _SMTPSHR_H_
#define _SMTPSHR_H_

#include"window.h"

#define	SUBJECT_LEN		50				/*subject����*/
#define ADDR_LEN		50				/*To,From����*/
#define HOST_LEN		50				/*����������������*/
#define PASSWORD_LEN	50				/*�û����볤��*/
#define USERID_LEN		50				/*�û�ID����󳤶�(����)*/
#define ID_LEN			50				/*�ʼ���messageID����*/
#define	DATE_LEN		50				/*ʱ�䳤��*/


/**********************************POP3*******************************************/
/**********************************POP3*******************************************/
/**********************************POP3*******************************************/


/*POP3����������ϲ�����͵���Ϣ*/
#define	ONCONNECTED		WM_USER + 1		/*CONNECT�ɹ�*/
#define	ONTOP			WM_USER + 2		/*TOP�������*/
#define	ONLISTNUM		WM_USER + 3		/*LIST NUM�������*/
#define	ONLIST			WM_USER + 4		/*LIST�������*/
#define	ONSTAT			WM_USER + 5		/*STAT�������*/
#define	ONRETR			WM_USER + 6		/*RETR�������*/	
#define	ONDELETE		WM_USER + 7		/*DELE�������*/
#define	ONUIDL			WM_USER + 8		/*UIDL�������*/	
#define	ONNOOP			WM_USER + 10	/*NOOP�������*/	
#define	ONRESET			WM_USER + 11	/*REST�������*/	
#define	ONQUIT			WM_USER + 12	/*QUIT�������*/

/*POP3����֪ͨ�ϲ����Ĵ�����Ϣ*/
#define	POP_ERROR_MSG	 WM_USER + 13
#define POP3_ERR_TIMEOUT			1	/*��ʱ����*/
#define POP3_ERR_PROTOCOL			2	/*Э�����*/
#define	POP3_ERR_USER				3	/*�û�����*/
#define POP3_ERR_PASS				4	/*�����*/
#define POP3_ERR_MEMORY				5	/*�ڴ治��*/
#define POP3_ERR_HOSTNAME			6	/*��������*/
#define POP3_ERR_OTHER				7	/*��������*/

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


/*SMTP����ṹ���ϲ�͸��*/
typedef void* SAGN;

#define ONSEND					WM_USER + 100		/*�ʼ����ͳɹ���Ϣ*/
#define SMTP_MSG_ERROR			WM_USER + 101		/*�ʼ�����ʧ����Ϣ*/
#define SMTP_ERR_USERPASS		1					/*�û��������*/
#define SMTP_ERR_HOSTNAME		2					/*��������������*/
#define SMTP_ERR_MEMORY			3					/*�ڴ治��*/
#define SMTP_ERR_PROTOCOL		4					/*Э���*/
#define SMTP_ERR_TIMEOUT		5					/*��ʱ��*/
#define SMTP_ERR_OTHER			6					/*��������*/

typedef struct MailInfo
{
	int		nToCcCount;					/*�����˵�ַ����*/
	int		nToCcLen;					/*�����˵�ַ����*/		
	int		nTextLen;					/*�ʼ����ĵĳ���*/			
	int		nPort;						/*Э��˿ں�*/
	char	UserID[USERID_LEN];			/*�û���*/
	char	Password[PASSWORD_LEN];		/*����*/
	char	From[ADDR_LEN];				/*������*/
	char	SmtpServer[HOST_LEN];		/*������������	*/
	char	*Data;						/*����To,Cc,�ʼ���˳�������ݵĲ������ȵĻ�����*/

}	MAILINFO;

/*SMTP����ӿ�*/
typedef struct SMTPAgentInterface
{
	int			(*CreateSMTPAgent)(SAGN pAgent,HWND hwnd,MAILINFO *SMailInfo);		
	int			(*SMTP_Sent)(SAGN pAgent);	    
	void		(*DestorySMTPAgent)(SAGN pAgent);

}	SMTPAGENTINTERFACE;

/*�õ�SMTP�ӿں���*/
void GetSMTPAgentInterface(SMTPAGENTINTERFACE** pAgentInterface);

#endif //_SPSHR_H_
