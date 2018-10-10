/**************************************************************************\
*
*                      Hopen Software System
*
* Copyright (c) 1999-2003 by Hopen Software Engineering Co., Ltd. 
*                       All Rights Reserved
*
* Model   :imeapi.h
*
* Purpose : Ime Interface declaration for usual applications 
*  
* Author  : lur
*
*-------------------------------------------------------------------------
*
* $Archive::                                                       $
* $Workfile::                                                      $
* $Revision::    $     $Date:: 2003-7-4                            $
* 
\**************************************************************************/
#ifndef _HOPEN_IME_COMMON_
#define _HOPEN_IME_COMMON_

#include "window.h"

#define		IMESTATEBTN_BKCOLOR		RGB(254, 209, 122)		//状态提示区域背景色
#define		IMESTATEBAR_BKCOLOR		RGB(100, 200,200)		//字符提示区域背景色
#define		IMEBUTTON_BKCOLOR		RGB(255, 100, 255)		//可点击区域背景色
#define		IMENAME_BKCOLOR			RGB(255, 200, 200)		//输入法名称背景色
#define		IMEWINDOW_BKCOLOR		RGB(255, 255, 200)		//输入法子窗口背景色
#define		IME_CURSOR_COLOR		RGB(200, 200, 255)		//输入法字符选择块颜色
#define		IME_KEYDOWN_COLOR		RGB(200, 255, 200)		//功能键按下状态颜色


//
//输入法程序的应用程序调用接口
//
DWORD	GetImeHandle(LPCSTR ImeName);
int		GetSpareWordNumber(HWND hEdit);
int		GetSmsWordNumber(HWND hSmsEdit);

typedef struct _tagInputEdit
{ //点击INPUTEDIT控件，进入输入法主界面
	long lStyle;
	int  nKeyBoardType;
	char szName[12];
	char szMask[20];
	long	lImeStyle;		//输入法的扩展属性
	long	dwImeParent;	//调用输入界面的主窗口	
}InputEdit;

//支持输入法的控件类初始化
void	InputEdit_SuperClass(void);
void	DirectInputEdit_SuperClass(void);

//输入法主界面的入口函数
int	InputEdit_Modify(char* ach_title, char** ach_modify, int len_modify, 
		int pos_caret, DWORD dwStyle, DWORD hIme, char* input_mask, DWORD dwExtStyle, DWORD dwParent);
//int		InputEdit_Modify(char* , char* , int, int, DWORD, DWORD, char*);

//直接进入输入法主界面
DWORD ImeDirectCall(InputEdit InputEditNode, int iLength, char *szData);

//输入法程序的退出函数
int		Ime_Exit (BOOL	bSave);
BOOL	IfImeExist (void);

//add  special messagebox for MPDA
//int WINAPI TipMessageBox(HWND hWnd, PCSTR pszText); //已经被MsgWin()替代

//输入法软键盘接口函数
#ifndef _SOFTKEYBOARD_HOPEN_
#define _SOFTKEYBOARD_HOPEN_ 
HWND	CreateSoftKeyboard(HWND hParent, LPCSTR ImeName);	//open one kind ime
BOOL	CloseSoftKeyboard(void);							//close all kind ime
#endif //_SOFTKEYBOARD_HOPEN_
//输入法程序的调用接口说明，结束

#endif //_HOPEN_IME_COMMON_
/*	
* end of file
*/