/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : ��ӵ绰����
 *
 * Purpose  : �������ӵĽ��� (�ɲ��Ž�������)
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
//#define     DISPLAYREDIRALTEXT//����������ʾ��Ҫ��ʾ"�ڼ����ز�"
//#define     GIFREFRESH_NORMAL       //gif����ˢ�·�ʽ:һ��ˢ��
#define     GIFREFRESH_NOTFLICKER   //gif����ˢ�·�ʽ:����˸ˢ��

#define     IDC_BUTTON_MIDDLE       7//���ż�
#define     IDC_BUTTON_QUIT_LINK    3//�˳���ť
#define     IDC_ME_VOICEDIAL        WM_USER + 100//��������
#define     IDC_ME_CURCALLS         WM_USER + 110//�г���ǰ����ͨ��
#define     IDC_ME_CUTALL           WM_USER + 120//�Ҷϼ����ͨ��
#define     IDC_PIC_INIT            WM_USER + 130//������ʼ��
#define     IDC_TIMER_LINK          150//��ʼ�����е绰�ļ�ʱ��
#define     IDC_TIMER_INIT          160//��ʼ����Ϣ�ļ�ʱ��
#define     IDC_TIMER_REDIAL        170//�Զ��ز��õļ�ʱ��

#define     TITLECAPTION            MBGetString(STR_MBPHONE_LINKING_CAPTION)//������
#define     BUTTONQUIT              (LPARAM)MBGetString(STR_MBPHONE_HANGUP) //�Ҷ�
#define     BUTTON_MIDDLE_TEXT      MBGetString(STR_MBPHONE_DIALNUMBER)//����

#define     DIALLINKINGBUSY         MBGetString(STR_MBPHONE_BUSY)//��·æ
#define     DIALLINKINGHALT         MBGetString(STR_MBPHONE_HALT)//"�����ж�"
#define     DIALLINKINGHALTFAIL     MBGetString(STR_MBPHONE_HANGUPFAIL)//"�Ҷ�ʧ��"
#define     DIALLINKINGWAIT         MBGetString(STR_MBPHONE_WAIT)//"���Ե�"
#define     DIALLING_CALLFORWARD    MBGetString(STR_MBPHONE_CALLFORWARDINGSTART)//"����ת��������"
#define     DIALLING_CALLFORWARDUN  MBGetString(STR_MBPHONE_CALLFORWARDINGUNSTART)//"����������ת��������"
#define     DIALLING_BARRING        MBGetString(STR_MBPHONE_DIALLING_BARRING)//���Ʋ���
#define     DIALLING_FARE           MBGetString(STR_MBPHONE_DIALLING_FARE)//"�ѵ���������"
#define     CLEW_EMERGENT           MBGetString(STR_MBPHONE_EMERGENT)//�����绰������...
#define     REDIALCLEW              MBGetString(STR_MBPHONE_REDIALCLEW)//"��%d���ز�"

static const int    iMinute           = 1000;
static const int    iPicY             = 5;//�����ļ���������
static const char * pClassName        = "MBDialLinkingWndClass";//��������
static const char * pMBLINKINGPICTURE = MBPIC_DIALLINK;//�������Ӷ������ļ���

static HINSTANCE    hInstance;
static char         cPhoneNumber[PHONENUMMAXLEN + 1];//�绰����
static DTMF_TYPE    dtmf_type;//��ǰ�绰�������Ƿ��зֻ���
static char         cPhoneNumberExtension[PHONENUMMAXLEN + 1];//�ֻ�����
static char         cPhoneNumberEx[PHONENUMMAXLEN + 1];//���зֻ��ŵ�ʱ��,����ֻ���֮ǰ�ĺ���
static SYSTEMTIME   sDialupTime;//����ʱ��
static int          iCalling;//���������������ŵı�־,���ڹҶ�
static MBConfig     mbconfig;//ͨ�������ļ�
static HWND         hDailLink = NULL;
static SIZE         PicSize = {0,0};
static PHONEITEM    PhoneItem;
#if defined(DISPLAYREDIRALTEXT)
static char         cRedial[50] = "";//������ʾ�ڼ����ز�
static RECT         rRedial;//��ʾ�ز�������
#endif
static unsigned int iRedial = 0;//���ڼ�¼��ǰ�ǵڼ����ز�

static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  int     GetActiveCall(CALLS_INFO * Info,int nLimmit);//�ü���绰�����
static  BOOL    IfExitActiveCall(CALLS_INFO * Info,int nLimmit);
static  BOOL    AutoRedialup(void);
static  void    AfterHaltDeal(WPARAM wParam,LPARAM lParam);//�����ж�֮��Ĵ���
static  BOOL    IfExistAlertingCall(CALLS_INFO * pInfo,int nLimmit);
#if defined(GIFREFRESH_NOTFLICKER)
static  void    PaintBkFunc(HGIFANIMATE hGIFAnimate,BOOL bEnd,int x,int y,HDC hdcMem);
#endif
/********************************************************************
* Function     CallMBDialLinkingWindow
* Purpose      ��ʾ�������ӵĽ���
* Params       pPhoneCode:�����ĺ���,pDialupTime:����ʱ��
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
    static  RECT        rName;//����
    static  RECT        rPhone;//�绰����
    static  CALLS_INFO  calls_info[MAX_COUNT];//�г���ǰ����ͨ��ʱ,��ȡ������Ϣ�Ľṹ
    static  BOOL        bHangup;//�Ƿ�ѡ���˹Ҷϼ�
    static  BOOL        bListCurCalls;//�Ƿ�������е�ǰͨ������Ϣ
    static  char        cPhone[PHONENUMMAXLEN + 20] = "";//������ʾ�������
    static  char        cName[PHONENUMMAXLEN + 1] = "";//������ʾ��������    
    static  BOOL        bCanDail;//�Ƿ����ѡ�񲦺ż�
            BOOL        bEmergent;//��ǰ�����Ƿ�����绰����
            int         iGsmSpecific;//GSM����ǰ׺�ĳ���
            int         iSW,iSH,iFontHeight;
            int         iPhoneNum;//��ǰ����ͨ������
            int         nActiveCall;//��õ�ǰ������绰�����
            LRESULT     lResult;
            HDC         hdc;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        bCanDail        = FALSE;//������ѡ�񲦺ż�
        bHangup         = FALSE;//û��ѡ��Ҷϼ�
        bListCurCalls   = FALSE;//�Ƿ�������е�ǰͨ������Ϣ
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT_LINK,BUTTONQUIT);

        GetScreenUsableWH2(&iSW,&iSH);
        //��ʾ�������貿��
        PostMessage(hWnd,IDC_PIC_INIT,0,0);
        //////////////////////////////////////////////////////////////////////////
        //��ʾ����,�������貿��
        iFontHeight = GetCharactorHeight(hWnd);

        SetRect(&rName,0,iPicY + PicSize.cy + 1,iSW,iPicY + PicSize.cy + 1 + iFontHeight);
        SetRect(&rPhone,0,rName.bottom,iSW,rName.bottom + iFontHeight * 2);
#if defined(DISPLAYREDIRALTEXT)
        SetRect(&rRedial,0,rPhone.bottom + 1,iSW,rPhone.bottom + 1 + iFontHeight * 2);
#endif
        //////////////////////////////////////////////////////////////////////////
        //ȡ�������ߵ���Ϣ
        memset(&PhoneItem,0x00,sizeof(PHONEITEM));
        memset(cPhoneNumberEx,0x00,sizeof(cPhoneNumberEx));
        memset(cPhoneNumberExtension,0x00,sizeof(cPhoneNumberExtension));

        iGsmSpecific = GsmSpecific(cPhoneNumber);//���ú����Ƿ����GSMǰ׺
        //���ú����Ƿ�����ֻ���,���ڷֻ��ŵĻ���ô��绰����ͷֻ���
        dtmf_type    = IfExistExtension(cPhoneNumber,cPhoneNumberEx,cPhoneNumberExtension);

        if (DTMF_NONE != dtmf_type)//�зֻ���,������ϵ��ʱӦ�ô��봿����(��ȥ�ֻ���,��ȥGSMǰ׺)
        {
            GetPhoneItem(&PhoneItem,cPhoneNumberEx + iGsmSpecific);
            iCalling  = ME_VoiceDial(hWnd,IDC_ME_VOICEDIAL,cPhoneNumberEx,NULL);//��������
            bEmergent = NumberIsEmergent(cPhoneNumberEx + iGsmSpecific,TRUE);//��ǰ�����Ƿ�����绰����
        }
        else//û�зֻ���,������ϵ��ʱӦ�ô��봿����(��ȥGSMǰ׺)
        {
            GetPhoneItem(&PhoneItem,cPhoneNumber + iGsmSpecific);
            iCalling  = ME_VoiceDial(hWnd,IDC_ME_VOICEDIAL,cPhoneNumber,NULL);//��������
            bEmergent = NumberIsEmergent(cPhoneNumber + iGsmSpecific,TRUE);//��ǰ�����Ƿ�����绰����
        }

        if (-1 == iCalling)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB MBDialLink��������ʧ��:%s\n",cPhoneNumber);
#endif
#ifndef _EMULATE_
            MBCallEndWindow(PhoneItem.cName,cPhoneNumber,NULL,NULL);//����ͨ����������
            PostMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_BUTTON_QUIT_LINK, 0 );
#endif
        }

        memset(cPhone ,0x00,sizeof(cPhone ));
        memset(cName  ,0x00,sizeof(cName  ));
#if defined(DISPLAYREDIRALTEXT)
        memset(cRedial,0x00,sizeof(cRedial));
#endif
        iRedial = 0;//��ǰ�ǵ�0���ز�
        hdc     = GetDC(hWnd);
        if (bEmergent)//��ǰ�����ǽ����绰����,��"�����绰������..."����cPhone��
        {
            StrAutoNewLine(hdc,CLEW_EMERGENT,cPhone,&rPhone,NULL);
        }
        else
        {
            if (iGsmSpecific < (int)strlen(cPhoneNumber))
                StrAutoNewLine(hdc,cPhoneNumber + iGsmSpecific,cPhone,&rPhone,NULL);//��ʾ�绰
            else
                StrAutoNewLine(hdc,cPhoneNumber,cPhone,&rPhone,NULL);//��ʾ�绰

            StrAutoNewLine(hdc,PhoneItem.cName,cName,&rName,NULL);//��ʾ����pbInfo.Name
        }
        ReleaseDC(hWnd,hdc);

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
        printf("\nMB MBDialLink��������,��ʾ����:%s,����:%d,iCalling:%d\n",
            cPhone,strlen(cPhone),iCalling);
#endif
        SetTimer(hWnd,IDC_TIMER_INIT,1000,NULL);//���ó�ʼ���ļ�ʱ��

        if (GetUsedItemNum() == 0)//��ǰû��ͨ��
            DisableState();

        break;

    case IDC_PIC_INIT:
        hRingGif = StartAnimatedGIFFromFile(hWnd,pMBLINKINGPICTURE,//������ʾ
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
            SetFocus(hWnd);//��������ý���

        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;

    case WM_TIMER:
        switch (wParam)
        {
        case IDC_TIMER_INIT://��ʼ����Ϣ�ļ�ʱ��
            KillTimer(hWnd,IDC_TIMER_INIT);
            ReadMobileConfig(&mbconfig);//��ȡͨ�����õ���Ϣ

            if (GetForward_Uncdtional())//�����Ƿ����������ת��
                AppMessageBox(hWnd,DIALLING_CALLFORWARDUN,TITLECAPTION, WAITTIMEOUT);
            else if (GetForward_Condition())//�����Ƿ��������ת��
                AppMessageBox(hWnd,DIALLING_CALLFORWARD,TITLECAPTION, WAITTIMEOUT);

            break;

        case IDC_TIMER_LINK:
            KillTimer(hWnd,IDC_TIMER_LINK);

            bListCurCalls = TRUE;//�Ѿ������е�ǰͨ������Ϣ
            SetListCurCalls(TRUE);//�����Ƿ������е�ǰͨ��

            if (-1 == ME_ListCurCalls(hWnd,IDC_ME_CURCALLS))//�г���ǰ����ͨ��
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
                printf("\nMB MBDialLink,ME_ListCurCallsʧ��\n");
#endif
                bListCurCalls = FALSE;//�����е�ǰͨ������Ϣ���
                SetListCurCalls(FALSE);//�����Ƿ������е�ǰͨ��
                AfterHaltDeal(wParam,lParam);
                MBCallEndWindow(PhoneItem.cName,cPhoneNumber,NULL,NULL);//����ͨ����������
            }
            break;

        case IDC_TIMER_REDIAL://�Զ��ز��õļ�ʱ��
            KillTimer(hWnd,IDC_TIMER_REDIAL);
            if (DTMF_NONE != dtmf_type)//�зֻ���
            {
                iCalling = ME_VoiceDial(hDailLink,IDC_ME_VOICEDIAL,cPhoneNumberEx,NULL);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
                printf("mb MBDialLink �Զ��ز�:%s\r\n",cPhoneNumberEx);
#endif
            }
            else//û�зֻ���
            {
                iCalling = ME_VoiceDial(hDailLink,IDC_ME_VOICEDIAL,cPhoneNumber,NULL);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
                printf("mb MBDialLink �Զ��ز�:%s\r\n",cPhoneNumber);
#endif
            }

            if (-1 == iCalling)
            {
                MBCallEndWindow(PhoneItem.cName,cPhoneNumber,NULL,NULL);//����ͨ����������
                PostMessage(hDailLink, WM_COMMAND, (WPARAM)IDC_BUTTON_QUIT_LINK, 0 );
            }
            break;
        }
        break;

    case WM_CLOSE://������Ͻǵ��˳���ťʱΪ�����˳�;
        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_BUTTON_QUIT_LINK, 0 );
        break;

    case WM_DESTROY ://֪ͨ���������,�˳����˳�;
        hDailLink       = NULL;
        iCalling        = 0;
        bHangup         = FALSE;
        bListCurCalls   = FALSE;
        if (hRingGif != NULL)
            EndAnimatedGIF(hRingGif);
        hRingGif = NULL;

        UnregisterClass(pClassName,NULL);
        DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)hInstance);
        if (GetUsedItemNum() == 0)//��ǰû��ͨ��
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
        DrawText(hdc,cPhone ,strlen(cPhone) ,&rPhone ,DT_CENTER|DT_VCENTER);//��ʾ�绰
#if defined(DISPLAYREDIRALTEXT)
        DrawText(hdc,cRedial,strlen(cRedial),&rRedial,DT_CENTER|DT_VCENTER);//��ʾ�ڼ����ز�
#endif
        EndPaint(hWnd, NULL);
        break;

    case WM_TODESKTOP:
        if (0 == wParam)//����������Զ�����
            return FALSE;
        SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT_LINK,0);
        return FALSE;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        //case VK_EARPHONE_KEY://�����ϵİ���
        case VK_F10:
        case VK_F2://�Ҷ�
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT_LINK,0);
            break;
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
            if (bCanDail)//����ѡ�񲦺ż�
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
        case IDC_BUTTON_MIDDLE://���ż�
            SuperAddDialNumber(NULL,FALSE,TRUE);
            break;
        case IDC_BUTTON_QUIT_LINK://�Ҷ�
            bHangup = TRUE;//ѡ��Ҷϼ�
            if (bListCurCalls)//���ڷ����е�ǰͨ������Ϣ
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
                printf("\nMB MBDialLinkѡ���˹Ҷϼ�,���ڷ����е�ǰͨ������Ϣ\n");
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
                printf("\nMB MBDialLink,�Ҷ�ʧ��,�Ҷϼ����ͨ��\n");
#endif
                if (-1 == ME_SwitchCallStatus(hWnd,IDC_ME_CUTALL,CUT_ACTIVED,0))//�Ҷϼ����ͨ��
                {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
                    printf("\nMB MBDialLink,ME_SwitchCallStatus,�Ҷϼ����ͨ��ʧ��\n");
#endif
#if defined(_EMULATE_)
                    DestroyWindow(hWnd);
#endif
                }
                return TRUE;
            }
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB MBDialLink,�Ҷ�\n");
#endif
            AfterHaltDeal(wParam,lParam);
            MBCallEndWindow(PhoneItem.cName,cPhoneNumber,NULL,NULL);//����ͨ����������
            break;
        }
        break;
    //**************************ME�ص���Ϣ*************************
    case IDC_ME_VOICEDIAL://��������,���ŷ���
        switch (wParam)
        {
        case ME_RS_SUCCESS://�ɹ�
            SetTimer(hWnd,IDC_TIMER_LINK,500,NULL);
            break;

        case ME_RS_USERSTOP://�û��жϲ����ɹ�,ȡ������
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("mb MBDialLink �û��жϲ����ɹ�,ȡ������\r\n");
#endif
            AfterHaltDeal(wParam,lParam);
            MBCallEndWindow(PhoneItem.cName,cPhoneNumber,NULL,NULL);//����ͨ����������
            break;

        case ME_RS_BUSY://��·æ
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB DialLink��·æ\n");
#endif
            if (!AutoRedialup())//�Զ��ز�
            {
                AfterHaltDeal(wParam,lParam);
                MBCallEndWindow(PhoneItem.cName,cPhoneNumber,NULL,NULL);//����ͨ����������
            }
            break;

        case ME_RS_NOCARRIER://ͨ���ж�
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB DialLinkͨ���ж�,wParam:%ld,lParam:%ld\n",wParam,lParam);
#endif
            switch (lParam)
            {
            case ME_E_CALLBARRED://��ʾ��ǰ�������Ƽ����޷������绰
                AfterHaltDeal(wParam,lParam);
                MBCallEndWindow(PhoneItem.cName,DIALLING_BARRING,NULL,NULL);//����ͨ����������
                break;
            case ME_E_MAXACM://�Ʒ��޶��ѳ�
                AfterHaltDeal(wParam,lParam);
                MBCallEndWindow(PhoneItem.cName,cPhoneNumber,NULL,DIALLING_FARE);//����ͨ����������
                break;
            default:
                if (!AutoRedialup())//�Զ��ز�
                {
                    AfterHaltDeal(wParam,lParam);
                    MBCallEndWindow(PhoneItem.cName,cPhoneNumber,NULL,NULL);//����ͨ����������
                }
                break;
            }
            break;

        default:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB DialLink,��������,���ŷ���default,wParam:%ld,lParam:%ld\n",wParam,lParam);
#endif
            SendAbortCommand(wParam,lParam);//����ȡ��������at����
            switch (lParam)
            {
            case ME_E_CALLBARRED://��ʾ��ǰ�������Ƽ����޷������绰
                AfterHaltDeal(wParam,lParam);
                MBCallEndWindow(PhoneItem.cName,DIALLING_BARRING,NULL,NULL);//����ͨ����������
                break;
            case ME_E_MAXACM://�Ʒ��޶��ѳ�
                AfterHaltDeal(wParam,lParam);
                MBCallEndWindow(PhoneItem.cName,cPhoneNumber,NULL,DIALLING_FARE);//����ͨ����������
                break;
            default:
                if (!AutoRedialup())//�Զ��ز�
                {
                    AfterHaltDeal(wParam,lParam);
                    MBCallEndWindow(PhoneItem.cName,cPhoneNumber,NULL,NULL);//����ͨ����������
                }
                break;
            }
            break;
        }
        break;

    case IDC_ME_CURCALLS://�г���ǰ����ͨ��,����
        bListCurCalls = FALSE;//�����е�ǰͨ������Ϣ���
        SetListCurCalls(FALSE);//�����Ƿ������е�ǰͨ��
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
        printf("\nMB MBDialLink�г���ǰ����ͨ��,wParam=%ld\n",wParam);
#endif
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            if (bHangup)//ѡ���˹Ҷϼ�
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
                printf("\nMB MBDialLinkѡ���˹Ҷϼ�\n");
#endif
                SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT_LINK,0);//���Ͱ��¹Ҷϼ�����Ϣ
                return TRUE;
            }
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB MBDialLink�г���ǰ����ͨ��\n");
#endif
            memset(calls_info,0x00,sizeof(calls_info));
            iPhoneNum = ME_GetResult(calls_info,sizeof(calls_info));

            if (iPhoneNum == -1)//������,��Ҫ�Ҷϼ����ͨ��
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
                printf("MB MBDialLink,iPhoneNum == -1\r\n");
#endif
                if (!AutoRedialup())//��ǰ�����Զ��ز�
                {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
                    printf("mb MBDialLink iPhoneNum == -1\r\n");
#endif
                    ME_SwitchCallStatus(hWnd,IDC_ME_CUTALL,CUT_ACTIVED,0);//�Ҷϼ����ͨ��
                    AfterHaltDeal(wParam,lParam);

                    MBCallEndWindow(PhoneItem.cName,cPhoneNumber,NULL,NULL);//����ͨ����������
                }
                return TRUE;
            }

            iPhoneNum = iPhoneNum / sizeof(CALLS_INFO);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB MBDialLink,iPhoneNum:%d\n",iPhoneNum);
#endif
            //����绰��������,��ôclcc�ṹ�ĸ���Ӧ�ö��ڵ�ǰͨ������
            if (iPhoneNum <= GetUsedItemNum())
            {
                break;
            }
            nActiveCall = GetActiveCall(calls_info,iPhoneNum);//��õ�ǰ������绰�����
            if (-1 == nActiveCall)//��ǰ������,Ҫ����õ绰�Ƿ���,�ڵĻ�����ͨ���б�
            {
                bCanDail = IfExistAlertingCall(calls_info,iPhoneNum);//�Ƿ����ѡ�񲦺ż�
#if defined(SUPPORTMIDDLEBUTTON)
                SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,//���ż�
                    MAKEWPARAM(IDC_BUTTON_MIDDLE,2), 
                    bCanDail ? (LPARAM)BUTTON_MIDDLE_TEXT : (LPARAM)"");
#endif
                if (!AutoRedialup())//��ǰ�����Զ��ز�
                {
                    if (!IfExitActiveCall(calls_info,iPhoneNum))
                    {
                        //��ǰ�绰���벻�ڽṹ������,�õ绰��ͨ����ռ��
                        ME_SwitchCallStatus(hWnd,IDC_ME_CUTALL,CUT_ACTIVED,0);//�Ҷϼ����ͨ��

                        PostMessage(hWnd,IDC_ME_VOICEDIAL,ME_RS_NOCARRIER,0);//�����ж�
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
                        printf("\nMB MBDialLink,IfExitActiveCall\n");
#endif
                        return TRUE;
                    }
                    SetTimer(hWnd,IDC_TIMER_LINK,500,NULL);
//                    bListCurCalls = TRUE;//�Ѿ������е�ǰͨ������Ϣ
//                    SetListCurCalls(TRUE);//�����Ƿ������е�ǰͨ��
//                    if (-1 == ME_ListCurCalls(hWnd,IDC_ME_CURCALLS))//���е�ǰͨ��
//                    {
//                        bListCurCalls = FALSE;//�����е�ǰͨ������Ϣ���
//                        SetListCurCalls(FALSE);//�����Ƿ������е�ǰͨ��
//                        ME_SwitchCallStatus(hWnd,IDC_ME_CUTALL,CUT_ACTIVED,0);//�Ҷϼ����ͨ��
//                        PostMessage(hWnd,IDC_ME_VOICEDIAL,ME_RS_NOCARRIER,0);//�����ж�
//#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
//                        printf("\nMB MBDialLink,nActiveCall==-1��ME_ListCurCallsʧ��\n");
//#endif
//                    }
//#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
//                    printf("\nMB MBDialLink,��ǰ�ǵ���ͨ������,nActiveCall=-1\n");
//#endif
                }
                return TRUE;
            }
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB MBDialLink,��ǰ�绰����:%s,%d\n",calls_info[nActiveCall].PhoneNum,strlen(calls_info[nActiveCall].PhoneNum));
#endif

            if (mbconfig.bGetClew)//���Ž�ͨ��ʾ��
                PlayClewMusic(1);

            if (strcmp(calls_info[nActiveCall].PhoneNum,cPhoneNumber) != 0)
            {
                char cCall[200] = "";
                sprintf(cCall,"���Ŵ���,�г��ĺ���%s,ԭ����:%s\r\n",calls_info[nActiveCall].PhoneNum,cPhoneNumber);
                SaveLog(cCall);
            }
            memcpy(&PhoneItem.Calls_Info,&calls_info[nActiveCall],sizeof(CALLS_INFO));
            memcpy(&PhoneItem.TimeInDial,&sDialupTime,sizeof(SYSTEMTIME));
            MBCallingWindow(&PhoneItem,cPhoneNumberExtension,dtmf_type);//����ͨ������
            DestroyWindow(hWnd);
            break;

        default://�����������ж�
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB MBDialLink,�г���ǰ����ͨ������default,wParam:%ld,bHangup:%d\n",wParam,bHangup);
#endif
            if (bHangup)//ѡ���˹Ҷϼ�
            {
                SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT_LINK,0);//���Ͱ��¹Ҷϼ�����Ϣ
                return TRUE;
            }
            if (!AutoRedialup())//��ǰ�����Զ��ز�
            {
                AfterHaltDeal(wParam,lParam);
                MBCallEndWindow(PhoneItem.cName,cPhoneNumber,NULL,NULL);//����ͨ����������
            }
            break;
        }
        break;

    case IDC_ME_CUTALL://�Ҷϼ����ͨ��
        switch(wParam)
        {
        case ME_RS_SUCCESS:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB MBDialLink,IDC_ME_CUTALL�Ҷϼ����ͨ��\n");
#endif
            AfterHaltDeal(wParam,lParam);
            break;
        default:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
            printf("\nMB MBDialLink,IDC_ME_CUTALL�Ҷϼ����ͨ��ʧ��\n");
#endif
            SendAbortCommand(wParam,lParam);//����ȡ��������at����
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
* Purpose      �Զ��ز�
* Params       
* Return     
* Remarks      
**********************************************************************/
static  BOOL    AutoRedialup(void)
{
    if ((!mbconfig.bAutoRedial) || (mbconfig.iAutoRedial <= 0))//�����Զ��ز�
        return FALSE;

    iRedial++;
    if (iRedial > (unsigned int)mbconfig.iAutoRedial)//�����Զ��ز���������
        return FALSE;

#if defined(DISPLAYREDIRALTEXT)
    sprintf(cRedial,REDIALCLEW,iRedial);
    InvalidateRect(hDailLink,NULL,TRUE);
#endif

    //CTA���Ե�ʱ��Ҫ��̶�ÿ���Զ��ز���ʱ����
    //��1���ز����5S
    //��2���ز����1MIN
    //��3���ز����1MIN
    //��4���ز����1MIN
    //��5���ز����3MIN
    //.................
    //��N���ز����3MIN
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
* Purpose      �������ӽ�������Żص���Ϣ
* Params       iType:���ӽ��,����NOCARRIER��
* Return     
* Remarks      
**********************************************************************/
void    SendMBDialLinkDialInfo(int iType)
{
    PostMessage(hDailLink,IDC_ME_VOICEDIAL,iType,0);
}
/********************************************************************
* Function     GetActiveCall
* Purpose      ��õ�ǰ������绰�����
* Params       
* Return     -1��ʾû���ҵ�
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
            printf("\nMB MBDialLink,GetActiveCall,����:%s,i:%d\n",Info[i].PhoneNum,i);
#endif
            return i;
        }
    }
    return -1;
}
/********************************************************************
* Function     IfExitActiveCall
* Purpose      ��ǰ�绰�����Ƿ��ڽṹ������
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
                printf("\nMB MBDialLink,IfExitActiveCall,����:%s,i:%d\n",Info[i].PhoneNum,i);
#endif
                return TRUE;
            }
        }
        else
        {
            if (strcmp(Info[i].PhoneNum,cPhoneNumberEx) == 0)
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALLINK)
                printf("\nMB MBDialLink,IfExitActiveCall,����:%s,i:%d\n",Info[i].PhoneNum,i);
#endif
                return TRUE;
            }
        }
    }
    return FALSE;
}
/********************************************************************
* Function     IfExistAlertingCall
* Purpose      ��ǰ�绰��Ϣ���Ƿ������������ĵ绰
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
* Purpose      �����ж�֮��Ĵ���
* Params       
* Return     
* Remarks      
**********************************************************************/
static  void    AfterHaltDeal(WPARAM wParam,LPARAM lParam)
{
    MBListCurCalls(LISTCURCALLS_REPEAT,wParam,lParam);//���е�ǰͨ��
    DestroyWindow(hDailLink);
}
/********************************************************************
* Function     IsWindow_DialLink
* Purpose      ��ǰ�Ƿ�������ӽ���
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
void    bInitMBLink(void)//��ʼ������
{
    GetImageDimensionFromFile(pMBLINKINGPICTURE,&PicSize);//��ö����ļ��ߴ�
}
//////////////////////////////////////////////////////////////////////////
#if defined(GIFREFRESH_NOTFLICKER)
// ��gif�Ļص�������ˢ�±���
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
