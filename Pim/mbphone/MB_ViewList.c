/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : ��ӵ绰����
 *
 * Purpose  : ��ʾ�б�
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

#define     IDC_BUTTON_QUIT         3//�˳���ť
#define     IDC_BUTTON_SET          4//ȷ����ť
#define     IDC_MENULIST            6

#define     IDC_MENUITEM0           7   //���±�
#define     IDC_MENUITEM1           8   //�ճ�
#define     IDC_MENUITEM2           9   //��ϵ��
#define     IDC_MENUITEM3           10  //����
#define     IDC_MENUITEM4           11  //����
#define     IDC_MENUITEM5           12  //�ʼ�

#define     TITLECAPTION            MBGetString(STR_MBPHONE_VIEW)
#define     ERROR1                  MBGetString(STR_MBRECORD_ERROR1)    //������ʾ
#define     LISTVIEW0               MBGetString(STR_MBPHONE_VIEW_LIST0)//"���±�"
#define     LISTVIEW1               MBGetString(STR_MBPHONE_VIEW_LIST1)//"�ճ�"
#define     LISTVIEW2               MBGetString(STR_MBRECORD_LIST_LINKMAN)//"��ϵ��"
#define     LISTVIEW3               MBGetString(STR_MBRECORD_LIST_SMS)//"����"
#define     LISTVIEW4               MBGetString(STR_MBRECORD_LIST_MMS)//"����"
#define     LISTVIEW5               MBGetString(STR_MBPHONE_VIEW_LIST2)//"�ʼ�"

static HINSTANCE    hInstance;
static const char * pClassName = "MBViewWndClass";//��������
static const char * pViewList1 = "S:���±�";
static const char * pViewList2 = "S:�ճ�";
static const char * pViewList3 = "S:��ϵ��";
static const char * pViewList4 = "S:��Ϣ";
static const char * pViewList5 = "S:�ʼ�";
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

    case WM_ACTIVATE://ȥ�������Ϣ�������Ҽ������˳��������˳�ʧ��
        if (WA_INACTIVE == LOWORD(wParam))
            SetFocus(GetDlgItem(hWnd,IDC_MENULIST));//��������ý���

        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;

    case WM_CLOSE://������Ͻǵ��˳���ťʱΪ�����˳�;
        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_BUTTON_QUIT, 0 );   
        break;

    case WM_DESTROY ://֪ͨ���������,�˳����˳�;
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
        case IDC_MENUITEM0://���±�
            switch(HIWORD(wParam))
            {
            case ML_SELNOTIFY:
                DlmNotify(PES_APP_CREATE,(int)pViewList1);//"S:���±�"
                DestroyWindow(hWnd);
                break;
            }
            break;
        case IDC_MENUITEM1://�ճ�
            switch(HIWORD(wParam))
            {
            case ML_SELNOTIFY:
                DlmNotify(PES_APP_CREATE,(int)pViewList2);//"S:�ճ�"
                DestroyWindow(hWnd);
                break;
            }
            break;
        case IDC_MENUITEM2://��ϵ��
            switch(HIWORD(wParam))
            {
            case ML_SELNOTIFY:
                DlmNotify(PES_APP_CREATE,(int)pViewList3);//"S:��ϵ��"
                DestroyWindow(hWnd);
                break;
            }
            break;
        case IDC_MENUITEM3://����
            switch(HIWORD(wParam))
            {
            case ML_SELNOTIFY:
                DlmNotify(PES_APP_CREATE,(int)pViewList4);//"S:��Ϣ"
                DestroyWindow(hWnd);
                break;
            }
            break;
        case IDC_MENUITEM4://����
            switch(HIWORD(wParam))
            {
            case ML_SELNOTIFY:
                DlmNotify(PES_APP_CREATE,(int)pViewList4);//"S:��Ϣ"
                DestroyWindow(hWnd);
                break;
            }
            break;
        case IDC_MENUITEM5://�ʼ�
            switch(HIWORD(wParam))
            {
            case ML_SELNOTIFY:
                DlmNotify(PES_APP_CREATE,(int)pViewList5);//"S:�����ʼ�"
                DestroyWindow(hWnd);
                break;
            }
            break;
        case IDC_BUTTON_SET://����
            SendMessage(hWnd,WM_KEYDOWN,VK_F5,0);
            break;
        case IDC_BUTTON_QUIT://�˳�
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
* Purpose     ������ʾ�б���
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
* Purpose    �����ؼ� 
* Params     hwnd�����ڹ��̾��
* Return     
* Remarks     ��������������Ľ���Ԫ�أ�����"ȷ��"��"ȡ��"��ť��Ϊ���ڼ�ͼ�꣬���ָʾ��ͷ. 
**********************************************************************/
static  BOOL    CreateControl(HWND hWnd)
{
    int isw,ish;
    HWND hMenuList;
    char *  listtxt[] =
    {
        "",//���±�
        "",//�ճ�  
        "",//��ϵ��
        "",//����  
        "",//����  
        "",//�ʼ�  
        ""
    };
    int idList[] = 
    {
        IDC_MENUITEM0,//���±�
        IDC_MENUITEM1,//�ճ�  
        IDC_MENUITEM2,//��ϵ��
        IDC_MENUITEM3,//����  
        IDC_MENUITEM4,//����  
        IDC_MENUITEM5,//�ʼ�  
        -1
    };

    listtxt[0] = LISTVIEW0;//���±�
    listtxt[1] = LISTVIEW1;//�ճ�  
    listtxt[2] = LISTVIEW2;//��ϵ��
    listtxt[3] = LISTVIEW3;//����  
    listtxt[4] = LISTVIEW4;//����  
    listtxt[5] = LISTVIEW5;//�ʼ�  

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
