/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 验证pin2码
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
#include    "me_wnd.h"

#define     IDC_BUTTON_QUIT         3//退出按钮
#define     IDC_BUTTON_SET          4//确定按钮
#define     IDC_EDITPIN2            5//pin2码
#define     IDC_VALIDPIN2           WM_USER + 100//验证pin2码的回调消息
#define     IDC_ME_PIN2LIMIT        WM_USER + 110//pin2码失效上限
#define     IDC_VALIDPUK2           WM_USER + 120//验证puk2码的回调消息

#define     TITLECAPTION            MBGetString(STR_MBRECORD_VALIDPIN2)//验证PIN2码
#define     BUTTONOK                (LPARAM)MBGetString(STR_WINDOW_OK)    //确定
#define     BUTTONQUIT              (LPARAM)MBGetString(STR_WINDOW_EXIT)  //退出
#define     ERROR1                  MBGetString(STR_MBRECORD_ERROR1)      //出错提示
#define     CLEWPIN2                MBGetString(STR_MBRECORD_CLEWPIN2)//请输入PIN2码
#define     VALIDPIN2FAIL           MBGetString(STR_MBRECORD_VALIDPIN2FAIL)//PIN2码验证失败
#define     VALIDPUK2FAIL           MBGetString(STR_MBRECORD_PUK2ERROR)//PUK2码错误
#define     CLEWPUK2                MBGetString(STR_MBRECORD_INPUTPUK2)//请输入PUK2码
#define     CLEWPIN2_NEW            MBGetString(STR_MBRECORD_INPUTNEWPIN2)//请输入新PIN2码
#define     CLEWPIN2_CON            MBGetString(STR_MBRECORD_NEWPIN2CON)//确认PIN2码
#define     CLEW_ERROR              MBGetString(STR_MBRECORD_CODECLEW)//密码不能为空
#define     CLEW_ERROR2             MBGetString(STR_MBRECORD_CONCODEERROR)//确认密码错误

#define PUK2NUMS                    10      //可以输入PUK码的最多次数

static const char * pClassName = "MBValidPin2WndClass";//窗口类名
static const char * pEditClass = "DINPUTEDIT";
static HINSTANCE    hInstance;
static BOOL         bPin2Valid;//pin2码是否合法
static char         cValidPin2[PIN2MAXLEN + 1];//pin2码
static int          iOperateType;//界面类型
static BOOL         bPin2Check = FALSE;//pin2码是否已经验证

#define TYPE_PIN2       0//输入pin2码    
#define TYPE_PUK2       1//输入puk2码    
#define TYPE_NEWPIN2    2//输入新的pin2码 
#define TYPE_NEWCONPIN2 3//确认新的pin2码 

static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  BOOL    CreateControl(HWND hWnd);//创建控件

BOOL    CallValidPin2(HWND hwndCall)
{
    HWND        hWnd;
    WNDCLASS    wc;
    MSG         msg;

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

    bPin2Valid       = FALSE;

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

    while (IsWindow(hWnd))
    {
        GetMessage(&msg, NULL, 0, 0);
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (bPin2Valid);
}
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static  char        cPuk2[PIN2MAXLEN + 1] = "";
    static  char        cPin2_new[PIN2MAXLEN + 1] = "";
            HWND        hEdit;
            char        cPin2[PIN2MAXLEN + 1] = "";
            LRESULT     lResult;
            HDC         hdc;
            RECT        rect;
            int         iPassLimit = 0;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        if (!CreateControl(hWnd))
        {
            AppMessageBox(NULL,ERROR1, TITLECAPTION, WAITTIMEOUT);
            return -1;
        }
        iOperateType = TYPE_PIN2;

        ME_GetPassInputLimit(hWnd,IDC_ME_PIN2LIMIT,PIN2);//pin2码失效上限
        break;

    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd,IDC_EDITPIN2));
        break;

    case WM_SETRBTNTEXT:
        SetRightButtonText(hWnd,wParam,lParam);
        break;

    case WM_ACTIVATE://去掉这个消息，单击右键可以退出，否则退出失败
        if (WA_INACTIVE != LOWORD(wParam))
            SetFocus(GetDlgItem(hWnd,IDC_EDITPIN2));//激活后设置焦点
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
        hdc = BeginPaint( hWnd, NULL);
        SetRect(&rect,10,10,GetScreenUsableWH1(SM_CXSCREEN) - 20,30);
        switch (iOperateType)
        {
        case TYPE_PIN2:
            DrawText(hdc,CLEWPIN2,strlen(CLEWPIN2),&rect,DT_CENTER);
            break;
        case TYPE_PUK2:
            DrawText(hdc,CLEWPUK2,strlen(CLEWPUK2),&rect,DT_CENTER);
            break;
        case TYPE_NEWPIN2:
            DrawText(hdc,CLEWPIN2_NEW,strlen(CLEWPIN2_NEW),&rect,DT_CENTER);
            break;
        case TYPE_NEWCONPIN2:
            DrawText(hdc,CLEWPIN2_CON,strlen(CLEWPIN2_CON),&rect,DT_CENTER);
            break;
        }
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
        case IDC_BUTTON_SET://设置
            hEdit = GetDlgItem(hWnd,IDC_EDITPIN2);
            GetWindowText(hEdit,cPin2,GetWindowTextLength(hEdit) + 1);
            if (strlen(cPin2) == 0)
            {
                AppMessageBox(hWnd,CLEW_ERROR,TITLECAPTION, WAITTIMEOUT);
                break;
            }
            switch (iOperateType)
            {
            case TYPE_PIN2:
                if (bPin2Check)//pin2码已经验证
                {
                    char cPinChecked[PIN2MAXLEN + 1] = "";
                    GetValidPin2(cPinChecked);
                    if (strcmp(cPin2,cPinChecked) == 0)
                    {
                        PostMessage(hWnd,IDC_VALIDPIN2,ME_RS_SUCCESS,0);
                    }
                    else
                    {
                        AppMessageBox(hWnd,VALIDPIN2FAIL,TITLECAPTION, WAITTIMEOUT);
                        SetWindowText(hEdit,"");
                        SetFocus(hEdit);
                    }
                }
                else//pin2码尚未验证
                {
                    if (-1 == ME_PasswordValidation(hWnd,IDC_VALIDPIN2,PIN2,cPin2,NULL))//验证pin2码
                    {
#ifndef _EMULATE_
                        AppMessageBox(hWnd,VALIDPIN2FAIL,TITLECAPTION, WAITTIMEOUT);
#else
                        PostMessage(hWnd,IDC_VALIDPIN2,ME_RS_SUCCESS,0);
#endif
                    }
                }
                break;
            case TYPE_PUK2:
                GetWindowText(hEdit,cPuk2,GetWindowTextLength(hEdit) + 1);

                iOperateType = TYPE_NEWPIN2;//提示输入新的pin2码
                SetWindowText(hEdit,"");
                SetFocus(hEdit);
                InvalidateRect(hWnd,NULL,TRUE);
                break;
            case TYPE_NEWPIN2:
                GetWindowText(hEdit,cPin2_new,GetWindowTextLength(hEdit) + 1);

                iOperateType = TYPE_NEWCONPIN2;//提示输入确认pin2码
                SetWindowText(hEdit,"");
                SetFocus(hEdit);
                InvalidateRect(hWnd,NULL,TRUE);
                break;
            case TYPE_NEWCONPIN2:
                if (strcmp(cPin2_new,cPin2) != 0)//新pin2码和确认密码不一致
                {
                    AppMessageBox(hWnd,CLEW_ERROR2,TITLECAPTION, WAITTIMEOUT);
                    iOperateType = TYPE_NEWPIN2;//提示重新输入pin2码
                    SetWindowText(hEdit,"");
                    SetFocus(hEdit);
                    InvalidateRect(hWnd,NULL,TRUE);
                    break;
                }
                if (-1 == ME_PasswordValidation(hWnd,IDC_VALIDPUK2,PUK2,cPuk2,cPin2_new))
                {
#ifndef _EMULATE_
                    AppMessageBox(hWnd,VALIDPUK2FAIL,TITLECAPTION, WAITTIMEOUT);
#else
                    PostMessage(hWnd,IDC_VALIDPUK2,ME_RS_SUCCESS,0);
#endif
                }
                break;
            }
            break;
        case IDC_BUTTON_QUIT://退出
            hEdit = GetDlgItem(hWnd,IDC_EDITPIN2);
            switch (iOperateType)
            {
            case TYPE_PIN2:
                bPin2Valid = FALSE;
                DestroyWindow(hWnd);
                break;
            case TYPE_PUK2:
                AppMessageBox(hWnd,CLEWPUK2,TITLECAPTION, WAITTIMEOUT);
                SetFocus(hEdit);
                break;
            case TYPE_NEWPIN2:
                iOperateType = TYPE_PUK2;
                SetWindowText(hEdit,"");
                SetFocus(hEdit);
                InvalidateRect(hWnd,NULL,TRUE);
                break;
            case TYPE_NEWCONPIN2:
                iOperateType = TYPE_NEWPIN2;
                SetWindowText(hEdit,"");
                SetFocus(hEdit);
                InvalidateRect(hWnd,NULL,TRUE);
                break;
            }
            break;
        }
        break;
    //*************me回调消息***************************
    case IDC_VALIDPIN2://验证pin2码
        hEdit = GetDlgItem(hWnd,IDC_EDITPIN2);
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            GetWindowText(hEdit,cValidPin2,GetWindowTextLength(hEdit) + 1);
            bPin2Valid = TRUE;
            bPin2Check = TRUE;//pin2码已经验证
            DestroyWindow(hWnd);
            break;
        default:
            ME_GetPassInputLimit(hWnd,IDC_ME_PIN2LIMIT,PIN2);//pin2码失效上限
            AppMessageBox(hWnd,VALIDPIN2FAIL,TITLECAPTION, WAITTIMEOUT);
            SetWindowText(hEdit,"");
            SetFocus(hEdit);
            break;
        }
        break;

    case IDC_ME_PIN2LIMIT://pin2码失效上限
        hEdit = GetDlgItem(hWnd,IDC_EDITPIN2);
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            ME_GetResult(&iPassLimit,sizeof(int));
            if ((iPassLimit == 0) || (iPassLimit == PUK2NUMS))//需要输入puk2码
            {
                iOperateType = TYPE_PUK2;
                SetWindowText(hEdit,"");
                SetFocus(hEdit);
                InvalidateRect(hWnd,NULL,TRUE);
            }
            break;
        default:
            break;
        }
        break;

    case IDC_VALIDPUK2://验证puk2码的回调消息
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            strcpy(cValidPin2,cPin2_new);
            bPin2Valid = TRUE;
            bPin2Check = TRUE;//pin2码已经验证
            DestroyWindow(hWnd);
            break;
        default:
            hEdit = GetDlgItem(hWnd,IDC_EDITPIN2);
            AppMessageBox(hWnd,VALIDPUK2FAIL,TITLECAPTION, WAITTIMEOUT);
            iOperateType = TYPE_PUK2;//重新输入puk2码
            SetWindowText(hEdit,"");
            SetFocus(hEdit);
            InvalidateRect(hWnd,NULL,TRUE);
            break;
        }
        break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
void    GetValidPin2(char * pPin2)//获得正确的pin2码
{
    strcpy(pPin2,cValidPin2);
}
void    SetPin2Change(void)//表示PIN2码已经修改
{
    bPin2Check = FALSE;
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
    int icontrolx = 10;
    HWND hEdit;

    SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, // 右键退出
                (WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT );

    SendMessage(hWnd , PWM_CREATECAPTIONBUTTON, //左键确定
                MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK );

    hEdit = CreateWindow (pEditClass,"",
        WS_VISIBLE | WS_CHILD  | ES_CENTER |WS_TABSTOP|WS_BORDER|ES_PASSWORD,
        icontrolx, 40, GetScreenUsableWH1(SM_CXSCREEN) - icontrolx * 2, GetEditControlHeight(),
        hWnd,(HMENU)IDC_EDITPIN2,NULL,NULL);

    if (hEdit == NULL)
        return FALSE;

    SendMessage(hEdit,EM_LIMITTEXT,8,0);//限制字符个数

    return TRUE;
}
