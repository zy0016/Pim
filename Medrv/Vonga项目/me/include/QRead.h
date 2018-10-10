/**********************************************************************\
 *
 *                      <���������ֻ�>
 *
 * Copyright (c) 1999-2001 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * ģ�飺	   ��д����
 *
 * �ļ���:     QRead.h
 *
 * ˵�� :      C Header
 *  
 * ����  :     ������
\**********************************************************************/

/**********************************************
*
*	��������
*
***********************************************/
#define	QREAD_FILE	"RAM:QRead.dat"
#define	QREAD_SEEK_ORD		0			//��������ѯ
#define	QREAD_SEEK_NUM		1			//�������ѯ
#define	QREAD_SEEK_TEXT		2			//��������ѯ

/**********************************************
*
*	��������
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
*	�˵��ؼ�ID
*
***********************************************/

/**********************************************
*
*	�Զ�����Ϣ
*
***********************************************/

/**********************************************
*
*	�˵�ģ��
*
***********************************************/
