/**************************************************************************\
*
*                      Hopen Software System
*
* Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
*                       All Rights Reserved
*
* Model   :	ͼƬ�����
*
* Purpose :	�ⲿ����ͼƬ������Ľӿ�
*  
* Author  : ������
*
*-------------------------------------------------------------------------
*
* $Archive::                                                       $
* $Workfile::                                                      $
* $Revision::    $     $Date::                                     $
* 
\**************************************************************************/

//����GIF��BMP
#define	PIC_TYPE_UNKNOW	-1			//δ֪����
#define	PIC_IS_BMP		0			//��ʾBMPͼ
#define	PIC_IS_GIF		1			//��ʾGIFͼ
#define	PIC_IS_JPG		2			//��ʾJPGͼ

//���÷�ʽ
#define CALL_TYPE_POWERON		1
#define CALL_TYPE_POWERDOWN		0

//�ⲿ����ͼƬ������Ľӿ�
BOOL	PicBrowInterface(HWND hWnd_Parent,char* PicFileName,int	nPicType);
#if defined SEF_SMARTPHONE
#else
BOOL	PowerMapInterface(HWND hWnd_Parent,char* PicFileName,int nPicType,int seconds,int CallType);
#endif