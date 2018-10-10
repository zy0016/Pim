/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :
 *
 * Filename:     Setupstr.h  
 *
 * Purpose:      Implements the setup application.
 *  
 * Author:       ZhaoXiaoHui     
 *
 *-------------------------------------------------------------------------
 * $Archive:: /RWP_SRC_Emu/Pda_app/Setup/setupstr.h                 $
 * $Revision:: 3  $     $Date:: 01-08-31 2:53p                                              $
 *
 * $History:
 \**************************************************************************/
#ifndef  SETUPSTR_H
#define  SETUPSTR_H

#include "hp_pdaex.h"
#include "setting.h"
#include "str_hp.h"
#include "str_public.h"
#include "hp_icon.h"

// PDP 
#define WND_X       0 
#define WND_Y       15 
#define WND_WIDTH   120
#define WND_HEIGHT  145

//mess
#define  IDM_GET             WM_USER + 401  //�õ�����
#define  IDM_DIAL_SETTEXT    WM_USER + 402  //���ò����ı�
#define  IDM_FROM_PIN        WM_USER + 403  //�����Ǹ�����pin or pin2
#define  IDM_CHANGE_NAME     WM_USER + 404  //���Ű�ť���ı�

//�����������ô��ڼ�Ĵ��ݽṹ
typedef struct tagFromSetDial
{
	HWND  hSrc;
    int   iIndex;
    int   iNew;
}FromSetDial;

extern long FS_WritePrivateProfileString(char * ,char * ,char * ,char * );
extern long FS_GetPrivateProfileString(char *, char *,char *,char *,unsigned long , char *); 							  																			 
extern long FS_WritePrivateProfileInt(char *,char *, long ,char * );
extern long FS_GetPrivateProfileInt( char *,char *,unsigned long ,char *);
												  				
extern void	DisplayScreen();

#define TEXT_CAPTION_OK     "ȷ��"
#define TEXT_CAPTION_CANCEL "�˳�"
#define TEXT_CAPTION_CLEAR  "���"
#define TEXT_WND_FAILD      "��������ʧ��!"

/********************* Contrast.c ************************/
#define TEXT_GRIGHT			"��"
#define TEXT_DARK			"��"
#define	TEXT_CONTRASTPROMPT	"    ������������,����\n\n�Աȶ�"

/************************* end ***************************/

/********************* bgcase.c***************************/
#define TEXT_CASESAVEERR1   "����������,�������"
#define TEXT_CASESAVEERR2   "���淽��ʧ��"
/*********************end********************************/

/********************* DateTime.c ************************/
#define TEXT_YEAR			"��"
#define TEXT_MONTH			"��"
#define TEXT_DAY			"��"
#define TEXT_HOUR			"ʱ"
#define TEXT_MINUTE			"��"
#define TEXT_SECOND			"��"
#define TEXT_DATE			"����"
#define TEXT_TIME			"ʱ��"
#define DATE_MSG_1          "�������ò���ȷ,������!"
#define DATE_MSG_2          "ʱ�����ò���ȷ,������!"
/************************* end ***************************/

/********************* InputPwd.c ************************/
#define TEXT_PASSWORD		"����"
#define TEXT_OK				"ȷ��"
#define TEXT_INPUTPINPWD	"������PIN%d��:"
#define TEXT_INPUTPHONEPWD  "�������ֻ�����"
#define TEXT_INPUTPUKPWD    "������PUK%d��"
#define TEXT_PROMPTPWD2		"������֤"
#define TEXT_PROMPTPWD3		"����PUK"
 
/************************* end ***************************/

/********************* MyMessage.c ************************/
#define TEXT_HOST			"S:����"
#define TEXT_NAME			"��  ��:"
#define TEXT_UNIT			"��  λ:"
#define TEXT_ADDRESS		"��  ַ:"
#define TEXT_MAIL			"E_Mail:"
#define TEXT_PHONE			"��  ��:"
#define TEXT_SHOWHOSTMESS	"����ʱ��ʾ������Ϣ"
/************************* end ****************************/

/********************* Password.c ************************/
#define TEXT_PROMPTPWD4		"�����������"
#define TEXT_PROMPTPWD5		"�������ô���"
#define TEXT_PROMPTPWD6		"ȷ���������벻һ��"
#define TEXT_PROMPTPWD7		"PIN���޸ĳɹ�"
#define TEXT_PROMPTPWD8		"PIN���޸�ʧ��"
//----- add -----------------------
#define TEXT_PROMPTPWD9		"������4-8λ����"
#define TEXT_PROMPTPWD10	"pin��������"
#define TEXT_PROMPTPWD11    "pin����ȡ��"
#define TEXT_PROMPTPWD12    "�������,SIM������!"
#define TEXT_PROMPTPWD13    "�ֻ�����������"
#define TEXT_PROMPTPWD14    "�ֻ�������ȡ��"
#define TEXT_PROMPTPWD15    "�ֻ��������óɹ�"
#define TEXT_PROMPTPWD16    "�ֻ���������ʧ��"
//---------------------------------
#define TEXT_OLDPASSWORD	"��������:"
#define TEXT_OLDPWD         "ԭ����"
#define TEXT_NEWPASSWORD	"������"
#define TEXT_PWDCONFIRM		"ȷ  ��"
#define TEXT_PINREMAIN       "������������%d��"
#define TEXT_PUKPASSWORD     "PUK��"
#define TEXT_NEWPIN          "��PIN��"
#define TEXT_PINON           "����"
#define TEXT_PINOFF          "ȡ��"
#define TEXT_PINSTART        "��������PIN��"
/************************* end ****************************/

/********************** penCalib.c ************************/
#define TEXT_PENPROMPT1		"    ��������豸û\n�жԵ��������ȷ��\nӦ������Ҫ����У׼\n��Ļ��\
                             \n    Ҫ��ʼ��У׼��\n�̣��밴ȷ����"
#define TEXT_CALIB			"У׼"
#define TEXT_PENEXITBUTTON	"�˳�"
#define TEXT_PENEXIT		"S:��У׼"
/************************* end ****************************/

/************************ penwnd.c ************************/
#define TEXT_PENPROMPT2		"������У׼"
#define TEXT_PENPROMPT3		"������У׼ʧ��"
#define TEXT_PENPROMPT4		"������У׼"
#define TEXT_PENPROMPT5		"�������Ͻ�"
#define TEXT_PENPROMPT6		"�������½�"
#define TEXT_PENPROMPT7		"��������"
#define TEXT_PENPROMPT8		"ʮ�ֽ��洦"
/************************* end ****************************/

/***************** ClrupFlh.c ************************************/
#define  MEMMGR_MESS_1  "������Ƭ,���Ե�..."
#define  MEMMGR_MESS_2  "������Ƭ����."
#define  MEMMGR_MESS_3  "������Ƭʧ��."
#define  MEMMGR_MESS_4  "������Ƭ�ǱȽϺ�ʱ"
#define  MEMMGR_MESS_5  "�Ĳ���,����ʱ������"
#define  MEMMGR_MESS_6  "�ȴ������ⶪʧ���ݡ�"
#define  MEMMGR_MESS_7  "                    "
#define  MEMMGR_MESS_8  "��Ƭ���������,�˳���Ч!"
#define  MEMMGR_MESS_9  "��������ʧ��"

/********************* end ****************************************/
#endif