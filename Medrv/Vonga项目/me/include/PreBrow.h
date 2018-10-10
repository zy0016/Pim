/**************************************************************************\
*
*                      Hopen Software System
*
* Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
*                       All Rights Reserved
*
* Model   :			PreBrow.h
*
* Purpose :			Ԥ����ý���ļ�����ͷ�ļ�
*  
* Author  :			������
*
*-------------------------------------------------------------------------
*
* $Archive::                                                       $
* $Workfile::                                                      $
* $Revision::    $     $Date::                                     $
* 
\**************************************************************************/
#ifndef _PREBROW_H_
#define _PREBROW_H_

#define PIC_PREBROW			0x0001
#define RING_PREBROW		0x0002
#define TEXT_PREBROW		0x0004
#define QTEXT_PREBROW		0x0008

#define RTN_IMAGE		0
#define RTN_SOUND		1
#define RTN_TEXT		2
#define RTN_QTEXT		3

//Ԥ����ý���ļ��ӿں�����hWndΪ���õĸ����ھ����returnmsgΪ������Ϣ��
HWND	PreBrowseInterface(HWND hWnd,UINT returnmsg,DWORD dwStyle);
BOOL	EnterPicturesBrow(HWND hWnd,UINT returnmsg);
BOOL	EnterRingsBrow(HWND hWnd,UINT returnmsg);
BOOL	EnterTextsBrow(HWND hWnd,UINT returnmsg);
extern  BOOL SMSQuickTextCreate(HWND hWndParent, UINT wCallBackMsg);

#endif //_PREBROW_H_

