/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : ��绰
 *
 * Purpose  : �ײ���Ϣ�ķַ�
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
#define WM_LIST_CALLS_WIND          WM_USER + 208//Ŀǰû��ʹ��
#define WM_LIST_CALLS_CCWA_CLIP     WM_USER + 209
#define WM_LIST_CALLS_RING          WM_USER + 210
#define WM_LIST_CALLS_RINGING       WM_USER + 211
#define WM_LIST_CALLS_CALLWAITOVER  WM_USER + 212

#define IDC_GETCALLWAIT             WM_USER + 110//��ѯ���еȴ�
#define IDC_SETCALLWAIT             WM_USER + 120//���ú��еȴ�

#define IDC_BEGIN_SEARCH_CF         WM_USER + 180//�յ����ע�����Ϣ֮��,��ʼ��ѯ����ת��
#define IDC_GETFORWAR_DUNCDTIONAL   WM_USER + 130//��ѯ������ת��
#define IDC_GETFORWAR_BUSY          WM_USER + 140//��ѯ��æת��
#define IDC_GETFORWAR_NOREPLY       WM_USER + 150//��ѯ��Ӧ��ת��
#define IDC_GETFORWAR_NOREACH       WM_USER + 160//��ѯ���ڷ�������ػ�ת��
#define IDC_GETLASTERROR            WM_USER + 170//��ȡͨ���ж�ԭ��

//�Է�����绰ʱ�ĳ�ʱTimer,���Է������Ҷ�
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
    {ME_URC_CRING       ,WM_URC_CRING},     //������(��ʶ��������)
    {ME_URC_RING        ,WM_URC_RING},      //������(��ʶ��������)
    {ME_URC_NOCARRIER   ,WM_URC_NOCARRIER}, //ͨ�����ͷ�
    {ME_URC_CSSU        ,WM_URC_CSSU},      //����ͨ�����ͷ�
    {ME_URC_CCWA        ,WM_URC_CCWA},      //���ú��еȴ���������
#endif
#ifdef WAVECOM_2C
    {ME_URC_CRING       ,WM_URC_CRING},     //������(��ʶ��������)
    {ME_URC_RING        ,WM_URC_RING},      //������(��ʶ��������)
    {ME_URC_NOCARRIER   ,WM_URC_NOCARRIER}, //ͨ�����ͷ�          
    {ME_URC_CSSU        ,WM_URC_CSSU},      //����ͨ�����ͷ�      
    {ME_URC_CCWA        ,WM_URC_CCWA},      //���ú��еȴ��������� 
    {ME_URC_WIND        ,WM_URC_WIND},
#endif
#ifdef TI_MODULE
    {ME_URC_CRING       ,WM_URC_CRING},     //������(��ʶ��������)
    {ME_URC_RING        ,WM_URC_RING},      //������(��ʶ��������)
    {ME_URC_NOCARRIER   ,WM_URC_NOCARRIER}, //ͨ�����ͷ�          
    {ME_URC_CSSU        ,WM_URC_CSSU},      //����ͨ�����ͷ�      
    {ME_URC_CCWA        ,WM_URC_CCWA},      //���ú��еȴ��������� 
#endif
    {-1,-1},
};
#define     CALLFORWARDING          MBGetString(STR_MBPHONE_CALLFORWARDING_CLEW)//ת������

static const char * pClassName = "MBPostMsgWndClass";
static HWND         MB_hWndPostMsg;//�ַ���Ϣ���ھ��
static CALLS_INFO   calls_info[MAX_COUNT + 1];//420���ֽ�

#ifndef _EMULATE_
static BOOL         bInitOver = FALSE;//��ʼ���Ƿ����
#else
static BOOL         bInitOver = TRUE ;//ģ�⻷����Ϊ��ʼ���Ѿ����
#endif

static BOOL         bSearchCF = FALSE;//��־�����л���ʱ���Ƿ��Ѿ���ѯ������ת����
static BOOL         bCalling;//�ӵ绰�Ľ����Ƿ��Ѿ�����
static BOOL         bRinging;//��������֮��,������Ϣ�᲻ͣ������,��ʱ����Ҫֻ����һ��,��Ҫ��־һ��
static BOOL         bCFClew = FALSE;//����CSSU��Ϣ���RING��Ϣһ�����ϵ�����,������ʱ����Ҫֻ����һ��,��Ҫ��־һ��
// �ڲ��������� 
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
* Purpose      ��ʼ����Ϣ�ַ����ش���
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
* Purpose      ���ش�����Ϣ�������
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
        CreateMBGSMWindow();//��ʼ��GSM����
        InitMBCalling();    //��ʼ��ͨ����������ͼƬ��Ϣ
        bInitMBDialup();    //��ʼ������
        bInitMBLink();      //��ʼ������
        ME_GetCallWaitingStatus(hWnd,IDC_GETCALLWAIT,CLASS_VOICE);
        //��������������ģ���������������ʱ������һ�º��еȴ�,
        //��ôͨ����ʱ��,���ղ����������Ϣ,������������ʱ��ǿ������
        //һ�º��еȴ�.
        break;

    case WM_DESTROY:
        OnDestroy(hWnd);
        break;

    case WM_URC_NOCARRIER://ע�����Ϣ,ͨ�����ͷ�
#if defined(SIEMENS_TC35)

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\nMB Phone receive NOCARRIER,���Ȼ�ȡͨ���ж�ԭ��\n");
#endif

        if (IsWindow_DialLink())
            SendMBDialLinkDialInfo(ME_URC_NOCARRIER);
        else
            ME_GetLastError(hWnd,IDC_GETLASTERROR);//���Ȼ�ȡͨ���ж�ԭ��
#endif
        break;

    case WM_URC_CSSU://ע�����Ϣ,����ͨ�����ͷ�
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\nMB Phone receive CSSU!\n");
#endif
        if (IsWindow_MBRing() && (!bCFClew))//��ǰ�ǽ�������,��CSSU��Ϣ�ǵ�һ������
        {
            ME_CSSU_ITEM me_cssu_item;
            if (-1 != ME_GetResult(&me_cssu_item,sizeof(ME_CSSU_ITEM)))
            {
                if (me_cssu_item.Action == 0)//�õ绰�Ǳ���ֻ�ת�ƹ�����,��֪������
                {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
                    printf("mb �õ绰�Ǳ���ֻ�ת�ƹ�����\r\n");
#endif
                    MessageMBRing(IDC_FORWARDING,NULL,NULL);
                    AppMessageBox(NULL,CALLFORWARDING,"",WAITTIMEOUT);
                    bCFClew = TRUE;
                    break;
                }
            }
        }
#if defined(SIEMENS_TC35)
        SetListCurCalls(TRUE);//�����Ƿ������е�ǰͨ��
        if (-1 == ME_ListCurCalls(hWnd,WM_LIST_CALLS_NOCAR_CCSU))//�г���ǰ����ͨ��
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("\nMB Phone receive CSSU==-1!\n");
#endif
        }
#endif
        break;

    case WM_URC_CCWA://ע�����Ϣ,���ú��еȴ���������
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\nMB Phone receive CCWA!\n");
#endif
        if (-1 == ME_ListCurCalls(hWnd,WM_LIST_CALLS_CCWA_CLIP))//�г���ǰ����ͨ��
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("\nMB Phone receive CCWA==-1!\n");
#endif
        }
        break;

    case WM_URC_CRING://ע�����Ϣ,������(��ʶ��������)
    case WM_URC_RING://ע�����Ϣ,������(��ʶ��������)
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\nMB Phone receive RING!bRinging:%d\r\n",bRinging);
#endif
        if (!bRinging)
        {
            bRinging = TRUE;
            if (-1 == ME_ListCurCalls(hWnd,WM_LIST_CALLS_RING))//�г���ǰ����ͨ��
            {
                bRinging = FALSE;
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
                printf("\nMB ME_ListCurCalls==-1\r\n");
#endif
            }
        }
        break;

    case WM_URC_CLIP://ע�����Ϣ,���ػ���
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\nMB Phone receive CLIP!\n");
#endif
        if (-1 == ME_ListCurCalls(hWnd,WM_LIST_CALLS_CCWA_CLIP))//�г���ǰ����ͨ��
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("\nMB Phone receive CLIP==-1!\n");
#endif
        }
        break;

    case WM_URC_WIND://ע�����Ϣ, ����+WIND:6��Ϣ(�Ҷ�),����wavecomģ����ʹ��
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\nMB Phone receive WIND\r\n");
#endif
        OnURCWIND(hWnd,(WPARAM)wParam,(LPARAM)lParam);//����+WIND:6��Ϣ(�Ҷ�),����wavecomģ����ʹ��
        break;

    case WM_LIST_CALLS_RINGING://�����ʱ��,�ӵ绰��timer,�г���ǰ����ͨ��
        OnListCallsInTimerRinging(hWnd, wParam, lParam);
        break;

    case WM_LIST_CALLS_RING://������(��ʶ��������),�г���ǰ����ͨ���Ļص���Ϣ
        OnListCallsInRing(hWnd, wParam, lParam);//����ýӵ绰�ĺ���
        break;

    case WM_LIST_CALLS_NOCAR_CCSU://ע�����Ϣ(ͨ�����ͷ�/����ͨ�����ͷ�),�г���ǰ����ͨ��
        OnListCallsInWind(hWnd, wParam, lParam);
        //�յ�no carrier��ccsu���е�ǰ�绰,�����������
        break;

    case WM_TIMER:
        switch (wParam)
        {
        case CALLING_TIMEROUT_TIMER_ID://���еȴ�������������,�ӵ绰֮�����������ʱ��
            if (-1 == ME_ListCurCalls(hWnd,WM_LIST_CALLS_CALLING))//�г���ǰ����ͨ��
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
                printf("\nMB CALLING_TIMEROUT_TIMER_ID == -1!\n");
#endif
            }
            break;
        case RINGING_TIMEROUT_TIMER_ID://���еȴ��رպ��������ʱ��,�ӵ绰
            if (-1 == ME_ListCurCalls(hWnd,WM_LIST_CALLS_RINGING))//�г���ǰ����ͨ��
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
                printf("\nMB RINGING_TIMEROUT_TIMER_ID == -1!\n");
#endif
            }
            break;
        }   
        break;

    case WM_LIST_CALLS_CALLING://�ӵ绰��timer,�г���ǰ����ͨ��
        OnListCallsInTimerCalling(hWnd, wParam, lParam);
        //�ڶԷ�����绰δ���������ÿһ��ʱ���е�ǰ�绰,�����������
        break;

    case WM_LIST_CALLS_WIND://�г���ǰ����ͨ��, ����+WIND:6��Ϣ(�Ҷ�),����wavecomģ����ʹ��
        OnListCallsInWind(hWnd, wParam, lParam);
        break;

    case WM_LIST_CALLS_CCWA_CLIP://���ú��еȴ���������,�г���ǰ����ͨ��
        OnListCallsInCcwaAndClip(hWnd, wParam, lParam);
        //�յ�ccwa��clip���е�ǰ�绰,�����������,����ýӵ绰�ĺ���
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

    case IDC_GETLASTERROR://��ȡͨ���ж�ԭ��
        ME_GetResult(&lLastError,sizeof(lLastError));

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("MB ��ȡͨ���ж�ԭ��,wParam:%ld,lParam:%ld,lLastError:%lx\r\n",wParam,lParam,lLastError);
#endif

        switch (lLastError)
        {
        case ME_E_MAXACM://��ʾ�Ʒ��޶��ѳ�
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("MB ��ȡͨ���ж�ԭ��,�Ʒ��޶��ѳ�\r\n");
#endif
            SetCostLimit(TRUE);//���õ�ǰͨ���ѵ���������
            break;
        }

        SetListCurCalls(TRUE);//�����Ƿ������е�ǰͨ��
        if (-1 == ME_ListCurCalls(hWnd,WM_LIST_CALLS_NOCAR_CCSU))//�г���ǰ����ͨ��
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("\nMB Phone receive NOCARRIER==-1\n");
#endif
        }
        break;
    //////////////////////////////////////////////////////////////////////////
    case IDC_GETCALLWAIT://��ѯ���еȴ�
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
    case IDC_SETCALLWAIT://��ѯ����ת��
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

    case IDC_GETFORWAR_DUNCDTIONAL://��ѯ������ת��
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            if (-1 != ME_GetResult(&callfwd_info,sizeof(CALLFWD_INFO)))
                SetForward_Uncdtional(callfwd_info.Enable);

            break;
        }
        ME_GetCallForwardStatus(hWnd,IDC_GETFORWAR_BUSY,FWD_BUSY,CLASS_VOICE);
        break;

    case IDC_GETFORWAR_BUSY://��ѯ��æת��
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            if (-1 != ME_GetResult(&callfwd_info,sizeof(CALLFWD_INFO)))
                SetForward_Busy(callfwd_info.Enable);

            break;
        }
        ME_GetCallForwardStatus(hWnd,IDC_GETFORWAR_NOREPLY,FWD_NOREPLY,CLASS_VOICE);
        break;

    case IDC_GETFORWAR_NOREPLY://��ѯ��Ӧ��ת��
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            if (-1 != ME_GetResult(&callfwd_info,sizeof(CALLFWD_INFO)))
                SetForward_NoReply(callfwd_info.Enable);

            break;
        }
        ME_GetCallForwardStatus(hWnd,IDC_GETFORWAR_NOREACH,FWD_NOREACH,CLASS_VOICE);
        break;

    case IDC_GETFORWAR_NOREACH://��ѯ���ڷ�������ػ�ת��
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            if (-1 != ME_GetResult(&callfwd_info,sizeof(CALLFWD_INFO)))
                SetForward_NoReach(callfwd_info.Enable);

            break;
        }
        bInitOver = TRUE;//��ʼ�����
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
* Purpose      ע����Ϣ
* Params       hWnd  -- ���ھ��
* Return       TRUE  -- �ɹ�
*              FALSE -- ʧ��       
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
* Purpose       ��ע����Ϣ 
* Params       hWnd -- ���ھ��
* Return           
* Remarks      ʵ���ϲ����з�ע��Ҳ����,���������û�е���
**********************************************************************/
static  void    OnDestroy(HWND hWnd)
{
    MB_hWndPostMsg = NULL;
    UnregisterClass(pClassName, NULL);
}
/********************************************************************
* Function     OnListCallsInCcwaAndClip
* Purpose      �յ�ccwa��clip���е�ǰ�绰,�����������
* Params       ccwa:���ú��еȴ���������,clip:������(��ʶ�������)
* Return          
* Remarks      ���еȴ�������������
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
            if ((calls_info[i].Stat == CALL_STAT_INCOMING) ||   //����
                (calls_info[i].Stat == CALL_STAT_WAITING))      //�ȴ�������
            {
                if (iDataLen > MAX_COUNT)
                {   //�Ѿ��ﵽ���ͨ��������ʱ��,�����˵绰,��ʱ��Ҫֱ�ӹҶϸõ绰
                    if (-1 == ME_SwitchCallStatus(hWnd,WM_LIST_CALLS_CALLWAITOVER,CUT_HELD,0))
                    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
                        printf("MB MBPstMsg OnListCallsInCcwaAndClip ME_SwitchCallStatus=-1\r\n");
#endif
                    }
                    return;
                }
                MessageMBDialUp(WM_CLOSE,0,0);//�رղ��Ž���
                if (MBPhoneRing(&calls_info[i]))//���ú��еȴ���������,�����ӵ绰����
                {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
                    printf("\nMB OnListCallsInCcwaAndClip ���ú��еȴ���������,i:%d,����:%s,index:%d\n",
                        i,calls_info[i].PhoneNum,calls_info[i].index);
#endif
                    bCalling = TRUE;
                    SetTimer(hWnd,CALLING_TIMEROUT_TIMER_ID,
                        TIMER_CALLING_TIMEOUT,NULL);
                    //������ʱ��,ÿ��һ��ʱ���г���ǰ�绰
                }
                break;
            }
        }
    }
}
/********************************************************************
* Function     OnListCallsInRing
* Purpose      �����ʱ����ýӵ绰����
* Params       
* Return       
* Remarks      ���еȴ��رպ�������,��û��ͨ����ʱ��ĵ�һ������
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
            if ((calls_info[i].Stat == CALL_STAT_INCOMING) ||   //����
                (calls_info[i].Stat == CALL_STAT_WAITING))      //�ȴ�������
            {
                MessageMBDialUp(WM_CLOSE,0,0);//�رղ��Ž���
                if (MBPhoneRing(&calls_info[i]))//�����ʱ����ýӵ绰����
                {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
                    printf("\nMB OnListCallsInRing ���еȴ��رպ�������,i:%d,����:%s,index:%d\n",
                        i,calls_info[i].PhoneNum,calls_info[i].index);
#endif
                    bCalling = TRUE;
                    SetTimer(hWnd,RINGING_TIMEROUT_TIMER_ID,
                        TIMER_RINGING_TIMEOUT, NULL);
                    //�����ʱ��,��Ҫÿ��һ��ʱ����һ�ε�ǰ����ͨ��,���������ͨ���ж�
                }
                return;
            }
        }
    }
}
/********************************************************************
* Function     OnURCWIND
* Purpose      ����+WIND:6��Ϣ(�Ҷ�),����wavecomģ����ʹ��
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
            printf("\nMB Phone receive OnURCWIND,���Ȼ�ȡͨ���ж�ԭ��\n");
#endif
            ME_GetLastError(hWnd,IDC_GETLASTERROR);//���Ȼ�ȡͨ���ж�ԭ��
        }
    }
#endif
}
/********************************************************************
* Function     OnListCallsInWind
* Purpose      �г���ǰ����ͨ��, ����+WIND:6��Ϣ(�Ҷ�),����wavecomģ����ʹ��
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
        //��Ҫ�رյĴ���:���Ž���,��������,ͨ������
        PostMessage(MB_hWndPostMsg,WM_CALLING_HANGUP,0,0);
        MessageMBDialUp(WM_CLOSE,0,0);//�رղ��Ž���
        MessageMBRing(WM_CALLING_HANGUP, NULL, NULL);//��������淢���յ��Է��Ҷϵ���Ϣ
        MessageMBCallingOne(WM_VOICE_HANGUPONE,ME_RS_SUCCESS,0);//��Ҫ�ر�ͨ������(����)
        bCalling = FALSE;
    }
    else
    {
        MBListCurCalls(LISTCURCALLS_REPEAT,wParam,lParam);

        if (IsWindow_MBRing())//���ڽ����绰(��û�н���ͨ������)
        {
            GetNewItemInfo(&newcall_info);//����������Ϣ

            iDataLen = iDataLen / sizeof(CALLS_INFO);

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("MB OnListCallsInWind,iDataLen:%d,����:%s,index:%d\r\n",
                iDataLen,newcall_info.PhoneNum,newcall_info.index);
#endif
            for (i = 0; i < iDataLen; i++)
            {
                if (calls_info[i].index == newcall_info.index)
                    break;
            }
            if (i == iDataLen)//�ڵ�ǰͨ���Ľṹ������û���ҵ���ǰ��ͨ����¼,��ʾ��ǰ�д���
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
                printf("MB OnListCallsInWind,(i == iDataLen) ����:%s,index:%d\r\n",
                    newcall_info.PhoneNum,newcall_info.index);
#endif
                KillTimer(hWnd, CALLING_TIMEROUT_TIMER_ID);
                bCalling = FALSE;
                MessageMBRing(WM_CALLING_HANGUP, NULL, NULL);//��������淢���յ��Է��Ҷϵ���Ϣ
            }
        }
    }
}
/********************************************************************
* Function     OnListCallsInTimerCalling
* Purpose      �ڶԷ�����绰δ���������ÿһ��ʱ���е�ǰ�绰,�����������
* Params       
* Return          
* Remarks      �෽ͨ����,�����������(�������еȴ�)
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
    if ((iDataLen == -1) && (bCalling))//��绰��ʱ���е�ǰ�绰ʧ��
    {
        PostMessage(MB_hWndPostMsg,WM_CALLING_HANGUP,0,0);
        bCalling = FALSE;
        KillTimer(hWnd, CALLING_TIMEROUT_TIMER_ID);
        MessageMBRing(WM_CALLING_HANGUP, NULL, NULL);//��������淢���յ��Է��Ҷϵ���Ϣ
    }
    else if (bCalling)
    {
        iDataLen = iDataLen / sizeof(CALLS_INFO);
        GetNewItemInfo(&NewCall_info);//����������Ϣ

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\nMB OnListCallsInTimerCalling,iDataLen:%d,����:%s,index:%d\n",
            iDataLen,NewCall_info.PhoneNum,NewCall_info.index);
#endif

        for (i = 0; i < iDataLen; i++)
        {
            if (calls_info[i].index == NewCall_info.index)
                break;
        }
        if (i == iDataLen)//�ڵ�ǰͨ���Ľṹ������û���ҵ���ǰ��ͨ����¼,��ʾ��ǰ�д���
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("\nMB OnListCallsInTimerCalling,i == iDataLen,i:%d,����:%s,index:%d\n",
                i,NewCall_info.PhoneNum,NewCall_info.index);
#endif

            KillTimer(hWnd, CALLING_TIMEROUT_TIMER_ID);
            bCalling = FALSE;
            MessageMBRing(WM_CALLING_HANGUP,NULL,NULL);//��������淢���յ��Է��Ҷϵ���Ϣ
        }
    }
}
/********************************************************************
* Function     OnListCallsInTimerRinging
* Purpose      �յ�no carrier��ccsu���е�ǰ�绰,�����������
* Params       no carrier:ͨ�����ͷ� ccsu:����ͨ�����ͷ�
* Return          
* Remarks      ���еȴ��رպ�������֮��,��������ʱ��,ÿ��һ��ʱ����ñ�����
*********************************************************************/
static  void    OnListCallsInTimerRinging(HWND hWnd, WPARAM wParam, LPARAM lParam)//�����ʱ��
{
    int         i,iDataLen;
    CALLS_INFO  newcall_info;

    memset(calls_info,0x00,sizeof(calls_info));
    iDataLen = ME_GetResult(calls_info,MAX_COUNT * sizeof(CALLS_INFO));

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
    printf("\nMB OnListCallsInTimerRinging,ME_GetResult return:%d,bRinging:%d\n",iDataLen,bRinging);
#endif
    //Ϊʲôֻ֪ͨ��������ͨ���ж�?��Ϊ��ʱ��û�н����绰
    if ((iDataLen == -1) && (bRinging))//��ǰ����ͨ����,�г���ǰͨ��ʧ��
    {
        bRinging = FALSE;//��ǰû��ͨ����
        bCalling = FALSE;
        KillTimer(hWnd, RINGING_TIMEROUT_TIMER_ID);
        MessageMBRing(WM_CALLING_HANGUP, NULL, NULL);//֪ͨ��������绰�ж�
    }
    else if (bRinging)
    {
        iDataLen = iDataLen / sizeof(CALLS_INFO);
        GetNewItemInfo(&newcall_info);

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\nMB OnListCallsInTimerRinging,iDataLen:%d,����:%s,index:%d\n",
            iDataLen,newcall_info.PhoneNum,newcall_info.index);
#endif

        for (i = 0; i < iDataLen; i++)
        {
            if (calls_info[i].index == newcall_info.index)
            {
                if (calls_info[i].Stat == CALL_STAT_ACTIVE)//�绰�Ѿ���ͨ
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
            printf("\nMB OnListCallsInTimerRinging,i == iDataLen,i:%d,����:%s,index:%d\n",
                i,newcall_info.PhoneNum,newcall_info.index);
#endif
            KillTimer(hWnd,RINGING_TIMEROUT_TIMER_ID);
            bRinging = FALSE;
            bCalling = FALSE;
            MessageMBRing(WM_CALLING_HANGUP, NULL, NULL);//֪ͨ��������绰�ж�
        }
    }
}
void    MessageMBPostMsg(int iCallMsg,WPARAM wParam, LPARAM lParam)//���ײ���Ϣ�ַ����ڷ�����Ϣ
{
    SendMessage(MB_hWndPostMsg,iCallMsg,wParam,lParam);
}
void    MessageMBPostMsgCallForward(void)//���¿�ʼ��ѯ�йغ���ת�Ƶ���Ϣ
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
    if (VK_EARPHONE_KEY == wParam)//�����ϵİ���
    {
        //CallMBLogListExtWindow(NULL,DIALED_REC | ANSWERED_REC | UNANSWER_REC);
        return TRUE;
    }
    if (lParam == 0)    //  the key is pressed
    {
        switch (wParam)
        {
        case VK_F1://��ʾ�Ѳ�,�ѽӵ绰�б�
            //CallMBLogListExtWindow(NULL,DIALED_REC | ANSWERED_REC | UNANSWER_REC);
            return TRUE;
        case VK_0:  //�������ּ�0
        case VK_1:  //�������ּ�1
        case VK_2:  //�������ּ�2
        case VK_3:  //�������ּ�3
        case VK_4:  //�������ּ�4
        case VK_5:  //�������ּ�5
        case VK_6:  //�������ּ�6
        case VK_7:  //�������ּ�7
        case VK_8:  //�������ּ�8
        case VK_9:  //�������ּ�9
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
    else//������
    {
        switch (wParam)
        {
        case VK_0://����
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
* Purpose      ��ô�绰���ֳ�ʼ���Ƿ����
* Params       
* Return      ��ɷ���TRUE,��ʼ��δ��ɷ���FALSE    
* Remarks      
*********************************************************************/
BOOL    GetInitState(void)
{
    return bInitOver;
}
