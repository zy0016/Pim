/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 通话记录部分
 *
 * Purpose  : 通话计时
 *            
\**************************************************************************/
#include    "window.h"
#include    "../mbphone/str_mb_plx.h"
#include    "setting.h"
#include    "RcCommon.h"
#include    "hp_pdaex.h"
#include    "../mbphone/MBPublic.h"

#define     TIMEMAXLEN              50
#define     IDC_BUTTON_QUIT         3//退出按钮
#define     IDC_BUTTON_SET          4//确定按钮

#define     TITLECAPTION            MBGetString(STR_MBRECORD_DURATION)  //通话计时
#define     BUTTONOK                (LPARAM)MBGetString(STR_MBRECORD_CLEAR)//确定
#define     BUTTONQUIT              (LPARAM)MBGetString(STR_WINDOW_BACK)    //退出

#define     IDS_LASTCALLTIME        MBGetString(STR_MBRECORD_LASTTIME)//"上次"
#define     IDS_OUTSUM              MBGetString(STR_MBRECORD_OUTCOUNT)//"呼出"
#define     IDS_INSUM               MBGetString(STR_MBRECORD_INCOUNT)//"接入"
#define     IDS_TOTAL               MBGetString(STR_MBRECORD_TOTAL)//"总计"

static HINSTANCE    hInstance;
static const char * pClassName  = "MBRecordCostWndClass";//窗口类名
static const char * pTime       = "%s:%02d:%02d:%02d";
static const int    iRectX      = 10;
static const int    iNumber60   = 60;

static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
//*********************************************************
BOOL    CallMBRecordCostWindow(HWND hwndCall)
{
    HWND        hWnd;
    WNDCLASS    wc;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassName;

    if (!RegisterClass(&wc))
        return FALSE;

    hWnd = CreateWindow(pClassName,TITLECAPTION,WS_CAPTION|WS_BORDER |PWS_STATICBAR,  
        PLX_WIN_POSITION , hwndCall, NULL, NULL, NULL);

    if (NULL == hWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd); 
    return (TRUE);
}
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    MBRecordCostTime MRCostTime;
    RECT    rTitleTime,rLastTime,rOutTime,rInTime;
    int     iSW,iy[4],iFontHeight,TimeAllHour = 0,TimeAllMin = 0,TimeAllSec = 0;
    char    cTotalTime[TIMEMAXLEN] = "",cLastTime[TIMEMAXLEN] = "",cOutTime[TIMEMAXLEN] = "",cInTime[TIMEMAXLEN] = "";
    LRESULT lResult;
    HDC     hdc;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT);
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK);
        break;

    case WM_SETFOCUS:
        SetFocus(hWnd);
        break;

    case WM_CLOSE://点击右上角的退出按钮时为保存退出;
        SendMessage(hWnd,WM_COMMAND,(WPARAM)IDC_BUTTON_QUIT,0);
        break;

    case WM_DESTROY ://通知程序管理器,此程序退出;
        UnregisterClass(pClassName,NULL);
        DlmNotify((WPARAM)PES_STCQUIT,(LPARAM)hInstance);
        break;

    case WM_PAINT :
        hdc = BeginPaint( hWnd, NULL);

        ReadRecord_Cost(&MRCostTime); 

        TimeAllSec = MRCostTime.nInSecond + MRCostTime.nOutSecond;
        if(TimeAllSec > iNumber60)
        {
            TimeAllSec -= iNumber60;
            TimeAllMin++;
        }
        TimeAllMin = TimeAllMin + MRCostTime.nInMin + MRCostTime.nOutMin;
        if(TimeAllMin > iNumber60)
        {
            TimeAllMin -= iNumber60;
            TimeAllHour++;
        }
        TimeAllHour = TimeAllHour + MRCostTime.nOutHour + MRCostTime.nInHour;

        sprintf(cLastTime, pTime,IDS_LASTCALLTIME, 
            MRCostTime.nLastHour, MRCostTime.nLastMin, MRCostTime.nLastSecond);
        sprintf(cOutTime, pTime, IDS_OUTSUM,
            MRCostTime.nOutHour, MRCostTime.nOutMin, MRCostTime.nOutSecond);
        sprintf(cInTime, pTime, IDS_INSUM,
            MRCostTime.nInHour, MRCostTime.nInMin, MRCostTime.nInSecond);
        sprintf(cTotalTime, pTime,IDS_TOTAL,//"记时总计"
            TimeAllHour, TimeAllMin, TimeAllSec);

        iSW         = GetScreenUsableWH1(SM_CXSCREEN);
        iFontHeight = GetCharactorHeight(hWnd);
        CountControlsYaxis(iy,iFontHeight,sizeof(iy));

        SetRect(&rTitleTime ,iRectX,iy[0],iSW - iRectX * 2,iy[0] + iFontHeight);//计时总计
        SetRect(&rLastTime  ,iRectX,iy[1],iSW - iRectX * 2,iy[1] + iFontHeight);//上次通话
        SetRect(&rOutTime   ,iRectX,iy[2],iSW - iRectX * 2,iy[2] + iFontHeight);//累计呼出
        SetRect(&rInTime    ,iRectX,iy[3],iSW - iRectX * 2,iy[3] + iFontHeight);//累计接入

        DrawText(hdc,cLastTime  ,-1,&rLastTime  ,DT_LEFT|DT_VCENTER);
        DrawText(hdc,cOutTime   ,-1,&rOutTime   ,DT_LEFT|DT_VCENTER);
        DrawText(hdc,cInTime    ,-1,&rInTime    ,DT_LEFT|DT_VCENTER);
        DrawText(hdc,cTotalTime ,-1,&rTitleTime ,DT_LEFT|DT_VCENTER);

        EndPaint(hWnd, NULL);
        break;

    case WM_TODESKTOP:
        SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
        return TRUE;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_F10:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
            break;
        case VK_F5:
        case VK_RETURN:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SET,0);
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        case IDC_BUTTON_SET://清零
            memset(&MRCostTime,0x00,sizeof(MBRecordCostTime));
            SaveRecord_Cost(&MRCostTime);
            InvalidateRect(hWnd, NULL, NULL);
            break;
        case IDC_BUTTON_QUIT://退出
            DestroyWindow(hWnd);  
            break;
        }
        break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
