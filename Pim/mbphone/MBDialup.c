/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : ��ӵ绰����
 *
 * Purpose  : ���Ž���
 *            
\**************************************************************************/
#include "MBDialup.h"

static const char * pClassName = "MBDialupWndClass";//��������
static HINSTANCE    hInstance;
static char         cPhoneNumber[PHONENUMMAXLEN + 1];//�绰����
static DIALITEMBTN  dialitembtn[MBBTN_NUM]; //ÿ�������Ľṹ
static HWND         hMBDialUp;              //���Ž���ľ��
static BOOL         bShow;
static BOOL         bSuperAddDial;          //��ǰ�����Ƿ���׷�Ӳ���
static BOOL         bAddCanSendNumber;      //��ǰ������׷�Ӳ��Ž����ʱ���Ƿ���Բ���
static BOOL         bSpecial;               //��ǰ�����Ƿ���ר�ò��Ž���

#if defined(MBDIALUPSTYLE_TOUCH)
static BOOL         bPenDown = FALSE;       //��ǰ�Ƿ��д����ʰ���
static int          iPicNumHeight;          //����ͼƬ�ĸ߶�
static int          iPicNumChWidth;         //������������Ŀ�
static HBITMAP      hPicNumber;             //����ͼƬ�ľ��
static HBITMAP      hPicNumBar;             //�����ľ��
static HBITMAP      hPicBackSpaceUp;        //�˸���ľ��(����)
static HBITMAP      hPicBackSpaceDown;      //�˸���ľ��(����)
static RECT         rPicNumBar;             //����������
static RECT         rPicBS;                 //�˸��������
static BtnState     ClearKeyDown = BTN_UP;  //��ǰ�Ƿ����˸������
static int          iGetPenArea;            //��¼������ѡ�������
#elif defined(MBDIALUPSTYLE_KEYBORD)
static BOOL         bKeyDown = FALSE;       //��ǰ�Ƿ��а�������
static BOOL         bClearKeyDown = FALSE;  //��ǰ�Ƿ������������
static HBITMAP      hBmpNumberLarge = NULL; //�绰�����ͼƬ
static HBITMAP      hBmpNumberSmall = NULL; //С��ͼƬ
static int          iPicNumberHeightLarge;  //����λͼ�ĸ߶�(С��)
static int          iPicNumberHeightSmall;  //����λͼ�ĸ߶�(���)
static int          iChWidthLarge;          //һ���ַ��������(���)
static int          iChWidthSmall;          //һ���ַ��������(С��)
static int          iNumberCountLarge;      //��������ʾ�����������(�������)
#endif

#if defined(MBDIAL_HAVEPROJECTMODE)
static const char * pProjectMode        = "2003"EXTENSIONSTRING"2"EXTENSIONSTRING;//����ģʽ��������
#endif

static const DWORD  dMBDialWindowStyle  = WS_BORDER|PWS_STATICBAR|WS_VISIBLE;//,���ڷ��WS_CAPTION
static const int    iPhoneNumMaxLen     = PHONENUMMAXLEN;
static const int    iMBButtonNum        = MBBTN_NUM;
static const int    iQuickDialNum       = 9;//���ٲ��ŵĸ���
static const int    iNumberCount        = 10;
static const char * pCh_Num0            = "0";
static const char * pCh_Num1            = "1";
static const char * pCh_Num2            = "2";
static const char * pCh_Num3            = "3";
static const char * pCh_Num4            = "4";
static const char * pCh_Num5            = "5";
static const char * pCh_Num4Asterick    = "4*";
static const char   cCh_Well            = '#';
static const char   cCh_Plus            = '+';
static const char   cCh_Asterisk        = '*';
static const char   cCh_Num0            = '0';
static const char   cCh_Num1            = '1';
static const char   cCh_Num2            = '2';
static const char   cCh_Num9            = '9';
static const char   cCh_None            = '\0';
static const char   cCh_ExtP            = EXTENSIONCHAR;
static const char   cCh_ExtW            = EXTENSIONCHARW;
#if defined(MBDIALUPSTYLE_TOUCH)
static const int    iPhoneNumberMaxLen  = 13;//һ����ʾ���ֵ�������
static const int    iBtn_ShowNums       = 12;//���Կ����İ�������
static const int    iNumberPicX         = 10;//һ����ʾ���ֵ�����ߵĺ���ĺ�����

static const char * pPicNumber          = MBPIC_DIALUP_NUMBER;//����ͼƬ
static const char * pPicNumBar          = MBPIC_DIALUP_NUMBAR;//�������ֵ�����ͼƬ
static const char * pPicBSUp            = MBPIC_DIALUP_BSUP;//�˸����ͼƬ(����)
static const char * pPicBSDown          = MBPIC_DIALUP_BSDOWN;//�˸����ͼƬ(����)
static const char * pPicButtonUp[]      = //���̰�ť(����״̬)
{
    MBPIC_DIALUP_BUTTONUP0,
    MBPIC_DIALUP_BUTTONUP1,
    MBPIC_DIALUP_BUTTONUP2,
    MBPIC_DIALUP_BUTTONUP3,
    MBPIC_DIALUP_BUTTONUP4,
    MBPIC_DIALUP_BUTTONUP5,
    MBPIC_DIALUP_BUTTONUP6,
    MBPIC_DIALUP_BUTTONUP7,
    MBPIC_DIALUP_BUTTONUP8,
    MBPIC_DIALUP_BUTTONUP9,
    MBPIC_DIALUP_BUTTONUPX,
    MBPIC_DIALUP_BUTTONUPJ,
    ""
};
static const char * pPicButtonDown[]    = //���̰�ť(����״̬)
{
    MBPIC_DIALUP_BUTTONDOWN0,
    MBPIC_DIALUP_BUTTONDOWN1,
    MBPIC_DIALUP_BUTTONDOWN2,
    MBPIC_DIALUP_BUTTONDOWN3,
    MBPIC_DIALUP_BUTTONDOWN4,
    MBPIC_DIALUP_BUTTONDOWN5,
    MBPIC_DIALUP_BUTTONDOWN6,
    MBPIC_DIALUP_BUTTONDOWN7,
    MBPIC_DIALUP_BUTTONDOWN8,
    MBPIC_DIALUP_BUTTONDOWN9,
    MBPIC_DIALUP_BUTTONDOWNX,
    MBPIC_DIALUP_BUTTONDOWNJ,
    ""
};
#elif defined(MBDIALUPSTYLE_KEYBORD)
static const int    iLINENUMBERNUMSMALL = 10;//һ���ַ�����(С��)
static const int    iLINENUMBERNUMLARGE = 10;//һ���ַ�����(���)
static const int    iNUMBERTOPY         = 2;//��һ�к����������
static const int    iNUMBERTOPX         = 5;//��ߵ�һ������ĺ�����
static const int    iNUMBERLINESTEP     = 3;//ÿ���ַ��ļ��

static const char * pPICNUMBERLARGE     = MBPIC_DIALUP_NUMBER;//������ֵ�ͼƬ
static const char * pPICNUMBERSMALL     = MBPIC_DIALUP_NUMBER;//С�����ֵ�ͼƬ
#endif

#if defined(KEYSWITCHTYPE_SHORT)
#define SWITCHKEY   KEYASTERICK//�л����Ǻŵļ�ֵ
static BOOL         bArriveKeySwitch;//ѡ���Ǻż���ʱ���������ʱ��,����������ڱ�ʶ�Ƿ�ﵽ��ʱ���趨��ʱ��
static const char   KeySwitch_List[] = {'*','+',EXTENSIONCHAR,EXTENSIONCHARW};
#endif
//////////////////////////////////////////////////////////////////////////
DWORD   MBDial_AppControl (int nCode, void* pInstance, WPARAM wParam, LPARAM lParam )
{
    WNDCLASS wc;
    DWORD dwRet = TRUE;

    switch (nCode)
    {
    case APP_INIT :
        hInstance = pInstance;
        break;

    case APP_INITINSTANCE :
        break;

    case APP_GETOPTION :
        switch( wParam )
        {
        case AS_ICON:
            break;

        case AS_STATE:
            if (hMBDialUp)
            {
                if (IsWindowVisible(hMBDialUp))
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

    case APP_ACTIVE :// ע��˳���Ĵ�����;       
        wc.style         = CS_OWNDC;
        wc.lpfnWndProc   = AppWndProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = NULL;
        wc.hIcon         = NULL;
        wc.hCursor       = NULL;
#if defined(MBDIALUPSTYLE_TOUCH)
        wc.hbrBackground = NULL;
#elif defined(MBDIALUPSTYLE_KEYBORD)
        wc.hbrBackground = GetStockObject(WHITE_BRUSH);
#endif
        wc.lpszMenuName  = NULL;
        wc.lpszClassName = pClassName;

        if (!RegisterClass(&wc))
            return FALSE;

        if (hMBDialUp)
        {
            if (bShow)
            {
                ShowWindow(hMBDialUp,SW_HIDE);
            }
            else
            {
                ShowWindow(hMBDialUp,SW_SHOW);
                SetFocus (hMBDialUp);
            }
            bShow = !bShow;
        }
        else
        {
            memset(cPhoneNumber,0x00,sizeof(cPhoneNumber));
            hMBDialUp = CreateWindow(pClassName, TITLECAPTION,//����  
                dMBDialWindowStyle,PLX_WIN_POSITION,NULL, NULL,NULL, NULL);

            if (NULL == hMBDialUp)
            {
                UnregisterClass(pClassName,NULL);
                return NULL;
            }
            ShowWindow(hMBDialUp, SW_SHOW);
            UpdateWindow(hMBDialUp);

            bShow = TRUE;
        }
        break;

    case APP_INACTIVE :
        SendMessage(hMBDialUp,WM_CLOSE,0,0);
        break;
    case AS_APPWND://���ش������ھ����û��ֵ���ؿա�
        dwRet = (DWORD)hMBDialUp;
        break;
    default :
        break;
    }
    return dwRet;
}
/********************************************************************
* Function   APP_CallPhone  
* Purpose   ���ò��Ž��� 
* Params    pPhoneNumber:�绰����
* Return    ���ò��Ž����Ƿ�ɹ�
* Remarks   
**********************************************************************/
BOOL    APP_CallPhone(const char * pPhoneNumber)
{
    if (APP_CallPhoneWindow(pPhoneNumber,FALSE,TRUE))
    {
        bSpecial = FALSE;//��ǰ���治��ר�ò��Ž���
        return TRUE;
    }
    return FALSE;
}
/********************************************************************
* Function   SuperAddDialNumber  
* Purpose   ����׷�Ӳ��Ž��� 
* Params    pPhoneNumber:�绰����,bSuperAdd:��ǰ�����Ƿ���׷�Ӳ��Ž���
            bCanSend:��ǰ������׷�Ӳ��Ž����ʱ���Ƿ���Բ���
* Return    ����׷�Ӳ��Ž����Ƿ�ɹ�
* Remarks   
**********************************************************************/
BOOL    SuperAddDialNumber(const char * pPhoneNumber,BOOL bSuperAdd,BOOL bCanSend)
{
    if (APP_CallPhoneWindow(pPhoneNumber,bSuperAdd,bCanSend))
    {
        bSpecial = FALSE;//��ǰ���治��ר�ò��Ž���
        return TRUE;
    }
    return FALSE;
}
/********************************************************************
* Function   APP_CallPhoneSpecial  
* Purpose   ����ר�ò��Ž���,�����������,ѡ�񲦳������뽫�Ღ��,ѡ���������ᵼ���˳�����
* Params    pPhoneNumber:�绰����,dWaitTime:ר�ò��Ž���Ĵ���ʱ��,����ֵ
* Return    ���ò��Ž����Ƿ�ɹ�
* Remarks   
**********************************************************************/
BOOL    APP_CallPhoneSpecial(const char * pPhoneNumber,DWORD dWaitTime)
{
    if (APP_CallPhoneWindow(pPhoneNumber,FALSE,TRUE))
    {
        if (dWaitTime != 0)
            SetTimer(hMBDialUp,IDC_TIMER_SPECIAL,dWaitTime,NULL);

        bSpecial = TRUE;//��ǰ������ר�ò��Ž���
        return TRUE;
    }
    return FALSE;
}
/********************************************************************
* Function   APP_CallPhoneNumber  
* Purpose   ���ò��Ź��ܲ���ֱ�Ӳ���ȥ,�����ֲ��Ž���,���ǳ������ӽ���
* Params    pPhoneNumber:�绰����
* Return    �����Ƿ񱻷��ͳ�ȥ
* Remarks   
**********************************************************************/
BOOL    APP_CallPhoneNumber(const char * pPhoneNumber)
{
    bSuperAddDial = FALSE;//��ǰ���治��׷�Ӳ��Ž���
    bSpecial      = FALSE;//��ǰ���治��ר�ò��Ž���

    switch (DealWithNumberBySend(pPhoneNumber))// DealWithNumberBySendLink
    {
    case RESULT_SUCC:       //��ɲ���
    case RESULT_GSM:        //�ú��������GSM����
    case RESUTL_USSD:       //�ú��������USSD����
    case RESUTL_PROJECTMODE://��ǰ�����ǹ���ģʽ����������
        return TRUE;
    case RESULT_NOINIT:     //��ʼ����δ���
    case RESULT_PHONENUM:   //ͨ��·������ͨ������������
    case RESULT_QUICKDIAL:  //���ٲ��Ų���û����Ӧ�ļ�¼
    case RESULT_ERRORNUMBER://����Ƿ�(���볤��Ϊ��)
    case RESULT_NONESIM:    //û��SIM����ʱ��,�����������(112��)
    case RESULT_LINKING:    //��ǰ���ڽ��в�������
    case RESULT_END:
        return FALSE;
    }
    return TRUE;
}
BOOL    APP_CallPhoneNumberSTK(const char * pPhoneNumber)
{
	SYSTEMTIME  sDialupTime;
	PHONEITEM PhoneItem;

    bSuperAddDial = FALSE;
    bSpecial      = FALSE;

    if (pPhoneNumber == NULL)
		return FALSE;
	if (strlen(pPhoneNumber) == 0)
		return FALSE;

	GetLocalTime(&sDialupTime);
	memset(&PhoneItem,0x00,sizeof(PHONEITEM));
	
	PhoneItem.Calls_Info.index = 1;
	PhoneItem.Calls_Info.Stat = CALL_STAT_ACTIVE;
	PhoneItem.Calls_Info.Mode = CALL_MODE_VOICE;
	PhoneItem.Calls_Info.Dir = CALL_DIR_ORIGINATED;
	PhoneItem.Calls_Info.Mpty = 0;
	strncpy(PhoneItem.Calls_Info.PhoneNum,pPhoneNumber,
		sizeof(PhoneItem.Calls_Info.PhoneNum) - 1);

	memcpy(&PhoneItem.TimeInDial,&sDialupTime,sizeof(SYSTEMTIME));
	MBCallingWindow(&PhoneItem,NULL,DTMF_NONE);
    return TRUE;
}
/********************************************************************
* Function   APP_CallPhoneWindow  
* Purpose   �������Ŵ��� 
* Params    pPhoneNumber:�绰����,bSuperAdd:��ǰ�����Ƿ���׷�Ӳ��Ž���
            bCanSend:��ǰ������׷�Ӳ��Ž����ʱ���Ƿ���Բ���
* Return    ���ò��Ž����Ƿ�ɹ�
* Remarks   
**********************************************************************/
static  BOOL    APP_CallPhoneWindow(const char * pPhoneNumber,BOOL bSuperAdd,BOOL bCanSend)
{
    WNDCLASS    wc;
    int         iSour;

    if (!IfValidPhoneNumber(pPhoneNumber))//����绰�����Ƿ�Ϸ�
    {
        AppMessageBox(NULL,INVALIDATENUM,TITLECAPTION,WAITTIMEOUT);
        return RESULT_ERRORNUMBER;
    }
    memset(cPhoneNumber,0x00,sizeof(cPhoneNumber));
    if (pPhoneNumber != NULL)
    {
        iSour = strlen(pPhoneNumber);
        strncpy(cPhoneNumber,pPhoneNumber,sizeof(cPhoneNumber) > iSour ? iSour + 1 : sizeof(cPhoneNumber) - 1);
    }

    bSuperAddDial     = bSuperAdd;//��ǰ�����Ƿ���׷�Ӳ���
    bAddCanSendNumber = bCanSend; //��ǰ������׷�Ӳ��Ž����ʱ���Ƿ���Բ���

    if (IsWindow(hMBDialUp))
    {
        MoveWindowToTop(hMBDialUp);        
    }
    else
    {
        wc.style         = CS_OWNDC;
        wc.lpfnWndProc   = AppWndProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = NULL;
        wc.hIcon         = NULL;
        wc.hCursor       = NULL;
#if defined(MBDIALUPSTYLE_TOUCH)
        wc.hbrBackground = NULL;
#elif defined(MBDIALUPSTYLE_KEYBORD)
        wc.hbrBackground = GetStockObject(WHITE_BRUSH);
#endif
        wc.lpszMenuName  = NULL;
        wc.lpszClassName = pClassName;

        if (!RegisterClass(&wc))
            return FALSE;

        hMBDialUp = CreateWindow(pClassName,TITLECAPTION, 
            dMBDialWindowStyle,PLX_WIN_POSITION , NULL, NULL, NULL, NULL);

        if (NULL == hMBDialUp)
        {
            UnregisterClass(pClassName,NULL);
            return FALSE;
        }        
    }

    ShowWindow(hMBDialUp, SW_SHOW);
    UpdateWindow(hMBDialUp);

    return (TRUE);
}
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    LRESULT lResult;
    HDC     hdc;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        MBDialupCreate(hWnd);
        break;

    case IDC_INIT112:
#ifndef _EMULATE_
        AppMessageBox(hWnd,EMERGENT,TITLECAPTION,WAITTIMEOUT);
#endif
        break;

    case WM_ACTIVATE:
        if (WA_INACTIVE != LOWORD(wParam))
            SetFocus(hWnd);//��������ý���
        //lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;

    case WM_CLOSE://������Ͻǵ��˳���ťʱΪ�����˳�;
        DestroyWindow(hWnd);
        break;

    case WM_DESTROY ://֪ͨ���������,�˳����˳�;
        memset(cPhoneNumber,0x00,sizeof(cPhoneNumber));
        hMBDialUp = NULL;
        UnregisterClass(pClassName,NULL);
        DlmNotify((WPARAM)PES_STCQUIT,(LPARAM)hInstance);
        break;

    case WM_PAINT :
        hdc = BeginPaint( hWnd, NULL);
        ShowPhoneNumber(hdc);
        EndPaint(hWnd, NULL);
        break;

    case WM_TIMER:
        switch (wParam)
        {
        case IDC_TIMER_SPECIAL://ר�ò��Ž���ȴ�ʱ��ļ�ʱ��
            DestroyWindow(hWnd);
            break;

        case IDC_TIMER_BACKSPACE://�����˸���ļ�ʱ��,��պ���
            KillTimer(hWnd,IDC_TIMER_BACKSPACE);
            memset(cPhoneNumber,0x00,sizeof(cPhoneNumber));
#if defined(MBDIALUPSTYLE_TOUCH)
            InvalidateRect(hWnd,&rPicNumBar,TRUE);
#elif defined(MBDIALUPSTYLE_KEYBORD)
            DestroyWindow(hWnd);//�˳�����
#endif
            break;

        case IDC_TIMER_LONGKEY://����*,0���ļ�ʱ��
            {
                int     iNumberLen;
                char    chCurrent;
                KillTimer(hWnd,IDC_TIMER_LONGKEY);
                iNumberLen = strlen(cPhoneNumber);
                chCurrent  = cPhoneNumber[iNumberLen - 1];//������һ���ַ�

                if (cCh_Asterisk == chCurrent)//���һ���ַ����Ǻ�,������дΪp
                    cPhoneNumber[iNumberLen - 1] = cCh_ExtP;
                else if (cCh_Num0 == chCurrent)//���һ���ַ�����,������дΪ+
                    cPhoneNumber[iNumberLen - 1] = cCh_Plus;
                else if (cCh_Well == chCurrent)//���һ���ַ��Ǿ���,������дΪw
                    cPhoneNumber[iNumberLen - 1] = cCh_ExtW;

#if defined(MBDIALUPSTYLE_TOUCH)
                InvalidateRect(hWnd,&rPicNumBar,TRUE);
#elif defined(MBDIALUPSTYLE_KEYBORD)
                RefreshLastNumberRect(1);
#endif
            }
            break;

        case IDC_TIMER_SHORTKEY://�̰��Ǻż��ļ�ʱ��
#if defined(KEYSWITCHTYPE_SHORT)//ѡ���Ǻż���ʱ��,�л��������ַ��ķ�ʽ:�̰�����ʽ
            bArriveKeySwitch = TRUE;
#endif
            break;
        }
        break;

    case WM_TODESKTOP:
        DestroyWindow(hWnd);
        return (!(IsWindow_MBRing() || IsWindow_DialLink() || IsWindow_MBCalling()));

#if defined(MBDIALUPSTYLE_TOUCH)
    case WM_PENUP:
        MBDialup_PenUp(hWnd,lParam);//��������̧������
        break;

    case WM_PENDOWN:
        MBDialup_PenDown(hWnd,lParam);//�������ʰ��µ����
        break;

    case WM_PENMOVE:
        GetOldAndNewRefreshArea(hWnd,lParam,iGetPenArea);//�������ƶ�ʱ��ˢ�±�Ҫ������
        break;
#endif

#if defined(MBDIALUPSTYLE_KEYBORD)
    case WM_KEYUP:
        switch(LOWORD(wParam))
        {
        case VK_F10://�˸�
            bClearKeyDown = FALSE;//��ǰû�����������
            KillTimer(hWnd,IDC_TIMER_BACKSPACE);
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
        case KEYASTERICK: //*
        case KEYWELL: //#
            bKeyDown = FALSE;//��ǰû�а�������
            KillTimer(hWnd,IDC_TIMER_LONGKEY);
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            break;
        }
        break;
#endif

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_RETURN:
        case VK_F1://����
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SET,0);
            break;
        case VK_F2://�˳�
            DestroyWindow(hWnd);
            break;
#if defined(MBDIALUPSTYLE_KEYBORD)
        case VK_F10://�˸�
            MBDialup_BackSpaceKey(hWnd);//���Ž������ü��̲��ŵĻ�,ѡ���˸���Ĳ���
            RefreshLastNumberRect(-1);//ˢ���ַ����� 
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
        case KEYASTERICK: //*
        case KEYWELL: //#
            MBDialup_Keyboard(hWnd,wParam);//���Ž������ü��̲��ŵĻ�,ѡ�񰴼��Ĳ���
            RefreshLastNumberRect(1);//ˢ���ַ����� 
            break;
#endif
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        case IDC_BUTTON_SET://����
            DialNumber(hWnd);
            break;

        case IDC_BUTTON_MIDDLE://�м�(��ϵ��)
            APP_GetOnePhone(hWnd,cPhoneNumber,sizeof(cPhoneNumber));
            break;

        case IDC_BUTTON_QUIT:
#if defined(MBDIALUPSTYLE_TOUCH)
            DestroyWindow( hWnd );//����
#elif defined(MBDIALUPSTYLE_KEYBORD)
            MBDialup_BackSpaceButton(hWnd);//�˸�
            RefreshLastNumberRect(-1);//ˢ���ַ����� 
#endif
            break;
        }
        break;
    //**************************ME�ص���Ϣ**********************
    case IDC_ME_SUPERADDDIAL://׷�Ӳ��ŵĻص���Ϣ
        break;

    case IDC_ME_SHORTCUT://��ݲ���
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALUP)
        printf("mb MBDialup ��ݲ��� wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
        if (ME_RS_SUCCESS != wParam)
            AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);

        break;
    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
/********************************************************************
* Function   DialNumber  
* Purpose   ִ�в��Ų��� 
* Params    
* Return    �Ƿ�ʼ����
* Remarks   ����TRUE�����²��Ŵ��ڹر�,���򲦺Ŵ��ڲ��ر�
**********************************************************************/
static  BOOL    DialNumber(HWND hWnd)
{
    if (bSuperAddDial)//��ǰ������׷�Ӳ��Ž�����
    {
        if (!bAddCanSendNumber)//��ǰ������׷�Ӳ��Ž����ʱ�򲻿��Բ���
            return FALSE;
    }
    switch (DealWithNumberBySend(cPhoneNumber))
    {
    case RESULT_SUCC:       //��ɲ���
    case RESULT_GSM:        //�ú��������GSM����
    case RESUTL_USSD:       //�ú��������USSD����
    case RESULT_PHONENUM:   //ͨ��·������ͨ������������
    case RESUTL_PROJECTMODE://��ǰ�����ǹ���ģʽ����������
        DestroyWindow(hWnd);
        return TRUE;
    case RESULT_NOINIT:     //��ʼ����δ���
    case RESULT_QUICKDIAL:  //���ٲ��Ų���û����Ӧ�ļ�¼
    case RESULT_ERRORNUMBER://����Ƿ�(���볤��Ϊ��)
    case RESULT_NONESIM:    //û��SIM����ʱ��,�����������(112��)
    case RESULT_LINKING:    //��ǰ���ڽ��в�������
    case RESULT_END:
        return FALSE;
    }
    DestroyWindow(hWnd);
    return TRUE;
}
/********************************************************************
* Function   DealWithNumberBySendLink  
* Purpose   ��APP_CallPhoneNumber��������,����ר�ýӿ�
* Params    pPhoneNumber:�绰����
* Return    
* Remarks   
**********************************************************************/
static  RESULT_BYSEND   DealWithNumberBySendLink(const char * pPhoneNumber)
{
    SYSTEMTIME  sDialupTime;//��¼�绰����ʱ��

    if (IsWindow_DialLink())//��������
        return RESULT_LINKING;

    if (pPhoneNumber == NULL)
        return RESULT_ERRORNUMBER;//����Ƿ�

    if (strlen(pPhoneNumber) == 0)
        return RESULT_ERRORNUMBER;//����Ƿ�

    if (!IfValidPhoneNumber(pPhoneNumber))//����绰�����Ƿ�Ϸ�
    {
        AppMessageBox(NULL,INVALIDATENUM,TITLECAPTION,WAITTIMEOUT);
        return RESULT_ERRORNUMBER;
    }

    if (GetUsedItemNum() == MAX_COUNT)//ͨ������̫����
    {
        AppMessageBox(NULL,MULTME_DIAL,TITLECAPTION,WAITTIMEOUT);//�����ٲ�����
        return RESULT_PHONENUM;//ͨ��·������ͨ������������
    }

    if (FindSIMState(FALSE) == 0)//û��SIM����ʱ����д���
    {
        if (!NumberIsEmergent(pPhoneNumber,FALSE))//�����������
        {
            AppMessageBox(NULL,EMERGENT,TITLECAPTION,WAITTIMEOUT);
            return RESULT_NONESIM;//û��SIM����ʱ��,�����������
        }
    }

    SaveMBRecordByPhoneNum(DIALED_REC,pPhoneNumber,&sDialupTime);//�����Ѳ���绰
    MBRecrodList_Refresh(DIALED_REC);

    CallMBDialLinkingWindow(pPhoneNumber,&sDialupTime);//�������ӵĽ���
    return RESULT_SUCC;//��ɲ���
}
/********************************************************************
* Function   DealWithNumberBySend  
* Purpose   ѡ�񲦺ż�֮��ִ�еĲ��� 
* Params    pPhoneNumber:�绰����
* Return    
* Remarks   
**********************************************************************/
static  RESULT_BYSEND   DealWithNumberBySend(const char * pPhoneNumber)
{
    SYSTEMTIME  sDialupTime;//��¼�绰����ʱ��
    DWORD       wPhoneNumber;
    int         iUsedItemNum,iSour;

    if (IsWindow_DialLink())//��������
        return RESULT_LINKING;

    if (pPhoneNumber == NULL)
        return RESULT_ERRORNUMBER;//����Ƿ�

    iSour = strlen(pPhoneNumber);
    if (iSour == 0)
        return RESULT_ERRORNUMBER;//����Ƿ�
    //////////////////////////////////////////////////////////////////////////
    strncpy(cPhoneNumber,pPhoneNumber,sizeof(cPhoneNumber) > iSour ? iSour + 1 : sizeof(cPhoneNumber) - 1);

    iUsedItemNum = GetUsedItemNum();//��õ�ǰͨ������
    if (0 == iUsedItemNum)//��û��ͨ����ʱ���֧�ֿ��ٲ���
    {
        if (IsWindow_DialLink() || (IsWindow_MBRing()))//��ǰ���ں���(����)һ���绰
        {
            if (-1 == ME_VoiceDial(hMBDialUp,IDC_ME_SHORTCUT,(char *)cPhoneNumber,NULL))
            {
                AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);
                return RESULT_ERRORNUMBER;
            }
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALUP)
            printf("mb MBDialup ��ǰ���ں���һ���绰��ʱ�򲦺�:%s\r\n",cPhoneNumber);
#endif
            return RESULT_ERRORNUMBER;
        }
        if ((1 == iSour) && StrIsNumber(cPhoneNumber))//��ǰ�ַ����Ƿ������ȫת��������
        {
            wPhoneNumber = atoi(cPhoneNumber);
            if ((1 <= wPhoneNumber) && (wPhoneNumber <= (DWORD)iQuickDialNum))//���ٲ��ŵĺ�������ֵ
            {
                if (0 == MB_GetDirectDial((WORD)wPhoneNumber,cPhoneNumber))//���������Ź���
                {
                    AppMessageBox(NULL,QUICKDIAL_FAIL,TITLECAPTION,WAITTIMEOUT);//���������
                    return RESULT_QUICKDIAL;//���ٲ��Ų���û����Ӧ�ļ�¼
                }
            }
        }
    }
    else//ͨ��������ѡ�����ּ�send
    {
        if (strcmp(cPhoneNumber,pCh_Num0) == 0)//���еȴ�ʱæ�����߹Ҷϱ��ֵ�ͨ��
        {
            if (!MBCalling_Shortcut(CUT_HELD,NULL))
                AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//����ʧ��

            return RESULT_GSM;//�ú��������GSM����
        }
        if (strcmp(cPhoneNumber,pCh_Num1) == 0)//�Ҷϼ����ͨ������ֱ�ӽ���һ���ȴ��ĵ绰���߼���һ�����ֵĵ绰
        {
            if (!MBCalling_Shortcut(CUT_ACTIVED,NULL))
                AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//����ʧ��

            return RESULT_GSM;
        }
        if ((iSour == 2) && (cPhoneNumber[0] == cCh_Num1))//�Ҷ�ָ����ŵļ���ͨ��
        {
            if ((cCh_Num0 <= cPhoneNumber[1]) && (cPhoneNumber[1] <= cCh_Num9))
            {
                if (!MBCalling_Shortcut(CUT_ACTIVED_X,cPhoneNumber))
                    AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//����ʧ��

                return RESULT_GSM;
            }
        }
        if (strcmp(cPhoneNumber,pCh_Num2) == 0)//�ڱ��ֺͼ����ͨ��״̬���л����ҽ��������ȴ��ĵ绰
        {
            if (!MBCalling_Shortcut(ACTIVE_SWITCH,NULL))
                AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//����ʧ��

            return RESULT_GSM;
        }
        if ((iSour == 2) && (cPhoneNumber[0] == cCh_Num2))//����ָ������������ͨ����Ϊ����
        {
            if ((cCh_Num0 <= cPhoneNumber[1]) && (cPhoneNumber[1] <= cCh_Num9))
            {
                if (!MBCalling_Shortcut(ACTIVE_SWITCH_X,cPhoneNumber))
                    AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//����ʧ��

                return RESULT_GSM;
            }
        }
        if (strcmp(cPhoneNumber,pCh_Num3) == 0)//�����ֵ�ͨ�����뼤��ͨ��
        {
            if (!MBCalling_Shortcut(ADD_TO_ACTIVE,NULL))
                AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//����ʧ��

            return RESULT_GSM;
        }
        if (strcmp(cPhoneNumber,pCh_Num4) == 0)//ͨ���е�������·�绰ʱ,ʹ����·�绰����,�û���ͨ�����˳�
        {
            if (!MBCalling_Shortcut(CALL_TRANSFER,cPhoneNumber))
                AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//����ʧ��

            return RESULT_GSM;
        }
        if ((strncmp(cPhoneNumber,pCh_Num4Asterick,strlen(pCh_Num4Asterick)) == 0) && 
            (iSour > (int)strlen(pCh_Num4Asterick)))
        {
            if (!MBCalling_Shortcut(999,cPhoneNumber))
                AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//����ʧ��

            return RESULT_GSM;
        }
        if (strcmp(cPhoneNumber,pCh_Num5) == 0)
        {
            if (!MBCalling_Shortcut(999,cPhoneNumber))
                AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//����ʧ��

            return RESULT_GSM;
        }
        if ((IsWindow_DialLink()) || (IsWindow_MBRing()))
        {   //��ǰ���ں���һ���绰�������ڽ����绰
            if (!MBCalling_Shortcut(999,cPhoneNumber))
                AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//����ʧ��

            return RESULT_GSM;
        }
    }

    if (GSMDealWith(cPhoneNumber,FALSE))//��ǰ�������GSM����,������GSM����
        return RESULT_GSM;//�ú��������GSM����

    if (USSDDealWith(cPhoneNumber,iUsedItemNum != 0))//��ǰ�������USSD����,������USSD����
        return RESUTL_USSD;

#if defined(MBDIAL_HAVEPROJECTMODE)
    if (strcmp(cPhoneNumber,pProjectMode) == 0)//��ǰ�����ǹ���ģʽ����������
    {
        CallProjectATWindow(NULL);
        return RESUTL_PROJECTMODE;
    }
#endif

    if (iUsedItemNum == MAX_COUNT)//ͨ������̫����
    {
        AppMessageBox(NULL,MULTME_DIAL,TITLECAPTION,WAITTIMEOUT);//�����ٲ�����
        return RESULT_PHONENUM;//ͨ��·������ͨ������������
    }

    if (!IfValidPhoneNumber(cPhoneNumber))//����绰�����Ƿ�Ϸ�
    {
        AppMessageBox(NULL,INVALIDATENUM,TITLECAPTION,WAITTIMEOUT);
        return RESULT_ERRORNUMBER;
    }

    if (FindSIMState(FALSE) == 0)//û��SIM����ʱ����д���
    {
        if (!NumberIsEmergent(cPhoneNumber,FALSE))//�����������
        {
            AppMessageBox(NULL,EMERGENT,TITLECAPTION,WAITTIMEOUT);
            return RESULT_NONESIM;//û��SIM����ʱ��,�����������
        }
    }

    SaveMBRecordByPhoneNum(DIALED_REC,cPhoneNumber,&sDialupTime);//�����Ѳ���绰
    MBRecrodList_Refresh(DIALED_REC);

    CallMBDialLinkingWindow(cPhoneNumber,&sDialupTime);//�������ӵĽ���
    return RESULT_SUCC;//��ɲ���
}
/********************************************************************
* Function   DealWithNumber  
* Purpose   ����GSM��������� 
* Params    
* Return    �����˷���TRUE,δ�ҵ��������Ŀ����FALSE
* Remarks   
**********************************************************************/
static  BOOL    DealWithNumber(HWND hWnd)
{
    if (!bSuperAddDial)//��ǰ����׷�Ӳ��Ž���(ֻ�п�ʼ���ŵ�ʱ��Ų�ѯ��ϵ�˲���)
    {
        if (GSM_PhoneBookDealWith(cPhoneNumber))
            return TRUE;
    }
    return (GSMDealWith(cPhoneNumber,TRUE));
}
/********************************************************************
* Function   GetDialitemBtnIndex  
* Purpose    ����ĳ���ַ�(1,2,*,#...)��dialitembtn�����е�λ��
* Params    �����ַ�
* Return     dialitembtn�����е�λ��,-1��ʾ����ʧ��
* Remarks   
**********************************************************************/
static  int     GetDialitemBtnIndex(char ch)
{
    int i;
    for (i = 0;i < iMBButtonNum;i++)
    {
        if (ch == dialitembtn[i].ch_short)
            return i;
    }
    return -1;
}
/********************************************************************
* Function   MessageMBDialUp  
* Purpose    �򲦺Ž��淢����Ϣ
* Params    
* Return    
* Remarks   
**********************************************************************/
void    MessageMBDialUp( UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    if ((hMBDialUp != NULL) && (IsWindow(hMBDialUp)))
        PostMessage(hMBDialUp,wMsgCmd,wParam,lParam);
}
/********************************************************************
* Function   IsWindow_Dialup  
* Purpose    ��ǰ�Ƿ���ڲ��Ž��� 
* Params     
* Return     
* Remarks    
**********************************************************************/
BOOL    IsWindow_Dialup(void)
{
    if (hMBDialUp == NULL)
        return FALSE;
    return (IsWindow(hMBDialUp));
}
#if defined(MBDIALUPSTYLE_KEYBORD)
/********************************************************************
* Function   GetDialitemBtnKey  
* Purpose   ���ĳ����ֵ��dialitembtn�����е�λ��
* Params    VK_1..
* Return    dialitembtn�����е�λ��,-1��ʾ����ʧ��
* Remarks   
**********************************************************************/
static  int     GetDialitemBtnKey(unsigned int iVKey)
{
    int i;
    for (i = 0;i < iMBButtonNum;i++)
    {
        if (iVKey == dialitembtn[i].iVKey)
            return i;
    }
    return -1;
}
/********************************************************************
* Function   RefreshLastNumberRect  
* Purpose   ˢ���ַ����� 
* Params    iAdd_Del:���µ�ǰˢ�µĲ���������(�޸�)�ַ�����ɾ���ַ�
            iAdd_Del>0�����ַ�,iAdd_Del<0,ɾ���ַ�
* Return    
* Remarks   ֻˢ������ӵ��ַ����ڵ�����
**********************************************************************/
static  void    RefreshLastNumberRect(int iAdd_Del)
{
    RECT rRefresh;
    int iNumberX,iNumberY,iPhoneLen;

    if (0 == iAdd_Del)
    {
        InvalidateRect(hMBDialUp,NULL,TRUE);
        return;
    }
    iPhoneLen = strlen(cPhoneNumber);

    if ((iPhoneLen == 0) ||
        (iAdd_Del > 0 && iPhoneLen == iNumberCountLarge + 1) || //��ǰ���ֵĸ�����������ַ���ʾ����(��ǰ���������ַ�)
        (iAdd_Del < 0 && iPhoneLen == iNumberCountLarge))       //��ǰ���ֵĸ������ڴ���ַ���ʾ����(��ǰ����ɾ���ַ�)
    {
        InvalidateRect(hMBDialUp,NULL,TRUE);
        return;
    }

    if (iPhoneLen > iNumberCountLarge)//�������>��������ʾ�����������(���)
    {
        if (iAdd_Del > 0)
            iPhoneLen--;

        iNumberX = iNUMBERTOPX + (iPhoneLen % iLINENUMBERNUMSMALL) * iChWidthSmall;
        iNumberY = iNUMBERTOPY + (iPhoneLen / iLINENUMBERNUMSMALL) * (iPicNumberHeightSmall + iNUMBERLINESTEP);

        rRefresh.top      = iNumberY;
        rRefresh.left     = iNumberX;
        rRefresh.right    = iNumberX + iChWidthSmall;
        rRefresh.bottom   = iNumberY + iPicNumberHeightSmall;
    }
    else
    {
        if (iAdd_Del > 0)
            iPhoneLen--;

        iNumberX = iNUMBERTOPX + (iPhoneLen % iLINENUMBERNUMLARGE) * iChWidthLarge;
        iNumberY = iNUMBERTOPY + (iPhoneLen / iLINENUMBERNUMLARGE) * (iPicNumberHeightLarge + iNUMBERLINESTEP);

        rRefresh.top      = iNumberY;
        rRefresh.left     = iNumberX;
        rRefresh.right    = iNumberX + iChWidthLarge;
        rRefresh.bottom   = iNumberY + iPicNumberHeightLarge;
    }
    InvalidateRect(hMBDialUp,&rRefresh,TRUE);
}
/********************************************************************
* Function   MBDialup_Keyboard  
* Purpose    ���Ž������ü��̲��ŵĻ�,ѡ�񰴼��Ĳ��� 
* Params     
* Return     
* Remarks    
**********************************************************************/
static  void    MBDialup_Keyboard(HWND hWnd,WPARAM wParam)
{
    int  iNumberLen,iChIndex;
    char chCurrent;

    if (bSpecial)//��ǰ������ר�ò��Ž���
    {
        DestroyWindow(hWnd);
        return;
    }
    if (bKeyDown)//��ǰ�Ѿ��а���������,������δ̧��
        return;

    bKeyDown    = TRUE;//��ǰ�а�������
    iNumberLen  = strlen(cPhoneNumber);
    iChIndex    = GetDialitemBtnKey(LOWORD(wParam));//���ĳ����ֵ��dialitembtn�����е�λ��

    if (iNumberLen < iPhoneNumMaxLen)
    {
#if defined(KEYSWITCHTYPE_LONG)
        //���ĳ����ֵ��dialitembtn�����е�λ��        
        cPhoneNumber[iNumberLen]     = dialitembtn[iChIndex].ch_short;
        cPhoneNumber[iNumberLen + 1] = cCh_None;

        KillTimer(hWnd,IDC_TIMER_LONGKEY);
        if ((cCh_Asterisk == dialitembtn[iChIndex].ch_short) ||
            (cCh_Num0     == dialitembtn[iChIndex].ch_short) ||
            (cCh_Well     == dialitembtn[iChIndex].ch_short))
            SetTimer(hWnd,IDC_TIMER_LONGKEY,TIMER_LONGKEY,NULL);//����������(*,0)��ʱ��

#elif defined(KEYSWITCHTYPE_SHORT)//ѡ���Ǻż���ʱ��,�л��������ַ��ķ�ʽ:�̰�����ʽ
        if (LOWORD(wParam) == SWITCHKEY)//ѡ��ļ����л���
        {
            KillTimer(hWnd,IDC_TIMER_SHORTKEY);
            SetTimer(hWnd,IDC_TIMER_SHORTKEY,TIMER_SHORTKEY,NULL);//�����̰�����ʱ��

            if (bArriveKeySwitch)//�����µ��ַ�
            {
                bArriveKeySwitch             = FALSE;
                cPhoneNumber[iNumberLen]     = dialitembtn[iChIndex].ch_short;
                cPhoneNumber[iNumberLen + 1] = cCh_None;
            }
            else//��δ����̰�����ʱʱ��,��Ҫ��ʾ��һ���л��ַ�
            {
                iChIndex = InCharArray(cPhoneNumber[iNumberLen - 1],
                    KeySwitch_List,sizeof(KeySwitch_List));

                if (iChIndex < sizeof(KeySwitch_List) - 1)
                    cPhoneNumber[iNumberLen - 1] = KeySwitch_List[iChIndex + 1];
                else
                    cPhoneNumber[iNumberLen - 1] = KeySwitch_List[0];

                cPhoneNumber[iNumberLen] = cCh_None;
            }
        }
        else//ѡ��ļ������л���,������һ���ַ�
        {
            cPhoneNumber[iNumberLen]     = dialitembtn[iChIndex].ch_short;
            cPhoneNumber[iNumberLen + 1] = cCh_None;
        }
#endif        
    }
    else
    {
        return;
    }

    if ((bSuperAddDial) &&        //��ǰ������׷�Ӳ��Ž���,��Ҫ��������ַ��������ͳ�ȥ
        (!GetListCurCalling()) && //û���е�ǰͨ��
        (!IsAllType_Specific(CALL_STAT_HELD)))//��ǰ����ȫ������
    {
        iNumberLen = strlen(cPhoneNumber);
        chCurrent  = cPhoneNumber[iNumberLen - 1];//������һ���ַ�
        ME_SendDTMF(hWnd,IDC_ME_SUPERADDDIAL,chCurrent);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALUP)
        printf("mb MBDialup ����׷�Ӻ���:%c\r\n",chCurrent);
#endif
    }
    if (DealWithNumber(hWnd))//����GSM����
    {
        memset(cPhoneNumber,0x00,sizeof(cPhoneNumber));
        DestroyWindow(hWnd);
        return;
    }
    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_SET,1),
        (iNumberLen == 0) ? (LPARAM)"" : (LPARAM)LEFTBUTTONTEXT);
}
/********************************************************************
* Function   MBDialup_BackSpaceKey  
* Purpose    ���Ž������ü��̲��ŵĻ�,ѡ���˸���Ĳ���
* Params     
* Return     
* Remarks    
**********************************************************************/
static  void    MBDialup_BackSpaceKey(HWND hWnd)
{
    int iNumberLen;
    if (bSpecial)//��ǰ������ר�ò��Ž���
    {
        DestroyWindow(hWnd);
        return;
    }
    if (bClearKeyDown)//��ǰ�����������
        return;
    bClearKeyDown = TRUE;//��ǰ�����������
    KillTimer(hWnd,IDC_TIMER_BACKSPACE);
    SetTimer(hWnd,IDC_TIMER_BACKSPACE,TIMER_BACKSPACE,NULL);//����������(�˸��)��ʱ��

    iNumberLen = strlen(cPhoneNumber);
    if (iNumberLen > 0)
        cPhoneNumber[iNumberLen - 1] = cCh_None;

    if (strlen(cPhoneNumber) == 0)
        DestroyWindow(hWnd);
}
/********************************************************************
* Function   MBDialup_BackSpaceButton  
* Purpose    ���Ž������ü��̲��ŵĻ�,ѡ���˸�ť�Ĳ���
* Params     
* Return     
* Remarks    
**********************************************************************/
static  void    MBDialup_BackSpaceButton(HWND hWnd)
{
    int iNumberLen;
    if (bSpecial)//��ǰ������ר�ò��Ž���
    {
        DestroyWindow(hWnd);
        return;
    }
    iNumberLen = strlen(cPhoneNumber);
    if (iNumberLen > 0)
        cPhoneNumber[iNumberLen - 1] = cCh_None;

    if (strlen(cPhoneNumber) == 0)
        DestroyWindow(hWnd);
}
#endif
#if defined(MBDIALUPSTYLE_TOUCH)
/********************************************************************
* Function   GetPenAreaInfo  
* Purpose   ���ݴ����ʵ����λ��,�����������ĸ����� 
* Params    pPoint:�����ʵ��/̧���λ��,pRefresh:������Ҫˢ�µ�����
            BtnState:������ʾ����
* Return    ��������ı��,AREAINFO_BS:ѡ����������˸��,0-11:��������,AREAINFO_NONE:��������
* Remarks   
**********************************************************************/
static  int     GetPenAreaInfo(HWND hWnd,POINT Point,BtnState BState)
{
    int i,iResult = AREAINFO_NONE;
    RECT rPicBtn;

    if (BState == BTN_UP)//������̧��
    {
        bPenDown = FALSE;//û�д����ʰ���
        if (PtInRect(&rPicBS,Point))//̧���������˸��
        {
            InvalidateRect(hWnd,&rPicBS,TRUE);
            ClearKeyDown = BTN_UP;
            iResult      = AREAINFO_BS;
        }
        for (i = 0;i < iBtn_ShowNums;i++)
        {
            memcpy(&rPicBtn,&dialitembtn[i].rPicBtn,sizeof(RECT));
            if ((i == 0) || (i == 10) || (i == 11))//0,*,#�ż������������⴦��
                rPicBtn.bottom -= 10;

            if (PtInRect(&rPicBtn,Point))//̧�������Ǽ�������
            {
                InvalidateRect(hWnd,&dialitembtn[i].rPicBtn,TRUE);
                dialitembtn[i].iState = BTN_UP;
                iResult               = i;
            }
        }
        if (ClearKeyDown == BTN_DOWN)//������ڻ������˸����������
        {
            InvalidateRect(hWnd,&rPicBS,TRUE);
            ClearKeyDown = BTN_UP;
        }
        for (i = 0;i < iBtn_ShowNums;i++)
        {
            if (dialitembtn[i].iState == BTN_DOWN)//������ڻ����ּ����м���������
            {
                InvalidateRect(hWnd,&dialitembtn[i].rPicBtn,TRUE);
                dialitembtn[i].iState = BTN_UP;
            }
        }
    }
    else//�����ʰ���
    {
        if (PtInRect(&rPicBS,Point))//����������˸��
        {
            InvalidateRect(hWnd,&rPicBS,TRUE);
            ClearKeyDown = BTN_DOWN;
            iResult      = AREAINFO_BS;

            for (i = 0;i < iBtn_ShowNums;i++)
            {
                if (dialitembtn[i].iState == BTN_DOWN)//������ڻ����ּ����м���������
                {
                    InvalidateRect(hWnd,&dialitembtn[i].rPicBtn,TRUE);
                    dialitembtn[i].iState = BTN_UP;
                }
            }
        }
        for (i = 0;i < iBtn_ShowNums;i++)
        {
            memcpy(&rPicBtn,&dialitembtn[i].rPicBtn,sizeof(RECT));
            if ((i == 0) || (i == 10) || (i == 11))//0,*,#�ż������������⴦��
                rPicBtn.bottom -= 10;

            if (PtInRect(&rPicBtn,Point))//���������Ǽ�������
            {
                InvalidateRect(hWnd,&dialitembtn[i].rPicBtn,TRUE);
                dialitembtn[i].iState = BTN_DOWN;
                iResult               = i;
                if (ClearKeyDown == BTN_DOWN)//������ڻ������˸����������
                {
                    InvalidateRect(hWnd,&rPicBS,TRUE);
                    ClearKeyDown = BTN_UP;
                }
            }
        }
        if (iResult == AREAINFO_NONE)//��������ǲ��Ž������������,���ǲ�������ˢ�µĲ���
        {
            if (ClearKeyDown == BTN_DOWN)//������ڻ������˸����������
            {
                InvalidateRect(hWnd,&rPicBS,TRUE);
                ClearKeyDown = BTN_UP;
            }
            for (i = 0;i < iBtn_ShowNums;i++)
            {
                if (dialitembtn[i].iState == BTN_DOWN)//������ڻ����ּ����м���������
                {
                    InvalidateRect(hWnd,&dialitembtn[i].rPicBtn,TRUE);
                    dialitembtn[i].iState = BTN_UP;
                }
            }
        }
    }    
    return iResult;
}
/********************************************************************
* Function   SetRefreshNumbar  
* Purpose   ����绰�����ʱ��,ˢ����Ӧ������
* Params    
* Return    
* Remarks   
**********************************************************************/
static  void    SetRefreshNumbar(HWND hWnd)
{
    int iPhoneLen,iNumberY;
    RECT rectNumber;

    iPhoneLen = strlen(cPhoneNumber);
    iNumberY  = (rPicNumBar.bottom - rPicNumBar.top - iPicNumHeight) / 2;
    if (iPhoneLen >= iPhoneNumberMaxLen)//�����������һ����ʾ������
    {
        SetRect(&rectNumber,iNumberPicX,iNumberY,
            iNumberPicX + iPhoneNumberMaxLen * iPicNumChWidth,
            iNumberY + iPicNumHeight);
    }
    else
    {
        SetRect(&rectNumber,
            (iPhoneLen - 1) * iPicNumChWidth + iNumberPicX,
            iNumberY,
            (iPhoneLen - 1) * iPicNumChWidth + iNumberPicX + iPicNumChWidth,
            iNumberY + iPicNumHeight);
    }
    InvalidateRect(hWnd,&rectNumber,TRUE);
}
/********************************************************************
* Function   GetOldAndNewRefreshArea  
* Purpose   �������ƶ�ʱ��ˢ�±�Ҫ������
* Params    lParam:��ǰ����������λ��,iOldPenArea:ԭ����������������
* Return    TRUE:���������������Ѿ��ı�,FALSE:��������������û�иı�
* Remarks   
**********************************************************************/
static  BOOL    GetOldAndNewRefreshArea(HWND hWnd,LPARAM lParam,int iOldPenArea)
{
    int iCurPenArea,i;
    POINT Point;

    Point.x     = LOWORD(lParam);
    Point.y     = HIWORD(lParam);
    iCurPenArea = GetPenAreaInfo(hWnd,Point,BTN_DOWN);//��õ�ǰ��������������

    if (iCurPenArea == iOldPenArea)//����û�иı�,
        return FALSE;

    if (iOldPenArea == AREAINFO_BS)//������ԭ���ڲ��Ž�����˸������
    {
        ClearKeyDown = BTN_UP;
        bPenDown     = FALSE;
        GetPenAreaInfo(hWnd,Point,BTN_UP);//���ݴ����ʵ����λ��,�����������ĸ�����
        KillTimer(hWnd,IDC_TIMER_BACKSPACE);
        InvalidateRect(hWnd,&rPicBS,TRUE);//ˢ���˸������
    }
    if ((0 <= iOldPenArea) && (iOldPenArea < iBtn_ShowNums))//������ԭ�������ּ�����,��Ҫˢ�����ּ�����
    {
        for (i = 0;i < iBtn_ShowNums;i++)
        {
            if (dialitembtn[i].iState == BTN_DOWN)//��ǰ�а�������
            {
                bPenDown              = FALSE;
                dialitembtn[i].iState = BTN_UP;//��־Ϊ̧��״̬
                KillTimer(hWnd,IDC_TIMER_LONGKEY);
                InvalidateRect(hWnd,&dialitembtn[i].rPicBtn,TRUE);
            }
        }
    }
    if (iCurPenArea == AREAINFO_NONE)//��õ�ǰ�������ڲ��Ž������������,���ǲ�������ˢ�µĲ���
    {
        bPenDown = FALSE;
        KillTimer(hWnd,IDC_TIMER_BACKSPACE);
        KillTimer(hWnd,IDC_TIMER_LONGKEY);
        if (ClearKeyDown == BTN_DOWN)//��ǰ�����˸������
        {
            ClearKeyDown = BTN_UP;
            InvalidateRect(hWnd,&rPicBS,TRUE);//ˢ���˸������
        }
        for (i = 0;i < iBtn_ShowNums;i++)
        {
            if (dialitembtn[i].iState == BTN_DOWN)//��ǰ�а�������
            {
                dialitembtn[i].iState = BTN_UP;//��־Ϊ̧��״̬
                InvalidateRect(hWnd,&dialitembtn[i].rPicBtn,TRUE);
            }
        }
    }
    return FALSE;
}
/********************************************************************
* Function   MBDialup_PenDown  
* Purpose    �������ʰ��µ���� 
* Params     
* Return     
* Remarks    
**********************************************************************/
static  void    MBDialup_PenDown(HWND hWnd,LPARAM lParam)
{
    POINT   point;
    int     iNumberLen;
    char    chCurrent;

    if (bSpecial)//��ǰ������ר�ò��Ž���,ֱ���˳�
    {
        memset(cPhoneNumber,0x00,sizeof(cPhoneNumber));
        DestroyWindow(hWnd);
        return;
    }
    if (bPenDown)//��ǰ�Ѿ��бʰ���
        return;

    bPenDown    = TRUE;//��ǰ�бʰ���
    point.x     = LOWORD(lParam);
    point.y     = HIWORD(lParam);
    iGetPenArea = GetPenAreaInfo(hWnd,point,BTN_DOWN);//���ݴ����ʵ����λ��,�����������ĸ�����
    if (AREAINFO_NONE == iGetPenArea)//���Ž������������,���ǲ�������ˢ�µĲ���
        return;

    iNumberLen = strlen(cPhoneNumber);
    if (AREAINFO_BS == iGetPenArea)//ѡ�����˸��
    {
        if (iNumberLen > 0)
            cPhoneNumber[iNumberLen - 1] = cCh_None;

        KillTimer(hWnd,IDC_TIMER_BACKSPACE);
        SetTimer(hWnd,IDC_TIMER_BACKSPACE,TIMER_BACKSPACE,NULL);//����������(�˸��)��ʱ��
        SetRefreshNumbar(hWnd);//����绰�����ʱ��,ˢ����Ӧ������
        return;
    }
    //ѡ���������
    if ((0 <= iGetPenArea) && (iGetPenArea < iBtn_ShowNums))
    {
        if (iNumberLen < iPhoneNumMaxLen)
        {
#if defined(KEYSWITCHTYPE_LONG)

            cPhoneNumber[iNumberLen]     = dialitembtn[iGetPenArea].ch_short;
            cPhoneNumber[iNumberLen + 1] = cCh_None;

            KillTimer(hWnd,IDC_TIMER_LONGKEY);
            if ((cCh_Asterisk == dialitembtn[iGetPenArea].ch_short) ||
                (cCh_Num0     == dialitembtn[iGetPenArea].ch_short) ||
                (cCh_Well     == dialitembtn[iGetPenArea].ch_short))
                SetTimer(hWnd,IDC_TIMER_LONGKEY,TIMER_LONGKEY,NULL);//����������(����,*,#)��ʱ��

#elif defined(KEYSWITCHTYPE_SHORT)//ѡ���Ǻż���ʱ��,�л��������ַ��ķ�ʽ:�̰�����ʽ
            if (10 == iGetPenArea)//�Ǻż����������λ��
            {
                int iChIndex;
                KillTimer(hWnd,IDC_TIMER_SHORTKEY);
                SetTimer(hWnd,IDC_TIMER_SHORTKEY,TIMER_SHORTKEY,NULL);//�����̰�����ʱ��
                if (bArriveKeySwitch)//�����µ��ַ�
                {
                    bArriveKeySwitch             = FALSE;
                    cPhoneNumber[iNumberLen]     = dialitembtn[iGetPenArea].ch_short;
                    cPhoneNumber[iNumberLen + 1] = cCh_None;
                }
                else//��δ����̰�����ʱʱ��,��Ҫ��ʾ��һ���л��ַ�
                {
                    iChIndex = InCharArray(cPhoneNumber[iNumberLen - 1],
                        KeySwitch_List,sizeof(KeySwitch_List));

                    if (iChIndex < sizeof(KeySwitch_List) - 1)
                        cPhoneNumber[iNumberLen - 1] = KeySwitch_List[iChIndex + 1];
                    else
                        cPhoneNumber[iNumberLen - 1] = KeySwitch_List[0];

                    cPhoneNumber[iNumberLen] = cCh_None;
                }
            }
            else//ѡ��ļ������л���,������һ���ַ�
            {
                cPhoneNumber[iNumberLen]     = dialitembtn[iGetPenArea].ch_short;
                cPhoneNumber[iNumberLen + 1] = cCh_None;
            }
#endif
        }
        else
        {
            return;
        }
        if ((bSuperAddDial) && //��ǰ������׷�Ӳ��Ž���,��Ҫ��������ַ��������ͳ�ȥ
            (!GetListCurCalling()) && //û���е�ǰͨ��
            (!IsAllType_Specific(CALL_STAT_HELD)))//��ǰ����ȫ������
        {
            iNumberLen  = strlen(cPhoneNumber);
            chCurrent   = cPhoneNumber[iNumberLen - 1];//������һ���ַ�
            ME_SendDTMF(hWnd,IDC_ME_SUPERADDDIAL,chCurrent);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALUP)
            printf("mb MBDialup ����׷�Ӻ���:%c\r\n",chCurrent);
#endif
        }

        if (DealWithNumber(hWnd))//����GSM����
        {
            memset(cPhoneNumber,0x00,sizeof(cPhoneNumber));
            DestroyWindow(hWnd);
            return;
        }
        SetRefreshNumbar(hWnd);//����绰�����ʱ��,ˢ����Ӧ������
    }
}
/********************************************************************
* Function   MBDialup_PenUp  
* Purpose    ��������̧������ 
* Params     
* Return     
* Remarks    
**********************************************************************/
static  void    MBDialup_PenUp(HWND hWnd,LPARAM lParam)
{
    POINT point;

    bPenDown    = FALSE;//��ǰû�д����ʰ���
    point.x     = LOWORD(lParam);
    point.y     = HIWORD(lParam);

    iGetPenArea = GetPenAreaInfo(hWnd,point,BTN_UP);//���ݴ����ʵ����λ��,�����������ĸ�����
    if (AREAINFO_NONE == iGetPenArea)//���Ž������������,���ǲ�������ˢ�µĲ���
        return;
    if (AREAINFO_BS == iGetPenArea)//ѡ�����˸��
    {
        KillTimer(hWnd,IDC_TIMER_BACKSPACE);
        return;
    }
    //ѡ���������
    if ((0 <= iGetPenArea) && (iGetPenArea < iBtn_ShowNums))
    {
        KillTimer(hWnd,IDC_TIMER_LONGKEY);
        //if (strlen(cPhoneNumber) < iPhoneNumMaxLen)
          //  SetRefreshNumbar(hWnd);//����绰�����ʱ��,ˢ����Ӧ������
    }
}
#endif
/********************************************************************
* Function   ShowPhoneNumber  
* Purpose   ��ʾ���� 
* Params    
* Return    
* Remarks   
**********************************************************************/
static  void    ShowPhoneNumber(HDC hdc)
{
    int i,iPhoneLen,index;
    int iNumberX,iNumberY,iDisplayNumber;
#if defined(MBDIALUPSTYLE_TOUCH)
    //��ʾ�����
    StretchBlt(hdc,
        rPicNumBar.left,rPicNumBar.top,
        rPicNumBar.right - rPicNumBar.left,rPicNumBar.bottom - rPicNumBar.top,
        (HDC)hPicNumBar,
        0,0,rPicNumBar.right - rPicNumBar.left,
        rPicNumBar.bottom - rPicNumBar.top,SRCCOPY);
    //��ʾ�˸��
    if (BTN_UP == ClearKeyDown)//û���˸������
    {
        StretchBlt(hdc,
            rPicBS.left,rPicBS.top,
            rPicBS.right - rPicBS.left,rPicBS.bottom - rPicBS.top,
            (HDC)hPicBackSpaceUp,
            0,0,rPicBS.right - rPicBS.left,rPicBS.bottom - rPicBS.top,SRCCOPY);
    }
    else//���˸������
    {
        StretchBlt(hdc,
            rPicBS.left,rPicBS.top,
            rPicBS.right - rPicBS.left,rPicBS.bottom - rPicBS.top,
            (HDC)hPicBackSpaceDown,
            0,0,rPicBS.right - rPicBS.left,rPicBS.bottom - rPicBS.top,SRCCOPY);
    }
    //��ʾ����
    for (i = 0;i < iBtn_ShowNums;i++)
    {
        if (dialitembtn[i].iState == BTN_UP)//�ü�û�а���
        {
            StretchBlt(hdc,
                dialitembtn[i].rPicBtn.left,dialitembtn[i].rPicBtn.top,
                dialitembtn[i].rPicBtn.right - dialitembtn[i].rPicBtn.left,
                dialitembtn[i].rPicBtn.bottom - dialitembtn[i].rPicBtn.top,
                (HDC)dialitembtn[i].hBtnUp,
                0,0,
                dialitembtn[i].rPicBtn.right - dialitembtn[i].rPicBtn.left,
                dialitembtn[i].rPicBtn.bottom - dialitembtn[i].rPicBtn.top,SRCCOPY);
        }
        else
        {
            StretchBlt(hdc,
                dialitembtn[i].rPicBtn.left,dialitembtn[i].rPicBtn.top,
                dialitembtn[i].rPicBtn.right - dialitembtn[i].rPicBtn.left,
                dialitembtn[i].rPicBtn.bottom - dialitembtn[i].rPicBtn.top,
                (HDC)dialitembtn[i].hBtnDown,
                0,0,
                dialitembtn[i].rPicBtn.right - dialitembtn[i].rPicBtn.left,
                dialitembtn[i].rPicBtn.bottom - dialitembtn[i].rPicBtn.top,SRCCOPY);
        }
    }
    //��ʾ����
    iPhoneLen = strlen(cPhoneNumber);
    if (iPhoneLen == 0)
        return;
    if (iPhoneLen > iPhoneNumberMaxLen)//ֻ��ʾ��ߵĺ���
        i = iPhoneLen - iPhoneNumberMaxLen;
    else
        i = 0;

    iDisplayNumber = 0;//�Ѿ���ʾ���ַ�����
    iNumberY       = (rPicNumBar.bottom - rPicNumBar.top - iPicNumHeight) / 2;
    for (;i < iPhoneLen;i++)
    {
        iNumberX = iDisplayNumber * iPicNumChWidth + iNumberPicX;
        index    = GetDialitemBtnIndex(cPhoneNumber[i]);
        StretchBlt(hdc,
            iNumberX,iNumberY,
            dialitembtn[index].rPicNum.right - dialitembtn[index].rPicNum.left,
            dialitembtn[index].rPicNum.bottom - dialitembtn[index].rPicNum.top,//Ŀ�ľ���
            (HDC)hPicNumber,
            dialitembtn[index].rPicNum.left,dialitembtn[index].rPicNum.top,
            dialitembtn[index].rPicNum.right - dialitembtn[index].rPicNum.left,
            dialitembtn[index].rPicNum.bottom - dialitembtn[index].rPicNum.top,//ͼ��ԭ����
            SRCCOPY);//��λͼ��Դ���ο�����Ŀ�ľ��Σ���չ��ѹ��λͼ��ʹ���ʺ�Ŀ�ľ��εķ�Χ

        iDisplayNumber++;
    }
#elif defined(MBDIALUPSTYLE_KEYBORD)
    iPhoneLen      = strlen(cPhoneNumber);//�������
    iDisplayNumber = 0;//�Ѿ���ʾ���ַ�����

    if (iPhoneLen > iNumberCountLarge)//�������>��������ʾ�����������(���)
    {
        for (i = 0;i < iPhoneLen;i++)//��Ҫ��ʾС���ַ�
        {
            iNumberX = iNUMBERTOPX + (iDisplayNumber % iLINENUMBERNUMSMALL) * iChWidthSmall;
            iNumberY = iNUMBERTOPY + (iDisplayNumber / iLINENUMBERNUMSMALL) * (iPicNumberHeightSmall + iNUMBERLINESTEP);

            index    = GetDialitemBtnIndex(cPhoneNumber[i]);
            StretchBlt( hdc,
                iNumberX,iNumberY,
                dialitembtn[index].rBtnPicSmall.right - dialitembtn[index].rBtnPicSmall.left,
                dialitembtn[index].rBtnPicSmall.bottom - dialitembtn[index].rBtnPicSmall.top,//Ŀ�ľ���
                (HDC)hBmpNumberSmall,
                dialitembtn[index].rBtnPicSmall.left,dialitembtn[index].rBtnPicSmall.top,
                dialitembtn[index].rBtnPicSmall.right - dialitembtn[index].rBtnPicSmall.left,
                dialitembtn[index].rBtnPicSmall.bottom - dialitembtn[index].rBtnPicSmall.top,//ͼ��ԭ����
                SRCCOPY);//��λͼ��Դ���ο�����Ŀ�ľ��Σ���չ��ѹ��λͼ��ʹ���ʺ�Ŀ�ľ��εķ�Χ
            iDisplayNumber++;
        }
    }
    else
    {
        for (i = 0;i < iPhoneLen;i++)//��Ҫ��ʾ����ַ�
        {
            iNumberX = iNUMBERTOPX + (iDisplayNumber % iLINENUMBERNUMLARGE) * iChWidthLarge;
            iNumberY = iNUMBERTOPY + (iDisplayNumber / iLINENUMBERNUMLARGE) * (iPicNumberHeightLarge + iNUMBERLINESTEP);

            index    = GetDialitemBtnIndex(cPhoneNumber[i]);
            StretchBlt( hdc,
                iNumberX,iNumberY,
                dialitembtn[index].rBtnPicLarge.right - dialitembtn[index].rBtnPicLarge.left,
                dialitembtn[index].rBtnPicLarge.bottom - dialitembtn[index].rBtnPicLarge.top,//Ŀ�ľ���
                (HDC)hBmpNumberLarge,
                dialitembtn[index].rBtnPicLarge.left,dialitembtn[index].rBtnPicLarge.top,
                dialitembtn[index].rBtnPicLarge.right - dialitembtn[index].rBtnPicLarge.left,
                dialitembtn[index].rBtnPicLarge.bottom - dialitembtn[index].rBtnPicLarge.top,//ͼ��ԭ����
                SRCCOPY);//��λͼ��Դ���ο�����Ŀ�ľ��Σ���չ��ѹ��λͼ��ʹ���ʺ�Ŀ�ľ��εķ�Χ
            iDisplayNumber++;
        }
    }
#endif
}
/********************************************************************
* Function   MBDialupCreate  
* Purpose    ��ʼ�����Ž��� 
* Params     
* Return     
* Remarks    
**********************************************************************/
static  void    MBDialupCreate(HWND hWnd)
{
    int iNumberLen;
    char chCurrent;
#if defined(MBDIALUPSTYLE_TOUCH)
    int i;

    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_SET,1),(LPARAM)LEFTBUTTONTEXT);//����
    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT,BUTTONBACK);//����
    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_MIDDLE,2),(LPARAM)BUTTON_MIDDLE_TEXT);//��ϵ��

    for (i = 0;i < iMBButtonNum;i++)
        dialitembtn[i].iState = BTN_UP;

    ClearKeyDown    = BTN_UP;//��ǰû���˸������
    bPenDown        = FALSE; //��ǰû�д����ʰ���

#elif defined(MBDIALUPSTYLE_KEYBORD)
    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_SET,1),
        (strlen(cPhoneNumber) == 0) ? (LPARAM)"" : (LPARAM)LEFTBUTTONTEXT);
    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT,BUTTONDELETE);//ɾ��

    bKeyDown        = FALSE;//��ǰû�а�������
    bClearKeyDown   = FALSE;//��ǰû�����������
#endif

    if (FindSIMState(FALSE) == 0)//û��SIM����ʱ����д���
    {
        PostMessage(hWnd,IDC_INIT112,0,0);
    }

    iNumberLen = strlen(cPhoneNumber);
    if ((bSuperAddDial) &&              //����׷�Ӻ���Ľ����,��Ҫ���̷��͵�һ���ַ�
        (!GetListCurCalling()) &&       //û���е�ǰͨ��
        (!IsAllType_Specific(CALL_STAT_HELD)))//��ǰ����ȫ������
    {        
        if (iNumberLen > 0)
        {
            chCurrent = cPhoneNumber[iNumberLen - 1];//������һ���ַ�
            ME_SendDTMF(hWnd,IDC_ME_SUPERADDDIAL,chCurrent);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALUP)
            printf("mb MBDialup ����׷�Ӻ���:%c\r\n",chCurrent);
#endif
        }
        else
        {
            PostMessage(hWnd,IDC_ME_SUPERADDDIAL,ME_RS_SUCCESS,0);
        }
    }
#if defined(KEYSWITCHTYPE_SHORT)
    if ((iNumberLen == 1) && (cPhoneNumber[0] == cCh_Asterisk))//ֻ��һ���ַ�,�����Ǻ�
    {
        bArriveKeySwitch = FALSE;
        KillTimer(hWnd,IDC_TIMER_SHORTKEY);
        SetTimer(hWnd,IDC_TIMER_SHORTKEY,TIMER_SHORTKEY,NULL);//�����̰�����ʱ��
    }
    else
    {
        bArriveKeySwitch = TRUE;
    }
#endif

    if (!GetInitState())//�绰���ֳ�ʼ����δ���
        MessageMBPostMsgCallForward();//���¿�ʼ��ѯ�йغ���ת�Ƶ���Ϣ
}
/********************************************************************
* Function   bInitMBDialup  
* Purpose    ��ʼ��������������� 
* Params     
* Return     
* Remarks    
**********************************************************************/
void    bInitMBDialup(void)
{
    static BOOL bInit = FALSE;
    BOOL        bTran;
    COLORREF    Color;
    int         i;
#if defined(MBDIALUPSTYLE_TOUCH)
    SIZE        NumSize;//����ͼƬ�ĳߴ�
    SIZE        BtnSizeBar;//������ͼƬ�ĳߴ�
    SIZE        BtnSizeBS;//�˸����ͼƬ�ĳߴ�
    HDC         hdcPicNum,hdcPicBtnUp,hdcPicBtnDown,hdcBar,hdcBSUp,hdcBSDown;

    if (bInit)
        return;

    GetImageDimensionFromFile(pPicNumBar,&BtnSizeBar);//������ͼƬ�ĳߴ�
    GetImageDimensionFromFile(pPicBSUp  ,&BtnSizeBS );//�˸����ͼƬ�ĳߴ�(����)

    SetRect(&rPicNumBar,0,0,BtnSizeBar.cx,BtnSizeBar.cy);//����������
    SetRect(&rPicBS,BtnSizeBar.cx,0,BtnSizeBS.cx + BtnSizeBar.cx,BtnSizeBS.cy);//�˸��������(����)

    hdcBar            = CreateMemoryDC(BtnSizeBar.cx,BtnSizeBar.cy);
    hdcBSUp           = CreateMemoryDC(BtnSizeBS.cx ,BtnSizeBS.cy );
    hdcBSDown         = CreateMemoryDC(BtnSizeBS.cx ,BtnSizeBS.cy );

    hPicNumBar        = CreateBitmapFromImageFile(hdcBar   ,pPicNumBar,&Color,&bTran);//�����ľ��
    hPicBackSpaceUp   = CreateBitmapFromImageFile(hdcBSUp  ,pPicBSUp  ,&Color,&bTran);//�˸���ľ��(����)
    hPicBackSpaceDown = CreateBitmapFromImageFile(hdcBSDown,pPicBSDown,&Color,&bTran);//�˸���ľ��(����)

    DeleteObject(hdcBar);
    DeleteObject(hdcBSUp);
    DeleteObject(hdcBSDown);
    //////////////////////////////////////////////////////////////////////////
    GetImageDimensionFromFile(pPicNumber,&NumSize); //�������ͼƬ�ߴ�
    iPicNumChWidth  = NumSize.cx / iMBButtonNum;    //������������Ŀ�
    iPicNumHeight   = NumSize.cy;                   //����ͼƬ�ĸ߶�

    hdcPicNum       = CreateMemoryDC(NumSize.cx,NumSize.cy);
    hPicNumber      = CreateBitmapFromImageFile(hdcPicNum,pPicNumber,&Color,&bTran);//��������ͼƬ
    DeleteObject(hdcPicNum);

    SetRect(&dialitembtn[0].rPicNum,iPicNumChWidth * 9,0,
        iPicNumChWidth * 10,iPicNumHeight);
    dialitembtn[0].ch_short = cCh_Num0;//'0'
    dialitembtn[0].iState   = BTN_UP;

    for (i = 1;i < iNumberCount;i++)//����1-9���ֵ�����
    {
        SetRect(&dialitembtn[i].rPicNum,(i - 1) * iPicNumChWidth,0,
            i * iPicNumChWidth,iPicNumHeight);

        dialitembtn[i].ch_short = cCh_Num0 + i;//'0'
        dialitembtn[i].iState   = BTN_UP;
    }
    SetRect(&dialitembtn[10].rPicNum,13 * iPicNumChWidth,0,
        14 * iPicNumChWidth,iPicNumHeight);

    dialitembtn[10].ch_short    = cCh_Asterisk;//'*';
    dialitembtn[10].iState      = BTN_UP;
    //////////////////////////////////////////////////////////////////////////
    SetRect(&dialitembtn[11].rPicNum,12 * iPicNumChWidth,0,
        13 * iPicNumChWidth,iPicNumHeight);

    dialitembtn[11].ch_short    = cCh_Well;//'#'
    dialitembtn[11].iState      = BTN_UP;
    //////////////////////////////////////////////////////////////////////////
    SetRect(&dialitembtn[12].rPicNum,11 * iPicNumChWidth,0,
        12 * iPicNumChWidth,iPicNumHeight);

    dialitembtn[12].ch_short    = cCh_Plus;//'+'
    dialitembtn[12].iState      = BTN_UP;
    //////////////////////////////////////////////////////////////////////////
    SetRect(&dialitembtn[13].rPicNum,10 * iPicNumChWidth,0,
        11 * iPicNumChWidth,iPicNumHeight);

    dialitembtn[13].ch_short    = cCh_ExtP;//'p'
    dialitembtn[13].iState      = BTN_UP;
    //////////////////////////////////////////////////////////////////////////
    SetRect(&dialitembtn[14].rPicNum,14 * iPicNumChWidth,0,
        15 * iPicNumChWidth,iPicNumHeight);

    dialitembtn[14].ch_short    = cCh_ExtW;//'w'
    dialitembtn[14].iState      = BTN_UP;
    //////////////////////////////////////////////////////////////////////////
    dialitembtn[0].iVKey        = VK_0;
    dialitembtn[1].iVKey        = VK_1;
    dialitembtn[2].iVKey        = VK_2;
    dialitembtn[3].iVKey        = VK_3;
    dialitembtn[4].iVKey        = VK_4;
    dialitembtn[5].iVKey        = VK_5;
    dialitembtn[6].iVKey        = VK_6;
    dialitembtn[7].iVKey        = VK_7;
    dialitembtn[8].iVKey        = VK_8;
    dialitembtn[9].iVKey        = VK_9;
    dialitembtn[10].iVKey       = KEYASTERICK;//*
    dialitembtn[11].iVKey       = KEYWELL;//#
    dialitembtn[12].iVKey       = VK_0;//+
    dialitembtn[13].iVKey       = KEYASTERICK;//p
    dialitembtn[14].iVKey       = KEYWELL;//w

    for (i = 0;i < iBtn_ShowNums;i++)//���ذ�ťͼƬ����Ϣ
    {
        GetImageDimensionFromFile(pPicButtonUp[i],&dialitembtn[i].sBtnNum);

        hdcPicBtnUp   = CreateMemoryDC(dialitembtn[i].sBtnNum.cx,dialitembtn[i].sBtnNum.cy);
        hdcPicBtnDown = CreateMemoryDC(dialitembtn[i].sBtnNum.cx,dialitembtn[i].sBtnNum.cy);

        dialitembtn[i].hBtnUp   = CreateBitmapFromImageFile(hdcPicBtnUp,
            pPicButtonUp[i],&Color,&bTran);
        dialitembtn[i].hBtnDown = CreateBitmapFromImageFile(hdcPicBtnDown,
            pPicButtonDown[i],&Color,&bTran);

        DeleteObject(hdcPicBtnUp);
        DeleteObject(hdcPicBtnDown);
    }
    SetRect(&dialitembtn[1].rPicBtn,
        0,rPicNumBar.bottom,dialitembtn[1].sBtnNum.cx,
        rPicNumBar.bottom + dialitembtn[1].sBtnNum.cy);

    SetRect(&dialitembtn[2].rPicBtn,
        dialitembtn[1].rPicBtn.right,rPicNumBar.bottom,
        dialitembtn[1].rPicBtn.right + dialitembtn[2].sBtnNum.cx,
        rPicNumBar.bottom + dialitembtn[2].sBtnNum.cy);

    SetRect(&dialitembtn[3].rPicBtn,
        dialitembtn[2].rPicBtn.right,rPicNumBar.bottom,
        dialitembtn[2].rPicBtn.right + dialitembtn[3].sBtnNum.cx,
        rPicNumBar.bottom + dialitembtn[3].sBtnNum.cy);

    SetRect(&dialitembtn[4].rPicBtn,
        0,dialitembtn[1].rPicBtn.bottom,dialitembtn[4].sBtnNum.cx,
        dialitembtn[1].rPicBtn.bottom + dialitembtn[4].sBtnNum.cy);

    SetRect(&dialitembtn[5].rPicBtn,
        dialitembtn[4].rPicBtn.right,dialitembtn[2].rPicBtn.bottom,
        dialitembtn[4].rPicBtn.right + dialitembtn[5].sBtnNum.cx,
        dialitembtn[2].rPicBtn.bottom + dialitembtn[5].sBtnNum.cy);

    SetRect(&dialitembtn[6].rPicBtn,
        dialitembtn[5].rPicBtn.right,dialitembtn[3].rPicBtn.bottom,
        dialitembtn[5].rPicBtn.right + dialitembtn[6].sBtnNum.cx,
        dialitembtn[3].rPicBtn.bottom + dialitembtn[6].sBtnNum.cy);

    SetRect(&dialitembtn[7].rPicBtn,
        0,dialitembtn[4].rPicBtn.bottom,dialitembtn[7].sBtnNum.cx,
        dialitembtn[4].rPicBtn.bottom + dialitembtn[7].sBtnNum.cy);

    SetRect(&dialitembtn[8].rPicBtn,
        dialitembtn[7].rPicBtn.right,dialitembtn[5].rPicBtn.bottom,
        dialitembtn[7].rPicBtn.right + dialitembtn[8].sBtnNum.cx,
        dialitembtn[5].rPicBtn.bottom + dialitembtn[8].sBtnNum.cy);

    SetRect(&dialitembtn[9].rPicBtn,
        dialitembtn[8].rPicBtn.right,dialitembtn[6].rPicBtn.bottom,
        dialitembtn[8].rPicBtn.right + dialitembtn[9].sBtnNum.cx,
        dialitembtn[6].rPicBtn.bottom + dialitembtn[9].sBtnNum.cy);

    SetRect(&dialitembtn[10].rPicBtn,//*
        0,dialitembtn[7].rPicBtn.bottom,dialitembtn[10].sBtnNum.cx,
        dialitembtn[7].rPicBtn.bottom + dialitembtn[10].sBtnNum.cy);

    SetRect(&dialitembtn[0].rPicBtn,
        dialitembtn[10].rPicBtn.right,dialitembtn[8].rPicBtn.bottom,
        dialitembtn[10].rPicBtn.right + dialitembtn[0].sBtnNum.cx,
        dialitembtn[8].rPicBtn.bottom + dialitembtn[0].sBtnNum.cy);

    SetRect(&dialitembtn[11].rPicBtn,//#
        dialitembtn[0].rPicBtn.right,dialitembtn[9].rPicBtn.bottom,
        dialitembtn[0].rPicBtn.right + dialitembtn[11].sBtnNum.cx,
        dialitembtn[9].rPicBtn.bottom + dialitembtn[11].sBtnNum.cy);

#elif defined(MBDIALUPSTYLE_KEYBORD)
    SIZE sizeLarge,sizeSmall;
    int  iPicChWidthLarge;//������������Ŀ�(����ַ�)
    int  iPicChWidthSmall;//������������Ŀ�(С���ַ�)
    int  iNumberLinesLarge;//������ʾ�绰������������(���)
    int  iScreenWidth;
    HDC  hdc;

    if (bInit)
        return;

    if (!GetImageDimensionFromFile(pPICNUMBERLARGE,&sizeLarge))//����ļ��ߴ�(���)
    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALUP)
        printf("\r\n �޷��ҵ��ļ�:%s",pPICNUMBERLARGE);
#endif
        return;
    }
    if (!GetImageDimensionFromFile(pPICNUMBERSMALL,&sizeSmall))//����ļ��ߴ�(С��)
    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALUP)
        printf("\r\n �޷��ҵ��ļ�:%s",pPICNUMBERSMALL);
#endif
        return;
    }
    iPicNumberHeightLarge   = sizeLarge.cy;//����λͼ�ĸ߶�(���)
    iPicChWidthLarge        = sizeLarge.cx / iMBButtonNum;//������������Ŀ�(С��)

    iPicNumberHeightSmall   = sizeSmall.cy;//����λͼ�ĸ߶�(С��)
    iPicChWidthSmall        = sizeSmall.cx / iMBButtonNum;//������������Ŀ�(С��)

    iNumberLinesLarge       = GetScreenUsableWH1(SM_CYSCREEN) / iPicNumberHeightLarge;//������ʾ�绰������������(���)
    //////////////////////////////////////////////////////////////////////////
    SetRect(&dialitembtn[0].rBtnPicLarge,iPicChWidthLarge * 9,0,
        iPicChWidthLarge * 10,iPicNumberHeightLarge);
    SetRect(&dialitembtn[0].rBtnPicSmall,iPicChWidthSmall * 9,0,
        iPicChWidthSmall * 10,iPicNumberHeightSmall);

    dialitembtn[0].ch_short = cCh_Num0;//'0'
    dialitembtn[0].iState   = BTN_UP;
    for (i = 1;i < iNumberCount;i++)//����1-9���ֵ�����
    {
        SetRect(&dialitembtn[i].rBtnPicLarge,(i - 1) * iPicChWidthLarge,0,
            i * iPicChWidthLarge,iPicNumberHeightLarge);
        SetRect(&dialitembtn[i].rBtnPicSmall,(i - 1) * iPicChWidthSmall,0,
            i * iPicChWidthSmall,iPicNumberHeightSmall);

        dialitembtn[i].ch_short = cCh_Num0 + i;//'0'
        dialitembtn[i].iState   = BTN_UP;
    }
    SetRect(&dialitembtn[10].rBtnPicLarge,13 * iPicChWidthLarge,0,
        14 * iPicChWidthLarge,iPicNumberHeightLarge);
    SetRect(&dialitembtn[10].rBtnPicSmall,13 * iPicChWidthSmall,0,
        14 * iPicChWidthSmall,iPicNumberHeightSmall);

    dialitembtn[10].ch_short    = cCh_Asterisk;//'*';
    dialitembtn[10].iState      = BTN_UP;
    //////////////////////////////////////////////////////////////
    SetRect(&dialitembtn[11].rBtnPicLarge,12 * iPicChWidthLarge,0,
        13 * iPicChWidthLarge,iPicNumberHeightLarge);
    SetRect(&dialitembtn[11].rBtnPicSmall,12 * iPicChWidthSmall,0,
        13 * iPicChWidthSmall,iPicNumberHeightSmall);

    dialitembtn[11].ch_short    = cCh_Well;//'#'
    dialitembtn[11].iState      = BTN_UP;
    //////////////////////////////////////////////////////////////
    SetRect(&dialitembtn[12].rBtnPicLarge,11 * iPicChWidthLarge,0,
        12 * iPicChWidthLarge,iPicNumberHeightLarge);
    SetRect(&dialitembtn[12].rBtnPicSmall,11 * iPicChWidthSmall,0,
        12 * iPicChWidthSmall,iPicNumberHeightSmall);

    dialitembtn[12].ch_short    = cCh_Plus;//'+'
    dialitembtn[12].iState      = BTN_UP;
    //////////////////////////////////////////////////////////////
    SetRect(&dialitembtn[13].rBtnPicLarge,10 * iPicChWidthLarge,0,
        11 * iPicChWidthLarge,iPicNumberHeightLarge);
    SetRect(&dialitembtn[13].rBtnPicSmall,10 * iPicChWidthSmall,0,
        11 * iPicChWidthSmall,iPicNumberHeightSmall);

    dialitembtn[13].ch_short    = cCh_ExtP;//'p'
    dialitembtn[13].iState      = BTN_UP;
    //////////////////////////////////////////////////////////////
    SetRect(&dialitembtn[14].rBtnPicLarge,14 * iPicChWidthLarge,0,
        15 * iPicChWidthLarge,iPicNumberHeightLarge);
    SetRect(&dialitembtn[14].rBtnPicSmall,14 * iPicChWidthSmall,0,
        15 * iPicChWidthSmall,iPicNumberHeightSmall);

    dialitembtn[14].ch_short    = cCh_ExtW;//'w'
    dialitembtn[14].iState      = BTN_UP;
    //////////////////////////////////////////////////////////////////////////
    dialitembtn[0].iVKey        = VK_0;
    dialitembtn[1].iVKey        = VK_1;
    dialitembtn[2].iVKey        = VK_2;
    dialitembtn[3].iVKey        = VK_3;
    dialitembtn[4].iVKey        = VK_4;
    dialitembtn[5].iVKey        = VK_5;
    dialitembtn[6].iVKey        = VK_6;
    dialitembtn[7].iVKey        = VK_7;
    dialitembtn[8].iVKey        = VK_8;
    dialitembtn[9].iVKey        = VK_9;
    dialitembtn[10].iVKey       = KEYASTERICK;//*
    dialitembtn[11].iVKey       = KEYWELL;//#
    dialitembtn[12].iVKey       = VK_0;//+
    dialitembtn[13].iVKey       = KEYASTERICK;//p
    dialitembtn[14].iVKey       = KEYWELL;//w
    //////////////////////////////////////////////////////////////////////////
    iScreenWidth        = GetScreenUsableWH1(SM_CXSCREEN);
    iChWidthLarge       = (iScreenWidth - 2 * iNUMBERTOPX) / iLINENUMBERNUMLARGE;//һ���ַ��������(���)
    iChWidthSmall       = (iScreenWidth - 2 * iNUMBERTOPX) / iLINENUMBERNUMSMALL;//һ���ַ��������(С��)

    iNumberCountLarge   = iNumberLinesLarge * iLINENUMBERNUMLARGE;//��������ʾ�����������(�������)

    hdc                 = CreateMemoryDC(sizeLarge.cx,sizeLarge.cy);
    hBmpNumberLarge     = CreateBitmapFromImageFile(hdc,pPICNUMBERLARGE,&Color,&bTran);
    DeleteObject(hdc);

    hdc                 = CreateMemoryDC(sizeSmall.cx,sizeSmall.cy);
    hBmpNumberSmall     = CreateBitmapFromImageFile(hdc,pPICNUMBERSMALL,&Color,&bTran);
    DeleteObject(hdc);

    if ((hBmpNumberLarge == NULL) && (hBmpNumberSmall == NULL))
    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALUP)
        printf("\r\n �޷����ͼƬ���.\r\n");
#endif
        return;
    }
#endif
    bInit = TRUE;
}
