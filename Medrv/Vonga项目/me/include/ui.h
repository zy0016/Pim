
/**************************************************************************\
 *
 *                      PDAOS Window System
 *
 * Copyright (c) 2000 by CASS Corporation & Software Engineering Center
 *                       All Rights Reserved
 *
 *
 * Filename  : ui.h
 * 
 * Purpose   : about UDB infomation
 *  
 * Author    : jingjing xu
 *
 * Date      : 2000/12/29
 *
 **************************************************************************/

#ifndef _UI_H_
#define _UI_H_

/*���ŵ�UDB������Ϣ*/
#define     DIALFILE			  "FLASH2:DialCfg.mal"

#define     UDB_K_DNS_SIZE        31
#define     UDB_K_NAME_SIZE       31
#define     UDB_K_PWD_SIZE        31
#define     UDB_K_TEL_SIZE        31

typedef struct{
	char   szISP[ UDB_K_DNS_SIZE+1 ];
	char   szPhoneNum[ UDB_K_TEL_SIZE+1 ];
	char   szUserName[ UDB_K_NAME_SIZE+1 ];
	char   szPassWord[ UDB_K_PWD_SIZE+1 ];
}DIALTYPE;


/* UDB���ݽṹ */
typedef struct
{
	unsigned long		ISPID;						/*	ISP��ID��			*/
	unsigned long		PrimarySelect;				/*	�Ƿ���ѡISP			*/
	unsigned long		ProxyFlag;					/*	�Ƿ�ʹ�ô��������	*/
	unsigned long		ProxyID;					/*	���������ID		*/
    unsigned long		DtType;                     /*  ��������GPRS/GSM    */

	char    ISPName[ UDB_K_NAME_SIZE+1 ];			/*	ISP��				*/
	char	ISPUserName[ UDB_K_NAME_SIZE+1 ];		/*	ISP��¼�û���		*/
	char	ISPPassword[ UDB_K_PWD_SIZE+1 ];		/*	ISP��¼����			*/
	char	PrimaryDns[ UDB_K_DNS_SIZE+1 ];			/*	������������		*/
	char	SlaveDns[ UDB_K_DNS_SIZE+1 ];			/*	��������������		*/

	char	ISPPhoneNum1[ UDB_K_TEL_SIZE+1 ];		/*	����ISP�ĵ绰����	*/
	char	ISPPhoneNum2[ UDB_K_TEL_SIZE+1 ];		/*	����ISP�ĵ绰����	*/
	char	ISPPhoneNum3[ UDB_K_TEL_SIZE+1 ];		/*	����ISP�ĵ绰����	*/
	//char	ISPPhoneNum4[ UDB_K_TEL_SIZE+1 ];		/*	����ISP�ĵ绰����	*/

}UDB_ISPINFO;


int	UDB_IsLogin( void );
int	UDB_GetISPInfo( int, void* );
int	UDB_GetLogUser( void );
int	UDB_CntISP( int );
int	UDB_EnumISP( int, int );
int GetPredefinedISP( UDB_ISPINFO *, int );
int	UDB_GetProfileInt( const int, char*, char*, int );
int	UDB_GetProfileString( const int, char*, char*, char*, char*, int );
int	UDB_WriteProfile(int a,char* b,char* c,char* d);
int UDB_WriteProfileInt( int a, char* b, char* c, int d) ;

#endif/*_UI_H_*/
