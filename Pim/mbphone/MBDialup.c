/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 打接电话部分
 *
 * Purpose  : 拨号界面
 *            
\**************************************************************************/
#include "MBDialup.h"

static const char * pClassName = "MBDialupWndClass";//窗口类名
static HINSTANCE    hInstance;
static char         cPhoneNumber[PHONENUMMAXLEN + 1];//电话号码
static DIALITEMBTN  dialitembtn[MBBTN_NUM]; //每个按键的结构
static HWND         hMBDialUp;              //拨号界面的句柄
static BOOL         bShow;
static BOOL         bSuperAddDial;          //当前界面是否是追加拨号
static BOOL         bAddCanSendNumber;      //当前界面是追加拨号界面的时候是否可以拨号
static BOOL         bSpecial;               //当前界面是否是专用拨号界面

#if defined(MBDIALUPSTYLE_TOUCH)
static BOOL         bPenDown = FALSE;       //当前是否有触摸笔按下
static int          iPicNumHeight;          //数字图片的高度
static int          iPicNumChWidth;         //单个数字区域的宽
static HBITMAP      hPicNumber;             //数字图片的句柄
static HBITMAP      hPicNumBar;             //输入框的句柄
static HBITMAP      hPicBackSpaceUp;        //退格键的句柄(正常)
static HBITMAP      hPicBackSpaceDown;      //退格键的句柄(按下)
static RECT         rPicNumBar;             //输入框的区域
static RECT         rPicBS;                 //退格键的区域
static BtnState     ClearKeyDown = BTN_UP;  //当前是否有退格键按下
static int          iGetPenArea;            //记录触摸笔选择的区域
#elif defined(MBDIALUPSTYLE_KEYBORD)
static BOOL         bKeyDown = FALSE;       //当前是否有按键按下
static BOOL         bClearKeyDown = FALSE;  //当前是否有清除键按下
static HBITMAP      hBmpNumberLarge = NULL; //电话号码的图片
static HBITMAP      hBmpNumberSmall = NULL; //小号图片
static int          iPicNumberHeightLarge;  //号码位图的高度(小号)
static int          iPicNumberHeightSmall;  //号码位图的高度(大号)
static int          iChWidthLarge;          //一个字符的最大宽度(大号)
static int          iChWidthSmall;          //一个字符的最大宽度(小号)
static int          iNumberCountLarge;      //界面上显示号码的最大个数(大号数字)
#endif

#if defined(MBDIAL_HAVEPROJECTMODE)
static const char * pProjectMode        = "2003"EXTENSIONSTRING"2"EXTENSIONSTRING;//工程模式启动号码
#endif

static const DWORD  dMBDialWindowStyle  = WS_BORDER|PWS_STATICBAR|WS_VISIBLE;//,窗口风格WS_CAPTION
static const int    iPhoneNumMaxLen     = PHONENUMMAXLEN;
static const int    iMBButtonNum        = MBBTN_NUM;
static const int    iQuickDialNum       = 9;//快速拨号的个数
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
static const int    iPhoneNumberMaxLen  = 13;//一行显示数字的最大个数
static const int    iBtn_ShowNums       = 12;//可以看到的按键个数
static const int    iNumberPicX         = 10;//一行显示数字的最左边的号码的横坐标

static const char * pPicNumber          = MBPIC_DIALUP_NUMBER;//数字图片
static const char * pPicNumBar          = MBPIC_DIALUP_NUMBAR;//输入数字的区域图片
static const char * pPicBSUp            = MBPIC_DIALUP_BSUP;//退格键的图片(正常)
static const char * pPicBSDown          = MBPIC_DIALUP_BSDOWN;//退格键的图片(按下)
static const char * pPicButtonUp[]      = //键盘按钮(正常状态)
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
static const char * pPicButtonDown[]    = //键盘按钮(按下状态)
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
static const int    iLINENUMBERNUMSMALL = 10;//一行字符个数(小号)
static const int    iLINENUMBERNUMLARGE = 10;//一行字符个数(大号)
static const int    iNUMBERTOPY         = 2;//第一行号码的纵坐标
static const int    iNUMBERTOPX         = 5;//左边第一个号码的横坐标
static const int    iNUMBERLINESTEP     = 3;//每行字符的间隔

static const char * pPICNUMBERLARGE     = MBPIC_DIALUP_NUMBER;//大号数字的图片
static const char * pPICNUMBERSMALL     = MBPIC_DIALUP_NUMBER;//小号数字的图片
#endif

#if defined(KEYSWITCHTYPE_SHORT)
#define SWITCHKEY   KEYASTERICK//切换键星号的键值
static BOOL         bArriveKeySwitch;//选择星号键的时候会启动计时器,这个变量用于标识是否达到计时器设定的时间
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

    case APP_ACTIVE :// 注册此程序的窗口类;       
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
            hMBDialUp = CreateWindow(pClassName, TITLECAPTION,//窗口  
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
    case AS_APPWND://返回创建窗口句柄，没有值返回空。
        dwRet = (DWORD)hMBDialUp;
        break;
    default :
        break;
    }
    return dwRet;
}
/********************************************************************
* Function   APP_CallPhone  
* Purpose   调用拨号界面 
* Params    pPhoneNumber:电话号码
* Return    调用拨号界面是否成功
* Remarks   
**********************************************************************/
BOOL    APP_CallPhone(const char * pPhoneNumber)
{
    if (APP_CallPhoneWindow(pPhoneNumber,FALSE,TRUE))
    {
        bSpecial = FALSE;//当前界面不是专用拨号界面
        return TRUE;
    }
    return FALSE;
}
/********************************************************************
* Function   SuperAddDialNumber  
* Purpose   调用追加拨号界面 
* Params    pPhoneNumber:电话号码,bSuperAdd:当前界面是否是追加拨号界面
            bCanSend:当前界面是追加拨号界面的时候是否可以拨号
* Return    调用追加拨号界面是否成功
* Remarks   
**********************************************************************/
BOOL    SuperAddDialNumber(const char * pPhoneNumber,BOOL bSuperAdd,BOOL bCanSend)
{
    if (APP_CallPhoneWindow(pPhoneNumber,bSuperAdd,bCanSend))
    {
        bSpecial = FALSE;//当前界面不是专用拨号界面
        return TRUE;
    }
    return FALSE;
}
/********************************************************************
* Function   APP_CallPhoneSpecial  
* Purpose   调用专用拨号界面,在这个界面中,选择拨出键号码将会拨出,选择其它键会导致退出界面
* Params    pPhoneNumber:电话号码,dWaitTime:专用拨号界面的存在时间,毫秒值
* Return    调用拨号界面是否成功
* Remarks   
**********************************************************************/
BOOL    APP_CallPhoneSpecial(const char * pPhoneNumber,DWORD dWaitTime)
{
    if (APP_CallPhoneWindow(pPhoneNumber,FALSE,TRUE))
    {
        if (dWaitTime != 0)
            SetTimer(hMBDialUp,IDC_TIMER_SPECIAL,dWaitTime,NULL);

        bSpecial = TRUE;//当前界面是专用拨号界面
        return TRUE;
    }
    return FALSE;
}
/********************************************************************
* Function   APP_CallPhoneNumber  
* Purpose   调用拨号功能并且直接拨出去,不出现拨号界面,而是出现连接界面
* Params    pPhoneNumber:电话号码
* Return    号码是否被发送出去
* Remarks   
**********************************************************************/
BOOL    APP_CallPhoneNumber(const char * pPhoneNumber)
{
    bSuperAddDial = FALSE;//当前界面不是追加拨号界面
    bSpecial      = FALSE;//当前界面不是专用拨号界面

    switch (DealWithNumberBySend(pPhoneNumber))// DealWithNumberBySendLink
    {
    case RESULT_SUCC:       //完成拨号
    case RESULT_GSM:        //该号码进行了GSM处理
    case RESUTL_USSD:       //该号码进行了USSD处理
    case RESUTL_PROJECTMODE://当前号码是工程模式的启动号码
        return TRUE;
    case RESULT_NOINIT:     //初始化尚未完成
    case RESULT_PHONENUM:   //通话路数或者通话个数不合适
    case RESULT_QUICKDIAL:  //快速拨号部分没有相应的记录
    case RESULT_ERRORNUMBER://号码非法(号码长度为零)
    case RESULT_NONESIM:    //没有SIM卡的时候,不是特殊号码(112等)
    case RESULT_LINKING:    //当前正在进行拨号连接
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
* Purpose   创建拨号窗口 
* Params    pPhoneNumber:电话号码,bSuperAdd:当前界面是否是追加拨号界面
            bCanSend:当前界面是追加拨号界面的时候是否可以拨号
* Return    调用拨号界面是否成功
* Remarks   
**********************************************************************/
static  BOOL    APP_CallPhoneWindow(const char * pPhoneNumber,BOOL bSuperAdd,BOOL bCanSend)
{
    WNDCLASS    wc;
    int         iSour;

    if (!IfValidPhoneNumber(pPhoneNumber))//考察电话号码是否合法
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

    bSuperAddDial     = bSuperAdd;//当前界面是否是追加拨号
    bAddCanSendNumber = bCanSend; //当前界面是追加拨号界面的时候是否可以拨号

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
            SetFocus(hWnd);//激活后设置焦点
        //lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;

    case WM_CLOSE://点击右上角的退出按钮时为保存退出;
        DestroyWindow(hWnd);
        break;

    case WM_DESTROY ://通知程序管理器,此程序退出;
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
        case IDC_TIMER_SPECIAL://专用拨号界面等待时间的计时器
            DestroyWindow(hWnd);
            break;

        case IDC_TIMER_BACKSPACE://长按退格键的计时器,清空号码
            KillTimer(hWnd,IDC_TIMER_BACKSPACE);
            memset(cPhoneNumber,0x00,sizeof(cPhoneNumber));
#if defined(MBDIALUPSTYLE_TOUCH)
            InvalidateRect(hWnd,&rPicNumBar,TRUE);
#elif defined(MBDIALUPSTYLE_KEYBORD)
            DestroyWindow(hWnd);//退出程序
#endif
            break;

        case IDC_TIMER_LONGKEY://长按*,0键的计时器
            {
                int     iNumberLen;
                char    chCurrent;
                KillTimer(hWnd,IDC_TIMER_LONGKEY);
                iNumberLen = strlen(cPhoneNumber);
                chCurrent  = cPhoneNumber[iNumberLen - 1];//获得最后一个字符

                if (cCh_Asterisk == chCurrent)//最后一个字符是星号,将它改写为p
                    cPhoneNumber[iNumberLen - 1] = cCh_ExtP;
                else if (cCh_Num0 == chCurrent)//最后一个字符是零,将它改写为+
                    cPhoneNumber[iNumberLen - 1] = cCh_Plus;
                else if (cCh_Well == chCurrent)//最后一个字符是井号,将它改写为w
                    cPhoneNumber[iNumberLen - 1] = cCh_ExtW;

#if defined(MBDIALUPSTYLE_TOUCH)
                InvalidateRect(hWnd,&rPicNumBar,TRUE);
#elif defined(MBDIALUPSTYLE_KEYBORD)
                RefreshLastNumberRect(1);
#endif
            }
            break;

        case IDC_TIMER_SHORTKEY://短按星号键的计时器
#if defined(KEYSWITCHTYPE_SHORT)//选择星号键的时候,切换成其它字符的方式:短按键方式
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
        MBDialup_PenUp(hWnd,lParam);//处理触摸笔抬起的情况
        break;

    case WM_PENDOWN:
        MBDialup_PenDown(hWnd,lParam);//处理触摸笔按下的情况
        break;

    case WM_PENMOVE:
        GetOldAndNewRefreshArea(hWnd,lParam,iGetPenArea);//触摸笔移动时候刷新必要的区域
        break;
#endif

#if defined(MBDIALUPSTYLE_KEYBORD)
    case WM_KEYUP:
        switch(LOWORD(wParam))
        {
        case VK_F10://退格
            bClearKeyDown = FALSE;//当前没有清除键按下
            KillTimer(hWnd,IDC_TIMER_BACKSPACE);
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
        case KEYASTERICK: //*
        case KEYWELL: //#
            bKeyDown = FALSE;//当前没有按键按下
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
        case VK_F1://拨号
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SET,0);
            break;
        case VK_F2://退出
            DestroyWindow(hWnd);
            break;
#if defined(MBDIALUPSTYLE_KEYBORD)
        case VK_F10://退格
            MBDialup_BackSpaceKey(hWnd);//拨号界面是用键盘拨号的话,选择退格键的操作
            RefreshLastNumberRect(-1);//刷新字符区域 
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
        case KEYASTERICK: //*
        case KEYWELL: //#
            MBDialup_Keyboard(hWnd,wParam);//拨号界面是用键盘拨号的话,选择按键的操作
            RefreshLastNumberRect(1);//刷新字符区域 
            break;
#endif
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        case IDC_BUTTON_SET://拨打
            DialNumber(hWnd);
            break;

        case IDC_BUTTON_MIDDLE://中键(联系人)
            APP_GetOnePhone(hWnd,cPhoneNumber,sizeof(cPhoneNumber));
            break;

        case IDC_BUTTON_QUIT:
#if defined(MBDIALUPSTYLE_TOUCH)
            DestroyWindow( hWnd );//返回
#elif defined(MBDIALUPSTYLE_KEYBORD)
            MBDialup_BackSpaceButton(hWnd);//退格
            RefreshLastNumberRect(-1);//刷新字符区域 
#endif
            break;
        }
        break;
    //**************************ME回调消息**********************
    case IDC_ME_SUPERADDDIAL://追加拨号的回调消息
        break;

    case IDC_ME_SHORTCUT://快捷操作
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALUP)
        printf("mb MBDialup 快捷操作 wParam:%ld,lParam:%ld\r\n",wParam,lParam);
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
* Purpose   执行拨号操作 
* Params    
* Return    是否开始拨号
* Remarks   返回TRUE将导致拨号窗口关闭,否则拨号窗口不关闭
**********************************************************************/
static  BOOL    DialNumber(HWND hWnd)
{
    if (bSuperAddDial)//当前界面是追加拨号界面且
    {
        if (!bAddCanSendNumber)//当前界面是追加拨号界面的时候不可以拨号
            return FALSE;
    }
    switch (DealWithNumberBySend(cPhoneNumber))
    {
    case RESULT_SUCC:       //完成拨号
    case RESULT_GSM:        //该号码进行了GSM处理
    case RESUTL_USSD:       //该号码进行了USSD处理
    case RESULT_PHONENUM:   //通话路数或者通话个数不合适
    case RESUTL_PROJECTMODE://当前号码是工程模式的启动号码
        DestroyWindow(hWnd);
        return TRUE;
    case RESULT_NOINIT:     //初始化尚未完成
    case RESULT_QUICKDIAL:  //快速拨号部分没有相应的记录
    case RESULT_ERRORNUMBER://号码非法(号码长度为零)
    case RESULT_NONESIM:    //没有SIM卡的时候,不是特殊号码(112等)
    case RESULT_LINKING:    //当前正在进行拨号连接
    case RESULT_END:
        return FALSE;
    }
    DestroyWindow(hWnd);
    return TRUE;
}
/********************************************************************
* Function   DealWithNumberBySendLink  
* Purpose   由APP_CallPhoneNumber函数调用,连接专用接口
* Params    pPhoneNumber:电话号码
* Return    
* Remarks   
**********************************************************************/
static  RESULT_BYSEND   DealWithNumberBySendLink(const char * pPhoneNumber)
{
    SYSTEMTIME  sDialupTime;//记录电话拨出时间

    if (IsWindow_DialLink())//正在连接
        return RESULT_LINKING;

    if (pPhoneNumber == NULL)
        return RESULT_ERRORNUMBER;//号码非法

    if (strlen(pPhoneNumber) == 0)
        return RESULT_ERRORNUMBER;//号码非法

    if (!IfValidPhoneNumber(pPhoneNumber))//考察电话号码是否合法
    {
        AppMessageBox(NULL,INVALIDATENUM,TITLECAPTION,WAITTIMEOUT);
        return RESULT_ERRORNUMBER;
    }

    if (GetUsedItemNum() == MAX_COUNT)//通话个数太多了
    {
        AppMessageBox(NULL,MULTME_DIAL,TITLECAPTION,WAITTIMEOUT);//不能再拨号了
        return RESULT_PHONENUM;//通话路数或者通话个数不合适
    }

    if (FindSIMState(FALSE) == 0)//没有SIM卡的时候进行处理
    {
        if (!NumberIsEmergent(pPhoneNumber,FALSE))//不是特殊号码
        {
            AppMessageBox(NULL,EMERGENT,TITLECAPTION,WAITTIMEOUT);
            return RESULT_NONESIM;//没有SIM卡的时候,不是特殊号码
        }
    }

    SaveMBRecordByPhoneNum(DIALED_REC,pPhoneNumber,&sDialupTime);//加入已拨打电话
    MBRecrodList_Refresh(DIALED_REC);

    CallMBDialLinkingWindow(pPhoneNumber,&sDialupTime);//正在连接的界面
    return RESULT_SUCC;//完成拨号
}
/********************************************************************
* Function   DealWithNumberBySend  
* Purpose   选择拨号键之后执行的操作 
* Params    pPhoneNumber:电话号码
* Return    
* Remarks   
**********************************************************************/
static  RESULT_BYSEND   DealWithNumberBySend(const char * pPhoneNumber)
{
    SYSTEMTIME  sDialupTime;//记录电话拨出时间
    DWORD       wPhoneNumber;
    int         iUsedItemNum,iSour;

    if (IsWindow_DialLink())//正在连接
        return RESULT_LINKING;

    if (pPhoneNumber == NULL)
        return RESULT_ERRORNUMBER;//号码非法

    iSour = strlen(pPhoneNumber);
    if (iSour == 0)
        return RESULT_ERRORNUMBER;//号码非法
    //////////////////////////////////////////////////////////////////////////
    strncpy(cPhoneNumber,pPhoneNumber,sizeof(cPhoneNumber) > iSour ? iSour + 1 : sizeof(cPhoneNumber) - 1);

    iUsedItemNum = GetUsedItemNum();//获得当前通话个数
    if (0 == iUsedItemNum)//还没有通话的时候才支持快速拨号
    {
        if (IsWindow_DialLink() || (IsWindow_MBRing()))//当前正在呼出(接入)一个电话
        {
            if (-1 == ME_VoiceDial(hMBDialUp,IDC_ME_SHORTCUT,(char *)cPhoneNumber,NULL))
            {
                AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);
                return RESULT_ERRORNUMBER;
            }
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALUP)
            printf("mb MBDialup 当前正在呼叫一个电话的时候拨号:%s\r\n",cPhoneNumber);
#endif
            return RESULT_ERRORNUMBER;
        }
        if ((1 == iSour) && StrIsNumber(cPhoneNumber))//当前字符串是否可以完全转化成数字
        {
            wPhoneNumber = atoi(cPhoneNumber);
            if ((1 <= wPhoneNumber) && (wPhoneNumber <= (DWORD)iQuickDialNum))//快速拨号的号码的最大值
            {
                if (0 == MB_GetDirectDial((WORD)wPhoneNumber,cPhoneNumber))//处理单键拨号功能
                {
                    AppMessageBox(NULL,QUICKDIAL_FAIL,TITLECAPTION,WAITTIMEOUT);//请输入号码
                    return RESULT_QUICKDIAL;//快速拨号部分没有相应的记录
                }
            }
        }
    }
    else//通话过程中选择数字键send
    {
        if (strcmp(cPhoneNumber,pCh_Num0) == 0)//呼叫等待时忙音或者挂断保持的通话
        {
            if (!MBCalling_Shortcut(CUT_HELD,NULL))
                AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//请求失败

            return RESULT_GSM;//该号码进行了GSM处理
        }
        if (strcmp(cPhoneNumber,pCh_Num1) == 0)//挂断激活的通话并且直接接入一个等待的电话或者激活一个保持的电话
        {
            if (!MBCalling_Shortcut(CUT_ACTIVED,NULL))
                AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//请求失败

            return RESULT_GSM;
        }
        if ((iSour == 2) && (cPhoneNumber[0] == cCh_Num1))//挂断指定序号的激活通话
        {
            if ((cCh_Num0 <= cPhoneNumber[1]) && (cPhoneNumber[1] <= cCh_Num9))
            {
                if (!MBCalling_Shortcut(CUT_ACTIVED_X,cPhoneNumber))
                    AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//请求失败

                return RESULT_GSM;
            }
        }
        if (strcmp(cPhoneNumber,pCh_Num2) == 0)//在保持和激活的通话状态间切换并且接入其它等待的电话
        {
            if (!MBCalling_Shortcut(ACTIVE_SWITCH,NULL))
                AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//请求失败

            return RESULT_GSM;
        }
        if ((iSour == 2) && (cPhoneNumber[0] == cCh_Num2))//将除指定序号外的所有通话变为保持
        {
            if ((cCh_Num0 <= cPhoneNumber[1]) && (cPhoneNumber[1] <= cCh_Num9))
            {
                if (!MBCalling_Shortcut(ACTIVE_SWITCH_X,cPhoneNumber))
                    AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//请求失败

                return RESULT_GSM;
            }
        }
        if (strcmp(cPhoneNumber,pCh_Num3) == 0)//将保持的通话加入激活通话
        {
            if (!MBCalling_Shortcut(ADD_TO_ACTIVE,NULL))
                AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//请求失败

            return RESULT_GSM;
        }
        if (strcmp(cPhoneNumber,pCh_Num4) == 0)//通话中当已有两路电话时,使这两路电话连起,用户从通话中退出
        {
            if (!MBCalling_Shortcut(CALL_TRANSFER,cPhoneNumber))
                AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//请求失败

            return RESULT_GSM;
        }
        if ((strncmp(cPhoneNumber,pCh_Num4Asterick,strlen(pCh_Num4Asterick)) == 0) && 
            (iSour > (int)strlen(pCh_Num4Asterick)))
        {
            if (!MBCalling_Shortcut(999,cPhoneNumber))
                AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//请求失败

            return RESULT_GSM;
        }
        if (strcmp(cPhoneNumber,pCh_Num5) == 0)
        {
            if (!MBCalling_Shortcut(999,cPhoneNumber))
                AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//请求失败

            return RESULT_GSM;
        }
        if ((IsWindow_DialLink()) || (IsWindow_MBRing()))
        {   //当前正在呼叫一个电话或者正在接听电话
            if (!MBCalling_Shortcut(999,cPhoneNumber))
                AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//请求失败

            return RESULT_GSM;
        }
    }

    if (GSMDealWith(cPhoneNumber,FALSE))//当前号码符合GSM序列,进行了GSM处理
        return RESULT_GSM;//该号码进行了GSM处理

    if (USSDDealWith(cPhoneNumber,iUsedItemNum != 0))//当前号码符合USSD序列,进行了USSD处理
        return RESUTL_USSD;

#if defined(MBDIAL_HAVEPROJECTMODE)
    if (strcmp(cPhoneNumber,pProjectMode) == 0)//当前号码是工程模式的启动号码
    {
        CallProjectATWindow(NULL);
        return RESUTL_PROJECTMODE;
    }
#endif

    if (iUsedItemNum == MAX_COUNT)//通话个数太多了
    {
        AppMessageBox(NULL,MULTME_DIAL,TITLECAPTION,WAITTIMEOUT);//不能再拨号了
        return RESULT_PHONENUM;//通话路数或者通话个数不合适
    }

    if (!IfValidPhoneNumber(cPhoneNumber))//考察电话号码是否合法
    {
        AppMessageBox(NULL,INVALIDATENUM,TITLECAPTION,WAITTIMEOUT);
        return RESULT_ERRORNUMBER;
    }

    if (FindSIMState(FALSE) == 0)//没有SIM卡的时候进行处理
    {
        if (!NumberIsEmergent(cPhoneNumber,FALSE))//不是特殊号码
        {
            AppMessageBox(NULL,EMERGENT,TITLECAPTION,WAITTIMEOUT);
            return RESULT_NONESIM;//没有SIM卡的时候,不是特殊号码
        }
    }

    SaveMBRecordByPhoneNum(DIALED_REC,cPhoneNumber,&sDialupTime);//加入已拨打电话
    MBRecrodList_Refresh(DIALED_REC);

    CallMBDialLinkingWindow(cPhoneNumber,&sDialupTime);//正在连接的界面
    return RESULT_SUCC;//完成拨号
}
/********************************************************************
* Function   DealWithNumber  
* Purpose   根据GSM规则处理号码 
* Params    
* Return    处理了返回TRUE,未找到处理的项目返回FALSE
* Remarks   
**********************************************************************/
static  BOOL    DealWithNumber(HWND hWnd)
{
    if (!bSuperAddDial)//当前不是追加拨号界面(只有开始拨号的时候才查询联系人部分)
    {
        if (GSM_PhoneBookDealWith(cPhoneNumber))
            return TRUE;
    }
    return (GSMDealWith(cPhoneNumber,TRUE));
}
/********************************************************************
* Function   GetDialitemBtnIndex  
* Purpose    返回某个字符(1,2,*,#...)在dialitembtn数组中的位置
* Params    数字字符
* Return     dialitembtn数组中的位置,-1表示查找失败
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
* Purpose    向拨号界面发送消息
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
* Purpose    当前是否存在拨号界面 
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
* Purpose   获得某个键值在dialitembtn数组中的位置
* Params    VK_1..
* Return    dialitembtn数组中的位置,-1表示查找失败
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
* Purpose   刷新字符区域 
* Params    iAdd_Del:导致当前刷新的操作是增加(修改)字符还是删除字符
            iAdd_Del>0增加字符,iAdd_Del<0,删除字符
* Return    
* Remarks   只刷新新添加的字符所在的区域
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
        (iAdd_Del > 0 && iPhoneLen == iNumberCountLarge + 1) || //当前数字的个数超过大号字符显示限制(当前正在增加字符)
        (iAdd_Del < 0 && iPhoneLen == iNumberCountLarge))       //当前数字的个数等于大号字符显示限制(当前正在删除字符)
    {
        InvalidateRect(hMBDialUp,NULL,TRUE);
        return;
    }

    if (iPhoneLen > iNumberCountLarge)//号码个数>界面上显示号码的最大个数(大号)
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
* Purpose    拨号界面是用键盘拨号的话,选择按键的操作 
* Params     
* Return     
* Remarks    
**********************************************************************/
static  void    MBDialup_Keyboard(HWND hWnd,WPARAM wParam)
{
    int  iNumberLen,iChIndex;
    char chCurrent;

    if (bSpecial)//当前界面是专用拨号界面
    {
        DestroyWindow(hWnd);
        return;
    }
    if (bKeyDown)//当前已经有按键按下了,而且尚未抬起
        return;

    bKeyDown    = TRUE;//当前有按键按下
    iNumberLen  = strlen(cPhoneNumber);
    iChIndex    = GetDialitemBtnKey(LOWORD(wParam));//获得某个键值在dialitembtn数组中的位置

    if (iNumberLen < iPhoneNumMaxLen)
    {
#if defined(KEYSWITCHTYPE_LONG)
        //获得某个键值在dialitembtn数组中的位置        
        cPhoneNumber[iNumberLen]     = dialitembtn[iChIndex].ch_short;
        cPhoneNumber[iNumberLen + 1] = cCh_None;

        KillTimer(hWnd,IDC_TIMER_LONGKEY);
        if ((cCh_Asterisk == dialitembtn[iChIndex].ch_short) ||
            (cCh_Num0     == dialitembtn[iChIndex].ch_short) ||
            (cCh_Well     == dialitembtn[iChIndex].ch_short))
            SetTimer(hWnd,IDC_TIMER_LONGKEY,TIMER_LONGKEY,NULL);//启动长按键(*,0)计时器

#elif defined(KEYSWITCHTYPE_SHORT)//选择星号键的时候,切换成其它字符的方式:短按键方式
        if (LOWORD(wParam) == SWITCHKEY)//选择的键是切换键
        {
            KillTimer(hWnd,IDC_TIMER_SHORTKEY);
            SetTimer(hWnd,IDC_TIMER_SHORTKEY,TIMER_SHORTKEY,NULL);//启动短按键计时器

            if (bArriveKeySwitch)//输入新的字符
            {
                bArriveKeySwitch             = FALSE;
                cPhoneNumber[iNumberLen]     = dialitembtn[iChIndex].ch_short;
                cPhoneNumber[iNumberLen + 1] = cCh_None;
            }
            else//尚未到达短按键计时时间,需要显示下一个切换字符
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
        else//选择的键不是切换键,输入下一个字符
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

    if ((bSuperAddDial) &&        //当前界面是追加拨号界面,需要将输入的字符主动发送出去
        (!GetListCurCalling()) && //没有列当前通话
        (!IsAllType_Specific(CALL_STAT_HELD)))//当前不是全部保持
    {
        iNumberLen = strlen(cPhoneNumber);
        chCurrent  = cPhoneNumber[iNumberLen - 1];//获得最后一个字符
        ME_SendDTMF(hWnd,IDC_ME_SUPERADDDIAL,chCurrent);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALUP)
        printf("mb MBDialup 发送追加号码:%c\r\n",chCurrent);
#endif
    }
    if (DealWithNumber(hWnd))//进行GSM操作
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
* Purpose    拨号界面是用键盘拨号的话,选择退格键的操作
* Params     
* Return     
* Remarks    
**********************************************************************/
static  void    MBDialup_BackSpaceKey(HWND hWnd)
{
    int iNumberLen;
    if (bSpecial)//当前界面是专用拨号界面
    {
        DestroyWindow(hWnd);
        return;
    }
    if (bClearKeyDown)//当前有清除键按下
        return;
    bClearKeyDown = TRUE;//当前有清除键按下
    KillTimer(hWnd,IDC_TIMER_BACKSPACE);
    SetTimer(hWnd,IDC_TIMER_BACKSPACE,TIMER_BACKSPACE,NULL);//启动长按键(退格键)计时器

    iNumberLen = strlen(cPhoneNumber);
    if (iNumberLen > 0)
        cPhoneNumber[iNumberLen - 1] = cCh_None;

    if (strlen(cPhoneNumber) == 0)
        DestroyWindow(hWnd);
}
/********************************************************************
* Function   MBDialup_BackSpaceButton  
* Purpose    拨号界面是用键盘拨号的话,选择退格按钮的操作
* Params     
* Return     
* Remarks    
**********************************************************************/
static  void    MBDialup_BackSpaceButton(HWND hWnd)
{
    int iNumberLen;
    if (bSpecial)//当前界面是专用拨号界面
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
* Purpose   根据触摸笔点击的位置,获得这个点在哪个区域 
* Params    pPoint:触摸笔点击/抬起的位置,pRefresh:返回需要刷新的区域
            BtnState:按键显示定义
* Return    返回区域的编号,AREAINFO_BS:选择的区域是退格键,0-11:键盘区域,AREAINFO_NONE:其它区域
* Remarks   
**********************************************************************/
static  int     GetPenAreaInfo(HWND hWnd,POINT Point,BtnState BState)
{
    int i,iResult = AREAINFO_NONE;
    RECT rPicBtn;

    if (BState == BTN_UP)//触摸笔抬起
    {
        bPenDown = FALSE;//没有触摸笔按下
        if (PtInRect(&rPicBS,Point))//抬起区域是退格键
        {
            InvalidateRect(hWnd,&rPicBS,TRUE);
            ClearKeyDown = BTN_UP;
            iResult      = AREAINFO_BS;
        }
        for (i = 0;i < iBtn_ShowNums;i++)
        {
            memcpy(&rPicBtn,&dialitembtn[i].rPicBtn,sizeof(RECT));
            if ((i == 0) || (i == 10) || (i == 11))//0,*,#号键进行区域特殊处理
                rPicBtn.bottom -= 10;

            if (PtInRect(&rPicBtn,Point))//抬起区域是键盘区域
            {
                InvalidateRect(hWnd,&dialitembtn[i].rPicBtn,TRUE);
                dialitembtn[i].iState = BTN_UP;
                iResult               = i;
            }
        }
        if (ClearKeyDown == BTN_DOWN)//如果现在还发现退格键被按下了
        {
            InvalidateRect(hWnd,&rPicBS,TRUE);
            ClearKeyDown = BTN_UP;
        }
        for (i = 0;i < iBtn_ShowNums;i++)
        {
            if (dialitembtn[i].iState == BTN_DOWN)//如果现在还发现键盘有键被按下了
            {
                InvalidateRect(hWnd,&dialitembtn[i].rPicBtn,TRUE);
                dialitembtn[i].iState = BTN_UP;
            }
        }
    }
    else//触摸笔按下
    {
        if (PtInRect(&rPicBS,Point))//点击区域是退格键
        {
            InvalidateRect(hWnd,&rPicBS,TRUE);
            ClearKeyDown = BTN_DOWN;
            iResult      = AREAINFO_BS;

            for (i = 0;i < iBtn_ShowNums;i++)
            {
                if (dialitembtn[i].iState == BTN_DOWN)//如果现在还发现键盘有键被按下了
                {
                    InvalidateRect(hWnd,&dialitembtn[i].rPicBtn,TRUE);
                    dialitembtn[i].iState = BTN_UP;
                }
            }
        }
        for (i = 0;i < iBtn_ShowNums;i++)
        {
            memcpy(&rPicBtn,&dialitembtn[i].rPicBtn,sizeof(RECT));
            if ((i == 0) || (i == 10) || (i == 11))//0,*,#号键进行区域特殊处理
                rPicBtn.bottom -= 10;

            if (PtInRect(&rPicBtn,Point))//按下区域是键盘区域
            {
                InvalidateRect(hWnd,&dialitembtn[i].rPicBtn,TRUE);
                dialitembtn[i].iState = BTN_DOWN;
                iResult               = i;
                if (ClearKeyDown == BTN_DOWN)//如果现在还发现退格键被按下了
                {
                    InvalidateRect(hWnd,&rPicBS,TRUE);
                    ClearKeyDown = BTN_UP;
                }
            }
        }
        if (iResult == AREAINFO_NONE)//点击区域是拨号界面的其它区域,就是不会引起刷新的部分
        {
            if (ClearKeyDown == BTN_DOWN)//如果现在还发现退格键被按下了
            {
                InvalidateRect(hWnd,&rPicBS,TRUE);
                ClearKeyDown = BTN_UP;
            }
            for (i = 0;i < iBtn_ShowNums;i++)
            {
                if (dialitembtn[i].iState == BTN_DOWN)//如果现在还发现键盘有键被按下了
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
* Purpose   输入电话号码的时候,刷新相应的区域
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
    if (iPhoneLen >= iPhoneNumberMaxLen)//号码个数大于一行显示最大个数
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
* Purpose   触摸笔移动时候刷新必要的区域
* Params    lParam:当前触摸笔所在位置,iOldPenArea:原来触摸笔所在区域
* Return    TRUE:触摸笔所在区域已经改变,FALSE:触摸笔所在区域没有改变
* Remarks   
**********************************************************************/
static  BOOL    GetOldAndNewRefreshArea(HWND hWnd,LPARAM lParam,int iOldPenArea)
{
    int iCurPenArea,i;
    POINT Point;

    Point.x     = LOWORD(lParam);
    Point.y     = HIWORD(lParam);
    iCurPenArea = GetPenAreaInfo(hWnd,Point,BTN_DOWN);//获得当前触摸笔所在区域

    if (iCurPenArea == iOldPenArea)//区域没有改变,
        return FALSE;

    if (iOldPenArea == AREAINFO_BS)//触摸笔原来在拨号界面的退格键区域
    {
        ClearKeyDown = BTN_UP;
        bPenDown     = FALSE;
        GetPenAreaInfo(hWnd,Point,BTN_UP);//根据触摸笔点击的位置,获得这个点在哪个区域
        KillTimer(hWnd,IDC_TIMER_BACKSPACE);
        InvalidateRect(hWnd,&rPicBS,TRUE);//刷新退格键区域
    }
    if ((0 <= iOldPenArea) && (iOldPenArea < iBtn_ShowNums))//触摸笔原来在数字键区域,需要刷新数字键区域
    {
        for (i = 0;i < iBtn_ShowNums;i++)
        {
            if (dialitembtn[i].iState == BTN_DOWN)//当前有按键按下
            {
                bPenDown              = FALSE;
                dialitembtn[i].iState = BTN_UP;//标志为抬起状态
                KillTimer(hWnd,IDC_TIMER_LONGKEY);
                InvalidateRect(hWnd,&dialitembtn[i].rPicBtn,TRUE);
            }
        }
    }
    if (iCurPenArea == AREAINFO_NONE)//获得当前触摸笔在拨号界面的其它区域,就是不会引起刷新的部分
    {
        bPenDown = FALSE;
        KillTimer(hWnd,IDC_TIMER_BACKSPACE);
        KillTimer(hWnd,IDC_TIMER_LONGKEY);
        if (ClearKeyDown == BTN_DOWN)//当前还有退格键按下
        {
            ClearKeyDown = BTN_UP;
            InvalidateRect(hWnd,&rPicBS,TRUE);//刷新退格键区域
        }
        for (i = 0;i < iBtn_ShowNums;i++)
        {
            if (dialitembtn[i].iState == BTN_DOWN)//当前有按键按下
            {
                dialitembtn[i].iState = BTN_UP;//标志为抬起状态
                InvalidateRect(hWnd,&dialitembtn[i].rPicBtn,TRUE);
            }
        }
    }
    return FALSE;
}
/********************************************************************
* Function   MBDialup_PenDown  
* Purpose    处理触摸笔按下的情况 
* Params     
* Return     
* Remarks    
**********************************************************************/
static  void    MBDialup_PenDown(HWND hWnd,LPARAM lParam)
{
    POINT   point;
    int     iNumberLen;
    char    chCurrent;

    if (bSpecial)//当前界面是专用拨号界面,直接退出
    {
        memset(cPhoneNumber,0x00,sizeof(cPhoneNumber));
        DestroyWindow(hWnd);
        return;
    }
    if (bPenDown)//当前已经有笔按下
        return;

    bPenDown    = TRUE;//当前有笔按下
    point.x     = LOWORD(lParam);
    point.y     = HIWORD(lParam);
    iGetPenArea = GetPenAreaInfo(hWnd,point,BTN_DOWN);//根据触摸笔点击的位置,获得这个点在哪个区域
    if (AREAINFO_NONE == iGetPenArea)//拨号界面的其它区域,就是不会引起刷新的部分
        return;

    iNumberLen = strlen(cPhoneNumber);
    if (AREAINFO_BS == iGetPenArea)//选择了退格键
    {
        if (iNumberLen > 0)
            cPhoneNumber[iNumberLen - 1] = cCh_None;

        KillTimer(hWnd,IDC_TIMER_BACKSPACE);
        SetTimer(hWnd,IDC_TIMER_BACKSPACE,TIMER_BACKSPACE,NULL);//启动长按键(退格键)计时器
        SetRefreshNumbar(hWnd);//输入电话号码的时候,刷新相应的区域
        return;
    }
    //选择键盘区域
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
                SetTimer(hWnd,IDC_TIMER_LONGKEY,TIMER_LONGKEY,NULL);//启动长按键(数字,*,#)计时器

#elif defined(KEYSWITCHTYPE_SHORT)//选择星号键的时候,切换成其它字符的方式:短按键方式
            if (10 == iGetPenArea)//星号键所在区域的位置
            {
                int iChIndex;
                KillTimer(hWnd,IDC_TIMER_SHORTKEY);
                SetTimer(hWnd,IDC_TIMER_SHORTKEY,TIMER_SHORTKEY,NULL);//启动短按键计时器
                if (bArriveKeySwitch)//输入新的字符
                {
                    bArriveKeySwitch             = FALSE;
                    cPhoneNumber[iNumberLen]     = dialitembtn[iGetPenArea].ch_short;
                    cPhoneNumber[iNumberLen + 1] = cCh_None;
                }
                else//尚未到达短按键计时时间,需要显示下一个切换字符
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
            else//选择的键不是切换键,输入下一个字符
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
        if ((bSuperAddDial) && //当前界面是追加拨号界面,需要将输入的字符主动发送出去
            (!GetListCurCalling()) && //没有列当前通话
            (!IsAllType_Specific(CALL_STAT_HELD)))//当前不是全部保持
        {
            iNumberLen  = strlen(cPhoneNumber);
            chCurrent   = cPhoneNumber[iNumberLen - 1];//获得最后一个字符
            ME_SendDTMF(hWnd,IDC_ME_SUPERADDDIAL,chCurrent);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALUP)
            printf("mb MBDialup 发送追加号码:%c\r\n",chCurrent);
#endif
        }

        if (DealWithNumber(hWnd))//进行GSM操作
        {
            memset(cPhoneNumber,0x00,sizeof(cPhoneNumber));
            DestroyWindow(hWnd);
            return;
        }
        SetRefreshNumbar(hWnd);//输入电话号码的时候,刷新相应的区域
    }
}
/********************************************************************
* Function   MBDialup_PenUp  
* Purpose    处理触摸笔抬起的情况 
* Params     
* Return     
* Remarks    
**********************************************************************/
static  void    MBDialup_PenUp(HWND hWnd,LPARAM lParam)
{
    POINT point;

    bPenDown    = FALSE;//当前没有触摸笔按下
    point.x     = LOWORD(lParam);
    point.y     = HIWORD(lParam);

    iGetPenArea = GetPenAreaInfo(hWnd,point,BTN_UP);//根据触摸笔点击的位置,获得这个点在哪个区域
    if (AREAINFO_NONE == iGetPenArea)//拨号界面的其它区域,就是不会引起刷新的部分
        return;
    if (AREAINFO_BS == iGetPenArea)//选择了退格键
    {
        KillTimer(hWnd,IDC_TIMER_BACKSPACE);
        return;
    }
    //选择键盘区域
    if ((0 <= iGetPenArea) && (iGetPenArea < iBtn_ShowNums))
    {
        KillTimer(hWnd,IDC_TIMER_LONGKEY);
        //if (strlen(cPhoneNumber) < iPhoneNumMaxLen)
          //  SetRefreshNumbar(hWnd);//输入电话号码的时候,刷新相应的区域
    }
}
#endif
/********************************************************************
* Function   ShowPhoneNumber  
* Purpose   显示号码 
* Params    
* Return    
* Remarks   
**********************************************************************/
static  void    ShowPhoneNumber(HDC hdc)
{
    int i,iPhoneLen,index;
    int iNumberX,iNumberY,iDisplayNumber;
#if defined(MBDIALUPSTYLE_TOUCH)
    //显示输入框
    StretchBlt(hdc,
        rPicNumBar.left,rPicNumBar.top,
        rPicNumBar.right - rPicNumBar.left,rPicNumBar.bottom - rPicNumBar.top,
        (HDC)hPicNumBar,
        0,0,rPicNumBar.right - rPicNumBar.left,
        rPicNumBar.bottom - rPicNumBar.top,SRCCOPY);
    //显示退格键
    if (BTN_UP == ClearKeyDown)//没有退格键按下
    {
        StretchBlt(hdc,
            rPicBS.left,rPicBS.top,
            rPicBS.right - rPicBS.left,rPicBS.bottom - rPicBS.top,
            (HDC)hPicBackSpaceUp,
            0,0,rPicBS.right - rPicBS.left,rPicBS.bottom - rPicBS.top,SRCCOPY);
    }
    else//有退格键按下
    {
        StretchBlt(hdc,
            rPicBS.left,rPicBS.top,
            rPicBS.right - rPicBS.left,rPicBS.bottom - rPicBS.top,
            (HDC)hPicBackSpaceDown,
            0,0,rPicBS.right - rPicBS.left,rPicBS.bottom - rPicBS.top,SRCCOPY);
    }
    //显示键盘
    for (i = 0;i < iBtn_ShowNums;i++)
    {
        if (dialitembtn[i].iState == BTN_UP)//该键没有按下
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
    //显示数字
    iPhoneLen = strlen(cPhoneNumber);
    if (iPhoneLen == 0)
        return;
    if (iPhoneLen > iPhoneNumberMaxLen)//只显示后边的号码
        i = iPhoneLen - iPhoneNumberMaxLen;
    else
        i = 0;

    iDisplayNumber = 0;//已经显示的字符个数
    iNumberY       = (rPicNumBar.bottom - rPicNumBar.top - iPicNumHeight) / 2;
    for (;i < iPhoneLen;i++)
    {
        iNumberX = iDisplayNumber * iPicNumChWidth + iNumberPicX;
        index    = GetDialitemBtnIndex(cPhoneNumber[i]);
        StretchBlt(hdc,
            iNumberX,iNumberY,
            dialitembtn[index].rPicNum.right - dialitembtn[index].rPicNum.left,
            dialitembtn[index].rPicNum.bottom - dialitembtn[index].rPicNum.top,//目的矩形
            (HDC)hPicNumber,
            dialitembtn[index].rPicNum.left,dialitembtn[index].rPicNum.top,
            dialitembtn[index].rPicNum.right - dialitembtn[index].rPicNum.left,
            dialitembtn[index].rPicNum.bottom - dialitembtn[index].rPicNum.top,//图象原矩形
            SRCCOPY);//把位图从源矩形拷贝到目的矩形，扩展或压缩位图以使其适合目的矩形的范围

        iDisplayNumber++;
    }
#elif defined(MBDIALUPSTYLE_KEYBORD)
    iPhoneLen      = strlen(cPhoneNumber);//号码个数
    iDisplayNumber = 0;//已经显示的字符个数

    if (iPhoneLen > iNumberCountLarge)//号码个数>界面上显示号码的最大个数(大号)
    {
        for (i = 0;i < iPhoneLen;i++)//需要显示小号字符
        {
            iNumberX = iNUMBERTOPX + (iDisplayNumber % iLINENUMBERNUMSMALL) * iChWidthSmall;
            iNumberY = iNUMBERTOPY + (iDisplayNumber / iLINENUMBERNUMSMALL) * (iPicNumberHeightSmall + iNUMBERLINESTEP);

            index    = GetDialitemBtnIndex(cPhoneNumber[i]);
            StretchBlt( hdc,
                iNumberX,iNumberY,
                dialitembtn[index].rBtnPicSmall.right - dialitembtn[index].rBtnPicSmall.left,
                dialitembtn[index].rBtnPicSmall.bottom - dialitembtn[index].rBtnPicSmall.top,//目的矩形
                (HDC)hBmpNumberSmall,
                dialitembtn[index].rBtnPicSmall.left,dialitembtn[index].rBtnPicSmall.top,
                dialitembtn[index].rBtnPicSmall.right - dialitembtn[index].rBtnPicSmall.left,
                dialitembtn[index].rBtnPicSmall.bottom - dialitembtn[index].rBtnPicSmall.top,//图象原矩形
                SRCCOPY);//把位图从源矩形拷贝到目的矩形，扩展或压缩位图以使其适合目的矩形的范围
            iDisplayNumber++;
        }
    }
    else
    {
        for (i = 0;i < iPhoneLen;i++)//需要显示大号字符
        {
            iNumberX = iNUMBERTOPX + (iDisplayNumber % iLINENUMBERNUMLARGE) * iChWidthLarge;
            iNumberY = iNUMBERTOPY + (iDisplayNumber / iLINENUMBERNUMLARGE) * (iPicNumberHeightLarge + iNUMBERLINESTEP);

            index    = GetDialitemBtnIndex(cPhoneNumber[i]);
            StretchBlt( hdc,
                iNumberX,iNumberY,
                dialitembtn[index].rBtnPicLarge.right - dialitembtn[index].rBtnPicLarge.left,
                dialitembtn[index].rBtnPicLarge.bottom - dialitembtn[index].rBtnPicLarge.top,//目的矩形
                (HDC)hBmpNumberLarge,
                dialitembtn[index].rBtnPicLarge.left,dialitembtn[index].rBtnPicLarge.top,
                dialitembtn[index].rBtnPicLarge.right - dialitembtn[index].rBtnPicLarge.left,
                dialitembtn[index].rBtnPicLarge.bottom - dialitembtn[index].rBtnPicLarge.top,//图象原矩形
                SRCCOPY);//把位图从源矩形拷贝到目的矩形，扩展或压缩位图以使其适合目的矩形的范围
            iDisplayNumber++;
        }
    }
#endif
}
/********************************************************************
* Function   MBDialupCreate  
* Purpose    初始化拨号界面 
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

    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_SET,1),(LPARAM)LEFTBUTTONTEXT);//拨打
    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT,BUTTONBACK);//返回
    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_MIDDLE,2),(LPARAM)BUTTON_MIDDLE_TEXT);//联系人

    for (i = 0;i < iMBButtonNum;i++)
        dialitembtn[i].iState = BTN_UP;

    ClearKeyDown    = BTN_UP;//当前没有退格键按下
    bPenDown        = FALSE; //当前没有触摸笔按下

#elif defined(MBDIALUPSTYLE_KEYBORD)
    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_SET,1),
        (strlen(cPhoneNumber) == 0) ? (LPARAM)"" : (LPARAM)LEFTBUTTONTEXT);
    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT,BUTTONDELETE);//删除

    bKeyDown        = FALSE;//当前没有按键按下
    bClearKeyDown   = FALSE;//当前没有清除键按下
#endif

    if (FindSIMState(FALSE) == 0)//没有SIM卡的时候进行处理
    {
        PostMessage(hWnd,IDC_INIT112,0,0);
    }

    iNumberLen = strlen(cPhoneNumber);
    if ((bSuperAddDial) &&              //进入追加号码的界面后,需要立刻发送第一个字符
        (!GetListCurCalling()) &&       //没有列当前通话
        (!IsAllType_Specific(CALL_STAT_HELD)))//当前不是全部保持
    {        
        if (iNumberLen > 0)
        {
            chCurrent = cPhoneNumber[iNumberLen - 1];//获得最后一个字符
            ME_SendDTMF(hWnd,IDC_ME_SUPERADDDIAL,chCurrent);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALUP)
            printf("mb MBDialup 发送追加号码:%c\r\n",chCurrent);
#endif
        }
        else
        {
            PostMessage(hWnd,IDC_ME_SUPERADDDIAL,ME_RS_SUCCESS,0);
        }
    }
#if defined(KEYSWITCHTYPE_SHORT)
    if ((iNumberLen == 1) && (cPhoneNumber[0] == cCh_Asterisk))//只有一个字符,就是星号
    {
        bArriveKeySwitch = FALSE;
        KillTimer(hWnd,IDC_TIMER_SHORTKEY);
        SetTimer(hWnd,IDC_TIMER_SHORTKEY,TIMER_SHORTKEY,NULL);//启动短按键计时器
    }
    else
    {
        bArriveKeySwitch = TRUE;
    }
#endif

    if (!GetInitState())//电话部分初始化尚未完成
        MessageMBPostMsgCallForward();//重新开始查询有关呼叫转移的信息
}
/********************************************************************
* Function   bInitMBDialup  
* Purpose    初始化拨号所需的数据 
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
    SIZE        NumSize;//数字图片的尺寸
    SIZE        BtnSizeBar;//输入框的图片的尺寸
    SIZE        BtnSizeBS;//退格键的图片的尺寸
    HDC         hdcPicNum,hdcPicBtnUp,hdcPicBtnDown,hdcBar,hdcBSUp,hdcBSDown;

    if (bInit)
        return;

    GetImageDimensionFromFile(pPicNumBar,&BtnSizeBar);//输入框的图片的尺寸
    GetImageDimensionFromFile(pPicBSUp  ,&BtnSizeBS );//退格键的图片的尺寸(正常)

    SetRect(&rPicNumBar,0,0,BtnSizeBar.cx,BtnSizeBar.cy);//输入框的区域
    SetRect(&rPicBS,BtnSizeBar.cx,0,BtnSizeBS.cx + BtnSizeBar.cx,BtnSizeBS.cy);//退格键的区域(正常)

    hdcBar            = CreateMemoryDC(BtnSizeBar.cx,BtnSizeBar.cy);
    hdcBSUp           = CreateMemoryDC(BtnSizeBS.cx ,BtnSizeBS.cy );
    hdcBSDown         = CreateMemoryDC(BtnSizeBS.cx ,BtnSizeBS.cy );

    hPicNumBar        = CreateBitmapFromImageFile(hdcBar   ,pPicNumBar,&Color,&bTran);//输入框的句柄
    hPicBackSpaceUp   = CreateBitmapFromImageFile(hdcBSUp  ,pPicBSUp  ,&Color,&bTran);//退格键的句柄(正常)
    hPicBackSpaceDown = CreateBitmapFromImageFile(hdcBSDown,pPicBSDown,&Color,&bTran);//退格键的句柄(按下)

    DeleteObject(hdcBar);
    DeleteObject(hdcBSUp);
    DeleteObject(hdcBSDown);
    //////////////////////////////////////////////////////////////////////////
    GetImageDimensionFromFile(pPicNumber,&NumSize); //获得数字图片尺寸
    iPicNumChWidth  = NumSize.cx / iMBButtonNum;    //单个数字区域的宽
    iPicNumHeight   = NumSize.cy;                   //数字图片的高度

    hdcPicNum       = CreateMemoryDC(NumSize.cx,NumSize.cy);
    hPicNumber      = CreateBitmapFromImageFile(hdcPicNum,pPicNumber,&Color,&bTran);//加载数字图片
    DeleteObject(hdcPicNum);

    SetRect(&dialitembtn[0].rPicNum,iPicNumChWidth * 9,0,
        iPicNumChWidth * 10,iPicNumHeight);
    dialitembtn[0].ch_short = cCh_Num0;//'0'
    dialitembtn[0].iState   = BTN_UP;

    for (i = 1;i < iNumberCount;i++)//设置1-9数字的区域
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

    for (i = 0;i < iBtn_ShowNums;i++)//加载按钮图片的信息
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
    int  iPicChWidthLarge;//单个数字区域的宽(大号字符)
    int  iPicChWidthSmall;//单个数字区域的宽(小号字符)
    int  iNumberLinesLarge;//界面显示电话号码的最大行数(大号)
    int  iScreenWidth;
    HDC  hdc;

    if (bInit)
        return;

    if (!GetImageDimensionFromFile(pPICNUMBERLARGE,&sizeLarge))//获得文件尺寸(大号)
    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALUP)
        printf("\r\n 无法找到文件:%s",pPICNUMBERLARGE);
#endif
        return;
    }
    if (!GetImageDimensionFromFile(pPICNUMBERSMALL,&sizeSmall))//获得文件尺寸(小号)
    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALUP)
        printf("\r\n 无法找到文件:%s",pPICNUMBERSMALL);
#endif
        return;
    }
    iPicNumberHeightLarge   = sizeLarge.cy;//号码位图的高度(大号)
    iPicChWidthLarge        = sizeLarge.cx / iMBButtonNum;//单个数字区域的宽(小号)

    iPicNumberHeightSmall   = sizeSmall.cy;//号码位图的高度(小号)
    iPicChWidthSmall        = sizeSmall.cx / iMBButtonNum;//单个数字区域的宽(小号)

    iNumberLinesLarge       = GetScreenUsableWH1(SM_CYSCREEN) / iPicNumberHeightLarge;//界面显示电话号码的最大行数(大号)
    //////////////////////////////////////////////////////////////////////////
    SetRect(&dialitembtn[0].rBtnPicLarge,iPicChWidthLarge * 9,0,
        iPicChWidthLarge * 10,iPicNumberHeightLarge);
    SetRect(&dialitembtn[0].rBtnPicSmall,iPicChWidthSmall * 9,0,
        iPicChWidthSmall * 10,iPicNumberHeightSmall);

    dialitembtn[0].ch_short = cCh_Num0;//'0'
    dialitembtn[0].iState   = BTN_UP;
    for (i = 1;i < iNumberCount;i++)//设置1-9数字的区域
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
    iChWidthLarge       = (iScreenWidth - 2 * iNUMBERTOPX) / iLINENUMBERNUMLARGE;//一个字符的最大宽度(大号)
    iChWidthSmall       = (iScreenWidth - 2 * iNUMBERTOPX) / iLINENUMBERNUMSMALL;//一个字符的最大宽度(小号)

    iNumberCountLarge   = iNumberLinesLarge * iLINENUMBERNUMLARGE;//界面上显示号码的最大个数(大号数字)

    hdc                 = CreateMemoryDC(sizeLarge.cx,sizeLarge.cy);
    hBmpNumberLarge     = CreateBitmapFromImageFile(hdc,pPICNUMBERLARGE,&Color,&bTran);
    DeleteObject(hdc);

    hdc                 = CreateMemoryDC(sizeSmall.cx,sizeSmall.cy);
    hBmpNumberSmall     = CreateBitmapFromImageFile(hdc,pPICNUMBERSMALL,&Color,&bTran);
    DeleteObject(hdc);

    if ((hBmpNumberLarge == NULL) && (hBmpNumberSmall == NULL))
    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBDIALUP)
        printf("\r\n 无法获得图片句柄.\r\n");
#endif
        return;
    }
#endif
    bInit = TRUE;
}
