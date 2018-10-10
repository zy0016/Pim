/***********************************************************************************
* 
* SYSTEM NAME：				ATTRACTOR
* SUBSYSTEM NAME：			UI辅助应用子系统
* MODEL BLOCK NAME：		UDB用户数据库操作
* SOURCE FILE NAME：		Database.h
* OBJECT FILE NAME：
* EXECUTABLE FILE NAME：
* AUTHOR：					郭燕鹏
* DATE WRITTEN：			1999.6.19
* REVISION：				V 1.0
*
***********************************************************************************/

#ifndef _DBINIT_H
#define	_DBINIT_H

#define	INIFILE				"FLASH:setup.ini"	/*系统配置文件*/
#define TEMPINI				"RAM:temp.ini"		/*临时配置文件*/
#define	UDB_F_MAILBOX		"FLASH:mailbox.dbf"	/*邮箱管理文件名*/
#define UDB_F_ISP			"FLASH:isp.dbf"		/*ISP管理文件名*/
#define UDB_F_FAVORITE		"FLASH:favorite.dbf"/*常用地址管理文件名*/
#define UDB_F_USER			"FLASH:user.dbf"	/*用户管理文件名*/
#define UDB_F_PROXY			"FLASH:proxy.dbf"	/*代理服务器管理文件名*/
#define UDB_F_TCPIP			"FLASH:tcpip.dbf"	/*TCPIP管理文件名 */

#define UDB_R_MAILBOX_MAX		6				/*邮箱管理文件中的最多记录数*/
#define UDB_R_ISP_MAX			6				/*ISP管理文件中的最多记录数*/
#define UDB_R_FAVORITE_MAX 		50				/*常用地址管理文件中的最多记录数*/
#define UDB_R_USER_MAX			6				/*用户管理文件中的最多记录数*/
#define UDB_R_PROXY_MAX			6				/*代理服务器管理文件中的最多记录数*/


#define	UDB_K_NAME_SIZE			36				/*名字类字段的长度*/
#define	UDB_K_PWD_SIZE			24				/*密码类字段的长度*/
#define	UDB_K_ADDR_SIZE			104				/*地址类字段的长度*/
#define	UDB_K_DNS_SIZE			16				/*域名服务器名类字段的长度*/
#define	UDB_K_TEL_SIZE			24				/*电话类字段的长度*/
#define	UDB_K_TITLE_SIZE		104				/*标题类字段的长度*/
#define	UDB_K_PORT_SIZE			12				/*端口类字段的长度*/
#define UDB_K_ADDR_SIZE1		64				/*地址类(代理服务器例外地址用）字段的长度*/

typedef struct									/*邮箱数据文件结构*/
{
	int				MailID;						/*该邮箱的ID号*/
	int				UserID;						/*使用该邮箱的用户的ID号*/
	char			MailboxName[UDB_K_NAME_SIZE];
												/*邮箱名*/
	char    		MailUserName[UDB_K_NAME_SIZE];	
												/*该邮箱的邮件服务器登录用户名*/
	char			MailPassword[UDB_K_PWD_SIZE];
												/*该邮箱的邮件服务器登录密码*/
	char			MailSMTPName[UDB_K_NAME_SIZE];	
												/*该邮箱的SMTP服务器的地址*/
	char			MailPOP3Name[UDB_K_NAME_SIZE];	
												/*该邮箱的POP3服务器的地址*/
	char			MailReturnAddress[UDB_K_ADDR_SIZE];		
												/*从该邮箱发出信件的回信地址*/
	BOOL			IsSaveBackup;				/*是否保存服务器上的备份*/
}UDB_MAILBOXINFO;

typedef struct									/*ISP数据文件结构 */
{
	unsigned long	ISPID;						/*ISP的ID号*/
	
	char    		ISPName[UDB_K_NAME_SIZE];	/*ISP名*/
	char			ISPUserName[UDB_K_NAME_SIZE];
												/*使用该ISP时的登录用户名*/
	char			ISPPassword[UDB_K_PWD_SIZE];/*使用该ISP时的登录密码*/
	char			PrimaryDns[UDB_K_DNS_SIZE];	/*主域名服务器*/
	char			SlaveDns[UDB_K_DNS_SIZE];	/*辅助域名服务器*/
	
	unsigned long	PrimarySelect;				/*是否手选ISP*/
	unsigned long	ProxyFlag;					/*是否使用代理服务器*/
	unsigned long	ProxyID;					/*代理服务器ID*/
	
	char			ISPPhoneNum1[UDB_K_TEL_SIZE];
												/*连接该ISP的电话号码*/
	char			ISPPhoneNum2[ UDB_K_TEL_SIZE];
												/*连接该ISP的电话号码*/
	char			ISPPhoneNum3[ UDB_K_TEL_SIZE];
												/*连接该ISP的电话号码*/
	char			ISPPhoneNum4[ UDB_K_TEL_SIZE];
												/*连接该ISP的电话号码*/
}UDB_ISPINFO;

/*定义常用地址表记录结构*/
typedef struct favorite
{
	char			PageTitle[UDB_K_TITLE_SIZE];/*页面标题*/
    char			URLAddress[UDB_K_ADDR_SIZE];/*页面地址*/
	char			SysUserName[UDB_K_NAME_SIZE];/*系统用户名*/
}UI_FAVORITE;


typedef struct									/*用户数据文件结构*/
{
	int				UserID;						/*用户的ID号*/
	char			UserName[UDB_K_NAME_SIZE];	/*用户名*/
	char			UserPassword[UDB_K_PWD_SIZE];
												/*用户的密码*/
	BOOL			UserAuthority;				/*用户的权限*/
	int				UserMailSize;				/*可以保存邮件的空间大小*/
	BOOL			ISPID1;						/*ISP1可否使用*/
	BOOL			ISPID2;						/*ISP2可否使用*/
	BOOL			ISPID3;						/*ISP3可否使用*/
	BOOL			ISPID4;						/*ISP4可否使用*/
	BOOL			ISPID5;						/*ISP5可否使用*/
	BOOL			ISPID6;						/*ISP6可否使用*/
}UDB_USERINFO;

typedef struct 									/*记录数据结构*/
{
	int				idxField;					/*字段的索引值*/
	union 
	{
		short			s;						/*短整型字段的数值*/
		int      		i;						/*带符号整型字段的数值*/
        unsigned int	u;						/*无符号整型字段的数值*/
        long			l;						/*长整型字段的数值*/
		BOOL			b;						/*布尔型*/
		char*     		p;						/*当该字段是字符串型时，p是指向字符串内容的指针*/
	}val;										/*表示字段数值的联合*/
}UDB_RECORD;

#pragma pack()

/***********************************************************************************
新增定义
***********************************************************************************/
#define		TranNetAddrLen		16
#define		MAXSTRINGLEN		16
/* MAC模块地址是动态获得 */
#define		MACADDR_DYN			"DYNAMIC"
/* 网关路由信息 */
typedef	struct	UI_tagNDIS_RouteInfo
{
	char	NetNum[TranNetAddrLen] ;	//网络号
	char	Mask[TranNetAddrLen] ;		//子网掩码
	char	GateWay[TranNetAddrLen] ;	//网关地址
}UI_NDIS_RouteInfo ;

/* 域名解析信息 */
typedef struct  UI_tagNDIS_ResolInfo
{
	char	Domain[MAXSTRINGLEN] ;	// 域信息
	char	Resolve1[TranNetAddrLen] ;	//域名解析服务器1
	char	Resolve2[TranNetAddrLen] ;	//域名解析服务器2
}UI_NDIS_ResolInfo ;

/* Add by zxh 2000.7.12 Begin */
/* 设置默认ISP信息 */

#define		MAXSTRLEN			32
#define		FUNERROR			0			/* 函数调用失败 */
#define		FUNOK				1			/* 函数调用成功 */
#define		MAXDEFISP			6			/* 最多设置6个ISP */

typedef struct setIntraDefISP
{		
	char	defISPName[MAXSTRLEN]  ;		/*默认"ISP"*/
	char	defLogUser[MAXSTRLEN]  ;		/*默认"用户名"*/
	char	defLogPWD[MAXSTRLEN]   ;		/*默认"密码"*/
	char	defLogTelNum[MAXSTRLEN];		/*默认"电话"*/
	char	defDNS1[MAXSTRLEN]	   ;		/*默认"主控DNS"*/
	char	defDNS2[MAXSTRLEN]	   ;		/*默认"辅控DNS"*/
} DEFISP[MAXDEFISP], defisp ;

typedef struct setIntraDefISP *LPDEFISP ;

int SetISP ( int index, LPDEFISP lpdefisp );//根据index的值(1~6)设置一个默认的ISP.
int SetPrimaryISP ( LPDEFISP lpdefisp );	//设置默认首选ISP.
int GetISP ( int index, LPDEFISP lpdefisp );//根据index的值(1~6)获得默认ISP的内容,
											//结果返回到lpdefish结构中.
int GetPrimaryISP ( LPDEFISP lpdefisp );	//获得默认首选ISP的值,结果返回到lpdefish结构中.
/* Add by zxh 2000.7.12 End */

#define		MAC_MaxRouteNum		3		//最多支持三个网关/一个网卡
#define		MAC_NUM				2		//设备个数
/* MAC设备信息 */
typedef struct  UI_tagNDIS_MACIPInfo
{
	char				MacName		[MAC_NUM][MAXSTRINGLEN] ;	//MAC名称
	char				LocalAddr	[MAC_NUM][TranNetAddrLen] ;		//本地地址
	char				LocalSubMask[MAC_NUM][TranNetAddrLen] ;		//子网掩码
	UI_NDIS_RouteInfo	RouteInfo	[MAC_NUM][MAC_MaxRouteNum] ;	//路由网关信息
	char				Default		[MAC_NUM][TranNetAddrLen] ;		//默认网关信息
	UI_NDIS_ResolInfo	ResolInfo	[MAC_NUM] ;						//域名解析信息
	BOOL				ifDHCP		[MAC_NUM] ;
	int					PrimarySelect [MAC_NUM] ;
}UI_NDIS_MACIPInfo ;

/***********************************************************************************
从TCPIP数据库中读出记录返回
***********************************************************************************/
int UI_GetTCPIPRecInfo( UI_NDIS_MACIPInfo*	ipInfo ) ;

/***********************************************************************************
创建并创建TCPIP数据库并更新记录值
***********************************************************************************/
int	UI_UpdateTCPIPRec( UI_NDIS_MACIPInfo*	ipInfo ) ;


#endif

/* END OF PROGRAM FILE */