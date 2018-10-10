/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 打接电话部分
 *
 * Purpose  : 接听电话
 *            
\**************************************************************************/
#include "MBRing.h"

#define DEBUG_MBRING
#define BLACKLIST//定义这个宏表示需要支持黑名单的处理

static const char * pClassName          = "MBRingAppClass";
static const char * pClassNameConfirm   = "RingConfirmWndClass";
static const char * pCFString           = "%s%s";
static const int    iPortraitX          = 3;//头像横坐标,也是动画的横坐标
static const int    iPortraitY          = 3;//头像纵坐标
static const int    iPorCartoonSpace    = 3;//头像和动画显示位置的间隔
static const int    iPortraitXSpace     = 3;//头像和显示文字的间隔
static const int    iWordSpace          = 3;//姓名,号码文字之间的间隔

static const int    iPortraitW          = 64;//头像最大宽度
static const int    iPortraitH          = 64;//头像最大高度
static HWND         hWndRingApp = NULL; //来电窗口
static PHONEITEM    NewPhoneItem;       //包含来电信息

static PBINFO       pbNameIcon;         //从联系人处读取信息的结构
static BOOL         bHangUp = FALSE;    //是否已经挂断电话
static BOOL         bCurSceneModeCall;  //当前情景模式是否是呼机模式
static BOOL         bExistEarphone;     //当前是否有耳机
static HINSTANCE    hInstance;
static HWND         hConfirmParent = NULL;
static HWND         hWnd_Confirm = NULL;
static MBConfig     mbconfig;           //通话设置信息
static BOOL         bCallForwarding;//接听界面收到这个消息表示当前来电是被别的手机转移过来的
/********************************************************************
* Function    MBPhoneRing 
* Purpose     接电话入口点 
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
    if ((bCurSceneModeCall) || //呼机模式,不必显示界面
#if defined(BLACKLIST)
        (pbNameIcon.bBlackList) ||//黑名单中直接挂断,不必显示界面
#endif
        ((bExistEarphone) && (mbconfig.bModeAuto)))//有耳机且当前是自动接听,不必显示界面
        return TRUE;

    ShowWindow(hWndRingApp, SW_SHOW);
    UpdateWindow(hWndRingApp);

    return TRUE;
}
/********************************************************************
* Function     MBRingAppWndProc
* Purpose      接电话过程函数
* Params       
* Return           
* Remarks      
**********************************************************************/
static LRESULT CALLBACK MBRingAppWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam)
{
    static  HGIFANIMATE     hRingGif;
    static  BOOL            bSCartoonValid; //gif动画的尺寸是否合法,TRUE合法,FALSE超大
    static  BOOL            bSetPortrait;   //是否设置了来电头像
    static  BOOL            bSetCartoon;    //是否设置了来电动画
    static  HBITMAP         hBitPortrait;   //头像句柄
    static  HBITMAP         hBitCartoon;    //动画句柄
    static  SIZE            sPortrait;      //头像图片尺寸
    static  SIZE            sCartoon;       //动画图片尺寸
    static  char            cName[PHONENUMMAXLEN + 1] = "";
    static  char            cPhone[PHONENUMMAXLEN + 1] = "";
    static  RECT            rPortrait;      //显示头像的最大区域
    static  RECT            rCartoon;       //显示动画的最大区域
    static  RECT            rName;          //显示姓名
    static  RECT            rCallForward;   //转接来电
    static  RECT            rPhoneNumber;   //显示电话号码
    static  BOOL            bCFClew;        //转接来电的显示标志,用于闪烁显示
            char            cCartoon[CARTOONFILENAMELEN + 1];
            COLORREF        Color;
            BOOL            bTran,bGetInfoFromPhonebook;
            HDC             hdc;
            LONG            lResult;
            int             PhoneGroupNum;  //电话路数
            int             iSour,iSH,iSW,iFontHeight;
            CARTOON         cartoon;

    lResult = (LRESULT)TRUE;

    switch (wMsgCmd)
    {
    case WM_CREATE :
        bCurSceneModeCall = GetSceneModeCall();
        if (bCurSceneModeCall)//呼机模式直接挂断
        {
            PostMessage(hWnd,WM_COMMAND,IDC_HANGUP,0);
            return TRUE;
        }
        /////////////////////查联系人部分/////////////////////////////////////////////////////
        memset(&pbNameIcon,0x00,sizeof(PBINFO));

        bGetInfoFromPhonebook = MB_GetInfoFromPhonebook(NewPhoneItem.Calls_Info.PhoneNum,&pbNameIcon);

#if defined(BLACKLIST)
        if (pbNameIcon.bBlackList)//该电话在黑名单中直接挂断
        {
            PostMessage(hWnd,WM_COMMAND,IDC_HANGUP,0);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRING)
            printf("mb MBRing.c 电话:%s 在黑名单中\r\n",NewPhoneItem.Calls_Info.PhoneNum);
#endif
            return TRUE;
        }
#endif
        if (bGetInfoFromPhonebook)//该电话在联系人中定义
        {
            iSour = strlen(pbNameIcon.Name);
            strncpy(NewPhoneItem.cName,pbNameIcon.Name,sizeof(NewPhoneItem.cName) > iSour ? iSour + 1:sizeof(NewPhoneItem.cName) - 1);//该电话不是陌生的,拷贝姓名
        }
        ///////通话设置部分///////////////////////////////////////////////////////////////////
        ReadMobileConfig(&mbconfig);//读取通话设置的信息
        bExistEarphone = EarphoneStatus();
        if (bExistEarphone)//有耳机
        {
            if (mbconfig.bModeAuto)//是自动接听
            {
                PostMessage(hWnd,WM_COMMAND,IDC_REPLACE_CALL,0);
                return TRUE;
            }
        }
        ///////////////显示按钮部分///////////////////////////////////////////////////////////
        PhoneGroupNum = GetPhoneGroupNum();
        if (2 == PhoneGroupNum)//已经有两路电话,需替代其中的一路
        {
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, 
                MAKEWPARAM(IDC_REPLACE_CALL,1),(LPARAM)IDS_REPLACE);//替换
        }
        else if ((0 == PhoneGroupNum) || (1 == PhoneGroupNum))
        {
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, 
                MAKEWPARAM(IDC_REPLACE_CALL,1),(LPARAM)IDS_VOICE_ANSWER);//接听
        }
        if (GetUsedItemNum() > 0)
        {
#if defined(SUPPORTMIDDLEBUTTON)
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,
                MAKEWPARAM(IDC_BUTTON_MIDDLE,2),(LPARAM)BUTTON_MIDDLE_TXT);//拨号
#endif
        }
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,
            (WPARAM)IDC_HANGUP,(LPARAM)IDS_HANGUP);//挂断
        /////////显示图片所需部分/////////////////////////////////////////////////////////////////
        iSW = GetScreenUsableWH1(SM_CXSCREEN);//屏幕宽
        iSH = GetScreenUsableWH1(SM_CYSCREEN);//屏幕高
        hdc = GetDC(hWnd);

        SetRect(&rPortrait,iPortraitX,iPortraitY,//设置头像区域
            iPortraitX + iPortraitW,iPortraitY + iPortraitH);
        SetRect(&rCartoon,0,rPortrait.bottom + iPorCartoonSpace,iSW,iSH);//设置动画区域

        if (FileIfExist(pbNameIcon.Icon))//设置头像,设置组图标就显示头像
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRING)
            printf("mb MBRing.c 设置了头像:%s\r\n",pbNameIcon.Icon);
#endif
            GetImageDimensionFromFile(pbNameIcon.Icon,&sPortrait);//获得头像文件尺寸
            hBitPortrait = CreateBitmapFromImageFile(hdc,pbNameIcon.Icon,&Color,&bTran);
            bSetPortrait = TRUE;//设置来电头像
        }
        else
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRING)
            printf("mb MBRing.c 头像:%s没有找到\r\n",pbNameIcon.Icon);
#endif
            hBitPortrait = NULL;
            bSetPortrait = FALSE;//没有设置头像
        }

        if (FileIfExist(pbNameIcon.PengName))//设置了组动画
        {
            iSour = strlen(pbNameIcon.PengName);
            strncpy(cCartoon,pbNameIcon.PengName,sizeof(cCartoon) > iSour ? iSour + 1 : sizeof(cCartoon) - 1);
            bSetCartoon = TRUE;//设置来电动画
        }
        else//没有设置组动画,从动画设置部分获取信息
        {
            GetCartoon(&cartoon);
            if (FileIfExist(cartoon.cCall))//动画设置部分设置了来电动画信息
            {
                iSour = strlen(cartoon.cCall);
                strncpy(cCartoon,cartoon.cCall,sizeof(cCartoon) > iSour ? iSour + 1 : sizeof(cCartoon) - 1);
                bSetCartoon = TRUE;//设置来电动画
            }
            else
            {
                bSetCartoon = FALSE;//没有设置来电动画
            }
        }

        if (bSetCartoon)//设置来电动画
        {
            GetImageDimensionFromFile(cCartoon,&sCartoon);
            if ((sCartoon.cx > rCartoon.right - rCartoon.left) || 
                (sCartoon.cy > rCartoon.bottom - rCartoon.top))//动画尺寸太大,显示缩略图
            {
                bSCartoonValid = FALSE;//尺寸太大
                hBitCartoon    = CreateBitmapFromImageFile(hdc,cCartoon,&Color,&bTran);
            }
            else
            {
                bSCartoonValid = TRUE;//尺寸合法
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

        SetRect(&rPhoneNumber,//用于显示号码
            rPortrait.right + iPortraitXSpace,
            rPortrait.top + iWordSpace,
            iSW - iPortraitXSpace,
            rPortrait.top + iWordSpace+ iFontHeight * 2 );
        SetRect(&rName,//用于显示姓名
            rPhoneNumber.left,
            rPhoneNumber.bottom + iWordSpace,
            iSW - iPortraitXSpace,
            rPhoneNumber.bottom + iWordSpace + iFontHeight * 2);
        SetRect(&rCallForward,//用于显示转接来电的标志
            rPhoneNumber.left,
            rName.bottom + iWordSpace,
            iSW - iPortraitXSpace,
            rName.bottom + iWordSpace + iFontHeight);

        StrAutoNewLine(hdc,NewPhoneItem.cName,cName,&rName,DT_CENTER|DT_VCENTER);
        StrAutoNewLine(hdc,NewPhoneItem.Calls_Info.PhoneNum,cPhone,&rPhoneNumber,DT_CENTER|DT_VCENTER);

        MBRingInit();//接电话之前需要执行的函数的集合
        MBPlayMusic(GetUsedItemNum());

        SetTimer(hWnd,IDC_TIMER_BRINGTOTOP,500,NULL);
        //界面出现之后启动计时器,每隔一段时间将窗口强行置顶
        break;

    case IDC_RING_HANGUP:
        //接听界面收到这个消息表示当前有部分通话被挂断,需要马上进行相应的处理
        PhoneGroupNum = GetPhoneGroupNum();
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRING)
        printf("\nMB MBRing.c IDC_RING_HANGUP,PhoneGroupNum:%d\n",PhoneGroupNum);
#endif
        if (1 == PhoneGroupNum)//当前有一路电话,修改按钮
        {
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, 
                MAKEWPARAM(IDC_REPLACE_CALL,1),(LPARAM)IDS_VOICE_ANSWER);//接听
            DestroyConfirm();//销毁确认窗口
        }
        break;

    case IDC_FORWARDING://接听界面收到这个消息表示当前来电是被别的手机转移过来的
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
            //在接听界面需要允许用户使用键盘操作,所以有拨号界面存在的时候,窗口不得置顶
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
        printf("\nMB MBRing向隐藏的消息分发窗口发通知消息,已经处理来电\n");
#endif
        MessageMBPostMsg(WM_CALLING_HANGUP,NULL,NULL);//向隐藏的消息分发窗口发通知消息,已经处理来电
        break;

    case WM_ACTIVATE:
        if (WA_INACTIVE != LOWORD(wParam))
            SetFocus(hWnd);//激活后设置焦点
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;

    case WM_TODESKTOP:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRING)
        printf("\nmb MBRing WM_TODESKTOP\n");
#endif
        if (0 == wParam)//程序管理器自动发送
            return FALSE;
        SendMessage(hWnd,WM_KEYDOWN,VK_F2,0);
        return FALSE;

    case WM_KEYDOWN:
        switch (LOWORD(wParam))
        {
        case VK_FLIP_ON://翻盖接听
            if (mbconfig.bModeTurn)
                SendMessage(hWnd,WM_COMMAND,IDC_REPLACE_CALL,0);//接听电话

            break;
        case VK_FLIP_OFF://合盖挂断
            if (mbconfig.bModeTurn)
                SendMessage(hWnd,WM_COMMAND,IDC_HANGUP,0);//挂断电话
            break;

        case VK_EARPHONE_KEY://耳机上的按键
        case VK_RETURN:
        case VK_F1://左softkey下边的键
            SendMessage(hWnd,WM_COMMAND,IDC_REPLACE_CALL,0);//接听电话
            break;
        case VK_F10:
        case VK_F2://右softkey下边的键
            SendMessage(hWnd,WM_COMMAND,IDC_HANGUP,0);//挂断电话
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
            if (0 == GetUsedItemNum())
            {
                if (mbconfig.bModeAnyKey)//按任意键接听(只用于单方通话)
                    SendMessage(hWnd,WM_COMMAND,IDC_REPLACE_CALL,0);//接听电话
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
                if (mbconfig.bModeAnyKey)//按任意键接听(只用于单方通话)
                    SendMessage(hWnd,WM_COMMAND,IDC_REPLACE_CALL,0);//接听电话
            }
            break;
        default:
            return  PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, NULL);
        Rectangle(hdc,rPortrait.left,rPortrait.top,rPortrait.right,rPortrait.bottom);
        if (bSetPortrait)//显示头像,或者是组图标
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
            StretchBlt(hdc,nDestX,nDestY,nDestW,nDestH,//目的矩形
                (HDC)hBitPortrait,0,0,sPortrait.cx,sPortrait.cy,//图象原矩形
                SRCCOPY);//把位图从源矩形拷贝到目的矩形，扩展或压缩位图以使其适合目的矩形的范围
        }

        if (bSetCartoon)//设置了来电动画
        {
            if (bSCartoonValid)//动画尺寸合法
            {
                PaintAnimatedGIF(hdc,hRingGif);
            }
            else//动画尺寸非法
            {
                StretchBlt(hdc,
                    (rCartoon.right - rCartoon.left - sCartoon.cx) / 2,
                    (rCartoon.bottom - rCartoon.top - sCartoon.cy) / 2 + rCartoon.top,
                    sCartoon.cx,sCartoon.cy,
                    (HDC)hBitCartoon,0,0,sCartoon.cx,sCartoon.cy,
                    SRCCOPY);
            }
        }
        if (bCallForwarding)//用于显示转接来电
        {
            if (bCFClew)
                DrawText(hdc,CALLFORWARDING,strlen(CALLFORWARDING),&rCallForward,DT_CENTER);

            bCFClew = !bCFClew;
        }
        if (strlen(pbNameIcon.Name) != 0)//显示来电者的名字
        {
            if (bCallForwarding)
                DrawText(hdc,cName,strlen(cName),&rName,DT_LEFT|DT_VCENTER);
            else
                DrawText(hdc,cName,strlen(cName),&rName,DT_CENTER|DT_VCENTER);
        }
        //显示电话号码
        DrawText(hdc,cPhone,strlen(cPhone),&rPhoneNumber,DT_CENTER|DT_VCENTER);
        EndPaint(hWnd, NULL);
        break;

    case WM_CALLING_HANGUP://收到对方挂断消息
        MBRingHangup();//来电接听的挂断处理
        break;

    case WM_SWITCH_CALLLINE:
    case WM_VOICER_ANSWER://来电应答
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            MBRingAnswerCall();//接听电话之后的处理
            break;
        default://通话中断
            SendAbortCommand(wParam,lParam);//发送取消操作的at命令
            MBCallEndWindow(NewPhoneItem.cName,NewPhoneItem.Calls_Info.PhoneNum,NULL,NULL);
            DestroyWindow(hWnd);
            break;
        }
        break;

    case WM_VOICER_HANGUP://主动挂断        
        if(bHangUp)
            break;
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            MBRingHangup();//来电接听的挂断处理
            MBListCurCalls(LISTCURCALLS_REPEAT,wParam,lParam);
            break;
        default://主动挂断失败
            SendAbortCommand(wParam,lParam);//发送取消操作的at命令
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
* Purpose      来电接听的挂断处理
* Params       
* Return           
* Remarks      
**********************************************************************/
static  void    MBRingHangup(void)
{
    Call_EndPlayMusic(RING_PHONE);

    bHangUp = TRUE;

    SaveMBRecordByPhoneNum(UNANSWER_REC,NewPhoneItem.Calls_Info.PhoneNum,NULL);//保存未接来电

    DestroyWindow(hWndRingApp);
#if defined(BLACKLIST)
    if ((!bCurSceneModeCall) && (!pbNameIcon.bBlackList))//不是呼机模式和黑名单则显示通话结束的界面
#else
    if (!bCurSceneModeCall)
#endif
        MBCallEndWindow(NewPhoneItem.cName,NewPhoneItem.Calls_Info.PhoneNum,NULL,NULL);

    if (GetUsedItemNum() == 0)//当前没有通话
        EnableState();

    MBRecrodList_Refresh(UNANSWER_REC);//通知通话记录列表刷新
}
/********************************************************************
* Function     OnCommandMsg
* Purpose      处理窗口的WM_COMMAND消息
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
    case IDC_REPLACE_CALL://接听
        Call_EndPlayMusic(RING_PHONE);
        if (0 == PhoneGroupNum)
        {
            if (-1 == ME_CallAnswer(hWnd,WM_VOICER_ANSWER))//接听失败
                AppMessageBox(hWnd,IDS_ANSWER_FAIL,TITLECAPTION,WAITTIMEOUT);
        }
        else if (1 == PhoneGroupNum)
        {
            if (MAX_COUNT == GetUsedItemNum())//一路电话，但是电话数已经达到最大容量
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

    case IDC_BUTTON_MIDDLE://拨号
        SuperAddDialNumber(NULL,FALSE,TRUE);
        break;

    case IDC_HANGUP://拒绝
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
* Purpose      获得来电的信息
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
* Purpose    向接听界面发送消息
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
* Purpose    当前是否存在接听界面
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
* Purpose    接听电话之后的处理
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

    if (bCallForwarding)//该电话是被转移过来的,在姓名之前增加提示语
    {
        sprintf(cNameCF,pCFString,CALLFORWARDING,NewPhoneItem.cName);
        iSour = strlen(cNameCF);
        strncpy(NewPhoneItem.cName,cNameCF,sizeof(NewPhoneItem.cName) > iSour ? iSour + 1 : sizeof(NewPhoneItem.cName) - 1);
    }
    NewPhoneItem.Calls_Info.Stat = CALL_STAT_ACTIVE;//记录状态,处于激活状态的通话

    SaveMBRecordByPhoneNum(ANSWERED_REC,NewPhoneItem.Calls_Info.PhoneNum,&sCurTime);//保存已接来电

    memcpy(&NewPhoneItem.TimeInDial,&sCurTime,sizeof(SYSTEMTIME));
    MBCallingWindow(&NewPhoneItem,NULL,DTMF_NONE);//进入通话窗口

    MBRecrodList_Refresh(ANSWERED_REC);//通知通话记录列表刷新

    DestroyWindow(hWndRingApp);
}
//////////////////////////////////////////////////////////////////////////
/********************************************************************
* Function     GotoConfirmWindow
* Purpose      确定替换窗口生成
* Params       
* Return           
* Remarks      挂断激活的通话
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
* Purpose       确定替换窗口函数
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
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_MIDDLE,2),(LPARAM)BUTTON_MIDDLE_TXT);//拨号
#endif
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_QUIT,0),(LPARAM)BUTTONQUIT);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON_SET://挂断激活的通话,新来的电话也一起被接通
            if (-1 == ME_SwitchCallStatus(hWnd,WM_SWITCH_CALLLINE,CUT_ACTIVED,0))
                AppMessageBox(hWnd,IDS_ANSWER_FAIL,TITLECAPTION,WAITTIMEOUT);
            break;

        case IDC_BUTTON_MIDDLE://拨号
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
        if (0 == wParam)//程序管理器自动发送
            return FALSE;
        SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
        return FALSE;

    case WM_KEYDOWN:
        switch (LOWORD(wParam))
        {
        case VK_EARPHONE_KEY://耳机上的按键
        case VK_F1://左softkey下边的键
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SET,0);
            break;
        case VK_F2://右softkey下边的键
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
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

    case WM_SWITCH_CALLLINE://挂断激活通话的回掉消息
        MBListCurCalls(LISTCURCALLS_HANGUP_ACTIVE,wParam,lParam);//列当前通话
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
* Purpose       销毁确认窗口
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
* Purpose      获得当前来电的电话号码
* Params       pPhoneNum:保存当前来电的电话号码的缓冲区,iBuflen:目标缓冲区的长度
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
* Purpose      接电话之前需要执行的函数的集合
* Params      
* Return           
* Remarks      int EncORDec();
                返回值1：调CloseVideo();
                返回值0：调Mini_OnStopVideoPlay(HWND hWnd)
**********************************************************************/
static  void    MBRingInit(void)
{
    //SetupHf_Lcd(TRUE);
    //SetupHf_LcdBklight(TRUE);//开背光
    DlmNotify(PMM_NEWS_ENABLE,PMF_MAIN_BKLIGHT|PMF_MAIN_SCREEN_LCD);
    EndPicBrowser();//来电关闭图片预览
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
    if (GetUsedItemNum() == 0)//当前没有通话
        DisableState();
}
