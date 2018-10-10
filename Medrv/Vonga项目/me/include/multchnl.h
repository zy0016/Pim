/**************************************************************************\
*
*                      Hopen Software System
*
* Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
*                       All Rights Reserved
*
* Model   : MP
*
* Purpose : multiplexer protocol
*  
* Author  : Lintao Zhou
*
*-------------------------------------------------------------------------
*
* $Archive::                                                       $
* $Workfile::                                                      $
* $Revision::    $     $Date::                                     $
* 
\**************************************************************************/

/**************************************************************************\
*
*	��������: ��ͨ��Э��ӿں���
*
*	�ļ�����: multchnl.h
*
*	��������: ������
*
*	������: 
*
*	�������ڣ�2002.05.20
*
\**************************************************************************/

#ifndef _MULTCHNL_H_
#define _MULTCHNL_H_

/**************************************************************************\
*
*	��������
*
\**************************************************************************/

#include <hp_modcfg.h>
//#define WAVECOM_2C
//#define SIEMENS_TC35

/* ����Siemens TC35/MC35ͨ������ */
#define MPAPI_CHANNEL1			"MUX-SIEMENS-DLC1"	/* DATA & AT Channel*/
#define MPAPI_CHANNEL2			"MUX-SIEMENS-DLC2"	/* AT Channel */
#define MPAPI_CHANNEL3			"MUX-SIEMENS-DLC3"	/* AT Channel */

/* ����Wavecom 2C/2Bͨ������ */
#define MPAPI_CHANNEL_AT		"MUX-WAVECOM-AT"	/* AT Channel*/
#define MPAPI_CHANNEL_DATA		"MUX-WAVECOM-DATA"	/* DATA Channel */

/* ����� */
#define HANDLE_BASE				'CMUX'

/**************************************************************************\
*
*	���ݽṹ���������Ͷ���
*
\**************************************************************************/

/**************************************************************************\
*
*	��������
*
\**************************************************************************/

/* ����ӿں��� */

/* ϵͳ��ʼ������ */
int MPAPP_Initial(void);

/* �û��ӿں��� */
int MPAPP_Startup	(char* dev_name, int baudrate);
int MPAPP_Startup2	(int com_handle);
int MPAPP_Closedown	(void);
int MPAPP_OpenFile	(char* name, int mode, int attrib);
int MPAPP_CloseFile	(int handle);
int MPAPP_ReadFile	(int handle, void* buf,  int bufsize);
int MPAPP_WriteFile	(int handle, void* data, int datalen);
int MPAPP_FileIoControl(int handle, int code, void *pbuf, int size);

/**************************** End Of Head File ****************************/
#endif	/* _MULTCHNL_H_ */