/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 打接电话部分
 *
 * Purpose  : 正在连接的界面 (由拨号界面进入的)
 *            
\**************************************************************************/
#include    "winpda.h"
#include    "window.h"
#include    "str_mb_plx.h"
#include    "hp_pdaex.h"
#include    "MBPublic.h"
#include    "hpimage.h"
#include    "stdio.h"

#define     DEBUG_MBDIALLINK
//#define     DISPLAYREDIRALTEXT//定义这个宏表示需要显示"第几次重拨"
//#define     GIFREFRESH_NORMAL       //gif动画刷新方式:一般刷新
#define     GIFREFRESH_NOTFLICKER   //gif动画刷新方式:无闪烁刷新

#define     IDC_BUTTON_MIDDLE       7//拨号键
#define     IDC_BUTTON_QUIT_LINK    3//退出按钮
#define     IDC_ME_VOICEDIAL        WM_USER + 100//语音拨号
#define     IDC_ME_CURCALLS         WM_USER + 110//列出当前所有通话
#define     IDC_ME_CUTALL           WM_USER + 120//挂断激活的通话
#define     IDC_PIC_INIT            WM_USER + 130//动画初始化
#define     IDC_TIMER_LINK          150//开始启动列电话的计时器
#define     IDC_TIMER_INIT          160//初始化信息的计时器
#define     IDC_TIMER_REDIAL        170//自动重拨用的计时器

#define     TITLECAPTION            MBGetString(STR_MBPHONE_LINKING_CAPTION)//连接中
#define     BUTTONQUIT              (LPARAM)MBGetString(STR_MBPHONE_HANGUP) //挂断
#define     BUTTON_MIDDLE_TEXT      MBGetString(STR_MBPHONE_DIALNUMBER)//拨号

#define     DIALLINKINGBUSY         MBGetString(STR_MBPHONE_BUSY)//线路忙
#define     DIALLINKINGHALT         MBGetString(STR_MBPHONE_HALT)//"连接中断"
#define     DIALLINKINGHALTFAIL     MBGetString(STR_MBPHONE_HANGUPFAIL)//"挂断失败"
#define     DIALLINKINGWAIT         MBGetString(STR_MBPHONE_WAIT)//"请稍等"
#define     DIALLING_CALLFORWARD    MBGetString(STR_MBPHONE_CALLFORWARDINGSTART)//"呼叫转移已启动"
#define     DIALLING_CALLFORWARDUN  MBGetString(STR_MBPHONE_CALLFORWARDINGUNSTART)//"无条件呼叫转移已启动"
#define     DIALLING_BARRING        MBGetString(STR_MBPHONE_DIALLING_BARRING)//限制拨出
#define     DIALLING_FARE           MBGetString(STR_MBPHONE_DIALLING_FARE)//"已到话费限制"
#define     CLEW_EMERGENT           MBGetString(STR_MBPHONE_EMERGENT)//紧急电话呼叫中...
#define     REDIALCLEW              MBGetString(STR_MBPHONE_REDIALCLEW)//"第%d次重拨"

static const int    iMinute           = 1000;
static const int    iPicY             = 5;//动画文件的纵坐标
static const char * pClassName        = "MBDialLinkingWndClass";//窗口类名
static const char * pMBLINKINGPICTURE = MBPIC_DIALLINK;//正在连接动画的文件名

static HINSTANCE    hInstance;
static char         cPhoneNumber[PHONENUMMAXLEN + 1];//电话号码
static DTMF_TYPE    dtmf_type;//当前电话号码中是否有分机号
static char         cPhoneNumberExtension[PHONENUMMAXLEN + 1];//分机号码
static char         cPhoneNumberEx[PHONENUMMAXLEN + 1];//当有分机号的时候,保存分机号之前的号码
static SYSTEMTIME   sDialupTime;//拨出时间
static int          iCalling;//保存正在语音拨号的标志,用于挂断
static MBConfig     mbconfig;//通话设置文件
static HWND         hDailLink = NULL;
static SIZE         PicSize = {0,0};
static PHONEITEM    PhoneItem;
#if defined(DISPLAYREDIRALTEXT)
static char         cRedial[50] = "";//用于显示第几次重拨
static RECT         rRedial;//显示重拨的区域
#endif
static unsigned int iRedial = 0;//用于记录当前是第几次重拨

static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  int     GetActiveCall(CALLS_INFO * Info,int nLimmit);//得激活电话的序号
static  BOOL    IfExitActiveCall(CALLS_INFO * Info,int nLimmit);
static  BOOL    AutoRedialup(void);
static  void    AfterHaltDeal(WPARAM wParam,LPARAM lParam);//连接中断之后的处理
static  BOOL    IfExistAlertingCall(CALLS_INFO * pInfo,int nLimmit);
#if defined(GIFREFRESH_NOTFLICKER)
static  void    PaintBkFunc(HGIFANIMATE hGIFAnimate,BOOL bEnd,int x,int y,HDC hdcMem);
#endif
/********************************************************************
* Function     CallMBDialLinkingWindow
* Purpose      显示正在连接的界面
* Params       pPhoneCode:拨出的号码,pDialupTime:拨出时间
* Return     
* Remarks      
**********************************************************************/
BOOL    CallMBDialLinkingWindow(const char * pPhoneCode,const SYSTEMTIME * pDialupTime)
{
    WNDCLASS    wc;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
#if defined(GIFREFRESH_NOTFLICKER)
    wc.hbrBackground = NULL;
#elif defined(GIFREFRESH_NORMAL)
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
#endif
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassName;

    if (!RegisterClass(&wc))
        return FALSE;

    if ((pPhoneCode == NULL) || (pDialupTime == NULL))
    {
        return FALSE;
    }
    else
    {
        strcpy(cPhoneNumber,pPhoneCode);
        memcpy(&sDialupTime,pDialupTime,sizeof(SYSTEMTIME));
    }

    hDailLink = CreateWindow(pClassName,TITLECAPTION, 
        WS_CAPTION|WS_BORDER |PWS_STATICBAR, 
        PLX_WIN_POSITION , NULL, NULL, NULL, NULL);

    if (NULL == hDailLink)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }

    ShowWindow(hDailLink, SW_SHOW);
    UpdateWindow(hDailLink); 

    return (TRUE);
}
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static  HGIFANIMATE hRingGif;
    static  RECT        rName;//姓名
    static  RECT        rPhone;//电话号码
    static  CALLS_INFO  calls_info[MAX_COUNT];//列出当前所有通话时,读取返回信息的结构
    static  BOOL        bHangup;//是否选择了挂断键
    static  BOOL        bListCurCalls;//是否发送完毕列当前通话的消息
    static  char        cPhone[PHONENUMMAXLEN + 20] = "";//用于显示来电号码
    static  char        cName[PHONENUMMAXLEN + 1] = "";//用于显示来电姓名    
    static  BOOL        bCanDail;//是否可以选择拨号键
            BOOL        bEmergent;//当前号码是否紧急电话号码
            int         iGsmSpecific;//GSM特殊前缀的长度
            int         iSW,iSH,iFontHeight;
            int         iPhoneNum;//当前正在通话个数
            int         nActiveCall;//获得当前被激活电话的序号
            LRESULT     lResult;
            HDC         hdc;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        bCanDail        = FALSE;//不可以选择拨号键
        bHangup         = FALSE;//没有选择挂断键
        bListCurCalls   = FALSE;//是否发送完毕列当前通话的消息
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT_LINK,BUTTONQUIT);

        GetScreenUsableWH2(&iSW,&iSH);
        //显示动画所需部分
        PostMessage(hWnd,IDC_PIC_INIT,0,0);
        //////////////////////////////////////////////////////////////////////////
        //显示姓名,号码所需部分
        iFontHeight = GetCharactorHeight(hWnd);

        SetRect(&rName,0,iPicY + PicSize.cy + 1,iSW,iPicY + PicSize.cy + 1 + iFontHeight);
        SetRect(&rPhone,0,rName.bottom,iSW,rName.bottom + iFontHeight * 2);
#if defined(DISPLAYREDIRALTEXT)
        SetRect(&rRedial,0,rPhone.bottom + 1,iSW,rPhone.bottom + 1 + iFontHeight * 2);
#endif
        //////////////////////////////////////////////////////////////////////////
        //取得来电者的信息
        memset(&PhoneItem,0x00,sizeof(PHONEITEM));
        memset(cPhoneNumberEx,0x00,sizeof(cPhoneNumberEx));
        memset(cPhoneNumberExtension,0x00,sizeof(cPhoneNumberExtension));

        iGsmSpecific = GsmSpecific(cPhoneNumber);//检测该号码是否包含GSM前缀
        //检查该号码是否包含分机号,存在分机号的话获得纯电话号码和分机号
        dtmf_type    = IfExistExtension(cPhoneNumber,cPhoneNumberEx,cPhoneNumberExtension);

        if (DTMF_NONE != dtmf_type)//有分机号,查找联系人时应该传入纯号码(除去分机号,除去GSM前缀)
        {
            GetPhoneItem(&PhoneItem,cPhoneNumberEx + iGsmSpecific);
            iCalling  = ME_VoiceDial(hWnd,IDC_ME_VOICEDIAL,cPhoneNumberEx,NULL);//语音拨号
            bEmergent = NumberIsEmergent(cPhoneNumberEx + iGsmSpecific,TRUE);//当前号码是否紧急电话号码
        }
        else//没有分机号,查找联系人时应该传入纯号码(除去GSM前缀)
        {
            GetPhoneItem(&PhoneItem,cPhoneNumber + iGsmSpecific);
            iCalling  = ME_VoiceDial(hWnd,IDC_ME_VOICEDIAL,cPhoneNumber,NULL);//语音拨号
            bEmergent = NumberIsEmergent(cPhoneNumber + iGsmSpecific,TRUE);//当前号码是否紧急电话号码
        }

        if (-1 == iCalling)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB MBDialLink语音拨号失败:%s\n",cPhoneNumber);
#endif
#ifndef _EMULATE_
            MBCallEndWindow(PhoneItem.cName,cPhoneNumber,NULL,NULL);//调用通话结束界面
            PostMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_BUTTON_QUIT_LINK, 0 );
#endif
        }

        memset(cPhone ,0x00,sizeof(cPhone ));
        memset(cName  ,0x00,sizeof(cName  ));
#if defined(DISPLAYREDIRALTEXT)
        memset(cRedial,0x00,sizeof(cRedial));
#endif
        iRedial = 0;//当前是第0次重拨
        hdc     = GetDC(hWnd);
        if (bEmergent)//当前号码是紧急电话号码,将"紧急电话呼叫中..."放在cPhone中
        {
            StrAutoNewLine(hdc,CLEW_EMERGENT,cPhone,&rPhone,NULL);
        }
        else
        {
            if (iGsmSpecific < (int)strlen(cPhoneNumber))
                StrAutoNewLine(hdc,cPhoneNumber + iGsmSpecific,cPhone,&rPhone,NULL);//显示电话
            else
                StrAutoNewLine(hdc,cPhoneNumber,cPhone,&rPhone,NULL);//显示电话

            StrAutoNewLine(hdc,PhoneItem.cName,cName,&rName,NULL);//显示姓名pbInfo.Name
        }
        ReleaseDC(hWnd,hdc);

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
        printf("\nMB MBDialLink语音拨号,显示号码:%s,长度:%d,iCalling:%d\n",
            cPhone,strlen(cPhone),iCalling);
#endif
        SetTimer(hWnd,IDC_TIMER_INIT,1000,NULL);//设置初始化的计时器

        if (GetUsedItemNum() == 0)//当前没有通话
            DisableState();

        break;

    case IDC_PIC_INIT:
        hRingGif = StartAnimatedGIFFromFile(hWnd,pMBLINKINGPICTURE,//居中显示
            (GetScreenUsableWH1(SM_CXSCREEN) - PicSize.cx) / 2,iPicY,DM_NONE);
#if defined(GIFREFRESH_NOTFLICKER)
        SetPaintBkFunc(hRingGif, PaintBkFunc);
        hdc = GetDC(hWnd);
        PaintAnimatedGIF(hdc, hRingGif);
        ReleaseDC(hWnd, hdc);
#endif
        InvalidateRect(hWnd,NULL,TRUE);
        break;

    case WM_ACTIVATE:
        if (WA_INACTIVE != LOWORD(wParam))
            SetFocus(hWnd);//激活后设置焦点

        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;

    case WM_TIMER:
        switch (wParam)
        {
        case IDC_TIMER_INIT://初始化信息的计时器
            KillTimer(hWnd,IDC_TIMER_INIT);
            ReadMobileConfig(&mbconfig);//读取通话设置的信息

            if (GetForward_Uncdtional())//考查是否存在无条件转移
                AppMessageBox(hWnd,DIALLING_CALLFORWARDUN,TITLECAPTION, WAITTIMEOUT);
            else if (GetForward_Condition())//考查是否存在条件转移
                AppMessageBox(hWnd,DIALLING_CALLFORWARD,TITLECAPTION, WAITTIMEOUT);

            break;

        case IDC_TIMER_LINK:
            KillTimer(hWnd,IDC_TIMER_LINK);

            bListCurCalls = TRUE;//已经发送列当前通话的消息
            SetListCurCalls(TRUE);//设置是否正在列当前通话

            if (-1 == ME_ListCurCalls(hWnd,IDC_ME_CURCALLS))//列出当前所有通话
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
                printf("\nMB MBDialLink,ME_ListCurCalls失败\n");
#endif
                bListCurCalls = FALSE;//发送列当前通话的消息完毕
                SetListCurCalls(FALSE);//设置是否正在列当前通话
                AfterHaltDeal(wParam,lParam);
                MBCallEndWindow(PhoneItem.cName,cPhoneNumber,NULL,NULL);//调用通话结束界面
            }
            break;

        case IDC_TIMER_REDIAL://自动重拨用的计时器
            KillTimer(hWnd,IDC_TIMER_REDIAL);
            if (DTMF_NONE != dtmf_type)//有分机号
            {
                iCalling = ME_VoiceDial(hDailLink,IDC_ME_VOICEDIAL,cPhoneNumberEx,NULL);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
                printf("mb MBDialLink 自动重拨:%s\r\n",cPhoneNumberEx);
#endif
            }
            else//没有分机号
            {
                iCalling = ME_VoiceDial(hDailLink,IDC_ME_VOICEDIAL,cPhoneNumber,NULL);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
                printf("mb MBDialLink 自动重拨:%s\r\n",cPhoneNumber);
#endif
            }

            if (-1 == iCalling)
            {
                MBCallEndWindow(PhoneItem.cName,cPhoneNumber,NULL,NULL);//调用通话结束界面
                PostMessage(hDailLink, WM_COMMAND, (WPARAM)IDC_BUTTON_QUIT_LINK, 0 );
            }
            break;
        }
        break;

    case WM_CLOSE://点击右上角的退出按钮时为保存退出;
        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_BUTTON_QUIT_LINK, 0 );
        break;

    case WM_DESTROY ://通知程序管理器,此程序退出;
        hDailLink       = NULL;
        iCalling        = 0;
        bHangup         = FALSE;
        bListCurCalls   = FALSE;
        if (hRingGif != NULL)
            EndAnimatedGIF(hRingGif);
        hRingGif = NULL;

        UnregisterClass(pClassName,NULL);
        DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)hInstance);
        if (GetUsedItemNum() == 0)//当前没有通话
            EnableState();

        break;

    case WM_PAINT :
        hdc = BeginPaint( hWnd, NULL);
#if defined(GIFREFRESH_NOTFLICKER)
        {
            HDC hMemDC;
            RECT rect;
            GetScreenUsableWH2(&iSW,&iSH);
            SetRect(&rect,0,0,iSW,iSH);
            hMemDC = CreateMemoryDC(rect.right,rect.bottom);
            ClearRect(hMemDC, &rect, RGB(255, 255, 255));

            PaintAnimatedGIF(hMemDC, hRingGif);
            SetBkMode(hMemDC, BM_TRANSPARENT);
            BitBlt(hdc, 0, 0,rect.right,rect.bottom,hMemDC, 0, 0, SRCCOPY);
            DeleteDC(hMemDC);
            hMemDC = NULL;
        }
#elif defined(GIFREFRESH_NORMAL)
        PaintAnimatedGIF(hdc, hRingGif);
#endif
        DrawText(hdc,cName  ,strlen(cName)  ,&rName  ,DT_CENTER|DT_VCENTER);
        DrawText(hdc,cPhone ,strlen(cPhone) ,&rPhone ,DT_CENTER|DT_VCENTER);//显示电话
#if defined(DISPLAYREDIRALTEXT)
        DrawText(hdc,cRedial,strlen(cRedial),&rRedial,DT_CENTER|DT_VCENTER);//显示第几次重拨
#endif
        EndPaint(hWnd, NULL);
        break;

    case WM_TODESKTOP:
        if (0 == wParam)//程序管理器自动发送
            return FALSE;
        SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT_LINK,0);
        return FALSE;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        //case VK_EARPHONE_KEY://耳机上的按键
        case VK_F10:
        case VK_F2://挂断
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT_LINK,0);
            break;
        case VK_0:  //按了数字键0
        case VK_1:  //按了数字键1
        case VK_2:  //按了数字键2
        case VK_3:  //按了数字键3
        case VK_4:  //按了数字键4
        case VK_5:  //按了数字键5
        case VK_6:  //按了数字键6
        case VK_7:  //按了数字键7
        case VK_8:  //按了数字键8
        case VK_9:  //按了数字键9
            if (bCanDail)//可以选择拨号键
            {
                char chSend[2] = "";
                chSend[0] = (LOWORD(wParam) - VK_0 + 48);
                SuperAddDialNumber(chSend,FALSE,TRUE);
            }
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        case IDC_BUTTON_MIDDLE://拨号键
            SuperAddDialNumber(NULL,FALSE,TRUE);
            break;
        case IDC_BUTTON_QUIT_LINK://挂断
            bHangup = TRUE;//选择挂断键
            if (bListCurCalls)//正在发送列当前通话的消息
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
                printf("\nMB MBDialLink选择了挂断键,正在发送列当前通话的消息\n");
#endif
                AppMessageBox(hWnd,DIALLINKINGWAIT,TITLECAPTION, WAITTIMEOUT);
                return TRUE;
            }
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB MBDialLink,ME_CancelFunction,iCalling:%d\n",iCalling);
#endif
            if (-1 == ME_CancelFunction(iCalling))
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
                printf("\nMB MBDialLink,挂断失败,挂断激活的通话\n");
#endif
                if (-1 == ME_SwitchCallStatus(hWnd,IDC_ME_CUTALL,CUT_ACTIVED,0))//挂断激活的通话
                {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
                    printf("\nMB MBDialLink,ME_SwitchCallStatus,挂断激活的通话失败\n");
#endif
#if defined(_EMULATE_)
                    DestroyWindow(hWnd);
#endif
                }
                return TRUE;
            }
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB MBDialLink,挂断\n");
#endif
            AfterHaltDeal(wParam,lParam);
            MBCallEndWindow(PhoneItem.cName,cPhoneNumber,NULL,NULL);//调用通话结束界面
            break;
        }
        break;
    //**************************ME回调消息*************************
    case IDC_ME_VOICEDIAL://语音拨号,拨号返回
        switch (wParam)
        {
        case ME_RS_SUCCESS://成功
            SetTimer(hWnd,IDC_TIMER_LINK,500,NULL);
            break;

        case ME_RS_USERSTOP://用户中断操作成功,取消拨号
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("mb MBDialLink 用户中断操作成功,取消拨号\r\n");
#endif
            AfterHaltDeal(wParam,lParam);
            MBCallEndWindow(PhoneItem.cName,cPhoneNumber,NULL,NULL);//调用通话结束界面
            break;

        case ME_RS_BUSY://线路忙
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB DialLink线路忙\n");
#endif
            if (!AutoRedialup())//自动重拨
            {
                AfterHaltDeal(wParam,lParam);
                MBCallEndWindow(PhoneItem.cName,cPhoneNumber,NULL,NULL);//调用通话结束界面
            }
            break;

        case ME_RS_NOCARRIER://通话中断
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB DialLink通话中断,wParam:%ld,lParam:%ld\n",wParam,lParam);
#endif
            switch (lParam)
            {
            case ME_E_CALLBARRED://表示当前呼叫限制激活无法拨出电话
                AfterHaltDeal(wParam,lParam);
                MBCallEndWindow(PhoneItem.cName,DIALLING_BARRING,NULL,NULL);//调用通话结束界面
                break;
            case ME_E_MAXACM://计费限额已超
                AfterHaltDeal(wParam,lParam);
                MBCallEndWindow(PhoneItem.cName,cPhoneNumber,NULL,DIALLING_FARE);//调用通话结束界面
                break;
            default:
                if (!AutoRedialup())//自动重拨
                {
                    AfterHaltDeal(wParam,lParam);
                    MBCallEndWindow(PhoneItem.cName,cPhoneNumber,NULL,NULL);//调用通话结束界面
                }
                break;
            }
            break;

        default:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB DialLink,语音拨号,拨号返回default,wParam:%ld,lParam:%ld\n",wParam,lParam);
#endif
            SendAbortCommand(wParam,lParam);//发送取消操作的at命令
            switch (lParam)
            {
            case ME_E_CALLBARRED://表示当前呼叫限制激活无法拨出电话
                AfterHaltDeal(wParam,lParam);
                MBCallEndWindow(PhoneItem.cName,DIALLING_BARRING,NULL,NULL);//调用通话结束界面
                break;
            case ME_E_MAXACM://计费限额已超
                AfterHaltDeal(wParam,lParam);
                MBCallEndWindow(PhoneItem.cName,cPhoneNumber,NULL,DIALLING_FARE);//调用通话结束界面
                break;
            default:
                if (!AutoRedialup())//自动重拨
                {
                    AfterHaltDeal(wParam,lParam);
                    MBCallEndWindow(PhoneItem.cName,cPhoneNumber,NULL,NULL);//调用通话结束界面
                }
                break;
            }
            break;
        }
        break;

    case IDC_ME_CURCALLS://列出当前所有通话,接听
        bListCurCalls = FALSE;//发送列当前通话的消息完毕
        SetListCurCalls(FALSE);//设置是否正在列当前通话
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
        printf("\nMB MBDialLink列出当前所有通话,wParam=%ld\n",wParam);
#endif
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            if (bHangup)//选择了挂断键
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
                printf("\nMB MBDialLink选择了挂断键\n");
#endif
                SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT_LINK,0);//发送按下挂断键的消息
                return TRUE;
            }
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB MBDialLink列出当前所有通话\n");
#endif
            memset(calls_info,0x00,sizeof(calls_info));
            iPhoneNum = ME_GetResult(calls_info,sizeof(calls_info));

            if (iPhoneNum == -1)//出错了,需要挂断激活的通话
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
                printf("MB MBDialLink,iPhoneNum == -1\r\n");
#endif
                if (!AutoRedialup())//当前不是自动重拨
                {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
                    printf("mb MBDialLink iPhoneNum == -1\r\n");
#endif
                    ME_SwitchCallStatus(hWnd,IDC_ME_CUTALL,CUT_ACTIVED,0);//挂断激活的通话
                    AfterHaltDeal(wParam,lParam);

                    MBCallEndWindow(PhoneItem.cName,cPhoneNumber,NULL,NULL);//调用通话结束界面
                }
                return TRUE;
            }

            iPhoneNum = iPhoneNum / sizeof(CALLS_INFO);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB MBDialLink,iPhoneNum:%d\n",iPhoneNum);
#endif
            //如果电话正常拨出,那么clcc结构的个数应该多于当前通话个数
            if (iPhoneNum <= GetUsedItemNum())
            {
                break;
            }
            nActiveCall = GetActiveCall(calls_info,iPhoneNum);//获得当前被激活电话的序号
            if (-1 == nActiveCall)//当前出错了,要考察该电话是否还在,在的话重列通话列表
            {
                bCanDail = IfExistAlertingCall(calls_info,iPhoneNum);//是否可以选择拨号键
#if defined(SUPPORTMIDDLEBUTTON)
                SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,//拨号键
                    MAKEWPARAM(IDC_BUTTON_MIDDLE,2), 
                    bCanDail ? (LPARAM)BUTTON_MIDDLE_TEXT : (LPARAM)"");
#endif
                if (!AutoRedialup())//当前不是自动重拨
                {
                    if (!IfExitActiveCall(calls_info,iPhoneNum))
                    {
                        //当前电话号码不在结构数组中,该电话不通或者占线
                        ME_SwitchCallStatus(hWnd,IDC_ME_CUTALL,CUT_ACTIVED,0);//挂断激活的通话

                        PostMessage(hWnd,IDC_ME_VOICEDIAL,ME_RS_NOCARRIER,0);//连接中断
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
                        printf("\nMB MBDialLink,IfExitActiveCall\n");
#endif
                        return TRUE;
                    }
                    SetTimer(hWnd,IDC_TIMER_LINK,500,NULL);
//                    bListCurCalls = TRUE;//已经发送列当前通话的消息
//                    SetListCurCalls(TRUE);//设置是否正在列当前通话
//                    if (-1 == ME_ListCurCalls(hWnd,IDC_ME_CURCALLS))//重列当前通话
//                    {
//                        bListCurCalls = FALSE;//发送列当前通话的消息完毕
//                        SetListCurCalls(FALSE);//设置是否正在列当前通话
//                        ME_SwitchCallStatus(hWnd,IDC_ME_CUTALL,CUT_ACTIVED,0);//挂断激活的通话
//                        PostMessage(hWnd,IDC_ME_VOICEDIAL,ME_RS_NOCARRIER,0);//连接中断
//#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
//                        printf("\nMB MBDialLink,nActiveCall==-1且ME_ListCurCalls失败\n");
//#endif
//                    }
//#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
//                    printf("\nMB MBDialLink,当前是单方通话界面,nActiveCall=-1\n");
//#endif
                }
                return TRUE;
            }
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB MBDialLink,当前电话号码:%s,%d\n",calls_info[nActiveCall].PhoneNum,strlen(calls_info[nActiveCall].PhoneNum));
#endif

            if (mbconfig.bGetClew)//播放接通提示音
                PlayClewMusic(1);

            if (strcmp(calls_info[nActiveCall].PhoneNum,cPhoneNumber) != 0)
            {
                char cCall[200] = "";
                sprintf(cCall,"拨号错误,列出的号码%s,原号码:%s\r\n",calls_info[nActiveCall].PhoneNum,cPhoneNumber);
                SaveLog(cCall);
            }
            memcpy(&PhoneItem.Calls_Info,&calls_info[nActiveCall],sizeof(CALLS_INFO));
            memcpy(&PhoneItem.TimeInDial,&sDialupTime,sizeof(SYSTEMTIME));
            MBCallingWindow(&PhoneItem,cPhoneNumberExtension,dtmf_type);//进入通话界面
            DestroyWindow(hWnd);
            break;

        default://可能是连接中断
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB MBDialLink,列出当前所有通话返回default,wParam:%ld,bHangup:%d\n",wParam,bHangup);
#endif
            if (bHangup)//选择了挂断键
            {
                SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT_LINK,0);//发送按下挂断键的消息
                return TRUE;
            }
            if (!AutoRedialup())//当前不是自动重拨
            {
                AfterHaltDeal(wParam,lParam);
                MBCallEndWindow(PhoneItem.cName,cPhoneNumber,NULL,NULL);//调用通话结束界面
            }
            break;
        }
        break;

    case IDC_ME_CUTALL://挂断激活的通话
        switch(wParam)
        {
        case ME_RS_SUCCESS:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB MBDialLink,IDC_ME_CUTALL挂断激活的通话\n");
#endif
            AfterHaltDeal(wParam,lParam);
            break;
        default:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB MBDialLink,IDC_ME_CUTALL挂断激活的通话失败\n");
#endif
            SendAbortCommand(wParam,lParam);//发送取消操作的at命令
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
* Function     AutoRedialup
* Purpose      自动重拨
* Params       
* Return     
* Remarks      
**********************************************************************/
static  BOOL    AutoRedialup(void)
{
    if ((!mbconfig.bAutoRedial) || (mbconfig.iAutoRedial <= 0))//不是自动重拨
        return FALSE;

    iRedial++;
    if (iRedial > (unsigned int)mbconfig.iAutoRedial)//到了自动重拨的最大次数
        return FALSE;

#if defined(DISPLAYREDIRALTEXT)
    sprintf(cRedial,REDIALCLEW,iRedial);
    InvalidateRect(hDailLink,NULL,TRUE);
#endif

    //CTA测试的时候要求固定每次自动重拨的时间间隔
    //第1次重拨间隔5S
    //第2次重拨间隔1MIN
    //第3次重拨间隔1MIN
    //第4次重拨间隔1MIN
    //第5次重拨间隔3MIN
    //.................
    //第N次重拨间隔3MIN
    if (1 == iRedial)
        SetTimer(hDailLink,IDC_TIMER_REDIAL,5 * iMinute,NULL);
    else if ((2 == iRedial) || (3 == iRedial) || (4 == iRedial))
        SetTimer(hDailLink,IDC_TIMER_REDIAL,60 * iMinute,NULL);
    else
        SetTimer(hDailLink,IDC_TIMER_REDIAL,180 * iMinute,NULL);

    return TRUE;
}
/********************************************************************
* Function     SendMBDialLinkDialInfo
* Purpose      发送连接结果给拨号回调消息
* Params       iType:连接结果,比如NOCARRIER等
* Return     
* Remarks      
**********************************************************************/
void    SendMBDialLinkDialInfo(int iType)
{
    PostMessage(hDailLink,IDC_ME_VOICEDIAL,iType,0);
}
/********************************************************************
* Function     GetActiveCall
* Purpose      获得当前被激活电话的序号
* Params       
* Return     -1表示没有找到
* Remarks      
**********************************************************************/
static  int     GetActiveCall(CALLS_INFO * Info,int nLimmit)
{
    int i;

    for (i = 0;i < nLimmit;i++)
    {
        if (CALL_STAT_ACTIVE == Info[i].Stat)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB MBDialLink,GetActiveCall,号码:%s,i:%d\n",Info[i].PhoneNum,i);
#endif
            return i;
        }
    }
    return -1;
}
/********************************************************************
* Function     IfExitActiveCall
* Purpose      当前电话号码是否在结构数组中
* Params       
* Return     
* Remarks      
**********************************************************************/
static  BOOL    IfExitActiveCall(CALLS_INFO * Info,int nLimmit)
{
    int i;
    for (i = 0;i < nLimmit;i++)
    {
        if (dtmf_type == DTMF_NONE)
        {
            if (strcmp(Info[i].PhoneNum,cPhoneNumber) == 0)
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
                printf("\nMB MBDialLink,IfExitActiveCall,号码:%s,i:%d\n",Info[i].PhoneNum,i);
#endif
                return TRUE;
            }
        }
        else
        {
            if (strcmp(Info[i].PhoneNum,cPhoneNumberEx) == 0)
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
                printf("\nMB MBDialLink,IfExitActiveCall,号码:%s,i:%d\n",Info[i].PhoneNum,i);
#endif
                return TRUE;
            }
        }
    }
    return FALSE;
}
/********************************************************************
* Function     IfExistAlertingCall
* Purpose      当前电话信息中是否存在正在振铃的电话
* Params       
* Return     
* Remarks      
**********************************************************************/
static  BOOL    IfExistAlertingCall(CALLS_INFO * pInfo,int nLimmit)
{
    int i;
    for (i = 0;i < nLimmit;i++)
    {
        if (pInfo[i].Stat == CALL_STAT_ALERTING)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("MB MBDialLink IfExistAlertingCall:%s is alerting\r\n",pInfo[i].PhoneNum);
#endif
            return TRUE;
        }
    }
    return FALSE;
}
/********************************************************************
* Function     AfterHaltDeal
* Purpose      连接中断之后的处理
* Params       
* Return     
* Remarks      
**********************************************************************/
static  void    AfterHaltDeal(WPARAM wParam,LPARAM lParam)
{
    MBListCurCalls(LISTCURCALLS_REPEAT,wParam,lParam);//重列当前通话
    DestroyWindow(hDailLink);
}
/********************************************************************
* Function     IsWindow_DialLink
* Purpose      当前是否存在连接界面
* Params       
* Return     
* Remarks      
**********************************************************************/
BOOL    IsWindow_DialLink(void)
{
    if (hDailLink == NULL)
        return FALSE;
    return (IsWindow(hDailLink));
}
void    bInitMBLink(void)//初始化连接
{
    GetImageDimensionFromFile(pMBLINKINGPICTURE,&PicSize);//获得动画文件尺寸
}
//////////////////////////////////////////////////////////////////////////
#if defined(GIFREFRESH_NOTFLICKER)
// 画gif的回调函数，刷新背景
static void PaintBkFunc(HGIFANIMATE hGIFAnimate,BOOL bEnd,int x,int y,HDC hdcMem)
{
    /*int width, height;
    RECT   rect;
    width = GetDeviceCaps(hdcMem, HORZRES);
    height = GetDeviceCaps(hdcMem, VERTRES);
    rect.bottom = height;
    rect.left = 0;
    rect.top = 0;
    rect.right = width;
    ClearRect(hdcMem, &rect, RGB(255, 255, 255));*/
}
#endif
