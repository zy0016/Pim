/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 通话计费
 *            
\**************************************************************************/
#include    "window.h"
#include    "string.h"
#include    "stdlib.h"
#include    "stdio.h" 
#include    "winpda.h"
#include    "str_public.h"
#include    "hp_pdaex.h"
#include    "../mbphone/str_mb_plx.h"
#include    "RcCommon.h"

#define DEBUG_MBRECORFARE

#define     IDC_BUTTON_QUIT         3//退出按钮
#define     IDC_BUTTON_SET          4//确定按钮
#define     IDC_BUTTON_TOTAL        6//累计费用清零按钮

#define     IDC_ME_RESETACM         WM_USER + 100
#define     IDC_ME_GETACM           WM_USER + 110//查询累计通话费用
#define     IDC_ME_GETLAST          WM_USER + 120//查询上次通话费用
#define     IDC_ME_GETPRICEUNIT     WM_USER + 130//获取计价信息

#define     TITLECAPTION            MBGetString(STR_MBRECORD_COST)//通话计费
#define     BUTTONOK                (LPARAM)MBGetString(STR_WINDOW_OK)    //确定
#define     BUTTONQUIT              (LPARAM)MBGetString(STR_WINDOW_EXIT)  //退出
#define     ERROR1                  MBGetString(STR_MBRECORD_ERROR1)      //出错提示
#define     BUTTONTXT_TOTAL         MBGetString(STR_MBRECORD_BUTTONTXT_TOTAL)//累计费用清零
#define     CLEWTXT_LAST            MBGetString(STR_MBRECORD_CLEWTXT_LAST)//最后通话
#define     CLEWTXT_TOTAL           MBGetString(STR_MBRECORD_CLEWTXT_TOTAL)//累计费用
#define     SETSUCC                 MBGetString(STR_MBRECORD_FARE_SETSUCC)//"设置成功"
#define     SETFAIL                 MBGetString(STR_MBRECORD_FARE_SETFAIL)//"设置失败"
#define     UNIT                    MBGetString(STR_MBRECORD_FACE_UNIT)//"单位"

static HINSTANCE    hInstance;
static const char * pClassName   = "MBFareWndClass";//窗口类名
static const char * pButtonClass = "BUTTON";
static const char * pClewFormat  = "%7ld %s";
static const char * pUnitFormat  = "%7lf %s";
static const int    iRectX       = 5;

static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  BOOL    CreateControl(HWND hWnd);//创建控件

BOOL    CallMBRecordFare(HWND hwndCall)
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

    hWnd = CreateWindow(pClassName,TITLECAPTION, WS_CAPTION|WS_BORDER |PWS_STATICBAR,
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
    static  ME_PUCINFO      me_pucinfo;
    static  unsigned long   lTotal,lLast;
            char            cPin2[PIN2MAXLEN + 1] = "",cTotal[50] = "",cLast[50] = "";
            LRESULT         lResult;
            HDC             hdc;
            int             iy[2],ish,iFontHeight;
            RECT            rTotal,rLast;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        if (!CreateControl(hWnd))
        {
            AppMessageBox(NULL,ERROR1, TITLECAPTION, WAITTIMEOUT);
            return -1;
        }
        lTotal = 0;
        lLast  = 0;
        memset(&me_pucinfo,0x00,sizeof(ME_PUCINFO));
        ME_GetCCM(hWnd,IDC_ME_GETLAST);//查询上次通话费用
        break;

    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd,IDC_BUTTON_TOTAL));
        break;

    case WM_ACTIVATE://去掉这个消息，单击右键可以退出，否则退出失败
        if (WA_INACTIVE != LOWORD(wParam))
            SetFocus(GetDlgItem(hWnd,IDC_BUTTON_TOTAL));//激活后设置焦点
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
        
    case WM_CLOSE://点击右上角的退出按钮时为保存退出;
        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_BUTTON_QUIT, 0 );   
        break;

    case WM_DESTROY ://通知程序管理器,此程序退出;
        UnregisterClass(pClassName,NULL);
        DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
        break;

    case WM_PAINT :
        hdc         = BeginPaint( hWnd, NULL);
        ish         = GetScreenUsableWH1(SM_CYSCREEN);
        iFontHeight = GetCharactorHeight(hWnd);
        CountControslYaxisExt(iy,iFontHeight,sizeof(iy),-1,ish / 2);
        SetRect(&rLast ,iRectX,iy[0],iRectX + GetStringWidth(hdc,CLEWTXT_LAST) ,iy[0] + iFontHeight);
        SetRect(&rTotal,iRectX,iy[1],iRectX + GetStringWidth(hdc,CLEWTXT_TOTAL),iy[1] + iFontHeight);

        DrawText(hdc,CLEWTXT_LAST,strlen(CLEWTXT_LAST),&rLast,DT_LEFT);
        DrawText(hdc,CLEWTXT_TOTAL,strlen(CLEWTXT_TOTAL),&rTotal,DT_LEFT);

        if ((strlen(me_pucinfo.currency) == 0) && (strlen(me_pucinfo.ppu) == 0))//按照单位显示
        {
            sprintf(cLast ,pClewFormat,lLast ,UNIT);
            sprintf(cTotal,pClewFormat,lTotal,UNIT);
        }
        else
        {
            sprintf(cLast ,pUnitFormat,lLast * MB_atof(me_pucinfo.ppu) ,me_pucinfo.currency);
            sprintf(cTotal,pUnitFormat,lTotal * MB_atof(me_pucinfo.ppu),me_pucinfo.currency);
        }
        TextOut(hdc,rLast.right  + iRectX,iy[0],cLast ,strlen(cLast ));
        TextOut(hdc,rTotal.right + iRectX,iy[1],cTotal,strlen(cTotal));
        EndPaint(hWnd, NULL);
        break;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_F10:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
            break;
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
        case IDC_BUTTON_TOTAL://累计费用清零按钮
            switch(HIWORD(wParam))
            {
            case BN_CLICKED:
                if (CallValidPin2(hWnd))//首先检测PIN2码
                {
                    GetValidPin2(cPin2);
                    if (-1 == ME_ResetACM(hWnd,IDC_ME_RESETACM,cPin2))
                        AppMessageBox(hWnd,SETFAIL,TITLECAPTION, WAITTIMEOUT);
                }
                break;
            }
            break;

        case IDC_BUTTON_SET://设置
            SendKeyDown(GetFocus(), VK_F5, 0);
            break;
        case IDC_BUTTON_QUIT://退出
            DestroyWindow(hWnd);
            break;
        }
        break;

    case IDC_ME_RESETACM://累计费用清零
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            AppMessageBox(hWnd,SETSUCC,TITLECAPTION, WAITTIMEOUT);
            DestroyWindow(hWnd);
            break;
        default:
            AppMessageBox(hWnd,SETFAIL,TITLECAPTION, WAITTIMEOUT);
            SetFocus(GetDlgItem(hWnd,IDC_BUTTON_TOTAL));
            break;
        }
        break;

    case IDC_ME_GETACM://查询累计通话费用
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            ME_GetResult(&lTotal,sizeof(lTotal));
            break;
        default:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRECORFARE)
            printf("mb MBRecordFare 查询累计通话费用失败,wParam:%ld,lParam:%d\r\n",wParam,lParam);
#endif            
            break;
        }
        ME_GetPricePerUnit(hWnd,IDC_ME_GETPRICEUNIT);//获取计价信息
        break;

    case IDC_ME_GETLAST://查询上次通话费用
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            ME_GetResult(&lLast,sizeof(lLast));
            break;
        default:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRECORFARE)
            printf("mb MBRecordFare 查询上次通话费用失败,wParam:%ld,lParam:%d\r\n",wParam,lParam);
#endif
            break;
        }
        ME_GetACM(hWnd,IDC_ME_GETACM);//查询累计通话费用
        break;

    case IDC_ME_GETPRICEUNIT://获取计价信息
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            ME_GetResult(&me_pucinfo,sizeof(ME_PUCINFO));
            TrimString(me_pucinfo.currency);
            TrimString(me_pucinfo.ppu);
            break;
        default:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRECORFARE)
            printf("mb MBRecordFare 获取计价信息失败,wParam:%ld,lParam:%d\r\n",wParam,lParam);
#endif
            break;
        }
        InvalidateRect(hWnd,NULL,TRUE);
        break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
/********************************************************************
* Function   CreateControl  
* Purpose    创建控件 
* Params     hwnd：窗口过程句柄
* Return     
* Remarks     创建本届面所需的界面元素，包括"确定"，"取消"按钮，为窗口加图标，添加指示箭头. 
**********************************************************************/
static  BOOL    CreateControl(HWND hWnd)
{
    int isw,ish,icontrolx = 5;
    SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, // 右键退出
                (WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT );

    SendMessage(hWnd , PWM_CREATECAPTIONBUTTON, //左键确定
                MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK );

    SendMessage( hWnd, PWM_SETSCROLLSTATE,
                SCROLLLEFT|SCROLLRIGHT|SCROLLDOWN|SCROLLUP, MASKALL );

    GetScreenUsableWH2(&isw,&ish);

    return (NULL != CreateWindow(pButtonClass,BUTTONTXT_TOTAL,
        BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE|WS_BITMAP|WS_TABSTOP,
        icontrolx,ish / 2 + 10,isw - icontrolx * 2,GetButtonControlHeight(),
        hWnd,(HMENU)IDC_BUTTON_TOTAL,hInstance,NULL));
}
