/**********************************************************************\
 *
 *                      <联想智能手机>
 *
 * Copyright (c) 1999-2001 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model：	   
 *
 * File Name:	SMSDispatch.h
 *
 * Purpse :     短信分发模块的外部接口
 *  
 * Author:		Zhao Hui
*-------------------------------------------------------------------------
*
* $Archive::                                                       $
* $Workfile::                                                      $
* $Revision::    $     $Date::                                     $
* 
\**************************************************************************/

#ifndef _SMS_DISPATCH_H_
#define _SMS_DISPATCH_H_

#include <winpda.h>
#include <MEdrv.h> 

typedef int (*PSMSTransact_f)(SMS_INFO *pstSMSInfo);

typedef struct Address
{
	unsigned short	port;
//	unsigned short	unused;
	unsigned char	loweraddr[ME_TELEPHONE_STRLEN];
}Address_t;

/*
 Error code 	
*/
#define ERR_SUCCESS		0
#define ERR_SYS			-1
#define ERR_LACK_MEM	-2
#define ERR_PARAM		-3
#define ERR_CTLBLK		-4
#define ERR_FAILURE		-5

/*
 External interface functions.
*/
int SMSDISPATCH_Init		(void);
int SMSDISPATCH_Shutdown	(void);
/*
 For text SMS.
*/
int SMSDISPATCH_Register	(char *pCondition, PSMSTransact_f pCbFun);
int SMSDISPATCH_SendSMS		(SMS_PDUSEND *pstSMSSend);
int SMSDISPATCH_UnReg		(char *pCondition);
/*
 For data SMS.
*/
int SMSDISPATCH_Open		(void);
int SMSDISPATCH_Close		(int handle);
int SMSDISPATCH_Ioctl		(int handle, unsigned short port, HWND hwnd, UINT msg);
int SMSDISPATCH_Send		(int handle, Address_t destaddr, char *pdata, unsigned short len);
int SMSDISPATCH_Recv		(int handle, Address_t *psrcaddr, char *pBuf, int Len);

#endif