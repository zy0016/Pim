#ifndef _HPPDAEX_H
#define _HPPDAEX_H

#include "window.h"
#include "hp_modcfg.h"
#include "hpime.h"


//����λ�ö���
#define HP_WIN_POSITION   0,24,176,196 
//��ʾ����ͳһ�ȴ�ʱ��
#define WAITTIMEOUT			30 
#define PS_SETTIME			35
#define SMARTPHONE2
   

HWND hTittleWnd;
/*********************  �����������Ӧ�ó�����Ҫ���õ���Ϣ���� *************************/
/*********************  ������ԭ���ӿڲ����Ĵ���ͳһ           *************************/

/***	���������ʹ�õ���Ϣ ***/

#define		WM_PROGRAM		 WM_USER+50
#define		WM_TOOLBAR		(WM_PROGRAM+1)
#define		WM_PENWND_QUIT	(WM_PROGRAM+3)
//#define		WM_DATEWND_QUIT	(WM_PROGRAM+4)
#define		WM_BATTERY      (WM_PROGRAM+5)
#define		WM_TRANSFER		(WM_PROGRAM+7)
#define		WM_APPNOTIFY	(WM_PROGRAM+8)

//�˳���Ϣ
#define     WM_QUERYSYSEXIT  (WM_PROGRAM+20)
#define     WM_SYSEXIT       (WM_PROGRAM+21)

#define     SYSEXIT_CONTINUE      100
#define     SYSEXIT_ABORT         101
#define     SYSEXIT_WAITREPLAY    102

//Set Caption and Staticbar colors
#define CAPTIONCOLOR          RGB(128,255,255)
#define STATICBARCOLOR        RGB(0,128,255)
#define FOCUSCOLOR            RGB(0,0,128)

/***	�����������Ӧ�ó��������Ϣ ***/
#define		PES_APP_QUIT		2	//	Ӧ�ó����˳�
#define		PES_APP_HIDE		3	//	���ص�ǰ����
#define		PES_APP_SHOW		4	//	��ʾһ��ָ����Ӧ�ó���
#define		PES_APP_CALL		5	//	����һ��ָ����Ӧ�ó�������
#define		PES_APP_CREATE		6	//	����һ��ָ����Ӧ�ó���
#define     PES_SYN_QUIT        7
#define     PES_IME_REGISTER    10  //  ע��һ�����뷨
#define     PES_IME_UNREGISTER  11  //  ע��һ�����뷨

#define		PES_STCQUIT			PES_APP_QUIT
#define     PES_DLMQUIT        	PES_APP_QUIT							
#define		PES_APPRUN			PES_APP_CALL
#define		PES_APPHIDE			PES_APP_HIDE
#define		PES_APPSHOW			PES_APP_SHOW
#define     PES_APPSTART		PES_APP_CREATE

/***	���������ΪWAP Browser & MMS�������Ϣ ***/

#define     PM_SMS           WM_USER + 1000	
#define     PM_MMS           WM_USER + 1001 

#define     NS_IDLE           0     //����״̬����
#define     NS_ONLINE_WAPB    1     //Wap����
#define     NS_ONLINE_MMS     3     //MMS����

#define     NS_DIS_WAPB       2     //Wap����֪ͨ
#define     NS_DIS_MMS        4     //MMS����֪ͨ

int   GetWapNetStates();
void  SetWapNetStates(int iState);

/***	������ڲ���ֵ ***/
#define		APPFILE_HIDE		0x0002  //  �����ļ�����
#define		APPFILE_AUTORUN		0x0001  //  �������ļ�����

#define		IME_FILE			0x10	//	���뷨��ִ���ļ�
#define		APP_FILE			0x00	//	Ӧ�ó����ִ���ļ�

/***   ����ʹ�õ���Ϣֵ  ***/
#define		APP_SYSTEM			2000
#define		APP_INIT            0	//	app initialize		
#define		APP_INITINSTANCE    1	//	app initialize hinstance used in dynamic
#define		APP_ACTIVE          2	//	app active and show app interface
#define		APP_INACTIVE        3	//	app hide interface
#define     APP_INACTIVE_NETCAT    1
#define     APP_GETOPTION       4
#define     APP_SETOPTION       5
#define		APP_DESTROY			16	//	app destroy	,call by other app
#define     APP_EXIT            10  //  ��̬���ض������Ϣ 

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
#define AS_IMEINFO          6  //���뷨���壬�������뷨��Ϣ

#define AS_APPWND           7

#define WM_RINGMSG    WM_USER+1001
#define DIRECT_DIAL   WM_USER+1002

/********************  �ṹ������� ************************************************/

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
**	ȫ�ֱ������ýӿ�
*/

DWORD	PDA_InitAppGlobalVarBlock(DWORD nSize);
DWORD	PDA_SetAppGlobalVar(DWORD nType, DWORD nValue);
DWORD	PDA_GetAppGlobalVar(DWORD nType);

typedef struct tagSMSData
{
	char* pSour;		//Դ��ַ�����Ƿ����ߵĵ绰���룬�ַ���
	char* pDest;		//Ŀ�ĵ�ַ�����ǽ����ߵĵ绰���루�������ģ����ַ���
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

//�����������������
typedef enum {
    PROG_IDLETIME,
    PROG_IDLEPLNM,
    PROG_IDLEPROMPT,
    PROG_IDLEDYNM
};
extern void Prog_SetDeskIdle(int iType, int iOffsetX, int iOffsetY);

/*
**	��̬���ؽӿ�
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

typedef	struct	_SHFILEINFO{//	�ļ��ӵ����ݽṹ

//	unsigned long	hBitmap;					//	�ļ�ͼ��
	HICON	 hIcon;	
	char	 szName[NAPP_NAME_PATH];			//	�ļ���ʾ��	1:appname

} SHFILEINFO;

DWORD WINAPI SHGetFileInfo(LPCTSTR pszPath, DWORD dwFileAttributes, 
						   SHFILEINFO *psfi, UINT cbFileInfo, UINT uFlags);

HINSTANCE	LoadModule(HINSTANCE hInstance, LPCSTR pName);



/**********�������Žӿ�********************************************************/
int PlayMusic ( char * pFileName, int StartPoint, int PlayMode);
int PlayStopAll (int seqHd);

//ѡ�񴰿� add by yizhang
extern void MsgSelWin( HWND hCall, char *szPrompt, char *szCaption );
#define			WM_SELCANCEL				WM_USER + 551
#define			WM_SELOK					WM_USER + 552
//end by yizhang

//��ɫ�ؼ��ӿ�
#define WM_COLOR_SELECT	(WM_USER+12345)
extern  int ReadDefaultColorFile();
extern  void WriteDefaultColorFile();
extern HWND InitColorWin(HWND hWndB);

//�ռ��жϷ���״ֵ̬
#define SPACE_CHECKERROR     0
#define SPACE_AVAILABLE      1
#define SPACE_NEEDCLEAR      2
#define SPACE_NEEDDELFILE    3
int IsFlashAvailable( int nSize );

/************ ֪ͨ״̬��ͼ��Ļ������  ********/
#define ICON_SET            50
#define ICON_CANCEL         51

#define ICON_MODE_STD       52    //�龰ģʽ ��׼
#define ICON_MODE_OUT       53    //  ����
#define ICON_MODE_WORK      54    //  �칫 
#define ICON_MODE_REST      55    //  ����
#define ICON_MODE_MEET      56    //  ����
#define ICON_MODE_SELF      57    //  �Զ���

#define SMS_NONE            0
#define SMS_IN              1
#define SMS_INFULL          2

/* ��ʱ����
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

/*    ��ʱ����
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

/********* �� �ػ����� **********************/
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
	WORD bMusic;   //���� �ػ� ���� �����Ƿ񲥷�����

	char *PowerOnMusic;
	char *PowerOffMusic;
	char *SmsMusic;
	char *RingMusic;	
}MUSICINFO, *PMUSICINFO;

LONG GetMusicInfo(int index);
LONG SetMusicInfo(int index, LONG lNewValue);

/**************** ��ʾ��Ϣ ********************/
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