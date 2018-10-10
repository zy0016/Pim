/**************************************************************************\
*
*                      Hopen Software System
*
* Copyright (c) 1999-2002 by Hopen Software Engineering Co., Ltd. 
*                       All Rights Reserved
*
* Model   :	ImeAdm.h
*
* Purpose :	���뷨����
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
#define		IME_DELETE_QUERYWIN				//���뷨�ַ���ɾ��ѯ�ʴ���
//#undef	IME_DELETE_QUERYWIN
//#define		IME_SCREEN_FORCEUPDATE			//���뷨��Ļǿ��ˢ��
//#undef	IME_SCREEN_FORCEUPDATE

#define		IME_MAX				16						//	the max num in system
#define		IME_STRLEN			12						//	the max num in system

#define		HW_BUF_NUM			512
#define		WM_SHOWCHAR			0x0601					//	notify ime window recognized over
#define		HW_CHR_BHL			0						//	0 lh(word order) 1 hl

#define		IME_DEFAULT_FILE	"ROM:/ime.ini"
#define		IME_SETTING_FILE	"FLASH2:/imeset.ini"


#define		WM_ENDINPUT			WM_USER+702		//���뷨�����洰�ڷ���
#define		WM_CLEARSTROKE		WM_USER+703		//�����д�ıʼ�
#define		WM_STARTINPUT		WM_USER+704		//�������뷨�����洰��
#define		WM_CLEARIMEWND		WM_USER+705		//������뷨�Ӵ�����ʾ����
#define		WM_HANDWFCHAR		WM_USER+706		//��д������ַ�ѡ����Ϣ

#define		WM_IMESETHOOK		WM_USER+707		//���뷨���ڵĹ���������Ϣ
//���뷨ģ�����Ϣ����״ֵ̬
#define		IMEWIN_ACT			10		//����ģ�鴰�ڼ���
#define		IMEWIN_ACTHOOK		20		//�������봰�ڱ���Ϣ����
#define		IMEWIN_UNHOOK		30		//ж�����봰�ڱ���Ϣ����

#define		IMEWND_ACTIVATE		WM_USER+708		// wParam refer to definition of WM_IMESETHOOK

#define		IMEWND_PENDOWN		WM_USER+710
#define		IMEWND_PENMOVE		WM_USER+711
#define		IMEWND_PENUP		WM_USER+712

#define		IME_STYLE_NUMBERFIRST		0x00008000L		//Ӣ������ʱ������������
#define		IME_STYLE_NORETURNKEY		0x00004000L		//Ӣ������ʱ������������


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

//��ע�����뷨����Ϣ����
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

//���뷨��Ĭ����Ϣ
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
#define	MES_DYNAMIC			0Xf2					//	��̬���뷨ģ��

#define	MD_DEFAULT			0x01				//	Ĭ�����뷨ģ��
#define	MD_NORMAL			0x02				//	��ͨ���뷨ģ��

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

//���뷨�����洰����Ϣ������
typedef struct tagImeMainwinList
{
	HWND	edit_hwnd;
	HWND	main_hwnd;			//���뷨�������ھ��
	HWND	parent_hwnd;		//�������뷨����������
	HWND	ime_hwnd;
	BYTE	bIme_main;
	HWND	hEnglishIme;
	HWND	hSymbolIme;
	HWND	hPinyinIme;
	HWND	hHandwfIme;
	int		len_return;		//�ַ����ĳ���
	BYTE	msg_unloop;		//���뷨������Ϣ����ֹͣ��־
	char**	ach_return;		//�����������ǰ���ַ������ݵĵ�ַ
	HIME	hCurrentIme;	//��ǰ������ģ����
	IMEINFO	aImeRegisterTab[IME_MAX];	//���뷨�Ĵ�����Ϣ	
	struct tagImeMainwinList *Next;		
} *PIMEMAINWINLIST, IMEMAINWINLIST;

//���뷨�����洰����Ϣ������ͷ
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

//���뷨���ڳ�ʼ����ע�ᴰ����
//HWND	Ime_Create(void);	//���뷨�ĳ�ʼ�����
DWORD	CreateEnglishIme(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);
DWORD	CreateSymbolIme(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);
DWORD	CreateHandwringIme(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);
DWORD	CreatePinYinIme(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);

//���뷨���������ں���
int		InputEdit_Modify(char* ach_title, char** ach_modify, int len_modify, int pos_caret, 
				DWORD dwStyle, HIME hIme, char *input_mask, DWORD dwExtStyle, DWORD dwParent);

//���뷨����ģ��Ľӿڶ���
BOOL	GetImeListFromProfile (PLISTOFSYSIME pRegisterIme);	//��ȡ���뷨��ע����Ϣ
BOOL	AddItemToSystem (char* strImeName );	//���뷨��ϵͳ��Ϣ�������
BOOL	DeleteItemFromSystem (char  *strImeName);	//���뷨��ϵͳ��Ϣ����ɾ��
void	UpdateDynamicInputModule (void);	//��̬����ģ����Ϣ����
BOOL	SetDefaultIme (LPCSTR  lpImeName);	//�ı����뷨ģ�������
BOOL	GetDefaultIme (PIMEDEFAULTINFO pDefaultInfo);	//������뷨��Ĭ�����

int		AddNewImeMainwinList(void);				//���뷨����������Ľڵ����
int		DeleteImeMainwinList(void);				//���뷨����������Ľڵ�ɾ��
int		RestoreImeMainwin(void);				//����
int		EnumerateImeMainwin(void);				//������洰�ڵĸ���

//���뷨�����湫���������ʺ���
BOOL	GetImeSysInfo(IMMINFO* pInfo);
BOOL	SetImeSysInfo(IMMINFO* pInfo);

// �����ֿ�
typedef struct tagPYLX
{
	WORD	wPYLX;			// �����ֵ���λ��
	WORD	wPYLXStart;		// �������ֿ��е���ʼ��ַ
	WORD	wPYLXCount;		// �������ֿ��еĸ���
}PYLXINDEX;

#endif  // _IMEDECLARATION_EXIST_

/*	File over
*/