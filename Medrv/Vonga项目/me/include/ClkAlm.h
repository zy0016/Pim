
/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :	���ӱ���
 *
 * Filename:    ClkAlm.h
 *
 * Purpose :    ���ӱ���ά��ģ���ⲿ�ӿ�����
 *  
 * Author  :    JQiao
 *
 *-------------------------------------------------------------------------
 *
 * $Archive:: /RWP_SRC_Emu/include/ClkAlm.h                         $
 * $Revision:: 8  $     $Date:: 01-10-26 16:48                      $
 *
 * $History:: ClkAlm.h                                              $
 * 
 * *****************  Version 8  *****************
 * User: Jqiao        Date: 01-10-26   Time: 16:48
 * Updated in $/RWP_SRC_Emu/include
 * 
 * *****************  Version 7  *****************
 * User: Jqiao        Date: 01-10-13   Time: 14:23
 * Updated in $/RWP_SRC_Emu/include
 * 
 * *****************  Version 6  *****************
 * User: Jqiao        Date: 01-06-14   Time: 14:48
 * Updated in $/RWP_SRC_Emu/include
 * �ύAlpha5�汾����
 * 
 * *****************  Version 5  *****************
 * User: Jqiao        Date: 01-05-30   Time: 15:44
 * Updated in $/RWP_SRC_Emu/include
 * 1.�������ӱ����Զ�����Ϣ
 * 2 ����ALM_NODE�ṹMsg��������С
 * 
 * *****************  Version 3  *****************
 * User: Jqiao        Date: 01-05-09   Time: 17:24
 * Updated in $/RWP_SRC_Emu/Pda_app/ClockAlm
 * 1.�޸��������ֶ�bOrigHour��bOrigMinute������ΪbBegHour��
 *   bBegMinute
 * 
 * *****************  Version 2  *****************
 * User: Jqiao        Date: 01-05-09   Time: 17:16
 * Updated in $/RWP_SRC_Emu/Pda_app/ClockAlm
 * 1.������ṹ�������˽��(����¼)ԭ�п�ʼСʱ�Ϳ�ʼ����2��
 *   �ֶ�
 * 
 * *****************  Version 1  *****************
 * User: Jqiao        Date: 01-05-08   Time: 15:38
 * Created in $/RWP_SRC_Emu/Pda_app/ClockAlm
 * ���ӱ���ά��ģ���ⲿ�ӿ�����
 * 
\**************************************************************************/

#ifndef _ALM_CLKALM_H
#define _ALM_CLKALM_H


// �궨��
#define MAX_LEN_ALMMSG	32	// ��������б�����Ϣ��󳤶�


// �Զ���ʱ��ṹ
typedef struct tagDate
{
	BYTE   bMonth;		// ��
	BYTE   bDay;		// ��
	BYTE   bHour;		// ʱ
	BYTE   bMinute;		// ��
	WORD   wYear;		// ��
	BYTE   bSecond;		// ��
	WORD   wUnused;
}DATE;
 
typedef struct tagAppParam
{
	WPARAM wParam;
	LPARAM lParam;
}APPPARAM, * PAPPPARAM;


// ������ṹ
typedef struct tagAlmNode
{
	BYTE       bFrom;				// ������¼������Ե�ģ��
	BOOL       bIsPIMApp;			// ������Ƿ�����PIM����

	APPPARAM   AppParam;			// ��Ҫ�����App����
	
	WORD	   wCaldYear;			// �ճ�ʱ��(��)
	BYTE       bCaldMonth;			// �ճ�ʱ��(��)
	BYTE       bCaldDay;			// �ճ�ʱ��(��)

	BYTE       bBegHour;			// ��¼ԭʼ��ʼʱ��(ʱ)
	BYTE       bBegMinute;			// ��¼ԭʼ��ʼʱ��(��)

	BYTE       bMonth;				// ����ʱ��(��)
	BYTE       bDay;				// ����ʱ��(��)
	BYTE       bHour;				// ����ʱ��(ʱ)

	BYTE       bMinute;				// ����ʱ��(��)
	BYTE       bRepCyc;				// �����ظ�����
	BYTE       bRepInterval;		// �����ظ����
	BYTE       bRepMethod;			// �����ظ�����(�����ظ�����Ϊ"ÿ��"��Ч)

	BYTE       bRepWeekday;			// �����ظ�����(�����ظ�����Ϊ"ÿ��"��Ч)
	BYTE	   bMask;				// �����ظ����������Ƿ���Ч	

	DWORD      dwDBIndex;			// ������¼��PIM���ݿ��е�����
	struct	   tagAlmNode * pNext;	// ��һ��� 

	DWORD      dwUnunsed;
	WORD       wYear;				// ����ʱ��(��)

	DATE	   RepEndDate;			// �����ظ���������
  
	char	   Msg[50];				// ��������

}ALM_NODE;


// ����ͷ���ṹ
typedef struct tagHead
{
	UINT	   nNodeNum;	// ����������(������ͷ���)
	ALM_NODE * pNext;		// ָ�������һ�����
}HEAD, * PHEAD;



// �Զ�����Ϣ
#define  WM_SHOWMSG	    (WM_USER + 200)		// ��ʾ������Ϣ
#define  WM_APPENDMSG   (WM_USER + 201)		// ׷�ӱ�����Ϣ
#define  WM_CHGSYSTIME  (WM_USER + 203)		// ϵͳʱ������������Ϣ


// ���ӱ����ⲿ�ӿں���
extern DWORD ClockAlm_AppControl();
extern BOOL  ALM_InitAlarm(void);
extern BOOL  ALM_RegisterAlarm(ALM_NODE *);
extern BOOL  ALM_UpdateAlarm(BYTE, DWORD, ALM_NODE *);
extern BOOL  ALM_DeleteAlarm(BYTE, DWORD);
extern void  ALM_FreeList(HEAD *);


#endif _ALM_CLKALM_H	// _ALM_CLKALM_H