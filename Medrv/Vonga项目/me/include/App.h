
/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :
 *
 * Filename:	app.h
 *
 * Purpose :
 *
 * Author  :	Jingjing Xu
 *
 *-------------------------------------------------------------------------
 *
 * $Archive::                                                       $
 * $Revision::    $     $Date::                                     $
 *
 * $History::                                                       $
 *
\**************************************************************************/

#ifndef __APP_H
#define __APP_H

#ifndef _WINDEF_
#include "hpdef.h"
#endif


typedef DWORD (*APPCONTROL)(int nCode, void* pInstance,
										WPARAM wParam, LPARAM lParam );

#define APP_NAMELEN		16

// Define struct for a application 
typedef struct tagAPPLICATION
{
    char achName[APP_NAMELEN];	// Name of the application
	char strIconName[APP_NAMELEN];	// Name of the application Icon file
    WORD attrib;				// Attribute of the application
    WORD dwStyle;				// Instance data size of the application
    APPCONTROL AppControl;		// Application control entry
} APPLICATION, *PAPPLICATION;

extern const APPLICATION* AppTable[];

typedef struct tagAPPGROUP
{
    char achName[APP_NAMELEN];	// Name of the application
	char strIconName[APP_NAMELEN];	// Name of the application Icon file
	APPLICATION **AppTable;  
} APPGROUP, *PAPPGROUP;

extern const APPGROUP* GrpTable[];


#endif
