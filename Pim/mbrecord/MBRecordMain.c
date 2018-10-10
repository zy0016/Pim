/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 通话记录部分
 *
 * Purpose  : 通话记录主列表部分
 *            
\**************************************************************************/
#include    "winpda.h"
#include    "hp_pdaex.h"
#include    "pubapp.h"
#include    "RcCommon.h"
#include    "../mbphone/MBPublic.h"
#include    "../mbphone/str_mb_plx.h"
#include    "MBRecordList.h"

#define     IDC_BUTTON_QUIT         3//退出
#define     IDC_BUTTON_SET          4//确定
#define     IDC_MENULIST            6

#define     IDC_MENUITEM_UNANSWER   7//未接电话
#define     IDC_MENUITEM_ANSWER     8//已接电话
#define     IDC_MENUITEM_DIAL       9//已拨电话
#define     IDC_MENUITEM_TIME       10//通话记时
#define     IDC_MENUITEM_CALLCOST   11//通话计费
#define     IDC_MENUITEM_COSTTYPE   12//计费方式
#define     IDC_MENUITEM_COSTLIMIT  13//话费限定

#define     TITLECAPTION            MBGetString(STR_MBRECORD_RECORD_CAPTION)//通话记录
#define     BUTTONOK                (LPARAM)MBGetString(STR_WINDOW_OK)      //确定
#define     BUTTONQUIT              (LPARAM)MBGetString(STR_WINDOW_EXIT)    //退出
#define     ERROR1                  MBGetString(STR_MBRECORD_ERROR1)        //出错提示

#define     LISTTXT_UNANSWER        MBGetString(STR_MBRECORD_DEL_UNANSWER)  //未接电话
#define     LISTTXT_ANSWER          MBGetString(STR_MBRECORD_DEL_ANSWER)//已接电话
#define     LISTTXT_DIAL            MBGetString(STR_MBRECORD_DEL_DIAL)  //已拨电话
#define     LISTTXT_TIME            MBGetString(STR_MBRECORD_TIMER)     //通话记时
#define     LISTTXT_CALLCOST        MBGetString(STR_MBRECORD_COST)      //通话计费
#define     LISTTXT_COSTTYPE        MBGetString(STR_MBRECORD_COSTTYPE)  //计费方式
#define     LISTTXT_COSTLIMIT       MBGetString(STR_MBRECORD_COSTSET)   //话费限定

#define     DISPLAY_NUMBER_SINGLE   MBGetString(STR_MBRECORD_UNANSDWER_S)//"%s %d个"
#define     DISPLAY_NUMBER_MUL      MBGetString(STR_MBRECORD_UNANSDWER_M)//"%s %d个"

//********************本地函数***********
static  LRESULT AppWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static  BOOL    CreateControl(HWND hWnd);//创建控件
static  void    SetMenuListText(HWND hMenuList);

typedef enum//主窗口显示风格
{
    MBRECORD_MAIN_WS_DETAIL, //显示细节
    MBRECORD_MAIN_WS_UNDETAIL//不显示细节
}MBRECORDMAIN_STYLE;
static const MBRECORDMAIN_STYLE MBRecordMain_Style = MBRECORD_MAIN_WS_DETAIL;

typedef enum//定义按钮信息
{
    UNDEFINED_LEFTRIGHTBUTTON   = 0,//没有定义任何按键
    DEFINED_LEFTBUTTON          = 1,//定义了左键
    DEFINED_RIGHTTBUTTON        = 2,//定义了右键
}MBRECORDMAIN_BUTTON;
static const DWORD MBRecordMain_Button = DEFINED_LEFTBUTTON | DEFINED_RIGHTTBUTTON;

static HWND             hWndApp = NULL;
static BOOL             bShow; 
static HINSTANCE        hInstance;
static MENULISTTEMPLATE MenuListElement;
static HMENULIST        MenuList;

static const char     * pClassName          = "MBLogMainWndClass";//窗口类名
static const char     * pMenuClassName      = "MENULIST";
static const char     * pListClassName      = "LISTBOX";
static const char     * pMBRecordListPic[]  =
{
    MBPIC_RECORD_UNANSWER,  //未接电话
    MBPIC_RECORD_ANSWER,    //已接电话
    MBPIC_RECORD_DIALED,    //已拨电话
    MBPIC_RECORD_CALLTIME,  //通话记时
    MBPIC_RECORD_CALLCOST,  //通话计费
    MBPIC_RECORD_COSTTYPE,  //计费方式
    MBPIC_RECORD_SETCOST,   //话费限定
    ""
};
//程序的入口函数;
DWORD   MobileRecord_AppControl (int nCode, void* pInstance, WPARAM wParam, LPARAM lParam )//MobileRecord_AppControl
{
    WNDCLASS wc;
    DWORD dwRet = TRUE;

    switch (nCode)
    {
    case APP_INIT :
        hInstance = pInstance;
        InitMBRecordData();//初始化通话记录所需数据
        InitMBPostMsg();
        break;

    case APP_INITINSTANCE :
        break;

    case APP_GETOPTION :
        switch( wParam )
        {
        case AS_ICON:
            break;

        case AS_STATE:
            if( hWndApp )
            {
                if (IsWindowVisible ( hWndApp ) )
                    dwRet = AS_ACTIVE;
                else
                    dwRet = AS_INACTIVE;
            }
            else
            {
                dwRet = AS_NOTRUNNING;
            }
            break;

        case AS_HELP:
            return  NULL;
        }
        break;

    case APP_SETOPTION :
        break;

    case APP_ACTIVE :
        // 注册此程序的窗口类;       
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

        if ( hWndApp )
        {
            if ( bShow )
            {
                ShowWindow ( hWndApp, SW_HIDE );
            }
            else
            {
                ShowWindow ( hWndApp, SW_SHOW );
                SetFocus ( hWndApp );
            }
            bShow = !bShow;
        }
        else
        {
            hWndApp = CreateWindow(pClassName, TITLECAPTION,//窗口  
                WS_CAPTION | WS_BORDER| PWS_STATICBAR, PLX_WIN_POSITION,
                NULL, NULL,NULL,NULL);

            if (NULL == hWndApp)
            {
                UnregisterClass(pClassName,NULL);
                return FALSE;
            }
            ShowWindow(hWndApp, SW_SHOW);
            UpdateWindow(hWndApp);

            bShow = TRUE;
        }
        break;

    case APP_INACTIVE :
        SendMessage ( hWndApp, WM_CLOSE, 0, 0 );
        break;
    case AS_APPWND://返回创建窗口句柄，没有值返回空。
        dwRet = (DWORD)hWndApp;
        break;

    default :
        break;
    }
    return dwRet;
}
BOOL    CallMBLogWindow(void)
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

    hWnd = CreateWindow(pClassName,TITLECAPTION,WS_CAPTION|WS_BORDER|PWS_STATICBAR,
        PLX_WIN_POSITION , NULL, NULL, NULL, NULL);

    if (NULL == hWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd); 
    return (TRUE);
}
//程序的窗口过程函数;
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    HWND    hMenuList;
    LRESULT lResult;
    char    cClassName[20] = "";

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        if (!CreateControl(hWnd))
        {
            AppMessageBox(NULL,ERROR1, TITLECAPTION, WAITTIMEOUT);
            return -1;
        }
        break;

    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd, IDC_MENULIST));
        break;

    case WM_ACTIVATE://去掉这个消息，单击右键可以退出，否则退出失败
        if (WA_INACTIVE != LOWORD(wParam))
        {
            hMenuList = GetDlgItem(hWnd, IDC_MENULIST);
            if (MBRECORD_MAIN_WS_DETAIL == MBRecordMain_Style)
                SetMenuListText(hMenuList);

            SetFocus(hMenuList);//激活后设置焦点
        }
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;

    case WM_CLOSE://点击右上角的退出按钮时为保存退出;
        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_BUTTON_QUIT, 0 );   
        break;

    case WM_DESTROY ://通知程序管理器,此程序退出;
        hWndApp = NULL;
        DestroyMenuList(MenuList);
        UnregisterClass(pClassName,NULL);
        DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
        break;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_F10:
        case VK_F2:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

    case WM_TODESKTOP:
        SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
        return TRUE;

    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        case IDC_MENUITEM_UNANSWER://未接电话
            switch(HIWORD(wParam))
            {
            case ML_SELNOTIFY:
                CallMBLogListExtWindow(hWnd,UNANSWER_REC,MBL_WS_DEFAULT);
                break;
            }
            break;

        case IDC_MENUITEM_ANSWER://已接电话
            switch(HIWORD(wParam))
            {
            case ML_SELNOTIFY:
                CallMBLogListExtWindow(hWnd,ANSWERED_REC,MBL_WS_DEFAULT);
                break;
            }
            break;

        case IDC_MENUITEM_DIAL://已拨电话
            switch(HIWORD(wParam))
            {
            case ML_SELNOTIFY:
                CallMBLogListExtWindow(hWnd,DIALED_REC,MBL_WS_DEFAULT);
                break;
            }
            break;

        case IDC_MENUITEM_TIME://通话记时
            switch(HIWORD(wParam))
            {
            case ML_SELNOTIFY:
                CallMBRecordCostWindow(hWnd);
                break;
            }
            break;

        case IDC_MENUITEM_CALLCOST://通话计费
            switch(HIWORD(wParam))
            {
            case ML_SELNOTIFY:
                if (FindSIMState(TRUE))
                    CallMBRecordFare(hWnd);
                break;
            }
            break;

        case IDC_MENUITEM_COSTTYPE://计费方式
            switch (HIWORD(wParam))
            {
            case ML_SELNOTIFY:
                if (FindSIMState(TRUE))
                {
                    if (CallValidPin2(hWnd))
                        CallChargingTypeWindow(hWnd);
                }
                break;
            }
            break;

        case IDC_MENUITEM_COSTLIMIT: //话费限定
            switch (HIWORD(wParam))
            {
            case ML_SELNOTIFY:
                if (FindSIMState(TRUE))
                {
                    if (CallValidPin2(hWnd))
                        CallChargingLimitWindow(hWnd);
                }
                break;
            }
            break;

        case IDC_BUTTON_SET://设置
            hMenuList = GetDlgItem(hWnd, IDC_MENULIST);
            GetClassName(hMenuList,cClassName,sizeof(cClassName));
            if (strcmp(cClassName,pMenuClassName) == 0)
            {
                SendMessage(hWnd,WM_KEYDOWN,VK_F5,0);
            }
            else if (strcmp(cClassName,pListClassName) == 0)
            {
                switch (SendMessage(hMenuList,LB_GETCURSEL,0,0))
                {
                case 0://未接电话
                    CallMBLogListExtWindow(hWnd,UNANSWER_REC,MBL_WS_DEFAULT);
                    break;
                case 1://已接电话
                    CallMBLogListExtWindow(hWnd,ANSWERED_REC,MBL_WS_DEFAULT);
                    break;
                case 2://已拨电话
                    CallMBLogListExtWindow(hWnd,DIALED_REC,MBL_WS_DEFAULT);
                    break;
                case 3://通话记时
                    CallMBRecordCostWindow(hWnd);
                    break;
                case 4://通话计费
                    if (FindSIMState(TRUE))
                        CallMBRecordFare(hWnd);
                    break;
                case 5://计费方式
                    if (FindSIMState(TRUE))
                    {
                        if (CallValidPin2(hWnd))
                            CallChargingTypeWindow(hWnd);
                    }
                    break;
                case 6://话费限定
                    if (FindSIMState(TRUE))
                    {
                        if (CallValidPin2(hWnd))
                            CallChargingLimitWindow(hWnd);
                    }
                    break;
                }
            }
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
/********************************************************************
* Function   SetSingleMenuListText  
* Purpose     设置通话记录单个列表中的文字
* Params     
* Return     
* Remarks      
**********************************************************************/
static  void    SetSingleMenuListText(HWND hMenuList,MBRECORDTYPE CallRecType,
                                      const char * pNumItem0,int iMenuItem)
{
    char szDisplay[100] = "",cClassName[20] = "";
    int iNum;

    iNum = GetRecorderNumber(CallRecType);
    if (iNum == 0)
        strcpy(szDisplay,pNumItem0);
    else if (iNum == 1)
        sprintf(szDisplay,DISPLAY_NUMBER_SINGLE,pNumItem0,iNum);
    else
        sprintf(szDisplay,DISPLAY_NUMBER_MUL,pNumItem0,iNum);

    GetClassName(hMenuList,cClassName,sizeof(cClassName));
    if (strcmp(cClassName,pMenuClassName) == 0)
    {
        SendMessage(hMenuList,ML_SETITEMTEXT,iMenuItem,(LPARAM)szDisplay);
    }
    else if (strcmp(cClassName,pListClassName) == 0)
    {
        SendMessage(hMenuList,LB_DELETESTRING,0,0);
        SendMessage(hMenuList,LB_INSERTSTRING,0,(LPARAM)szDisplay);
    }
}
/********************************************************************
* Function   SetMenuListText  
* Purpose     设置通话记录列表中的文字
* Params     
* Return     
* Remarks      
**********************************************************************/
static  void    SetMenuListText(HWND hMenuList)
{
    SetSingleMenuListText(hMenuList,UNANSWER_REC,LISTTXT_UNANSWER,IDC_MENUITEM_UNANSWER );
    SetSingleMenuListText(hMenuList,ANSWERED_REC,LISTTXT_ANSWER  ,IDC_MENUITEM_ANSWER   );
    SetSingleMenuListText(hMenuList,DIALED_REC  ,LISTTXT_DIAL    ,IDC_MENUITEM_DIAL     );
}
/********************************************************************
* Function   CreateControl  
* Purpose    创建控件 
* Params     hwnd：窗口过程句柄
* Return     
* Remarks      
**********************************************************************/
static  BOOL    CreateControl(HWND hWnd)
{
    HWND hMenuList;
    int isw,ish;
    char * listtxt[] =
    {
        "",//未接电话
        "",//已接电话
        "",//已拨电话
        "",//通话记时
        "",//通话计费
        "",//计费方式
        "",//话费限定
        ""
    };
    int idList[] = 
    {
        IDC_MENUITEM_UNANSWER,  //未接电话
        IDC_MENUITEM_ANSWER,    //已接电话
        IDC_MENUITEM_DIAL,      //已拨电话
        IDC_MENUITEM_TIME,      //通话记时
        IDC_MENUITEM_CALLCOST,  //通话计费
        IDC_MENUITEM_COSTTYPE,  //计费方式
        IDC_MENUITEM_COSTLIMIT, //话费限定
        -1
    };

    listtxt[0] = LISTTXT_UNANSWER;  //未接电话
    listtxt[1] = LISTTXT_ANSWER;    //已接电话
    listtxt[2] = LISTTXT_DIAL;      //已拨电话
    listtxt[3] = LISTTXT_TIME;      //通话记时
    listtxt[4] = LISTTXT_CALLCOST;  //通话计费
    listtxt[5] = LISTTXT_COSTTYPE;  //计费方式
    listtxt[6] = LISTTXT_COSTLIMIT; //话费限定

    if (MBRecordMain_Button & DEFINED_LEFTBUTTON)
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, //左键确定
        MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK);

    if (MBRecordMain_Button & DEFINED_RIGHTTBUTTON)
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, // 右键退出
        (WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT);

    SendMessage(hWnd,PWM_SETSCROLLSTATE,
                SCROLLLEFT|SCROLLRIGHT|SCROLLDOWN|SCROLLUP,MASKALL);

    GetScreenUsableWH2(&isw,&ish);

    MenuList = LoadMenuList(&MenuListElement,FALSE);

    if (MenuList == NULL)//创建listbox控件
    {
        hMenuList = CreateWindow(pListClassName,"",
            WS_VISIBLE | WS_CHILD | LBS_BITMAP | WS_VSCROLL,
            0,0,isw,ish,hWnd,(HMENU)IDC_MENULIST,NULL,NULL);
    }
    else
    {
        hMenuList = CreateWindow(pMenuClassName, "", //显示情景模式各个功能列表的列表框
            WS_CHILD | WS_VISIBLE | WS_VSCROLL,0, 0 , isw, ish,
            hWnd, (HMENU)IDC_MENULIST, NULL, (PVOID)MenuList);
    }
    if (hMenuList == NULL)
        return FALSE;
    MenuList_AddString(hMenuList,listtxt,idList,(char**)pMBRecordListPic);
    return TRUE;
}
