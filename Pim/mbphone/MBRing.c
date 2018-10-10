/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : ��ӵ绰����
 *
 * Purpose  : �����绰
 *            
\**************************************************************************/
#include "MBRing.h"

#define DEBUG_MBRING
#define BLACKLIST//����������ʾ��Ҫ֧�ֺ������Ĵ���

static const char * pClassName          = "MBRingAppClass";
static const char * pClassNameConfirm   = "RingConfirmWndClass";
static const char * pCFString           = "%s%s";
static const int    iPortraitX          = 3;//ͷ�������,Ҳ�Ƕ����ĺ�����
static const int    iPortraitY          = 3;//ͷ��������
static const int    iPorCartoonSpace    = 3;//ͷ��Ͷ�����ʾλ�õļ��
static const int    iPortraitXSpace     = 3;//ͷ�����ʾ���ֵļ��
static const int    iWordSpace          = 3;//����,��������֮��ļ��

static const int    iPortraitW          = 64;//ͷ�������
static const int    iPortraitH          = 64;//ͷ�����߶�
static HWND         hWndRingApp = NULL; //���細��
static PHONEITEM    NewPhoneItem;       //����������Ϣ

static PBINFO       pbNameIcon;         //����ϵ�˴���ȡ��Ϣ�Ľṹ
static BOOL         bHangUp = FALSE;    //�Ƿ��Ѿ��Ҷϵ绰
static BOOL         bCurSceneModeCall;  //��ǰ�龰ģʽ�Ƿ��Ǻ���ģʽ
static BOOL         bExistEarphone;     //��ǰ�Ƿ��ж���
static HINSTANCE    hInstance;
static HWND         hConfirmParent = NULL;
static HWND         hWnd_Confirm = NULL;
static MBConfig     mbconfig;           //ͨ��������Ϣ
static BOOL         bCallForwarding;//���������յ������Ϣ��ʾ��ǰ�����Ǳ�����ֻ�ת�ƹ�����
/********************************************************************
* Function    MBPhoneRing 
* Purpose     �ӵ绰��ڵ� 
* Params       
* Return           
* Remarks      
**********************************************************************/
BOOL    MBPhoneRing(const CALLS_INFO * pNewItemInfo)
{
    WNDCLASS wc;

    if (IsWindow(hWndRingApp))
        return FALSE;

    wc.style         = 0;
    wc.lpfnWndProc   = MBRingAppWndProc;
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

    memset(&NewPhoneItem, 0x00, sizeof(PHONEITEM));
    memcpy(&NewPhoneItem.Calls_Info, pNewItemInfo, sizeof(CALLS_INFO));

    hWndRingApp = CreateWindow(pClassName, 
        TITLECAPTION,WS_CAPTION|WS_BORDER |PWS_STATICBAR, 
        PLX_WIN_POSITION,NULL, NULL,NULL, NULL);

    if (NULL == hWndRingApp)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
    if ((bCurSceneModeCall) || //����ģʽ,������ʾ����
#if defined(BLACKLIST)
        (pbNameIcon.bBlackList) ||//��������ֱ�ӹҶ�,������ʾ����
#endif
        ((bExistEarphone) && (mbconfig.bModeAuto)))//�ж����ҵ�ǰ���Զ�����,������ʾ����
        return TRUE;

    ShowWindow(hWndRingApp, SW_SHOW);
    UpdateWindow(hWndRingApp);

    return TRUE;
}
/********************************************************************
* Function     MBRingAppWndProc
* Purpose      �ӵ绰���̺���
* Params       
* Return           
* Remarks      
**********************************************************************/
static LRESULT CALLBACK MBRingAppWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam)
{
    static  HGIFANIMATE     hRingGif;
    static  BOOL            bSCartoonValid; //gif�����ĳߴ��Ƿ�Ϸ�,TRUE�Ϸ�,FALSE����
    static  BOOL            bSetPortrait;   //�Ƿ�����������ͷ��
    static  BOOL            bSetCartoon;    //�Ƿ����������綯��
    static  HBITMAP         hBitPortrait;   //ͷ����
    static  HBITMAP         hBitCartoon;    //�������
    static  SIZE            sPortrait;      //ͷ��ͼƬ�ߴ�
    static  SIZE            sCartoon;       //����ͼƬ�ߴ�
    static  char            cName[PHONENUMMAXLEN + 1] = "";
    static  char            cPhone[PHONENUMMAXLEN + 1] = "";
    static  RECT            rPortrait;      //��ʾͷ����������
    static  RECT            rCartoon;       //��ʾ�������������
    static  RECT            rName;          //��ʾ����
    static  RECT            rCallForward;   //ת������
    static  RECT            rPhoneNumber;   //��ʾ�绰����
    static  BOOL            bCFClew;        //ת���������ʾ��־,������˸��ʾ
            char            cCartoon[CARTOONFILENAMELEN + 1];
            COLORREF        Color;
            BOOL            bTran,bGetInfoFromPhonebook;
            HDC             hdc;
            LONG            lResult;
            int             PhoneGroupNum;  //�绰·��
            int             iSour,iSH,iSW,iFontHeight;
            CARTOON         cartoon;

    lResult = (LRESULT)TRUE;

    switch (wMsgCmd)
    {
    case WM_CREATE :
        bCurSceneModeCall = GetSceneModeCall();
        if (bCurSceneModeCall)//����ģʽֱ�ӹҶ�
        {
            PostMessage(hWnd,WM_COMMAND,IDC_HANGUP,0);
            return TRUE;
        }
        /////////////////////����ϵ�˲���/////////////////////////////////////////////////////
        memset(&pbNameIcon,0x00,sizeof(PBINFO));

        bGetInfoFromPhonebook = MB_GetInfoFromPhonebook(NewPhoneItem.Calls_Info.PhoneNum,&pbNameIcon);

#if defined(BLACKLIST)
        if (pbNameIcon.bBlackList)//�õ绰�ں�������ֱ�ӹҶ�
        {
            PostMessage(hWnd,WM_COMMAND,IDC_HANGUP,0);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRING)
            printf("mb MBRing.c �绰:%s �ں�������\r\n",NewPhoneItem.Calls_Info.PhoneNum);
#endif
            return TRUE;
        }
#endif
        if (bGetInfoFromPhonebook)//�õ绰����ϵ���ж���
        {
            iSour = strlen(pbNameIcon.Name);
            strncpy(NewPhoneItem.cName,pbNameIcon.Name,sizeof(NewPhoneItem.cName) > iSour ? iSour + 1:sizeof(NewPhoneItem.cName) - 1);//�õ绰����İ����,��������
        }
        ///////ͨ�����ò���///////////////////////////////////////////////////////////////////
        ReadMobileConfig(&mbconfig);//��ȡͨ�����õ���Ϣ
        bExistEarphone = EarphoneStatus();
        if (bExistEarphone)//�ж���
        {
            if (mbconfig.bModeAuto)//���Զ�����
            {
                PostMessage(hWnd,WM_COMMAND,IDC_REPLACE_CALL,0);
                return TRUE;
            }
        }
        ///////////////��ʾ��ť����///////////////////////////////////////////////////////////
        PhoneGroupNum = GetPhoneGroupNum();
        if (2 == PhoneGroupNum)//�Ѿ�����·�绰,��������е�һ·
        {
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, 
                MAKEWPARAM(IDC_REPLACE_CALL,1),(LPARAM)IDS_REPLACE);//�滻
        }
        else if ((0 == PhoneGroupNum) || (1 == PhoneGroupNum))
        {
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, 
                MAKEWPARAM(IDC_REPLACE_CALL,1),(LPARAM)IDS_VOICE_ANSWER);//����
        }
        if (GetUsedItemNum() > 0)
        {
#if defined(SUPPORTMIDDLEBUTTON)
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,
                MAKEWPARAM(IDC_BUTTON_MIDDLE,2),(LPARAM)BUTTON_MIDDLE_TXT);//����
#endif
        }
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,
            (WPARAM)IDC_HANGUP,(LPARAM)IDS_HANGUP);//�Ҷ�
        /////////��ʾͼƬ���貿��/////////////////////////////////////////////////////////////////
        iSW = GetScreenUsableWH1(SM_CXSCREEN);//��Ļ��
        iSH = GetScreenUsableWH1(SM_CYSCREEN);//��Ļ��
        hdc = GetDC(hWnd);

        SetRect(&rPortrait,iPortraitX,iPortraitY,//����ͷ������
            iPortraitX + iPortraitW,iPortraitY + iPortraitH);
        SetRect(&rCartoon,0,rPortrait.bottom + iPorCartoonSpace,iSW,iSH);//���ö�������

        if (FileIfExist(pbNameIcon.Icon))//����ͷ��,������ͼ�����ʾͷ��
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRING)
            printf("mb MBRing.c ������ͷ��:%s\r\n",pbNameIcon.Icon);
#endif
            GetImageDimensionFromFile(pbNameIcon.Icon,&sPortrait);//���ͷ���ļ��ߴ�
            hBitPortrait = CreateBitmapFromImageFile(hdc,pbNameIcon.Icon,&Color,&bTran);
            bSetPortrait = TRUE;//��������ͷ��
        }
        else
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRING)
            printf("mb MBRing.c ͷ��:%sû���ҵ�\r\n",pbNameIcon.Icon);
#endif
            hBitPortrait = NULL;
            bSetPortrait = FALSE;//û������ͷ��
        }

        if (FileIfExist(pbNameIcon.PengName))//�������鶯��
        {
            iSour = strlen(pbNameIcon.PengName);
            strncpy(cCartoon,pbNameIcon.PengName,sizeof(cCartoon) > iSour ? iSour + 1 : sizeof(cCartoon) - 1);
            bSetCartoon = TRUE;//�������綯��
        }
        else//û�������鶯��,�Ӷ������ò��ֻ�ȡ��Ϣ
        {
            GetCartoon(&cartoon);
            if (FileIfExist(cartoon.cCall))//�������ò������������綯����Ϣ
            {
                iSour = strlen(cartoon.cCall);
                strncpy(cCartoon,cartoon.cCall,sizeof(cCartoon) > iSour ? iSour + 1 : sizeof(cCartoon) - 1);
                bSetCartoon = TRUE;//�������綯��
            }
            else
            {
                bSetCartoon = FALSE;//û���������綯��
            }
        }

        if (bSetCartoon)//�������綯��
        {
            GetImageDimensionFromFile(cCartoon,&sCartoon);
            if ((sCartoon.cx > rCartoon.right - rCartoon.left) || 
                (sCartoon.cy > rCartoon.bottom - rCartoon.top))//�����ߴ�̫��,��ʾ����ͼ
            {
                bSCartoonValid = FALSE;//�ߴ�̫��
                hBitCartoon    = CreateBitmapFromImageFile(hdc,cCartoon,&Color,&bTran);
            }
            else
            {
                bSCartoonValid = TRUE;//�ߴ�Ϸ�
                hRingGif       = StartAnimatedGIFFromFile(hWnd,cCartoon,
                    (rCartoon.right - rCartoon.left - sCartoon.cx) / 2,
                    (rCartoon.bottom - rCartoon.top - sCartoon.cy) / 2 + rCartoon.top,DM_NONE);
            }
        }
        ReleaseDC(hWnd,hdc);

        bHangUp         = FALSE;
        bCallForwarding = FALSE;

        memset(cName ,0x00,sizeof(cName ));
        memset(cPhone,0x00,sizeof(cPhone));

        iFontHeight = GetCharactorHeight(hWnd);

        SetRect(&rPhoneNumber,//������ʾ����
            rPortrait.right + iPortraitXSpace,
            rPortrait.top + iWordSpace,
            iSW - iPortraitXSpace,
            rPortrait.top + iWordSpace+ iFontHeight * 2 );
        SetRect(&rName,//������ʾ����
            rPhoneNumber.left,
            rPhoneNumber.bottom + iWordSpace,
            iSW - iPortraitXSpace,
            rPhoneNumber.bottom + iWordSpace + iFontHeight * 2);
        SetRect(&rCallForward,//������ʾת������ı�־
            rPhoneNumber.left,
            rName.bottom + iWordSpace,
            iSW - iPortraitXSpace,
            rName.bottom + iWordSpace + iFontHeight);

        StrAutoNewLine(hdc,NewPhoneItem.cName,cName,&rName,DT_CENTER|DT_VCENTER);
        StrAutoNewLine(hdc,NewPhoneItem.Calls_Info.PhoneNum,cPhone,&rPhoneNumber,DT_CENTER|DT_VCENTER);

        MBRingInit();//�ӵ绰֮ǰ��Ҫִ�еĺ����ļ���
        MBPlayMusic(GetUsedItemNum());

        SetTimer(hWnd,IDC_TIMER_BRINGTOTOP,500,NULL);
        //�������֮��������ʱ��,ÿ��һ��ʱ�佫����ǿ���ö�
        break;

    case IDC_RING_HANGUP:
        //���������յ������Ϣ��ʾ��ǰ�в���ͨ�����Ҷ�,��Ҫ���Ͻ�����Ӧ�Ĵ���
        PhoneGroupNum = GetPhoneGroupNum();
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRING)
        printf("\nMB MBRing.c IDC_RING_HANGUP,PhoneGroupNum:%d\n",PhoneGroupNum);
#endif
        if (1 == PhoneGroupNum)//��ǰ��һ·�绰,�޸İ�ť
        {
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, 
                MAKEWPARAM(IDC_REPLACE_CALL,1),(LPARAM)IDS_VOICE_ANSWER);//����
            DestroyConfirm();//����ȷ�ϴ���
        }
        break;

    case IDC_FORWARDING://���������յ������Ϣ��ʾ��ǰ�����Ǳ�����ֻ�ת�ƹ�����
        SetTimer(hWnd,IDC_TIMER_CF,1500,NULL);
        bCallForwarding = TRUE;
        bCFClew         = TRUE;
        InvalidateRect(hWnd,NULL,TRUE);
        UpdateWindow(hWnd);
        break;

    case WM_TIMER:
        switch (wParam)
        {
        case IDC_TIMER_CF:
            InvalidateRect(hWnd,&rCallForward,TRUE);
            UpdateWindow(hWnd);
            break;
        case IDC_TIMER_BRINGTOTOP:
            //�ڽ���������Ҫ�����û�ʹ�ü��̲���,�����в��Ž�����ڵ�ʱ��,���ڲ����ö�
            if (!IsWindow_Dialup())
                MoveWindowToTop(hWnd);

            break;
        }
        break;

    case WM_COMMAND :
        OnCommandMsg(hWnd, wParam, lParam);
        break;

    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;

    case WM_DESTROY:
        UnregisterClass(pClassName, NULL);
        KillTimer(hWnd,IDC_TIMER_CF);
        if (hRingGif != NULL)
        {
            EndAnimatedGIF(hRingGif);
            hRingGif = NULL;
        }
        if (hBitPortrait != NULL)
        {
            DeleteObject(hBitPortrait);
            hBitPortrait = NULL;
        }
        if (hBitCartoon != NULL)
        {
            DeleteObject(hBitCartoon);
            hBitCartoon = NULL;
        }
        hWndRingApp = NULL;
        Call_EndPlayMusic(RING_PHONE);
        DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRING)
        printf("\nMB MBRing�����ص���Ϣ�ַ����ڷ�֪ͨ��Ϣ,�Ѿ���������\n");
#endif
        MessageMBPostMsg(WM_CALLING_HANGUP,NULL,NULL);//�����ص���Ϣ�ַ����ڷ�֪ͨ��Ϣ,�Ѿ���������
        break;

    case WM_ACTIVATE:
        if (WA_INACTIVE != LOWORD(wParam))
            SetFocus(hWnd);//��������ý���
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;

    case WM_TODESKTOP:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRING)
        printf("\nmb MBRing WM_TODESKTOP\n");
#endif
        if (0 == wParam)//����������Զ�����
            return FALSE;
        SendMessage(hWnd,WM_KEYDOWN,VK_F2,0);
        return FALSE;

    case WM_KEYDOWN:
        switch (LOWORD(wParam))
        {
        case VK_FLIP_ON://���ǽ���
            if (mbconfig.bModeTurn)
                SendMessage(hWnd,WM_COMMAND,IDC_REPLACE_CALL,0);//�����绰

            break;
        case VK_FLIP_OFF://�ϸǹҶ�
            if (mbconfig.bModeTurn)
                SendMessage(hWnd,WM_COMMAND,IDC_HANGUP,0);//�Ҷϵ绰
            break;

        case VK_EARPHONE_KEY://�����ϵİ���
        case VK_RETURN:
        case VK_F1://��softkey�±ߵļ�
            SendMessage(hWnd,WM_COMMAND,IDC_REPLACE_CALL,0);//�����绰
            break;
        case VK_F10:
        case VK_F2://��softkey�±ߵļ�
            SendMessage(hWnd,WM_COMMAND,IDC_HANGUP,0);//�Ҷϵ绰
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
            if (0 == GetUsedItemNum())
            {
                if (mbconfig.bModeAnyKey)//�����������(ֻ���ڵ���ͨ��)
                    SendMessage(hWnd,WM_COMMAND,IDC_REPLACE_CALL,0);//�����绰
            }
            else
            {
                char chSend[2] = "";
                chSend[0] = (LOWORD(wParam) - VK_0 + 48);
                SuperAddDialNumber(chSend,FALSE,TRUE);
            }
            break;
        case KEYASTERICK: //*
        case KEYWELL: //#
        case VK_UP:
        case VK_DOWN:
        case VK_LEFT:
        case VK_RIGHT:
            if (0 == GetUsedItemNum())
            {
                if (mbconfig.bModeAnyKey)//�����������(ֻ���ڵ���ͨ��)
                    SendMessage(hWnd,WM_COMMAND,IDC_REPLACE_CALL,0);//�����绰
            }
            break;
        default:
            return  PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, NULL);
        Rectangle(hdc,rPortrait.left,rPortrait.top,rPortrait.right,rPortrait.bottom);
        if (bSetPortrait)//��ʾͷ��,��������ͼ��
        {
            int nDestX,nDestY,nDestW,nDestH;
            if (iPortraitW > sPortrait.cx)
            {
                nDestX = (iPortraitW - sPortrait.cx) / 2 + iPortraitX;
                nDestW = sPortrait.cx;
            }
            else
            {
                nDestX = iPortraitX;
                nDestW = iPortraitW;
            }
            if (iPortraitH > sPortrait.cy)
            {
                nDestY = (iPortraitH - sPortrait.cy) / 2 + iPortraitY;
                nDestH = sPortrait.cy;
            }
            else
            {
                nDestY = iPortraitY;
                nDestH = iPortraitH;
            }
            StretchBlt(hdc,nDestX,nDestY,nDestW,nDestH,//Ŀ�ľ���
                (HDC)hBitPortrait,0,0,sPortrait.cx,sPortrait.cy,//ͼ��ԭ����
                SRCCOPY);//��λͼ��Դ���ο�����Ŀ�ľ��Σ���չ��ѹ��λͼ��ʹ���ʺ�Ŀ�ľ��εķ�Χ
        }

        if (bSetCartoon)//���������綯��
        {
            if (bSCartoonValid)//�����ߴ�Ϸ�
            {
                PaintAnimatedGIF(hdc,hRingGif);
            }
            else//�����ߴ�Ƿ�
            {
                StretchBlt(hdc,
                    (rCartoon.right - rCartoon.left - sCartoon.cx) / 2,
                    (rCartoon.bottom - rCartoon.top - sCartoon.cy) / 2 + rCartoon.top,
                    sCartoon.cx,sCartoon.cy,
                    (HDC)hBitCartoon,0,0,sCartoon.cx,sCartoon.cy,
                    SRCCOPY);
            }
        }
        if (bCallForwarding)//������ʾת������
        {
            if (bCFClew)
                DrawText(hdc,CALLFORWARDING,strlen(CALLFORWARDING),&rCallForward,DT_CENTER);

            bCFClew = !bCFClew;
        }
        if (strlen(pbNameIcon.Name) != 0)//��ʾ�����ߵ�����
        {
            if (bCallForwarding)
                DrawText(hdc,cName,strlen(cName),&rName,DT_LEFT|DT_VCENTER);
            else
                DrawText(hdc,cName,strlen(cName),&rName,DT_CENTER|DT_VCENTER);
        }
        //��ʾ�绰����
        DrawText(hdc,cPhone,strlen(cPhone),&rPhoneNumber,DT_CENTER|DT_VCENTER);
        EndPaint(hWnd, NULL);
        break;

    case WM_CALLING_HANGUP://�յ��Է��Ҷ���Ϣ
        MBRingHangup();//��������ĹҶϴ���
        break;

    case WM_SWITCH_CALLLINE:
    case WM_VOICER_ANSWER://����Ӧ��
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            MBRingAnswerCall();//�����绰֮��Ĵ���
            break;
        default://ͨ���ж�
            SendAbortCommand(wParam,lParam);//����ȡ��������at����
            MBCallEndWindow(NewPhoneItem.cName,NewPhoneItem.Calls_Info.PhoneNum,NULL,NULL);
            DestroyWindow(hWnd);
            break;
        }
        break;

    case WM_VOICER_HANGUP://�����Ҷ�        
        if(bHangUp)
            break;
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            MBRingHangup();//��������ĹҶϴ���
            MBListCurCalls(LISTCURCALLS_REPEAT,wParam,lParam);
            break;
        default://�����Ҷ�ʧ��
            SendAbortCommand(wParam,lParam);//����ȡ��������at����
            AppMessageBox(hWnd,IDS_HANGUP_FAIL,TITLECAPTION,WAITTIMEOUT);
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
* Function     MBRingHangup
* Purpose      ��������ĹҶϴ���
* Params       
* Return           
* Remarks      
**********************************************************************/
static  void    MBRingHangup(void)
{
    Call_EndPlayMusic(RING_PHONE);

    bHangUp = TRUE;

    SaveMBRecordByPhoneNum(UNANSWER_REC,NewPhoneItem.Calls_Info.PhoneNum,NULL);//����δ������

    DestroyWindow(hWndRingApp);
#if defined(BLACKLIST)
    if ((!bCurSceneModeCall) && (!pbNameIcon.bBlackList))//���Ǻ���ģʽ�ͺ���������ʾͨ�������Ľ���
#else
    if (!bCurSceneModeCall)
#endif
        MBCallEndWindow(NewPhoneItem.cName,NewPhoneItem.Calls_Info.PhoneNum,NULL,NULL);

    if (GetUsedItemNum() == 0)//��ǰû��ͨ��
        EnableState();

    MBRecrodList_Refresh(UNANSWER_REC);//֪ͨͨ����¼�б�ˢ��
}
/********************************************************************
* Function     OnCommandMsg
* Purpose      �����ڵ�WM_COMMAND��Ϣ
* Params       
* Return           
* Remarks      
**********************************************************************/
static void OnCommandMsg(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    int PhoneGroupNum;

    PhoneGroupNum = GetPhoneGroupNum();
    switch (LOWORD(wParam))
    {
    case IDC_REPLACE_CALL://����
        Call_EndPlayMusic(RING_PHONE);
        if (0 == PhoneGroupNum)
        {
            if (-1 == ME_CallAnswer(hWnd,WM_VOICER_ANSWER))//����ʧ��
                AppMessageBox(hWnd,IDS_ANSWER_FAIL,TITLECAPTION,WAITTIMEOUT);
        }
        else if (1 == PhoneGroupNum)
        {
            if (MAX_COUNT == GetUsedItemNum())//һ·�绰�����ǵ绰���Ѿ��ﵽ�������
            {
                GotoConfirmWindow(hWnd);
            }
            else
            {
                if (-1 == ME_SwitchCallStatus(hWnd,WM_SWITCH_CALLLINE,ACTIVE_SWITCH,0))
                    AppMessageBox(hWnd,SWITCHFAIL,TITLECAPTION,WAITTIMEOUT);
            }
        }
        else if (2 == PhoneGroupNum)
        {
            GotoConfirmWindow(hWnd);
        }
        break;

    case IDC_BUTTON_MIDDLE://����
        SuperAddDialNumber(NULL,FALSE,TRUE);
        break;

    case IDC_HANGUP://�ܾ�
        Call_EndPlayMusic(RING_PHONE);
        if (0 == PhoneGroupNum)
        {
            if (-1 == ME_SwitchCallStatus(hWnd,WM_VOICER_HANGUP,CUT_HELD,0))
                AppMessageBox(hWnd,IDS_HANGUP_FAIL,TITLECAPTION,WAITTIMEOUT);
        }
        else if ((1 == PhoneGroupNum) || (2 == PhoneGroupNum))
        {
            if (-1 == ME_SwitchCallStatus(hWnd,WM_VOICER_HANGUP,CUT_HELD,0))
                AppMessageBox(hWnd,IDS_HANGUP_FAIL,TITLECAPTION,WAITTIMEOUT);
        }
        break;
    }
}
/********************************************************************
* Function     GetNewItemInfo
* Purpose      ����������Ϣ
* Params       
* Return           
* Remarks      
**********************************************************************/
void    GetNewItemInfo(CALLS_INFO * pNewItemInfo)
{
    memset(pNewItemInfo,0x00,sizeof(CALLS_INFO));
    memcpy(pNewItemInfo,&NewPhoneItem.Calls_Info,sizeof(CALLS_INFO));
}
/********************************************************************
* Function   MessageMBRing  
* Purpose    ��������淢����Ϣ
* Params    
* Return    
* Remarks   
**********************************************************************/
void    MessageMBRing(UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    if ((hWndRingApp != NULL) && (IsWindow(hWndRingApp)))
        PostMessage(hWndRingApp,wMsgCmd,wParam,lParam);
}
/********************************************************************
* Function   IsWindow_MBRing  
* Purpose    ��ǰ�Ƿ���ڽ�������
* Params    
* Return    
* Remarks   
**********************************************************************/
BOOL    IsWindow_MBRing(void)
{
    if (hWndRingApp == NULL)
        return FALSE;
    return (IsWindow(hWndRingApp));
}
/********************************************************************
* Function   MBRingAnswerCall  
* Purpose    �����绰֮��Ĵ���
* Params    
* Return    
* Remarks   
**********************************************************************/
void    MBRingAnswerCall(void)
{
    SYSTEMTIME  sCurTime;
    char        cNameCF[PHONENUMMAXLEN + 10] = "";
    int         iSour;

    if (!IsWindow(hWndRingApp))
        return;

    if (bCallForwarding)//�õ绰�Ǳ�ת�ƹ�����,������֮ǰ������ʾ��
    {
        sprintf(cNameCF,pCFString,CALLFORWARDING,NewPhoneItem.cName);
        iSour = strlen(cNameCF);
        strncpy(NewPhoneItem.cName,cNameCF,sizeof(NewPhoneItem.cName) > iSour ? iSour + 1 : sizeof(NewPhoneItem.cName) - 1);
    }
    NewPhoneItem.Calls_Info.Stat = CALL_STAT_ACTIVE;//��¼״̬,���ڼ���״̬��ͨ��

    SaveMBRecordByPhoneNum(ANSWERED_REC,NewPhoneItem.Calls_Info.PhoneNum,&sCurTime);//�����ѽ�����

    memcpy(&NewPhoneItem.TimeInDial,&sCurTime,sizeof(SYSTEMTIME));
    MBCallingWindow(&NewPhoneItem,NULL,DTMF_NONE);//����ͨ������

    MBRecrodList_Refresh(ANSWERED_REC);//֪ͨͨ����¼�б�ˢ��

    DestroyWindow(hWndRingApp);
}
//////////////////////////////////////////////////////////////////////////
/********************************************************************
* Function     GotoConfirmWindow
* Purpose      ȷ���滻��������
* Params       
* Return           
* Remarks      �Ҷϼ����ͨ��
**********************************************************************/
static BOOL     GotoConfirmWindow(HWND hWnd)
{
    WNDCLASS wc;

    hConfirmParent = hWnd;

    wc.style         = 0;
    wc.lpfnWndProc   = ConfirmWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassNameConfirm;

    if (!RegisterClass(&wc))
        return FALSE;

    hWnd_Confirm = CreateWindow(pClassNameConfirm, 
        IDS_PL_CONFIRM, WS_CAPTION | WS_CLIPCHILDREN | PWS_STATICBAR, 
        PLX_WIN_POSITION,hConfirmParent,NULL,NULL, NULL);

    if (NULL == hWnd_Confirm)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
    ShowWindow(hWnd_Confirm, SW_SHOW);
    UpdateWindow(hWnd_Confirm);

    return TRUE;
}
/********************************************************************
* Function      ConfirmWndProc     
* Purpose       ȷ���滻���ں���
* Params       
* Return           
* Remarks      
**********************************************************************/
static LRESULT CALLBACK ConfirmWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    HDC     hdc;
    LRESULT lResult;
    RECT    rect;

    lResult = (LRESULT)TRUE;

    switch (wMsgCmd)
    {
    case WM_CREATE:
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_SET,1),(LPARAM)BUTTONOK);
#if defined(SUPPORTMIDDLEBUTTON)
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_MIDDLE,2),(LPARAM)BUTTON_MIDDLE_TXT);//����
#endif
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_QUIT,0),(LPARAM)BUTTONQUIT);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON_SET://�Ҷϼ����ͨ��,�����ĵ绰Ҳһ�𱻽�ͨ
            if (-1 == ME_SwitchCallStatus(hWnd,WM_SWITCH_CALLLINE,CUT_ACTIVED,0))
                AppMessageBox(hWnd,IDS_ANSWER_FAIL,TITLECAPTION,WAITTIMEOUT);
            break;

        case IDC_BUTTON_MIDDLE://����
            SuperAddDialNumber(NULL,FALSE,TRUE);
            break;

        case IDC_BUTTON_QUIT:
            DestroyWindow(hWnd);
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            break;
        }
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd,NULL);
        SetRect(&rect,0,20,GetScreenUsableWH1(SM_CXSCREEN),40);
        StrAutoNewLine(hdc,IDS_CLOSECURCALL,NULL,&rect,DT_CENTER);
        EndPaint(hWnd,NULL);
        break;

    case WM_TODESKTOP:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRING)
        printf("\nmb MBRing_ext WM_TODESKTOP\n");
#endif
        if (0 == wParam)//����������Զ�����
            return FALSE;
        SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
        return FALSE;

    case WM_KEYDOWN:
        switch (LOWORD(wParam))
        {
        case VK_EARPHONE_KEY://�����ϵİ���
        case VK_F1://��softkey�±ߵļ�
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SET,0);
            break;
        case VK_F2://��softkey�±ߵļ�
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
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
            {
                char chSend[2] = "";
                chSend[0] = (LOWORD(wParam) - VK_0 + 48);
                SuperAddDialNumber(chSend,FALSE,TRUE);
            }
            break;
        default:
            return  PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

    case WM_SWITCH_CALLLINE://�Ҷϼ���ͨ���Ļص���Ϣ
        MBListCurCalls(LISTCURCALLS_HANGUP_ACTIVE,wParam,lParam);//�е�ǰͨ��
        break;

    case WM_DESTROY:
        hWnd_Confirm = NULL;
        UnregisterClass(pClassNameConfirm,NULL);
        DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
        break;
    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam); 
        break;
    }
    return lResult;
}
/********************************************************************
* Function      DestroyConfirm       
* Purpose       ����ȷ�ϴ���
* Params       
* Return           
* Remarks      
**********************************************************************/
static  void    DestroyConfirm(void)
{
    if (IsWindow(hWnd_Confirm))
        DestroyWindow(hWnd_Confirm);
}
/********************************************************************
* Function     GetCurrentPhoneNum
* Purpose      ��õ�ǰ����ĵ绰����
* Params       pPhoneNum:���浱ǰ����ĵ绰����Ļ�����,iBuflen:Ŀ�껺�����ĳ���
* Return           
* Remarks      
**********************************************************************/
void    GetCurrentPhoneNum(char * pPhoneNum,unsigned int iBuflen)
{
    int isour;

    isour = strlen(NewPhoneItem.Calls_Info.PhoneNum);
    strncpy(pPhoneNum,NewPhoneItem.Calls_Info.PhoneNum,
        (int)iBuflen > isour ? isour + 1 : iBuflen - 1);
}
/********************************************************************
* Function     MBRingInit
* Purpose      �ӵ绰֮ǰ��Ҫִ�еĺ����ļ���
* Params      
* Return           
* Remarks      int EncORDec();
                ����ֵ1����CloseVideo();
                ����ֵ0����Mini_OnStopVideoPlay(HWND hWnd)
**********************************************************************/
static  void    MBRingInit(void)
{
    //SetupHf_Lcd(TRUE);
    //SetupHf_LcdBklight(TRUE);//������
    DlmNotify(PMM_NEWS_ENABLE,PMF_MAIN_BKLIGHT|PMF_MAIN_SCREEN_LCD);
    EndPicBrowser();//����ر�ͼƬԤ��
#ifndef _EMULATE_
#if defined(EXCEPTION)
    switch (EncORDec())
    {
    case 0:
        Mini_OnStopVideoPlay(NULL);
        break;
    case 1:
        CLoseVideo();
        break;
    }
#endif
#endif
    if (GetUsedItemNum() == 0)//��ǰû��ͨ��
        DisableState();
}
