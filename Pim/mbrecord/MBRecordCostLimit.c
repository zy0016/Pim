/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 话费限定
 *            
\**************************************************************************/
#include    "window.h"
#include    "../mbphone/str_mb_plx.h"
#include    "setting.h"
#include    "RcCommon.h"
#include    "hp_pdaex.h"
#include    "../mbphone/MBPublic.h"
#include    "me_wnd.h"

#define     IDC_BUTTON_RIGHT        3//退出按钮
#define     IDC_BUTTON_LEFT         4//确定按钮
#define     IDC_CLOSE               5//关
#define     IDC_OPEN                6//开
#define     IDC_EDIT_LIMIT          7//单位限额
#define     IDC_ME_GETACMMAX        WM_USER + 100//获取最大累计Call Meter值
#define     IDC_ME_SETACMMAX        WM_USER + 110//设置最大累计Call Meter值

#define     ACMMAXLEN               10

#define     TITLECAPTION            MBGetString(STR_MBRECORD_COSTTYPE)//计费方式
#define     BUTTON_LEFT             (LPARAM)MBGetString(STR_WINDOW_OK)    //确定
#define     BUTTON_RIGHT            (LPARAM)MBGetString(STR_WINDOW_BACK)  //退出
#define     ERROR1                  MBGetString(STR_SETUP_ERROR1)         //出错提示
#define     COSTTYPE_FAIL           MBGetString(STR_MBRECORD_FARE_SETFAIL)//设置失败

#define     RADIO_CLOSE             MBGetString(STR_MBRECORD_CLOSE)//关
#define     RADIO_OPEN              MBGetString(STR_MBRECORD_OPEN)//开
#define     TXT_UNITLIMIT           MBGetString(STR_MBRECORD_UNITLIMIT)//单位限额
#define     CLEW_INPUTUNITLIMIT     MBGetString(STR_MBRECORD_INPUTUNITLIMIT)//"请输入单位限额"
#define     CLEW_SETFAIL            MBGetString(STR_MBRECORD_FARE_SETFAIL)//设置失败

static const char * pClassName      = "MBChargingLimitndClass";//窗口类名
static const char * pString         = "%ld";
static const char * pButtonClass    = "BUTTON";
static const char * pEditClass      = "DINPUTEDIT";
static const int    iControlY       = 20;
static HINSTANCE    hInstance;

static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  BOOL    CreateControl(HWND hWnd,HWND * phClose);//创建控件

BOOL    CallChargingLimitWindow(HWND hwndCall)//话费限定
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
    static  HWND        hFocus = 0;
            HWND        hEditLimit;
            LRESULT     lResult;
            DWORD       dAcmMax = 0;
            RECT        rect;
            char        cAcmMax[ACMMAXLEN + 1] = "",cPin2[PIN2MAXLEN + 1] = "";
            HDC         hdc;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        if (!CreateControl(hWnd,&hFocus))
        {
            AppMessageBox(NULL,ERROR1, TITLECAPTION, WAITTIMEOUT);
            return -1;
        }
        ME_GetACMmax(hWnd,IDC_ME_GETACMMAX);//获取最大累计Call Meter值
        break;

    case WM_SETFOCUS:
        SetFocus(hFocus);
        break;

    case WM_SETRBTNTEXT:
        SetRightButtonText(hWnd,wParam,lParam);
        break;

    case WM_ACTIVATE://去掉这个消息，单击右键可以退出，否则退出失败
        if (WA_INACTIVE != LOWORD(wParam))
            SetFocus(hFocus);//激活后设置焦点
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;

    case WM_CLOSE://点击右上角的退出按钮时为保存退出;
        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_BUTTON_RIGHT, 0 );   
        break;

    case WM_DESTROY ://通知程序管理器,此程序退出;
        UnregisterClass(pClassName,NULL);
        DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
        break;

    case WM_PAINT :
        hdc = BeginPaint( hWnd, NULL);
        SetRect(&rect,0,iControlY + GetButtonControlHeight(),
            GetScreenUsableWH1(SM_CXSCREEN),
            iControlY + GetButtonControlHeight() + GetCharactorHeight(hWnd));

        DrawText(hdc,TXT_UNITLIMIT,strlen(TXT_UNITLIMIT),&rect,DT_CENTER);
        EndPaint(hWnd, NULL);
        break;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_F10:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_RIGHT,0);
            break;
        case VK_RETURN:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_LEFT,0);
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;
    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        case IDC_CLOSE://关
            switch (HIWORD(wParam))
            {
            case BN_SETFOCUS:
                CheckRadioButton(hWnd,IDC_CLOSE,IDC_OPEN,IDC_CLOSE);
                hFocus = GetDlgItem(hWnd,IDC_CLOSE);
                break;
            }
            break;

        case IDC_OPEN://开
            switch (HIWORD(wParam))
            {
            case BN_SETFOCUS:
                CheckRadioButton(hWnd,IDC_CLOSE,IDC_OPEN,IDC_OPEN);
                hFocus = GetDlgItem(hWnd,IDC_CLOSE);
                break;
            }
            break;

        case IDC_EDIT_LIMIT:
            switch(HIWORD(wParam))
            {
            case EN_SETFOCUS:
                hFocus = GetDlgItem(hWnd,IDC_EDIT_LIMIT);
                break;
            }
            break;

        case IDC_BUTTON_LEFT://设置
            GetValidPin2(cPin2);
            if (IsDlgButtonChecked(hWnd,IDC_CLOSE) == BST_CHECKED)//关
            {
                dAcmMax = 0;
            }
            else if (IsDlgButtonChecked(hWnd,IDC_OPEN) == BST_CHECKED)//开
            {
                hEditLimit = GetDlgItem(hWnd,IDC_EDIT_LIMIT);
                GetWindowText(hEditLimit,cAcmMax,GetWindowTextLength(hEditLimit) + 1);
                if (strlen(cAcmMax) == 0)
                {
                    AppMessageBox(hWnd,CLEW_INPUTUNITLIMIT,TITLECAPTION, WAITTIMEOUT);
                    SetFocus(hEditLimit);
                    break;
                }
                dAcmMax = atol(cAcmMax);
            }
            if (-1 == ME_SetACMmax(hWnd,IDC_ME_SETACMMAX,dAcmMax,cPin2))
                AppMessageBox(hWnd,CLEW_SETFAIL,TITLECAPTION, WAITTIMEOUT);
            break;
        case IDC_BUTTON_RIGHT://退出
            DealWithRightButton(hWnd);
            break;
        }
        break;

    case IDC_ME_GETACMMAX://获取最大累计Call Meter值
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            ME_GetResult(&dAcmMax,sizeof(DWORD));
            hEditLimit = GetDlgItem(hWnd,IDC_EDIT_LIMIT);
            if (dAcmMax > 0)
            {
                sprintf(cAcmMax,pString,dAcmMax);//"%ld"
                SetWindowText(hEditLimit,cAcmMax);
                CheckRadioButton(hWnd,IDC_CLOSE,IDC_OPEN,IDC_OPEN);
            }
            else
            {
                SetWindowText(hEditLimit,"");
                CheckRadioButton(hWnd,IDC_CLOSE,IDC_OPEN,IDC_CLOSE);
            }
            break;
        default:
            break;
        }
        break;
    case IDC_ME_SETACMMAX:
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            DestroyWindow(hWnd);
            break;
        default:
            AppMessageBox(hWnd,CLEW_SETFAIL,TITLECAPTION, WAITTIMEOUT);
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
static  BOOL    CreateControl(HWND hWnd,HWND * phClose)
{
    int icontrolw,icontrolh,ix[2];
    int icontrolw1,icontrolw2;
    HWND HOn,hEditLimit;

    SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, // 右键退出
                (WPARAM)IDC_BUTTON_RIGHT,BUTTON_RIGHT );

    SendMessage(hWnd , PWM_CREATECAPTIONBUTTON, //左键确定
                MAKEWPARAM(IDC_BUTTON_LEFT,1),BUTTON_LEFT );

    SendMessage( hWnd, PWM_SETSCROLLSTATE,
                SCROLLLEFT|SCROLLRIGHT|SCROLLDOWN|SCROLLUP, MASKALL );

    icontrolw1  = GetRadioButtonControlWidth(RADIO_CLOSE);
    icontrolw2  = GetRadioButtonControlWidth(RADIO_OPEN);

    icontrolw   = icontrolw1 >= icontrolw2 ? icontrolw1 : icontrolw2;

    CountControlsAbscissa(ix,icontrolw,sizeof(ix));
    icontrolh = GetButtonControlHeight();

    * phClose = CreateWindow(pButtonClass,RADIO_CLOSE,//关
        BS_AUTORADIOBUTTON|WS_CHILD|WS_VISIBLE|WS_TABSTOP,
        ix[0],iControlY,icontrolw,icontrolh,hWnd,(HMENU)IDC_CLOSE,hInstance,NULL);

    if (* phClose == NULL)
        return FALSE;

    HOn = CreateWindow(pButtonClass,RADIO_OPEN,//开
        BS_AUTORADIOBUTTON|WS_CHILD|WS_VISIBLE|WS_TABSTOP,
        ix[1],iControlY,icontrolw,icontrolh,hWnd,(HMENU)IDC_OPEN,hInstance,NULL);

    if (HOn == NULL)
        return FALSE;

    hEditLimit = CreateWindow (pEditClass,"", //单位话费
        WS_VISIBLE | WS_CHILD | ES_CENTER |WS_TABSTOP|ES_NUMBER|WS_BORDER,
        ix[0],iControlY + icontrolh * 2,GetScreenUsableWH1(SM_CXSCREEN) - ix[0] * 2,GetEditControlHeight(),
        hWnd,(HMENU)IDC_EDIT_LIMIT,NULL,NULL);

    if (hEditLimit == NULL)
        return FALSE;

    SendMessage(hEditLimit,EM_LIMITTEXT,ACMMAXLEN,0);

    return TRUE;
}
