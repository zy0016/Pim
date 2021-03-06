/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 通话记录部分
 *
 * Purpose  : 显示已拨已接未接电话列表的功能
 *            
\**************************************************************************/
#include    "MBRecordView.h"
#include    "MBRecordList.h"
#include    "../mbphone/str_mb_plx.h"
#include    "setting.h"
#include    "hp_pdaex.h"
#include    "RcCommon.h"
#include    "phonebookext.h"
#include    "hpimage.h"

#define     IDC_BUTTON_RIGHT        3//查看按钮
#define     IDC_BUTTON_SET          4//确定按钮
#define     IDC_LIST                5

#define     TITLECAPTION            MBGetString(STR_MBRECORD_RECORD_CAPTION)
#define     TITLECAPTION1           MBGetString(STR_MBRECORD_DEL_UNANSWER)
#define     TITLECAPTION2           MBGetString(STR_MBRECORD_DEL_ANSWER)
#define     TITLECAPTION3           MBGetString(STR_MBRECORD_DEL_DIAL)

#define     BUTTONOK                (LPARAM)MBGetString(STR_WINDOW_OK)  //确定
#define     BUTTONRIGHT             (LPARAM)MBGetString(STR_WINDOW_BACK)//返回
#define     ERROR1                  MBGetString(STR_MBRECORD_ERROR1)    //出错提示

#define     IDS_MBRECORD_VIEW       MBGetString(STR_MBPHONE_VIEW)           //查看
#define     IDS_MBRECORD_SHIKE      MBGetString(STR_MBRECORD_SHIKE)         //"时间"
#define     IDS_MBRECORD_NUM        MBGetString(STR_MBRECORD_NUM)           //"号码"
#define     IDS_MBRECORD_NAME       MBGetString(STR_MBRECORD_NAME)          //"姓名"
#define     IDS_MBRECORD_UNKNOWN    MBGetString(STR_MBRECORD_UNKNOWN)       //"未知"
#define     IDS_MBRECORD_DURATION   MBGetString(STR_MBRECORD_DURATIONVIEW)  //"计时"

#define     MBRECORDDELETEONE       MBGetString(STR_MBRECORD_LIST_DELONE)   //是否删除
#define     MBRECORDDELETEALL       MBGetString(STR_MBRECORD_LIST_DELALL)   //是否删除
#define     MBRECORDLISTDELFAIL     MBGetString(STR_MBRECORD_DEL_FAIL)      //删除失败

#define     MBRECORDMENUVIEW        MBGetString(STR_MBRECORD_LIST_VIEW)     //"查看"
#define     MBRECORDMENUSORTTIME    MBGetString(STR_MBRECORD_LIST_TIME)     //"时间序"
#define     MBRECORDMENUSORTNAME    MBGetString(STR_MBRECORD_LIST_NAME)     //"人名序"
#define     MBRECORDMENUPICKUP      MBGetString(STR_MBRECORD_LIST_PICKUP)   //"提取到"
#define     MBRECORDMENUPHONE       MBGetString(STR_MBRECORD_LIST_PHONE)    //"电话"
#define     MBRECORDMENULINKMAN     MBGetString(STR_MBRECORD_LIST_LINKMAN)  //"联系人"
#define     MBRECORDMENUSMS         MBGetString(STR_MBRECORD_LIST_SMS)      //"短信"
#define     MBRECORDMENUMMS         MBGetString(STR_MBRECORD_LIST_MMS)      //"彩信"

#define     MBRECORDMENUDELETE      MBGetString(STR_MBRECORD_LIST_DELETE)   //删除
#define     MBRECORDMENUDELETEONE   MBGetString(STR_MBRECORD_LIST_DELETEONE)//单条
#define     MBRECORDMENUDELETEALL   MBGetString(STR_MBRECORD_LIST_DELETEALL)//全部
#define     NODATATEXT              MBGetString(STR_MBRECORD_NODATATEXT)    //"无号码记录"

#define     TITLECAPTIONMAXLEN      40//标题最大长度
#define     IDC_REFRESHRECORDLIST   WM_USER + 21//刷新列表框的消息

//#define GETINFOFROMPHONEBOOK_QUICK
//使用这个宏表示在从联系人中查询号码的时候,首先在当前链表中进行查询,
//找到相同号码的节点之后,直接从该节点中返回姓名及其附加信息.
//这样做可以避免频繁的查询联系人的数据,对于有些特殊情况来说可以节省一些时间.

#define SORT_INMENU//定义这个宏表示菜单中包含"排序"项目

//*********************菜单部分*****************************
#if defined(SORT_INMENU)
#define     MENU_POPUP_PICKUP       2//子菜单"提取到"的位置标志
#define     MENU_POPUP_DELETE       3//子菜单"删除"的位置标志
#else
#define     MENU_POPUP_PICKUP       1//子菜单"提取到"的位置标志
#define     MENU_POPUP_DELETE       2//子菜单"删除"的位置标志
#endif

#define     IDM_VIEW                90//查看
#define     IDM_SORT                95//排序
#define     IDM_DIALRECORD          100//电话
#define     IDM_SAVERECORD          110//联系人
#define     IDM_SENDSMS             120//短信
#define     IDM_SENDMMS             130//彩信

#define     IDM_DELETEONE           140//本条记录
#define     IDM_DELETEALL           150//所有记录
static const MENUITEMTEMPLATE MenuGet[] = //提取到
{
    { MF_STRING, IDM_DIALRECORD ,"", NULL},//电话
    { MF_STRING, IDM_SAVERECORD ,"", NULL},//联系人
    { MF_STRING, IDM_SENDSMS    ,"", NULL},//短信
    { MF_STRING, IDM_SENDMMS    ,"", NULL},//彩信
    { MF_END, 0, NULL, NULL }
};
static const MENUITEMTEMPLATE MenuDelete[] = //删除
{
    { MF_STRING, IDM_DELETEONE  ,"", NULL},//本条记录
    { MF_STRING, IDM_DELETEALL  ,"", NULL},//所有记录
    { MF_END, 0, NULL, NULL }
};

static const MENUITEMTEMPLATE MenuMain[] = 
{
    { MF_STRING,IDM_VIEW  ,"", NULL},//查看
#if defined(SORT_INMENU)
    { MF_STRING,IDM_SORT  ,"", NULL},//排序
#endif
    { MF_STRING|MF_POPUP,0,"",(PMENUITEMTEMPLATE)MenuGet},//提取到
    { MF_STRING|MF_POPUP,1,"",(PMENUITEMTEMPLATE)MenuDelete},//删除
    { MF_END, 0, NULL, NULL }
};
static const MENUTEMPLATE MenuTemplate =
{
    0,
    (PMENUITEMTEMPLATE)MenuMain
};

static const int iMBRecords[] = //通话记录的类型(未接,已接,已拨)
{
    UNANSWER_REC,
    ANSWERED_REC,
    DIALED_REC
};
typedef enum
{
    iSORT_TIME = 1,//按照时间排序
    iSORT_NAME = 2 //按照人名排序
}SORTTYPE;

static const int    iRecordTypeCount = sizeof(iMBRecords) / sizeof(int);//通话记录种类的个数
static const int    iViewMaxLen      = 20;//列表框显示号码的最大长度
static const char * pListBox         = "LISTBOX";
static const char * pClassName       = "MBRecordLogListExt";//窗口类名
static const char * pSetTitle        = "%s(%d/%d)";
static const char * pCHOMIT          = "...";//电话号码显示不下之后显示的省略号
static const char * pIconInName      = MBPIC_RECORDLIST_IN; //呼入图标
static const char * pIconOutName     = MBPIC_RECORDLIST_OUT;//呼出图标
static const char * pIconUnName      = MBPIC_RECORDLIST_UN; //未接图标

typedef struct tagMBRecordListExt
{
    int                         iMBRecordType;               //通话记录的类型(未接,已接,已拨)
    int                         iMBRecordIndex;              //通话记录在文件中的位置(索引)
    char                        cDisplay[PHONENUMMAXLEN + 1];//保存人名,电话号码
    char                        cNumber[PHONENUMMAXLEN + 1]; //电话号码
    BOOL                        bInPhonebook;                //号码是否在联系人中
    SYSTEMTIME                  Time;                        //接通时间
    struct tagMBRecordListExt * pNext;
}tagMBRecordListExt;
typedef struct tagMBRecordListExt MBRECORDLISTEXT;
static MBRECORDLISTEXT    * pMBRecordListExt;//链表

//当前所有的通话记录保存在这个结构数组中
static HINSTANCE            hInstance;
static DWORD                dRecordListType;//通话记录的类型(未接,已接,已拨)
static HWND                 hWndApp;
static HBITMAP              hIconUnans;     //未接图标
static HBITMAP              hIconIn;        //呼入图标
static HBITMAP              hIconOut;       //呼出图标
static char                 cTitle[TITLECAPTIONMAXLEN + 1] = "";//标题
static WNDPROC              OldListProc;
static BOOL                 bRefreshList;//激活窗口之后是否需要刷新列表
static MBRECORDLIST_STYLE   MBL_Style;//列表框风格

static  LRESULT CALLBACK NewListProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  BOOL    CreateControl(HWND hWnd,HWND * hList);
static  int     GetRecordListCount(void);
static  void    DestroyRecordListExt(void);
static  int     AddNodeMBRecordListExt(MBRECORDTYPE iMBRecordType,int iMBRecordIndex,
                                       BOOL bInPhonebook,const SYSTEMTIME * pTime,
                                       const char * pDisplay,const char * pNumber,
                                       SORTTYPE SortType);
static  BOOL    LoadMBRecordToLink(SORTTYPE SortType);
static  void    ReloadRecordList(HWND hWnd,HWND hList,SORTTYPE SortType);
static  BOOL    GetMBRecordListExtNode(MBRECORDLISTEXT * pNode,int iIndex);
static  void    SetTitle(HWND hWnd,HWND hList);
static  void    InitMBRecorListExt(void);
static  void    MBRecordListOperate(HWND hWnd,HWND hList,int iType);
static  void    SetLIstBoxIcon(HWND hList,int index,MBRECORDTYPE type);
static  BOOL    RefreshMBRecordList_One(HWND hList,const MBRECORDLISTEXT * pcMBRecordListExt);
static  void    RefreshMBRecordList_All(HWND hList);
static  void    SrotMBRecordList(SORTTYPE SortType);
static  void    MBRecordList_Sort(HWND hWnd,HWND hList,SORTTYPE SortType);
static  BOOL    GetIfHaveSameNumber(const char * pNumber);
#if defined(GETINFOFROMPHONEBOOK_QUICK)
static  BOOL    GetSameNumber(const char * pNumber,char * pDisplayName,
                              unsigned int iBufferlen,BOOL * pInAddress);
#endif
//*********************************************************
BOOL    CallMBLogListExtWindow(HWND hwndCall,DWORD dtype,DWORD MBRecordList_Style)
{
    WNDCLASS    wc;
    HMENU       hSubMenu,hMenu;

    if (MBRecordList_Style & MBL_WS_NODATA_SHOWMESSAGEBOX)
    {
        int i,iNumCount = 0;
        for (i = 0;i < iRecordTypeCount;i++)
        {
            if (dtype & iMBRecords[i])
                iNumCount += GetRecorderNumber(iMBRecords[i]);
        }
        if (0 == iNumCount)
        {
            AppMessageBox(NULL,NODATATEXT,TITLECAPTION,WAITTIMEOUT);
            return TRUE;
        }
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

    MBL_Style       = MBRecordList_Style;
    dRecordListType = dtype;
    switch (dRecordListType)
    {
    case UNANSWER_REC:
        strcpy(cTitle,TITLECAPTION1);
        break;
    case ANSWERED_REC:
        strcpy(cTitle,TITLECAPTION2);
        break;
    case DIALED_REC:
        strcpy(cTitle,TITLECAPTION3);
        break;
    case ANSWERED_REC | DIALED_REC://显示已拨,已接电话列表
        strcpy(cTitle,TITLECAPTION);
        break;
    case ANSWERED_REC | UNANSWER_REC://显示已接,未接电话列表
        strcpy(cTitle,TITLECAPTION);
        break;
    case DIALED_REC | UNANSWER_REC://显示已拨,未接电话列表
        strcpy(cTitle,TITLECAPTION);
        break;
    case ANSWERED_REC | DIALED_REC | UNANSWER_REC://显示未接,已拨,已接电话列表
        strcpy(cTitle,TITLECAPTION);
        break;
    default:
        return FALSE;
    }

    hMenu = LoadMenuIndirect(&MenuTemplate);
    ModifyMenu(hMenu,IDM_VIEW,MF_BYCOMMAND|MF_ENABLED,IDM_VIEW,MBRECORDMENUVIEW);               //查看
    ModifyMenu(hMenu,IDM_SORT,MF_BYCOMMAND|MF_ENABLED,IDM_SORT,MBRECORDMENUSORTNAME);           //人名序

    ModifyMenu(hMenu,IDM_DIALRECORD,MF_BYCOMMAND|MF_ENABLED,IDM_DIALRECORD,MBRECORDMENUPHONE);  //电话
    ModifyMenu(hMenu,IDM_SAVERECORD,MF_BYCOMMAND|MF_ENABLED,IDM_SAVERECORD,MBRECORDMENULINKMAN);//联系人
    ModifyMenu(hMenu,IDM_SENDSMS,MF_BYCOMMAND|MF_ENABLED,IDM_SENDSMS,MBRECORDMENUSMS);          //短信
    ModifyMenu(hMenu,IDM_SENDMMS,MF_BYCOMMAND|MF_ENABLED,IDM_SENDMMS,MBRECORDMENUMMS);          //彩信

    ModifyMenu(hMenu,IDM_DELETEONE,MF_BYCOMMAND|MF_ENABLED,IDM_DELETEONE,MBRECORDMENUDELETEONE);//删除一条
    ModifyMenu(hMenu,IDM_DELETEALL,MF_BYCOMMAND|MF_ENABLED,IDM_DELETEALL,MBRECORDMENUDELETEALL);//全部删除

    hSubMenu = GetSubMenu(hMenu,MENU_POPUP_PICKUP);
    ModifyMenu(hMenu,MENU_POPUP_PICKUP,MF_BYPOSITION|MF_POPUP,(DWORD)hSubMenu,MBRECORDMENUPICKUP);//提取到
    hSubMenu = GetSubMenu(hMenu,MENU_POPUP_DELETE);
    ModifyMenu(hMenu,MENU_POPUP_DELETE,MF_BYPOSITION|MF_POPUP,(DWORD)hSubMenu,MBRECORDMENUDELETE);//删除

    hWndApp = CreateWindow(pClassName,cTitle,WS_CAPTION|WS_BORDER|PWS_STATICBAR,
        PLX_WIN_POSITION,hwndCall,(HMENU)hMenu,NULL,NULL);

    if (NULL == hWndApp)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
    ShowWindow(hWndApp, SW_SHOW);
    UpdateWindow(hWndApp);
    return (TRUE);
}
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static  SORTTYPE    SortType;//当前是按照时间排序,还是按照人名排序
    static  BOOL        bInit;//用来标志当前是否是第一次激活本窗口
            HWND        hList;
            int         icur;
            LRESULT     lResult;
            HMENU       hSubMenuGet,hSubMenuDel,hMenu;
            HDC         hdc;

    lResult = (LRESULT)TRUE;
    switch (wMsgCmd)
    {
    case WM_CREATE :
        InitMBRecorListExt();

        if (!CreateControl(hWnd,&hList))
        {
            AppMessageBox(NULL,ERROR1, cTitle, WAITTIMEOUT);
            return -1;
        }
        SortType     = iSORT_TIME;//按照时间排序
        bRefreshList = FALSE;//激活窗口之后不需要刷新列表
        bInit        = FALSE;

        ReloadRecordList(hWnd,hList,SortType);//加载通话记录到列表框中

        OldListProc  = (WNDPROC)GetWindowLong(hList, GWL_WNDPROC);
        SetWindowLong(hList, GWL_WNDPROC, (long)NewListProc);
        break;

    case WM_INITMENU:
        hList       = GetDlgItem(hWnd,IDC_LIST);
        icur        = SendMessage(hList,LB_GETCURSEL,0,0);
        hMenu       = GetMenu(hWnd);
        hSubMenuGet = GetSubMenu(hMenu,MENU_POPUP_PICKUP);
        hSubMenuDel = GetSubMenu(hMenu,MENU_POPUP_DELETE);
        if (-1 == icur)
        {
            EnableMenuItem(hMenu,IDM_VIEW,MF_GRAYED);
            EnableMenuItem(hMenu,IDM_SORT,MF_GRAYED);
            ModifyMenu(hMenu,MENU_POPUP_PICKUP,MF_BYPOSITION|MF_POPUP|MF_GRAYED,(DWORD)hSubMenuGet,MBRECORDMENUPICKUP);
            ModifyMenu(hMenu,MENU_POPUP_DELETE,MF_BYPOSITION|MF_POPUP|MF_GRAYED,(DWORD)hSubMenuDel,MBRECORDMENUDELETE);
        }
        else
        {
            EnableMenuItem(hMenu,IDM_VIEW,MF_ENABLED);
            EnableMenuItem(hMenu,IDM_SORT,MF_ENABLED);
            ModifyMenu(hMenu,MENU_POPUP_PICKUP,MF_BYPOSITION|MF_POPUP|MF_ENABLED,(DWORD)hSubMenuGet,MBRECORDMENUPICKUP);
            ModifyMenu(hMenu,MENU_POPUP_DELETE,MF_BYPOSITION|MF_POPUP|MF_ENABLED,(DWORD)hSubMenuDel,MBRECORDMENUDELETE);
        }
        break;
    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd,IDC_LIST));
        break;

    case WM_ACTIVATE://去掉这个消息，单击右键可以退出，否则退出失败
        if (WA_INACTIVE != LOWORD(wParam))
        {
            if (UNANSWER_REC & dRecordListType)
            {
                ResetnUnanswered();//未接电话个数复位
            }
            if (bRefreshList)//激活窗口之后需要刷新列表
            {
                bRefreshList = FALSE;
                PostMessage(hWnd,IDC_REFRESHRECORDLIST,0,0);
            }
            else
            {
                hList = GetDlgItem(hWnd,IDC_LIST);
                SetFocus(hList);//激活后设置焦点
                if (bInit)
                    RefreshMBRecordList_All(hList);
                else
                    bInit = TRUE;
            }
        }
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;

    case WM_CLOSE://点击右上角的退出按钮时为保存退出;
        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_BUTTON_RIGHT, 0 );   
        break;

    case WM_DESTROY :       //通知程序管理器,此程序退出;
        hWndApp = NULL;
        DestroyRecordListExt();
        UnregisterClass(pClassName,NULL);
        DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd,NULL);
        if (MBL_Style & MBL_WS_NODATA_NOTSHOWLIST)
        {
            RECT rect;
            int iSH;
            iSH = GetScreenUsableWH1(SM_CYSCREEN);
            SetRect(&rect,0,iSH / 2,GetScreenUsableWH1(SM_CXSCREEN),iSH);
            StrAutoNewLine(hdc,NODATATEXT,NULL,&rect,DT_HCENTER|DT_VCENTER);
        }
        EndPaint(hWnd, NULL);
        break;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_F5:
            SendMessage(hWnd,WM_COMMAND,IDM_VIEW,0);
            break;
        case VK_F1://拨号
            SendMessage(hWnd,WM_COMMAND,IDM_DIALRECORD,0);
            break;
        case VK_F10:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_RIGHT,0);
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

    case WM_TODESKTOP:
        DestroyWindow(hWnd);
        return TRUE;

    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        case IDC_LIST:
            switch (HIWORD(wParam))
            {
            case LBN_DBLCLK:
                SendMessage(hWnd,WM_COMMAND,IDM_VIEW,0);
                break;
            case LBN_SELCHANGE:
                SetTitle(hWnd,GetDlgItem(hWnd,IDC_LIST));
                break;
            }
            break;
        case IDM_VIEW://查看
            MBRecordListOperate(hWnd,GetDlgItem(hWnd,IDC_LIST),IDM_VIEW);
            break;
        case IDM_DIALRECORD://电话
            MBRecordListOperate(hWnd,GetDlgItem(hWnd,IDC_LIST),IDM_DIALRECORD);
            break;
        case IDM_SAVERECORD://联系人
            MBRecordListOperate(hWnd,GetDlgItem(hWnd,IDC_LIST),IDM_SAVERECORD);
            break;
        case IDM_SENDSMS://短信
            MBRecordListOperate(hWnd,GetDlgItem(hWnd,IDC_LIST),IDM_SENDSMS);
            break;
        case IDM_SENDMMS://彩信
            MBRecordListOperate(hWnd,GetDlgItem(hWnd,IDC_LIST),IDM_SENDMMS);
            break;
        case IDM_DELETEONE://删除本条记录
            MBRecordListOperate(hWnd,GetDlgItem(hWnd,IDC_LIST),IDM_DELETEONE);
            break;
        case IDM_DELETEALL://删除所有记录
            MBRecordListOperate(hWnd,GetDlgItem(hWnd,IDC_LIST),IDM_DELETEALL);
            break;

        case IDM_SORT://排序
            hMenu = GetMenu(hWnd);
            hList = GetDlgItem(hWnd,IDC_LIST);
            if (iSORT_TIME == SortType)//当前是按照时间排序
            {
                MBRecordList_Sort(hWnd,hList,iSORT_NAME);//需要按照人名排序
                ModifyMenu(hMenu,IDM_SORT,MF_BYCOMMAND|MF_ENABLED,//文字变成时间序
                    IDM_SORT,MBRECORDMENUSORTTIME);

                if (MBL_Style & MBL_WS_MIDDLEBUTTON)//显示中间键
                    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,
                    MAKEWPARAM(IDM_SORT,2),(LPARAM)MBRECORDMENUSORTTIME);

                SortType = iSORT_NAME;
            }
            else//当前是按照人名排序
            {
                MBRecordList_Sort(hWnd,hList,iSORT_TIME);//需要按照时间排序
                ModifyMenu(hMenu,IDM_SORT,MF_BYCOMMAND|MF_ENABLED,
                    IDM_SORT,MBRECORDMENUSORTNAME);//文字变成人名序

                if (MBL_Style & MBL_WS_MIDDLEBUTTON)//显示中间键
                    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,
                    MAKEWPARAM(IDM_SORT,2),(LPARAM)MBRECORDMENUSORTNAME);

                SortType = iSORT_TIME;
            }
            SetFocus(hList);
            break;
        case IDC_BUTTON_RIGHT:
            DestroyWindow(hWnd);
            break;
        }
        break;

    case IDC_REFRESHRECORDLIST://刷新电话列表
        hList = GetDlgItem(hWnd,IDC_LIST);
        ReloadRecordList(hWnd,hList,SortType);//重新加载通话记录到列表框中
        SetFocus(hList);
        break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
/********************************************************************
* Function   MBRecrodList_Refresh  
* Purpose    标志激活窗口的时候需要刷新列表
* Params     
* Return     
* Remarks     
**********************************************************************/
void    MBRecrodList_Refresh(MBRECORDTYPE MBRecord_Type)
{
    if (IsWindow(hWndApp))
    {
        if (dRecordListType & MBRecord_Type)//当前列表类型中存在需要重新加载的通话类型
            bRefreshList = TRUE;
    }
}
/********************************************************************
* Function   MBRecordListOperate  
* Purpose    执行菜单操作
* Params     
* Return     
* Remarks     
**********************************************************************/
static  void    MBRecordListOperate(HWND hWnd,HWND hList,int iType)
{
    int             icur,i;
    MBRECORDLISTEXT MBRecordListExt;
    CallRecord      MBRecord;

    icur = SendMessage(hList,LB_GETCURSEL,0,0);
    if (-1 == icur)
        return;
    if (!GetMBRecordListExtNode(&MBRecordListExt,icur))
        return;
    switch (iType)
    {
    case IDM_VIEW://查看
        {
            MBRECORD_VIEW   MBRecord_View;
            int             iSour;

            MBRecord_View.hWnd          = hWnd;
            MBRecord_View.index         = MBRecordListExt.iMBRecordIndex;
            MBRecord_View.MBRecordType  = MBRecordListExt.iMBRecordType;

            iSour = strlen(IDS_MBRECORD_VIEW);//窗口标题
            strncpy(MBRecord_View.TitleInfo.cTitle,IDS_MBRECORD_VIEW,
                sizeof(MBRecord_View.TitleInfo.cTitle) > iSour ? iSour + 1 : sizeof(MBRecord_View.TitleInfo.cTitle) - 1);

            iSour = strlen(IDS_MBRECORD_NUM);//号码
            strncpy(MBRecord_View.TitleInfo.cNumber,IDS_MBRECORD_NUM,
                sizeof(MBRecord_View.TitleInfo.cNumber) > iSour ? iSour + 1 : sizeof(MBRecord_View.TitleInfo.cNumber) - 1);

            iSour = strlen(IDS_MBRECORD_NAME);//姓名
            strncpy(MBRecord_View.TitleInfo.cName,IDS_MBRECORD_NAME,
                sizeof(MBRecord_View.TitleInfo.cName) > iSour ? iSour + 1 : sizeof(MBRecord_View.TitleInfo.cName) - 1);

            iSour = strlen(IDS_MBRECORD_SHIKE);//时间
            strncpy(MBRecord_View.TitleInfo.cTime,IDS_MBRECORD_SHIKE,
                sizeof(MBRecord_View.TitleInfo.cTime) > iSour ? iSour + 1 : sizeof(MBRecord_View.TitleInfo.cTime) - 1);

            iSour = strlen(IDS_MBRECORD_DURATION);//计时
            strncpy(MBRecord_View.TitleInfo.cDuration,IDS_MBRECORD_DURATION,
                sizeof(MBRecord_View.TitleInfo.cDuration) > iSour ? iSour + 1 : sizeof(MBRecord_View.TitleInfo.cDuration) - 1);

            MBRecord_View.ItemStyle = MBV_WS_NUMBER | MBV_WS_NAME | MBV_WS_TIME | MBV_WS_DURATION;
            MBRecord_View.TimeStyle = TS_NORMAL;

            iSour = strlen(IDS_MBRECORD_UNKNOWN);
            strncpy(MBRecord_View.cUnknow,IDS_MBRECORD_UNKNOWN,
                sizeof(MBRecord_View.cUnknow) > iSour ? iSour + 1 : sizeof(MBRecord_View.cUnknow) - 1);

            MBRecord_View.MBRecordView_Style = MBV_WS_DEFAULT;
            bRefreshList = FALSE;
            /*由于CallMBLogViewWindow窗口销毁之后,本窗口的WM_ACTIVATE消息会首先到来,
            然后CallMBLogViewWindow函数才会有返回值回来,为了避免在WM_ACTIVATE消息中
            发送IDC_REFRESHRECORDLIST消息进行刷新,所以在此将bRefreshList赋值为FALSE.
            保证在WM_ACTIVATE消息中不会进行刷新.
            本电话已经修改了姓名,由于在CallMBLogViewWindow窗口中删除记录的话,
            会调用MBRecrodList_Refresh函数,所以在窗口激活的时候会自动刷新,不必在
            这里主动发送IDC_REFRESHRECORDLIST消息进行刷新*/
            if (MBRECORD_RS_MODIFY == CallMBLogViewWindow(&MBRecord_View))
                RefreshMBRecordList_One(hList,&MBRecordListExt);
        }
        break;

    case IDM_DIALRECORD://电话
        if (-1 != GetMBRecord(&MBRecord,sizeof(CallRecord),
            MBRecordListExt.iMBRecordType,MBRecordListExt.iMBRecordIndex))
            APP_CallPhoneNumber(MBRecord.PhoneNo);//当前电话已经拨出
        break;

    case IDM_SAVERECORD://联系人
        if (-1 != GetMBRecord(&MBRecord,sizeof(CallRecord),
            MBRecordListExt.iMBRecordType,MBRecordListExt.iMBRecordIndex))
        {
            if (MB_SaveToPhoneBook(hWnd,MBRecord.PhoneNo))//本电话已经修改了姓名
                RefreshMBRecordList_One(hList,&MBRecordListExt);
        }
        break;

    case IDM_SENDSMS://短信
        if (-1 != GetMBRecord(&MBRecord,sizeof(CallRecord),
            MBRecordListExt.iMBRecordType,MBRecordListExt.iMBRecordIndex))
            MB_SendSms(MBRecord.PhoneNo);
        break;

    case IDM_SENDMMS://彩信
        if (-1 != GetMBRecord(&MBRecord,sizeof(CallRecord),
            MBRecordListExt.iMBRecordType,MBRecordListExt.iMBRecordIndex))
            MB_SendMms(MBRecord.PhoneNo);
        break;

    case IDM_DELETEONE://删除本条记录
        bRefreshList = FALSE;
        /*由于询问窗口销毁之后,本窗口的WM_ACTIVATE消息会首先到来,
        然后询问函数才会有返回值回来,为了避免在WM_ACTIVATE消息中
        发送IDC_REFRESHRECORDLIST消息进行刷新,所以在此将bRefreshList赋值为FALSE.
        保证在WM_ACTIVATE消息中不会进行刷新*/
        if (AppConfirmWin(hWnd,MBRECORDDELETEONE,"",cTitle,
            CONFIRM_OK,CONFIRM_CANCEL))
        {
            if (MBL_Style & MBL_WS_NOSHOWREPEATDATA)//列表中不显示号码重复的数据(但是文件中保存有号码重复的记录)
            {
                if (DeleteSameNumber(MBRecordListExt.iMBRecordType,MBRecordListExt.cNumber))
                    PostMessage(hWnd,IDC_REFRESHRECORDLIST,1,0);
                else
                    AppMessageBox(hWnd,MBRECORDLISTDELFAIL,cTitle, WAITTIMEOUT);
            }
            else
            {
                if (DeleteMBRecord(MBRecordListExt.iMBRecordType,MBRecordListExt.iMBRecordIndex))
                    PostMessage(hWnd,IDC_REFRESHRECORDLIST,1,0);
                else
                    AppMessageBox(hWnd,MBRECORDLISTDELFAIL,cTitle, WAITTIMEOUT);
            }
        }
        break;
    case IDM_DELETEALL://删除所有记录
        bRefreshList = FALSE;
        /*由于询问窗口销毁之后,本窗口的WM_ACTIVATE消息会首先到来,
        然后询问函数才会有返回值回来,为了避免在WM_ACTIVATE消息中
        发送IDC_REFRESHRECORDLIST消息进行刷新,所以在此将bRefreshList赋值为FALSE.
        保证在WM_ACTIVATE消息中不会进行刷新*/
        if (AppConfirmWin(hWnd,MBRECORDDELETEALL,"",cTitle,CONFIRM_OK,CONFIRM_CANCEL))
        {
            for (i = 0;i < iRecordTypeCount;i++)
            {
                if (dRecordListType & iMBRecords[i])
                {
                    if (!DeleteMBRecord(iMBRecords[i],-1))
                    {
                        AppMessageBox(hWnd,MBRECORDLISTDELFAIL,cTitle, WAITTIMEOUT);
                        return;
                    }
                }
            }
            PostMessage(hWnd,IDC_REFRESHRECORDLIST,1,0);
        }
        break;
    }
}
/********************************************************************
* Function   InitMBRecorListExt  
* Purpose    初始化图片部分
* Params     
* Return     
* Remarks     
**********************************************************************/
static  void    InitMBRecorListExt(void)
{
    static BOOL bInit = FALSE;
    SIZE        size;
    COLORREF    Color;
    BOOL        bGif;
    HDC         hdc;

    if (bInit)
        return;

    GetImageDimensionFromFile(pIconInName,&size);
    hdc         = CreateMemoryDC(size.cx,size.cy);
    hIconIn     = CreateBitmapFromImageFile(hdc,pIconInName ,&Color,&bGif);
    hIconOut    = CreateBitmapFromImageFile(hdc,pIconOutName,&Color,&bGif);
    hIconUnans  = CreateBitmapFromImageFile(hdc,pIconUnName ,&Color,&bGif);
    DeleteObject(hdc);

    bInit = TRUE;
}
/********************************************************************
* Function   SetTitle  
* Purpose    设置标题
* Params     
* Return     
* Remarks     
**********************************************************************/
static  void    SetTitle(HWND hWnd,HWND hList)
{
    if (MBL_Style & MBL_WS_COUNT)
    {
        int icur,iCount;
        char title[TITLECAPTIONMAXLEN + 1] = "";

        icur   = SendMessage(hList,LB_GETCURSEL,0,0);
        iCount = SendMessage(hList,LB_GETCOUNT ,0,0);

        sprintf(title,pSetTitle,cTitle,icur + 1,iCount);
        SetWindowText(hWnd,title);
    }
}
/********************************************************************
* Function   GetIfHaveSameNumber  
* Purpose    当前号码是否已经存在于链表之中
* Params     pNumber:需要考察的电话号码
* Return     找到重复的号码返回TRUE,否则返回FALSE;
* Remarks     
**********************************************************************/
static  BOOL    GetIfHaveSameNumber(const char * pNumber)
{
    MBRECORDLISTEXT * p = pMBRecordListExt;

    if (NULL == pMBRecordListExt)
        return FALSE;

    while (p != NULL)
    {
        if (strcmp(p->cNumber,pNumber) == 0)
            return TRUE;
        p = p->pNext;
    }
    return FALSE;
}
#if defined(GETINFOFROMPHONEBOOK_QUICK)
/********************************************************************
* Function   GetIfHaveSameNumber  
* Purpose    当前号码是否已经存在于链表之中,是的话,返回姓名
* Params     pNumber:需要考察的电话号码,pDisplayName:用于返回姓名的buff,iBufferlen:buff长度
* Return     找到重复的号码返回TRUE,否则返回FALSE;
* Remarks     
**********************************************************************/
static  BOOL    GetSameNumber(const char * pNumber,char * pDisplayName,
                              unsigned int iBufferlen,BOOL * pInAddress)
{
    MBRECORDLISTEXT * p = pMBRecordListExt;

    if (NULL == pMBRecordListExt)
        return FALSE;

    while (p != NULL)
    {
        if (strcmp(p->cNumber,pNumber) == 0)
        {
            if (pDisplayName != NULL)
            {
                memset(pDisplayName,0x00,iBufferlen);
                strncpy(pDisplayName,p->cDisplay,iBufferlen - 1);
            }
            if (pInAddress != NULL)
                * pInAddress = p->bInPhonebook;

            return TRUE;
        }
        p = p->pNext;
    }
    return FALSE;
}
#endif
/********************************************************************
* Function   GetMBRecordListExtNode  
* Purpose    获得相应位置的节点的信息
* Params     pNode:返回相应位置的节点的信息,iIndex:节点在链表中的位置
* Return     找到这个节点返回TRUE,否则返回FALSE;
* Remarks     
**********************************************************************/
static  BOOL    GetMBRecordListExtNode(MBRECORDLISTEXT * pNode,int iIndex)
{
    int                 i = 0,iCount;
    MBRECORDLISTEXT   * p = pMBRecordListExt;

    if (NULL == pMBRecordListExt)
        return FALSE;
    iCount = GetRecordListCount();

    if ((0 == iCount) || (iIndex < 0) || (iIndex > iCount - 1))//链表为空或者节点位置非法
        return FALSE;

    while (p != NULL)
    {
        if (i < iIndex)
        {
            p = p->pNext;
            i++;
        }
        else
        {
            memcpy(pNode,p,sizeof(MBRECORDLISTEXT));//找到这个节点,返回数据
            return TRUE;
        }
    }
    return FALSE;
}
/********************************************************************
* Function   ReloadRecordList  
* Purpose    加载通话记录到列表框中
* Params     iSortType:排序类别
* Return     
* Remarks     
**********************************************************************/
static  void    ReloadRecordList(HWND hWnd,HWND hList,SORTTYPE SortType)
{
    int                 index,iOldSel;
    char                cDisplayData[PHONENUMMAXLEN + 1] = "";
    MBRECORDLISTEXT   * p;

    iOldSel = SendMessage(hList,LB_GETCURSEL,0,0);
    LoadMBRecordToLink(SortType);//加载通话记录到链表中
    SendMessage(hList,LB_RESETCONTENT,0,0);

    p = pMBRecordListExt;
    while (p != NULL)
    {
        if (p->bInPhonebook)//该电话不是陌生的
        {
            strcpy(cDisplayData,p->cDisplay);
        }
        else//该电话是陌生的
        {
            if ((int)strlen(p->cDisplay) > iViewMaxLen)
            {
                strncpy(cDisplayData,p->cDisplay,iViewMaxLen);
                strcat(cDisplayData,pCHOMIT);//"..."添加省略号
            }
            else
            {
                strcpy(cDisplayData,p->cDisplay);
            }
        }
        index = SendMessage(hList,LB_ADDSTRING,0,(LPARAM)cDisplayData);
        SetLIstBoxIcon(hList,index,p->iMBRecordType);
        p = p->pNext;
    }

    SendMessage(hList,LB_SETCURSEL,(iOldSel >= 0) ? iOldSel : 0,0);

    if (MBL_Style & MBL_WS_MIDDLEBUTTON)//显示中间键
    {
        if (SendMessage(hList,LB_GETCOUNT,0,0) == 0)//没有记录的时候不显示中间的按钮
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDM_SORT,2),(LPARAM)"");
        else
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDM_SORT,2),(LPARAM)MBRECORDMENUSORTNAME);
    }

    SetTitle(hWnd,hList);//设置标题
    if (MBL_Style & MBL_WS_NODATA_NOTSHOWLIST)//没有数据的时候不显示列表框,只在界面上显示"无号码记录"
    {
        ShowWindow(hList,SendMessage(hList,LB_GETCOUNT,0,0) == 0 ? SW_HIDE : SW_SHOW);
    }
}
/********************************************************************
* Function   LoadMBRecordToLink  
* Purpose    加载通话记录到链表中 
* Params     iSortType:排序类别
* Return     
* Remarks      
**********************************************************************/
static  BOOL    LoadMBRecordToLink(SORTTYPE SortType)
{
    int         i,j,index,iRecordCount;
    CallRecord  MBRecords;//保存通话数据
    PBINFO      pbNameIcon;
    char        cDisplayData[PHONENUMMAXLEN + 1] = "";
    BOOL        bInAddress;

    DestroyRecordListExt();//销毁所有节点
    for (j = 0;j < iRecordTypeCount;j++)
    {
        if (dRecordListType & iMBRecords[j])
        {
            iRecordCount = GetRecorderNumber(iMBRecords[j]);//获得已接,已拨,未接记录个数

            for (i = 0;i < iRecordCount;i++)
            {
                memset(&MBRecords,0x00,sizeof(MBRecords));
                //返回一个通话信息
                if (-1 == GetMBRecord(&MBRecords,sizeof(CallRecord),iMBRecords[j],i))
                    continue;

                if (strlen(MBRecords.PhoneNo) == 0)
                    break;

                if (MBL_Style & MBL_WS_NOSHOWREPEATDATA)//列表中不显示号码重复的数据(但是文件中保存有号码重复的记录)
                {
                    if (GetIfHaveSameNumber(MBRecords.PhoneNo))//存在重复号码
                        continue;
                }
                memset(&pbNameIcon,0x00,sizeof(PBINFO));
#if defined(GETINFOFROMPHONEBOOK_QUICK)
                if (!GetSameNumber(MBRecords.PhoneNo,cDisplayData,
                    sizeof(cDisplayData),&bInAddress))
#endif
                {
                if (MB_GetInfoFromPhonebook(MBRecords.PhoneNo,&pbNameIcon))//该电话不是陌生的
                {
                    strcpy(cDisplayData,pbNameIcon.Name);//记录姓名
                    bInAddress = TRUE;
                }
                else
                {
                    strcpy(cDisplayData,MBRecords.PhoneNo);//记录号码,用于排序
                    bInAddress = FALSE;
                }
                }
                index = AddNodeMBRecordListExt(iMBRecords[j],i,bInAddress,//添加节点
                                                &MBRecords.Time,cDisplayData,
                                                MBRecords.PhoneNo,SortType);
            }
        }
    }
    return TRUE;
}
/********************************************************************
* Function     AddNodeMBRecordListExt
* Purpose      将节点加入到链表相应位置
* Params       iMBRecordType:通话记录的类型(未接,已接,已拨)
                iMBRecordIndex:通话记录在文件中的位置
                bInPhonebook:号码是否在联系人中
                pTime:接通时间
                pDisplay:保存人名,电话号码
                iSortType:排序类别
* Return        节点的添加位置,-1表示添加失败
* Remarks      
**********************************************************************/
static  int     AddNodeMBRecordListExt(MBRECORDTYPE iMBRecordType,int iMBRecordIndex,
                                       BOOL bInPhonebook,const SYSTEMTIME * pTime,
                                       const char * pDisplay,const char * pNumber,
                                       SORTTYPE SortType)
{
    MBRECORDLISTEXT * p = NULL,* pCur = NULL,* pCurPre = NULL;
    int iSour,iCount = 0,index;

    if ((pDisplay == NULL) || (pTime == NULL))
        return -1;

    p = (MBRECORDLISTEXT *)malloc(sizeof(MBRECORDLISTEXT));
    if (p == NULL)
        return -1;

    memset(p,0x00,sizeof(MBRECORDLISTEXT));

    iSour = strlen(pDisplay);
    strncpy(p->cDisplay,pDisplay,sizeof(p->cDisplay) > iSour ? iSour + 1: sizeof(p->cDisplay) - 1);

    iSour = strlen(pNumber);
    strncpy(p->cNumber,pNumber,sizeof(p->cNumber) > iSour ? iSour + 1 : sizeof(p->cNumber) - 1);

    p->iMBRecordType    = iMBRecordType;        //通话记录的类型(未接,已接,已拨)
    p->iMBRecordIndex   = iMBRecordIndex;       //通话记录在文件中的位置
    p->bInPhonebook     = bInPhonebook;         //号码是否在联系人中
    memcpy(&p->Time,pTime,sizeof(SYSTEMTIME));  //接通时间

    iCount = GetRecordListCount();
    index  = 0;//当前指针指向的节点的位置
    if (iCount == 0)//链表为空,不用排序直接加入头节点
    {
        pMBRecordListExt = p;
        p->pNext         = NULL;
        return (index);
    }
    if (iCount == 1)//链表中只有一个节点
    {
        if (iSORT_TIME == SortType)//按照时间排序
        {
            if (TimeCmp(&p->Time,&pMBRecordListExt->Time) < 0)
            {
                //需要添加的节点时间小于当前节点时间,将节点插入到链表尾
                pMBRecordListExt->pNext = p;
                p->pNext                = NULL;
                index++;
            }
            else//需要添加的节点时间大于当前节点时间,将节点插入到链表头
            {
                p->pNext         = pMBRecordListExt;
                pMBRecordListExt = p;
            }
        }
        else if (iSORT_NAME == SortType)//按照人名排序
        {
            if (strcmp(p->cDisplay,pMBRecordListExt->cDisplay) > 0)
            {
                //需要添加的节点姓名大于当前节点姓名,将节点插入到链表尾
                pMBRecordListExt->pNext = p;
                p->pNext                = NULL;
                index++;
            }
            else//需要添加的节点姓名小于当前节点姓名,将节点插入到链表头
            {
                p->pNext         = pMBRecordListExt;
                pMBRecordListExt = p;
            }
        }
        return (index);
    }
    //链表节点个数大于1个
    pCur    = pMBRecordListExt;//指向当前节点
    pCurPre = NULL;//指向当前节点的前一个节点
    while (pCur != NULL)
    {
        if (iSORT_TIME == SortType)//按照时间排序
        {
            if (TimeCmp(&p->Time,&pCur->Time) < 0)
            {
                //需要添加的节点时间小于当前节点时间,向后遍历链表
                pCurPre = pCur;
                pCur    = pCur->pNext;
                index++;
            }
            else//需要添加的节点时间大于当前节点时间,将节点插入到pCur的前边
            {
                if (NULL == pCurPre)//当前节点的前一个节点为空,则插入到头节点
                {
                    p->pNext         = pMBRecordListExt;
                    pMBRecordListExt = p;
                }
                else//当前节点的前一个节点非空,插入到pCurPre节点之后
                {
                    p->pNext        = pCur;
                    pCurPre->pNext  = p;
                }
                return (index);
            }
        }
        else if (iSORT_NAME == SortType)//按照人名排序
        {
            if (strcmp(p->cDisplay,pCur->cDisplay) > 0)
            {
                //需要添加的节点姓名大于于当前节点姓名,向后遍历链表
                pCurPre = pCur;
                pCur    = pCur->pNext;
                index++;
            }
            else//需要添加的节点姓名小于当前节点姓名,将节点插入到pCur的前边
            {
                if (NULL == pCurPre)//当前节点的前一个节点为空,则插入到头节点
                {
                    p->pNext         = pMBRecordListExt;
                    pMBRecordListExt = p;
                }
                else//当前节点的前一个节点非空,插入到pCurPre节点之后
                {
                    p->pNext        = pCur;
                    pCurPre->pNext  = p;
                }
                return (index);
            }
        }
    }
    //这时pCurPre指向链表结尾,需要将节点插入链表结尾
    pCurPre->pNext = p;
    p->pNext       = NULL;

    return (index);
}
/********************************************************************
* Function     DestroyRecordListExt
* Purpose      删除所有节点
* Params       
* Return           
* Remarks      
**********************************************************************/
static  void    DestroyRecordListExt(void)
{
    MBRECORDLISTEXT * p = NULL;

    while (pMBRecordListExt != NULL)
    {
        p                = pMBRecordListExt;
        pMBRecordListExt = pMBRecordListExt->pNext;
        free(p);
    }
}
/********************************************************************
* Function     MBList_GetCount
* Purpose      获得链表节点数
* Params       
* Return           
* Remarks      
**********************************************************************/
static  int     GetRecordListCount(void)
{
    int               iCount = 0;
    MBRECORDLISTEXT * p = pMBRecordListExt;

    while (p != NULL)
    {
        p = p->pNext;
        iCount++;
    }
    return (iCount);
}
/********************************************************************
* Function     MBRecordList_Sort
* Purpose      列表排序
* Params       SortType:排序方式
* Return           
* Remarks      
**********************************************************************/
static  void    MBRecordList_Sort(HWND hWnd,HWND hList,SORTTYPE SortType)
{
    MBRECORDLISTEXT   * p;
    char                cDisplayData[PHONENUMMAXLEN + 1] = "";
    int                 index;

    SrotMBRecordList(SortType);//链表排序
    SendMessage(hList,LB_RESETCONTENT,0,0);

    p = pMBRecordListExt;
    while (p != NULL)//重新添加列表项目
    {
        if (p->bInPhonebook)//该电话不是陌生的
        {
            strcpy(cDisplayData,p->cDisplay);
        }
        else//该电话是陌生的
        {
            if ((int)strlen(p->cDisplay) > iViewMaxLen)
            {
                strncpy(cDisplayData,p->cDisplay,iViewMaxLen);
                strcat(cDisplayData,pCHOMIT);//"..."添加省略号
            }
            else
            {
                strcpy(cDisplayData,p->cDisplay);
            }
        }
        index = SendMessage(hList,LB_ADDSTRING,0,(LPARAM)cDisplayData);
        SetLIstBoxIcon(hList,index,p->iMBRecordType);
        p = p->pNext;
    }
    SendMessage(hList,LB_SETCURSEL,0,0);
    SetTitle(hWnd,hList);//设置标题
}
/********************************************************************
* Function     iGetNewMBRecrodListCount
* Purpose      获得新链表的个数
* Params       
* Return           
* Remarks      
**********************************************************************/
static  int     iGetNewMBRecrodListCount(MBRECORDLISTEXT * pNew)
{
    int                 iCount = 0;
    MBRECORDLISTEXT   * p = pNew;
    while (p != NULL)
    {
        p = p->pNext;
        iCount++;
    }
    return (iCount);
}
/********************************************************************
* Function     SrotMBRecordList
* Purpose      链表排序
* Params       
* Return           
* Remarks      
**********************************************************************/
static  void    SrotMBRecordList(SORTTYPE SortType)
{
    BOOL              bInsert;//是否已经插入数据
    int               iCount;
    MBRECORDLISTEXT * pNew = NULL;//新链表
    MBRECORDLISTEXT * pOld = pMBRecordListExt;//旧链表
    MBRECORDLISTEXT * pCur = NULL,* pCurPre = NULL,* p = NULL;

    while (pOld != NULL)
    {
        iCount = iGetNewMBRecrodListCount(pNew);
        p      = pOld;//p指向旧链表的当前节点
        pOld   = pOld->pNext;

        if (0 == iCount)//链表为空,不用排序直接加入头节点
        {
            pNew = p;
            pNew->pNext = NULL;
            continue;
        }
        if (1 == iCount)
        {
            switch (SortType)
            {
            case iSORT_TIME:
                if (TimeCmp(&p->Time,&pNew->Time) < 0)
                {   //需要添加的节点时间小于当前节点时间,将节点插入到链表尾
                    pNew->pNext = p;
                    p->pNext = NULL;
                }
                else//需要添加的节点时间大于当前节点时间,将节点插入到链表头
                {
                    p->pNext = pNew;
                    pNew     = p;
                }
                break;
            case iSORT_NAME:
                if (strcmp(p->cDisplay,pNew->cDisplay) > 0)
                {   //需要添加的节点姓名大于当前节点姓名,将节点插入到链表尾
                    pNew->pNext = p;
                    p->pNext = NULL;
                }
                else//需要添加的节点姓名小于当前节点姓名,将节点插入到链表头
                {
                    p->pNext = pNew;
                    pNew     = p;
                }
                break;
            }
            continue;
        }
        //链表节点个数大于1个
        bInsert = FALSE;
        pCur    = pNew;//指向当前节点
        pCurPre = NULL;//指向当前节点的前一个节点
        while (pCur != NULL)
        {
            if (iSORT_TIME == SortType)
            {
                if (TimeCmp(&p->Time,&pCur->Time) < 0)
                {
                    //需要添加的节点时间小于当前节点时间,向后遍历链表
                    pCurPre = pCur;
                    pCur    = pCur->pNext;
                    continue;
                }
                else//需要添加的节点时间大于当前节点时间,将节点插入到pCur的前边
                {
                    if (NULL == pCurPre)//当前节点的前一个节点为空,则插入到头节点
                    {
                        p->pNext = pNew;
                        pNew     = p;
                    }
                    else//当前节点的前一个节点非空,插入到pCurPre节点之后
                    {
                        p->pNext       = pCur;
                        pCurPre->pNext = p;
                    }
                    bInsert = TRUE;
                    break;
                }
            }
            else if (iSORT_NAME == SortType)
            {
                if (strcmp(p->cDisplay,pCur->cDisplay) > 0)
                {
                    //需要添加的节点姓名大于于当前节点姓名,向后遍历链表
                    pCurPre = pCur;
                    pCur    = pCur->pNext;
                    continue;
                }
                else//需要添加的节点姓名小于当前节点姓名,将节点插入到pCur的前边
                {
                    if (NULL == pCurPre)//当前节点的前一个节点为空,则插入到头节点
                    {
                        p->pNext = pNew;
                        pNew     = p;
                    }
                    else//当前节点的前一个节点非空,插入到pCurPre节点之后
                    {
                        p->pNext       = pCur;
                        pCurPre->pNext = p;
                    }
                    bInsert = TRUE;
                    break;
                }
            }
        }
        if (!bInsert)
        {
            //这时pCurPre指向链表结尾,需要将节点插入链表结尾
            pCurPre->pNext = p;
            p->pNext       = NULL;
        }
    }
    pMBRecordListExt = pNew;
}
/********************************************************************
* Function     RefreshMBRecordList_All
* Purpose      窗口刷新之后,考查联系人部分是否有更新,如果有更新的话,重新刷新列表
* Params       
* Return           
* Remarks      
**********************************************************************/
static  void    RefreshMBRecordList_All(HWND hList)
{
    MBRECORDLISTEXT   * p;
    PBINFO              pbNameIcon;
    int                 iSour,i = 0,index,iOldSel;
    BOOL                bModify;//该节点的数据是否有了修改
    char                cDisplayData[PHONENUMMAXLEN + 1] = "";

    iOldSel = SendMessage(hList,LB_GETCURSEL,0,0);
    p       = pMBRecordListExt;

    while (p != NULL)
    {
        memset(&pbNameIcon,0x00,sizeof(PBINFO));
#if defined(GETINFOFROMPHONEBOOK_QUICK)
        if (GetSameNumber(p->cNumber,cDisplayData,sizeof(cDisplayData),
            &p->bInPhonebook))
        {
            if (strcmp(p->cDisplay,cDisplayData) != 0)
            {
                iSour = strlen(cDisplayData);
                strncpy(p->cDisplay,cDisplayData,
                    sizeof(p->cDisplay) > iSour ? iSour + 1 : sizeof(p->cDisplay) - 1);
                bModify = TRUE;
            }
            else
            {
                bModify = FALSE;
            }
        }
        else
#endif
        {
        if (MB_GetInfoFromPhonebook(p->cNumber,&pbNameIcon))//该电话不是陌生的
        {
            if (p->bInPhonebook)//该电话原来就不是陌生的
            {
                if (strcmp(p->cDisplay,pbNameIcon.Name) != 0)//姓名有所改变
                {
                    iSour = strlen(pbNameIcon.Name);
                    strncpy(p->cDisplay,pbNameIcon.Name,
                        sizeof(p->cDisplay) > iSour ? iSour + 1 : sizeof(p->cDisplay) - 1);
                    bModify = TRUE;

                    strcpy(cDisplayData,p->cDisplay);
                }
                else//姓名没有改变
                {
                    bModify = FALSE;
                }
            }
            else//该电话原来是陌生的
            {
                iSour = strlen(pbNameIcon.Name);
                strncpy(p->cDisplay,pbNameIcon.Name,
                    sizeof(p->cDisplay) > iSour ? iSour + 1 : sizeof(p->cDisplay) - 1);
                p->bInPhonebook = TRUE;
                bModify = TRUE;

                strcpy(cDisplayData,p->cDisplay);
            }
        }
        else//该电话是陌生的
        {
            if (p->bInPhonebook)//该电话原来不是陌生的
            {
                iSour = strlen(p->cNumber);
                strncpy(p->cDisplay,p->cNumber,
                    sizeof(p->cDisplay) > iSour ? iSour + 1 : sizeof(p->cDisplay) - 1);
                p->bInPhonebook = FALSE;
                bModify = TRUE;

                if ((int)strlen(p->cDisplay) > iViewMaxLen)
                {
                    strncpy(cDisplayData,p->cDisplay,iViewMaxLen);
                    strcat(cDisplayData,pCHOMIT);//"..."添加省略号
                }
                else
                {
                    strcpy(cDisplayData,p->cDisplay);
                }
            }
            else
            {
                bModify = FALSE;
            }
        }
        }
        if (bModify)
        {
            SendMessage(hList,LB_DELETESTRING,i,0);
            index = SendMessage(hList,LB_INSERTSTRING,i,(LPARAM)cDisplayData);
            SetLIstBoxIcon(hList,index,p->iMBRecordType);
        }
        p = p->pNext;
        i++;
    }
    SendMessage(hList,LB_SETCURSEL,(iOldSel >= 0) ? iOldSel : 0,0);
}
/********************************************************************
* Function     RefreshMBRecordList_One
* Purpose      当用户在联系人中修改了某个节点的数据之后,需要更新列表框
* Params       pcMBRecordListExt:需要修改的节点
* Return           
* Remarks      
**********************************************************************/
static  BOOL    RefreshMBRecordList_One(HWND hList,const MBRECORDLISTEXT * pcMBRecordListExt)
{
    MBRECORDLISTEXT   * p = (MBRECORDLISTEXT *)pMBRecordListExt;
    int                 i = 0,iSour,index,iOldSel;
    PBINFO              pbNameIcon;
    char                cDisplayData[PHONENUMMAXLEN + 1] = "";

    iOldSel = SendMessage(hList,LB_GETCURSEL,0,0);
    while (p != NULL)
    {
        if ((pcMBRecordListExt->iMBRecordType == p->iMBRecordType) &&
            strcmp(pcMBRecordListExt->cDisplay,p->cDisplay) == 0)
        {   //在链表中查找显示名称为pcMBRecordListExt->cDisplay的数据
            if (MB_GetInfoFromPhonebook(pcMBRecordListExt->cNumber,&pbNameIcon))
            {
                iSour = strlen(pbNameIcon.Name);
                strncpy(p->cDisplay,pbNameIcon.Name,
                    sizeof(p->cDisplay) > iSour ? iSour + 1 : sizeof(p->cDisplay) - 1);
                p->bInPhonebook = TRUE;

                strcpy(cDisplayData,p->cDisplay);
            }
            else
            {
                iSour = strlen(pcMBRecordListExt->cNumber);
                strncpy(p->cDisplay,pcMBRecordListExt->cNumber,
                    sizeof(p->cDisplay) > iSour ? iSour + 1 : sizeof(p->cDisplay) - 1);
                p->bInPhonebook = FALSE;

                if ((int)strlen(p->cDisplay) > iViewMaxLen)
                {
                    strncpy(cDisplayData,p->cDisplay,iViewMaxLen);
                    strcat(cDisplayData,pCHOMIT);//"..."添加省略号
                }
                else
                {
                    strcpy(cDisplayData,p->cDisplay);
                }
            }
            SendMessage(hList,LB_DELETESTRING,i,0);
            index = SendMessage(hList,LB_INSERTSTRING,i,(LPARAM)cDisplayData);
            SetLIstBoxIcon(hList,index,p->iMBRecordType);
            SendMessage(hList,LB_SETCURSEL,index,0);
        }
        p = p->pNext;
        i++;
    }
    SendMessage(hList,LB_SETCURSEL,(iOldSel >= 0) ? iOldSel : 0,0);
    return TRUE;
}
/********************************************************************
* Function   SetLIstBoxIcon  
* Purpose     设置listbox控件每一个项目的图标
* Params    
* Return     
* Remarks      
**********************************************************************/
static  void    SetLIstBoxIcon(HWND hList,int index,MBRECORDTYPE type)
{
    DWORD dWord = MAKEWPARAM((WORD)IMAGE_BITMAP, (WORD)index);
    switch (type)
    {
    case UNANSWER_REC:
        SendMessage(hList,LB_SETIMAGE,(WPARAM)dWord,(LPARAM)hIconUnans);
        break;
    case ANSWERED_REC:
        SendMessage(hList,LB_SETIMAGE,(WPARAM)dWord,(LPARAM)hIconIn);
        break;
    case DIALED_REC:
        SendMessage(hList,LB_SETIMAGE,(WPARAM)dWord,(LPARAM)hIconOut);
        break;
    }
}
/********************************************************************
* Function   CreateControl  
* Purpose    创建控件 
* Params     hwnd：窗口过程句柄
* Return     
* Remarks      
**********************************************************************/
static  BOOL    CreateControl(HWND hWnd,HWND * hList)
{
    int     iscreenw,iscreenh;
    DWORD   dStyle;

    SendMessage(hWnd,PWM_SETSCROLLSTATE,SCROLLLEFT|SCROLLRIGHT|SCROLLDOWN|SCROLLUP,MASKALL);
    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_RIGHT,BUTTONRIGHT);
    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK);

    if (MBL_Style & MBL_WS_MIDDLEBUTTON)//显示中间键
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDM_SORT,2),
        (LPARAM)MBRECORDMENUSORTNAME);

    GetScreenUsableWH2(&iscreenw,&iscreenh);

    if (MBL_Style & MBL_WS_NOVSCROLL)//不显示滚动条
        dStyle = WS_VISIBLE | WS_CHILD | WS_TABSTOP | LBS_BITMAP;
    else
        dStyle = WS_VISIBLE | WS_CHILD | WS_TABSTOP | LBS_BITMAP | WS_VSCROLL;

    * hList = CreateWindow(pListBox,"",dStyle,
        0,0,iscreenw,iscreenh,hWnd,(HMENU)IDC_LIST,NULL,NULL);

    return (* hList != NULL);
}
static  LRESULT CALLBACK NewListProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;
    switch(message)
    {
    case WM_KEYUP:
        switch (LOWORD(wParam))
        {
        case VK_EARPHONE_KEY://耳机上的按键
            SendMessage(hWndApp,WM_COMMAND,IDM_DIALRECORD,0);
            break;
        default:
            lResult = OldListProc(hWnd,message,wParam,lParam);
        }
        break;

    case WM_KEYDOWN:
        switch(wParam)
        {
        case VK_F1:
            SendMessage(hWndApp,WM_KEYDOWN,VK_F1,0);
            break;
        default:
            lResult = OldListProc(hWnd,message,wParam,lParam);
            break;
        }
        break;
    default:
        lResult = OldListProc(hWnd,message,wParam,lParam);
        break;
    }
    return lResult;
}
/********************************************************************
* Function   APP_ViewUnanswered  
* Purpose     显示未接来电列表
* Params    
* Return     
* Remarks      
**********************************************************************/
BOOL    APP_ViewUnanswered(void)
{
    return (CallMBLogListExtWindow(NULL,UNANSWER_REC,MBL_WS_DEFAULT));
}
