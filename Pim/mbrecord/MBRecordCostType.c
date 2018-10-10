/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 计费方式
 *            
\**************************************************************************/
#include    "window.h"
#include    "../mbphone/str_mb_plx.h"
#include    "setting.h"
#include    "RcCommon.h"
#include    "hp_pdaex.h"
#include    "../mbphone/MBPublic.h"

#define     IDC_BUTTON_QUIT         3//退出按钮
#define     IDC_BUTTON_SET          4//确定按钮
#define     IDC_COIN                5//货币
#define     IDC_UNIT                6//单位
#define     IDC_ME_GETPRICEUNIT     WM_USER + 100//获取计价信息
#define     IDC_ME_SETPRICEUNIT     WM_USER + 120//设置计价信息

#define     TITLECAPTION            MBGetString(STR_MBRECORD_COSTTYPE)//计费方式
#define     BUTTONOK                (LPARAM)MBGetString(STR_WINDOW_OK)    //确定
#define     BUTTONQUIT              (LPARAM)MBGetString(STR_WINDOW_BACK)  //退出
#define     ERROR1                  MBGetString(STR_SETUP_ERROR1)         //出错提示
#define     COSTTYPE_FAIL           MBGetString(STR_MBRECORD_FARE_SETFAIL)//设置失败

#define     COSTTYPE_COIN           MBGetString(STR_MBRECORD_MONEY)//货币
#define     COSTTYPE_UNIT           MBGetString(STR_MBRECORD_UNIT)//单位

static const char * pClassName   = "MBChargingTypeWndClass";//窗口类名
static const char * pButtonClass = "BUTTON";
static HINSTANCE    hInstance;

static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  BOOL    CreateControl(HWND hWnd,HWND * phCoin);//创建控件

BOOL    CallChargingTypeWindow(HWND hwndCall)
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
    static  HWND        hFocus = 0;
            LRESULT     lResult;
            ME_PUCINFO  me_pucinfo;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        if (!CreateControl(hWnd,&hFocus))
        {
            AppMessageBox(NULL,ERROR1, TITLECAPTION, WAITTIMEOUT);
            return -1;
        }
        if (-1 != ME_GetPricePerUnit(hWnd,IDC_ME_GETPRICEUNIT))//获取计价信息
            WaitWindowState(hWnd,TRUE);
        break;

    case WM_SETFOCUS:
        SetFocus(hFocus);
        break;

    case WM_ACTIVATE://去掉这个消息，单击右键可以退出，否则退出失败
        if (WA_INACTIVE != LOWORD(wParam))
            SetFocus(hFocus);//激活后设置焦点
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;

    case WM_CLOSE://点击右上角的退出按钮时为保存退出;
        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_BUTTON_QUIT, 0 );   
        break;

    case WM_DESTROY ://通知程序管理器,此程序退出;
        UnregisterClass(pClassName,NULL);
        DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
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
        case IDC_COIN://货币
            if(HIWORD(wParam) == BN_SETFOCUS)
            {
                CheckRadioButton(hWnd,IDC_COIN,IDC_UNIT,IDC_COIN);
                hFocus = GetDlgItem(hWnd,IDC_COIN);
            }
            break;
        case IDC_UNIT://单位
            if(HIWORD(wParam) == BN_SETFOCUS)
            {
                CheckRadioButton(hWnd,IDC_COIN,IDC_UNIT,IDC_UNIT);
                hFocus = GetDlgItem(hWnd,IDC_UNIT);
            }
            break;
        case IDC_BUTTON_SET://设置
            if (IsDlgButtonChecked(hWnd,IDC_COIN) == BST_CHECKED)//货币
            {
                CallSetCost(NULL);//话费设定
                DestroyWindow(hWnd);
            }
            else if (IsDlgButtonChecked(hWnd,IDC_UNIT) == BST_CHECKED)//单位
            {
                char cPin2[PIN2MAXLEN + 1] = "";
                memset(&me_pucinfo,0x00,sizeof(ME_PUCINFO));
                GetValidPin2(cPin2);
                if (-1 == ME_SetPricePerUnit(hWnd,IDC_ME_SETPRICEUNIT,&me_pucinfo,cPin2))
                {
#ifndef _EMULATE_
                    AppMessageBox(NULL,COSTTYPE_FAIL, TITLECAPTION, WAITTIMEOUT);
#else
                    PostMessage(hWnd,IDC_ME_SETPRICEUNIT,ME_RS_SUCCESS,0);
#endif
                }
            }
            break;
        case IDC_BUTTON_QUIT://退出
            DestroyWindow(hFocus);
            DestroyWindow( hWnd );
            break;
        }
        break;

    case IDC_ME_GETPRICEUNIT://获取计价信息
        WaitWindowState(hWnd,FALSE);
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            ME_GetResult(&me_pucinfo,sizeof(ME_PUCINFO));
            TrimString(me_pucinfo.currency);
            if (strlen(me_pucinfo.currency) != 0)
                SetFocus(GetDlgItem(hWnd,IDC_COIN));
            else
                SetFocus(GetDlgItem(hWnd,IDC_UNIT));

            break;
        default:
            break;
        }
        break;

    case IDC_ME_SETPRICEUNIT://设置计价信息
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
            break;
        default:
            AppMessageBox(NULL,COSTTYPE_FAIL, TITLECAPTION, WAITTIMEOUT);
            break;
        }
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
static  BOOL    CreateControl(HWND hWnd,HWND * phCoin)
{
    int icx,icontrolw,icontrolh,iy[2],icontrolw1,icontrolw2;
    HWND hUnit;

    SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, // 右键退出
                (WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT );

    SendMessage(hWnd , PWM_CREATECAPTIONBUTTON, //左键确定
                MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK );

    SendMessage( hWnd, PWM_SETSCROLLSTATE,
                SCROLLLEFT|SCROLLRIGHT|SCROLLDOWN|SCROLLUP, MASKALL );

    icontrolw1  = GetRadioButtonControlWidth(COSTTYPE_COIN);
    icontrolw2  = GetRadioButtonControlWidth(COSTTYPE_UNIT);

    icontrolw   = icontrolw1 >= icontrolw2 ? icontrolw1 : icontrolw2;
    icx         = (GetScreenUsableWH1(SM_CXSCREEN) - icontrolw) / 2;

    icontrolh   = GetButtonControlHeight();

    CountControlsYaxis(iy,icontrolh,sizeof(iy));

    * phCoin = CreateWindow(pButtonClass,COSTTYPE_COIN,//货币
        BS_AUTORADIOBUTTON|WS_CHILD|WS_VISIBLE|WS_TABSTOP,
        icx,iy[0],icontrolw,icontrolh,hWnd,(HMENU)IDC_COIN,hInstance,NULL);

    if (* phCoin == NULL)
        return FALSE;

    hUnit = CreateWindow(pButtonClass,COSTTYPE_UNIT,//单位
        BS_AUTORADIOBUTTON|WS_CHILD|WS_VISIBLE|WS_TABSTOP,
        icx,iy[1],icontrolw,icontrolh,hWnd,(HMENU)IDC_UNIT,hInstance,NULL);

    if (hUnit == NULL)
        return FALSE;

    return TRUE;
}
