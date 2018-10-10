/**************************************************************************\
*
*                      Hopen Software System
*
* Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
*                       All Rights Reserved
*
* Model   :			控件
*
* Purpose :			游戏帮助的控件
*  
* Author  :			郭新松
*
*-------------------------------------------------------------------------
*
* $Archive::                                                       $
* $Workfile::                                                      $
* $Revision::    $     $Date::                                     $
* 
\**************************************************************************/
#ifndef	_GAMEHELP_
#define	_GAMEHELP_

#include "window.h"
/* GameHelp definition start */
BOOL GAMEHELP_RegisterClass(void*);

/*GameHelp Control*/
typedef struct tagSTRU_GAMEHELP
{
    BOOL    bFocus;
	BOOL	bSetLBtn;
	WORD	wLBtnID;
    WORD    wID;
    HWND    hParentWnd;
	HWND	hWnd_MyEdit;
	RECT	RectScroll;
	WNDPROC	OldEditProc;
	int		nCurLen;			//当前的第一行是整个文本的行数，0为基数
}STRU_GAMEHELP, *PSTRU_GAMEHELP;


//GameHelp message
#define	GHP_SETTEXT		(WM_USER+10)
#define	GHP_SETLBTN		(WM_USER+11)

//GameHelp Notify message
#define NHP_SETFOCUS    1
#define NHP_KILLFOCUS   2

//GameHelp's macro
#define WC_GAMEHELP   "GXSGAMEHELPWNDCLASS"

#define	SetGameHelpText(hWnd,lParam)	\
	SendMessage(hWnd,GHP_SETTEXT,NULL,lParam)

#endif	//_GAMEHELP_