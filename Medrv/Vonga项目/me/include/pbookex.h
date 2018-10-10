/**************************************************************************\
*
*                      Hopen Software System
*
* Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
*                       All Rights Reserved
*
* Model   :		pbookex.h
*
* Purpose :		规定通讯簿的外部调用接口
*  
* Author  :		郭新松
*
*-------------------------------------------------------------------------
*
* $Archive::                                                       $
* $Workfile::                                                      $
* $Revision::    $     $Date::                                     $
* 
\**************************************************************************/

#define WM_PNOSELRETURN		(WM_USER+13)

BOOL SavePNoToPBook(HWND hParent, char* pPhoneNo);		//外部接口功能为保存电话号码至通讯簿
BOOL GetPNoFromPBook(HWND hParent);		//外部接口功能为显示通讯簿记录列表界面，并在选中后返回记录内容
