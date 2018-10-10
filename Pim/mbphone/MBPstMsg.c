/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 打电话
 *
 * Purpose  : 底层消息的分发
 *            
\**************************************************************************/
#include "winpda.h"  
#include "string.h"
#include "malloc.h"
#include "stdlib.h"
#include "stdio.h" 
#include "str_mb_plx.h"
#include "MBPublic.h"
#include "../mbrecord/RcCommon.h"
#include "hp_pdaex.h"

#define DEBUG_MBPSTMSG
//#define HOOK

#define TIMER_CALLING_TIMEOUT       3000
#define TIMER_RINGING_TIMEOUT       3000

#define WM_URC_NOCARRIER            WM_USER + 200
#define WM_URC_CSSU                 WM_USER + 201
#define WM_URC_WIND                 WM_USER + 202
#define WM_URC_CLIP                 WM_USER + 203
#define WM_URC_CCWA                 WM_USER + 204
#define WM_URC_RING                 WM_USER + 205
#define WM_URC_CRING                WM_USER + 213
#define WM_LIST_CALLS_NOCAR_CCSU    WM_USER + 206 
#define WM_LIST_CALLS_CALLING       WM_USER + 207
#define WM_LIST_CALLS_WIND          WM_USER + 208//目前没有使用
#define WM_LIST_CALLS_CCWA_CLIP     WM_USER + 209
#define WM_LIST_CALLS_RING          WM_USER + 210
#define WM_LIST_CALLS_RINGING       WM_USER + 211
#define WM_LIST_CALLS_CALLWAITOVER  WM_USER + 212

#define IDC_GETCALLWAIT             WM_USER + 110//查询呼叫等待
#define IDC_SETCALLWAIT             WM_USER + 120//设置呼叫等待

#define IDC_BEGIN_SEARCH_CF         WM_USER + 180//收到这个注册的消息之后,开始查询呼叫转移
#define IDC_GETFORWAR_DUNCDTIONAL   WM_USER + 130//查询无条件转移
#define IDC_GETFORWAR_BUSY          WM_USER + 140//查询遇忙转移
#define IDC_GETFORWAR_NOREPLY       WM_USER + 150//查询无应答转移
#define IDC_GETFORWAR_NOREACH       WM_USER + 160//查询不在服务区或关机转移
#define IDC_GETLASTERROR            WM_USER + 170//获取通话中断原因

//对方拨打电话时的超时Timer,即对方主动挂断
#define CALLING_TIMEROUT_TIMER_ID   WM_USER + 1
#define RINGING_TIMEROUT_TIMER_ID   WM_USER + 2

typedef struct
{
    int iUrc;
    int iMsg;
}ME_URCINFO;

static const ME_URCINFO Me_UrcInfo[] =
{
#ifdef SIEMENS_TC35
    {ME_URC_CRING       ,WM_URC_CRING},     //有来电(标识来电类型)
    {ME_URC_RING        ,WM_URC_RING},      //有来电(标识来电类型)
    {ME_URC_NOCARRIER   ,WM_URC_NOCARRIER}, //通话被释放
    {ME_URC_CSSU        ,WM_URC_CSSU},      //保持通话被释放
    {ME_URC_CCWA        ,WM_URC_CCWA},      //设置呼叫等待后有来电
#endif
#ifdef WAVECOM_2C
    {ME_URC_CRING       ,WM_URC_CRING},     //有来电(标识来电类型)
    {ME_URC_RING        ,WM_URC_RING},      //有来电(标识来电类型)
    {ME_URC_NOCARRIER   ,WM_URC_NOCARRIER}, //通话被释放          
    {ME_URC_CSSU        ,WM_URC_CSSU},      //保持通话被释放      
    {ME_URC_CCWA        ,WM_URC_CCWA},      //设置呼叫等待后有来电 
    {ME_URC_WIND        ,WM_URC_WIND},
#endif
#ifdef TI_MODULE
    {ME_URC_CRING       ,WM_URC_CRING},     //有来电(标识来电类型)
    {ME_URC_RING        ,WM_URC_RING},      //有来电(标识来电类型)
    {ME_URC_NOCARRIER   ,WM_URC_NOCARRIER}, //通话被释放          
    {ME_URC_CSSU        ,WM_URC_CSSU},      //保持通话被释放      
    {ME_URC_CCWA        ,WM_URC_CCWA},      //设置呼叫等待后有来电 
#endif
    {-1,-1},
};
#define     CALLFORWARDING          MBGetString(STR_MBPHONE_CALLFORWARDING_CLEW)//转接来电

static const char * pClassName = "MBPostMsgWndClass";
static HWND         MB_hWndPostMsg;//分发消息窗口句柄
static CALLS_INFO   calls_info[MAX_COUNT + 1];//420个字节

#ifndef _EMULATE_
static BOOL         bInitOver = FALSE;//初始化是否完成
#else
static BOOL         bInitOver = TRUE ;//模拟环境认为初始化已经完成
#endif

static BOOL         bSearchCF = FALSE;//标志网络切换的时候是否已经查询过呼叫转移了
static BOOL         bCalling;//接电话的界面是否已经出现
static BOOL         bRinging;//由于来电之后,来电消息会不停的上来,这时候需要只处理一次,需要标志一下
static BOOL         bCFClew = FALSE;//由于CSSU消息会和RING消息一样不断的上来,所以这时候需要只处理一次,需要标志一下
// 内部函数声明 
static  LRESULT CALLBACK MBPostMsgWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static  LRESULT CALLBACK MBRecPhoneCall_KeyBoardProc(DWORD nUser,WPARAM wParam,LPARAM lParam);
static  BOOL    OnCreate(HWND hWnd);
static  void    OnDestroy(HWND hWnd);
static  void    OnURCWIND(HWND hWnd, WPARAM wParam, LPARAM lParam);

static  void    OnListCallsInTimerCalling(HWND hWnd, WPARAM wParam, LPARAM lParam);
static  void    OnListCallsInWind(HWND hWnd, WPARAM wParam, LPARAM lParam);
static  void    OnListCallsInCcwaAndClip(HWND hWnd, WPARAM wParam, LPARAM lParam);
static  void    OnListCallsInRing(HWND hWnd, WPARAM wParam, LPARAM lParam);
static  void    OnListCallsInTimerRinging(HWND hWnd, WPARAM wParam, LPARAM lParam);
/********************************************************************
* Function     InitMBPostMsg
* Purpose      初始化消息分发隐藏窗口
* Params       
* Return           
* Remarks      
**********************************************************************/
BOOL    InitMBPostMsg(void)
{
    WNDCLASS wc;

    if (IsWindow(MB_hWndPostMsg))
        return TRUE;

    wc.style         = 0;
    wc.lpfnWndProc   = MBPostMsgWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassName;

    if (!RegisterClass(&wc))
        return FALSE;

    MB_hWndPostMsg = CreateWindow(pClassName,"",WS_CAPTION,0,0,100,100,NULL,NULL,NULL,NULL);

    return (MB_hWndPostMsg != NULL);
}
/********************************************************************
* Function     MBPostMsgWndProc
* Purpose      隐藏窗口消息处理程序
* Params       
* Return           
* Remarks      
**********************************************************************/
static LRESULT CALLBACK MBPostMsgWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT         lResult;
    CALLFWD_INFO    callfwd_info;
    unsigned long   lLastError;

    lResult = (LRESULT)TRUE;

    switch (message)
    {
    case WM_CREATE:
        bRinging = FALSE;
        lResult = (LRESULT)OnCreate(hWnd);
#if defined(HOOK)
        {
            /*PMHOOK  pmhook;
            pmhook.f_CallBack   = MBRecPhoneCall_KeyBoardProc;
            pmhook.v_nUser      = 0;
            pmhook.v_nType      = PMS_HOOK_KEY_IDLE;
            DlmNotify(PMM_NEWS_REGISTER_HOOK, (LPARAM)&pmhook);*/

            SetWindowsHookEx(WH_KEYBOARD,MBRecPhoneCall_KeyBoardProc,NULL,NULL);
        }
#endif
        CreateMBGSMWindow();//初始化GSM部分
        InitMBCalling();    //初始化通话界面所需图片信息
        bInitMBDialup();    //初始化拨号
        bInitMBLink();      //初始化连接
        ME_GetCallWaitingStatus(hWnd,IDC_GETCALLWAIT,CLASS_VOICE);
        //发现西门子无线模块如果不在启动的时候设置一下呼叫等待,
        //那么通话的时候,将收不到来电的消息,所以在启动的时候强行设置
        //一下呼叫等待.
        break;

    case WM_DESTROY:
        OnDestroy(hWnd);
        break;

    case WM_URC_NOCARRIER://注册的消息,通话被释放
#if defined(SIEMENS_TC35)

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\nMB Phone receive NOCARRIER,首先获取通话中断原因\n");
#endif

        if (IsWindow_DialLink())
            SendMBDialLinkDialInfo(ME_URC_NOCARRIER);
        else
            ME_GetLastError(hWnd,IDC_GETLASTERROR);//首先获取通话中断原因
#endif
        break;

    case WM_URC_CSSU://注册的消息,保持通话被释放
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\nMB Phone receive CSSU!\n");
#endif
        if (IsWindow_MBRing() && (!bCFClew))//当前是接听界面,且CSSU消息是第一次上来
        {
            ME_CSSU_ITEM me_cssu_item;
            if (-1 != ME_GetResult(&me_cssu_item,sizeof(ME_CSSU_ITEM)))
            {
                if (me_cssu_item.Action == 0)//该电话是别的手机转移过来的,不知道号码
                {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
                    printf("mb 该电话是别的手机转移过来的\r\n");
#endif
                    MessageMBRing(IDC_FORWARDING,NULL,NULL);
                    AppMessageBox(NULL,CALLFORWARDING,"",WAITTIMEOUT);
                    bCFClew = TRUE;
                    break;
                }
            }
        }
#if defined(SIEMENS_TC35)
        SetListCurCalls(TRUE);//设置是否正在列当前通话
        if (-1 == ME_ListCurCalls(hWnd,WM_LIST_CALLS_NOCAR_CCSU))//列出当前所有通话
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("\nMB Phone receive CSSU==-1!\n");
#endif
        }
#endif
        break;

    case WM_URC_CCWA://注册的消息,设置呼叫等待后有来电
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\nMB Phone receive CCWA!\n");
#endif
        if (-1 == ME_ListCurCalls(hWnd,WM_LIST_CALLS_CCWA_CLIP))//列出当前所有通话
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("\nMB Phone receive CCWA==-1!\n");
#endif
        }
        break;

    case WM_URC_CRING://注册的消息,有来电(标识来电类型)
    case WM_URC_RING://注册的消息,有来电(标识来电类型)
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\nMB Phone receive RING!bRinging:%d\r\n",bRinging);
#endif
        if (!bRinging)
        {
            bRinging = TRUE;
            if (-1 == ME_ListCurCalls(hWnd,WM_LIST_CALLS_RING))//列出当前所有通话
            {
                bRinging = FALSE;
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
                printf("\nMB ME_ListCurCalls==-1\r\n");
#endif
            }
        }
        break;

    case WM_URC_CLIP://注册的消息,开关机盖
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\nMB Phone receive CLIP!\n");
#endif
        if (-1 == ME_ListCurCalls(hWnd,WM_LIST_CALLS_CCWA_CLIP))//列出当前所有通话
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("\nMB Phone receive CLIP==-1!\n");
#endif
        }
        break;

    case WM_URC_WIND://注册的消息, 处理+WIND:6消息(挂断),仅在wavecom模块中使用
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\nMB Phone receive WIND\r\n");
#endif
        OnURCWIND(hWnd,(WPARAM)wParam,(LPARAM)lParam);//处理+WIND:6消息(挂断),仅在wavecom模块中使用
        break;

    case WM_LIST_CALLS_RINGING://来电的时候,接电话的timer,列出当前所有通话
        OnListCallsInTimerRinging(hWnd, wParam, lParam);
        break;

    case WM_LIST_CALLS_RING://有来电(标识来电类型),列出当前所有通话的回调消息
        OnListCallsInRing(hWnd, wParam, lParam);//会调用接电话的函数
        break;

    case WM_LIST_CALLS_NOCAR_CCSU://注册的消息(通话被释放/保持通话被释放),列出当前所有通话
        OnListCallsInWind(hWnd, wParam, lParam);
        //收到no carrier和ccsu后列当前电话,根据情况处理
        break;

    case WM_TIMER:
        switch (wParam)
        {
        case CALLING_TIMEROUT_TIMER_ID://呼叫等待启动后有来电,接电话之后会启动本计时器
            if (-1 == ME_ListCurCalls(hWnd,WM_LIST_CALLS_CALLING))//列出当前所有通话
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
                printf("\nMB CALLING_TIMEROUT_TIMER_ID == -1!\n");
#endif
            }
            break;
        case RINGING_TIMEROUT_TIMER_ID://呼叫等待关闭候有来电的时候,接电话
            if (-1 == ME_ListCurCalls(hWnd,WM_LIST_CALLS_RINGING))//列出当前所有通话
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
                printf("\nMB RINGING_TIMEROUT_TIMER_ID == -1!\n");
#endif
            }
            break;
        }   
        break;

    case WM_LIST_CALLS_CALLING://接电话的timer,列出当前所有通话
        OnListCallsInTimerCalling(hWnd, wParam, lParam);
        //在对方拨打电话未接听的情况每一段时间列当前电话,根据情况处理
        break;

    case WM_LIST_CALLS_WIND://列出当前所有通话, 处理+WIND:6消息(挂断),仅在wavecom模块中使用
        OnListCallsInWind(hWnd, wParam, lParam);
        break;

    case WM_LIST_CALLS_CCWA_CLIP://设置呼叫等待后有来电,列出当前所有通话
        OnListCallsInCcwaAndClip(hWnd, wParam, lParam);
        //收到ccwa和clip后列当前电话,根据情况处理,会调用接电话的函数
        break;

    case WM_CALLING_HANGUP:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\nMB MBPstMsg receive WM_CALLING_HANGUP\n");
#endif
        KillTimer(hWnd, RINGING_TIMEROUT_TIMER_ID);
        KillTimer(hWnd, CALLING_TIMEROUT_TIMER_ID);
        bCalling = FALSE;
        bRinging = FALSE;

        bCFClew  = FALSE;
        break;

    case IDC_GETLASTERROR://获取通话中断原因
        ME_GetResult(&lLastError,sizeof(lLastError));

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("MB 获取通话中断原因,wParam:%ld,lParam:%ld,lLastError:%lx\r\n",wParam,lParam,lLastError);
#endif

        switch (lLastError)
        {
        case ME_E_MAXACM://表示计费限额已超
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("MB 获取通话中断原因,计费限额已超\r\n");
#endif
            SetCostLimit(TRUE);//设置当前通话已到话费限制
            break;
        }

        SetListCurCalls(TRUE);//设置是否正在列当前通话
        if (-1 == ME_ListCurCalls(hWnd,WM_LIST_CALLS_NOCAR_CCSU))//列出当前所有通话
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("\nMB Phone receive NOCARRIER==-1\n");
#endif
        }
        break;
    //////////////////////////////////////////////////////////////////////////
    case IDC_GETCALLWAIT://查询呼叫等待
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            {
                BOOL bCallWait = FALSE;
                if (ME_GetResult(&bCallWait,sizeof(BOOL)) != -1)
                {
                    ME_SetCallWaitingStatus(hWnd,IDC_SETCALLWAIT,bCallWait,CLASS_VOICE);
                }
            }
            break;
        }
        break;
    case IDC_SETCALLWAIT://查询呼叫转移
        ME_GetCallForwardStatus(hWnd,IDC_GETFORWAR_DUNCDTIONAL,FWD_UNCDTIONAL,CLASS_VOICE);
        break;
    //////////////////////////////////////////////////////////////////////////
    case IDC_BEGIN_SEARCH_CF:
        if (!bSearchCF)
        {
            ME_CREG_URC me_creg_urc;
            ME_GetResult(&me_creg_urc,sizeof(ME_CREG_URC));

            if ((me_creg_urc.Stat == CREG_HOMEREG) || 
                (me_creg_urc.Stat == CREG_ROAMING))
                ME_GetCallForwardStatus(hWnd,IDC_GETFORWAR_DUNCDTIONAL,FWD_UNCDTIONAL,CLASS_VOICE);

            bSearchCF = TRUE;
        }
        break;

    case IDC_GETFORWAR_DUNCDTIONAL://查询无条件转移
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            if (-1 != ME_GetResult(&callfwd_info,sizeof(CALLFWD_INFO)))
                SetForward_Uncdtional(callfwd_info.Enable);

            break;
        }
        ME_GetCallForwardStatus(hWnd,IDC_GETFORWAR_BUSY,FWD_BUSY,CLASS_VOICE);
        break;

    case IDC_GETFORWAR_BUSY://查询遇忙转移
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            if (-1 != ME_GetResult(&callfwd_info,sizeof(CALLFWD_INFO)))
                SetForward_Busy(callfwd_info.Enable);

            break;
        }
        ME_GetCallForwardStatus(hWnd,IDC_GETFORWAR_NOREPLY,FWD_NOREPLY,CLASS_VOICE);
        break;

    case IDC_GETFORWAR_NOREPLY://查询无应答转移
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            if (-1 != ME_GetResult(&callfwd_info,sizeof(CALLFWD_INFO)))
                SetForward_NoReply(callfwd_info.Enable);

            break;
        }
        ME_GetCallForwardStatus(hWnd,IDC_GETFORWAR_NOREACH,FWD_NOREACH,CLASS_VOICE);
        break;

    case IDC_GETFORWAR_NOREACH://查询不在服务区或关机转移
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            if (-1 != ME_GetResult(&callfwd_info,sizeof(CALLFWD_INFO)))
                SetForward_NoReach(callfwd_info.Enable);

            break;
        }
        bInitOver = TRUE;//初始化完成
        break;
    //////////////////////////////////////////////////////////////////////////
    default :
        lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
        break;
    } 
    return lResult;
}
/********************************************************************
* Function     OnCreate
* Purpose      注册消息
* Params       hWnd  -- 窗口句柄
* Return       TRUE  -- 成功
*              FALSE -- 失败       
* Remarks      
**********************************************************************/
static  BOOL    OnCreate(HWND hWnd)
{
    int i = 0;
    while (Me_UrcInfo[i].iUrc != -1)
    {
        if (ME_RegisterIndication(Me_UrcInfo[i].iUrc,IND_ORDINAL,hWnd,Me_UrcInfo[i].iMsg) == -1)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("\nMB ME_RegisterIndication is %d fail\n",Me_UrcInfo[i].iUrc);
#endif
            return FALSE;
        }
        i++;
    }
    ME_RegisterIndication(ME_URC_CREG,IND_ORDINAL,hWnd,IDC_BEGIN_SEARCH_CF);
    return TRUE;
}
/********************************************************************
* Function     OnDestroy
* Purpose       反注册消息 
* Params       hWnd -- 窗口句柄
* Return           
* Remarks      实际上不进行反注册也可以,这个函数就没有调用
**********************************************************************/
static  void    OnDestroy(HWND hWnd)
{
    MB_hWndPostMsg = NULL;
    UnregisterClass(pClassName, NULL);
}
/********************************************************************
* Function     OnListCallsInCcwaAndClip
* Purpose      收到ccwa和clip后列当前电话,根据情况处理
* Params       ccwa:设置呼叫等待后有来电,clip:有来电(标识来电号码)
* Return          
* Remarks      呼叫等待启动后有来电
*********************************************************************/
static  void    OnListCallsInCcwaAndClip(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    int i,iDataLen;

    memset(calls_info,0x00,sizeof(calls_info));
    iDataLen = ME_GetResult(calls_info, sizeof(calls_info));
    if (iDataLen > 0)
        iDataLen = iDataLen / sizeof(CALLS_INFO);
    else
        PostMessage(MB_hWndPostMsg,WM_CALLING_HANGUP,0,0);

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
    printf("\nMB OnListCallsInCcwaAndClip,ME_GetResult return:%d\n",iDataLen);
#endif
    if (iDataLen > 0)
    {
        for (i = 0; i < iDataLen; i++)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("\nMB  calls_info[%d].Stat=%d\n",i,calls_info[i].Stat);
#endif
            if ((calls_info[i].Stat == CALL_STAT_INCOMING) ||   //来电
                (calls_info[i].Stat == CALL_STAT_WAITING))      //等待接听中
            {
                if (iDataLen > MAX_COUNT)
                {   //已经达到最大通话个数的时候,又来了电话,这时需要直接挂断该电话
                    if (-1 == ME_SwitchCallStatus(hWnd,WM_LIST_CALLS_CALLWAITOVER,CUT_HELD,0))
                    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
                        printf("MB MBPstMsg OnListCallsInCcwaAndClip ME_SwitchCallStatus=-1\r\n");
#endif
                    }
                    return;
                }
                MessageMBDialUp(WM_CLOSE,0,0);//关闭拨号界面
                if (MBPhoneRing(&calls_info[i]))//设置呼叫等待后有来电,启动接电话界面
                {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
                    printf("\nMB OnListCallsInCcwaAndClip 设置呼叫等待后有来电,i:%d,号码:%s,index:%d\n",
                        i,calls_info[i].PhoneNum,calls_info[i].index);
#endif
                    bCalling = TRUE;
                    SetTimer(hWnd,CALLING_TIMEROUT_TIMER_ID,
                        TIMER_CALLING_TIMEOUT,NULL);
                    //启动计时器,每隔一段时间列出当前电话
                }
                break;
            }
        }
    }
}
/********************************************************************
* Function     OnListCallsInRing
* Purpose      来电的时候调用接电话界面
* Params       
* Return       
* Remarks      呼叫等待关闭后有来电,还没有通话的时候的第一个来电
**********************************************************************/
static  void    OnListCallsInRing(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    int i,iDataLen;

    memset(calls_info,0x00,sizeof(calls_info));
    iDataLen = ME_GetResult(calls_info, MAX_COUNT * sizeof(CALLS_INFO));
    if (iDataLen > 0)
        iDataLen = iDataLen / sizeof(CALLS_INFO);
    else
        PostMessage(MB_hWndPostMsg,WM_CALLING_HANGUP,0,0);

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
    printf("\nMB OnListCallsInRing,ME_GetResult return:%d\n",iDataLen);
#endif
    if (iDataLen > 0)
    {
        for (i = 0; i < iDataLen; i++)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("\nMB calls_info[%d].Stat=%d\n",i,calls_info[i].Stat);
#endif
            if ((calls_info[i].Stat == CALL_STAT_INCOMING) ||   //来电
                (calls_info[i].Stat == CALL_STAT_WAITING))      //等待接听中
            {
                MessageMBDialUp(WM_CLOSE,0,0);//关闭拨号界面
                if (MBPhoneRing(&calls_info[i]))//来电的时候调用接电话界面
                {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
                    printf("\nMB OnListCallsInRing 呼叫等待关闭后有来电,i:%d,号码:%s,index:%d\n",
                        i,calls_info[i].PhoneNum,calls_info[i].index);
#endif
                    bCalling = TRUE;
                    SetTimer(hWnd,RINGING_TIMEROUT_TIMER_ID,
                        TIMER_RINGING_TIMEOUT, NULL);
                    //来电的时候,需要每隔一段时间列一次当前所有通话,处理意外的通话中断
                }
                return;
            }
        }
    }
}
/********************************************************************
* Function     OnURCWIND
* Purpose      处理+WIND:6消息(挂断),仅在wavecom模块中使用
* Params       
* Return       
* Remarks      
**********************************************************************/
static  void    OnURCWIND(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
#ifdef WAVECOM_2C
    ME_WIND_ITEM ME_Wind_Item;
    memset(&ME_Wind_Item,0x00,sizeof(ME_WIND_ITEM));
    if (-1 != ME_GetResult( &ME_Wind_Item, sizeof(ME_Wind_Item)))
    {
        if (ME_Wind_Item.Event == ME_WIND_CALL_RELEASED)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("\nMB Phone receive OnURCWIND,首先获取通话中断原因\n");
#endif
            ME_GetLastError(hWnd,IDC_GETLASTERROR);//首先获取通话中断原因
        }
    }
#endif
}
/********************************************************************
* Function     OnListCallsInWind
* Purpose      列出当前所有通话, 处理+WIND:6消息(挂断),仅在wavecom模块中使用
* Params       
* Return       
* Remarks      
**********************************************************************/
static  void    OnListCallsInWind(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    int         i,iDataLen;
    CALLS_INFO  newcall_info;

    memset(calls_info,0x00,sizeof(calls_info));
    iDataLen = ME_GetResult(calls_info, MAX_COUNT * sizeof(CALLS_INFO));

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
    printf("\nMB OnListCallsInWind,iDataLen:%d\r\n",iDataLen);
#endif

    if (iDataLen == -1)
    {
        //需要关闭的窗口:拨号界面,接听界面,通话界面
        PostMessage(MB_hWndPostMsg,WM_CALLING_HANGUP,0,0);
        MessageMBDialUp(WM_CLOSE,0,0);//关闭拨号界面
        MessageMBRing(WM_CALLING_HANGUP, NULL, NULL);//向接听界面发送收到对方挂断的消息
        MessageMBCallingOne(WM_VOICE_HANGUPONE,ME_RS_SUCCESS,0);//需要关闭通话界面(单方)
        bCalling = FALSE;
    }
    else
    {
        MBListCurCalls(LISTCURCALLS_REPEAT,wParam,lParam);

        if (IsWindow_MBRing())//正在接听电话(还没有进入通话界面)
        {
            GetNewItemInfo(&newcall_info);//获得来电的信息

            iDataLen = iDataLen / sizeof(CALLS_INFO);

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("MB OnListCallsInWind,iDataLen:%d,号码:%s,index:%d\r\n",
                iDataLen,newcall_info.PhoneNum,newcall_info.index);
#endif
            for (i = 0; i < iDataLen; i++)
            {
                if (calls_info[i].index == newcall_info.index)
                    break;
            }
            if (i == iDataLen)//在当前通话的结构数组中没有找到当前的通话记录,表示当前有错了
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
                printf("MB OnListCallsInWind,(i == iDataLen) 号码:%s,index:%d\r\n",
                    newcall_info.PhoneNum,newcall_info.index);
#endif
                KillTimer(hWnd, CALLING_TIMEROUT_TIMER_ID);
                bCalling = FALSE;
                MessageMBRing(WM_CALLING_HANGUP, NULL, NULL);//向接听界面发送收到对方挂断的消息
            }
        }
    }
}
/********************************************************************
* Function     OnListCallsInTimerCalling
* Purpose      在对方拨打电话未接听的情况每一段时间列当前电话,根据情况处理
* Params       
* Return          
* Remarks      多方通话中,外边来电的情况(启动呼叫等待)
*********************************************************************/
static  void    OnListCallsInTimerCalling(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    int         i,iDataLen;
    CALLS_INFO  NewCall_info;

    memset(calls_info,0x00,sizeof(calls_info));
    iDataLen = ME_GetResult(calls_info, MAX_COUNT * sizeof(CALLS_INFO));

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
    printf("\nMB OnListCallsInTimerCalling,ME_GetResult return:%d,bCalling:%d\n",iDataLen,bCalling);
#endif
    if ((iDataLen == -1) && (bCalling))//打电话的时候列当前电话失败
    {
        PostMessage(MB_hWndPostMsg,WM_CALLING_HANGUP,0,0);
        bCalling = FALSE;
        KillTimer(hWnd, CALLING_TIMEROUT_TIMER_ID);
        MessageMBRing(WM_CALLING_HANGUP, NULL, NULL);//向接听界面发送收到对方挂断的消息
    }
    else if (bCalling)
    {
        iDataLen = iDataLen / sizeof(CALLS_INFO);
        GetNewItemInfo(&NewCall_info);//获得来电的信息

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\nMB OnListCallsInTimerCalling,iDataLen:%d,号码:%s,index:%d\n",
            iDataLen,NewCall_info.PhoneNum,NewCall_info.index);
#endif

        for (i = 0; i < iDataLen; i++)
        {
            if (calls_info[i].index == NewCall_info.index)
                break;
        }
        if (i == iDataLen)//在当前通话的结构数组中没有找到当前的通话记录,表示当前有错了
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("\nMB OnListCallsInTimerCalling,i == iDataLen,i:%d,号码:%s,index:%d\n",
                i,NewCall_info.PhoneNum,NewCall_info.index);
#endif

            KillTimer(hWnd, CALLING_TIMEROUT_TIMER_ID);
            bCalling = FALSE;
            MessageMBRing(WM_CALLING_HANGUP,NULL,NULL);//向接听界面发送收到对方挂断的消息
        }
    }
}
/********************************************************************
* Function     OnListCallsInTimerRinging
* Purpose      收到no carrier和ccsu后列当前电话,根据情况处理
* Params       no carrier:通话被释放 ccsu:保持通话被释放
* Return          
* Remarks      呼叫等待关闭后有来电之后,会启动计时器,每隔一段时间调用本函数
*********************************************************************/
static  void    OnListCallsInTimerRinging(HWND hWnd, WPARAM wParam, LPARAM lParam)//来电的时候
{
    int         i,iDataLen;
    CALLS_INFO  newcall_info;

    memset(calls_info,0x00,sizeof(calls_info));
    iDataLen = ME_GetResult(calls_info,MAX_COUNT * sizeof(CALLS_INFO));

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
    printf("\nMB OnListCallsInTimerRinging,ME_GetResult return:%d,bRinging:%d\n",iDataLen,bRinging);
#endif
    //为什么只通知接听界面通话中断?因为这时候还没有接听电话
    if ((iDataLen == -1) && (bRinging))//当前正在通话中,列出当前通话失败
    {
        bRinging = FALSE;//当前没有通话了
        bCalling = FALSE;
        KillTimer(hWnd, RINGING_TIMEROUT_TIMER_ID);
        MessageMBRing(WM_CALLING_HANGUP, NULL, NULL);//通知接听界面电话中断
    }
    else if (bRinging)
    {
        iDataLen = iDataLen / sizeof(CALLS_INFO);
        GetNewItemInfo(&newcall_info);

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\nMB OnListCallsInTimerRinging,iDataLen:%d,号码:%s,index:%d\n",
            iDataLen,newcall_info.PhoneNum,newcall_info.index);
#endif

        for (i = 0; i < iDataLen; i++)
        {
            if (calls_info[i].index == newcall_info.index)
            {
                if (calls_info[i].Stat == CALL_STAT_ACTIVE)//电话已经接通
                {
                    KillTimer(hWnd,RINGING_TIMEROUT_TIMER_ID);
                    MBRingAnswerCall();
                }
            }
            return;
        }
        if ((i == iDataLen) && (bCalling))
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("\nMB OnListCallsInTimerRinging,i == iDataLen,i:%d,号码:%s,index:%d\n",
                i,newcall_info.PhoneNum,newcall_info.index);
#endif
            KillTimer(hWnd,RINGING_TIMEROUT_TIMER_ID);
            bRinging = FALSE;
            bCalling = FALSE;
            MessageMBRing(WM_CALLING_HANGUP, NULL, NULL);//通知接听界面电话中断
        }
    }
}
void    MessageMBPostMsg(int iCallMsg,WPARAM wParam, LPARAM lParam)//给底层消息分发窗口发送消息
{
    SendMessage(MB_hWndPostMsg,iCallMsg,wParam,lParam);
}
void    MessageMBPostMsgCallForward(void)//重新开始查询有关呼叫转移的信息
{
    ME_GetCallForwardStatus(MB_hWndPostMsg,IDC_GETFORWAR_DUNCDTIONAL,FWD_UNCDTIONAL,CLASS_VOICE);
}
static  LRESULT CALLBACK MBRecPhoneCall_KeyBoardProc(DWORD nUser,WPARAM wParam,LPARAM lParam)
{
    static  const char  cCh_Well     = '#';
    static  const char  cCh_Asterisk = '*';
    static  const char  cCh_Plus     = '+';
                  char  cNum[2]      = "";

//    if (nUser < 0 )  // do not process the message 
//  {
//        return FALSE;
//  }
    if (VK_EARPHONE_KEY == wParam)//耳机上的按键
    {
        //CallMBLogListExtWindow(NULL,DIALED_REC | ANSWERED_REC | UNANSWER_REC);
        return TRUE;
    }
    if (lParam == 0)    //  the key is pressed
    {
        switch (wParam)
        {
        case VK_F1://显示已拨,已接电话列表
            //CallMBLogListExtWindow(NULL,DIALED_REC | ANSWERED_REC | UNANSWER_REC);
            return TRUE;
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
            cNum[0] = (char)wParam;
            APP_CallPhone(cNum);
            return TRUE;
        case VK_F3://*
            cNum[0] = cCh_Asterisk;//'*';
            APP_CallPhone(cNum);
            return TRUE;
        case VK_F4://#
            cNum[0] = cCh_Well;//'#';
            APP_CallPhone(cNum);
            return TRUE;
        }
    }
    else//长按键
    {
        switch (wParam)
        {
        case VK_0://长按
            cNum[0] = cCh_Plus;
            APP_CallPhone(cNum);
            break;
        case VK_F3://*
            //SetLibrateIfSwitch();
            return TRUE;
        }
    }
    return FALSE;
}
/********************************************************************
* Function     GetInitState
* Purpose      获得打电话部分初始化是否完成
* Params       
* Return      完成返回TRUE,初始化未完成返回FALSE    
* Remarks      
*********************************************************************/
BOOL    GetInitState(void)
{
    return bInitOver;
}
