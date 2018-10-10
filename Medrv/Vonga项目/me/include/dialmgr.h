/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 2000-2001 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :
 *
 * Filename:	dialmgr.h
 *
 * Purpose :	
 *
 * Author  :	
 *
 * Date    :	
 *-------------------------------------------------------------------------
 *
 * $Archive::                                                       $
 * $Revision::    $     $Date::                                     $
 *
 * $History::                                                       $
 *
\**************************************************************************/

#ifndef __DIALMGR_H
#define __DIALMGR_H

#include "dialer.h"

int DIALMGR_Initialize(void);
int DIALMGR_SelConnect(HWND hWnd, UINT msg, int nSelectType, void *pDialInfo, 
                       BOOL bWait);
int DIALMGR_HangUp(HWND hWnd);

#define DIALMGR_GetLineState    DIALER_GetLineState
#define DIALMGR_GetError        Dialer_GetError

#endif  // __DIALMGR_H

