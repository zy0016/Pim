/**************************************************************************\
*
*                      Hopen Software System
*
* Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
*                       All Rights Reserved
*
* Model   :		pbookex.h
*
* Purpose :		�涨ͨѶ�����ⲿ���ýӿ�
*  
* Author  :		������
*
*-------------------------------------------------------------------------
*
* $Archive::                                                       $
* $Workfile::                                                      $
* $Revision::    $     $Date::                                     $
* 
\**************************************************************************/

#define WM_PNOSELRETURN		(WM_USER+13)

BOOL SavePNoToPBook(HWND hParent, char* pPhoneNo);		//�ⲿ�ӿڹ���Ϊ����绰������ͨѶ��
BOOL GetPNoFromPBook(HWND hParent);		//�ⲿ�ӿڹ���Ϊ��ʾͨѶ����¼�б���棬����ѡ�к󷵻ؼ�¼����
