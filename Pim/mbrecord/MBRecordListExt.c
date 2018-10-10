/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : ͨ����¼����
 *
 * Purpose  : ��ʾ�Ѳ��ѽ�δ�ӵ绰�б��Ĺ���
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

#define     IDC_BUTTON_RIGHT        3//�鿴��ť
#define     IDC_BUTTON_SET          4//ȷ����ť
#define     IDC_LIST                5

#define     TITLECAPTION            MBGetString(STR_MBRECORD_RECORD_CAPTION)
#define     TITLECAPTION1           MBGetString(STR_MBRECORD_DEL_UNANSWER)
#define     TITLECAPTION2           MBGetString(STR_MBRECORD_DEL_ANSWER)
#define     TITLECAPTION3           MBGetString(STR_MBRECORD_DEL_DIAL)

#define     BUTTONOK                (LPARAM)MBGetString(STR_WINDOW_OK)  //ȷ��
#define     BUTTONRIGHT             (LPARAM)MBGetString(STR_WINDOW_BACK)//����
#define     ERROR1                  MBGetString(STR_MBRECORD_ERROR1)    //������ʾ

#define     IDS_MBRECORD_VIEW       MBGetString(STR_MBPHONE_VIEW)           //�鿴
#define     IDS_MBRECORD_SHIKE      MBGetString(STR_MBRECORD_SHIKE)         //"ʱ��"
#define     IDS_MBRECORD_NUM        MBGetString(STR_MBRECORD_NUM)           //"����"
#define     IDS_MBRECORD_NAME       MBGetString(STR_MBRECORD_NAME)          //"����"
#define     IDS_MBRECORD_UNKNOWN    MBGetString(STR_MBRECORD_UNKNOWN)       //"δ֪"
#define     IDS_MBRECORD_DURATION   MBGetString(STR_MBRECORD_DURATIONVIEW)  //"��ʱ"

#define     MBRECORDDELETEONE       MBGetString(STR_MBRECORD_LIST_DELONE)   //�Ƿ�ɾ��
#define     MBRECORDDELETEALL       MBGetString(STR_MBRECORD_LIST_DELALL)   //�Ƿ�ɾ��
#define     MBRECORDLISTDELFAIL     MBGetString(STR_MBRECORD_DEL_FAIL)      //ɾ��ʧ��

#define     MBRECORDMENUVIEW        MBGetString(STR_MBRECORD_LIST_VIEW)     //"�鿴"
#define     MBRECORDMENUSORTTIME    MBGetString(STR_MBRECORD_LIST_TIME)     //"ʱ����"
#define     MBRECORDMENUSORTNAME    MBGetString(STR_MBRECORD_LIST_NAME)     //"������"
#define     MBRECORDMENUPICKUP      MBGetString(STR_MBRECORD_LIST_PICKUP)   //"��ȡ��"
#define     MBRECORDMENUPHONE       MBGetString(STR_MBRECORD_LIST_PHONE)    //"�绰"
#define     MBRECORDMENULINKMAN     MBGetString(STR_MBRECORD_LIST_LINKMAN)  //"��ϵ��"
#define     MBRECORDMENUSMS         MBGetString(STR_MBRECORD_LIST_SMS)      //"����"
#define     MBRECORDMENUMMS         MBGetString(STR_MBRECORD_LIST_MMS)      //"����"

#define     MBRECORDMENUDELETE      MBGetString(STR_MBRECORD_LIST_DELETE)   //ɾ��
#define     MBRECORDMENUDELETEONE   MBGetString(STR_MBRECORD_LIST_DELETEONE)//����
#define     MBRECORDMENUDELETEALL   MBGetString(STR_MBRECORD_LIST_DELETEALL)//ȫ��
#define     NODATATEXT              MBGetString(STR_MBRECORD_NODATATEXT)    //"�޺����¼"

#define     TITLECAPTIONMAXLEN      40//������󳤶�
#define     IDC_REFRESHRECORDLIST   WM_USER + 21//ˢ���б������Ϣ

//#define GETINFOFROMPHONEBOOK_QUICK
//ʹ��������ʾ�ڴ���ϵ���в�ѯ�����ʱ��,�����ڵ�ǰ�����н��в�ѯ,
//�ҵ���ͬ����Ľڵ�֮��,ֱ�ӴӸýڵ��з����������丽����Ϣ.
//���������Ա���Ƶ���Ĳ�ѯ��ϵ�˵�����,������Щ���������˵���Խ�ʡһЩʱ��.

#define SORT_INMENU//����������ʾ�˵��а���"����"��Ŀ

//*********************�˵�����*****************************
#if defined(SORT_INMENU)
#define     MENU_POPUP_PICKUP       2//�Ӳ˵�"��ȡ��"��λ�ñ�־
#define     MENU_POPUP_DELETE       3//�Ӳ˵�"ɾ��"��λ�ñ�־
#else
#define     MENU_POPUP_PICKUP       1//�Ӳ˵�"��ȡ��"��λ�ñ�־
#define     MENU_POPUP_DELETE       2//�Ӳ˵�"ɾ��"��λ�ñ�־
#endif

#define     IDM_VIEW                90//�鿴
#define     IDM_SORT                95//����
#define     IDM_DIALRECORD          100//�绰
#define     IDM_SAVERECORD          110//��ϵ��
#define     IDM_SENDSMS             120//����
#define     IDM_SENDMMS             130//����

#define     IDM_DELETEONE           140//������¼
#define     IDM_DELETEALL           150//���м�¼
static const MENUITEMTEMPLATE MenuGet[] = //��ȡ��
{
    { MF_STRING, IDM_DIALRECORD ,"", NULL},//�绰
    { MF_STRING, IDM_SAVERECORD ,"", NULL},//��ϵ��
    { MF_STRING, IDM_SENDSMS    ,"", NULL},//����
    { MF_STRING, IDM_SENDMMS    ,"", NULL},//����
    { MF_END, 0, NULL, NULL }
};
static const MENUITEMTEMPLATE MenuDelete[] = //ɾ��
{
    { MF_STRING, IDM_DELETEONE  ,"", NULL},//������¼
    { MF_STRING, IDM_DELETEALL  ,"", NULL},//���м�¼
    { MF_END, 0, NULL, NULL }
};

static const MENUITEMTEMPLATE MenuMain[] = 
{
    { MF_STRING,IDM_VIEW  ,"", NULL},//�鿴
#if defined(SORT_INMENU)
    { MF_STRING,IDM_SORT  ,"", NULL},//����
#endif
    { MF_STRING|MF_POPUP,0,"",(PMENUITEMTEMPLATE)MenuGet},//��ȡ��
    { MF_STRING|MF_POPUP,1,"",(PMENUITEMTEMPLATE)MenuDelete},//ɾ��
    { MF_END, 0, NULL, NULL }
};
static const MENUTEMPLATE MenuTemplate =
{
    0,
    (PMENUITEMTEMPLATE)MenuMain
};

static const int iMBRecords[] = //ͨ����¼������(δ��,�ѽ�,�Ѳ�)
{
    UNANSWER_REC,
    ANSWERED_REC,
    DIALED_REC
};
typedef enum
{
    iSORT_TIME = 1,//����ʱ������
    iSORT_NAME = 2 //������������
}SORTTYPE;

static const int    iRecordTypeCount = sizeof(iMBRecords) / sizeof(int);//ͨ����¼����ĸ���
static const int    iViewMaxLen      = 20;//�б�����ʾ�������󳤶�
static const char * pListBox         = "LISTBOX";
static const char * pClassName       = "MBRecordLogListExt";//��������
static const char * pSetTitle        = "%s(%d/%d)";
static const char * pCHOMIT          = "...";//�绰������ʾ����֮����ʾ��ʡ�Ժ�
static const char * pIconInName      = MBPIC_RECORDLIST_IN; //����ͼ��
static const char * pIconOutName     = MBPIC_RECORDLIST_OUT;//����ͼ��
static const char * pIconUnName      = MBPIC_RECORDLIST_UN; //δ��ͼ��

typedef struct tagMBRecordListExt
{
    int                         iMBRecordType;               //ͨ����¼������(δ��,�ѽ�,�Ѳ�)
    int                         iMBRecordIndex;              //ͨ����¼���ļ��е�λ��(����)
    char                        cDisplay[PHONENUMMAXLEN + 1];//��������,�绰����
    char                        cNumber[PHONENUMMAXLEN + 1]; //�绰����
    BOOL                        bInPhonebook;                //�����Ƿ�����ϵ����
    SYSTEMTIME                  Time;                        //��ͨʱ��
    struct tagMBRecordListExt * pNext;
}tagMBRecordListExt;
typedef struct tagMBRecordListExt MBRECORDLISTEXT;
static MBRECORDLISTEXT    * pMBRecordListExt;//����

//��ǰ���е�ͨ����¼����������ṹ������
static HINSTANCE            hInstance;
static DWORD                dRecordListType;//ͨ����¼������(δ��,�ѽ�,�Ѳ�)
static HWND                 hWndApp;
static HBITMAP              hIconUnans;     //δ��ͼ��
static HBITMAP              hIconIn;        //����ͼ��
static HBITMAP              hIconOut;       //����ͼ��
static char                 cTitle[TITLECAPTIONMAXLEN + 1] = "";//����
static WNDPROC              OldListProc;
static BOOL                 bRefreshList;//�����֮���Ƿ���Ҫˢ���б�
static MBRECORDLIST_STYLE   MBL_Style;//�б�����

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
    case ANSWERED_REC | DIALED_REC://��ʾ�Ѳ�,�ѽӵ绰�б�
        strcpy(cTitle,TITLECAPTION);
        break;
    case ANSWERED_REC | UNANSWER_REC://��ʾ�ѽ�,δ�ӵ绰�б�
        strcpy(cTitle,TITLECAPTION);
        break;
    case DIALED_REC | UNANSWER_REC://��ʾ�Ѳ�,δ�ӵ绰�б�
        strcpy(cTitle,TITLECAPTION);
        break;
    case ANSWERED_REC | DIALED_REC | UNANSWER_REC://��ʾδ��,�Ѳ�,�ѽӵ绰�б�
        strcpy(cTitle,TITLECAPTION);
        break;
    default:
        return FALSE;
    }

    hMenu = LoadMenuIndirect(&MenuTemplate);
    ModifyMenu(hMenu,IDM_VIEW,MF_BYCOMMAND|MF_ENABLED,IDM_VIEW,MBRECORDMENUVIEW);               //�鿴
    ModifyMenu(hMenu,IDM_SORT,MF_BYCOMMAND|MF_ENABLED,IDM_SORT,MBRECORDMENUSORTNAME);           //������

    ModifyMenu(hMenu,IDM_DIALRECORD,MF_BYCOMMAND|MF_ENABLED,IDM_DIALRECORD,MBRECORDMENUPHONE);  //�绰
    ModifyMenu(hMenu,IDM_SAVERECORD,MF_BYCOMMAND|MF_ENABLED,IDM_SAVERECORD,MBRECORDMENULINKMAN);//��ϵ��
    ModifyMenu(hMenu,IDM_SENDSMS,MF_BYCOMMAND|MF_ENABLED,IDM_SENDSMS,MBRECORDMENUSMS);          //����
    ModifyMenu(hMenu,IDM_SENDMMS,MF_BYCOMMAND|MF_ENABLED,IDM_SENDMMS,MBRECORDMENUMMS);          //����

    ModifyMenu(hMenu,IDM_DELETEONE,MF_BYCOMMAND|MF_ENABLED,IDM_DELETEONE,MBRECORDMENUDELETEONE);//ɾ��һ��
    ModifyMenu(hMenu,IDM_DELETEALL,MF_BYCOMMAND|MF_ENABLED,IDM_DELETEALL,MBRECORDMENUDELETEALL);//ȫ��ɾ��

    hSubMenu = GetSubMenu(hMenu,MENU_POPUP_PICKUP);
    ModifyMenu(hMenu,MENU_POPUP_PICKUP,MF_BYPOSITION|MF_POPUP,(DWORD)hSubMenu,MBRECORDMENUPICKUP);//��ȡ��
    hSubMenu = GetSubMenu(hMenu,MENU_POPUP_DELETE);
    ModifyMenu(hMenu,MENU_POPUP_DELETE,MF_BYPOSITION|MF_POPUP,(DWORD)hSubMenu,MBRECORDMENUDELETE);//ɾ��

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
    static  SORTTYPE    SortType;//��ǰ�ǰ���ʱ������,���ǰ�����������
    static  BOOL        bInit;//������־��ǰ�Ƿ��ǵ�һ�μ������
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
        SortType     = iSORT_TIME;//����ʱ������
        bRefreshList = FALSE;//�����֮����Ҫˢ���б�
        bInit        = FALSE;

        ReloadRecordList(hWnd,hList,SortType);//����ͨ����¼���б�����

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

    case WM_ACTIVATE://ȥ�������Ϣ�������Ҽ������˳��������˳�ʧ��
        if (WA_INACTIVE != LOWORD(wParam))
        {
            if (UNANSWER_REC & dRecordListType)
            {
                ResetnUnanswered();//δ�ӵ绰������λ
            }
            if (bRefreshList)//�����֮����Ҫˢ���б�
            {
                bRefreshList = FALSE;
                PostMessage(hWnd,IDC_REFRESHRECORDLIST,0,0);
            }
            else
            {
                hList = GetDlgItem(hWnd,IDC_LIST);
                SetFocus(hList);//��������ý���
                if (bInit)
                    RefreshMBRecordList_All(hList);
                else
                    bInit = TRUE;
            }
        }
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;

    case WM_CLOSE://������Ͻǵ��˳���ťʱΪ�����˳�;
        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_BUTTON_RIGHT, 0 );   
        break;

    case WM_DESTROY :       //֪ͨ���������,�˳����˳�;
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
        case VK_F1://����
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
        case IDM_VIEW://�鿴
            MBRecordListOperate(hWnd,GetDlgItem(hWnd,IDC_LIST),IDM_VIEW);
            break;
        case IDM_DIALRECORD://�绰
            MBRecordListOperate(hWnd,GetDlgItem(hWnd,IDC_LIST),IDM_DIALRECORD);
            break;
        case IDM_SAVERECORD://��ϵ��
            MBRecordListOperate(hWnd,GetDlgItem(hWnd,IDC_LIST),IDM_SAVERECORD);
            break;
        case IDM_SENDSMS://����
            MBRecordListOperate(hWnd,GetDlgItem(hWnd,IDC_LIST),IDM_SENDSMS);
            break;
        case IDM_SENDMMS://����
            MBRecordListOperate(hWnd,GetDlgItem(hWnd,IDC_LIST),IDM_SENDMMS);
            break;
        case IDM_DELETEONE://ɾ��������¼
            MBRecordListOperate(hWnd,GetDlgItem(hWnd,IDC_LIST),IDM_DELETEONE);
            break;
        case IDM_DELETEALL://ɾ�����м�¼
            MBRecordListOperate(hWnd,GetDlgItem(hWnd,IDC_LIST),IDM_DELETEALL);
            break;

        case IDM_SORT://����
            hMenu = GetMenu(hWnd);
            hList = GetDlgItem(hWnd,IDC_LIST);
            if (iSORT_TIME == SortType)//��ǰ�ǰ���ʱ������
            {
                MBRecordList_Sort(hWnd,hList,iSORT_NAME);//��Ҫ������������
                ModifyMenu(hMenu,IDM_SORT,MF_BYCOMMAND|MF_ENABLED,//���ֱ��ʱ����
                    IDM_SORT,MBRECORDMENUSORTTIME);

                if (MBL_Style & MBL_WS_MIDDLEBUTTON)//��ʾ�м��
                    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,
                    MAKEWPARAM(IDM_SORT,2),(LPARAM)MBRECORDMENUSORTTIME);

                SortType = iSORT_NAME;
            }
            else//��ǰ�ǰ�����������
            {
                MBRecordList_Sort(hWnd,hList,iSORT_TIME);//��Ҫ����ʱ������
                ModifyMenu(hMenu,IDM_SORT,MF_BYCOMMAND|MF_ENABLED,
                    IDM_SORT,MBRECORDMENUSORTNAME);//���ֱ��������

                if (MBL_Style & MBL_WS_MIDDLEBUTTON)//��ʾ�м��
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

    case IDC_REFRESHRECORDLIST://ˢ�µ绰�б�
        hList = GetDlgItem(hWnd,IDC_LIST);
        ReloadRecordList(hWnd,hList,SortType);//���¼���ͨ����¼���б�����
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
* Purpose    ��־����ڵ�ʱ����Ҫˢ���б�
* Params     
* Return     
* Remarks     
**********************************************************************/
void    MBRecrodList_Refresh(MBRECORDTYPE MBRecord_Type)
{
    if (IsWindow(hWndApp))
    {
        if (dRecordListType & MBRecord_Type)//��ǰ�б������д�����Ҫ���¼��ص�ͨ������
            bRefreshList = TRUE;
    }
}
/********************************************************************
* Function   MBRecordListOperate  
* Purpose    ִ�в˵�����
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
    case IDM_VIEW://�鿴
        {
            MBRECORD_VIEW   MBRecord_View;
            int             iSour;

            MBRecord_View.hWnd          = hWnd;
            MBRecord_View.index         = MBRecordListExt.iMBRecordIndex;
            MBRecord_View.MBRecordType  = MBRecordListExt.iMBRecordType;

            iSour = strlen(IDS_MBRECORD_VIEW);//���ڱ���
            strncpy(MBRecord_View.TitleInfo.cTitle,IDS_MBRECORD_VIEW,
                sizeof(MBRecord_View.TitleInfo.cTitle) > iSour ? iSour + 1 : sizeof(MBRecord_View.TitleInfo.cTitle) - 1);

            iSour = strlen(IDS_MBRECORD_NUM);//����
            strncpy(MBRecord_View.TitleInfo.cNumber,IDS_MBRECORD_NUM,
                sizeof(MBRecord_View.TitleInfo.cNumber) > iSour ? iSour + 1 : sizeof(MBRecord_View.TitleInfo.cNumber) - 1);

            iSour = strlen(IDS_MBRECORD_NAME);//����
            strncpy(MBRecord_View.TitleInfo.cName,IDS_MBRECORD_NAME,
                sizeof(MBRecord_View.TitleInfo.cName) > iSour ? iSour + 1 : sizeof(MBRecord_View.TitleInfo.cName) - 1);

            iSour = strlen(IDS_MBRECORD_SHIKE);//ʱ��
            strncpy(MBRecord_View.TitleInfo.cTime,IDS_MBRECORD_SHIKE,
                sizeof(MBRecord_View.TitleInfo.cTime) > iSour ? iSour + 1 : sizeof(MBRecord_View.TitleInfo.cTime) - 1);

            iSour = strlen(IDS_MBRECORD_DURATION);//��ʱ
            strncpy(MBRecord_View.TitleInfo.cDuration,IDS_MBRECORD_DURATION,
                sizeof(MBRecord_View.TitleInfo.cDuration) > iSour ? iSour + 1 : sizeof(MBRecord_View.TitleInfo.cDuration) - 1);

            MBRecord_View.ItemStyle = MBV_WS_NUMBER | MBV_WS_NAME | MBV_WS_TIME | MBV_WS_DURATION;
            MBRecord_View.TimeStyle = TS_NORMAL;

            iSour = strlen(IDS_MBRECORD_UNKNOWN);
            strncpy(MBRecord_View.cUnknow,IDS_MBRECORD_UNKNOWN,
                sizeof(MBRecord_View.cUnknow) > iSour ? iSour + 1 : sizeof(MBRecord_View.cUnknow) - 1);

            MBRecord_View.MBRecordView_Style = MBV_WS_DEFAULT;
            bRefreshList = FALSE;
            /*����CallMBLogViewWindow��������֮��,�����ڵ�WM_ACTIVATE��Ϣ�����ȵ���,
            Ȼ��CallMBLogViewWindow�����Ż��з���ֵ����,Ϊ�˱�����WM_ACTIVATE��Ϣ��
            ����IDC_REFRESHRECORDLIST��Ϣ����ˢ��,�����ڴ˽�bRefreshList��ֵΪFALSE.
            ��֤��WM_ACTIVATE��Ϣ�в������ˢ��.
            ���绰�Ѿ��޸�������,������CallMBLogViewWindow������ɾ����¼�Ļ�,
            �����MBRecrodList_Refresh����,�����ڴ��ڼ����ʱ����Զ�ˢ��,������
            ������������IDC_REFRESHRECORDLIST��Ϣ����ˢ��*/
            if (MBRECORD_RS_MODIFY == CallMBLogViewWindow(&MBRecord_View))
                RefreshMBRecordList_One(hList,&MBRecordListExt);
        }
        break;

    case IDM_DIALRECORD://�绰
        if (-1 != GetMBRecord(&MBRecord,sizeof(CallRecord),
            MBRecordListExt.iMBRecordType,MBRecordListExt.iMBRecordIndex))
            APP_CallPhoneNumber(MBRecord.PhoneNo);//��ǰ�绰�Ѿ�����
        break;

    case IDM_SAVERECORD://��ϵ��
        if (-1 != GetMBRecord(&MBRecord,sizeof(CallRecord),
            MBRecordListExt.iMBRecordType,MBRecordListExt.iMBRecordIndex))
        {
            if (MB_SaveToPhoneBook(hWnd,MBRecord.PhoneNo))//���绰�Ѿ��޸�������
                RefreshMBRecordList_One(hList,&MBRecordListExt);
        }
        break;

    case IDM_SENDSMS://����
        if (-1 != GetMBRecord(&MBRecord,sizeof(CallRecord),
            MBRecordListExt.iMBRecordType,MBRecordListExt.iMBRecordIndex))
            MB_SendSms(MBRecord.PhoneNo);
        break;

    case IDM_SENDMMS://����
        if (-1 != GetMBRecord(&MBRecord,sizeof(CallRecord),
            MBRecordListExt.iMBRecordType,MBRecordListExt.iMBRecordIndex))
            MB_SendMms(MBRecord.PhoneNo);
        break;

    case IDM_DELETEONE://ɾ��������¼
        bRefreshList = FALSE;
        /*����ѯ�ʴ�������֮��,�����ڵ�WM_ACTIVATE��Ϣ�����ȵ���,
        Ȼ��ѯ�ʺ����Ż��з���ֵ����,Ϊ�˱�����WM_ACTIVATE��Ϣ��
        ����IDC_REFRESHRECORDLIST��Ϣ����ˢ��,�����ڴ˽�bRefreshList��ֵΪFALSE.
        ��֤��WM_ACTIVATE��Ϣ�в������ˢ��*/
        if (AppConfirmWin(hWnd,MBRECORDDELETEONE,"",cTitle,
            CONFIRM_OK,CONFIRM_CANCEL))
        {
            if (MBL_Style & MBL_WS_NOSHOWREPEATDATA)//�б��в���ʾ�����ظ�������(�����ļ��б����к����ظ��ļ�¼)
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
    case IDM_DELETEALL://ɾ�����м�¼
        bRefreshList = FALSE;
        /*����ѯ�ʴ�������֮��,�����ڵ�WM_ACTIVATE��Ϣ�����ȵ���,
        Ȼ��ѯ�ʺ����Ż��з���ֵ����,Ϊ�˱�����WM_ACTIVATE��Ϣ��
        ����IDC_REFRESHRECORDLIST��Ϣ����ˢ��,�����ڴ˽�bRefreshList��ֵΪFALSE.
        ��֤��WM_ACTIVATE��Ϣ�в������ˢ��*/
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
* Purpose    ��ʼ��ͼƬ����
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
* Purpose    ���ñ���
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
* Purpose    ��ǰ�����Ƿ��Ѿ�����������֮��
* Params     pNumber:��Ҫ����ĵ绰����
* Return     �ҵ��ظ��ĺ��뷵��TRUE,���򷵻�FALSE;
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
* Purpose    ��ǰ�����Ƿ��Ѿ�����������֮��,�ǵĻ�,��������
* Params     pNumber:��Ҫ����ĵ绰����,pDisplayName:���ڷ���������buff,iBufferlen:buff����
* Return     �ҵ��ظ��ĺ��뷵��TRUE,���򷵻�FALSE;
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
* Purpose    �����Ӧλ�õĽڵ����Ϣ
* Params     pNode:������Ӧλ�õĽڵ����Ϣ,iIndex:�ڵ��������е�λ��
* Return     �ҵ�����ڵ㷵��TRUE,���򷵻�FALSE;
* Remarks     
**********************************************************************/
static  BOOL    GetMBRecordListExtNode(MBRECORDLISTEXT * pNode,int iIndex)
{
    int                 i = 0,iCount;
    MBRECORDLISTEXT   * p = pMBRecordListExt;

    if (NULL == pMBRecordListExt)
        return FALSE;
    iCount = GetRecordListCount();

    if ((0 == iCount) || (iIndex < 0) || (iIndex > iCount - 1))//����Ϊ�ջ��߽ڵ�λ�÷Ƿ�
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
            memcpy(pNode,p,sizeof(MBRECORDLISTEXT));//�ҵ�����ڵ�,��������
            return TRUE;
        }
    }
    return FALSE;
}
/********************************************************************
* Function   ReloadRecordList  
* Purpose    ����ͨ����¼���б�����
* Params     iSortType:�������
* Return     
* Remarks     
**********************************************************************/
static  void    ReloadRecordList(HWND hWnd,HWND hList,SORTTYPE SortType)
{
    int                 index,iOldSel;
    char                cDisplayData[PHONENUMMAXLEN + 1] = "";
    MBRECORDLISTEXT   * p;

    iOldSel = SendMessage(hList,LB_GETCURSEL,0,0);
    LoadMBRecordToLink(SortType);//����ͨ����¼��������
    SendMessage(hList,LB_RESETCONTENT,0,0);

    p = pMBRecordListExt;
    while (p != NULL)
    {
        if (p->bInPhonebook)//�õ绰����İ����
        {
            strcpy(cDisplayData,p->cDisplay);
        }
        else//�õ绰��İ����
        {
            if ((int)strlen(p->cDisplay) > iViewMaxLen)
            {
                strncpy(cDisplayData,p->cDisplay,iViewMaxLen);
                strcat(cDisplayData,pCHOMIT);//"..."����ʡ�Ժ�
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

    if (MBL_Style & MBL_WS_MIDDLEBUTTON)//��ʾ�м��
    {
        if (SendMessage(hList,LB_GETCOUNT,0,0) == 0)//û�м�¼��ʱ����ʾ�м�İ�ť
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDM_SORT,2),(LPARAM)"");
        else
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDM_SORT,2),(LPARAM)MBRECORDMENUSORTNAME);
    }

    SetTitle(hWnd,hList);//���ñ���
    if (MBL_Style & MBL_WS_NODATA_NOTSHOWLIST)//û�����ݵ�ʱ����ʾ�б���,ֻ�ڽ�������ʾ"�޺����¼"
    {
        ShowWindow(hList,SendMessage(hList,LB_GETCOUNT,0,0) == 0 ? SW_HIDE : SW_SHOW);
    }
}
/********************************************************************
* Function   LoadMBRecordToLink  
* Purpose    ����ͨ����¼�������� 
* Params     iSortType:�������
* Return     
* Remarks      
**********************************************************************/
static  BOOL    LoadMBRecordToLink(SORTTYPE SortType)
{
    int         i,j,index,iRecordCount;
    CallRecord  MBRecords;//����ͨ������
    PBINFO      pbNameIcon;
    char        cDisplayData[PHONENUMMAXLEN + 1] = "";
    BOOL        bInAddress;

    DestroyRecordListExt();//�������нڵ�
    for (j = 0;j < iRecordTypeCount;j++)
    {
        if (dRecordListType & iMBRecords[j])
        {
            iRecordCount = GetRecorderNumber(iMBRecords[j]);//����ѽ�,�Ѳ�,δ�Ӽ�¼����

            for (i = 0;i < iRecordCount;i++)
            {
                memset(&MBRecords,0x00,sizeof(MBRecords));
                //����һ��ͨ����Ϣ
                if (-1 == GetMBRecord(&MBRecords,sizeof(CallRecord),iMBRecords[j],i))
                    continue;

                if (strlen(MBRecords.PhoneNo) == 0)
                    break;

                if (MBL_Style & MBL_WS_NOSHOWREPEATDATA)//�б��в���ʾ�����ظ�������(�����ļ��б����к����ظ��ļ�¼)
                {
                    if (GetIfHaveSameNumber(MBRecords.PhoneNo))//�����ظ�����
                        continue;
                }
                memset(&pbNameIcon,0x00,sizeof(PBINFO));
#if defined(GETINFOFROMPHONEBOOK_QUICK)
                if (!GetSameNumber(MBRecords.PhoneNo,cDisplayData,
                    sizeof(cDisplayData),&bInAddress))
#endif
                {
                if (MB_GetInfoFromPhonebook(MBRecords.PhoneNo,&pbNameIcon))//�õ绰����İ����
                {
                    strcpy(cDisplayData,pbNameIcon.Name);//��¼����
                    bInAddress = TRUE;
                }
                else
                {
                    strcpy(cDisplayData,MBRecords.PhoneNo);//��¼����,��������
                    bInAddress = FALSE;
                }
                }
                index = AddNodeMBRecordListExt(iMBRecords[j],i,bInAddress,//���ӽڵ�
                                                &MBRecords.Time,cDisplayData,
                                                MBRecords.PhoneNo,SortType);
            }
        }
    }
    return TRUE;
}
/********************************************************************
* Function     AddNodeMBRecordListExt
* Purpose      ���ڵ���뵽������Ӧλ��
* Params       iMBRecordType:ͨ����¼������(δ��,�ѽ�,�Ѳ�)
                iMBRecordIndex:ͨ����¼���ļ��е�λ��
                bInPhonebook:�����Ƿ�����ϵ����
                pTime:��ͨʱ��
                pDisplay:��������,�绰����
                iSortType:�������
* Return        �ڵ������λ��,-1��ʾ����ʧ��
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

    p->iMBRecordType    = iMBRecordType;        //ͨ����¼������(δ��,�ѽ�,�Ѳ�)
    p->iMBRecordIndex   = iMBRecordIndex;       //ͨ����¼���ļ��е�λ��
    p->bInPhonebook     = bInPhonebook;         //�����Ƿ�����ϵ����
    memcpy(&p->Time,pTime,sizeof(SYSTEMTIME));  //��ͨʱ��

    iCount = GetRecordListCount();
    index  = 0;//��ǰָ��ָ��Ľڵ��λ��
    if (iCount == 0)//����Ϊ��,��������ֱ�Ӽ���ͷ�ڵ�
    {
        pMBRecordListExt = p;
        p->pNext         = NULL;
        return (index);
    }
    if (iCount == 1)//������ֻ��һ���ڵ�
    {
        if (iSORT_TIME == SortType)//����ʱ������
        {
            if (TimeCmp(&p->Time,&pMBRecordListExt->Time) < 0)
            {
                //��Ҫ���ӵĽڵ�ʱ��С�ڵ�ǰ�ڵ�ʱ��,���ڵ���뵽����β
                pMBRecordListExt->pNext = p;
                p->pNext                = NULL;
                index++;
            }
            else//��Ҫ���ӵĽڵ�ʱ����ڵ�ǰ�ڵ�ʱ��,���ڵ���뵽����ͷ
            {
                p->pNext         = pMBRecordListExt;
                pMBRecordListExt = p;
            }
        }
        else if (iSORT_NAME == SortType)//������������
        {
            if (strcmp(p->cDisplay,pMBRecordListExt->cDisplay) > 0)
            {
                //��Ҫ���ӵĽڵ��������ڵ�ǰ�ڵ�����,���ڵ���뵽����β
                pMBRecordListExt->pNext = p;
                p->pNext                = NULL;
                index++;
            }
            else//��Ҫ���ӵĽڵ�����С�ڵ�ǰ�ڵ�����,���ڵ���뵽����ͷ
            {
                p->pNext         = pMBRecordListExt;
                pMBRecordListExt = p;
            }
        }
        return (index);
    }
    //�����ڵ��������1��
    pCur    = pMBRecordListExt;//ָ��ǰ�ڵ�
    pCurPre = NULL;//ָ��ǰ�ڵ��ǰһ���ڵ�
    while (pCur != NULL)
    {
        if (iSORT_TIME == SortType)//����ʱ������
        {
            if (TimeCmp(&p->Time,&pCur->Time) < 0)
            {
                //��Ҫ���ӵĽڵ�ʱ��С�ڵ�ǰ�ڵ�ʱ��,����������
                pCurPre = pCur;
                pCur    = pCur->pNext;
                index++;
            }
            else//��Ҫ���ӵĽڵ�ʱ����ڵ�ǰ�ڵ�ʱ��,���ڵ���뵽pCur��ǰ��
            {
                if (NULL == pCurPre)//��ǰ�ڵ��ǰһ���ڵ�Ϊ��,����뵽ͷ�ڵ�
                {
                    p->pNext         = pMBRecordListExt;
                    pMBRecordListExt = p;
                }
                else//��ǰ�ڵ��ǰһ���ڵ�ǿ�,���뵽pCurPre�ڵ�֮��
                {
                    p->pNext        = pCur;
                    pCurPre->pNext  = p;
                }
                return (index);
            }
        }
        else if (iSORT_NAME == SortType)//������������
        {
            if (strcmp(p->cDisplay,pCur->cDisplay) > 0)
            {
                //��Ҫ���ӵĽڵ����������ڵ�ǰ�ڵ�����,����������
                pCurPre = pCur;
                pCur    = pCur->pNext;
                index++;
            }
            else//��Ҫ���ӵĽڵ�����С�ڵ�ǰ�ڵ�����,���ڵ���뵽pCur��ǰ��
            {
                if (NULL == pCurPre)//��ǰ�ڵ��ǰһ���ڵ�Ϊ��,����뵽ͷ�ڵ�
                {
                    p->pNext         = pMBRecordListExt;
                    pMBRecordListExt = p;
                }
                else//��ǰ�ڵ��ǰһ���ڵ�ǿ�,���뵽pCurPre�ڵ�֮��
                {
                    p->pNext        = pCur;
                    pCurPre->pNext  = p;
                }
                return (index);
            }
        }
    }
    //��ʱpCurPreָ��������β,��Ҫ���ڵ����������β
    pCurPre->pNext = p;
    p->pNext       = NULL;

    return (index);
}
/********************************************************************
* Function     DestroyRecordListExt
* Purpose      ɾ�����нڵ�
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
* Purpose      ��������ڵ���
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
* Purpose      �б�����
* Params       SortType:����ʽ
* Return           
* Remarks      
**********************************************************************/
static  void    MBRecordList_Sort(HWND hWnd,HWND hList,SORTTYPE SortType)
{
    MBRECORDLISTEXT   * p;
    char                cDisplayData[PHONENUMMAXLEN + 1] = "";
    int                 index;

    SrotMBRecordList(SortType);//��������
    SendMessage(hList,LB_RESETCONTENT,0,0);

    p = pMBRecordListExt;
    while (p != NULL)//���������б���Ŀ
    {
        if (p->bInPhonebook)//�õ绰����İ����
        {
            strcpy(cDisplayData,p->cDisplay);
        }
        else//�õ绰��İ����
        {
            if ((int)strlen(p->cDisplay) > iViewMaxLen)
            {
                strncpy(cDisplayData,p->cDisplay,iViewMaxLen);
                strcat(cDisplayData,pCHOMIT);//"..."����ʡ�Ժ�
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
    SetTitle(hWnd,hList);//���ñ���
}
/********************************************************************
* Function     iGetNewMBRecrodListCount
* Purpose      ����������ĸ���
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
* Purpose      ��������
* Params       
* Return           
* Remarks      
**********************************************************************/
static  void    SrotMBRecordList(SORTTYPE SortType)
{
    BOOL              bInsert;//�Ƿ��Ѿ���������
    int               iCount;
    MBRECORDLISTEXT * pNew = NULL;//������
    MBRECORDLISTEXT * pOld = pMBRecordListExt;//������
    MBRECORDLISTEXT * pCur = NULL,* pCurPre = NULL,* p = NULL;

    while (pOld != NULL)
    {
        iCount = iGetNewMBRecrodListCount(pNew);
        p      = pOld;//pָ��������ĵ�ǰ�ڵ�
        pOld   = pOld->pNext;

        if (0 == iCount)//����Ϊ��,��������ֱ�Ӽ���ͷ�ڵ�
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
                {   //��Ҫ���ӵĽڵ�ʱ��С�ڵ�ǰ�ڵ�ʱ��,���ڵ���뵽����β
                    pNew->pNext = p;
                    p->pNext = NULL;
                }
                else//��Ҫ���ӵĽڵ�ʱ����ڵ�ǰ�ڵ�ʱ��,���ڵ���뵽����ͷ
                {
                    p->pNext = pNew;
                    pNew     = p;
                }
                break;
            case iSORT_NAME:
                if (strcmp(p->cDisplay,pNew->cDisplay) > 0)
                {   //��Ҫ���ӵĽڵ��������ڵ�ǰ�ڵ�����,���ڵ���뵽����β
                    pNew->pNext = p;
                    p->pNext = NULL;
                }
                else//��Ҫ���ӵĽڵ�����С�ڵ�ǰ�ڵ�����,���ڵ���뵽����ͷ
                {
                    p->pNext = pNew;
                    pNew     = p;
                }
                break;
            }
            continue;
        }
        //�����ڵ��������1��
        bInsert = FALSE;
        pCur    = pNew;//ָ��ǰ�ڵ�
        pCurPre = NULL;//ָ��ǰ�ڵ��ǰһ���ڵ�
        while (pCur != NULL)
        {
            if (iSORT_TIME == SortType)
            {
                if (TimeCmp(&p->Time,&pCur->Time) < 0)
                {
                    //��Ҫ���ӵĽڵ�ʱ��С�ڵ�ǰ�ڵ�ʱ��,����������
                    pCurPre = pCur;
                    pCur    = pCur->pNext;
                    continue;
                }
                else//��Ҫ���ӵĽڵ�ʱ����ڵ�ǰ�ڵ�ʱ��,���ڵ���뵽pCur��ǰ��
                {
                    if (NULL == pCurPre)//��ǰ�ڵ��ǰһ���ڵ�Ϊ��,����뵽ͷ�ڵ�
                    {
                        p->pNext = pNew;
                        pNew     = p;
                    }
                    else//��ǰ�ڵ��ǰһ���ڵ�ǿ�,���뵽pCurPre�ڵ�֮��
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
                    //��Ҫ���ӵĽڵ����������ڵ�ǰ�ڵ�����,����������
                    pCurPre = pCur;
                    pCur    = pCur->pNext;
                    continue;
                }
                else//��Ҫ���ӵĽڵ�����С�ڵ�ǰ�ڵ�����,���ڵ���뵽pCur��ǰ��
                {
                    if (NULL == pCurPre)//��ǰ�ڵ��ǰһ���ڵ�Ϊ��,����뵽ͷ�ڵ�
                    {
                        p->pNext = pNew;
                        pNew     = p;
                    }
                    else//��ǰ�ڵ��ǰһ���ڵ�ǿ�,���뵽pCurPre�ڵ�֮��
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
            //��ʱpCurPreָ��������β,��Ҫ���ڵ����������β
            pCurPre->pNext = p;
            p->pNext       = NULL;
        }
    }
    pMBRecordListExt = pNew;
}
/********************************************************************
* Function     RefreshMBRecordList_All
* Purpose      ����ˢ��֮��,������ϵ�˲����Ƿ��и���,����и��µĻ�,����ˢ���б�
* Params       
* Return           
* Remarks      
**********************************************************************/
static  void    RefreshMBRecordList_All(HWND hList)
{
    MBRECORDLISTEXT   * p;
    PBINFO              pbNameIcon;
    int                 iSour,i = 0,index,iOldSel;
    BOOL                bModify;//�ýڵ�������Ƿ������޸�
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
        if (MB_GetInfoFromPhonebook(p->cNumber,&pbNameIcon))//�õ绰����İ����
        {
            if (p->bInPhonebook)//�õ绰ԭ���Ͳ���İ����
            {
                if (strcmp(p->cDisplay,pbNameIcon.Name) != 0)//���������ı�
                {
                    iSour = strlen(pbNameIcon.Name);
                    strncpy(p->cDisplay,pbNameIcon.Name,
                        sizeof(p->cDisplay) > iSour ? iSour + 1 : sizeof(p->cDisplay) - 1);
                    bModify = TRUE;

                    strcpy(cDisplayData,p->cDisplay);
                }
                else//����û�иı�
                {
                    bModify = FALSE;
                }
            }
            else//�õ绰ԭ����İ����
            {
                iSour = strlen(pbNameIcon.Name);
                strncpy(p->cDisplay,pbNameIcon.Name,
                    sizeof(p->cDisplay) > iSour ? iSour + 1 : sizeof(p->cDisplay) - 1);
                p->bInPhonebook = TRUE;
                bModify = TRUE;

                strcpy(cDisplayData,p->cDisplay);
            }
        }
        else//�õ绰��İ����
        {
            if (p->bInPhonebook)//�õ绰ԭ������İ����
            {
                iSour = strlen(p->cNumber);
                strncpy(p->cDisplay,p->cNumber,
                    sizeof(p->cDisplay) > iSour ? iSour + 1 : sizeof(p->cDisplay) - 1);
                p->bInPhonebook = FALSE;
                bModify = TRUE;

                if ((int)strlen(p->cDisplay) > iViewMaxLen)
                {
                    strncpy(cDisplayData,p->cDisplay,iViewMaxLen);
                    strcat(cDisplayData,pCHOMIT);//"..."����ʡ�Ժ�
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
* Purpose      ���û�����ϵ�����޸���ĳ���ڵ������֮��,��Ҫ�����б���
* Params       pcMBRecordListExt:��Ҫ�޸ĵĽڵ�
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
        {   //�������в�����ʾ����ΪpcMBRecordListExt->cDisplay������
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
                    strcat(cDisplayData,pCHOMIT);//"..."����ʡ�Ժ�
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
* Purpose     ����listbox�ؼ�ÿһ����Ŀ��ͼ��
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
* Purpose    �����ؼ� 
* Params     hwnd�����ڹ��̾��
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

    if (MBL_Style & MBL_WS_MIDDLEBUTTON)//��ʾ�м��
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDM_SORT,2),
        (LPARAM)MBRECORDMENUSORTNAME);

    GetScreenUsableWH2(&iscreenw,&iscreenh);

    if (MBL_Style & MBL_WS_NOVSCROLL)//����ʾ������
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
        case VK_EARPHONE_KEY://�����ϵİ���
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
* Purpose     ��ʾδ�������б�
* Params    
* Return     
* Remarks      
**********************************************************************/
BOOL    APP_ViewUnanswered(void)
{
    return (CallMBLogListExtWindow(NULL,UNANSWER_REC,MBL_WS_DEFAULT));
}