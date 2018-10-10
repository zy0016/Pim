/**************************************************************************\
*
*                      Hopen Software System
*
* Copyright (c) 1999-2002 by Hopen Software Engineering Co., Ltd. 
*                       All Rights Reserved
*
* Model   :	ImeAdm.h
*
* Purpose :	输入法管理
*  
* Author  : 
*
*-------------------------------------------------------------------------
*
* $Archive::                                                       $
* $Workfile::                                                      $
* $Revision::    $     $Date::                                     $
* 
\**************************************************************************/
#ifndef _IMEDECLARATION_EXIST_
#define _IMEDECLARATION_EXIST_

#include "window.h" 
#include "hpime.h"

/*	define IME const
*/
#define		IME_DELETE_QUERYWIN				//输入法字符串删除询问窗口
//#undef	IME_DELETE_QUERYWIN
//#define		IME_SCREEN_FORCEUPDATE			//输入法屏幕强制刷新
//#undef	IME_SCREEN_FORCEUPDATE

#define		IME_MAX				16						//	the max num in system
#define		IME_STRLEN			12						//	the max num in system

#define		HW_BUF_NUM			512
#define		WM_SHOWCHAR			0x0601					//	notify ime window recognized over
#define		HW_CHR_BHL			0						//	0 lh(word order) 1 hl

#define		IME_DEFAULT_FILE	"ROM:/ime.ini"
#define		IME_SETTING_FILE	"FLASH2:/imeset.ini"


#define		WM_ENDINPUT			WM_USER+702		//输入法主界面窗口返回
#define		WM_CLEARSTROKE		WM_USER+703		//清除手写的笔迹
#define		WM_STARTINPUT		WM_USER+704		//弹出输入法主界面窗口
#define		WM_CLEARIMEWND		WM_USER+705		//清除输入法子窗口显示区域
#define		WM_HANDWFCHAR		WM_USER+706		//手写输入的字符选择消息

#define		WM_IMESETHOOK		WM_USER+707		//输入法窗口的钩子设置消息
//输入法模块笔消息钩子状态值
#define		IMEWIN_ACT			10		//输入模块窗口激活
#define		IMEWIN_ACTHOOK		20		//生成输入窗口笔消息钩子
#define		IMEWIN_UNHOOK		30		//卸载输入窗口笔消息钩子

#define		IMEWND_ACTIVATE		WM_USER+708		// wParam refer to definition of WM_IMESETHOOK

#define		IMEWND_PENDOWN		WM_USER+710
#define		IMEWND_PENMOVE		WM_USER+711
#define		IMEWND_PENUP		WM_USER+712

#define		IME_STYLE_NUMBERFIRST		0x00008000L		//英数输入时数字输入优先
#define		IME_STYLE_NORETURNKEY		0x00004000L		//英数输入时数字输入优先


/*	define ime register struct type
*/
typedef struct tagImeRegisterInfo
{	
	DWORD   wParam;									//	Ime win handle
	DWORD   lParam;									//	hold Param															
	int		(*Recv_ImeMessage)(int dwMsgCmd, WPARAM wParam, LPARAM lParam);
	char	achImeName[IME_STRLEN];					//	Ime Name
	BYTE    idMajor;								//	major version
	BYTE    idMinor;								//  minor version
	BYTE	dwStyle;								//	Ime attribute
	BYTE	dwStyleEx;								//	extern arrtible
}IMEINFO, *PIMEINFO;								//	ime register info type

typedef DWORD (*IMECONTROL)(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);

//已注册输入法的信息链表
typedef struct tagListOfSysime
{
	int		nTotalNum ;	
	struct  tagNodeOfImeList  *Next ;
} *PLISTOFSYSIME, LISTOFSYSIME ;

typedef struct tagNodeOfImeList
{
	int	   nDeleteState ;
	int    nLoadStyle ;
	int    nAttribute ;
	char   sInputModuleName[IME_STRLEN] ;
	struct tagNodeOfImeList  *Next ;
} *PNODEOFIMELIST, NODEOFIMELIST ;

//输入法的默认信息
typedef struct tagImeDefaultInfo
{
	char	strLanguage[20];
	char*	strImeName;
} *PIMEDEFAULTINFO, IMEDEFAULTINFO;

typedef struct tagIme
{
    char		achName[16];               
    IMECONTROL	imeControl;  
	WPARAM		wParam;
	LPARAM		lParam;
} IMEENTRY, *PIMEENTRY;

typedef struct 
{
    BYTE aCharPt[HW_BUF_NUM]; 
    int  nCharPtNum;
    BOOL bAlphaNumOnly;
}HWTRACE, *PTRACE;

typedef struct tagHWS_matchInfo
{
	HWND    hWnd;									//	which window word to send
	char*	pResult;								//	recognized buffer
	PTRACE	pTrace;									//	save points of strokes
	BYTE*	bstop;									//	the addr of ban flag 
	BYTE    nMatch;									//	recognized number;
	BYTE    nLevel;									//	hanwang recognized area
}HWMATCH, *PHWMATCH;

typedef struct tagHWS_settingInfo
{	
	int		nTime;									//	waite timer
	BYTE	bMatch;									//	whethere permit recoginzed
	BYTE    nMatch;									//	recognized number;
	BYTE	nLevel;									//	hanwang recognise degree
	BYTE	bEnglish;								//	chang word to english or num
}HWSET, PHWSET;

/*	ime  attribute flags
*/
#define MD_DISABLE			0x1f					//	forbidden state
#define MD_ENABLE			0x2f					//	promit state

#define MES_SYSTEM			0xf1					//	system style
#define	MES_DYNAMIC			0Xf2					//	动态输入法模块

#define	MD_DEFAULT			0x01				//	默认输入法模块
#define	MD_NORMAL			0x02				//	普通输入法模块

/*	The Message code of manager send to ime
*/
#define	IME_SHOW			0x1001					//	SHOW ime child window
#define	IME_HIDE			0x1002					//	HIDE ime child window
#define	IME_DESTROY			0x1003					//	destory ime child window
#define	IME_ERROR			0x1004					//	error occur
#define IME_CREATE			0x1005					//	create ime module window

/* The error cod
*/
#define IMEERR_ARDREG       0x0001                  //  the ime aready register
#define IMEERR_URSED        0x0002					//	the ime used now 
#define IMEERR_SYSTEM       0x0003					//	the system ime can't unregister

/*	define ime handle type
*/
typedef DWORD HIME;									//	ime handle type

/*	define ime register struct type
*/
typedef struct tagImeFunctionTable
{
	int		(*Ime_Register)(const PIMEINFO pInfo);
	int		(*Ime_UnRegister)(HIME hIme);
	int     (*Ime_Open)(HIME hIme);
	int     (*Ime_Close)(HIME hIme);
	int		(*Ime_GetOpen)(void);
	int		(*Ime_Enable)(HIME hIme, BOOL bEnable);	
	int		(*Ime_GetInfo)(HIME hIme, PIMEINFO imeInfo);
	BOOL	(*Ime_SetInfo)(HIME hIme, PIMEINFO imeInfo);
	HIME    (*Ime_GetHandle)(LPCSTR ImeName);
	int		(*Ime_Enum)(int wParam, int lParam);	
	int     (*Ime_SetHook)(BOOL whook);
	int     (*Ime_Convert)(void);
	int		(*Ime_Run)(HWND hParent, HIME hIme);
} IMEFUNCTABLE, *PIMEFUNCTABLE;				//	ime register info type

/*	define system information of the main window of the ime 	
*/
typedef struct tagImmInfo
{
	HWND	hMainWnd;	//main window of the ime
	HWND	hEditWnd;	//edit window of the ime
	HWND	hImeWnd;	//the current ime module
	BYTE	bId;		//the identify number of ime module
	WPARAM	wParam;		//reserved	
	LPARAM	lParam;		//reserved
}IMMINFO, *PIMMINFO;

//输入法主界面窗口信息单链表
typedef struct tagImeMainwinList
{
	HWND	edit_hwnd;
	HWND	main_hwnd;			//输入法弹出窗口句柄
	HWND	parent_hwnd;		//调用输入法的属主窗口
	HWND	ime_hwnd;
	BYTE	bIme_main;
	HWND	hEnglishIme;
	HWND	hSymbolIme;
	HWND	hPinyinIme;
	HWND	hHandwfIme;
	int		len_return;		//字符串的长度
	BYTE	msg_unloop;		//输入法界面消息阻塞停止标志
	char**	ach_return;		//进入输入界面前的字符串内容的地址
	HIME	hCurrentIme;	//当前的输入模块句柄
	IMEINFO	aImeRegisterTab[IME_MAX];	//输入法的窗口信息	
	struct tagImeMainwinList *Next;		
} *PIMEMAINWINLIST, IMEMAINWINLIST;

//输入法主界面窗口信息单链表头
typedef struct tagImeMainwinListHead 
{
	int	   nNumOfMainwin;
	struct tagImeMainwinList *Next;	
} *PIMEMAINWINLISTHEAD, IMEMAINWINLISTHEAD;

/*	extern interface
*/
extern	BOOL	HWs_initialize(void);
extern	void	HWs_SetMatch(PHWMATCH pmatch);

BOOL	ImeGetInterface(PIMEFUNCTABLE pTable);		//	open one register ime

//输入法窗口初始化，注册窗口类
//HWND	Ime_Create(void);	//输入法的初始化入口
DWORD	CreateEnglishIme(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);
DWORD	CreateSymbolIme(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);
DWORD	CreateHandwringIme(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);
DWORD	CreatePinYinIme(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);

//输入法主界面的入口函数
int		InputEdit_Modify(char* ach_title, char** ach_modify, int len_modify, int pos_caret, 
				DWORD dwStyle, HIME hIme, char *input_mask, DWORD dwExtStyle, DWORD dwParent);

//输入法管理模块的接口定义
BOOL	GetImeListFromProfile (PLISTOFSYSIME pRegisterIme);	//获取输入法的注册信息
BOOL	AddItemToSystem (char* strImeName );	//输入法的系统信息链表添加
BOOL	DeleteItemFromSystem (char  *strImeName);	//输入法的系统信息链表删除
void	UpdateDynamicInputModule (void);	//动态输入模块信息更新
BOOL	SetDefaultIme (LPCSTR  lpImeName);	//改变输入法模块的属性
BOOL	GetDefaultIme (PIMEDEFAULTINFO pDefaultInfo);	//获得输入法的默认情况

int		AddNewImeMainwinList(void);				//输入法主窗口链表的节点添加
int		DeleteImeMainwinList(void);				//输入法主窗口链表的节点删除
int		RestoreImeMainwin(void);				//待用
int		EnumerateImeMainwin(void);				//输入界面窗口的个数

//输入法主界面公共变量访问函数
BOOL	GetImeSysInfo(IMMINFO* pInfo);
BOOL	SetImeSysInfo(IMMINFO* pInfo);

// 联想字库
typedef struct tagPYLX
{
	WORD	wPYLX;			// 索引字的区位码
	WORD	wPYLXStart;		// 在联想字库中的起始地址
	WORD	wPYLXCount;		// 在联想字库中的个数
}PYLXINDEX;

#endif  // _IMEDECLARATION_EXIST_

/*	File over
*/