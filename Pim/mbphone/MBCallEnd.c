/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 打接电话部分
 *
 * Purpose  : 通话结束的提示界面
 *            
\**************************************************************************/
#include    "winpda.h"
#include    "str_mb_plx.h"
#include    "hp_pdaex.h"
#include    "hpimage.h"
#include    "MBPublic.h"
#include    "stdio.h"

#define     IDC_TIMER               3
#define     TIMER_DURATION          2000//持续显示时间
#define     TITLECAPTIONONE         MBGetString(STR_MBPHONE_CALLONEEND)//通话结束
#define     CALLTIME                MBGetString(STR_MBPHONE_CALLINGTIME)//"通话时间:"

static const char * pClassNameOne   = "MBCallOneEndWndClass";     //窗口类名
static const char * PicCallEnd      = "";
static const char * pTime           = "%s%s";

static const int    iSXSpace        = 10;//单方通话的左边空白大小
static const int    iSYSpace        = 5;//单方通话的上边空白大小

static HINSTANCE    hInstanceOne;
static char         cName[PHONENUMMAXLEN + 1];
static char         cPhoneNumber[PHONENUMMAXLEN + 1];
static char         cLastTimer[30];
static char         cCostInfo[PHONENUMMAXLEN + 1];

static  LRESULT AppWndProcOne( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
/********************************************************************
* Function   MBCallEndWindow  
* Purpose   显示通话结束界面
* Params    pName:姓名,pPhoneCode:号码,pDuration:时间,pCost:费用
* Return    
* Remarks   
**********************************************************************/
BOOL    MBCallEndWindow(const char * pName,const char * pPhoneCode,const char * pDuration,const char * pCost)
{
    HWND        hWnd;
    WNDCLASS    wc;
    int         iSour;

    if (GetUsedItemNum() != 0)//if ((IsWindow_MBRing()) || (IsWindow_Dialup()))
        return FALSE;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AppWndProcOne;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassNameOne;

    if (!RegisterClass(&wc))
        return FALSE;

    memset(cName        ,0x00,sizeof(cName       ));
    memset(cPhoneNumber ,0x00,sizeof(cPhoneNumber));
    memset(cLastTimer   ,0x00,sizeof(cLastTimer  ));
    memset(cCostInfo    ,0x00,sizeof(cCostInfo   ));

    if (pName != NULL)
    {
        iSour = strlen(pName);
        strncpy(cName,pName,sizeof(cName) > iSour ? iSour + 1 : sizeof(cName) - 1);
    }
    if (pPhoneCode != NULL)
    {
        iSour = strlen(pPhoneCode);
        strncpy(cPhoneNumber,pPhoneCode,sizeof(cPhoneNumber) > iSour ? iSour + 1 : sizeof(cPhoneNumber) - 1);
    }
    if (pDuration != NULL)
    {
        sprintf(cLastTimer,pTime,CALLTIME,pDuration);
        //iSour = strlen(pDuration);
        //strncat(cLastTimer,pDuration,sizeof(cLastTimer) > iSour ? iSour + 1 : sizeof(cLastTimer) - 1);
    }
    if (pCost != NULL)
    {
        iSour = strlen(pCost);
        strncpy(cCostInfo,pCost,sizeof(cCostInfo) > iSour ? iSour + 1 : sizeof(cCostInfo) - 1);
    }

    hWnd = CreateWindow(pClassNameOne,TITLECAPTIONONE, 
        WS_CAPTION|WS_BORDER |PWS_STATICBAR, PLX_WIN_POSITION , 
        NULL, NULL, NULL, NULL);

    if (NULL == hWnd)
    {
        UnregisterClass(pClassNameOne,NULL);
        return FALSE;
    }

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd); 

    return (TRUE);
}
static LRESULT AppWndProcOne ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static  HGIFANIMATE hgifanimate;
    static  RECT        rName;
    static  RECT        rPhone;
    static  RECT        rLastTimer;
    static  RECT        rCostInfo;
            int         iSW,iFontHeight,iy[4];
            LRESULT     lResult;
            HDC         hdc;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        SetTimer(hWnd,IDC_TIMER,TIMER_DURATION,0);

        iSW         = GetScreenUsableWH1(SM_CXSCREEN);
        iFontHeight = GetCharactorHeight(hWnd);
        CountControlsYaxis(iy,iFontHeight * 2,sizeof(iy));

        SetRect(&rName      ,iSXSpace,iy[0],iSW - iSXSpace * 2,iy[0] + iFontHeight * 2);
        SetRect(&rPhone     ,iSXSpace,iy[1],iSW - iSXSpace * 2,iy[1] + iFontHeight * 2);
        SetRect(&rLastTimer ,iSXSpace,iy[2],iSW - iSXSpace * 2,iy[2] + iFontHeight * 2);
        SetRect(&rCostInfo  ,iSXSpace,iy[3],iSW - iSXSpace * 2,iy[3] + iFontHeight * 2);

        hgifanimate = StartAnimatedGIFFromFile(hWnd,PicCallEnd,
            rName.right + iSXSpace,iSYSpace + iFontHeight / 2,DM_NONE);
        break;
    case WM_TIMER:
        switch (wParam)
        {
        case IDC_TIMER:
            DestroyWindow(hWnd);
            break;
        }
        break;
    case WM_DESTROY ://通知程序管理器,此程序退出;
        EndAnimatedGIF(hgifanimate);
        KillTimer(hWnd,IDC_TIMER);
        UnregisterClass(pClassNameOne,NULL);
        DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstanceOne );
        break;

    case WM_PAINT :
        hdc = BeginPaint( hWnd, NULL);
        PaintAnimatedGIF(hdc,hgifanimate);
        StrAutoNewLine(hdc,cName        ,NULL,&rName     ,DT_LEFT|DT_VCENTER);
        StrAutoNewLine(hdc,cLastTimer   ,NULL,&rLastTimer,DT_LEFT|DT_VCENTER);
        StrAutoNewLine(hdc,cCostInfo    ,NULL,&rCostInfo ,DT_LEFT|DT_VCENTER);
        StrAutoNewLine(hdc,cPhoneNumber ,NULL,&rPhone    ,DT_LEFT|DT_VCENTER);
        EndPaint(hWnd, NULL);
        break;

    case WM_TODESKTOP:
        DestroyWindow(hWnd);
        return FALSE;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_F2:
        case VK_F10:
            DestroyWindow(hWnd);
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
