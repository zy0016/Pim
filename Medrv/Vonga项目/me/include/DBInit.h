/***********************************************************************************
* 
* SYSTEM NAME��				ATTRACTOR
* SUBSYSTEM NAME��			UI����Ӧ����ϵͳ
* MODEL BLOCK NAME��		UDB�û����ݿ����
* SOURCE FILE NAME��		Database.h
* OBJECT FILE NAME��
* EXECUTABLE FILE NAME��
* AUTHOR��					������
* DATE WRITTEN��			1999.6.19
* REVISION��				V 1.0
*
***********************************************************************************/

#ifndef _DBINIT_H
#define	_DBINIT_H

#define	INIFILE				"FLASH:setup.ini"	/*ϵͳ�����ļ�*/
#define TEMPINI				"RAM:temp.ini"		/*��ʱ�����ļ�*/
#define	UDB_F_MAILBOX		"FLASH:mailbox.dbf"	/*��������ļ���*/
#define UDB_F_ISP			"FLASH:isp.dbf"		/*ISP�����ļ���*/
#define UDB_F_FAVORITE		"FLASH:favorite.dbf"/*���õ�ַ�����ļ���*/
#define UDB_F_USER			"FLASH:user.dbf"	/*�û������ļ���*/
#define UDB_F_PROXY			"FLASH:proxy.dbf"	/*��������������ļ���*/
#define UDB_F_TCPIP			"FLASH:tcpip.dbf"	/*TCPIP�����ļ��� */

#define UDB_R_MAILBOX_MAX		6				/*��������ļ��е�����¼��*/
#define UDB_R_ISP_MAX			6				/*ISP�����ļ��е�����¼��*/
#define UDB_R_FAVORITE_MAX 		50				/*���õ�ַ�����ļ��е�����¼��*/
#define UDB_R_USER_MAX			6				/*�û������ļ��е�����¼��*/
#define UDB_R_PROXY_MAX			6				/*��������������ļ��е�����¼��*/


#define	UDB_K_NAME_SIZE			36				/*�������ֶεĳ���*/
#define	UDB_K_PWD_SIZE			24				/*�������ֶεĳ���*/
#define	UDB_K_ADDR_SIZE			104				/*��ַ���ֶεĳ���*/
#define	UDB_K_DNS_SIZE			16				/*���������������ֶεĳ���*/
#define	UDB_K_TEL_SIZE			24				/*�绰���ֶεĳ���*/
#define	UDB_K_TITLE_SIZE		104				/*�������ֶεĳ���*/
#define	UDB_K_PORT_SIZE			12				/*�˿����ֶεĳ���*/
#define UDB_K_ADDR_SIZE1		64				/*��ַ��(��������������ַ�ã��ֶεĳ���*/

typedef struct									/*���������ļ��ṹ*/
{
	int				MailID;						/*�������ID��*/
	int				UserID;						/*ʹ�ø�������û���ID��*/
	char			MailboxName[UDB_K_NAME_SIZE];
												/*������*/
	char    		MailUserName[UDB_K_NAME_SIZE];	
												/*��������ʼ���������¼�û���*/
	char			MailPassword[UDB_K_PWD_SIZE];
												/*��������ʼ���������¼����*/
	char			MailSMTPName[UDB_K_NAME_SIZE];	
												/*�������SMTP�������ĵ�ַ*/
	char			MailPOP3Name[UDB_K_NAME_SIZE];	
												/*�������POP3�������ĵ�ַ*/
	char			MailReturnAddress[UDB_K_ADDR_SIZE];		
												/*�Ӹ����䷢���ż��Ļ��ŵ�ַ*/
	BOOL			IsSaveBackup;				/*�Ƿ񱣴�������ϵı���*/
}UDB_MAILBOXINFO;

typedef struct									/*ISP�����ļ��ṹ */
{
	unsigned long	ISPID;						/*ISP��ID��*/
	
	char    		ISPName[UDB_K_NAME_SIZE];	/*ISP��*/
	char			ISPUserName[UDB_K_NAME_SIZE];
												/*ʹ�ø�ISPʱ�ĵ�¼�û���*/
	char			ISPPassword[UDB_K_PWD_SIZE];/*ʹ�ø�ISPʱ�ĵ�¼����*/
	char			PrimaryDns[UDB_K_DNS_SIZE];	/*������������*/
	char			SlaveDns[UDB_K_DNS_SIZE];	/*��������������*/
	
	unsigned long	PrimarySelect;				/*�Ƿ���ѡISP*/
	unsigned long	ProxyFlag;					/*�Ƿ�ʹ�ô��������*/
	unsigned long	ProxyID;					/*���������ID*/
	
	char			ISPPhoneNum1[UDB_K_TEL_SIZE];
												/*���Ӹ�ISP�ĵ绰����*/
	char			ISPPhoneNum2[ UDB_K_TEL_SIZE];
												/*���Ӹ�ISP�ĵ绰����*/
	char			ISPPhoneNum3[ UDB_K_TEL_SIZE];
												/*���Ӹ�ISP�ĵ绰����*/
	char			ISPPhoneNum4[ UDB_K_TEL_SIZE];
												/*���Ӹ�ISP�ĵ绰����*/
}UDB_ISPINFO;

/*���峣�õ�ַ���¼�ṹ*/
typedef struct favorite
{
	char			PageTitle[UDB_K_TITLE_SIZE];/*ҳ�����*/
    char			URLAddress[UDB_K_ADDR_SIZE];/*ҳ���ַ*/
	char			SysUserName[UDB_K_NAME_SIZE];/*ϵͳ�û���*/
}UI_FAVORITE;


typedef struct									/*�û������ļ��ṹ*/
{
	int				UserID;						/*�û���ID��*/
	char			UserName[UDB_K_NAME_SIZE];	/*�û���*/
	char			UserPassword[UDB_K_PWD_SIZE];
												/*�û�������*/
	BOOL			UserAuthority;				/*�û���Ȩ��*/
	int				UserMailSize;				/*���Ա����ʼ��Ŀռ��С*/
	BOOL			ISPID1;						/*ISP1�ɷ�ʹ��*/
	BOOL			ISPID2;						/*ISP2�ɷ�ʹ��*/
	BOOL			ISPID3;						/*ISP3�ɷ�ʹ��*/
	BOOL			ISPID4;						/*ISP4�ɷ�ʹ��*/
	BOOL			ISPID5;						/*ISP5�ɷ�ʹ��*/
	BOOL			ISPID6;						/*ISP6�ɷ�ʹ��*/
}UDB_USERINFO;

typedef struct 									/*��¼���ݽṹ*/
{
	int				idxField;					/*�ֶε�����ֵ*/
	union 
	{
		short			s;						/*�������ֶε���ֵ*/
		int      		i;						/*�����������ֶε���ֵ*/
        unsigned int	u;						/*�޷��������ֶε���ֵ*/
        long			l;						/*�������ֶε���ֵ*/
		BOOL			b;						/*������*/
		char*     		p;						/*�����ֶ����ַ�����ʱ��p��ָ���ַ������ݵ�ָ��*/
	}val;										/*��ʾ�ֶ���ֵ������*/
}UDB_RECORD;

#pragma pack()

/***********************************************************************************
��������
***********************************************************************************/
#define		TranNetAddrLen		16
#define		MAXSTRINGLEN		16
/* MACģ���ַ�Ƕ�̬��� */
#define		MACADDR_DYN			"DYNAMIC"
/* ����·����Ϣ */
typedef	struct	UI_tagNDIS_RouteInfo
{
	char	NetNum[TranNetAddrLen] ;	//�����
	char	Mask[TranNetAddrLen] ;		//��������
	char	GateWay[TranNetAddrLen] ;	//���ص�ַ
}UI_NDIS_RouteInfo ;

/* ����������Ϣ */
typedef struct  UI_tagNDIS_ResolInfo
{
	char	Domain[MAXSTRINGLEN] ;	// ����Ϣ
	char	Resolve1[TranNetAddrLen] ;	//��������������1
	char	Resolve2[TranNetAddrLen] ;	//��������������2
}UI_NDIS_ResolInfo ;

/* Add by zxh 2000.7.12 Begin */
/* ����Ĭ��ISP��Ϣ */

#define		MAXSTRLEN			32
#define		FUNERROR			0			/* ��������ʧ�� */
#define		FUNOK				1			/* �������óɹ� */
#define		MAXDEFISP			6			/* �������6��ISP */

typedef struct setIntraDefISP
{		
	char	defISPName[MAXSTRLEN]  ;		/*Ĭ��"ISP"*/
	char	defLogUser[MAXSTRLEN]  ;		/*Ĭ��"�û���"*/
	char	defLogPWD[MAXSTRLEN]   ;		/*Ĭ��"����"*/
	char	defLogTelNum[MAXSTRLEN];		/*Ĭ��"�绰"*/
	char	defDNS1[MAXSTRLEN]	   ;		/*Ĭ��"����DNS"*/
	char	defDNS2[MAXSTRLEN]	   ;		/*Ĭ��"����DNS"*/
} DEFISP[MAXDEFISP], defisp ;

typedef struct setIntraDefISP *LPDEFISP ;

int SetISP ( int index, LPDEFISP lpdefisp );//����index��ֵ(1~6)����һ��Ĭ�ϵ�ISP.
int SetPrimaryISP ( LPDEFISP lpdefisp );	//����Ĭ����ѡISP.
int GetISP ( int index, LPDEFISP lpdefisp );//����index��ֵ(1~6)���Ĭ��ISP������,
											//������ص�lpdefish�ṹ��.
int GetPrimaryISP ( LPDEFISP lpdefisp );	//���Ĭ����ѡISP��ֵ,������ص�lpdefish�ṹ��.
/* Add by zxh 2000.7.12 End */

#define		MAC_MaxRouteNum		3		//���֧����������/һ������
#define		MAC_NUM				2		//�豸����
/* MAC�豸��Ϣ */
typedef struct  UI_tagNDIS_MACIPInfo
{
	char				MacName		[MAC_NUM][MAXSTRINGLEN] ;	//MAC����
	char				LocalAddr	[MAC_NUM][TranNetAddrLen] ;		//���ص�ַ
	char				LocalSubMask[MAC_NUM][TranNetAddrLen] ;		//��������
	UI_NDIS_RouteInfo	RouteInfo	[MAC_NUM][MAC_MaxRouteNum] ;	//·��������Ϣ
	char				Default		[MAC_NUM][TranNetAddrLen] ;		//Ĭ��������Ϣ
	UI_NDIS_ResolInfo	ResolInfo	[MAC_NUM] ;						//����������Ϣ
	BOOL				ifDHCP		[MAC_NUM] ;
	int					PrimarySelect [MAC_NUM] ;
}UI_NDIS_MACIPInfo ;

/***********************************************************************************
��TCPIP���ݿ��ж�����¼����
***********************************************************************************/
int UI_GetTCPIPRecInfo( UI_NDIS_MACIPInfo*	ipInfo ) ;

/***********************************************************************************
����������TCPIP���ݿⲢ���¼�¼ֵ
***********************************************************************************/
int	UI_UpdateTCPIPRec( UI_NDIS_MACIPInfo*	ipInfo ) ;


#endif

/* END OF PROGRAM FILE */