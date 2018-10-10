/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 打接电话部分
 *
 * Purpose  : 显示列表
 *            
\**************************************************************************/
#include "stdio.h"
#include "stdlib.h"
#include "window.h"
#include "string.h"
#include "pubapp.h"
#include "pmi.h"
#include "MBPublic.h"
#include "str_mb_plx.h"
#include "winpda.h"
#include "hp_pdaex.h"

#define     IDC_BUTTON_QUIT         3//退出按钮
#define     IDC_BUTTON_SET          4//确定按钮
#define     IDC_MENULIST            6

#define     IDC_MENUITEM0           7   //记事本
#define     IDC_MENUITEM1           8   //日程
#define     IDC_MENUITEM2           9   //联系人
#define     IDC_MENUITEM3           10  //短信
#define     IDC_MENUITEM4           11  //彩信
#define     IDC_MENUITEM5           12  //邮件

#define     TITLECAPTION            MBGetString(STR_MBPHONE_VIEW)
#define     ERROR1                  MBGetString(STR_MBRECORD_ERROR1)    //出错提示
#define     LISTVIEW0               MBGetString(STR_MBPHONE_VIEW_LIST0)//"记事本"
#define     LISTVIEW1               MBGetString(STR_MBPHONE_VIEW_LIST1)//"日程"
#define     LISTVIEW2               MBGetString(STR_MBRECORD_LIST_LINKMAN)//"联系人"
#define     LISTVIEW3               MBGetString(STR_MBRECORD_LIST_SMS)//"短信"
#define     LISTVIEW4               MBGetString(STR_MBRECORD_LIST_MMS)//"彩信"
#define     LISTVIEW5               MBGetString(STR_MBPHONE_VIEW_LIST2)//"邮件"

static HINSTANCE    hInstance;
static const char * pClassName = "MBViewWndClass";//窗口类名
static const char * pViewList1 = "S:记事本";
static const char * pViewList2 = "S:日程";
static const char * pViewList3 = "S:联系人";
static const char * pViewList4 = "S:信息";
static const char * pViewList5 = "S:邮件";
static const char * pMBPic_List[] =
{
    MBPIC_VIEWLIST1,
    MBPIC_VIEWLIST2,
    MBPIC_VIEWLIST3,
    MBPIC_VIEWLIST4,
    MBPIC_VIEWLIST5,
    MBPIC_VIEWLIST6,
    ""
};

static MENULISTTEMPLATE MenuListElement;
static HMENULIST        MenuList;
static HWND             hWndViewList;

static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static BOOL    CreateControl(HWND hWnd);
//*********************************************************
BOOL    MBViewList(HWND hwndCall)
{
    WNDCLASS wc;

    if (IsWindow(hWndViewList))
    {
        MoveWindowToTop(hWndViewList);
        ShowWindow(hWndViewList,SW_SHOW);
        UpdateWindow(hWndViewList);
        return TRUE;
    }
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

    hWndViewList = CreateWindow(pClassName,
        TITLECAPTION,WS_CAPTION|WS_BORDER |PWS_STATICBAR, 
        PLX_WIN_POSITION,hwndCall, NULL, NULL, NULL);

    if (NULL == hWndViewList)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }

    ShowWindow(hWndViewList, SW_SHOW);
    UpdateWindow(hWndViewList); 

    return (TRUE);
}
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    LRESULT lResult;

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
        SetFocus(GetDlgItem(hWnd,IDC_MENULIST));
        break;

    case WM_ACTIVATE://去掉这个消息，单击右键可以退出，否则退出失败
        if (WA_INACTIVE == LOWORD(wParam))
            SetFocus(GetDlgItem(hWnd,IDC_MENULIST));//激活后设置焦点

        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;

    case WM_CLOSE://点击右上角的退出按钮时为保存退出;
        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_BUTTON_QUIT, 0 );   
        break;

    case WM_DESTROY ://通知程序管理器,此程序退出;
        hWndViewList = NULL;
        UnregisterClass(pClassName,NULL);
        DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
        break;

    case WM_TODESKTOP:
        SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
        return FALSE;

    case WM_KEYDOWN:
        switch (LOWORD(wParam))
        {
        case VK_F10:
            DestroyWindow( hWnd );  
            break;
        default:
            return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        case IDC_MENUITEM0://记事本
            switch(HIWORD(wParam))
            {
            case ML_SELNOTIFY:
                DlmNotify(PES_APP_CREATE,(int)pViewList1);//"S:记事本"
                DestroyWindow(hWnd);
                break;
            }
            break;
        case IDC_MENUITEM1://日程
            switch(HIWORD(wParam))
            {
            case ML_SELNOTIFY:
                DlmNotify(PES_APP_CREATE,(int)pViewList2);//"S:日程"
                DestroyWindow(hWnd);
                break;
            }
            break;
        case IDC_MENUITEM2://联系人
            switch(HIWORD(wParam))
            {
            case ML_SELNOTIFY:
                DlmNotify(PES_APP_CREATE,(int)pViewList3);//"S:联系人"
                DestroyWindow(hWnd);
                break;
            }
            break;
        case IDC_MENUITEM3://短信
            switch(HIWORD(wParam))
            {
            case ML_SELNOTIFY:
                DlmNotify(PES_APP_CREATE,(int)pViewList4);//"S:消息"
                DestroyWindow(hWnd);
                break;
            }
            break;
        case IDC_MENUITEM4://彩信
            switch(HIWORD(wParam))
            {
            case ML_SELNOTIFY:
                DlmNotify(PES_APP_CREATE,(int)pViewList4);//"S:消息"
                DestroyWindow(hWnd);
                break;
            }
            break;
        case IDC_MENUITEM5://邮件
            switch(HIWORD(wParam))
            {
            case ML_SELNOTIFY:
                DlmNotify(PES_APP_CREATE,(int)pViewList5);//"S:电子邮件"
                DestroyWindow(hWnd);
                break;
            }
            break;
        case IDC_BUTTON_SET://设置
            SendMessage(hWnd,WM_KEYDOWN,VK_F5,0);
            break;
        case IDC_BUTTON_QUIT://退出
            DestroyMenuList(MenuList);
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
* Function   DestroyViewList  
* Purpose     销毁显示列表窗口
* Params     
* Return     
* Remarks     
**********************************************************************/
void    DestroyViewList(void)
{
    if (IsWindow(hWndViewList))
        DestroyWindow(hWndViewList);
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
    int isw,ish;
    HWND hMenuList;
    char *  listtxt[] =
    {
        "",//记事本
        "",//日程  
        "",//联系人
        "",//短信  
        "",//彩信  
        "",//邮件  
        ""
    };
    int idList[] = 
    {
        IDC_MENUITEM0,//记事本
        IDC_MENUITEM1,//日程  
        IDC_MENUITEM2,//联系人
        IDC_MENUITEM3,//短信  
        IDC_MENUITEM4,//彩信  
        IDC_MENUITEM5,//邮件  
        -1
    };

    listtxt[0] = LISTVIEW0;//记事本
    listtxt[1] = LISTVIEW1;//日程  
    listtxt[2] = LISTVIEW2;//联系人
    listtxt[3] = LISTVIEW3;//短信  
    listtxt[4] = LISTVIEW4;//彩信  
    listtxt[5] = LISTVIEW5;//邮件  

    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT,(LPARAM)MBGetString(STR_WINDOW_BACK));
    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_SET,1),(LPARAM)MBGetString(STR_WINDOW_OK));
    SendMessage(hWnd,PWM_SETSCROLLSTATE,SCROLLLEFT|SCROLLRIGHT|SCROLLDOWN|SCROLLUP,MASKALL);

    GetScreenUsableWH2(&isw,&ish);

    MenuList = LoadMenuList(&MenuListElement,FALSE);

    if (MenuList == NULL)
        return FALSE;

    hMenuList = CreateWindow("MENULIST","",WS_CHILD | WS_VISIBLE | WS_VSCROLL,
        0,0,isw,ish,hWnd,(HMENU)IDC_MENULIST,NULL,(PVOID)MenuList);

    if (hMenuList == NULL)
        return FALSE;

    MenuList_AddString(hMenuList,listtxt,idList,(char**)pMBPic_List);

    return TRUE;
}
