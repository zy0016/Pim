/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : ͨ����¼����
 *
 * Purpose  : ͨ����¼���б���
 *            
\**************************************************************************/
#include    "winpda.h"
#include    "hp_pdaex.h"
#include    "pubapp.h"
#include    "RcCommon.h"
#include    "../mbphone/MBPublic.h"
#include    "../mbphone/str_mb_plx.h"
#include    "MBRecordList.h"

#define     IDC_BUTTON_QUIT         3//�˳�
#define     IDC_BUTTON_SET          4//ȷ��
#define     IDC_MENULIST            6

#define     IDC_MENUITEM_UNANSWER   7//δ�ӵ绰
#define     IDC_MENUITEM_ANSWER     8//�ѽӵ绰
#define     IDC_MENUITEM_DIAL       9//�Ѳ��绰
#define     IDC_MENUITEM_TIME       10//ͨ����ʱ
#define     IDC_MENUITEM_CALLCOST   11//ͨ���Ʒ�
#define     IDC_MENUITEM_COSTTYPE   12//�Ʒѷ�ʽ
#define     IDC_MENUITEM_COSTLIMIT  13//�����޶�

#define     TITLECAPTION            MBGetString(STR_MBRECORD_RECORD_CAPTION)//ͨ����¼
#define     BUTTONOK                (LPARAM)MBGetString(STR_WINDOW_OK)      //ȷ��
#define     BUTTONQUIT              (LPARAM)MBGetString(STR_WINDOW_EXIT)    //�˳�
#define     ERROR1                  MBGetString(STR_MBRECORD_ERROR1)        //������ʾ

#define     LISTTXT_UNANSWER        MBGetString(STR_MBRECORD_DEL_UNANSWER)  //δ�ӵ绰
#define     LISTTXT_ANSWER          MBGetString(STR_MBRECORD_DEL_ANSWER)//�ѽӵ绰
#define     LISTTXT_DIAL            MBGetString(STR_MBRECORD_DEL_DIAL)  //�Ѳ��绰
#define     LISTTXT_TIME            MBGetString(STR_MBRECORD_TIMER)     //ͨ����ʱ
#define     LISTTXT_CALLCOST        MBGetString(STR_MBRECORD_COST)      //ͨ���Ʒ�
#define     LISTTXT_COSTTYPE        MBGetString(STR_MBRECORD_COSTTYPE)  //�Ʒѷ�ʽ
#define     LISTTXT_COSTLIMIT       MBGetString(STR_MBRECORD_COSTSET)   //�����޶�

#define     DISPLAY_NUMBER_SINGLE   MBGetString(STR_MBRECORD_UNANSDWER_S)//"%s %d��"
#define     DISPLAY_NUMBER_MUL      MBGetString(STR_MBRECORD_UNANSDWER_M)//"%s %d��"

//********************���غ���***********
static  LRESULT AppWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static  BOOL    CreateControl(HWND hWnd);//�����ؼ�
static  void    SetMenuListText(HWND hMenuList);

typedef enum//��������ʾ���
{
    MBRECORD_MAIN_WS_DETAIL, //��ʾϸ��
    MBRECORD_MAIN_WS_UNDETAIL//����ʾϸ��
}MBRECORDMAIN_STYLE;
static const MBRECORDMAIN_STYLE MBRecordMain_Style = MBRECORD_MAIN_WS_DETAIL;

typedef enum//���尴ť��Ϣ
{
    UNDEFINED_LEFTRIGHTBUTTON   = 0,//û�ж����κΰ���
    DEFINED_LEFTBUTTON          = 1,//���������
    DEFINED_RIGHTTBUTTON        = 2,//�������Ҽ�
}MBRECORDMAIN_BUTTON;
static const DWORD MBRecordMain_Button = DEFINED_LEFTBUTTON | DEFINED_RIGHTTBUTTON;

static HWND             hWndApp = NULL;
static BOOL             bShow; 
static HINSTANCE        hInstance;
static MENULISTTEMPLATE MenuListElement;
static HMENULIST        MenuList;

static const char     * pClassName          = "MBLogMainWndClass";//��������
static const char     * pMenuClassName      = "MENULIST";
static const char     * pListClassName      = "LISTBOX";
static const char     * pMBRecordListPic[]  =
{
    MBPIC_RECORD_UNANSWER,  //δ�ӵ绰
    MBPIC_RECORD_ANSWER,    //�ѽӵ绰
    MBPIC_RECORD_DIALED,    //�Ѳ��绰
    MBPIC_RECORD_CALLTIME,  //ͨ����ʱ
    MBPIC_RECORD_CALLCOST,  //ͨ���Ʒ�
    MBPIC_RECORD_COSTTYPE,  //�Ʒѷ�ʽ
    MBPIC_RECORD_SETCOST,   //�����޶�
    ""
};
//�������ں���;
DWORD   MobileRecord_AppControl (int nCode, void* pInstance, WPARAM wParam, LPARAM lParam )//MobileRecord_AppControl
{
    WNDCLASS wc;
    DWORD dwRet = TRUE;

    switch (nCode)
    {
    case APP_INIT :
        hInstance = pInstance;
        InitMBRecordData();//��ʼ��ͨ����¼��������
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
        // ע��˳���Ĵ�����;       
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
            hWndApp = CreateWindow(pClassName, TITLECAPTION,//����  
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
    case AS_APPWND://���ش������ھ����û��ֵ���ؿա�
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
//����Ĵ��ڹ��̺���;
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

    case WM_ACTIVATE://ȥ�������Ϣ�������Ҽ������˳��������˳�ʧ��
        if (WA_INACTIVE != LOWORD(wParam))
        {
            hMenuList = GetDlgItem(hWnd, IDC_MENULIST);
            if (MBRECORD_MAIN_WS_DETAIL == MBRecordMain_Style)
                SetMenuListText(hMenuList);

            SetFocus(hMenuList);//��������ý���
        }
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;

    case WM_CLOSE://������Ͻǵ��˳���ťʱΪ�����˳�;
        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_BUTTON_QUIT, 0 );   
        break;

    case WM_DESTROY ://֪ͨ���������,�˳����˳�;
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
        case IDC_MENUITEM_UNANSWER://δ�ӵ绰
            switch(HIWORD(wParam))
            {
            case ML_SELNOTIFY:
                CallMBLogListExtWindow(hWnd,UNANSWER_REC,MBL_WS_DEFAULT);
                break;
            }
            break;

        case IDC_MENUITEM_ANSWER://�ѽӵ绰
            switch(HIWORD(wParam))
            {
            case ML_SELNOTIFY:
                CallMBLogListExtWindow(hWnd,ANSWERED_REC,MBL_WS_DEFAULT);
                break;
            }
            break;

        case IDC_MENUITEM_DIAL://�Ѳ��绰
            switch(HIWORD(wParam))
            {
            case ML_SELNOTIFY:
                CallMBLogListExtWindow(hWnd,DIALED_REC,MBL_WS_DEFAULT);
                break;
            }
            break;

        case IDC_MENUITEM_TIME://ͨ����ʱ
            switch(HIWORD(wParam))
            {
            case ML_SELNOTIFY:
                CallMBRecordCostWindow(hWnd);
                break;
            }
            break;

        case IDC_MENUITEM_CALLCOST://ͨ���Ʒ�
            switch(HIWORD(wParam))
            {
            case ML_SELNOTIFY:
                if (FindSIMState(TRUE))
                    CallMBRecordFare(hWnd);
                break;
            }
            break;

        case IDC_MENUITEM_COSTTYPE://�Ʒѷ�ʽ
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

        case IDC_MENUITEM_COSTLIMIT: //�����޶�
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

        case IDC_BUTTON_SET://����
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
                case 0://δ�ӵ绰
                    CallMBLogListExtWindow(hWnd,UNANSWER_REC,MBL_WS_DEFAULT);
                    break;
                case 1://�ѽӵ绰
                    CallMBLogListExtWindow(hWnd,ANSWERED_REC,MBL_WS_DEFAULT);
                    break;
                case 2://�Ѳ��绰
                    CallMBLogListExtWindow(hWnd,DIALED_REC,MBL_WS_DEFAULT);
                    break;
                case 3://ͨ����ʱ
                    CallMBRecordCostWindow(hWnd);
                    break;
                case 4://ͨ���Ʒ�
                    if (FindSIMState(TRUE))
                        CallMBRecordFare(hWnd);
                    break;
                case 5://�Ʒѷ�ʽ
                    if (FindSIMState(TRUE))
                    {
                        if (CallValidPin2(hWnd))
                            CallChargingTypeWindow(hWnd);
                    }
                    break;
                case 6://�����޶�
                    if (FindSIMState(TRUE))
                    {
                        if (CallValidPin2(hWnd))
                            CallChargingLimitWindow(hWnd);
                    }
                    break;
                }
            }
            break;

        case IDC_BUTTON_QUIT://�˳�
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
* Purpose     ����ͨ����¼�����б��е�����
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
* Purpose     ����ͨ����¼�б��е�����
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
* Purpose    �����ؼ� 
* Params     hwnd�����ڹ��̾��
* Return     
* Remarks      
**********************************************************************/
static  BOOL    CreateControl(HWND hWnd)
{
    HWND hMenuList;
    int isw,ish;
    char * listtxt[] =
    {
        "",//δ�ӵ绰
        "",//�ѽӵ绰
        "",//�Ѳ��绰
        "",//ͨ����ʱ
        "",//ͨ���Ʒ�
        "",//�Ʒѷ�ʽ
        "",//�����޶�
        ""
    };
    int idList[] = 
    {
        IDC_MENUITEM_UNANSWER,  //δ�ӵ绰
        IDC_MENUITEM_ANSWER,    //�ѽӵ绰
        IDC_MENUITEM_DIAL,      //�Ѳ��绰
        IDC_MENUITEM_TIME,      //ͨ����ʱ
        IDC_MENUITEM_CALLCOST,  //ͨ���Ʒ�
        IDC_MENUITEM_COSTTYPE,  //�Ʒѷ�ʽ
        IDC_MENUITEM_COSTLIMIT, //�����޶�
        -1
    };

    listtxt[0] = LISTTXT_UNANSWER;  //δ�ӵ绰
    listtxt[1] = LISTTXT_ANSWER;    //�ѽӵ绰
    listtxt[2] = LISTTXT_DIAL;      //�Ѳ��绰
    listtxt[3] = LISTTXT_TIME;      //ͨ����ʱ
    listtxt[4] = LISTTXT_CALLCOST;  //ͨ���Ʒ�
    listtxt[5] = LISTTXT_COSTTYPE;  //�Ʒѷ�ʽ
    listtxt[6] = LISTTXT_COSTLIMIT; //�����޶�

    if (MBRecordMain_Button & DEFINED_LEFTBUTTON)
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, //���ȷ��
        MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK);

    if (MBRecordMain_Button & DEFINED_RIGHTTBUTTON)
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, // �Ҽ��˳�
        (WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT);

    SendMessage(hWnd,PWM_SETSCROLLSTATE,
                SCROLLLEFT|SCROLLRIGHT|SCROLLDOWN|SCROLLUP,MASKALL);

    GetScreenUsableWH2(&isw,&ish);

    MenuList = LoadMenuList(&MenuListElement,FALSE);

    if (MenuList == NULL)//����listbox�ؼ�
    {
        hMenuList = CreateWindow(pListClassName,"",
            WS_VISIBLE | WS_CHILD | LBS_BITMAP | WS_VSCROLL,
            0,0,isw,ish,hWnd,(HMENU)IDC_MENULIST,NULL,NULL);
    }
    else
    {
        hMenuList = CreateWindow(pMenuClassName, "", //��ʾ�龰ģʽ���������б���б��
            WS_CHILD | WS_VISIBLE | WS_VSCROLL,0, 0 , isw, ish,
            hWnd, (HMENU)IDC_MENULIST, NULL, (PVOID)MenuList);
    }
    if (hMenuList == NULL)
        return FALSE;
    MenuList_AddString(hMenuList,listtxt,idList,(char**)pMBRecordListPic);
    return TRUE;
}
