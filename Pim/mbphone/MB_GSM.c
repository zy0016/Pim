/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : ��ӵ绰����
 *
 * Purpose  : ����GSM�������ļ�
 *            
\**************************************************************************/
#include    "MB_GSM.h"

#define DEBUG_MB_GSM

//�������Ʋ����������
#if defined(SIEMENS_TC35)
#define GSM_CALLBAR_NONESERVICE     133 //δ��ͨ����
#define GSM_CALLBAR_INVALIDCODE     16  //�������������
#define GSM_CALLBAR_INVALIDSTRING   260 //�º����������ȷ���벻һ��
#endif
#if defined(WAVECOM_2C)
#define GSM_CALLBAR_NONESERVICE     30  //δ��ͨ����
#define GSM_CALLBAR_INVALIDCODE     16  //�������������
#define GSM_CALLBAR_INVALIDSTRING   3   //�º����������ȷ���벻һ��
#endif
#if defined(TI_MODULE)
#define GSM_CALLBAR_NONESERVICE     30  //δ��ͨ����
#define GSM_CALLBAR_INVALIDCODE     16  //�������������
#define GSM_CALLBAR_INVALIDSTRING   3   //�º����������ȷ���벻һ��
#endif
static HINSTANCE    hInstance;
static GSMTYPE      GSMType;//GSM��������
static int          iGSMBSType = -1;//BS��������

static BOOL         bGSMAutoClose;//�Ƿ��Զ��رմ���
static HWND         hWndGSM = NULL;
static BOOL         bCancelOperate = FALSE;//��ǰ�����Ƿ��Ѿ���ȡ��

static char       * pSucc;//�ɹ���ʾ��(iSuccMaxLen 256���ַ�)
static char       * pFail;//ʧ����ʾ��(iFailMaxLen 256���ַ�)
static char       * pParam1;//���ڱ���һЩ����(iParamMaxLen 100���ַ�)
static char       * pParam2;//���ڱ���һЩ����(iParamMaxLen 100���ַ�)
static char       * pParam3;//���ڱ���һЩ����(iParamMaxLen 100���ַ�)
static char       * pParam4;//���ڱ���һЩ����(iParamMaxLen 100���ַ�)

static int          iCallForward;//�Ժ���ת�ƵĲ���
static RECT         hEditRect;

static const char * pImei           = "IMEI:%s";
static const char * pClassName      = "MBGSMOperateWndClass";//��������
static const char * pInputControl   = "EDIT";
static const char * pAsterisk       = "*";
static const char * pWell           = "#";
static const char * pNewLine        = "\n";
static const char * pColon          = ":";
static const char   Ch_Asterisk     = '*';
static const char   Ch_well         = '#';
static const char   Ch_End          = '\0';
static const char   Ch_Num1         = '1';

static const int    iGSM            = 1;//�������GSM��׼�Ļ��ķ���ֵ
static const int    iUnGSM          = 0;//���벻����GSM��׼�Ļ��ķ���ֵ
static const int    iGSM_Error      = -1;//AT�����ʧ�ܷ���-1
static const int    iGSM_Param      = -2;//GSM����������
static const int    iUnBSParam      = -1;//��ʾû��BS����
static const int    iClewMaxLen     = 2048;//��ʾ�����󳤶�
static const int    iSuccMaxLen     = 256;//�ɹ���ʾ�����󳤶�
static const int    iFailMaxLen     = 256;//ʧ����ʾ�����󳤶�
static const int    iParamMaxLen    = 100;//pParam����󳤶�

static const enum
{
    GSM_CF_ACTIVATE,                //�������ת��
    GSM_CF_ACTIVATELOGIN,           //���ע�����ת��
    GSM_CF_DEACTIVATE,              //��ֹ����ת��
    GSM_CF_DEACTIVATELOGIN          //��ֹ����ע�����ת��
};
static const enum
{
    GSM_CB_CHECK,                   //��ѯ��������
    GSM_CB_CHECKBS                  //��ѯ��������(֧��BS����)
};
static const char * pGSMCode_T_SC[] = //����ת�ư�������T��ʱ��,��T��Ӧ��SC����
{
    "002",      //all call forwarding
    "004",      //all conditional call forwarding
    "61",       //call forwarding on no answer
    ""
};
//////////////////////////////////////////////////////////////////////////
static const char * pGSMCode_BS[] = //The Network service codes (BS)
{
    "10",       //All teleservices
    "19",       //All teleservices except SMS
    "20",       //All bearer services
    "21",       //All asynchronous services
    "22",       //All synchronous services
    "11",       //Telephony                     BSVOICE
    "12",       //All data teleservices         BSDATA
    "13",       //Fax services                  BSFAX
    "16",       //Short Message Services        BSSMS
    "24",       //All data circuit synchronous  BSDCS
    "25",       //All data circuit asynchronous BSDCA
    "26",       //All dedicated packet access   BSDPAC
    "27",       //All dedicated PAD access      BSDPAD
    ""
};
static const char * pBSContent[] =//BS�����Ľ���(�������úͲ�ѯʧ�ܵ�ʱ����ʾ)
{
    "(Voice + SMS + FAX)",                      //10
    "(Voice + FAX)",                            //19
    "(Data circuit asynchron + syncron. + PAD)",//20
    "(Data circuit asynchron + PAD)",           //21
    "(Data circuit synchron+packet)",           //22
    "(Voice)",                                  //11 BSVOICE
    "(SMS + FAX)",                              //12 BSDATA
    "(FAX)",                                    //13 BSFAX
    "(SMS)",                                    //16 BSSMS
    "(Data circuit synchron)",                  //24 BSDCS
    "(Data circuit asynchron)",                 //25 BSDCA
    "(Packet)",                                 //26 BSDPAC
    "(PAD)",                                    //27 BSDPAD
    ""
};
static const char * pBSContentSearch[] =//BS�����Ľ���(���ڲ�ѯ�ɹ���ʱ����ʾ)
{
    "(Telephony)",                              //BSVOICE
    "(All data teleservices)",                  //BSDATA
    "(Fax services)",                           //BSFAX 
    "(Short Message Services)",                 //BSSMS 
    "(All data circuit synchronous)",           //BSDCS 
    "(All data circuit asynchronous)",          //BSDCA 
    "(All dedicated packet access)",            //BSDPAC
    "(All dedicated PAD access)",               //BSDPAD
    ""
};
static const int iBSME[] =
{
    BSVOICE,    // Voice
    BSDATA,     // Data
    BSFAX,      // FAX
    BSSMS,      // Short Message
    BSDCS,      // Data circuit sync
    BSDCA,      // Data circuit async
    BSDPAC,     // Dedicated packet access
    BSDPAD      // Dedicated PAD access
};
//////////////////////////////////////////////////////////////////////////
static const char * pIMEI[] =
{
    "*#06#",    //��IMEI��
    ""
};
static const char * pSecurity[] =
{
    "**04*",    //�޸�pin��
    "**042*",   //�޸�pin2��
    "**05*",    //pin1�����
    "**052*",   //pin2�����
    ""
};
static const char * pCallBarring[] =
{
    "**03*330*",//�޸ĺ�����������
    "**03**",   //�޸ĺ�����������
    "*03*330*", //�޸ĺ�����������
    "*03**",    //�޸ĺ�����������
    ""
};
static const char * pUnlockPS[] =
{
    "*#0003*",   //�ֻ�ģ���������
    ""
};
//////////////////////////////////////////////////////////////////////////
static const char * pForwardingCheck[] = //����ת�ƵĲ�ѯ����(֧��BS����)
{
    "*#002",   //all call forwarding/����ת��
    "*#004",   //all conditional call forwarding/��������ת��
    "*#21",    //call forwarding unconditional/������ת��
    "*#61",    //call forwarding on no answer/��Ӧ��ת��
    "*#62",    //call forwarding on not reachable/���ڷ�������ػ�ת��
    "*#67",    //call busy/��æת��
    ""
};
static const char * pForwardingActive[] = //����ת�Ƶļ������(֧��BS����)
{
    "*002",    //all call forwarding/����ת��
    "*004",    //all conditional call forwarding/��������ת��
    "*21",     //call forwarding unconditional/������ת��
    "*61",     //call forwarding on no answer/��Ӧ��ת��
    "*62",     //call forwarding on not reachable/���ڷ�������ػ�ת��
    "*67",     //call busy/��æת��
    ""
};
static const char * pForwardingActiveLogin[] =//����ת�Ƶ�ע�ἤ�����(֧��BS������T����)
{
    "**002",   //all call forwarding/����ת��
    "**004",   //all conditional call forwarding/��������ת��
    "**21",    //call forwarding unconditional/������ת��
    "**61",    //call forwarding on no answer/��Ӧ��ת��
    "**62",    //call forwarding on not reachable/���ڷ�������ػ�ת��
    "**67",    //call busy/��æת��
    ""
};
static const char * pForwardingDeactive[] = //����ת�ƵĹرղ���(֧��BS����)
{
    "#002",    //all call forwarding/����ת��
    "#004",    //all conditional call forwarding/��������ת��
    "#21",     //call forwarding unconditional/������ת��
    "#61",     //call forwarding on no answer/��Ӧ��ת��
    "#62",     //call forwarding on not reachable/���ڷ�������ػ�ת��
    "#67",     //call busy/��æת��
    ""
};
static const char * pForwardingDeactiveUnlogin[] = //����ת�ƵĹرշ�ע�����(֧��BS����)
{
    "##002",   //all call forwarding/����ת��
    "##004",   //all conditional call forwarding/��������ת��
    "##21",    //call forwarding unconditional/������ת��
    "##61",    //call forwarding on no answer/��Ӧ��ת��
    "##62",    //call forwarding on not reachable/���ڷ�������ػ�ת��
    "##67",    //call busy/��æת��
    ""
};
//////////////////////////////////////////////////////////////////////////
static const char * pCallBarringActive[] = //�������Ƶļ������(֧��BS����)
{
    "*33*",     //call barring of outgoing call/�������в����绰
    "*331*",    //call barring of outgoing international call/�������в������ʳ�;
    "*332*",    //call barring of outgoing international calls except to HPLMN/�������в������ʳ�;,���˱���
    "*35*",     //call barring of incoming calls/�������в���绰
    "*351*",    //call barring of incoming calls if roaming/���ƹ�������ʱ����
    ""
};
static const char * pCallBarringDeactive[] = //�������ƵĽ�ֹ����(֧��BS����)
{
    "#33*",     //call barring of outgoing call/�������в����绰
    "#331*",    //call barring of outgoing international call/�������в������ʳ�;
    "#332*",    //call barring of outgoing international calls except to HPLMN/�������в������ʳ�;,���˱���
    "#35*",     //call barring of incoming calls/�������в���绰
    "#351*",    //call barring of incoming calls if roaming/���ƹ�������ʱ����
    "#330*",    //all barring service (only for deactivation)/�������ͨ������
    "#333*",    //all outgoing barring service (only for deactivation)/������в�������
    "#353*",    //all incoming barring service (only for deactivation)/������в�������
    ""
};
static const char * pCallBarringCheck[] = //�������ƵĲ�ѯ����(��֧��BS����)
{
    "*#33#",    //call barring of outgoing call/�������в����绰
    "*#331#",   //call barring of outgoing international call/�������в������ʳ�;
    "*#332#",   //call barring of outgoing international calls except to HPLMN/�������в������ʳ�;,���˱���
    "*#35#",    //call barring of incoming calls/�������в���绰
    "*#351#",   //call barring of incoming calls if roaming/���ƹ�������ʱ����
    ""
};
static const char * pCallBarringCheckBS[] = //�������ƵĲ�ѯ����(֧��BS����)
{
    "*#33**",    //call barring of outgoing call/�������в����绰
    "*#331**",   //call barring of outgoing international call/�������в������ʳ�;
    "*#332**",   //call barring of outgoing international calls except to HPLMN/�������в������ʳ�;,���˱���
    "*#35**",    //call barring of incoming calls/�������в���绰
    "*#351**",   //call barring of incoming calls if roaming/���ƹ�������ʱ����
    ""
};
//////////////////////////////////////////////////////////////////////////
static const char * pCallWaiting[] = //���еȴ��Ĳ���(֧��BS����)
{
    "*43",     //������еȴ�
    "*#43",    //��ѯ���еȴ�
    "#43",     //��ֹ���еȴ�
    ""
};
//////////////////////////////////////////////////////////////////////////
static const char * pPNPCheck[] =//Phone number presentation
{
    "*#30#",//CLIP check status
    "*#31#",//CLIR check status
    "*#76#",//COLP check status(Connected Line identification Presentation)
    "*#77#",//Check status COLR(connected Line identification Restriction)
    ""
};
static const char * pGSMSpecific[] =//GSM�������
{
    "*31#",//Suppress CLIR for a voice call
    "#31#",//Activate CLIR for a voice call
    ""
};
static const char * pFileLog_Delete[] = 
{
    "#####",
    ""
};
static const char * pFileLog_Read[] = 
{
    "00000#",
    ""
};
//////////////////////////////////////////////////////////////////////////
static const  GSMOPERATE  GsmOperate[] = 
{   //ǰ׺���ַ                ������ַ              ���Ӳ���               ִ�з�ʽ,�Ƿ���г���ƥ��
    {pIMEI                     ,GsmIMEI              ,NULL                  ,TRUE  ,TRUE},
    {pSecurity                 ,GsmSecurity          ,NULL                  ,TRUE  ,FALSE},
    {pUnlockPS                 ,GsmUnlockPS          ,NULL                  ,TRUE  ,FALSE},

    {pForwardingCheck          ,GsmForwardingCheck   ,NULL                  ,FALSE ,FALSE},
    {pForwardingActive         ,GsmForwardingOperate ,GSM_CF_ACTIVATE       ,FALSE ,FALSE},
    {pForwardingActiveLogin    ,GsmForwardingOperate ,GSM_CF_ACTIVATELOGIN  ,FALSE ,FALSE},
    {pForwardingDeactive       ,GsmForwardingOperate ,GSM_CF_DEACTIVATE     ,FALSE ,FALSE},
    {pForwardingDeactiveUnlogin,GsmForwardingOperate ,GSM_CF_DEACTIVATELOGIN,FALSE ,FALSE},

    {pCallBarring              ,GsmModifyCallBarring ,NULL                  ,FALSE ,FALSE},
    {pCallBarringActive        ,GsmCallBarringOperate,TRUE                  ,FALSE ,FALSE},
    {pCallBarringDeactive      ,GsmCallBarringOperate,FALSE                 ,FALSE ,FALSE},
    {pCallBarringCheck         ,GsmCallBarringCheck  ,GSM_CB_CHECK          ,FALSE ,FALSE},
    {pCallBarringCheckBS       ,GsmCallBarringCheck  ,GSM_CB_CHECKBS        ,FALSE ,FALSE},

    {pCallWaiting              ,GsmCallWaitingOperate,NULL                  ,FALSE ,FALSE},

    {pPNPCheck                 ,GsmPNPCheck          ,NULL                  ,FALSE ,TRUE},
    {pGSMSpecific              ,GsmCLIRDeal          ,NULL                  ,FALSE ,TRUE},

    {pFileLog_Delete           ,ReadFileLog_Delete   ,NULL                  ,FALSE ,TRUE},
    {pFileLog_Read             ,ReadFileLog_Read     ,NULL                  ,FALSE ,TRUE},
    {NULL                      ,NULL                 ,NULL                  ,NULL  ,NULL}
};
///ussd����///////////////////////////////////////////////////////////////////////
static void * pUssd = NULL;
#define USSDREPLYMAXLEN     40//ussd�ظ���ʱ������������󳤶�
///ѡ�����ּ�+#�Ļ�,��ѯ��ϵ�˲���///////////////////////////////////////////////////////////////////////
static int          iPhoneBookSIMMaxLen = 100;//sim����ϵ�˸��������ֵ
static PHONEBOOK    phonebook;
#define GSM_PHONEBOOKMAX    4//�������ּ�+#������ϵ�˼�¼���ܵ�ʱ��,GSM�淶�й涨��������󳤶�
/*�Ϸ���ָ�����鱣������ĳһ��GSM������Ҫʶ�������
����˼·:
�ṹ����GsmOperate��pGSMPre���ĳ��GSM�������еĵ�ַ,�����Ϸ�ָ������ĵ�ַ
GsmOperate��pFunction���ÿ��GSM�������ж�Ӧ�Ĵ�����
wParam���ִ�������������ʱ������Ҫ�ĸ��Ӳ���
GSMʶ���ʱ������ȡ���ṹ�����е�һ��,�����һ���ָ������ĵ�ַ,
�õ�ǰ�绰������ָ�������н��в�ѯ,���绰�����ǰ׺����ָ��������ĳһ���ʱ��
�����ָ�������Ӧ�Ĵ�������,���н�һ����ɨ��,���򿼲�ָ�������е���һ��.
�����ָ�������е�ÿһ������ڵ绰�����ǰ׺ʱ,ȡ���ṹ�����е���һ�����ɨ��

���绰�������GSM������ʱ��,GSM���������ᷢ��AT����,
������ͳɹ�����iGSM,at�����ʧ�ܷ���iGSM_Error,
���绰����ǰ׺����GSM�������Ǻ�ߵĲ��ֲ�����GSM������ʱ��Ҳ����˵��Ȼ�绰�����ǰ׺
����ĳ��GSM��������,�����������벢�����������Ҫִ�е�GSM����,����iUnGSM

����GSMDealWith��������FALSE��ʾ���������ܲ���һ���Ϸ���GSM����,�����û���Ҫ����
����һ����뷢�ͳ�ȥ

��GSM��������iGSM�Ļ���ʾ��ĳ��GSM����ִ�����.AT�����Ѿ�����,��Ҫ��ʾһ����ʾ���ʾ
���ڽ���GSM����,����GSMDealWith��������TRUE��ʾ����������ĳ��GSM��������*/
/********************************************************************
* Function   CallMBGSMWindow  
* Purpose    ����һ����������GSM����
* Params    
* Return    
* Remarks   
**********************************************************************/
BOOL    CreateMBGSMWindow(void)
{
    WNDCLASS wc;

    if (hWndGSM != NULL)
        return TRUE;

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

    hWndGSM = CreateWindow(pClassName,TITLECAPTION, 
        WS_CAPTION|WS_BORDER|PWS_STATICBAR,PLX_WIN_POSITION , 
        NULL, NULL, NULL, NULL);

    if (NULL == hWndGSM)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }

    return (TRUE);
}
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    LRESULT lResult;
    HWND    hGSMEdit;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK);
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT);

        hGSMEdit = CreateWindow(pInputControl, NULL, 
            WS_VISIBLE | WS_CHILD |WS_BORDER|ES_MULTILINE|ES_UNDERLINE | WS_VSCROLL|ES_READONLY,
            0, 0, GetScreenUsableWH1(SM_CXSCREEN), GetScreenUsableWH1(SM_CYSCREEN),
            hWnd,(HMENU)IDC_GSMEDIT,NULL,NULL);

        if (hGSMEdit == NULL)
            return FALSE;

        iGSMBSType = iUnBSParam;
        GetClientRect(hGSMEdit,&hEditRect);
        hEditRect.right -= 7;

        pSucc   = (char *)malloc(iSuccMaxLen );
        pFail   = (char *)malloc(iFailMaxLen );
        pParam1 = (char *)malloc(iParamMaxLen);
        pParam2 = (char *)malloc(iParamMaxLen);
        pParam3 = (char *)malloc(iParamMaxLen);
        pParam4 = (char *)malloc(iParamMaxLen);

        if ((pSucc == NULL) || (pFail == NULL) || (pParam1 == NULL) || 
            (pParam2 == NULL) || (pParam3 == NULL) || (pParam4 == NULL))
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
            printf("mb MB_GSM mallocʧ��\r\n");
#endif
            return FALSE;
        }
        memset(pParam1,0x00,iParamMaxLen);
        memset(pParam2,0x00,iParamMaxLen);
        memset(pParam3,0x00,iParamMaxLen);
        memset(pParam4,0x00,iParamMaxLen);
        memset(pSucc  ,0x00,iSuccMaxLen );
        memset(pFail  ,0x00,iFailMaxLen );
        //***********ussd��ʼ��******************
        ME_USSD_SetNotify(hWnd,IDC_USSD_NOTIFY);
        //***********��ϵ����Ϣ��ʼ��***************************
        ME_GetPhonebookMemStatus(hWnd,IDC_ME_PHONE_SIM_AMX,PBOOK_SM);
        break;

    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd,IDC_GSMEDIT));//hGSMEdit
        break;

    case WM_ACTIVATE://ȥ�������Ϣ�������Ҽ������˳��������˳�ʧ��
        if (WA_INACTIVE != LOWORD(wParam))
            SetFocus(GetDlgItem(hWnd,IDC_GSMEDIT));//��������ý���

        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;

    case WM_TODESKTOP:
        SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
        return TRUE;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
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
        case IDC_BUTTON_SET://ȷ��
        case IDC_BUTTON_QUIT://�˳�
            iGSMBSType = iUnBSParam;
            ShowWindow(hWnd,SW_HIDE);
            break;
        //**********************************************************
        case IDC_USSD_LEFTBUTTON://����USSD���ܵ����
            {
                char cUssd[USSDREPLYMAXLEN + 1] = "";
                CallReplyControlWindow(hWnd,TITLECAPTION,USSDREPLYMAXLEN,cUssd,
                    BUTTONTXTLEFT_REPLY,BUTTONTXTRIGHT,0);
                if (strlen(cUssd) > 0)
                {
                    ME_USSD_Response(pUssd,cUssd);
                    MBWaitWindowState(hWnd,TRUE,IDC_CANCELOPERATE,TRUE);
                }
            }
            break;
        case IDC_USSD_RIGHTBUTTON://����USSD���ܵ��Ҽ�
            ME_USSD_Abort(pUssd);
            break;
        //**********************************************************
        case IDC_PHONE_LEFTBUTTON://���ڲ�ѯ��ϵ�˹��ܵ����,���ڲ�������
            APP_CallPhoneNumber(phonebook.PhoneNum);
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
            break;
        case IDC_PHONE_RIGHTBUTTON://���ڲ�ѯ��ϵ�˹��ܵ��Ҽ�
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
            break;
        }
        break;
    //*******************�ص���Ϣ******************************
    case IDC_GSMOPERATE://GSM�����Ļص���Ϣ
        if (bCancelOperate)//��ǰ�����Ѿ���ȡ��
            break;

        MBWaitWindowState(hWnd,FALSE,NULL,FALSE);
        switch (wParam)
        {
        case ME_RS_SUCCESS://GSM�����ɹ�
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
            printf("mb MB_GSM.c �յ�GSM�����ɹ�����Ϣ,GSMType:%d\r\n",GSMType);
#endif
            if (GSMClewText_Succ(hWnd))
                memset(pFail,0x00,iFailMaxLen);
            else
                SetWindowTextExt(GetDlgItem(hWnd,IDC_GSMEDIT),pFail);//hGSMEdit

            break;
        default://GSM����ʧ��
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
            printf("mb MB_GSM.c �յ�GSM����ʧ�ܵ���Ϣ,wParam:%ld,lParam:%ld,GSMType:%d\r\n",
                wParam,lParam,GSMType);
#endif
            GSMClewText_Fail(hWnd,wParam,lParam);
            break;
        }
        break;

    case IDC_USSD_NOTIFY://ussdע����Ϣ
        if (bCancelOperate)//��ǰ�����Ѿ���ȡ��
            break;

        if (!USSDDealRespose(hWnd,wParam,lParam))
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);

        break;

    case IDC_ME_PHONE_SIM_AMX://��ѯSIM������ϵ�˵�������
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            {
                PBMEM_INFO pbmen_info;
                memset(&pbmen_info,0x00,sizeof(PBMEM_INFO));
                ME_GetResult(&pbmen_info,sizeof(PBMEM_INFO));
                iPhoneBookSIMMaxLen = pbmen_info.Total;
            }
            break;
        }
        break;

    case IDC_CANCELOPERATE://��־ȡ�������Ļص���Ϣ
        bCancelOperate = TRUE;//��ǰ�����Ѿ���ȡ��
        SetWindowTextExt(GetDlgItem(hWnd,IDC_GSMEDIT),GSMOPERATECANCEL);//����ȡ��
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK);
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT);
        ShowWindow(hWnd,SW_SHOW);
        UpdateWindow(hWnd);
        break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
/********************************************************************
* Function   GSMClewText_Fail  
* Purpose    ����GSM��ʾ(��Ϣ����ʧ��)
* Params    
* Return    
* Remarks   
**********************************************************************/
static  BOOL    GSMClewText_Fail(HWND hWnd,WPARAM wParam,WPARAM lParam)
{
    memset(pSucc,0x00,iSuccMaxLen);
    switch (GSMType)
    {
    case GSM_IMEI:
        strcpy(pFail,IMEIFAIL);
        break;
    case GSM_CALLFORWARDING1:       //���к���ת��
    case GSM_CALLFORWARDING2:       //��������ת��
    case GSM_CALLFORWARDING3:       //������ת��
    case GSM_CALLFORWARDING4:       //��Ӧ��ת��
    case GSM_CALLFORWARDING5:       //���ڷ�������ػ�ת��
    case GSM_CALLFORWARDING6:       //��æת��
    case GSM_CALLFORWARDING1_CHECK: //��ѯ����ת��
    case GSM_CALLFORWARDING2_CHECK: //��ѯ��������ת��
    case GSM_CALLFORWARDING3_CHECK: //��ѯ������ת��
    case GSM_CALLFORWARDING4_CHECK: //��ѯ��Ӧ��ת��
    case GSM_CALLFORWARDING5_CHECK: //��ѯ���ڷ�������ػ�ת��
    case GSM_CALLFORWARDING6_CHECK: //��ѯ��æת��
    case GSM_CALLBARRING33:         //�������в����绰
    case GSM_CALLBARRING331:        //�������в������ʳ�;,
    case GSM_CALLBARRING332:        //�������в������ʳ�;(������)
    case GSM_CALLBARRING35:         //�������в���绰
    case GSM_CALLBARRING351:        //���ƹ�������ʱ����
    case GSM_CALLBARRING330:        //�������ͨ������
    case GSM_CALLBARRING333:        //������в�������
    case GSM_CALLBARRING353:        //������в�������
    case GSM_CALLBARRING33_CHECK:   //33 �������в����绰
    case GSM_CALLBARRING331_CHECK:  //331 �������в������ʳ�;
    case GSM_CALLBARRING332_CHECK:  //332 �������в������ʳ�;,���˱���
    case GSM_CALLBARRING35_CHECK:   //35 �������в���绰
    case GSM_CALLBARRING351_CHECK:  //351 ���ƹ�������ʱ����
        SetWindowTextExt(GetDlgItem(hWnd,IDC_GSMEDIT),pFail);//hGSMEdit
        return TRUE;
    case GSM_CALLBAR://����������
        switch (lParam)
        {
        case GSM_CALLBAR_NONESERVICE://133://δ��ͨ����
            StrAutoNewLine(NULL,GSMBARRING_ERROR1,pFail,&hEditRect,NULL);
            break;
        case GSM_CALLBAR_INVALIDSTRING://260://�º����������ȷ���벻һ��
            StrAutoNewLine(NULL,GSMBARRING_ERROR2,pFail,&hEditRect,NULL);
            break;
        case GSM_CALLBAR_INVALIDCODE://16://�������������
            StrAutoNewLine(NULL,GSMBARRING_ERROR3,pFail,&hEditRect,NULL);
            break;
        }
        break;
    }
    SetWindowTextExt(GetDlgItem(hWnd,IDC_GSMEDIT),pFail);//hGSMEdit
    return TRUE;
}
/********************************************************************
* Function   GSMClewText_Succ  
* Purpose    ����GSM��ʾ(��Ϣ���سɹ�)
* Params    
* Return    
* Remarks   
**********************************************************************/
static  BOOL    GSMClewText_Succ(HWND hWnd)
{
    HWND hEdit;

    hEdit = GetDlgItem(hWnd,IDC_GSMEDIT);
    if (GSM_IMEI == GSMType)//��IMEI��
    {
        char cIMEI[PHONENUMMAXLEN] = "";
        if (-1 == ME_GetResult(cIMEI,sizeof(cIMEI)))
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
            printf("mb MB_GSM IMEI fail\r\n");
#endif
            return FALSE;
        }
        sprintf(pSucc,pImei,cIMEI);

        SetWindowTextExt(hEdit,pSucc);//hGSMEdit
    }
    else if (GSM_CALLWAIT_CHECK == GSMType)//��ѯ���еȴ�
    {
        CWINFO * pCWInfo;
        int iBS,iCount,i;
        int iLength;
        char * pClew = NULL;

        iLength = ME_GetResult(NULL,NULL);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
        printf("mb MB_GSM.c,��ѯ���еȴ�,iLength:%lu\r\n",iLength);
#endif
        if (iLength <= 0)
            return FALSE;

        pCWInfo = (CWINFO *)malloc(iLength);
        if (NULL == pCWInfo)
            return FALSE;

        pClew = (char *)malloc(iClewMaxLen);
        if (NULL == pClew)
        {
            free(pCWInfo);
            return FALSE;
        }
        memset(pCWInfo,0x00,iLength);
        memset(pClew,0x00,iClewMaxLen);

        iLength = ME_GetResult(pCWInfo,iLength);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
        printf("mb MB_GSM.c,��ѯ���еȴ�,ME_GetResult=%lu\r\n",iLength);
#endif
        iCount = iLength / sizeof(CWINFO);
        for (i = 0;i < iCount;i++)
        {
            iBS = InIntArray(pCWInfo[i].BS,iBSME,sizeof(iBSME));
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
            printf("mb MB_GSM.c ��ѯ���еȴ� pCWInfo[%d].BS:%d,iBS:%d\r\n",i,pCWInfo[i].BS,iBS);
#endif
            StrAutoNewLine(NULL,GSMCALLWAITING,pParam1,&hEditRect,NULL);
            StrAutoNewLine(NULL,(iBS == iUnBSParam) ? "" : pBSContentSearch[iBS],
                pParam2,&hEditRect,NULL);
            StrAutoNewLine(NULL,pCWInfo[i].Active ? GSMOPERATEACTIVATE : GSMOPERATEDEACTIVATE,
                pParam3,&hEditRect,NULL);

            StringCombination(pSucc,pParam1,pParam2,pParam3,NULL);

            strcat(pClew,pSucc);
            strcat(pClew,pNewLine);
            strcat(pClew,pNewLine);
        }
        SetWindowTextExt(hEdit,pClew);//hGSMEdit
        free(pClew);
        free(pCWInfo);
    }
    else if ((GSM_CALLFORWARDING_CHECKBEGIN < GSMType) && 
        (GSMType < GSM_CALLFORWARDING_CHECKEND))//������ת�Ƶ�״̬
    {
        CFINFO * pCFInfo;
        int iLength;
        int iCount,i,iBS;
        char * pClew = NULL;

        iLength = ME_GetResult(NULL,NULL);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
        printf("mb MB_GSM.c,������ת��,iLength:%lu\r\n",iLength);
#endif
        if (iLength <= 0)
            return FALSE;

        pCFInfo = (CFINFO *)malloc(iLength);
        if (NULL == pCFInfo)
            return FALSE;

        pClew = (char *)malloc(iClewMaxLen);
        if (NULL == pClew)
        {
            free(pCFInfo);
            return FALSE;
        }
        memset(pCFInfo,0x00,iLength);
        memset(pClew,0x00,iClewMaxLen);

        iLength = ME_GetResult(pCFInfo,iLength);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
        printf("mb MB_GSM.c,��ѯ����ת��,ME_GetResult=%lu\r\n",iLength);
#endif
        iCount = iLength / sizeof(CFINFO);
        for (i = 0;i < iCount;i++)
        {
            iBS = InIntArray(pCFInfo[i].BS,iBSME,sizeof(iBSME));
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
            printf("mb MB_GSM.c ��ѯ����ת�� pCFInfo[%d].BS:%d,iBS:%d,Reason:%d,����:%s,״̬:%d\r\n",
                i,pCFInfo[i].BS,iBS,pCFInfo[i].Reason,pCFInfo[i].DN,pCFInfo[i].Active);
#endif
            switch (pCFInfo[i].Reason)
            {
            case CFU://������ת��
                StrAutoNewLine(NULL,GSMCALLFORWARDING3,pParam1,&hEditRect,NULL);
                break;
            case CFB:// ��æת��
                StrAutoNewLine(NULL,GSMCALLFORWARDING6,pParam1,&hEditRect,NULL);
                break;
            case CFNRy://��Ӧ��ת��
                StrAutoNewLine(NULL,GSMCALLFORWARDING4,pParam1,&hEditRect,NULL);
                break;
            case CFNRc://���ɴ�ת��
                StrAutoNewLine(NULL,GSMCALLFORWARDING5,pParam1,&hEditRect,NULL);
                break;
            }
            StrAutoNewLine(NULL,(iBS == iUnBSParam) ? "" : pBSContentSearch[iBS],
                pParam2,&hEditRect,NULL);
            StrAutoNewLine(NULL,pCFInfo[i].Active ? GSMOPERATEACTIVATE : GSMOPERATEDEACTIVATE,
                pParam3,&hEditRect,NULL);
            if (strlen(pCFInfo[i].DN) != 0)
            {
                strcpy(pParam4,GSMCALLFORWARDING_NO);
                strcat(pParam4,pCFInfo[i].DN);
            }
            else
            {
                strcpy(pParam4,"");
            }
            StringCombination(pSucc,pParam1,pParam2,pParam3,pParam4);
            strcat(pClew,pSucc);
            strcat(pClew,pNewLine);
            strcat(pClew,pNewLine);
        }
        SetWindowTextExt(hEdit,pClew);//hGSMEdit
        free(pClew);
        free(pCFInfo);
    }
    else if ((GSM_CALLBARRING_CHECKBEGIN < GSMType) && 
        (GSMType < GSM_CALLBARRING_CHECKEND))//��ѯ�������Ƶ�״̬
    {
        CBINFO * pCBInfo;
        int iLength;
        int iCount,i,iBS;
        char * pClew = NULL;

        iLength = ME_GetResult(NULL,NULL);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
        printf("mb MB_GSM.c,��ѯ��������,iLength:%lu\r\n",iLength);
#endif
        if (iLength <= 0)
            return FALSE;

        pCBInfo = (CBINFO *)malloc(iLength);
        if (NULL == pCBInfo)
            return FALSE;

        pClew = (char *)malloc(iClewMaxLen);
        if (NULL == pClew)
        {
            free(pCBInfo);
            return FALSE;
        }
        memset(pCBInfo,0x00,iLength);
        memset(pClew,0x00,iClewMaxLen);

        iLength = ME_GetResult(pCBInfo,iLength);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
        printf("mb MB_GSM.c,��ѯ��������,ME_GetResult=%lu\r\n",iLength);
#endif
        iCount = iLength / sizeof(CBINFO);
        for (i = 0;i < iCount;i++)
        {
            iBS = InIntArray(pCBInfo[i].BS,iBSME,sizeof(iBSME));
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
            printf("mb MB_GSM.c ��ѯ��������,BS:%d,iBS:%d,Fac:%d,״̬:%d\r\n",
                pCBInfo[i].BS,iBS,pCBInfo[i].Fac,pCBInfo[i].Active);
#endif
            switch (pCBInfo[i].Fac)
            {
            case BAOC://�������к���
                StrAutoNewLine(NULL,GSMCALLBARRING33,pParam1,&hEditRect,NULL);
                break;
            case BOIC://�������й��ʺ���
                StrAutoNewLine(NULL,GSMCALLBARRING331,pParam1,&hEditRect,NULL);
                break;
            case BOIC_exHC://�������й��ʺ���������
                StrAutoNewLine(NULL,GSMCALLBARRING332,pParam1,&hEditRect,NULL);
                break;
            case BAIC://�������к���
                StrAutoNewLine(NULL,GSMCALLBARRING35,pParam1,&hEditRect,NULL);
                break;
            case BIC_Roam://����ʱ�������к���
                StrAutoNewLine(NULL,GSMCALLBARRING351,pParam1,&hEditRect,NULL);
                break;
            }
            StrAutoNewLine(NULL,(iBS == iUnBSParam) ? "" : pBSContentSearch[iBS],
                pParam2,&hEditRect,NULL);
            StrAutoNewLine(NULL,pCBInfo[i].Active ? GSMOPERATEACTIVATE : GSMOPERATEDEACTIVATE,
                pParam3,&hEditRect,NULL);

            StringCombination(pSucc,pParam1,pParam2,pParam3,NULL);
            strcat(pClew,pSucc);
            strcat(pClew,pNewLine);
            strcat(pClew,pNewLine);
        }
        SetWindowTextExt(hEdit,pClew);//hGSMEdit
        free(pClew);
        free(pCBInfo);
    }
    else if (GSM_CLIP_CHECK == GSMType)//CLIP check status
    {
        CLIPINFO clipinfo;
        ME_GetResult(&clipinfo,sizeof(CLIPINFO));
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
        printf("mb MB_GSM GSM_CLIP_CHECK:%d\r\n",clipinfo.Status);
#endif
        switch (clipinfo.Status)
        {
        case CLIPNONE:// δ�ṩCLIP����
            sprintf(pParam1,GSMCLIP,pColon,GSMCLIP_NONE);
            break;
        case CLIPALLOW:// �ṩCLIP����
            sprintf(pParam1,GSMCLIP,pColon,GSMCLIP_ALLOW);
            break;
        case CLIPUNKNOWN:// ���δ֪
            sprintf(pParam1,GSMCLIP,pColon,GSMCLIP_UNKNOWN);
            break;
        }
        StrAutoNewLine(NULL,pParam1,pSucc,&hEditRect,NULL);
        SetWindowTextExt(hEdit,pSucc);//hGSMEdit
    }
    else if (GSM_CLIR_CHECK == GSMType)//CLIR check status
    {
        CLIRINFO clirinfo;
        ME_GetResult(&clirinfo,sizeof(CLIRINFO));
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
        printf("mb MB_GSM GSM_CLIR_CHECK:%d\r\n",clirinfo.Status);
#endif
        switch (clirinfo.Status)
        {
        case CLIRNONE://δ�ṩCLIR����
            sprintf(pParam1,GSMCLIR,pColon,GSMCLIR1);
            break;
        case CLIRPERMANENT://�����÷�ʽ�ṩCLIR����
            sprintf(pParam1,GSMCLIR,pColon,GSMCLIR2);
            break;
        case CLIRUNKNOWN://���δ֪
            sprintf(pParam1,GSMCLIR,pColon,GSMCLIR3);
            break;
        case CLIRTEMPRES://CLIR��ʱģʽ����
            sprintf(pParam1,GSMCLIR,pColon,GSMCLIR4);
            break;
        case CLIRTEMPALLOW://CLIR��ʱģʽ����
            sprintf(pParam1,GSMCLIR,pColon,GSMCLIR5);
            break;
        }
        StrAutoNewLine(NULL,pParam1,pSucc,&hEditRect,NULL);
        SetWindowTextExt(hEdit,pSucc);//hGSMEdit
    }
    else if (GSM_COLP_CHECK == GSMType)//COLP check status(Connected Line identification Presentation)
    {
        COLPINFO colpinfo;
        ME_GetResult(&colpinfo,sizeof(COLPINFO));
        sprintf(pSucc,GSMCOLP,pColon,colpinfo.Active ? GSMOPERATEACTIVATE : GSMOPERATEDEACTIVATE);
        SetWindowTextExt(hEdit,pSucc);//hGSMEdit
    }
    else if (GSM_COLR_CHECK == GSMType)//Check status COLR(connected Line identification Restriction)
    {
        COLRINFO colrinfo;
        ME_GetResult(&colrinfo,sizeof(COLRINFO));
        sprintf(pSucc,GSMCOLR,pColon,colrinfo.Active ? GSMOPERATEACTIVATE : GSMOPERATEDEACTIVATE);
        SetWindowTextExt(hEdit,pSucc);//hGSMEdit
    }
    else if (GSM_FILELOG_DELETE == GSMType)
    {
        RemoveLog();
        SetWindowTextExt(hEdit,"remove file!");
    }
    else if (GSM_FILELOG_READ == GSMType)
    {
        DWORD dFileSize;
        char * pString = NULL;
        dFileSize = ReadLog(NULL);
        if (0 == dFileSize)
        {
            SetWindowTextExt(hEdit,"");
        }
        else
        {
            dFileSize++;
            pString = (char*)malloc(dFileSize);
            if (NULL == pString)
            {
                SetWindowTextExt(hEdit,"malloc fail!");
            }
            else
            {
                memset(pString,0x00,dFileSize);
                ReadLog(pString);
                SetWindowTextExt(hEdit,pString);
                free(pString);
            }
        }
    }
    else
    {
        SetWindowTextExt(hEdit,pSucc);//hGSMEdit
        switch (GSMType)//��¼����ת�Ƶ���Ϣ
        {
        case GSM_PIN2://�޸�pin2��
            SetPin2Change();//��ʾPIN2���Ѿ��޸�
            break;
        case GSM_CALLFORWARDING1://����ת��
            if (iCallForward == GSM_CF_ACTIVATELOGIN)//���ע�����ת��
                SetForward_All(TRUE);
            else if (iCallForward == GSM_CF_DEACTIVATELOGIN)//��ֹ����ע�����ת��
                SetForward_All(FALSE);
            break;
        case GSM_CALLFORWARDING2://��������ת��
            if (iCallForward == GSM_CF_ACTIVATELOGIN)//���ע�����ת��
                SetForward_Condition(TRUE);
            else if (iCallForward == GSM_CF_DEACTIVATELOGIN)//��ֹ����ע�����ת��
                SetForward_Condition(FALSE);
            break;
        case GSM_CALLFORWARDING3://������ת��
            if (iCallForward == GSM_CF_ACTIVATELOGIN)//���ע�����ת��
            {
                SetForward_Uncdtional(TRUE);
            }
            else if (iCallForward == GSM_CF_DEACTIVATELOGIN)//��ֹ����ע�����ת��
            {
                SetForward_Uncdtional(FALSE);
                MessageMBPostMsgCallForward();//���¿�ʼ��ѯ�йغ���ת�Ƶ���Ϣ
            }
            break;
        case GSM_CALLFORWARDING4://��Ӧ��ת��
            if (iCallForward == GSM_CF_ACTIVATELOGIN)//���ע�����ת��
                SetForward_NoReply(TRUE);
            else if (iCallForward == GSM_CF_DEACTIVATELOGIN)//��ֹ����ע�����ת��
                SetForward_NoReply(FALSE);
            break;
        case GSM_CALLFORWARDING5://���ڷ�������ػ�ת��
            if (iCallForward == GSM_CF_ACTIVATELOGIN)//���ע�����ת��
                SetForward_NoReach(TRUE);
            else if (iCallForward == GSM_CF_DEACTIVATELOGIN)//��ֹ����ע�����ת��
                SetForward_NoReach(FALSE);
            break;
        case GSM_CALLFORWARDING6://��æת��
            if (iCallForward == GSM_CF_ACTIVATELOGIN)//���ע�����ת��
                SetForward_Busy(TRUE);
            else if (iCallForward == GSM_CF_DEACTIVATELOGIN)//��ֹ����ע�����ת��
                SetForward_Busy(FALSE);
            break;
        }
    }
    return TRUE;
}
/********************************************************************
* Function   OperateGSMWindow  
* Purpose    ����GSM����
* Params    iGSM_Type:��������,bShow:�Ƿ��ڴ������ڵ�ʱ����ʾ��,
            bAutoClose:�Ƿ����յ��ص���Ϣ֮���Զ��رմ���
* Return    
* Remarks   
**********************************************************************/
static  void    OperateGSMWindow(GSMTYPE iGSM_Type,BOOL bShow,BOOL bAutoClose)
{
    GSMType       = iGSM_Type;
    bGSMAutoClose = bAutoClose;

    if (bShow)
    {
        SetWindowText(hWndGSM,TITLECAPTION);//���ñ���
        SendMessage(hWndGSM,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK);
        SendMessage(hWndGSM,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT);

        ShowWindow(hWndGSM,SW_SHOW);
        UpdateWindow(hWndGSM);  
    }
}
//////////////////////////////////////////////////////////////////////////
/********************************************************************
* Function   GSMDealWith  
* Purpose    ��GSM�б��в����ַ���,ִ����Ӧ�ĺ���,�к���ִ�з���TRUE,���򷵻�FALSE
* Params    cPhoneNumber:����,
            bExecuteAtOnce:ִ�з�ʽ,TRUE:����ִ��,FALSE:ѡ�񲦺ż�֮����ִ��
* Return    TRUE:����GSM����,FALSE:������GSM����
* Remarks   
**********************************************************************/
BOOL    GSMDealWith(const char * cPhoneNumber,BOOL bExecuteAtOnce)
{
    int         i = 0,iPreLength,iFuncRes;
    int         iPrePlace;
    char        ** pGsmPre;
    GSMSTRUCT   GsmStruct;

    if (cPhoneNumber[strlen(cPhoneNumber) - 1] != Ch_well)//����������#
        return FALSE;

    if (hWndGSM == NULL)
    {
        CreateMBGSMWindow();
    }

    GsmStruct.hWnd      = hWndGSM;
    GsmStruct.iCallMsg  = IDC_GSMOPERATE;

    while ((GsmOperate[i].pGSMPre != NULL) && (GsmOperate[i].pFunction != NULL))
    {
        if (GsmOperate[i].bExecuteAtOnce != bExecuteAtOnce)
        {
            i++;
            continue;
        }
        pGsmPre = (char **)GsmOperate[i].pGSMPre;
        iPrePlace = 0;
        while (** pGsmPre)
        {
            iPreLength = strlen(* pGsmPre);

            if (strncmp(* pGsmPre,cPhoneNumber,iPreLength) == 0)
            {   //�ҵ��˷���GSMǰ׺���е�һ������
                if (GsmOperate[i].bGSMLengthMatch)//����GSM����ĳ���ƥ��
                {
                    if (strcmp(* pGsmPre,cPhoneNumber) != 0)
                        return FALSE;//��Ȼ�������GSMǰ׺,���Ǻ�ߵĲ����Ƕ����
                }
                bCancelOperate          = FALSE;//��ǰ������δ��ȡ��
                GsmStruct.pPhoneNumber  = (char *)cPhoneNumber;
                GsmStruct.pGsmPre       = (char **)GsmOperate[i].pGSMPre;
                GsmStruct.iStation      = iPrePlace;
                GsmStruct.wParam        = GsmOperate[i].wParam;
                iFuncRes = ((GsmOperate[i].pFunction)(&GsmStruct));
                if (iGSM == iFuncRes)
                {
                    SetWindowTextExt(GetDlgItem(hWndGSM,IDC_GSMEDIT),"");//hGSMEdit
                    MBWaitWindowState(hWndGSM,TRUE,IDC_CANCELOPERATE,TRUE);
                    return TRUE;
                }
                else if (iGSM_Error == iFuncRes)//AT�����ʧ��
                {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
                    printf("mb MB_GSM.c AT�����ʧ��:%s,GSMType:%d\r\n",
                        cPhoneNumber,GSMType);
#endif
                    GSMClewText_Fail(hWndGSM,0,0);
                    return TRUE;
                }
                else if (iGSM_Param == iFuncRes)//GSM����������
                {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
                    printf("mb MB_GSM.c GSM����������\r\n");
#endif
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }
            iPrePlace++;
            pGsmPre++;
        }
        i++;
    }
    return FALSE;
}
//////////////////////////////////////////////////////////////////////////
/********************************************************************
* Function   GsmIMEI  
* Purpose    ��ѯIMEI��
* Params    
* Return    �������GSM ��ѯIMEI�ű�׼�Ļ�����1,���򷵻�0,AT�����ʧ�ܷ���-1
* Remarks   
**********************************************************************/
static  int     GsmIMEI(GSMSTRUCT * pGsmStruct)
{
    OperateGSMWindow(GSM_IMEI,TRUE,FALSE);
    if (-1 == ME_GetEquipmentId(pGsmStruct->hWnd,pGsmStruct->iCallMsg))
    {
        strcpy(pFail,IMEIFAIL);
        return iGSM_Error;//-1
    }
    return iGSM;//1
}
/********************************************************************
* Function   GsmSecurity  
* Purpose    �޸�pin/pin2/puk/puk2��
* Params    
* Return    �������GSM�޸İ�ȫ�����׼�Ļ�����1,���򷵻�0,AT�����ʧ�ܷ���-1
* Remarks   
**********************************************************************/
static  int     GsmSecurity(GSMSTRUCT * pGsmStruct)
{
    int iPreLength;//���ǰ׺�ĳ���    
    char * p = NULL;
    const char * pCFClew[4];
    const int iSecurity[] = {LOCK_SC,LOCK_P2,PUK1,PUK2};
    const int iGSMOperate[] = {GSM_PIN1,GSM_PIN2,GSM_PUK1,GSM_PUK2};

    iPreLength  = strlen(pGsmStruct->pGsmPre[pGsmStruct->iStation]);//���ǰ׺�ĳ���
    p           = pGsmStruct->pPhoneNumber + iPreLength;//����GSM��ǰ׺

    memset(pParam1,0x00,iParamMaxLen);
    memset(pParam2,0x00,iParamMaxLen);
    memset(pParam3,0x00,iParamMaxLen);

    if (3 != GetGSMParam(p,pParam1,pParam2,pParam3))
        return iUnGSM;

    if ((strlen(pParam1) == 0) || (strlen(pParam2) == 0) || (strlen(pParam3) == 0))
    {
        AppMessageBox(NULL,GSM_WRONGPASSWORD,TITLECAPTION,WAITTIMEOUT);
        return iGSM_Param;
    }
    if (strcmp(pParam2,pParam3) != 0)//�����벻����ȷ������
    {
        AppMessageBox(NULL,GSM_WRONGPASSWORD,TITLECAPTION,WAITTIMEOUT);
        return iGSM_Param;//0
    }

    pCFClew[0] = GSMPIN1MODIFY;//pin1���޸�%s
    pCFClew[1] = GSMPIN2MODIFY;//pin2���޸�%s
    pCFClew[2] = GSMPUK1MODIFY;//PIN����%s
    pCFClew[3] = GSMPUK2MODIFY;//PIN2����%s

    sprintf(pParam1,pCFClew[pGsmStruct->iStation],GSMOPERATESUCC);
    StrAutoNewLine(NULL,pParam1,pSucc,&hEditRect,NULL);

    sprintf(pParam1,pCFClew[pGsmStruct->iStation],GSMOPERATEFAIL);
    StrAutoNewLine(NULL,pParam1,pFail,&hEditRect,NULL);

    OperateGSMWindow(iGSMOperate[pGsmStruct->iStation],TRUE,FALSE);

    switch (pGsmStruct->iStation)
    {
    case GSM_PIN1:
    case GSM_PIN2:
    case GSM_PUK1:
        if (-1 == ME_PasswordValidation(pGsmStruct->hWnd,pGsmStruct->iCallMsg,
            iSecurity[pGsmStruct->iStation],pParam1,pParam2))
        {
            return iGSM_Error;//-1
        }
        break;
    case GSM_PUK2:
        if (-1 == ME_SpecialDial(pGsmStruct->hWnd,pGsmStruct->iCallMsg,
            pGsmStruct->pPhoneNumber,FALSE))
        {
            return iGSM_Error;//-1
        }
        break;
    }
    return iGSM;
}
/********************************************************************
* Function   GsmUnlockPS  
* Purpose   ģ���������
* Params    
* Return    �������GSM ģ����������Ļ�����1,���򷵻�0,AT�����ʧ�ܷ���-1
* Remarks   
**********************************************************************/
static  int     GsmUnlockPS(GSMSTRUCT * pGsmStruct)
{
    int iPreLength;//���ǰ׺�ĳ���
    char cPassword[PHONENUMMAXLEN] = "",* p = NULL;

    iPreLength  = strlen(pGsmStruct->pGsmPre[pGsmStruct->iStation]);//���ǰ׺�ĳ���
    p           = pGsmStruct->pPhoneNumber + iPreLength;//����GSM��ǰ׺

    if (1 != GetGSMParam(p,cPassword,NULL,NULL))
        return iUnGSM;

    if (strlen(cPassword) == 0)
        return iUnGSM;

    sprintf(pParam1,GSMUNLOCKPS,GSMOPERATESUCC);
    StrAutoNewLine(NULL,pParam1,pSucc,&hEditRect,NULL);

    sprintf(pParam1,GSMUNLOCKPS,GSMOPERATEFAIL);
    StrAutoNewLine(NULL,pParam1,pFail,&hEditRect,NULL);

    OperateGSMWindow(GSM_UNLOCKPS,TRUE,FALSE);
    if (-1 == ME_SetPhoneLockStatus(pGsmStruct->hWnd,pGsmStruct->iCallMsg,
        LOCK_PS,cPassword,FALSE))
    {
        return iGSM_Error;//-1
    }
    return iGSM;//1
}
//////////////////////////////////////////////////////////////////////////
/********************************************************************
* Function   GsmForwardingOperate  
* Purpose   ����ע��/��ֹ��ע�����ת��
* Params    
* Return    �������GSM ����ע��/��ֹ��ע�����ת�Ʊ�׼�Ļ�����1,���򷵻�0,AT�����ʧ�ܷ���-1
* Remarks   
**********************************************************************/
static  int     GsmForwardingOperate(GSMSTRUCT * pGsmStruct)
{
    int  iPreLength;//���ǰ׺�ĳ���
    char cSC[10] = "",cClew[30] = "",* p = NULL;
    int  iParam;
    const char * pCFClew[6];
    const int iGSMOperate[] = 
    {
        GSM_CALLFORWARDING1,//���к���ת��
        GSM_CALLFORWARDING2,//��������ת��
        GSM_CALLFORWARDING3,//������ת��
        GSM_CALLFORWARDING4,//��Ӧ��ת��
        GSM_CALLFORWARDING5,//���ڷ�������ػ�ת��
        GSM_CALLFORWARDING6 //��æת��
    };

    memset(pParam1,0x00,iParamMaxLen);
    memset(pParam2,0x00,iParamMaxLen);
    memset(pParam3,0x00,iParamMaxLen);
    memset(pParam4,0x00,iParamMaxLen);

    iPreLength  = strlen(pGsmStruct->pGsmPre[pGsmStruct->iStation]);//���ǰ׺�ĳ���
    p           = pGsmStruct->pPhoneNumber + iPreLength;//����GSM��ǰ׺
    iGSMBSType  = iUnBSParam;
    iParam      = GetGSMParam(p,pParam1,pParam2,pParam3);

    switch (iParam)
    {
    case 0:
        break;
    case 1://���벻��Ϊ��
        if (strlen(pParam1) == 0)
            return iUnGSM;

        break;
    case 2://BS��������Ϊ��
        iGSMBSType = InStrArray(pParam2,pGSMCode_BS);
        if (iGSMBSType == -1)//�Ƿ���BS����
            return iUnGSM;

        break;
    case 3://T��������Ϊ��(BS��ѡ)
        if (strlen(pParam3) == 0)
            return iUnGSM;

        if (strlen(pParam2) != 0)//��BS����
        {
            iGSMBSType = InStrArray(pParam2,pGSMCode_BS);
            if (iGSMBSType == -1)//�Ƿ���BS����
                return iUnGSM;
        }
        if (!StrIsNumber(pParam3))//�Ƿ���T����
            return iUnGSM;

        GetGSMSC(pGsmStruct->pPhoneNumber,cSC);
        if (InStrArray(cSC,pGSMCode_T_SC) == -1)//�Ƿ���SC����
            return iUnGSM;

        break;
    default:
        return iUnGSM;
    }
    switch (pGsmStruct->wParam)
    {
    case GSM_CF_ACTIVATE:           //�������ת��
        strcpy(cClew,GSMOPERATEACTIVATE);
        break;
    case GSM_CF_DEACTIVATE:         //��ֹ����ת��
        strcpy(cClew,GSMOPERATEDEACTIVATE);
        break;
    case GSM_CF_ACTIVATELOGIN:      //���ע�����ת��
        strcpy(cClew,GSMOPERATE_RA);
        break;
    case GSM_CF_DEACTIVATELOGIN:    //��ֹ����ע�����ת��
        strcpy(cClew,GSMOPERATE_UD);
        break;
    }
    iCallForward = pGsmStruct->wParam;//����Ժ���ת�ƵĲ���

    pCFClew[0] = GSMCALLFORWARDING1;//���к���ת��
    pCFClew[1] = GSMCALLFORWARDING2;//��������ת��
    pCFClew[2] = GSMCALLFORWARDING3;//������ת��
    pCFClew[3] = GSMCALLFORWARDING4;//��Ӧ��ת��
    pCFClew[4] = GSMCALLFORWARDING5;//���ڷ�������ػ�ת��
    pCFClew[5] = GSMCALLFORWARDING6;//��æת��

    StrAutoNewLine(NULL,pCFClew[pGsmStruct->iStation],pParam2,&hEditRect,NULL);
    StrAutoNewLine(NULL,(iUnBSParam == iGSMBSType) ? "" : pBSContent[iGSMBSType],pParam3,&hEditRect,NULL);
    StrAutoNewLine(NULL,cClew,pParam4,&hEditRect,NULL);

    StrAutoNewLine(NULL,GSMOPERATESUCC_1,pParam1,&hEditRect,NULL);
    StringCombination(pSucc,pParam1,pParam2,pParam3,pParam4);

    StrAutoNewLine(NULL,GSMOPERATEFAIL_1,pParam1,&hEditRect,NULL);
    StringCombination(pFail,pParam1,pParam2,pParam3,pParam4);

    OperateGSMWindow(iGSMOperate[pGsmStruct->iStation],TRUE,FALSE);

    if (-1 == ME_SpecialDial(pGsmStruct->hWnd,pGsmStruct->iCallMsg,
        pGsmStruct->pPhoneNumber,FALSE))
    {
        return iGSM_Error;//-1
    }
    return iGSM;//1
}
/********************************************************************
* Function   GsmForwardingCheck  
* Purpose   ������ת�Ƶ�״̬
* Params    
* Return    �������GSM �������ת�Ʊ�׼�Ļ�����1,���򷵻�0,AT�����ʧ�ܷ���-1
* Remarks   
**********************************************************************/
static  int     GsmForwardingCheck(GSMSTRUCT * pGsmStruct)
{
    int     iPreLength;//���ǰ׺�ĳ���
    char  * p = NULL,cSC[10] = "";
    int     iParam;
    const char *  pCFClew[6];
    const int iGSMOperate[] = 
    {
        GSM_CALLFORWARDING1_CHECK,//��ѯ����ת��
        GSM_CALLFORWARDING2_CHECK,//��ѯ��������ת��
        GSM_CALLFORWARDING3_CHECK,//��ѯ������ת��
        GSM_CALLFORWARDING4_CHECK,//��ѯ��Ӧ��ת��
        GSM_CALLFORWARDING5_CHECK,//��ѯ���ڷ�������ػ�ת��
        GSM_CALLFORWARDING6_CHECK //��ѯ��æת��
    };

    memset(pParam1,0x00,iParamMaxLen);
    memset(pParam2,0x00,iParamMaxLen);
    memset(pParam3,0x00,iParamMaxLen);

    iPreLength  = strlen(pGsmStruct->pGsmPre[pGsmStruct->iStation]);//���ǰ׺�ĳ���
    p           = pGsmStruct->pPhoneNumber + iPreLength;//����GSM��ǰ׺
    iGSMBSType  = iUnBSParam;
    iParam      = GetGSMParam(p,pParam1,pParam2,pParam3);

    switch (iParam)
    {
    case 0://û�в���
        break;
    case 2:
        if ((strlen(pParam1) != 0) || (strlen(pParam2) == 0))//�е绰�������û��BS����
            return iUnGSM;

        iGSMBSType = InStrArray(pParam2,pGSMCode_BS);
        if (iGSMBSType == -1)//�Ƿ���BS����
            return iUnGSM;

        break;
    case 3:
        if ((strlen(pParam1) != 0) ||   //�е绰�������û��BS����
            (strlen(pParam2) == 0) ||   //û��BS����
            (strlen(pParam3) == 0))     //û��T����
            return iUnGSM;

        iGSMBSType = InStrArray(pParam2,pGSMCode_BS);
        if (iGSMBSType == -1)//�Ƿ���BS����
            return iUnGSM;

        GetGSMSC(pGsmStruct->pPhoneNumber,cSC);
        if (InStrArray(cSC,pGSMCode_T_SC) == -1)//�Ƿ���SC����
            return iUnGSM;

        break;
    default:
        return iUnGSM;
    }

    pCFClew[0] = GSMCALLFORWARDING1;//���к���ת��
    pCFClew[1] = GSMCALLFORWARDING2;//��������ת��
    pCFClew[2] = GSMCALLFORWARDING3;//������ת��
    pCFClew[3] = GSMCALLFORWARDING4;//��Ӧ��ת��
    pCFClew[4] = GSMCALLFORWARDING5;//���ڷ�������ػ�ת��
    pCFClew[5] = GSMCALLFORWARDING6;//��æת��

    StrAutoNewLine(NULL,GSMOPERATESEARCH_FAIL,pParam1,&hEditRect,NULL);
    StrAutoNewLine(NULL,pCFClew[pGsmStruct->iStation],pParam2,&hEditRect,NULL);
    StrAutoNewLine(NULL,(iUnBSParam == iGSMBSType) ? "" : pBSContent[iGSMBSType],pParam3,&hEditRect,NULL);
    StringCombination(pFail,pParam1,pParam2,pParam3,NULL);

    OperateGSMWindow(iGSMOperate[pGsmStruct->iStation],TRUE,FALSE);

    if (-1 == ME_SpecialDial(pGsmStruct->hWnd,pGsmStruct->iCallMsg,
        pGsmStruct->pPhoneNumber,FALSE))
    {
        return iGSM_Error;//-1
    }
    return iGSM;//1
}
//////////////////////////////////////////////////////////////////////////
/********************************************************************
* Function   GsmModifyCallBarring  
* Purpose    �޸ĺ���������
* Params    
* Return    �������GSM�޸İ�ȫ�����׼�Ļ�����1,���򷵻�0,AT�����ʧ�ܷ���-1
* Remarks   
**********************************************************************/
static  int     GsmModifyCallBarring(GSMSTRUCT * pGsmStruct)
{
    int iPreLength;//���ǰ׺�ĳ���
    char * p = NULL;
    int iparam;

    memset(pParam1,0x00,iParamMaxLen);
    memset(pParam2,0x00,iParamMaxLen);
    memset(pParam3,0x00,iParamMaxLen);

    iPreLength  = strlen(pGsmStruct->pGsmPre[pGsmStruct->iStation]);//���ǰ׺�ĳ���
    p           = pGsmStruct->pPhoneNumber + iPreLength;//����GSM��ǰ׺
    iparam      = GetGSMParam(p,pParam1,pParam2,pParam3);

    if (3 != iparam)
        return iUnGSM;

    sprintf(pParam1,GSMBARRINGMODIFY,GSMOPERATESUCC);
    StrAutoNewLine(NULL,pParam1,pSucc,&hEditRect,NULL);

    sprintf(pParam1,GSMBARRINGMODIFY,GSMOPERATEFAIL);
    StrAutoNewLine(NULL,pParam1,pFail,&hEditRect,NULL);

    OperateGSMWindow(GSM_CALLBAR,TRUE,FALSE);
    if (-1 == ME_SpecialDial(pGsmStruct->hWnd,pGsmStruct->iCallMsg,
        pGsmStruct->pPhoneNumber,FALSE))
    {
        return iGSM_Error;//-1
    }
    return iGSM;//1
}
/********************************************************************
* Function   GsmCallBarringOperate  
* Purpose   ����,��ֹ��������
* Params    
* Return    �������GSM ����,��ֹ�������Ʊ�׼�Ļ�����1,���򷵻�0,AT�����ʧ�ܷ���-1
* Remarks   
**********************************************************************/
static  int     GsmCallBarringOperate(GSMSTRUCT * pGsmStruct)
{
    int iPreLength;//���ǰ׺�ĳ���
    char cCallBarring[PHONENUMMAXLEN] = "",cBS[PHONENUMMAXLEN] = "";
    char * p = NULL;
    int iparam;
    const char * pCFClew[8];
    const int iGSMOperate[] = 
    {
        GSM_CALLBARRING33, //�������в����绰
        GSM_CALLBARRING331,//�������в������ʳ�;,
        GSM_CALLBARRING332,//�������в������ʳ�;(������)
        GSM_CALLBARRING35, //�������в���绰
        GSM_CALLBARRING351,//���ƹ�������ʱ����
        GSM_CALLBARRING330,//�������ͨ������
        GSM_CALLBARRING333,//������в�������
        GSM_CALLBARRING353 //������в�������
    };

    memset(pParam1,0x00,iParamMaxLen);
    memset(pParam2,0x00,iParamMaxLen);
    memset(pParam3,0x00,iParamMaxLen);
    memset(pParam4,0x00,iParamMaxLen);

    iPreLength = strlen(pGsmStruct->pGsmPre[pGsmStruct->iStation]);//���ǰ׺�ĳ���
    iGSMBSType = iUnBSParam;

    p = pGsmStruct->pPhoneNumber + iPreLength;//����GSM��ǰ׺
    iparam = GetGSMParam(p,cCallBarring,cBS,NULL);

    if ((-1 == iparam) || (0 == iparam) || (3 == iparam))
        return iUnGSM;

    if (strlen(cCallBarring) == 0)
        return iUnGSM;

    if (2 == iparam)
    {
        iGSMBSType = InStrArray(cBS,pGSMCode_BS);
        if (iGSMBSType == -1)//�Ƿ���BS����
            return iUnGSM;
    }
    pCFClew[0] = GSMCALLBARRING33; //�������в����绰
    pCFClew[1] = GSMCALLBARRING331;//�������в������ʳ�;,
    pCFClew[2] = GSMCALLBARRING332;//�������в������ʳ�;(������) 
    pCFClew[3] = GSMCALLBARRING35; //�������в���绰
    pCFClew[4] = GSMCALLBARRING351;//���ƹ�������ʱ����
    pCFClew[5] = GSMCALLBARRING330;//�������ͨ������
    pCFClew[6] = GSMCALLBARRING333;//������в�������
    pCFClew[7] = GSMCALLBARRING353;//������в�������

    StrAutoNewLine(NULL,pCFClew[pGsmStruct->iStation],pParam2,&hEditRect,NULL);
    StrAutoNewLine(NULL,(iUnBSParam == iGSMBSType) ? "" : pBSContent[iGSMBSType],pParam3,&hEditRect,NULL);
    StrAutoNewLine(NULL,pGsmStruct->wParam ? GSMOPERATEACTIVATE : GSMOPERATEDEACTIVATE,pParam4,&hEditRect,NULL);

    StrAutoNewLine(NULL,GSMOPERATESUCC_1,pParam1,&hEditRect,NULL);
    StringCombination(pSucc,pParam1,pParam2,pParam3,pParam4);

    StrAutoNewLine(NULL,GSMOPERATEFAIL_1,pParam1,&hEditRect,NULL);
    StringCombination(pFail,pParam1,pParam2,pParam3,pParam4);

    OperateGSMWindow(iGSMOperate[pGsmStruct->iStation],TRUE,FALSE);
    if (-1 == ME_SpecialDial(pGsmStruct->hWnd,pGsmStruct->iCallMsg,
        pGsmStruct->pPhoneNumber,FALSE))
    {
        return iGSM_Error;
    }
    return iGSM;//0
}
/********************************************************************
* Function   GsmCallBarringCheck  
* Purpose   ��ѯ�������Ƶ�״̬
* Params    
* Return    �������GSM ��������״̬��׼�Ļ�����1,���򷵻�0,AT�����ʧ�ܷ���-1
* Remarks   *#SC#������*#SC**BS#
**********************************************************************/
static  int     GsmCallBarringCheck(GSMSTRUCT * pGsmStruct)
{
    int iPreLength;//���ǰ׺�ĳ���
    char cBS[PHONENUMMAXLEN] = "",* p = NULL;
    int iparam;
    const char * pCFClew[5];
    const int iGSMOperate[] =
    {
        GSM_CALLBARRING33_CHECK,    //33 �������в����绰
        GSM_CALLBARRING331_CHECK,   //331 �������в������ʳ�;
        GSM_CALLBARRING332_CHECK,   //332 �������в������ʳ�;,���˱���
        GSM_CALLBARRING35_CHECK,    //35 �������в���绰
        GSM_CALLBARRING351_CHECK    //351 ���ƹ�������ʱ����
    };

    memset(pParam1,0x00,iParamMaxLen);
    memset(pParam2,0x00,iParamMaxLen);
    memset(pParam3,0x00,iParamMaxLen);

    iGSMBSType = iUnBSParam;
    if (pGsmStruct->wParam == GSM_CB_CHECKBS)
    {
        iPreLength  = strlen(pGsmStruct->pGsmPre[pGsmStruct->iStation]);//���ǰ׺�ĳ���
        p           = pGsmStruct->pPhoneNumber + iPreLength;//����GSM��ǰ׺
        iparam      = GetGSMParam(p,cBS,NULL,NULL);

        if (1 != iparam)//����ֻ������һ������
            return iUnGSM;        

        iGSMBSType = InStrArray(cBS,pGSMCode_BS);
        if (iGSMBSType == -1)//�Ƿ���BS����
            return iUnGSM;
    }
    pCFClew[0] = GSMCALLBARRING33;  //33 �������в����绰
    pCFClew[1] = GSMCALLBARRING331; //331 �������в������ʳ�;
    pCFClew[2] = GSMCALLBARRING332; //332 �������в������ʳ�;,���˱���
    pCFClew[3] = GSMCALLBARRING35;  //35 �������в���绰
    pCFClew[4] = GSMCALLBARRING351; //351 ���ƹ�������ʱ����

    StrAutoNewLine(NULL,GSMOPERATESEARCH_FAIL,pParam1,&hEditRect,NULL);
    StrAutoNewLine(NULL,pCFClew[pGsmStruct->iStation],pParam2,&hEditRect,NULL);
    StrAutoNewLine(NULL,(iUnBSParam == iGSMBSType) ? "" : pBSContent[iGSMBSType],pParam3,&hEditRect,NULL);
    StringCombination(pFail,pParam1,pParam2,pParam3,NULL);

    OperateGSMWindow(iGSMOperate[pGsmStruct->iStation],TRUE,FALSE);

    if (-1 == ME_SpecialDial(pGsmStruct->hWnd,pGsmStruct->iCallMsg,
        pGsmStruct->pPhoneNumber,FALSE))
    {
        return iGSM_Error;
    }
    return iGSM;//0
}
//////////////////////////////////////////////////////////////////////////
/********************************************************************
* Function   GsmCallWaitingOperate  
* Purpose    ��,��ѯ,�رպ��еȴ�
* Params    
* Return    �������GSM ��,��ѯ,�رպ��еȴ���׼�Ļ�����1,���򷵻�0,AT�����ʧ�ܷ���-1
* Remarks   
**********************************************************************/
static  int     GsmCallWaitingOperate(GSMSTRUCT * pGsmStruct)
{
    char cBS[PHONENUMMAXLEN] = "",* p;
    int iPreLength;//���ǰ׺�ĳ���
    int iparam;

    iPreLength  = strlen(pGsmStruct->pGsmPre[pGsmStruct->iStation]);//���ǰ׺�ĳ���
    p           = pGsmStruct->pPhoneNumber + iPreLength;//����GSM��ǰ׺
    iGSMBSType  = iUnBSParam;
    iparam      = GetGSMParam(p,cBS,NULL,NULL);

    switch (iparam)
    {
    case 0://û��BS����
        break;
    case 1://�в���,������BS����
        iGSMBSType = InStrArray(cBS,pGSMCode_BS);
        if (-1 == iGSMBSType)
            return iUnGSM;
        break;
    default:
        return iUnGSM;
    }
    switch (pGsmStruct->iStation)
    {
    case 0://��
        StrAutoNewLine(NULL,GSMCALLWAITING,pParam2,&hEditRect,NULL);
        StrAutoNewLine(NULL,(iUnBSParam == iGSMBSType) ? "" : pBSContent[iGSMBSType],pParam3,&hEditRect,NULL);
        StrAutoNewLine(NULL,GSMOPERATEACTIVATE,pParam4,&hEditRect,NULL);

        StrAutoNewLine(NULL,GSMOPERATESUCC_1,pParam1,&hEditRect,NULL);
        StringCombination(pSucc,pParam1,pParam2,pParam3,pParam4);

        StrAutoNewLine(NULL,GSMOPERATEFAIL_1,pParam1,&hEditRect,NULL);
        StringCombination(pFail,pParam1,pParam2,pParam3,pParam4);

        OperateGSMWindow(GSM_CALLWAIT_ACTIVE,TRUE,FALSE);
        break;
    case 1://��ѯ
        StrAutoNewLine(NULL,GSMOPERATESEARCH_FAIL,pParam1,&hEditRect,NULL);
        StrAutoNewLine(NULL,GSMCALLWAITING,pParam2,&hEditRect,NULL);
        StrAutoNewLine(NULL,(iUnBSParam == iGSMBSType) ? "" : pBSContent[iGSMBSType],pParam3,&hEditRect,NULL);
        StringCombination(pFail,pParam1,pParam2,pParam3,NULL);

        OperateGSMWindow(GSM_CALLWAIT_CHECK,TRUE,FALSE);
        break;
    case 2://�ر�
        StrAutoNewLine(NULL,GSMCALLWAITING,pParam2,&hEditRect,NULL);
        StrAutoNewLine(NULL,(iUnBSParam == iGSMBSType) ? "" : pBSContent[iGSMBSType],pParam3,&hEditRect,NULL);
        StrAutoNewLine(NULL,GSMOPERATEDEACTIVATE,pParam4,&hEditRect,NULL);

        StrAutoNewLine(NULL,GSMOPERATESUCC_1,pParam1,&hEditRect,NULL);
        StringCombination(pSucc,pParam1,pParam2,pParam3,pParam4);

        StrAutoNewLine(NULL,GSMOPERATEFAIL_1,pParam1,&hEditRect,NULL);
        StringCombination(pFail,pParam1,pParam2,pParam3,pParam4);

        OperateGSMWindow(GSM_CALLWAIT_DEACTIVE,TRUE,FALSE);
        break;
    }
    if (-1 == ME_SpecialDial(pGsmStruct->hWnd,pGsmStruct->iCallMsg,
        pGsmStruct->pPhoneNumber,FALSE))
    {
        return iGSM_Error;
    }
    return iGSM;//1
}
//////////////////////////////////////////////////////////////////////////
/********************************************************************
* Function   GsmPNPCheck  
* Purpose    Phone number presentation�Ĳ�ѯ
* Params    
* Return    �������GSM ��ѯCLIP״̬��׼�Ļ�����1,���򷵻�0,AT�����ʧ�ܷ���-1
* Remarks   
**********************************************************************/
static  int     GsmPNPCheck(GSMSTRUCT * pGsmStruct)
{
    switch (pGsmStruct->iStation)
    {
    case 0://CLIP check status,*#30#
        sprintf(pParam1,GSMCLIP,GSMOPERATESEARCH,GSMOPERATEFAIL);
        StrAutoNewLine(NULL,pParam1,pFail,&hEditRect,NULL);
        OperateGSMWindow(GSM_CLIP_CHECK,TRUE,FALSE);
        break;
    case 1://CLIR check status,*#31#
        sprintf(pParam1,GSMCLIR,GSMOPERATESEARCH,GSMOPERATEFAIL);
        StrAutoNewLine(NULL,pParam1,pFail,&hEditRect,NULL);
        OperateGSMWindow(GSM_CLIR_CHECK,TRUE,FALSE);
        break;
    case 2://COLP check status(Connected Line identification Presentation),*#76#
        sprintf(pParam1,GSMCOLP,GSMOPERATESEARCH,GSMOPERATEFAIL);
        StrAutoNewLine(NULL,pParam1,pFail,&hEditRect,NULL);
        OperateGSMWindow(GSM_COLP_CHECK,TRUE,FALSE);
        break;
    case 3://Check status COLR(connected Line identification Restriction),*#77#
        sprintf(pParam1,GSMCOLR,GSMOPERATESEARCH,GSMOPERATEFAIL);
        StrAutoNewLine(NULL,pParam1,pFail,&hEditRect,NULL);
        OperateGSMWindow(GSM_COLR_CHECK,TRUE,FALSE);
        break;
    }
    if (-1 == ME_SpecialDial(pGsmStruct->hWnd,pGsmStruct->iCallMsg,
                            pGsmStruct->pPhoneNumber,FALSE))
    {
        return iGSM_Error;
    }
    return iGSM;//1
}
/********************************************************************
* Function   GsmCLIRDeal  
* Purpose    CLIR�Ĳ���
* Params    
* Return    �������GSM ��ѯCLIP״̬��׼�Ļ�����1,���򷵻�0,AT�����ʧ�ܷ���-1
* Remarks   
**********************************************************************/
static  int     GsmCLIRDeal(GSMSTRUCT * pGsmStruct)
{
    switch (pGsmStruct->iStation)
    {
    case 0://Suppress CLIR for a voice call,*31#
        sprintf(pParam1,GSMCLIRSUPPRESS,GSMOPERATESUCC);
        StrAutoNewLine(NULL,pParam1,pSucc,&hEditRect,NULL);

        sprintf(pParam1,GSMCLIRSUPPRESS,GSMOPERATEFAIL);
        StrAutoNewLine(NULL,pParam1,pFail,&hEditRect,NULL);

        OperateGSMWindow(GSM_CLIR_SUPPRESS,TRUE,FALSE);
        break;
    case 1://Activate CLIR for a voice call,#31#
        sprintf(pParam1,GSMCLIRINVOKE,GSMOPERATESUCC);
        StrAutoNewLine(NULL,pParam1,pSucc,&hEditRect,NULL);

        sprintf(pParam1,GSMCLIRINVOKE,GSMOPERATEFAIL);
        StrAutoNewLine(NULL,pParam1,pFail,&hEditRect,NULL);

        OperateGSMWindow(GSM_CLIR_INVOKE,TRUE,FALSE);
        break;
    }
    if (-1 == ME_SpecialDial(pGsmStruct->hWnd,pGsmStruct->iCallMsg,
                            pGsmStruct->pPhoneNumber,FALSE))
    {
        return iGSM_Error;//-1
    }
    return iGSM;//1
}

static  int     ReadFileLog_Delete(GSMSTRUCT * pGsmStruct)
{
    OperateGSMWindow(GSM_FILELOG_DELETE,TRUE,FALSE);
    PostMessage(pGsmStruct->hWnd,pGsmStruct->iCallMsg,ME_RS_SUCCESS,0);
    return iGSM;//1
}
static  int     ReadFileLog_Read(GSMSTRUCT * pGsmStruct)
{
    OperateGSMWindow(GSM_FILELOG_READ,TRUE,FALSE);
    PostMessage(pGsmStruct->hWnd,pGsmStruct->iCallMsg,ME_RS_SUCCESS,0);
    return iGSM;//1
}
//////////////////////////////////////////////////////////////////////////
/********************************************************************
* Function   GetGSMSC  
* Purpose   ���SC����
* Params    pGsmCode��Ҫɨ����ַ���,pSC:����SC����
* Return    
* Remarks   
**********************************************************************/
static  void    GetGSMSC(const char * pGsmCode,char * pSC)
{
    char cSC[PHONENUMMAXLEN] = "",* p = NULL;

    p = (char *)pGsmCode;

    while ((* p == Ch_Asterisk) || (* p == Ch_well))
        p++;

    while ((* p != Ch_Asterisk) && (* p != Ch_well))
        strncat(cSC,p++,1);

    strcpy(pSC,cSC);
}
/********************************************************************
* Function   GetGSMParam  
* Purpose   ��ǰ�ַ����е�GSM�����ĸ���
* Params    pGsmCode:��Ҫɨ����ַ���
            param1:���ز���һ
            param2:���ز�����
            param3:���ز�����
            bFirstAsterisk:����һ���ַ����ͺŵ�ʱ��,�Ƿ���д���
* Return    ��ǰ�ַ����е�GSM�����ĸ���,-1:��ʾ�ⲻ��һ���Ϸ���GSM����
* Remarks   
**********************************************************************/
static  int     GetGSMParam(const char * pGsmCode,char * param1,char * param2,char * param3)
{
    char * p1,* p2,* pEnd,* p3;
    int iparam1,iparam2,iparam3;

    pEnd = strstr(pGsmCode,pWell);//���ҽ�����"#"
    if (NULL == pEnd)//û�ҵ�,��ʾ������
        return -1;

    p3 = pEnd;//ָ�������
    p3++;
    if (* p3 != Ch_End)//#��֮�����ַ�,
        return -1;

    if (* pGsmCode == Ch_Asterisk)//��һ���ַ����Ǻ�(*),��ʾ����Ǻ��ǲ����ķָ���,����
    {
        pGsmCode++;
        if (* pGsmCode == Ch_well)//�Ǻ�֮����Ǿ���,�ⲻ��һ���Ϸ���GSM����
            return -1;
    }
    if (* pGsmCode == Ch_well)//��һ���ַ��ǽ�����(#)��ʾû�в���
    {
        if (param1 != NULL)
            strcpy(param1,"");

        if (param2 != NULL)
            strcpy(param2,"");

        if (param3 != NULL)
            strcpy(param3,"");

        return 0;
    }

    p1 = strstr(pGsmCode,pAsterisk);//���ҵ�һ���Ǻ�
    if (NULL == p1)//û�ҵ��Ǻ�,��ʾֻ��һ������(12#)
    {
        iparam1 = abs(pGsmCode - pEnd);
        if (param1 != NULL)
        {
            if (0 != iparam1)
                strncpy(param1,pGsmCode,iparam1);
            else
                strcpy(param1,"");
        }
        if (param2 != NULL)
            strcpy(param2,"");

        if (param3 != NULL)
            strcpy(param3,"");

        return 1;
    }
    //�ҵ���һ���Ǻ�(12*)
    iparam1 = abs(pGsmCode - p1);
    if (param1 != NULL)
    {
        if (0 != iparam1)
            strncpy(param1,pGsmCode,iparam1);//��õ�һ������
        else
            strcpy(param1,"");
    }

    p1++;//������һ������

    p2 = strstr(p1,pAsterisk);//���ҵڶ����Ǻ�
    if (NULL == p2)//û�ҵ��Ǻ�,��ʾ�ж�������(12*23#)
    {
        iparam2 = abs(p1 - pEnd);
        if (param2 != NULL)
        {
            if (0 != iparam2)
                strncpy(param2,p1,iparam2);
            else
                strcpy(param2,"");
        }
        if (param3 != NULL)
            strcpy(param3,"");

        return 2;
    }
    //�ҵ��ڶ����Ǻ�(12*23*)
    iparam2 = abs(p2 - p1);
    if (param2 != NULL)
    {
        if (0 != iparam2)
            strncpy(param2,p1,iparam2);//��õڶ�������
        else
            strcpy(param2,"");
    }

    p2++;//�����ڶ�������
    iparam3 = abs(p2 - pEnd);

    if (param3 != NULL)
    {
        if (0 != iparam3)
            strncpy(param3,p2,iparam3);//��õ���������
        else
            strcpy(param3,"");
    }
    return 3;
}
/********************************************************************
* Function   StringCombination  
* Purpose   ��p1,p2,p3,p4��ϳ�һ���ַ���
* Params    
* Return    
* Remarks   
**********************************************************************/
static  void    StringCombination(char * pResult,const char * p1,const char * p2,
                                  const char * p3,const char * p4)
{
    if (p1 != NULL)
    {
        if (strlen(p1) != 0)
            strcpy(pResult,p1);
    }
    if (p2 != NULL)
    {
        if (strlen(p2) != 0)
        {
            strcat(pResult,pNewLine);
            strcat(pResult,p2);
        }
    }
    if (p3 != NULL)
    {
        if (strlen(p3) != 0)
        {
            strcat(pResult,pNewLine);
            strcat(pResult,p3);
        }
    }
    if (p4 != NULL)
    {
        if (strlen(p4) != 0)
        {
            strcat(pResult,pNewLine);
            strcat(pResult,p4);
        }
    }
}
/********************************************************************
* Function   SetWindowTextExt  
* Purpose   �����ı����е�����
* Params    pString:���õ�����
* Return    
* Remarks   
**********************************************************************/
static  void    SetWindowTextExt(HWND hEdit,const char * pString)
{
    SetWindowText(hEdit,pString ? pString : "");
}
/********************************************************************
* Function   GsmSpecific  
* Purpose   ��ǰ�����ǰ׺�Ƿ���һ��GSM���б�־
* Params    pNumber:�绰����
* Return    ��ǰ�������ӵڼ����ַ���ʼ
* Remarks   
**********************************************************************/
int     GsmSpecific(const char * pNumber)
{
    int iGSMPlace = 0;
    char ** p;

    if (pNumber == NULL)
        return 0;
    if (strlen(pNumber) == 0)
        return 0;

    p = (char**)pGSMSpecific;
    while (**p)
    {
        if (strncmp(* p,pNumber,strlen(* p)) == 0)
            return strlen(pGSMSpecific[iGSMPlace]);

        p++;
        iGSMPlace++;
    }
    return 0;//��ǰ�����ǰ׺����һ��GSM���б�־
}
//////////����ΪUSSD����//////////////////////////////////////////////
/********************************************************************
* Function   USSDDealWith  
* Purpose    USSD����
* Params    pString:����,
            bCalling:�Ƿ���ͨ����
* Return    TRUE:����USSD����,FALSE:������USSD����
* Remarks   
**********************************************************************/
BOOL    USSDDealWith(const char * pString,BOOL bCalling)
{
    int iStrLen;
    BOOL bUssd = FALSE;

    if (pString == NULL)
        return FALSE;

    iStrLen = strlen(pString);
    if (0 == iStrLen)
        return FALSE;

    if (pString[iStrLen - 1] == Ch_well)
    {
        bUssd = TRUE;
    }
    else if (iStrLen < 3)
    {
        if (bCalling)
            bUssd = TRUE;
        else if (!((iStrLen == 2) && (pString[0] == Ch_Num1)))
            bUssd = TRUE;
    }

    if (bUssd)
    {
        pUssd = ME_USSD_Request((char* )pString);
        if (pUssd == NULL)
            return FALSE;

        bCancelOperate = FALSE;//��ǰ������δ��ȡ��
        MBWaitWindowState(hWndGSM,TRUE,IDC_CANCELOPERATE,TRUE);
        return TRUE;
    }
    return FALSE;
}
/********************************************************************
* Function   USSDDealRespose  
* Purpose    �����յ���USSD��Ϣ
* Params    
* Return    
* Remarks   
**********************************************************************/
static  BOOL    USSDDealRespose(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    MBWaitWindowState(hWnd,FALSE,NULL,FALSE);
    switch (wParam)
    {
    case ME_USSD_ABORT://ȡ��
        ShowWindow(hWnd,SW_HIDE);
        return TRUE;

    case ME_USSD_REFUSE://�ܾ�,�û�ֻ��ѡ���˳�
        StrAutoNewLine(NULL,USSD_NOSERVICE,pParam1,&hEditRect,NULL);
        SetWindowTextExt(GetDlgItem(hWnd,IDC_GSMEDIT),pParam1);

        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK);
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT);
        ShowWindow(hWnd,SW_SHOW);
        UpdateWindow(hWnd);
        return TRUE;

    case ME_USSD_NOTIFY://֪ͨ
        SetUssdInfo(hWnd);
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_USSD_RIGHTBUTTON,(LPARAM)BUTTONTXTRIGHT);
        return TRUE;

    case ME_USSD_REQUEST://����
        SetUssdInfo(hWnd);
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_USSD_LEFTBUTTON,1),(LPARAM)BUTTONTXTLEFT_REPLY);
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_USSD_RIGHTBUTTON,(LPARAM)BUTTONTXTRIGHT);
        return TRUE;

    case ME_USSD_DISCONNECT:
        SetUssdInfo(hWnd);
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK);
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT);
        return TRUE;

    default:
        return FALSE;
    }
}
/********************************************************************
* Function   SetUssdInfo  
* Purpose    ���û�õ�ussd������
* Params    
* Return    
* Remarks   
**********************************************************************/
static  BOOL    SetUssdInfo(HWND hWnd)
{
    USSD_INFO   ussd_info;
    char      * pUssdString;

    memset(&ussd_info,0x00,sizeof(USSD_INFO));
    if (-1 == ME_GetUSSDInfo(&ussd_info))
        return FALSE;

    if (ussd_info.DataLen == 0)
        return FALSE;

    pUssdString = (char *)malloc(ussd_info.DataLen + 1);
    if (NULL == pUssdString)
        return FALSE;

    memset(pUssdString,0x00,ussd_info.DataLen + 1);

    if (CBS_ALPHA_UCS2 == ussd_info.Code)
    {
        WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)ussd_info.Data,
            ussd_info.DataLen / 2, (LPSTR)pUssdString, ussd_info.DataLen + 1, 0, 0);
    }
    else
    {
        memcpy(pUssdString,ussd_info.Data,ussd_info.DataLen);
    }
    SetWindowTextExt(GetDlgItem(hWnd,IDC_GSMEDIT),pUssdString);//hGSMEdit
    free(pUssdString);
    ShowWindow(hWnd,SW_SHOW);
    UpdateWindow(hWnd);

    return TRUE;
}
//////////////////////////////////////////////////////////////////////////
/********************************************************************
* Function   GSM_PhoneBookDealWith  
* Purpose    �������ּ�+#������ϵ�˼�¼�Ĺ���
* Params    cPhoneNumber:����,
* Return    TRUE:����GSM����,FALSE:������GSM����
* Remarks   GSM�淶�й涨�����ֵ�λ����1-3λ,���һλ������#
**********************************************************************/
BOOL    GSM_PhoneBookDealWith(const char * pPhoneNumber)
{
    int iLen,iIndex,iFunc;
    char cString[GSM_PHONEBOOKMAX] = "";
    HWND hEdit;

    if (NULL == pPhoneNumber)
        return FALSE;

    iLen = strlen(pPhoneNumber);
    if ((iLen < 2) || (iLen > GSM_PHONEBOOKMAX))
        return FALSE;

    if (pPhoneNumber[iLen - 1] != Ch_well)//����������#
        return FALSE;

    strncpy(cString,pPhoneNumber,iLen - 1);

    if (!StrIsNumber(cString))//��ǰ�ַ�������ת��Ϊ����
        return FALSE;

    iIndex = atoi(cString);
    if (iIndex > iPhoneBookSIMMaxLen)//sim����ϵ�˸��������ֵ
        return FALSE;

    if (hWndGSM == NULL)
        CreateMBGSMWindow();

    memset(&phonebook,0x00,sizeof(PHONEBOOK));
    iFunc = MB_GetSIMRcdByOder(iIndex,&phonebook);
    if (iFunc == -1)//û���������
        return FALSE;
    if (iFunc == 0)//û��������¼
    {
        AppMessageBox(NULL,WITHOUTDATA,TITLECAPTION,WAITTIMEOUT);
        return TRUE;
    }
    if (strlen(phonebook.PhoneNum) == 0)
    {
        AppMessageBox(NULL,INVALIDATENUM,TITLECAPTION,WAITTIMEOUT);
        return TRUE;
    }
    if (!IfValidPhoneNumber(phonebook.PhoneNum))
    {
        AppMessageBox(NULL,INVALIDATENUM,TITLECAPTION,WAITTIMEOUT);
        return TRUE;
    }
    strcpy(pParam1,phonebook.Text);
    strcat(pParam1,pNewLine);
    strcat(pParam1,phonebook.PhoneNum);

    hEdit = GetDlgItem(hWndGSM,IDC_GSMEDIT);
    SetWindowTextExt(hEdit,pParam1);
    SendMessage(hWndGSM,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_PHONE_LEFTBUTTON,1),BUTTONOK);
    SendMessage(hWndGSM,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_PHONE_RIGHTBUTTON,BUTTONQUIT);
    ShowWindow(hWndGSM,SW_SHOW);
    UpdateWindow(hWndGSM);

    SetFocus(hEdit);

    return TRUE;
}
