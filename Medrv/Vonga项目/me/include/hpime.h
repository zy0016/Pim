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

#define		IMESTATEBTN_BKCOLOR		RGB(254, 209, 122)		//״̬��ʾ���򱳾�ɫ
#define		IMESTATEBAR_BKCOLOR		RGB(100, 200,200)		//�ַ���ʾ���򱳾�ɫ
#define		IMEBUTTON_BKCOLOR		RGB(255, 100, 255)		//�ɵ�����򱳾�ɫ
#define		IMENAME_BKCOLOR			RGB(255, 200, 200)		//���뷨���Ʊ���ɫ
#define		IMEWINDOW_BKCOLOR		RGB(255, 255, 200)		//���뷨�Ӵ��ڱ���ɫ
#define		IME_CURSOR_COLOR		RGB(200, 200, 255)		//���뷨�ַ�ѡ�����ɫ
#define		IME_KEYDOWN_COLOR		RGB(200, 255, 200)		//���ܼ�����״̬��ɫ


//
//���뷨�����Ӧ�ó�����ýӿ�
//
DWORD	GetImeHandle(LPCSTR ImeName);
int		GetSpareWordNumber(HWND hEdit);
int		GetSmsWordNumber(HWND hSmsEdit);

typedef struct _tagInputEdit
{ //���INPUTEDIT�ؼ����������뷨������
	long lStyle;
	int  nKeyBoardType;
	char szName[12];
	char szMask[20];
	long	lImeStyle;		//���뷨����չ����
	long	dwImeParent;	//������������������	
}InputEdit;

//֧�����뷨�Ŀؼ����ʼ��
void	InputEdit_SuperClass(void);
void	DirectInputEdit_SuperClass(void);

//���뷨���������ں���
int	InputEdit_Modify(char* ach_title, char** ach_modify, int len_modify, 
		int pos_caret, DWORD dwStyle, DWORD hIme, char* input_mask, DWORD dwExtStyle, DWORD dwParent);
//int		InputEdit_Modify(char* , char* , int, int, DWORD, DWORD, char*);

//ֱ�ӽ������뷨������
DWORD ImeDirectCall(InputEdit InputEditNode, int iLength, char *szData);

//���뷨������˳�����
int		Ime_Exit (BOOL	bSave);
BOOL	IfImeExist (void);

//add  special messagebox for MPDA
//int WINAPI TipMessageBox(HWND hWnd, PCSTR pszText); //�Ѿ���MsgWin()���

//���뷨����̽ӿں���
#ifndef _SOFTKEYBOARD_HOPEN_
#define _SOFTKEYBOARD_HOPEN_ 
HWND	CreateSoftKeyboard(HWND hParent, LPCSTR ImeName);	//open one kind ime
BOOL	CloseSoftKeyboard(void);							//close all kind ime
#endif //_SOFTKEYBOARD_HOPEN_
//���뷨����ĵ��ýӿ�˵��������

#endif //_HOPEN_IME_COMMON_
/*	
* end of file
*/