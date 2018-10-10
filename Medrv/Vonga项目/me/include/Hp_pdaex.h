#ifndef _HPPDAEX_H
#define _HPPDAEX_H

#include "window.h"
#include "hp_modcfg.h"
#include "hpime.h"


//窗口位置定义
#define HP_WIN_POSITION   0,24,176,196 
//提示窗口统一等待时间
#define WAITTIMEOUT			30 
#define PS_SETTIME			35
#define SMARTPHONE2
   

HWND hTittleWnd;
/*********************  程序管理器，应用程序需要调用的消息代码 *************************/
/*********************  保持与原来接口参数的代码统一           *************************/

/***	程序管理器使用的消息 ***/

#define		WM_PROGRAM		 WM_USER+50
#define		WM_TOOLBAR		(WM_PROGRAM+1)
#define		WM_PENWND_QUIT	(WM_PROGRAM+3)
//#define		WM_DATEWND_QUIT	(WM_PROGRAM+4)
#define		WM_BATTERY      (WM_PROGRAM+5)
#define		WM_TRANSFER		(WM_PROGRAM+7)
#define		WM_APPNOTIFY	(WM_PROGRAM+8)

//退出消息
#define     WM_QUERYSYSEXIT  (WM_PROGRAM+20)
#define     WM_SYSEXIT       (WM_PROGRAM+21)

#define     SYSEXIT_CONTINUE      100
#define     SYSEXIT_ABORT         101
#define     SYSEXIT_WAITREPLAY    102

//Set Caption and Staticbar colors
#define CAPTIONCOLOR          RGB(128,255,255)
#define STATICBARCOLOR        RGB(0,128,255)
#define FOCUSCOLOR            RGB(0,0,128)

/***	程序管理器与应用程序调用消息 ***/
#define		PES_APP_QUIT		2	//	应用程序退出
#define		PES_APP_HIDE		3	//	隐藏当前程序
#define		PES_APP_SHOW		4	//	显示一个指定的应用程序
#define		PES_APP_CALL		5	//	调用一个指定的应用程序的入口
#define		PES_APP_CREATE		6	//	创建一个指定的应用程序
#define     PES_SYN_QUIT        7
#define     PES_IME_REGISTER    10  //  注册一个输入法
#define     PES_IME_UNREGISTER  11  //  注销一个输入法

#define		PES_STCQUIT			PES_APP_QUIT
#define     PES_DLMQUIT        	PES_APP_QUIT							
#define		PES_APPRUN			PES_APP_CALL
#define		PES_APPHIDE			PES_APP_HIDE
#define		PES_APPSHOW			PES_APP_SHOW
#define     PES_APPSTART		PES_APP_CREATE

/***	程序管理器为WAP Browser & MMS定义的消息 ***/

#define     PM_SMS           WM_USER + 1000	
#define     PM_MMS           WM_USER + 1001 

#define     NS_IDLE           0     //网络状态空闲
#define     NS_ONLINE_WAPB    1     //Wap在线
#define     NS_ONLINE_MMS     3     //MMS在线

#define     NS_DIS_WAPB       2     //Wap断线通知
#define     NS_DIS_MMS        4     //MMS断线通知

int   GetWapNetStates();
void  SetWapNetStates(int iState);

/***	程序入口参数值 ***/
#define		APPFILE_HIDE		0x0002  //  隐藏文件类型
#define		APPFILE_AUTORUN		0x0001  //  自启动文件类型

#define		IME_FILE			0x10	//	输入法可执行文件
#define		APP_FILE			0x00	//	应用程序可执行文件

/***   程序使用的消息值  ***/
#define		APP_SYSTEM			2000
#define		APP_INIT            0	//	app initialize		
#define		APP_INITINSTANCE    1	//	app initialize hinstance used in dynamic
#define		APP_ACTIVE          2	//	app active and show app interface
#define		APP_INACTIVE        3	//	app hide interface
#define     APP_INACTIVE_NETCAT    1
#define     APP_GETOPTION       4
#define     APP_SETOPTION       5
#define		APP_DESTROY			16	//	app destroy	,call by other app
#define     APP_EXIT            10  //  动态加载定义的消息 

#define     DLL_INIT			6
#define     DLL_INITINSTANCE	7
#define     DLL_GETFUNADD		8
#define     DLL_EXIT			9

#define		APP_GETICON			(APP_SYSTEM+1)	//	app get info
#define		APP_GETVERSION		(APP_SYSTEM+2)	//	app get version
#define		APP_GETDATA			(APP_SYSTEM+3)	//	app get data
#define		APP_SETDATA			(APP_SYSTEM+4)	//	app Set data
#define		APP_GETHELP			(APP_SYSTEM+5)	//	app get info

#define AS_NOTRUNNING       0
#define AS_INACTIVE         1
#define AS_ACTIVE           2
#define AS_HELP				3
#define AS_ICON				4
#define AS_STATE			5
#define AS_IMEINFO          6  //输入法定义，接受输入法信息

#define AS_APPWND           7

#define WM_RINGMSG    WM_USER+1001
#define DIRECT_DIAL   WM_USER+1002

/********************  结构定义结束 ************************************************/

//add by yizhang for ignor switching focus of control in wml browser
#define CS_NOSYSCTRL		0x20000000L

//end by yizhang

HWND    GetDesktopWindow(void);
BOOL	Init_WaitWindow(void);
void    WaitWindowState(HWND hParent, BOOL bShow);
BOOL	IsWaitWindowShow();

void Play(char *pName);

void EnableFastKey( void );
void DisableFastKey( void );
void EnableTouchpen( void );
void DisableTouchpen ( void );

#define WM_CHECKCURSOR	WM_USER + 700

//FOR DEMO	ADD BY YIZHANG
#define WM_ME_INIT		WM_USER + 800
#define ME_SUCCESS    0
#define ME_ERROR      1
//END BY YIZHANG


/*----- Program Manager interface function declare ---------*/

typedef struct tagAPPENTRY{
	DWORD	    	nCode;	
	WPARAM		    wParam;
	LPARAM		    lParam;
	HINSTANCE		hApp;
	char*		    pAppName;
} APPENTRY, *PAPPENTRY;


extern  DWORD RMonitorNotify(DWORD Param1, DWORD Param2);
extern  BOOL GetDIALState();
extern	BOOL GetMultState();

#define MSG_NO_CARRIER			(10013)
#define WM_SETUPUPDATE		WM_USER+10

//Window message to change right button text for edit 
#define WM_EDITCLEAR	WM_USER + 500
#define WM_EDITEXIT		WM_USER + 501
#define WM_SETRBTNTEXT	WM_USER + 503


/*
**	全局变量调用接口
*/

DWORD	PDA_InitAppGlobalVarBlock(DWORD nSize);
DWORD	PDA_SetAppGlobalVar(DWORD nType, DWORD nValue);
DWORD	PDA_GetAppGlobalVar(DWORD nType);

typedef struct tagSMSData
{
	char* pSour;		//源地址，就是发送者的电话号码，字符串
	char* pDest;		//目的地址，就是接收者的电话号码（短信中心），字符串
	unsigned long datalen;		//data length
	unsigned char* pdata;		//data point
}SMSData;

typedef unsigned long  (*PMSG_CALLBACK)(unsigned long  wMsg, \
										unsigned long  Param1, \
										unsigned long  Param2);
BOOL	    PMSG_MessageInit(void);
HANDLE      PMSG_RegisteWnd( DWORD nType, HWND hWnd , PMSG_CALLBACK msg_call);
BOOL        PMSG_UnRegisteWnd(DWORD nType, HANDLE handle);
BOOL	    PMSG_MessageSend(DWORD nType, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

#define		PDA_ACTION_POWERON	0	//
#define		PDA_ACTION_SOFTKBD	1	//
#define		PDA_ACTION_EXTCARD	2	//
#define		PDA_ACTION_FILE		3	//
#define		PHONE_SMS_IN	    4

void *		GetResourceSection(HINSTANCE hInstance, DWORD * pSize);
HWND		GetDesktopWindow( void );
BOOL WINAPI	RegisterDesktopWindow( HWND hWnd );

//新增程序管理器定义
typedef enum {
    PROG_IDLETIME,
    PROG_IDLEPLNM,
    PROG_IDLEPROMPT,
    PROG_IDLEDYNM
};
extern void Prog_SetDeskIdle(int iType, int iOffsetX, int iOffsetY);

/*
**	动态加载接口
*/
typedef		BOOL (*DLMFILEPROC)( LPCTSTR lpszFileName, long lUserData );
BOOL		EnumDlmFiles(DLMFILEPROC lpfnDlmFile, long lUserData);

DWORD		RunModule(DWORD type, HINSTANCE hInstance, DWORD param1, DWORD param2);
void		UnloadModule(HINSTANCE hInstance, HINSTANCE hModuleInst);
HINSTANCE   DllLoadLibrary(LPCSTR lpName);
DWORD       GetFuncAddress(HINSTANCE hInstance, LPCSTR lpFuncName);
void        DllFreeLibrary(HINSTANCE hInstance);

#define		HEAP_EXPANDABLE		0x10		/* Heap expandable */
//int			LocalCreateHeap (unsigned long heap_size, int flags); /* This is in static library */
//void		LocalDestroyHeap (void);
//int			CRT_InitInstance(HINSTANCE hInstance);

#ifdef	_EMULATE_

#define		DLMEXPORT	_declspec(dllexport)

#else

#define		DLMEXPORT

#endif //_EMULATE_

#define		SHGFI_ICON		0x55
#define		NAPP_NAME_PATH	12

typedef	struct	_SHFILEINFO{//	文件接点数据结构

//	unsigned long	hBitmap;					//	文件图标
	HICON	 hIcon;	
	char	 szName[NAPP_NAME_PATH];			//	文件显示名	1:appname

} SHFILEINFO;

DWORD WINAPI SHGetFileInfo(LPCTSTR pszPath, DWORD dwFileAttributes, 
						   SHFILEINFO *psfi, UINT cbFileInfo, UINT uFlags);

HINSTANCE	LoadModule(HINSTANCE hInstance, LPCSTR pName);



/**********声音播放接口********************************************************/
int PlayMusic ( char * pFileName, int StartPoint, int PlayMode);
int PlayStopAll (int seqHd);

//选择窗口 add by yizhang
extern void MsgSelWin( HWND hCall, char *szPrompt, char *szCaption );
#define			WM_SELCANCEL				WM_USER + 551
#define			WM_SELOK					WM_USER + 552
//end by yizhang

//颜色控件接口
#define WM_COLOR_SELECT	(WM_USER+12345)
extern  int ReadDefaultColorFile();
extern  void WriteDefaultColorFile();
extern HWND InitColorWin(HWND hWndB);

//空间判断返回状态值
#define SPACE_CHECKERROR     0
#define SPACE_AVAILABLE      1
#define SPACE_NEEDCLEAR      2
#define SPACE_NEEDDELFILE    3
int IsFlashAvailable( int nSize );

/************ 通知状态栏图标的绘制情况  ********/
#define ICON_SET            50
#define ICON_CANCEL         51

#define ICON_MODE_STD       52    //情景模式 标准
#define ICON_MODE_OUT       53    //  户外
#define ICON_MODE_WORK      54    //  办公 
#define ICON_MODE_REST      55    //  静音
#define ICON_MODE_MEET      56    //  会议
#define ICON_MODE_SELF      57    //  自定义

#define SMS_NONE            0
#define SMS_IN              1
#define SMS_INFULL          2

/* 暂时保留
#define MAIL_NEW			0		//lParam
#define MAIL_DISAPPEAR		1
// for PS_SINFO 
#define SI_NEW		        0		//lParam
#define SI_DISAPPEAR		1
*/ 

#define PS_SETSGN           61
#define PS_SETGPRS          62
#define PS_SETMODE          63
#define PS_SETALARM         64
#define PS_SETCALL          65
#define PS_SETTEL           66
#define PS_SETSMS           67
#define PS_SETBAT           68  

#define PS_SETDESKTOP       34      //set desktop
#define PS_SETLANGUAGE      42

/*    暂时保留
#define PS_BATTERY_CHARGE	31      //battery states
#define PS_MAIL				32      //mail symbol
#define PS_SINFO			33      //small information symbol
#define PS_SETTIME          35
#define PS_CALLDIALCON      37
#define PS_RUNDIALCON       38
#define PS_STOPDIALCON      39
*/
#define PS_REGRINGWND       40
/*
#define PS_GETREGRINGWND    41
#define PS_SETSMSTXT        50
#define PS_SETMMSTXT        50
*/

DWORD DlmNotify( int wParam, int lParam );

/********* 开 关机音乐 **********************/
#define MI_POWERON_OPEN       1     
#define MI_POWEROFF_OPEN      2
#define MI_SMS_OPEN           3
#define MI_RING_OPEN          4
#define MI_POWERON_MUSIC      5
#define MI_POWEROFF_MUSIC     6
#define MI_SMS_MUSIC          7
#define MI_RING_MUSIC         8

typedef struct _tagMusicInfo
{ 
	WORD bMusic;   //开机 关机 短信 来电是否播放音乐

	char *PowerOnMusic;
	char *PowerOffMusic;
	char *SmsMusic;
	char *RingMusic;	
}MUSICINFO, *PMUSICINFO;

LONG GetMusicInfo(int index);
LONG SetMusicInfo(int index, LONG lNewValue);

/**************** 提示消息 ********************/
#define   ANM_ADD      1
#define   ANM_DELETE   2
#define   ANM_MODIFY   3
typedef struct _AppNotifyData {
	DWORD cbSize;
	HWND hWnd;
	UINT uID;
	void (*CallbackFunc)(void);
	LPSTR strMessage;
} APPNOTIFYDATA, *PAPPNOTIFYDATA;
UINT  SHAppNotifyMessage(DWORD dwMessage, PAPPNOTIFYDATA pdata);

#endif // _HPPDAEX_H