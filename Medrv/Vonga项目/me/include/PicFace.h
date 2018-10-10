/**************************************************************************\
*
*                      Hopen Software System
*
* Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
*                       All Rights Reserved
*
* Model   :	图片浏览器
*
* Purpose :	外部调用图片浏览器的接口
*  
* Author  : 郭新松
*
*-------------------------------------------------------------------------
*
* $Archive::                                                       $
* $Workfile::                                                      $
* $Revision::    $     $Date::                                     $
* 
\**************************************************************************/

//区分GIF和BMP
#define	PIC_TYPE_UNKNOW	-1			//未知类型
#define	PIC_IS_BMP		0			//显示BMP图
#define	PIC_IS_GIF		1			//显示GIF图
#define	PIC_IS_JPG		2			//显示JPG图

//调用方式
#define CALL_TYPE_POWERON		1
#define CALL_TYPE_POWERDOWN		0

//外部调用图片浏览器的接口
BOOL	PicBrowInterface(HWND hWnd_Parent,char* PicFileName,int	nPicType);
#if defined SEF_SMARTPHONE
#else
BOOL	PowerMapInterface(HWND hWnd_Parent,char* PicFileName,int nPicType,int seconds,int CallType);
#endif