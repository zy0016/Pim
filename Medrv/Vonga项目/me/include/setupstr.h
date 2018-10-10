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
#define  IDM_GET             WM_USER + 401  //得到铃声
#define  IDM_DIAL_SETTEXT    WM_USER + 402  //设置拨号文本
#define  IDM_FROM_PIN        WM_USER + 403  //保存那个密码pin or pin2
#define  IDM_CHANGE_NAME     WM_USER + 404  //拨号按钮名改变

//创建拨号设置窗口间的传递结构
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

#define TEXT_CAPTION_OK     "确定"
#define TEXT_CAPTION_CANCEL "退出"
#define TEXT_CAPTION_CLEAR  "清除"
#define TEXT_WND_FAILD      "创建窗口失败!"

/********************* Contrast.c ************************/
#define TEXT_GRIGHT			"暗"
#define TEXT_DARK			"亮"
#define	TEXT_CONTRASTPROMPT	"    按滚动条两端,调节\n\n对比度"

/************************* end ***************************/

/********************* bgcase.c***************************/
#define TEXT_CASESAVEERR1   "方案数已满,不能另存"
#define TEXT_CASESAVEERR2   "保存方案失败"
/*********************end********************************/

/********************* DateTime.c ************************/
#define TEXT_YEAR			"年"
#define TEXT_MONTH			"月"
#define TEXT_DAY			"日"
#define TEXT_HOUR			"时"
#define TEXT_MINUTE			"分"
#define TEXT_SECOND			"秒"
#define TEXT_DATE			"日期"
#define TEXT_TIME			"时间"
#define DATE_MSG_1          "日期设置不正确,请重设!"
#define DATE_MSG_2          "时间设置不正确,请重设!"
/************************* end ***************************/

/********************* InputPwd.c ************************/
#define TEXT_PASSWORD		"密码"
#define TEXT_OK				"确定"
#define TEXT_INPUTPINPWD	"请输入PIN%d码:"
#define TEXT_INPUTPHONEPWD  "请输入手机密码"
#define TEXT_INPUTPUKPWD    "请输入PUK%d码"
#define TEXT_PROMPTPWD2		"开机验证"
#define TEXT_PROMPTPWD3		"输入PUK"
 
/************************* end ***************************/

/********************* MyMessage.c ************************/
#define TEXT_HOST			"S:机主"
#define TEXT_NAME			"姓  名:"
#define TEXT_UNIT			"单  位:"
#define TEXT_ADDRESS		"地  址:"
#define TEXT_MAIL			"E_Mail:"
#define TEXT_PHONE			"电  话:"
#define TEXT_SHOWHOSTMESS	"开机时显示主人信息"
/************************* end ****************************/

/********************* Password.c ************************/
#define TEXT_PROMPTPWD4		"密码输入错误"
#define TEXT_PROMPTPWD5		"密码设置错误"
#define TEXT_PROMPTPWD6		"确认与新密码不一致"
#define TEXT_PROMPTPWD7		"PIN码修改成功"
#define TEXT_PROMPTPWD8		"PIN码修改失败"
//----- add -----------------------
#define TEXT_PROMPTPWD9		"请输入4-8位密码"
#define TEXT_PROMPTPWD10	"pin码已启用"
#define TEXT_PROMPTPWD11    "pin码已取消"
#define TEXT_PROMPTPWD12    "输入错误,SIM卡将锁!"
#define TEXT_PROMPTPWD13    "手机密码已启用"
#define TEXT_PROMPTPWD14    "手机密码已取消"
#define TEXT_PROMPTPWD15    "手机密码设置成功"
#define TEXT_PROMPTPWD16    "手机密码设置失败"
//---------------------------------
#define TEXT_OLDPASSWORD	"输入密码:"
#define TEXT_OLDPWD         "原密码"
#define TEXT_NEWPASSWORD	"新密码"
#define TEXT_PWDCONFIRM		"确  认"
#define TEXT_PINREMAIN       "您还能再输入%d次"
#define TEXT_PUKPASSWORD     "PUK码"
#define TEXT_NEWPIN          "新PIN码"
#define TEXT_PINON           "启用"
#define TEXT_PINOFF          "取消"
#define TEXT_PINSTART        "请先启用PIN码"
/************************* end ****************************/

/********************** penCalib.c ************************/
#define TEXT_PENPROMPT1		"    如果您的设备没\n有对点击作出正确反\n应，则需要重新校准\n屏幕。\
                             \n    要开始再校准过\n程，请按确定。"
#define TEXT_CALIB			"校准"
#define TEXT_PENEXITBUTTON	"退出"
#define TEXT_PENEXIT		"S:笔校准"
/************************* end ****************************/

/************************ penwnd.c ************************/
#define TEXT_PENPROMPT2		"笔输入校准"
#define TEXT_PENPROMPT3		"笔输入校准失败"
#define TEXT_PENPROMPT4		"需重新校准"
#define TEXT_PENPROMPT5		"请点击左上角"
#define TEXT_PENPROMPT6		"请点击右下角"
#define TEXT_PENPROMPT7		"请点击中心"
#define TEXT_PENPROMPT8		"十字交叉处"
/************************* end ****************************/

/***************** ClrupFlh.c ************************************/
#define  MEMMGR_MESS_1  "整理碎片,请稍等..."
#define  MEMMGR_MESS_2  "整理碎片结束."
#define  MEMMGR_MESS_3  "整理碎片失败."
#define  MEMMGR_MESS_4  "整理碎片是比较耗时"
#define  MEMMGR_MESS_5  "的操作,整理时请耐心"
#define  MEMMGR_MESS_6  "等待，避免丢失数据。"
#define  MEMMGR_MESS_7  "                    "
#define  MEMMGR_MESS_8  "碎片整理过程中,退出无效!"
#define  MEMMGR_MESS_9  "创建窗口失败"

/********************* end ****************************************/
#endif