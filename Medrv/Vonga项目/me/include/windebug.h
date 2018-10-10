/**************************************************************************\
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :	Window
 *
 * Purpose :    Gets the current object info, used in emulator.
 *  
 * Author  :    WeiZhong
 *
 *-------------------------------------------------------------------------
 *
 * $Archive:: /hopenpda/include/windebug.h                          $
 * $Revision:: 3  $     $Date:: 00-01-10 9:20a                      $
 *
 * $History:: windebug.h                                            $
 * 
 * *****************  Version 3  *****************
 * User: Zwei         Date: 00-01-10   Time: 9:20a
 * Updated in $/hopenpda/include
 * 
 * *****************  Version 1  *****************
 * User: Zwei         Date: 99-10-10   Time: 8:29a
 * Created in $/hopenpda/include
 * 
\**************************************************************************/

#ifndef __WINDEBUG_H
#define __WINDEBUG_H

#ifndef _MSC_VER
#error This header file can only be used in VC compiler!
#endif

/* Assume C declarations for C++ */
#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagWINDEBUGINFO
{
    int nGDIObjNum;     // Store the GDI object number
    int nGDIObjMax;     // Store the maximum GDI object number
    int nPenNum;        // Store the pen object number
    int nBrushNum;      // Store the brush object number
    int nBitmapNum;     // Store the bitmap object number
    int nFontNum;       // Store the font object number
    int nPaletteNum;    // Store the palette object number
    int nDCNum;         // Store the dc object number

    int nUserObjNum;    // Store the user object number
    int nUserObjMax;    // Store the maximum user object number
    int nWindowNum;     // Store the window object number
    int nCursorNum;     // Store the cursor object number
    int nIconNum;       // Store the icon object number
    int nMenuNum;       // Store the menu object number
} WINDEBUGINFO, *PWINDEBUGINFO;

_declspec(dllexport) void GetWinDebugInfo(PWINDEBUGINFO pWinDebugInfo);

#ifdef __cplusplus
}                       /* End of extern "C" */
#endif

#endif  // __WINDEBUG_H
