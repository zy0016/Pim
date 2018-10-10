/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 打接电话部分
 *
 * Purpose  : 进行GSM操作的文件
 *            
\**************************************************************************/
#include    "MB_GSM.h"

#define DEBUG_MB_GSM

//呼叫限制操作错误代码
#if defined(SIEMENS_TC35)
#define GSM_CALLBAR_NONESERVICE     133 //未开通服务
#define GSM_CALLBAR_INVALIDCODE     16  //呼叫限制码错误
#define GSM_CALLBAR_INVALIDSTRING   260 //新呼叫限制码和确认码不一致
#endif
#if defined(WAVECOM_2C)
#define GSM_CALLBAR_NONESERVICE     30  //未开通服务
#define GSM_CALLBAR_INVALIDCODE     16  //呼叫限制码错误
#define GSM_CALLBAR_INVALIDSTRING   3   //新呼叫限制码和确认码不一致
#endif
#if defined(TI_MODULE)
#define GSM_CALLBAR_NONESERVICE     30  //未开通服务
#define GSM_CALLBAR_INVALIDCODE     16  //呼叫限制码错误
#define GSM_CALLBAR_INVALIDSTRING   3   //新呼叫限制码和确认码不一致
#endif
static HINSTANCE    hInstance;
static GSMTYPE      GSMType;//GSM号码类型
static int          iGSMBSType = -1;//BS参数类型

static BOOL         bGSMAutoClose;//是否自动关闭窗口
static HWND         hWndGSM = NULL;
static BOOL         bCancelOperate = FALSE;//当前操作是否已经被取消

static char       * pSucc;//成功提示语(iSuccMaxLen 256个字符)
static char       * pFail;//失败提示语(iFailMaxLen 256个字符)
static char       * pParam1;//用于保存一些参数(iParamMaxLen 100个字符)
static char       * pParam2;//用于保存一些参数(iParamMaxLen 100个字符)
static char       * pParam3;//用于保存一些参数(iParamMaxLen 100个字符)
static char       * pParam4;//用于保存一些参数(iParamMaxLen 100个字符)

static int          iCallForward;//对呼叫转移的操作
static RECT         hEditRect;

static const char * pImei           = "IMEI:%s";
static const char * pClassName      = "MBGSMOperateWndClass";//窗口类名
static const char * pInputControl   = "EDIT";
static const char * pAsterisk       = "*";
static const char * pWell           = "#";
static const char * pNewLine        = "\n";
static const char * pColon          = ":";
static const char   Ch_Asterisk     = '*';
static const char   Ch_well         = '#';
static const char   Ch_End          = '\0';
static const char   Ch_Num1         = '1';

static const int    iGSM            = 1;//号码符合GSM标准的话的返回值
static const int    iUnGSM          = 0;//号码不符合GSM标准的话的返回值
static const int    iGSM_Error      = -1;//AT命令发送失败返回-1
static const int    iGSM_Param      = -2;//GSM参数不合适
static const int    iUnBSParam      = -1;//表示没有BS参数
static const int    iClewMaxLen     = 2048;//提示语的最大长度
static const int    iSuccMaxLen     = 256;//成功提示语的最大长度
static const int    iFailMaxLen     = 256;//失败提示语的最大长度
static const int    iParamMaxLen    = 100;//pParam的最大长度

static const enum
{
    GSM_CF_ACTIVATE,                //激活呼叫转移
    GSM_CF_ACTIVATELOGIN,           //激活并注册呼叫转移
    GSM_CF_DEACTIVATE,              //禁止呼叫转移
    GSM_CF_DEACTIVATELOGIN          //禁止并反注册呼叫转移
};
static const enum
{
    GSM_CB_CHECK,                   //查询呼叫限制
    GSM_CB_CHECKBS                  //查询呼叫限制(支持BS参数)
};
static const char * pGSMCode_T_SC[] = //呼叫转移包含参数T的时候,与T对应的SC参数
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
static const char * pBSContent[] =//BS参数的解释(用于设置和查询失败的时候显示)
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
static const char * pBSContentSearch[] =//BS参数的解释(用于查询成功的时候显示)
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
    "*#06#",    //查IMEI号
    ""
};
static const char * pSecurity[] =
{
    "**04*",    //修改pin码
    "**042*",   //修改pin2码
    "**05*",    //pin1码解锁
    "**052*",   //pin2码解锁
    ""
};
static const char * pCallBarring[] =
{
    "**03*330*",//修改呼叫限制密码
    "**03**",   //修改呼叫限制密码
    "*03*330*", //修改呼叫限制密码
    "*03**",    //修改呼叫限制密码
    ""
};
static const char * pUnlockPS[] =
{
    "*#0003*",   //手机模块密码解锁
    ""
};
//////////////////////////////////////////////////////////////////////////
static const char * pForwardingCheck[] = //呼叫转移的查询操作(支持BS参数)
{
    "*#002",   //all call forwarding/所有转移
    "*#004",   //all conditional call forwarding/所有条件转移
    "*#21",    //call forwarding unconditional/无条件转移
    "*#61",    //call forwarding on no answer/无应答转移
    "*#62",    //call forwarding on not reachable/不在服务区或关机转移
    "*#67",    //call busy/遇忙转移
    ""
};
static const char * pForwardingActive[] = //呼叫转移的激活操作(支持BS参数)
{
    "*002",    //all call forwarding/所有转移
    "*004",    //all conditional call forwarding/所有条件转移
    "*21",     //call forwarding unconditional/无条件转移
    "*61",     //call forwarding on no answer/无应答转移
    "*62",     //call forwarding on not reachable/不在服务区或关机转移
    "*67",     //call busy/遇忙转移
    ""
};
static const char * pForwardingActiveLogin[] =//呼叫转移的注册激活操作(支持BS参数和T参数)
{
    "**002",   //all call forwarding/所有转移
    "**004",   //all conditional call forwarding/所有条件转移
    "**21",    //call forwarding unconditional/无条件转移
    "**61",    //call forwarding on no answer/无应答转移
    "**62",    //call forwarding on not reachable/不在服务区或关机转移
    "**67",    //call busy/遇忙转移
    ""
};
static const char * pForwardingDeactive[] = //呼叫转移的关闭操作(支持BS参数)
{
    "#002",    //all call forwarding/所有转移
    "#004",    //all conditional call forwarding/所有条件转移
    "#21",     //call forwarding unconditional/无条件转移
    "#61",     //call forwarding on no answer/无应答转移
    "#62",     //call forwarding on not reachable/不在服务区或关机转移
    "#67",     //call busy/遇忙转移
    ""
};
static const char * pForwardingDeactiveUnlogin[] = //呼叫转移的关闭反注册操作(支持BS参数)
{
    "##002",   //all call forwarding/所有转移
    "##004",   //all conditional call forwarding/所有条件转移
    "##21",    //call forwarding unconditional/无条件转移
    "##61",    //call forwarding on no answer/无应答转移
    "##62",    //call forwarding on not reachable/不在服务区或关机转移
    "##67",    //call busy/遇忙转移
    ""
};
//////////////////////////////////////////////////////////////////////////
static const char * pCallBarringActive[] = //呼叫限制的激活操作(支持BS参数)
{
    "*33*",     //call barring of outgoing call/限制所有拨出电话
    "*331*",    //call barring of outgoing international call/限制所有拨出国际长途
    "*332*",    //call barring of outgoing international calls except to HPLMN/限制所有拨出国际长途,除了本国
    "*35*",     //call barring of incoming calls/限制所有拨入电话
    "*351*",    //call barring of incoming calls if roaming/限制国际漫游时来电
    ""
};
static const char * pCallBarringDeactive[] = //呼叫限制的禁止操作(支持BS参数)
{
    "#33*",     //call barring of outgoing call/限制所有拨出电话
    "#331*",    //call barring of outgoing international call/限制所有拨出国际长途
    "#332*",    //call barring of outgoing international calls except to HPLMN/限制所有拨出国际长途,除了本国
    "#35*",     //call barring of incoming calls/限制所有拨入电话
    "#351*",    //call barring of incoming calls if roaming/限制国际漫游时来电
    "#330*",    //all barring service (only for deactivation)/解除所有通话限制
    "#333*",    //all outgoing barring service (only for deactivation)/解除所有拨出限制
    "#353*",    //all incoming barring service (only for deactivation)/解除所有拨入限制
    ""
};
static const char * pCallBarringCheck[] = //呼叫限制的查询操作(不支持BS参数)
{
    "*#33#",    //call barring of outgoing call/限制所有拨出电话
    "*#331#",   //call barring of outgoing international call/限制所有拨出国际长途
    "*#332#",   //call barring of outgoing international calls except to HPLMN/限制所有拨出国际长途,除了本国
    "*#35#",    //call barring of incoming calls/限制所有拨入电话
    "*#351#",   //call barring of incoming calls if roaming/限制国际漫游时来电
    ""
};
static const char * pCallBarringCheckBS[] = //呼叫限制的查询操作(支持BS参数)
{
    "*#33**",    //call barring of outgoing call/限制所有拨出电话
    "*#331**",   //call barring of outgoing international call/限制所有拨出国际长途
    "*#332**",   //call barring of outgoing international calls except to HPLMN/限制所有拨出国际长途,除了本国
    "*#35**",    //call barring of incoming calls/限制所有拨入电话
    "*#351**",   //call barring of incoming calls if roaming/限制国际漫游时来电
    ""
};
//////////////////////////////////////////////////////////////////////////
static const char * pCallWaiting[] = //呼叫等待的操作(支持BS参数)
{
    "*43",     //激活呼叫等待
    "*#43",    //查询呼叫等待
    "#43",     //禁止呼叫等待
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
static const char * pGSMSpecific[] =//GSM特殊号码
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
{   //前缀表地址                函数地址              附加参数               执行方式,是否进行长度匹配
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
///ussd部分///////////////////////////////////////////////////////////////////////
static void * pUssd = NULL;
#define USSDREPLYMAXLEN     40//ussd回复的时候可以输入的最大长度
///选择数字键+#的话,查询联系人部分///////////////////////////////////////////////////////////////////////
static int          iPhoneBookSIMMaxLen = 100;//sim卡联系人个数的最大值
static PHONEBOOK    phonebook;
#define GSM_PHONEBOOKMAX    4//处理数字键+#调用联系人记录功能的时候,GSM规范中规定的序列最大长度
/*上方的指针数组保留的是某一类GSM操作需要识别的序列
处理思路:
结构数组GsmOperate中pGSMPre项保留某个GSM操作序列的地址,就是上方指针数组的地址
GsmOperate中pFunction项保存每个GSM操作序列对应的处理函数
wParam项保存执行这个处理函数的时候所需要的附加参数
GSM识别的时候首先取出结构数组中的一项,获得这一项的指针数组的地址,
让当前电话号码在指针数组中进行查询,当电话号码的前缀等于指针数组中某一项的时候
进入该指针数组对应的处理函数中,进行进一步的扫描,否则考查指针数组中的下一项.
当这个指针数组中的每一项都不等于电话号码的前缀时,取出结构数组中的下一项进行扫描

当电话号码符合GSM操作的时候,GSM操作函数会发送AT命令,
该命令发送成功返回iGSM,at命令发送失败返回iGSM_Error,
当电话号码前缀符合GSM操作但是后边的部分不符合GSM操作的时候也就是说虽然电话号码的前缀
符合某个GSM操作序列,但是整个号码并不符合这个需要执行的GSM操作,返回iUnGSM

整个GSMDealWith函数返回FALSE表示这个号码可能不是一个合法的GSM序列,可能用户需要把它
当作一般号码发送出去

当GSM函数返回iGSM的话表示将某个GSM操作执行完毕.AT命令已经发出,需要显示一个提示框表示
正在进行GSM操作,整个GSMDealWith函数返回TRUE表示这个号码符合某个GSM操作序列*/
/********************************************************************
* Function   CallMBGSMWindow  
* Purpose    创建一个窗口用于GSM操作
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
            printf("mb MB_GSM malloc失败\r\n");
#endif
            return FALSE;
        }
        memset(pParam1,0x00,iParamMaxLen);
        memset(pParam2,0x00,iParamMaxLen);
        memset(pParam3,0x00,iParamMaxLen);
        memset(pParam4,0x00,iParamMaxLen);
        memset(pSucc  ,0x00,iSuccMaxLen );
        memset(pFail  ,0x00,iFailMaxLen );
        //***********ussd初始化******************
        ME_USSD_SetNotify(hWnd,IDC_USSD_NOTIFY);
        //***********联系人信息初始化***************************
        ME_GetPhonebookMemStatus(hWnd,IDC_ME_PHONE_SIM_AMX,PBOOK_SM);
        break;

    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd,IDC_GSMEDIT));//hGSMEdit
        break;

    case WM_ACTIVATE://去掉这个消息，单击右键可以退出，否则退出失败
        if (WA_INACTIVE != LOWORD(wParam))
            SetFocus(GetDlgItem(hWnd,IDC_GSMEDIT));//激活后设置焦点

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
        case IDC_BUTTON_SET://确定
        case IDC_BUTTON_QUIT://退出
            iGSMBSType = iUnBSParam;
            ShowWindow(hWnd,SW_HIDE);
            break;
        //**********************************************************
        case IDC_USSD_LEFTBUTTON://用于USSD功能的左键
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
        case IDC_USSD_RIGHTBUTTON://用于USSD功能的右键
            ME_USSD_Abort(pUssd);
            break;
        //**********************************************************
        case IDC_PHONE_LEFTBUTTON://用于查询联系人功能的左键,用于拨出号码
            APP_CallPhoneNumber(phonebook.PhoneNum);
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
            break;
        case IDC_PHONE_RIGHTBUTTON://用于查询联系人功能的右键
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
            break;
        }
        break;
    //*******************回调消息******************************
    case IDC_GSMOPERATE://GSM操作的回调消息
        if (bCancelOperate)//当前操作已经被取消
            break;

        MBWaitWindowState(hWnd,FALSE,NULL,FALSE);
        switch (wParam)
        {
        case ME_RS_SUCCESS://GSM操作成功
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
            printf("mb MB_GSM.c 收到GSM操作成功的消息,GSMType:%d\r\n",GSMType);
#endif
            if (GSMClewText_Succ(hWnd))
                memset(pFail,0x00,iFailMaxLen);
            else
                SetWindowTextExt(GetDlgItem(hWnd,IDC_GSMEDIT),pFail);//hGSMEdit

            break;
        default://GSM操作失败
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
            printf("mb MB_GSM.c 收到GSM操作失败的消息,wParam:%ld,lParam:%ld,GSMType:%d\r\n",
                wParam,lParam,GSMType);
#endif
            GSMClewText_Fail(hWnd,wParam,lParam);
            break;
        }
        break;

    case IDC_USSD_NOTIFY://ussd注册消息
        if (bCancelOperate)//当前操作已经被取消
            break;

        if (!USSDDealRespose(hWnd,wParam,lParam))
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);

        break;

    case IDC_ME_PHONE_SIM_AMX://查询SIM卡中联系人的最大个数
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

    case IDC_CANCELOPERATE://标志取消操作的回调消息
        bCancelOperate = TRUE;//当前操作已经被取消
        SetWindowTextExt(GetDlgItem(hWnd,IDC_GSMEDIT),GSMOPERATECANCEL);//操作取消
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
* Purpose    用于GSM提示(消息返回失败)
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
    case GSM_CALLFORWARDING1:       //所有呼叫转移
    case GSM_CALLFORWARDING2:       //所有条件转移
    case GSM_CALLFORWARDING3:       //无条件转移
    case GSM_CALLFORWARDING4:       //无应答转移
    case GSM_CALLFORWARDING5:       //不在服务区或关机转移
    case GSM_CALLFORWARDING6:       //遇忙转移
    case GSM_CALLFORWARDING1_CHECK: //查询所有转移
    case GSM_CALLFORWARDING2_CHECK: //查询所有条件转移
    case GSM_CALLFORWARDING3_CHECK: //查询无条件转移
    case GSM_CALLFORWARDING4_CHECK: //查询无应答转移
    case GSM_CALLFORWARDING5_CHECK: //查询不在服务区或关机转移
    case GSM_CALLFORWARDING6_CHECK: //查询遇忙转移
    case GSM_CALLBARRING33:         //限制所有拨出电话
    case GSM_CALLBARRING331:        //限制所有拨出国际长途,
    case GSM_CALLBARRING332:        //限制所有拨出国际长途(除本国)
    case GSM_CALLBARRING35:         //限制所有拨入电话
    case GSM_CALLBARRING351:        //限制国际漫游时来电
    case GSM_CALLBARRING330:        //解除所有通话限制
    case GSM_CALLBARRING333:        //解除所有拨出限制
    case GSM_CALLBARRING353:        //解除所有拨入限制
    case GSM_CALLBARRING33_CHECK:   //33 限制所有拨出电话
    case GSM_CALLBARRING331_CHECK:  //331 限制所有拨出国际长途
    case GSM_CALLBARRING332_CHECK:  //332 限制所有拨出国际长途,除了本国
    case GSM_CALLBARRING35_CHECK:   //35 限制所有拨入电话
    case GSM_CALLBARRING351_CHECK:  //351 限制国际漫游时来电
        SetWindowTextExt(GetDlgItem(hWnd,IDC_GSMEDIT),pFail);//hGSMEdit
        return TRUE;
    case GSM_CALLBAR://呼叫限制码
        switch (lParam)
        {
        case GSM_CALLBAR_NONESERVICE://133://未开通服务
            StrAutoNewLine(NULL,GSMBARRING_ERROR1,pFail,&hEditRect,NULL);
            break;
        case GSM_CALLBAR_INVALIDSTRING://260://新呼叫限制码和确认码不一致
            StrAutoNewLine(NULL,GSMBARRING_ERROR2,pFail,&hEditRect,NULL);
            break;
        case GSM_CALLBAR_INVALIDCODE://16://呼叫限制码错误
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
* Purpose    用于GSM提示(消息返回成功)
* Params    
* Return    
* Remarks   
**********************************************************************/
static  BOOL    GSMClewText_Succ(HWND hWnd)
{
    HWND hEdit;

    hEdit = GetDlgItem(hWnd,IDC_GSMEDIT);
    if (GSM_IMEI == GSMType)//查IMEI号
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
    else if (GSM_CALLWAIT_CHECK == GSMType)//查询呼叫等待
    {
        CWINFO * pCWInfo;
        int iBS,iCount,i;
        int iLength;
        char * pClew = NULL;

        iLength = ME_GetResult(NULL,NULL);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
        printf("mb MB_GSM.c,查询呼叫等待,iLength:%lu\r\n",iLength);
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
        printf("mb MB_GSM.c,查询呼叫等待,ME_GetResult=%lu\r\n",iLength);
#endif
        iCount = iLength / sizeof(CWINFO);
        for (i = 0;i < iCount;i++)
        {
            iBS = InIntArray(pCWInfo[i].BS,iBSME,sizeof(iBSME));
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
            printf("mb MB_GSM.c 查询呼叫等待 pCWInfo[%d].BS:%d,iBS:%d\r\n",i,pCWInfo[i].BS,iBS);
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
        (GSMType < GSM_CALLFORWARDING_CHECKEND))//检查呼叫转移的状态
    {
        CFINFO * pCFInfo;
        int iLength;
        int iCount,i,iBS;
        char * pClew = NULL;

        iLength = ME_GetResult(NULL,NULL);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
        printf("mb MB_GSM.c,检查呼叫转移,iLength:%lu\r\n",iLength);
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
        printf("mb MB_GSM.c,查询呼叫转移,ME_GetResult=%lu\r\n",iLength);
#endif
        iCount = iLength / sizeof(CFINFO);
        for (i = 0;i < iCount;i++)
        {
            iBS = InIntArray(pCFInfo[i].BS,iBSME,sizeof(iBSME));
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
            printf("mb MB_GSM.c 查询呼叫转移 pCFInfo[%d].BS:%d,iBS:%d,Reason:%d,号码:%s,状态:%d\r\n",
                i,pCFInfo[i].BS,iBS,pCFInfo[i].Reason,pCFInfo[i].DN,pCFInfo[i].Active);
#endif
            switch (pCFInfo[i].Reason)
            {
            case CFU://无条件转移
                StrAutoNewLine(NULL,GSMCALLFORWARDING3,pParam1,&hEditRect,NULL);
                break;
            case CFB:// 遇忙转移
                StrAutoNewLine(NULL,GSMCALLFORWARDING6,pParam1,&hEditRect,NULL);
                break;
            case CFNRy://无应答转移
                StrAutoNewLine(NULL,GSMCALLFORWARDING4,pParam1,&hEditRect,NULL);
                break;
            case CFNRc://不可达转移
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
        (GSMType < GSM_CALLBARRING_CHECKEND))//查询呼叫限制的状态
    {
        CBINFO * pCBInfo;
        int iLength;
        int iCount,i,iBS;
        char * pClew = NULL;

        iLength = ME_GetResult(NULL,NULL);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
        printf("mb MB_GSM.c,查询呼叫限制,iLength:%lu\r\n",iLength);
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
        printf("mb MB_GSM.c,查询呼叫限制,ME_GetResult=%lu\r\n",iLength);
#endif
        iCount = iLength / sizeof(CBINFO);
        for (i = 0;i < iCount;i++)
        {
            iBS = InIntArray(pCBInfo[i].BS,iBSME,sizeof(iBSME));
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
            printf("mb MB_GSM.c 查询呼叫限制,BS:%d,iBS:%d,Fac:%d,状态:%d\r\n",
                pCBInfo[i].BS,iBS,pCBInfo[i].Fac,pCBInfo[i].Active);
#endif
            switch (pCBInfo[i].Fac)
            {
            case BAOC://限制所有呼出
                StrAutoNewLine(NULL,GSMCALLBARRING33,pParam1,&hEditRect,NULL);
                break;
            case BOIC://限制所有国际呼出
                StrAutoNewLine(NULL,GSMCALLBARRING331,pParam1,&hEditRect,NULL);
                break;
            case BOIC_exHC://限制所有国际呼出除本国
                StrAutoNewLine(NULL,GSMCALLBARRING332,pParam1,&hEditRect,NULL);
                break;
            case BAIC://限制所有呼入
                StrAutoNewLine(NULL,GSMCALLBARRING35,pParam1,&hEditRect,NULL);
                break;
            case BIC_Roam://漫游时限制所有呼入
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
        case CLIPNONE:// 未提供CLIP服务
            sprintf(pParam1,GSMCLIP,pColon,GSMCLIP_NONE);
            break;
        case CLIPALLOW:// 提供CLIP服务
            sprintf(pParam1,GSMCLIP,pColon,GSMCLIP_ALLOW);
            break;
        case CLIPUNKNOWN:// 结果未知
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
        case CLIRNONE://未提供CLIR服务
            sprintf(pParam1,GSMCLIR,pColon,GSMCLIR1);
            break;
        case CLIRPERMANENT://以永久方式提供CLIR服务
            sprintf(pParam1,GSMCLIR,pColon,GSMCLIR2);
            break;
        case CLIRUNKNOWN://结果未知
            sprintf(pParam1,GSMCLIR,pColon,GSMCLIR3);
            break;
        case CLIRTEMPRES://CLIR临时模式限制
            sprintf(pParam1,GSMCLIR,pColon,GSMCLIR4);
            break;
        case CLIRTEMPALLOW://CLIR临时模式允许
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
        switch (GSMType)//记录呼叫转移的信息
        {
        case GSM_PIN2://修改pin2码
            SetPin2Change();//表示PIN2码已经修改
            break;
        case GSM_CALLFORWARDING1://所有转移
            if (iCallForward == GSM_CF_ACTIVATELOGIN)//激活并注册呼叫转移
                SetForward_All(TRUE);
            else if (iCallForward == GSM_CF_DEACTIVATELOGIN)//禁止并反注册呼叫转移
                SetForward_All(FALSE);
            break;
        case GSM_CALLFORWARDING2://所有条件转移
            if (iCallForward == GSM_CF_ACTIVATELOGIN)//激活并注册呼叫转移
                SetForward_Condition(TRUE);
            else if (iCallForward == GSM_CF_DEACTIVATELOGIN)//禁止并反注册呼叫转移
                SetForward_Condition(FALSE);
            break;
        case GSM_CALLFORWARDING3://无条件转移
            if (iCallForward == GSM_CF_ACTIVATELOGIN)//激活并注册呼叫转移
            {
                SetForward_Uncdtional(TRUE);
            }
            else if (iCallForward == GSM_CF_DEACTIVATELOGIN)//禁止并反注册呼叫转移
            {
                SetForward_Uncdtional(FALSE);
                MessageMBPostMsgCallForward();//重新开始查询有关呼叫转移的信息
            }
            break;
        case GSM_CALLFORWARDING4://无应答转移
            if (iCallForward == GSM_CF_ACTIVATELOGIN)//激活并注册呼叫转移
                SetForward_NoReply(TRUE);
            else if (iCallForward == GSM_CF_DEACTIVATELOGIN)//禁止并反注册呼叫转移
                SetForward_NoReply(FALSE);
            break;
        case GSM_CALLFORWARDING5://不在服务区或关机转移
            if (iCallForward == GSM_CF_ACTIVATELOGIN)//激活并注册呼叫转移
                SetForward_NoReach(TRUE);
            else if (iCallForward == GSM_CF_DEACTIVATELOGIN)//禁止并反注册呼叫转移
                SetForward_NoReach(FALSE);
            break;
        case GSM_CALLFORWARDING6://遇忙转移
            if (iCallForward == GSM_CF_ACTIVATELOGIN)//激活并注册呼叫转移
                SetForward_Busy(TRUE);
            else if (iCallForward == GSM_CF_DEACTIVATELOGIN)//禁止并反注册呼叫转移
                SetForward_Busy(FALSE);
            break;
        }
    }
    return TRUE;
}
/********************************************************************
* Function   OperateGSMWindow  
* Purpose    用于GSM操作
* Params    iGSM_Type:操作类型,bShow:是否在创建窗口的时候显示它,
            bAutoClose:是否在收到回调消息之后自动关闭窗口
* Return    
* Remarks   
**********************************************************************/
static  void    OperateGSMWindow(GSMTYPE iGSM_Type,BOOL bShow,BOOL bAutoClose)
{
    GSMType       = iGSM_Type;
    bGSMAutoClose = bAutoClose;

    if (bShow)
    {
        SetWindowText(hWndGSM,TITLECAPTION);//设置标题
        SendMessage(hWndGSM,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK);
        SendMessage(hWndGSM,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT);

        ShowWindow(hWndGSM,SW_SHOW);
        UpdateWindow(hWndGSM);  
    }
}
//////////////////////////////////////////////////////////////////////////
/********************************************************************
* Function   GSMDealWith  
* Purpose    在GSM列表中查找字符串,执行相应的函数,有函数执行返回TRUE,否则返回FALSE
* Params    cPhoneNumber:号码,
            bExecuteAtOnce:执行方式,TRUE:立即执行,FALSE:选择拨号键之后再执行
* Return    TRUE:进行GSM处理,FALSE:不进行GSM处理
* Remarks   
**********************************************************************/
BOOL    GSMDealWith(const char * cPhoneNumber,BOOL bExecuteAtOnce)
{
    int         i = 0,iPreLength,iFuncRes;
    int         iPrePlace;
    char        ** pGsmPre;
    GSMSTRUCT   GsmStruct;

    if (cPhoneNumber[strlen(cPhoneNumber) - 1] != Ch_well)//结束符不是#
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
            {   //找到了符合GSM前缀序列的一个号码
                if (GsmOperate[i].bGSMLengthMatch)//进行GSM号码的长度匹配
                {
                    if (strcmp(* pGsmPre,cPhoneNumber) != 0)
                        return FALSE;//虽然号码符合GSM前缀,但是后边的部分是多余的
                }
                bCancelOperate          = FALSE;//当前操作尚未被取消
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
                else if (iGSM_Error == iFuncRes)//AT命令发送失败
                {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
                    printf("mb MB_GSM.c AT命令发送失败:%s,GSMType:%d\r\n",
                        cPhoneNumber,GSMType);
#endif
                    GSMClewText_Fail(hWndGSM,0,0);
                    return TRUE;
                }
                else if (iGSM_Param == iFuncRes)//GSM参数不合适
                {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MB_GSM)
                    printf("mb MB_GSM.c GSM参数不合适\r\n");
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
* Purpose    查询IMEI号
* Params    
* Return    号码符合GSM 查询IMEI号标准的话返回1,否则返回0,AT命令发送失败返回-1
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
* Purpose    修改pin/pin2/puk/puk2码
* Params    
* Return    号码符合GSM修改安全密码标准的话返回1,否则返回0,AT命令发送失败返回-1
* Remarks   
**********************************************************************/
static  int     GsmSecurity(GSMSTRUCT * pGsmStruct)
{
    int iPreLength;//获得前缀的长度    
    char * p = NULL;
    const char * pCFClew[4];
    const int iSecurity[] = {LOCK_SC,LOCK_P2,PUK1,PUK2};
    const int iGSMOperate[] = {GSM_PIN1,GSM_PIN2,GSM_PUK1,GSM_PUK2};

    iPreLength  = strlen(pGsmStruct->pGsmPre[pGsmStruct->iStation]);//获得前缀的长度
    p           = pGsmStruct->pPhoneNumber + iPreLength;//跳过GSM的前缀

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
    if (strcmp(pParam2,pParam3) != 0)//新密码不等于确认密码
    {
        AppMessageBox(NULL,GSM_WRONGPASSWORD,TITLECAPTION,WAITTIMEOUT);
        return iGSM_Param;//0
    }

    pCFClew[0] = GSMPIN1MODIFY;//pin1码修改%s
    pCFClew[1] = GSMPIN2MODIFY;//pin2码修改%s
    pCFClew[2] = GSMPUK1MODIFY;//PIN解锁%s
    pCFClew[3] = GSMPUK2MODIFY;//PIN2解锁%s

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
* Purpose   模块密码解锁
* Params    
* Return    号码符合GSM 模块密码解锁的话返回1,否则返回0,AT命令发送失败返回-1
* Remarks   
**********************************************************************/
static  int     GsmUnlockPS(GSMSTRUCT * pGsmStruct)
{
    int iPreLength;//获得前缀的长度
    char cPassword[PHONENUMMAXLEN] = "",* p = NULL;

    iPreLength  = strlen(pGsmStruct->pGsmPre[pGsmStruct->iStation]);//获得前缀的长度
    p           = pGsmStruct->pPhoneNumber + iPreLength;//跳过GSM的前缀

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
* Purpose   激活注册/禁止反注册呼叫转移
* Params    
* Return    号码符合GSM 激活注册/禁止反注册呼叫转移标准的话返回1,否则返回0,AT命令发送失败返回-1
* Remarks   
**********************************************************************/
static  int     GsmForwardingOperate(GSMSTRUCT * pGsmStruct)
{
    int  iPreLength;//获得前缀的长度
    char cSC[10] = "",cClew[30] = "",* p = NULL;
    int  iParam;
    const char * pCFClew[6];
    const int iGSMOperate[] = 
    {
        GSM_CALLFORWARDING1,//所有呼叫转移
        GSM_CALLFORWARDING2,//所有条件转移
        GSM_CALLFORWARDING3,//无条件转移
        GSM_CALLFORWARDING4,//无应答转移
        GSM_CALLFORWARDING5,//不在服务区或关机转移
        GSM_CALLFORWARDING6 //遇忙转移
    };

    memset(pParam1,0x00,iParamMaxLen);
    memset(pParam2,0x00,iParamMaxLen);
    memset(pParam3,0x00,iParamMaxLen);
    memset(pParam4,0x00,iParamMaxLen);

    iPreLength  = strlen(pGsmStruct->pGsmPre[pGsmStruct->iStation]);//获得前缀的长度
    p           = pGsmStruct->pPhoneNumber + iPreLength;//跳过GSM的前缀
    iGSMBSType  = iUnBSParam;
    iParam      = GetGSMParam(p,pParam1,pParam2,pParam3);

    switch (iParam)
    {
    case 0:
        break;
    case 1://号码不能为零
        if (strlen(pParam1) == 0)
            return iUnGSM;

        break;
    case 2://BS参数不能为零
        iGSMBSType = InStrArray(pParam2,pGSMCode_BS);
        if (iGSMBSType == -1)//非法的BS参数
            return iUnGSM;

        break;
    case 3://T参数不能为零(BS可选)
        if (strlen(pParam3) == 0)
            return iUnGSM;

        if (strlen(pParam2) != 0)//有BS参数
        {
            iGSMBSType = InStrArray(pParam2,pGSMCode_BS);
            if (iGSMBSType == -1)//非法的BS参数
                return iUnGSM;
        }
        if (!StrIsNumber(pParam3))//非法的T参数
            return iUnGSM;

        GetGSMSC(pGsmStruct->pPhoneNumber,cSC);
        if (InStrArray(cSC,pGSMCode_T_SC) == -1)//非法的SC参数
            return iUnGSM;

        break;
    default:
        return iUnGSM;
    }
    switch (pGsmStruct->wParam)
    {
    case GSM_CF_ACTIVATE:           //激活呼叫转移
        strcpy(cClew,GSMOPERATEACTIVATE);
        break;
    case GSM_CF_DEACTIVATE:         //禁止呼叫转移
        strcpy(cClew,GSMOPERATEDEACTIVATE);
        break;
    case GSM_CF_ACTIVATELOGIN:      //激活并注册呼叫转移
        strcpy(cClew,GSMOPERATE_RA);
        break;
    case GSM_CF_DEACTIVATELOGIN:    //禁止并反注册呼叫转移
        strcpy(cClew,GSMOPERATE_UD);
        break;
    }
    iCallForward = pGsmStruct->wParam;//保存对呼叫转移的操作

    pCFClew[0] = GSMCALLFORWARDING1;//所有呼叫转移
    pCFClew[1] = GSMCALLFORWARDING2;//所有条件转移
    pCFClew[2] = GSMCALLFORWARDING3;//无条件转移
    pCFClew[3] = GSMCALLFORWARDING4;//无应答转移
    pCFClew[4] = GSMCALLFORWARDING5;//不在服务区或关机转移
    pCFClew[5] = GSMCALLFORWARDING6;//遇忙转移

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
* Purpose   检查呼叫转移的状态
* Params    
* Return    号码符合GSM 激活呼叫转移标准的话返回1,否则返回0,AT命令发送失败返回-1
* Remarks   
**********************************************************************/
static  int     GsmForwardingCheck(GSMSTRUCT * pGsmStruct)
{
    int     iPreLength;//获得前缀的长度
    char  * p = NULL,cSC[10] = "";
    int     iParam;
    const char *  pCFClew[6];
    const int iGSMOperate[] = 
    {
        GSM_CALLFORWARDING1_CHECK,//查询所有转移
        GSM_CALLFORWARDING2_CHECK,//查询所有条件转移
        GSM_CALLFORWARDING3_CHECK,//查询无条件转移
        GSM_CALLFORWARDING4_CHECK,//查询无应答转移
        GSM_CALLFORWARDING5_CHECK,//查询不在服务区或关机转移
        GSM_CALLFORWARDING6_CHECK //查询遇忙转移
    };

    memset(pParam1,0x00,iParamMaxLen);
    memset(pParam2,0x00,iParamMaxLen);
    memset(pParam3,0x00,iParamMaxLen);

    iPreLength  = strlen(pGsmStruct->pGsmPre[pGsmStruct->iStation]);//获得前缀的长度
    p           = pGsmStruct->pPhoneNumber + iPreLength;//跳过GSM的前缀
    iGSMBSType  = iUnBSParam;
    iParam      = GetGSMParam(p,pParam1,pParam2,pParam3);

    switch (iParam)
    {
    case 0://没有参数
        break;
    case 2:
        if ((strlen(pParam1) != 0) || (strlen(pParam2) == 0))//有电话号码或者没有BS参数
            return iUnGSM;

        iGSMBSType = InStrArray(pParam2,pGSMCode_BS);
        if (iGSMBSType == -1)//非法的BS参数
            return iUnGSM;

        break;
    case 3:
        if ((strlen(pParam1) != 0) ||   //有电话号码或者没有BS参数
            (strlen(pParam2) == 0) ||   //没有BS参数
            (strlen(pParam3) == 0))     //没有T参数
            return iUnGSM;

        iGSMBSType = InStrArray(pParam2,pGSMCode_BS);
        if (iGSMBSType == -1)//非法的BS参数
            return iUnGSM;

        GetGSMSC(pGsmStruct->pPhoneNumber,cSC);
        if (InStrArray(cSC,pGSMCode_T_SC) == -1)//非法的SC参数
            return iUnGSM;

        break;
    default:
        return iUnGSM;
    }

    pCFClew[0] = GSMCALLFORWARDING1;//所有呼叫转移
    pCFClew[1] = GSMCALLFORWARDING2;//所有条件转移
    pCFClew[2] = GSMCALLFORWARDING3;//无条件转移
    pCFClew[3] = GSMCALLFORWARDING4;//无应答转移
    pCFClew[4] = GSMCALLFORWARDING5;//不在服务区或关机转移
    pCFClew[5] = GSMCALLFORWARDING6;//遇忙转移

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
* Purpose    修改呼叫限制码
* Params    
* Return    号码符合GSM修改安全密码标准的话返回1,否则返回0,AT命令发送失败返回-1
* Remarks   
**********************************************************************/
static  int     GsmModifyCallBarring(GSMSTRUCT * pGsmStruct)
{
    int iPreLength;//获得前缀的长度
    char * p = NULL;
    int iparam;

    memset(pParam1,0x00,iParamMaxLen);
    memset(pParam2,0x00,iParamMaxLen);
    memset(pParam3,0x00,iParamMaxLen);

    iPreLength  = strlen(pGsmStruct->pGsmPre[pGsmStruct->iStation]);//获得前缀的长度
    p           = pGsmStruct->pPhoneNumber + iPreLength;//跳过GSM的前缀
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
* Purpose   激活,禁止呼叫限制
* Params    
* Return    号码符合GSM 激活,禁止呼叫限制标准的话返回1,否则返回0,AT命令发送失败返回-1
* Remarks   
**********************************************************************/
static  int     GsmCallBarringOperate(GSMSTRUCT * pGsmStruct)
{
    int iPreLength;//获得前缀的长度
    char cCallBarring[PHONENUMMAXLEN] = "",cBS[PHONENUMMAXLEN] = "";
    char * p = NULL;
    int iparam;
    const char * pCFClew[8];
    const int iGSMOperate[] = 
    {
        GSM_CALLBARRING33, //限制所有拨出电话
        GSM_CALLBARRING331,//限制所有拨出国际长途,
        GSM_CALLBARRING332,//限制所有拨出国际长途(除本国)
        GSM_CALLBARRING35, //限制所有拨入电话
        GSM_CALLBARRING351,//限制国际漫游时来电
        GSM_CALLBARRING330,//解除所有通话限制
        GSM_CALLBARRING333,//解除所有拨出限制
        GSM_CALLBARRING353 //解除所有拨入限制
    };

    memset(pParam1,0x00,iParamMaxLen);
    memset(pParam2,0x00,iParamMaxLen);
    memset(pParam3,0x00,iParamMaxLen);
    memset(pParam4,0x00,iParamMaxLen);

    iPreLength = strlen(pGsmStruct->pGsmPre[pGsmStruct->iStation]);//获得前缀的长度
    iGSMBSType = iUnBSParam;

    p = pGsmStruct->pPhoneNumber + iPreLength;//跳过GSM的前缀
    iparam = GetGSMParam(p,cCallBarring,cBS,NULL);

    if ((-1 == iparam) || (0 == iparam) || (3 == iparam))
        return iUnGSM;

    if (strlen(cCallBarring) == 0)
        return iUnGSM;

    if (2 == iparam)
    {
        iGSMBSType = InStrArray(cBS,pGSMCode_BS);
        if (iGSMBSType == -1)//非法的BS参数
            return iUnGSM;
    }
    pCFClew[0] = GSMCALLBARRING33; //限制所有拨出电话
    pCFClew[1] = GSMCALLBARRING331;//限制所有拨出国际长途,
    pCFClew[2] = GSMCALLBARRING332;//限制所有拨出国际长途(除本国) 
    pCFClew[3] = GSMCALLBARRING35; //限制所有拨入电话
    pCFClew[4] = GSMCALLBARRING351;//限制国际漫游时来电
    pCFClew[5] = GSMCALLBARRING330;//解除所有通话限制
    pCFClew[6] = GSMCALLBARRING333;//解除所有拨出限制
    pCFClew[7] = GSMCALLBARRING353;//解除所有拨入限制

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
* Purpose   查询呼叫限制的状态
* Params    
* Return    号码符合GSM 呼叫限制状态标准的话返回1,否则返回0,AT命令发送失败返回-1
* Remarks   *#SC#或者是*#SC**BS#
**********************************************************************/
static  int     GsmCallBarringCheck(GSMSTRUCT * pGsmStruct)
{
    int iPreLength;//获得前缀的长度
    char cBS[PHONENUMMAXLEN] = "",* p = NULL;
    int iparam;
    const char * pCFClew[5];
    const int iGSMOperate[] =
    {
        GSM_CALLBARRING33_CHECK,    //33 限制所有拨出电话
        GSM_CALLBARRING331_CHECK,   //331 限制所有拨出国际长途
        GSM_CALLBARRING332_CHECK,   //332 限制所有拨出国际长途,除了本国
        GSM_CALLBARRING35_CHECK,    //35 限制所有拨入电话
        GSM_CALLBARRING351_CHECK    //351 限制国际漫游时来电
    };

    memset(pParam1,0x00,iParamMaxLen);
    memset(pParam2,0x00,iParamMaxLen);
    memset(pParam3,0x00,iParamMaxLen);

    iGSMBSType = iUnBSParam;
    if (pGsmStruct->wParam == GSM_CB_CHECKBS)
    {
        iPreLength  = strlen(pGsmStruct->pGsmPre[pGsmStruct->iStation]);//获得前缀的长度
        p           = pGsmStruct->pPhoneNumber + iPreLength;//跳过GSM的前缀
        iparam      = GetGSMParam(p,cBS,NULL,NULL);

        if (1 != iparam)//现在只可能有一个参数
            return iUnGSM;        

        iGSMBSType = InStrArray(cBS,pGSMCode_BS);
        if (iGSMBSType == -1)//非法的BS参数
            return iUnGSM;
    }
    pCFClew[0] = GSMCALLBARRING33;  //33 限制所有拨出电话
    pCFClew[1] = GSMCALLBARRING331; //331 限制所有拨出国际长途
    pCFClew[2] = GSMCALLBARRING332; //332 限制所有拨出国际长途,除了本国
    pCFClew[3] = GSMCALLBARRING35;  //35 限制所有拨入电话
    pCFClew[4] = GSMCALLBARRING351; //351 限制国际漫游时来电

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
* Purpose    打开,查询,关闭呼叫等待
* Params    
* Return    号码符合GSM 打开,查询,关闭呼叫等待标准的话返回1,否则返回0,AT命令发送失败返回-1
* Remarks   
**********************************************************************/
static  int     GsmCallWaitingOperate(GSMSTRUCT * pGsmStruct)
{
    char cBS[PHONENUMMAXLEN] = "",* p;
    int iPreLength;//获得前缀的长度
    int iparam;

    iPreLength  = strlen(pGsmStruct->pGsmPre[pGsmStruct->iStation]);//获得前缀的长度
    p           = pGsmStruct->pPhoneNumber + iPreLength;//跳过GSM的前缀
    iGSMBSType  = iUnBSParam;
    iparam      = GetGSMParam(p,cBS,NULL,NULL);

    switch (iparam)
    {
    case 0://没有BS参数
        break;
    case 1://有参数,可能是BS参数
        iGSMBSType = InStrArray(cBS,pGSMCode_BS);
        if (-1 == iGSMBSType)
            return iUnGSM;
        break;
    default:
        return iUnGSM;
    }
    switch (pGsmStruct->iStation)
    {
    case 0://打开
        StrAutoNewLine(NULL,GSMCALLWAITING,pParam2,&hEditRect,NULL);
        StrAutoNewLine(NULL,(iUnBSParam == iGSMBSType) ? "" : pBSContent[iGSMBSType],pParam3,&hEditRect,NULL);
        StrAutoNewLine(NULL,GSMOPERATEACTIVATE,pParam4,&hEditRect,NULL);

        StrAutoNewLine(NULL,GSMOPERATESUCC_1,pParam1,&hEditRect,NULL);
        StringCombination(pSucc,pParam1,pParam2,pParam3,pParam4);

        StrAutoNewLine(NULL,GSMOPERATEFAIL_1,pParam1,&hEditRect,NULL);
        StringCombination(pFail,pParam1,pParam2,pParam3,pParam4);

        OperateGSMWindow(GSM_CALLWAIT_ACTIVE,TRUE,FALSE);
        break;
    case 1://查询
        StrAutoNewLine(NULL,GSMOPERATESEARCH_FAIL,pParam1,&hEditRect,NULL);
        StrAutoNewLine(NULL,GSMCALLWAITING,pParam2,&hEditRect,NULL);
        StrAutoNewLine(NULL,(iUnBSParam == iGSMBSType) ? "" : pBSContent[iGSMBSType],pParam3,&hEditRect,NULL);
        StringCombination(pFail,pParam1,pParam2,pParam3,NULL);

        OperateGSMWindow(GSM_CALLWAIT_CHECK,TRUE,FALSE);
        break;
    case 2://关闭
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
* Purpose    Phone number presentation的查询
* Params    
* Return    号码符合GSM 查询CLIP状态标准的话返回1,否则返回0,AT命令发送失败返回-1
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
* Purpose    CLIR的操作
* Params    
* Return    号码符合GSM 查询CLIP状态标准的话返回1,否则返回0,AT命令发送失败返回-1
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
* Purpose   获得SC参数
* Params    pGsmCode需要扫描的字符串,pSC:返回SC参数
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
* Purpose   当前字符串中的GSM参数的个数
* Params    pGsmCode:需要扫描的字符串
            param1:返回参数一
            param2:返回参数二
            param3:返回参数三
            bFirstAsterisk:当第一个字符是型号的时候,是否进行处理
* Return    当前字符串中的GSM参数的个数,-1:表示这不是一个合法的GSM号码
* Remarks   
**********************************************************************/
static  int     GetGSMParam(const char * pGsmCode,char * param1,char * param2,char * param3)
{
    char * p1,* p2,* pEnd,* p3;
    int iparam1,iparam2,iparam3;

    pEnd = strstr(pGsmCode,pWell);//查找结束符"#"
    if (NULL == pEnd)//没找到,表示出错了
        return -1;

    p3 = pEnd;//指向结束符
    p3++;
    if (* p3 != Ch_End)//#号之后还有字符,
        return -1;

    if (* pGsmCode == Ch_Asterisk)//第一个字符是星号(*),表示这个星号是参数的分隔符,跳过
    {
        pGsmCode++;
        if (* pGsmCode == Ch_well)//星号之后就是井号,这不是一个合法的GSM号码
            return -1;
    }
    if (* pGsmCode == Ch_well)//第一个字符是结束符(#)表示没有参数
    {
        if (param1 != NULL)
            strcpy(param1,"");

        if (param2 != NULL)
            strcpy(param2,"");

        if (param3 != NULL)
            strcpy(param3,"");

        return 0;
    }

    p1 = strstr(pGsmCode,pAsterisk);//查找第一个星号
    if (NULL == p1)//没找到星号,表示只有一个参数(12#)
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
    //找到第一个星号(12*)
    iparam1 = abs(pGsmCode - p1);
    if (param1 != NULL)
    {
        if (0 != iparam1)
            strncpy(param1,pGsmCode,iparam1);//获得第一个参数
        else
            strcpy(param1,"");
    }

    p1++;//跳过第一个参数

    p2 = strstr(p1,pAsterisk);//查找第二个星号
    if (NULL == p2)//没找到星号,表示有二个参数(12*23#)
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
    //找到第二个星号(12*23*)
    iparam2 = abs(p2 - p1);
    if (param2 != NULL)
    {
        if (0 != iparam2)
            strncpy(param2,p1,iparam2);//获得第二个参数
        else
            strcpy(param2,"");
    }

    p2++;//跳过第二个参数
    iparam3 = abs(p2 - pEnd);

    if (param3 != NULL)
    {
        if (0 != iparam3)
            strncpy(param3,p2,iparam3);//获得第三个参数
        else
            strcpy(param3,"");
    }
    return 3;
}
/********************************************************************
* Function   StringCombination  
* Purpose   将p1,p2,p3,p4组合成一个字符串
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
* Purpose   设置文本框中的文字
* Params    pString:设置的内容
* Return    
* Remarks   
**********************************************************************/
static  void    SetWindowTextExt(HWND hEdit,const char * pString)
{
    SetWindowText(hEdit,pString ? pString : "");
}
/********************************************************************
* Function   GsmSpecific  
* Purpose   当前号码的前缀是否是一个GSM序列标志
* Params    pNumber:电话号码
* Return    当前号码号码从第几个字符开始
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
    return 0;//当前号码的前缀不是一个GSM序列标志
}
//////////以下为USSD处理//////////////////////////////////////////////
/********************************************************************
* Function   USSDDealWith  
* Purpose    USSD处理
* Params    pString:号码,
            bCalling:是否在通话中
* Return    TRUE:进行USSD处理,FALSE:不进行USSD处理
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

        bCancelOperate = FALSE;//当前操作尚未被取消
        MBWaitWindowState(hWndGSM,TRUE,IDC_CANCELOPERATE,TRUE);
        return TRUE;
    }
    return FALSE;
}
/********************************************************************
* Function   USSDDealRespose  
* Purpose    处理收到的USSD消息
* Params    
* Return    
* Remarks   
**********************************************************************/
static  BOOL    USSDDealRespose(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    MBWaitWindowState(hWnd,FALSE,NULL,FALSE);
    switch (wParam)
    {
    case ME_USSD_ABORT://取消
        ShowWindow(hWnd,SW_HIDE);
        return TRUE;

    case ME_USSD_REFUSE://拒绝,用户只能选择退出
        StrAutoNewLine(NULL,USSD_NOSERVICE,pParam1,&hEditRect,NULL);
        SetWindowTextExt(GetDlgItem(hWnd,IDC_GSMEDIT),pParam1);

        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK);
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT);
        ShowWindow(hWnd,SW_SHOW);
        UpdateWindow(hWnd);
        return TRUE;

    case ME_USSD_NOTIFY://通知
        SetUssdInfo(hWnd);
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_USSD_RIGHTBUTTON,(LPARAM)BUTTONTXTRIGHT);
        return TRUE;

    case ME_USSD_REQUEST://请求
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
* Purpose    设置获得的ussd的文字
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
* Purpose    处理数字键+#调用联系人记录的功能
* Params    cPhoneNumber:号码,
* Return    TRUE:进行GSM处理,FALSE:不进行GSM处理
* Remarks   GSM规范中规定的数字的位数是1-3位,最后一位必须是#
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

    if (pPhoneNumber[iLen - 1] != Ch_well)//结束符不是#
        return FALSE;

    strncpy(cString,pPhoneNumber,iLen - 1);

    if (!StrIsNumber(cString))//当前字符串不能转换为数字
        return FALSE;

    iIndex = atoi(cString);
    if (iIndex > iPhoneBookSIMMaxLen)//sim卡联系人个数的最大值
        return FALSE;

    if (hWndGSM == NULL)
        CreateMBGSMWindow();

    memset(&phonebook,0x00,sizeof(PHONEBOOK));
    iFunc = MB_GetSIMRcdByOder(iIndex,&phonebook);
    if (iFunc == -1)//没有这个功能
        return FALSE;
    if (iFunc == 0)//没有这条记录
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
