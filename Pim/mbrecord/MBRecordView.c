/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 通话记录部分
 *
 * Purpose  : 通话记录相应查看界面
 *            
\**************************************************************************/
#include    "MBRecordView.h"
#include    "winpda.h"
#include    "hp_pdaex.h"
#include    "../mbphone/MBPublic.h"
#include    "../mbphone/str_mb_plx.h"

#define     VIEWINFONUM             4//显示项目

#define     IDC_BUTTON_QUIT         3//退出按钮

#define     BUTTONQUIT              MBGetString(STR_WINDOW_BACK)    //退出

#define     MBRECORDLISTDELFAIL     MBGetString(STR_MBRECORD_DEL_FAIL)      //删除失败
#define     IDS_MBRECORD_CALLBACK   MBGetString(STR_MBRECORD_VIEW_CALLBACK) //回叫
#define     MBRECORDDELETEONE       MBGetString(STR_MBRECORD_LIST_DELONE)   //是否删除?

#define     MONDAY                  MBGetString(STR_MBRECORD_MONDAY)    //"星期一"
#define     TUESDAY                 MBGetString(STR_MBRECORD_TUESDAY)   //"星期二"
#define     WEDNESDAY               MBGetString(STR_MBRECORD_WEDNESDAY) //"星期三"
#define     THURSDAY                MBGetString(STR_MBRECORD_THURSDAY)  //"星期四"
#define     FRIDAY                  MBGetString(STR_MBRECORD_FRIDAY)    //"星期五"
#define     SATURDAY                MBGetString(STR_MBRECORD_SATURDAY)  //"星期六"
#define     SUNDAY                  MBGetString(STR_MBRECORD_SUNDAY)    //"星期日"
#define     AM                      MBGetString(STR_MBRECORD_AM)        //"上午"
#define     PM                      MBGetString(STR_MBRECORD_PM)        //"下午"
//**********************菜单部分**********************
#define     MENU_DELETE             MBGetString(STR_MBRECORD_LIST_DELETE)   //删除
#define     MENU_PHONE              MBGetString(STR_MBRECORD_LIST_PHONE)    //电话
#define     MENU_LINKMAN            MBGetString(STR_MBRECORD_LIST_LINKMAN)  //联系人
#define     MENU_SMS                MBGetString(STR_MBRECORD_LIST_SMS)      //短信
#define     MENU_MMS                MBGetString(STR_MBRECORD_LIST_MMS)      //彩信

#define     IDM_PHONE               100
#define     IDM_LINKMAN             110
#define     IDM_SMS                 120
#define     IDM_MMS                 130
#define     IDM_DELETE              140
static const MENUITEMTEMPLATE MBRecordViewMenu[] = 
{
    { MF_STRING, IDM_PHONE  ,"", NULL},//Phone
    { MF_STRING, IDM_DELETE ,"", NULL},//删除
    { MF_STRING, IDM_LINKMAN,"", NULL},//联系人
    { MF_STRING, IDM_SMS    ,"", NULL},//短信
    { MF_STRING, IDM_MMS    ,"", NULL},//彩信
    { MF_END, 0, NULL, NULL }
};
static const MENUTEMPLATE MBRecordViewMenuTemplate =
{
    0,
    (PMENUITEMTEMPLATE)MBRecordViewMenu
};

static HINSTANCE        hInstance;
static MBRECORD_RS      BackValue;
static MBRECORD_VIEW    MBRecord_View;

static const char * pClassName      = "MBRecordViewWndClass";//窗口类名
static const char * pTimeNormal     = "%s:%04d-%02d-%02d %02d:%02d";        //年月日时分
static const char * pTimeNormal_Un  = "%04d-%02d-%02d %02d:%02d";           //年月日时分
static const char * pTime_Week      = "%s:%04d-%02d-%02d %s %02d:%02d";     //年月日星期时分
static const char * pTime_Week_Un   = "%04d-%02d-%02d %s %02d:%02d";        //年月日星期时分
static const char * pTime_Week_M    = "%s:%04d-%02d-%02d %s %02d:%02d %s";  //年月日星期时分+上下午
static const char * pTime_Week_M_Un = "%04d-%02d-%02d %s %02d:%02d %s";     //年月日星期时分+上下午
static const char * pTime_M         = "%s:%04d-%02d-%02d %02d:%02d %s";     //年月日时分+上下午
static const char * pTime_M_Un      = "%04d-%02d-%02d %02d:%02d %s";        //年月日时分+上下午

static const char * pTimeUsed       = "%s:%02d:%02d:%02d";
static const char * pTimeUsed_Un    = "%02d:%02d:%02d";
static const char * pString         = "%s:%s";
static const char * pString_Un      = "%s";
static const int    iRectX          = 10;
static const int    iAM_PM          = 12;

static  LRESULT AppWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam);
static  void    ShowMBRecorInfo(HWND hWnd,HDC hdc,CallRecord * pMBRecord);
static  BOOL    GetWeekInfo(const SYSTEMTIME * pTime,char * pWeek);

MBRECORD_RS CallMBLogViewWindow(const MBRECORD_VIEW * pMBRecord_Type)
{
    HWND        hWnd;
    WNDCLASS    wc;
    HMENU       hMenu;
    MSG         msg;

    if (NULL == pMBRecord_Type)
        return MBRECORD_RS_CREATEFAIL;

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
        return MBRECORD_RS_CREATEFAIL;

    memset(&MBRecord_View,0x00          ,sizeof(MBRECORD_VIEW));
    memcpy(&MBRecord_View,pMBRecord_Type,sizeof(MBRECORD_VIEW));

    hMenu = LoadMenuIndirect((PMENUTEMPLATE)&MBRecordViewMenuTemplate);
    ModifyMenu(hMenu,IDM_PHONE  ,MF_BYCOMMAND|MF_ENABLED,IDM_PHONE  ,MENU_PHONE  );
    ModifyMenu(hMenu,IDM_DELETE ,MF_BYCOMMAND|MF_ENABLED,IDM_DELETE ,MENU_DELETE );
    ModifyMenu(hMenu,IDM_LINKMAN,MF_BYCOMMAND|MF_ENABLED,IDM_LINKMAN,MENU_LINKMAN);
    ModifyMenu(hMenu,IDM_SMS    ,MF_BYCOMMAND|MF_ENABLED,IDM_SMS    ,MENU_SMS    );
    ModifyMenu(hMenu,IDM_MMS    ,MF_BYCOMMAND|MF_ENABLED,IDM_MMS    ,MENU_MMS    );

    hWnd = CreateWindow(pClassName,MBRecord_View.TitleInfo.cTitle, 
        WS_CAPTION|WS_BORDER |PWS_STATICBAR, 
        PLX_WIN_POSITION, MBRecord_View.hWnd, hMenu, NULL, NULL);

    if (NULL == hWnd)
    {
        UnregisterClass(pClassName,NULL);
        return MBRECORD_RS_CREATEFAIL;
    }
    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd); 

    while (IsWindow(hWnd))
    {
        GetMessage(&msg, NULL, 0, 0);
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (BackValue);
}
static LRESULT AppWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam)
{
    static  CallRecord  MBRecord;
            LRESULT     lResult;
            HDC         hdc;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,
            (WPARAM)IDC_BUTTON_QUIT,(LPARAM)BUTTONQUIT);

        if (MBRecord_View.MBRecordView_Style & MBV_WS_BUTTON_MIDDLE)//包含中间键
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, //回叫
            MAKEWPARAM(IDM_PHONE,2),(LPARAM)IDS_MBRECORD_CALLBACK);

        if (-1 == GetMBRecord(&MBRecord,sizeof(CallRecord),
            MBRecord_View.MBRecordType,MBRecord_View.index))
            return FALSE;

        break;

    case WM_SETFOCUS:
        SetFocus(hWnd);
        break;

    case WM_CLOSE://点击右上角的退出按钮时为保存退出;
        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_BUTTON_QUIT, 0 );   
        break;

    case WM_DESTROY ://通知程序管理器,此程序退出;
        UnregisterClass(pClassName,NULL);
        DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
        break;

    case WM_PAINT :
        hdc = BeginPaint(hWnd,NULL);
        ShowMBRecorInfo(hWnd,hdc,&MBRecord);
        EndPaint(hWnd, NULL);
        break;

    case WM_TODESKTOP:
        SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
        return TRUE;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_F1:
            SendMessage(hWnd,WM_COMMAND,IDM_PHONE,0);
            break;
        case VK_F10:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        case IDM_DELETE://删除
            if (AppConfirmWin(hWnd,MBRECORDDELETEONE,"",
                MBRecord_View.TitleInfo.cTitle,CONFIRM_OK,CONFIRM_CANCEL))
            {
                if (DeleteMBRecord(MBRecord_View.MBRecordType,MBRecord_View.index))
                {
                    MBRecrodList_Refresh(MBRecord_View.MBRecordType);
                    BackValue = MBRECORD_RS_DELETE;//本条数据已经删除
                    DestroyWindow(hWnd);
                }
                else
                {
                    AppMessageBox(hWnd,MBRECORDLISTDELFAIL,
                        MBRecord_View.TitleInfo.cTitle, WAITTIMEOUT);
                }
            }
            break;
        case IDM_PHONE://电话
            APP_CallPhoneNumber(MBRecord.PhoneNo);//调用拨号功能并且直接拨出去
            BackValue = MBRECORD_RS_DIALUP;//本电话已经拨出
            break;
        case IDM_LINKMAN://联系人
            if (MB_SaveToPhoneBook(hWnd,MBRecord.PhoneNo))
                BackValue = MBRECORD_RS_MODIFY;//本电话已经修改了姓名
            break;
        case IDM_SMS://短信
            MB_SendSms(MBRecord.PhoneNo);
            break;
        case IDM_MMS://彩信
            MB_SendMms(MBRecord.PhoneNo);
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
* Function   ShowMBRecorInfo  
* Purpose    显示文字
* Params     pMBRecord:通话计时信息
* Return     
* Remarks      
**********************************************************************/
static  void    ShowMBRecorInfo(HWND hWnd,HDC hdc,CallRecord * pMBRecord)
{
    RECT   rRect[VIEWINFONUM];
    int    iSW,iy[VIEWINFONUM],iFontHeight,i;
    char   cBufPhone[PHONENUMMAXLEN + MBRECORD_CLEW_NAMEMAXLEN + 2] = "";
    char   cBufName[MBRECORD_CLEW_NAMEMAXLEN + DB_NAMELEN + 2] = "";
    char   cBufTime[MBRECORD_CLEW_TIMEMAXLEN + 31] = "";
    char   cBufTimeUsed[MBRECORD_CLEW_DURATIONMAXLEN + 11] = "";
    char   cWeek[10] = "";
    PBINFO pbNameIcon;

    //显示号码
    if ((MBRecord_View.ItemStyle & MBV_WS_NUMBER) && 
        (strlen(MBRecord_View.TitleInfo.cNumber) > 0))//显示号码标题
        sprintf(cBufPhone,pString,MBRecord_View.TitleInfo.cNumber,pMBRecord->PhoneNo);
    else
        sprintf(cBufPhone,pString_Un,pMBRecord->PhoneNo);
    //显示姓名
    memset(&pbNameIcon,0x00, sizeof(PBINFO));
    if (MB_GetInfoFromPhonebook(pMBRecord->PhoneNo,&pbNameIcon))//该电话不是陌生的
    {
        if ((MBRecord_View.ItemStyle & MBV_WS_NAME) &&//显示姓名标题
            (strlen(MBRecord_View.TitleInfo.cName) > 0))
            sprintf(cBufName,pString,MBRecord_View.TitleInfo.cName,pbNameIcon.Name);
        else
            sprintf(cBufName,pString_Un,pbNameIcon.Name);
    }
    else
    {
        if ((MBRecord_View.ItemStyle & MBV_WS_NAME) &&//显示姓名标题
            (strlen(MBRecord_View.TitleInfo.cName) > 0))
            sprintf(cBufName,pString,MBRecord_View.TitleInfo.cName,MBRecord_View.cUnknow);
        else
            sprintf(cBufName,pString_Un,MBRecord_View.cUnknow);
    }
    //显示通话时间
    switch (MBRecord_View.TimeStyle)
    {
    case TS_NORMAL://年月日时分
        if ((MBRecord_View.ItemStyle & MBV_WS_TIME) &&//显示时间标题
            (strlen(MBRecord_View.TitleInfo.cTime) > 0))
        {
            sprintf(cBufTime,pTimeNormal,MBRecord_View.TitleInfo.cTime,
                pMBRecord->Time.wYear,pMBRecord->Time.wMonth, pMBRecord->Time.wDay,
                pMBRecord->Time.wHour,pMBRecord->Time.wMinute);
        }
        else
        {
            sprintf(cBufTime,pTimeNormal_Un,
                pMBRecord->Time.wYear,pMBRecord->Time.wMonth,pMBRecord->Time.wDay,
                pMBRecord->Time.wHour,pMBRecord->Time.wMinute);
        }
        break;
    case TS_NORMAL_WEEK://年月日星期时分
        GetWeekInfo(&pMBRecord->Time,cWeek);//获得星期几的文字
        if ((MBRecord_View.ItemStyle & MBV_WS_TIME) &&//显示时间标题
            (strlen(MBRecord_View.TitleInfo.cTime) > 0))
        {
            sprintf(cBufTime,pTime_Week,MBRecord_View.TitleInfo.cTime,
                pMBRecord->Time.wYear,pMBRecord->Time.wMonth,pMBRecord->Time.wDay,
                cWeek,pMBRecord->Time.wHour,pMBRecord->Time.wMinute);
        }
        else
        {
            sprintf(cBufTime,pTime_Week_Un,
                pMBRecord->Time.wYear,pMBRecord->Time.wMonth,pMBRecord->Time.wDay,
                cWeek,pMBRecord->Time.wHour,pMBRecord->Time.wMinute);
        }
        break;
    case TS_NORMAL_WEEK_M://年月日星期时分+上下午
        GetWeekInfo(&pMBRecord->Time,cWeek);//获得星期几的文字
        if ((MBRecord_View.ItemStyle & MBV_WS_TIME) &&//显示时间标题
            (strlen(MBRecord_View.TitleInfo.cTime) > 0))
        {
            sprintf(cBufTime,pTime_Week_M,MBRecord_View.TitleInfo.cTime,
                pMBRecord->Time.wYear,pMBRecord->Time.wMonth,pMBRecord->Time.wDay,
                cWeek,pMBRecord->Time.wHour,pMBRecord->Time.wMinute,
                pMBRecord->Time.wHour >= iAM_PM ? PM : AM);
        }
        else
        {
            sprintf(cBufTime,pTime_Week_M_Un,
                pMBRecord->Time.wYear,pMBRecord->Time.wMonth,pMBRecord->Time.wDay,
                cWeek,pMBRecord->Time.wHour,pMBRecord->Time.wMinute,
                pMBRecord->Time.wHour >= iAM_PM ? PM : AM);
        }
        break;
    case TS_NORMAL_M://年月日时分+上下午
        if ((MBRecord_View.ItemStyle & MBV_WS_TIME) &&//显示时间标题
            (strlen(MBRecord_View.TitleInfo.cTime) > 0))
        {
            sprintf(cBufTime,pTime_M,MBRecord_View.TitleInfo.cTime,
                pMBRecord->Time.wYear,pMBRecord->Time.wMonth, pMBRecord->Time.wDay,
                pMBRecord->Time.wHour,pMBRecord->Time.wMinute,
                pMBRecord->Time.wHour >= iAM_PM ? PM : AM);
        }
        else
        {
            sprintf(cBufTime,pTime_M_Un,
                pMBRecord->Time.wYear,pMBRecord->Time.wMonth,pMBRecord->Time.wDay,
                pMBRecord->Time.wHour,pMBRecord->Time.wMinute,
                pMBRecord->Time.wHour >= iAM_PM ? PM : AM);
        }
        break;
    }
    //显示计时
    if ((MBRecord_View.ItemStyle & MBV_WS_DURATION) && //显示计时标题
        (strlen(MBRecord_View.TitleInfo.cDuration) > 0))
    {
        sprintf(cBufTimeUsed,pTimeUsed,MBRecord_View.TitleInfo.cDuration, 
            pMBRecord->TimeUsed.wHour,pMBRecord->TimeUsed.wMinute,
            pMBRecord->TimeUsed.wSecond);
    }
    else
    {
        sprintf(cBufTimeUsed,pTimeUsed_Un,
            pMBRecord->TimeUsed.wHour,pMBRecord->TimeUsed.wMinute,
            pMBRecord->TimeUsed.wSecond);
    }

    iSW         = GetScreenUsableWH1(SM_CXSCREEN);
    iFontHeight = GetCharactorHeight(hWnd);
    CountControlsYaxis(iy,iFontHeight * 2,sizeof(iy));

    for (i = 0;i < VIEWINFONUM;i++)
        SetRect(&rRect[i],iRectX,iy[i],iSW - iRectX,iy[i] + iFontHeight * 2);

    StrAutoNewLine(hdc,cBufPhone   ,NULL,&rRect[0],DT_VCENTER|DT_LEFT);
    StrAutoNewLine(hdc,cBufName    ,NULL,&rRect[1],DT_VCENTER|DT_LEFT);
    StrAutoNewLine(hdc,cBufTime    ,NULL,&rRect[2],DT_VCENTER|DT_LEFT);
    StrAutoNewLine(hdc,cBufTimeUsed,NULL,&rRect[3],DT_VCENTER|DT_LEFT);
}
/********************************************************************
* Function   GetWeekInfo  
* Purpose    获得星期几的文字
* Params     pTime:当前时间,pString返回结果
* Return     
* Remarks      
**********************************************************************/
static  BOOL    GetWeekInfo(const SYSTEMTIME * pTime,char * pWeek)
{
    if ((pTime == NULL) || (pString == NULL))
        return FALSE;

    switch (pTime->wDayOfWeek)
    {
    case 0:
        strcpy(pWeek,SUNDAY);
        return TRUE;
    case 1:
        strcpy(pWeek,MONDAY);
        return TRUE;
    case 2:
        strcpy(pWeek,TUESDAY);
        return TRUE;
    case 3:
        strcpy(pWeek,WEDNESDAY);
        return TRUE;
    case 4:
        strcpy(pWeek,THURSDAY);
        return TRUE;
    case 5:
        strcpy(pWeek,FRIDAY);
        return TRUE;
    case 6:
        strcpy(pWeek,SATURDAY);
        return TRUE;
    default:
        return FALSE;
    }
}
