/**********************************************************************\
 *
 *                      <联想智能手机>
 *
 * Copyright (c) 1999-2001 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * 模块：	   读写提速
 *
 * 文件名:     QRead.h
 *
 * 说明 :      C Header
 *  
 * 作者  :     郭新松
\**********************************************************************/

/**********************************************
*
*	常量定义
*
***********************************************/
#define	QREAD_FILE	"RAM:QRead.dat"
#define	QREAD_SEEK_ORD		0			//按索引查询
#define	QREAD_SEEK_NUM		1			//按号码查询
#define	QREAD_SEEK_TEXT		2			//按姓名查询

/**********************************************
*
*	函数声明
*
***********************************************/
BOOL	InitQReadSIM(HWND hParent);
LRESULT CALLBACK QReadSIMRecordWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,LPARAM lParam);
BOOL CreateQReadFile(PHONEBOOK* Content);//,int nMax
BOOL ReadQReadFile(PHONEBOOK* Content,int nMax);
BOOL WriteQReadFile(PHONEBOOK* Content);
int ItemNumQReadFile();
BOOL SeekQReadFile(PHONEBOOK* Content,int nMode);
BOOL IsQreadInit();

/**********************************************
*
*	菜单控件ID
*
***********************************************/

/**********************************************
*
*	自定义消息
*
***********************************************/

/**********************************************
*
*	菜单模板
*
***********************************************/
