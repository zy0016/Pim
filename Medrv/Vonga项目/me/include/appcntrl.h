/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   : app controls model for mobile product
 *
 * Purpose : implement MenuList control
 *  
 * Author  : 
 *
 *-------------------------------------------------------------------------
 *
 * $Archive::                                                       $
 * $Workfile::                                                      $
 * $Revision::    $     $Date::                                     $
 * 
\**************************************************************************/


#ifndef _APPCNTRL_H
#define _APPCNTRL_H

// focus notification
#define ML_NOTIFY       1


#define ML_SETBGPIC         WM_USER+100 // set background picture
#define ML_SETITEMSIZE      WM_USER+101 // set item width and height
#define ML_SETITEMSTARTPOS  WM_USER+102 // set position of the first item
#define ML_SETICONOFFSET    WM_USER+103 // set icon offset from the left

#define ML_SETITEMICON      WM_USER+110 // set item icon
#define ML_SETITEMDATA      WM_USER+111 // set item data

#define ML_GETITEMDATA      WM_USER+120 // get item data

#define ML_ADDITEM          WM_USER+130 // add item

// item align
#define MLI_LEFT        0x0000
#define MLI_CENTER      0x0001
#define MLI_RIGHT       0x0002
#define MLI_HMASK       0x0003

// item notification flag
#define MLI_NOTIFY      0x0004

// item focus flag
#define MLI_SELECTED    0x0100

typedef void    *HMENULIST;

typedef struct tagMENULISTITEMTEMPLATE
{
    WORD    wID;                // item ID
    DWORD   dwFlags;            // item flags
    int     nPicAreaWidth;      // icon area width
    PCSTR   lpszIconName;       // icon name
    PCSTR   lpszItemName;       // item string
} MENULISTITEMTEMPLATE, *PMENULISTITEMTEMPLATE;

typedef struct tagMENULISTTEMPLATE
{
    //DWORD                   version;
    //BOOL                    bVScroll;
    DWORD                   cmlit;          // Count of Menulist items
    PCSTR                   lpszBgPic;      // background bitmap name
    int                     nItemWidth;     // item width
    int                     nItemHeight;    // item height
    int                     nTopLeftX;      // topleft x of the top item
    int                     nTopLeftY;      // topleft y of the top item
    int                     nLeftGap;       // left gap of each icon
    //PMENULISTITEMTEMPLATE   pMenuListItems;
    PMENULISTITEMTEMPLATE    pmlItems;     // Menulist items: mlItems[cdit]
} MENULISTTEMPLATE;

HMENULIST LoadMenuListIndirect(const MENULISTTEMPLATE* pMenuListTemplate);
BOOL DestroyMenuList(HMENULIST hMenuList);

#endif  // _APPCNTRL_H
