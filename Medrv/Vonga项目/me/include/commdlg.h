/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :    Window
 *
 * Purpose :    Header file for Common Dialog APIs.
 *  
 * Author  :    GuNing
 *
 *-------------------------------------------------------------------------
 *
 * $Archive::                                                       $
 * $Revision::    $     $Date::                                     $
 *
 * $History::                                                       $
 * 
\**************************************************************************/

#ifndef __COMMDLG_H
#define __COMMDLG_H

/* Assume C declarations for C++ */
#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagOFN { //ofn
    DWORD           lStructSize;
    HWND            hwndOwner;
    HINSTANCE       hInstance;
    LPCTSTR         lpstrFilter;
    LPSTR           lpstrCustomFilter;
    DWORD           nMaxCustFilter;
    DWORD           nFilterIndex;
    LPSTR           lpstrFile;
    DWORD           nMaxFile;
    LPSTR           lpstrFileTitle;
    DWORD           nMaxFileTitle;
    LPCTSTR         lpstrInitialDir;
    LPCTSTR         lpstrTitle;
    DWORD           Flags;
    WORD            nFileOffset;
    WORD            nFileExtension;
    LPCTSTR         lpstrDefExt;
    DWORD           lCustData;
//    LPOFNHOOKPROC   lpfnHook;
    void *          lpfnHook;
    LPCTSTR         lpTemplateName;
} OPENFILENAME, *LPOPENFILENAME;

BOOL    WINAPI  GetOpenFileName(LPOPENFILENAME lpofn);
BOOL    WINAPI  GetSaveFileName(LPOPENFILENAME lpofn);

#ifdef __cplusplus
}                       /* End of extern "C" */
#endif

#endif // __COMMDLG_H