/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 打接电话部分
 *
 * Purpose  : 通话界面
 *            
\**************************************************************************/
#include "MBCalling.h"

#define DEBUG_MBCALLING

static const int    iMaxCount   = MAX_COUNT;
static const int    iPortraitX  = 3;//头像区域横坐标
static const int    iPortraitY  = 3;//头像区域纵坐标
static const int    iPortraitW  = 80;//头像区域最大宽度
static const int    iPortraitH  = 80;//头像区域最大高度
static const int    iDTMFEditX  = 10;
static const int    iDTMFEditY  = 10;

static const double dZero       = 0.0;
static const int    iSecond60   = 60;
static const char   ch_End      = '\0';
static const char * pClassName  = "MBCallingWndClass";//窗口类名
static const char * pDTMFClass  = "MBCallingDTMFWndClass";
static const char * pNum0       = "0";
static const char * pPlus       = "+";
static const char * pAsterisk   = "*";
static const char * pWell       = "#";
static const char * pTime       = "%02d:%02d:%02d";
static const char * pTimeEnd    = "%s:%02d:%02d:%02d";
static const char * pCostInfo1  = "%s:%f";
static const char * pCostInfo2  = "%ld";
static const char   ch_Plus     = '+';
#if defined(WAVECOM_2C)
static const char * pAbortAT    = "AT+WAC\r";
static const int    iAbortValue = 515;
static const int    iTimeOut    = 4000;
#endif

static MBCALLTYPE   CallingType;                    //当前通话方式
static INTERFACE    InterfaceType = INTERFACE_NONE; //当前界面类型

static char         cPhoneNumberExtension[PHONENUMMAXLEN + 1];//保存分机号
static DTMF_TYPE    Dtmf_Type;              //是否有分机号

static HWND         hCalling;               //通话界面句柄
static HWND         hMBList;                //多方通话界面列表控件
static PHONEITEM    PhoneItem[MAX_COUNT];   //电话信息
static int          PhoneGroupNum = 0;      //电话的路的个数
static HINSTANCE    hInstance;

static BOOL         bSelectSingleConnect;   //是否选择了单独通话的按钮
static BOOL         bListCurCalling;        //是否正在列当前通话(TRUE正在列当前通话,FALSE没有列当前通话)
/*由于在当前通话全为保持的时候,发送分机号会导致出错,所以在没有激活通话的时候需要
避免发送分机号.发送分机号之前要检测当前各个通话的状态,全部为保持的时候不能发送分机号
当通话个数改变的时候,需要重新列当前通话,只有当列当前通话的操作完成之后才可以发送分机号.
*/
static BOOL         bCanListCurCalling;     //是否可以在IDC_TIMER_ERROR消息中列当前通话
static OPERATETYPE  ListCurCallsType;       //列当前通话之前的操作
static WPARAM       ListCurCallswParam;     //列当前通话之前的at命令回掉消息的wparam参数
static WPARAM       ListCurCallslParam;     //列当前通话之前的at命令回掉消息的lparam参数

static BOOL         bMute = FALSE;          //当前是否正在静音
static BOOL         bHolding = FALSE;       //是否正在被Hold
#if defined(MBCALLING_RECORD)
static HWND         hBRecord;               //界面按钮句柄
static BOOL         bRecord = FALSE;        //当前是否正在录音
#endif
//////////////////////////通话所需数据//////////////////////
static RECT         rName;                  //姓名
static RECT         rPhoneCode;             //号码
static RECT         rTime;                  //通话时间
static RECT         rPhoto;                 //图片区域
static RECT         rCostInfo;              //显示通话费用的区域
static int          iCallingOneNum;         //单方通话的时候,通话信息位于结构数组中的位置
static HBITMAP      hPortrait = NULL;       //单方通话的人物头像
static SIZE         sPortrait;              //单方通话的人物头像的尺寸
static MBConfig     MBCongig_Calling;       //通话设置信息
static int          iTime_Second;           //通话计时,用于播放50秒提示音
#if defined(MBCALLING_TIMEACCURATE)
static int          iInitSecond;            //协助通话计时(秒)的变量,用于保证显示计时准确
#endif
static char         cName[PHONENUMMAXLEN + 1];//用于显示来电姓名
static char         cPhone[PHONENUMMAXLEN + 1];//用于显示来电号码
//****************关于计费信息************************************
static BOOL         bCostLimit = FALSE;     //当前通话是否已到话费限制
static BOOL         bAOCInfo;               //是否显示通话计费信息
static BOOL         bCanGetCurrentCCM;      //是否可以发送消息读取当前计费信息
static DWORD        dCostInfo;              //当前Call Meter值
static ME_PUCINFO   me_pucinfo;             //计价信息
typedef enum                                //通话计费的显示类型
{
    COSTTYPE_UNIT,                          //按单位显示
    COSTTYPE_PRICE                          //按单价显示
}COSTTYPE;
static COSTTYPE     CostType;               //通话计费显示方式
static double       dCost;                  //通话计费的单价信息
/********************************************************************
* Function     MBCallingWindow
* Purpose      调用通话界面
* Params       NewItemInfo:新来的电话信息,pExtension:分机号,dtmf_type:分机号类别
* Return           
* Remarks      本函数由接入/拨出功能调用
**********************************************************************/
BOOL    MBCallingWindow(const PHONEITEM * pPhoneItem,const char * pExtension,
                        DTMF_TYPE dtmf_type)
{
    int iFirstNotUsedItem;  //得第一个未用ITEM
    int nUsedItemNum;       //得已用Item个数

    if ((1 == PhoneGroupNum) || (0 == PhoneGroupNum))
    {
        iFirstNotUsedItem = GetFirstNotUsedItem();//得第一个未用ITEM
        if (-1 == iFirstNotUsedItem)
            return FALSE;

        SetAllType(CALL_STAT_HELD); //全部设为指定类型:保持状态

        memcpy(&PhoneItem[iFirstNotUsedItem],pPhoneItem,sizeof(PHONEITEM));
        PhoneItem[iFirstNotUsedItem].ItemStat = ITEMUSED;//标志为已经使用

        SetPhoneGroupNum();//设置电话的路数

        nUsedItemNum = GetUsedItemNum();//得已用Item个数

        if (1 == nUsedItemNum)//单方通话
            CallingType = MBCALLINGONE;
        else if ((1 < nUsedItemNum) && (nUsedItemNum <= iMaxCount))//多方通话
            CallingType = MBCALLINGMULT;
        else
            return FALSE;

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("\nMB MBCallingWindow,CallingType:%d,号码:%s\n",
            CallingType,PhoneItem[iFirstNotUsedItem].Calls_Info.PhoneNum);
#endif
        Dtmf_Type = dtmf_type;
        if (pExtension != NULL)
        {
            if (strlen(pExtension) == 0)
                Dtmf_Type = DTMF_NONE;                      //没有分机号
            else
                strcpy(cPhoneNumberExtension,pExtension);   //记录分机号
        }
        else
        {
            Dtmf_Type = DTMF_NONE;                          //没有分机号
        }
        return (MBTalkingWindow());
    }
    return FALSE;
}
/********************************************************************
* Function   MessageMBCallingOne  
* Purpose    向单方通话界面发送消息
* Params    
* Return    
* Remarks   
**********************************************************************/
void    MessageMBCallingOne(UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    if ((hCalling != NULL) && (IsWindow(hCalling)))
        PostMessage(hCalling,wMsgCmd,wParam,lParam);
}
/********************************************************************
* Function   IsWindow_MBCalling  
* Purpose    当前是否存在通话界面 
* Params     
* Return     
* Remarks    
**********************************************************************/
BOOL    IsWindow_MBCalling(void)
{
    if (hCalling == NULL)
        return FALSE;
    return (IsWindow(hCalling));
}
/********************************************************************
* Function     SetListCurCalls
* Purpose      设置是否正在列当前通话
* Params       
* Return      无 
* Remarks     
**********************************************************************/
void    SetListCurCalls(BOOL bListCur)
{
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling 设置是否正在列当前通话:%d\r\n",bListCur);
#endif
    bListCurCalling = bListCur;
}
/********************************************************************
* Function     GetListCurCalling
* Purpose      获得是否正在列当前通话
* Params       
* Return      
* Remarks     
**********************************************************************/
BOOL    GetListCurCalling(void)
{
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    if (bListCurCalling)
        printf("mb MBCalling 正在列当前通话\r\n");
    else
        printf("mb MBCalling 没有列当前通话\r\n");
#endif
    return bListCurCalling;
}
/********************************************************************
* Function     ReadMobileConfig
* Purpose      读取通话设置的信息
* Params       
* Return      无 
* Remarks     第一次读取的时候从文件中读取,以后从内存中读取
**********************************************************************/
void    ReadMobileConfig(MBConfig * pConfig)
{
    if (NULL == hCalling)//尚未显示通话窗口
    {
        iTime_Second = 0;//通话计时,用于播放50秒提示音
        ReadMobileConfigFile(&MBCongig_Calling);
    }
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling ReadMobileConfig,MBCongig_Calling.b50Tip:%d\r\n",MBCongig_Calling.b50Tip);
#endif
    memcpy(pConfig,&MBCongig_Calling,sizeof(MBConfig));
}
/********************************************************************
* Function     GetFirstNotUsedItem
* Purpose      得第一个未用ITEM
* Params       
* Return           
* Remarks      
**********************************************************************/
static  int     GetFirstNotUsedItem(void)
{
    int i;
    for (i = 0;i < iMaxCount;i++)
    {
        if (ITEMNOTUSED == PhoneItem[i].ItemStat)
            return i;
    }
    return -1;
}
/********************************************************************
* Function     GetUsedItemNum
* Purpose      得已用Item个数(用于判断当前是否是多方通话)
* Params       
* Return     0:没有通话,1:单方通话,否则大于1多方通话       
* Remarks      
**********************************************************************/
int     GetUsedItemNum(void)
{
    int nRet = 0,i;
    for(i = 0;i < iMaxCount;i++)
    {
        if ((ITEMUSED == PhoneItem[i].ItemStat) && (0 != PhoneItem[i].Calls_Info.index))
            nRet++;
    }
    return nRet;
}
/********************************************************************
* Function     GetItemTypeNum
* Purpose      得已用激活或者保持的Item个数
* Params       
* Return     -1:iType非法
* Remarks      
**********************************************************************/
static  int     GetItemTypeNum(int iType)
{
    int nRet = 0,i;
    if ((CALL_STAT_ACTIVE != iType) && (CALL_STAT_HELD != iType))
        return -1;

    for (i = 0;i < iMaxCount;i++)
    {
        if ((ITEMUSED == PhoneItem[i].ItemStat) &&
            (0 != PhoneItem[i].Calls_Info.index) &&
            (iType == PhoneItem[i].Calls_Info.Stat))
            nRet++;
    }
    return nRet;
}
/********************************************************************
* Function     GetTheNUsedItem
* Purpose      得第n个使用的记录
* Params       
* Return           
* Remarks      
**********************************************************************/
static  int     GetTheNUsedItem(int nTheN)
{
    int i;
    if (nTheN <= 0)
        return -1;

    for (i = 0 ;i < iMaxCount;i++)
    {
        if ((ITEMUSED == PhoneItem[i].ItemStat) && (0 != PhoneItem[i].Calls_Info.index))
        {
            nTheN--;
            if (0 == nTheN)
                return i;
        }
    }
    return -1;
}
/********************************************************************
* Function     SetPhoneGroupNum
* Purpose      设置电话的路的个数
* Params       
* Return       
* Remarks      
**********************************************************************/
static  void    SetPhoneGroupNum(void)
{
    int iHeldNum,iActiveNum;

    iHeldNum   = GetItemTypeNum(CALL_STAT_HELD);
    iActiveNum = GetItemTypeNum(CALL_STAT_ACTIVE);

    if ((iHeldNum > 0) && (iActiveNum > 0))
        PhoneGroupNum = 2;
    else if ((iHeldNum == 0) && (iActiveNum > 0) || (iHeldNum > 0) && (iActiveNum == 0))
        PhoneGroupNum = 1;
    else 
        PhoneGroupNum = 0;
}
/********************************************************************
* Function     GetPhoneGroupNum
* Purpose      获得电话的路的个数
* Params       
* Return           
* Remarks      
**********************************************************************/
int     GetPhoneGroupNum(void)
{
    SetPhoneGroupNum();
    return (PhoneGroupNum);
}
/********************************************************************
* Function     ExecuteHangup
* Purpose      处理挂断键的操作
* Params       
* Return       
* Remarks      
**********************************************************************/
static  void    ExecuteHangup(void)
{
    int iHeldNum,iActiveNum;

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("\nMB MBCalling,挂断键,CallingType:%d\n",CallingType);
#endif
    switch (CallingType)
    {
    case MBCALLINNONE://显示通话结束界面时候退出
        DestroyWindow(hCalling);
        break;

    case MBCALLINGONE://单方通话
        if (-1 == ME_SwitchCallStatus(hCalling,WM_VOICE_HANGUPONE,CUT_ALL,0))
            AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
        break;

    case MBCALLINGMULT://多方通话(挂断激活的通话)
        SetPhoneGroupNum();
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("MB MBCalling,挂断键 PhoneGroupNum:%d\r\n",PhoneGroupNum);
#endif
        if (PhoneGroupNum == 2)//如果是两路，按END键挂断激活的
        {
            if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_CUT_ACTIVED,CUT_ACTIVED,0))
                AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
        }
        else if (PhoneGroupNum == 1)//如果是一路，无论保持还是激活 按END键就是挂断
        {
            iHeldNum   = GetItemTypeNum(CALL_STAT_HELD);
            iActiveNum = GetItemTypeNum(CALL_STAT_ACTIVE);
            if ((iHeldNum == 0) && (iActiveNum > 0))//只有激活的通话
            {
                if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_CUT_ACTIVED,CUT_ACTIVED,0))
                    AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
            }
            else//只有保持的通话
            {
                if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_CUT_HELD,CUT_HELD,0))
                    AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
            }
        }
        else//出错了
        {
            MBListCurCalls(LISTCURCALLS_REPEAT,-1,-1);//列当前通话
        }
        break;
    }
}
/********************************************************************
* Function     AfterHangup_Single
* Purpose      单方通话中的挂断之后的处理
* Params       
* Return       
* Remarks      这时候通话肯定已经挂断了
**********************************************************************/
static  void    AfterHangup_Single(void)
{
    char                cLastTime[10] = "";//显示通话计时所需数组
    char                cCostInfo[50] = "";
    MBRecordCostTime    CurCostPara;//单方通话计时

    if (PhoneItem[iCallingOneNum].ItemStat != ITEMUSED)
    {
        MBCallEndWindow(CALLEND,NULL,NULL,NULL);//调用通话结束界面
        DestroyWindow(hCalling);
        return;
    }

    ReadRecord_Cost(&CurCostPara);          //获得单方通话计时信息
    RemTime(&CurCostPara,iCallingOneNum);   //计算通话时间
    SaveRecord_Cost(&CurCostPara);          //记录单方通话的时间到通话计时部分
    if (PhoneItem[iCallingOneNum].Calls_Info.Dir == CALL_DIR_ORIGINATED)
    {   //进行通话记录,需要考察当前是发起端,还是终端
        SaveMBRecordUsedTime(DIALED_REC,
            PhoneItem[iCallingOneNum].Calls_Info.PhoneNum,
            &PhoneItem[iCallingOneNum].TimeInDial,&PhoneItem[iCallingOneNum].TimeUsed);
    }
    else if (PhoneItem[iCallingOneNum].Calls_Info.Dir == CALL_DIR_TERMINATED)
    {
        SaveMBRecordUsedTime(ANSWERED_REC,
            PhoneItem[iCallingOneNum].Calls_Info.PhoneNum,
            &PhoneItem[iCallingOneNum].TimeInDial,&PhoneItem[iCallingOneNum].TimeUsed);
    }
    PhoneItem[iCallingOneNum].ItemStat = ITEMNOTUSED;

    sprintf(cLastTime,pTime,
        PhoneItem[iCallingOneNum].TimeUsed.wHour,
        PhoneItem[iCallingOneNum].TimeUsed.wMinute,
        PhoneItem[iCallingOneNum].TimeUsed.wSecond);

    if (bAOCInfo)//显示通话计费信息
    {
        if (bCostLimit)//设置当前通话已到话费限制
        {
            strcpy(cCostInfo,DIALLING_FARE);
        }
        else
        {
            switch (CostType)
            {
            case COSTTYPE_UNIT://单位
                sprintf(cCostInfo,pCostInfo2,dCostInfo);//"%ld"
                break;
            case COSTTYPE_PRICE://单价
                sprintf(cCostInfo,pCostInfo1,me_pucinfo.currency,dCostInfo * dCost);//"%s:%f"
                MB_DealDouble(cCostInfo,3);
                break;
            }
        }
        MBCallEndWindow(PhoneItem[iCallingOneNum].cName,//调用通话结束界面,传入计费信息
            PhoneItem[iCallingOneNum].Calls_Info.PhoneNum,cLastTime,cCostInfo);
    }
    else
    {
        MBCallEndWindow(PhoneItem[iCallingOneNum].cName,//调用通话结束界面
            PhoneItem[iCallingOneNum].Calls_Info.PhoneNum,cLastTime,NULL);
    }
    memset(&PhoneItem[iCallingOneNum],0x00,sizeof(PHONEITEM));
    DestroyWindow(hCalling);
}
/********************************************************************
* Function     AfterHangup_MultOne
* Purpose      多方通话中的挂断处理
* Params       iIndex:结构数组中的位置
* Return       -1:出错,1,正常
* Remarks      多方通话中的挂断一个电话
**********************************************************************/
int     AfterHangup_MultOne(int iIndex)
{
    MBRecordCostTime MultCostPara;   //多方通话计时

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling AfterHangup_MultOne,iIndex:%d\r\n",iIndex);
#endif
    if ((iIndex < 0) || (iIndex > iMaxCount - 1) || 
        (PhoneItem[iIndex].ItemStat != ITEMUSED))
        return -1;

    ReadRecord_Cost(&MultCostPara); //获得多方通话计时信息
    RemTime(&MultCostPara,iIndex);  //计算多方通话的通话计时
    SaveRecord_Cost(&MultCostPara); //保存多方通话的通话计时

    PhoneItem[iIndex].ItemStat = ITEMNOTUSED;//标记为未使用

    if (PhoneItem[iIndex].Calls_Info.Dir == CALL_DIR_ORIGINATED)
    {
        SaveMBRecordUsedTime(DIALED_REC,
            PhoneItem[iIndex].Calls_Info.PhoneNum,
            &PhoneItem[iIndex].TimeInDial,&PhoneItem[iIndex].TimeUsed);
    }
    else if (PhoneItem[iIndex].Calls_Info.Dir == CALL_DIR_TERMINATED)
    {
        SaveMBRecordUsedTime(ANSWERED_REC,
            PhoneItem[iIndex].Calls_Info.PhoneNum,
            &PhoneItem[iIndex].TimeInDial,&PhoneItem[iIndex].TimeUsed);
    }
    memset(&PhoneItem[iIndex],0x00,sizeof(PHONEITEM));
    return 1;
}
/********************************************************************
* Function     AfterHangup_Mult_All
* Purpose      多方通话中全部挂断之后的处理
* Params       
* Return       
* Remarks      这时候所有通话肯定已经挂断
**********************************************************************/
static  void    AfterHangup_Mult_All(void)
{
    int                 i;
    char                cTimer[40] = "";
    MBRecordCostTime    MultCostPara;   //多方通话计时

    ReadRecord_Cost(&MultCostPara);//获得多方通话计时信息

    MBList_ResetItem();//删除多方通话所有节点
    for (i = 0;i < iMaxCount;i++)
    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("MB 多方通话全部挂断,PhoneItem[%d].ItemStat:%d,号码:%s\r\n",
            i,PhoneItem[i].ItemStat,PhoneItem[i].Calls_Info.PhoneNum);
#endif
        if ((PhoneItem[i].ItemStat == ITEMUSED) && (0 != PhoneItem[i].Calls_Info.index))
        {
            RemTime(&MultCostPara,i);//计算多方通话的通话计时
            if (PhoneItem[i].Calls_Info.Dir == CALL_DIR_ORIGINATED)
            {
                SaveMBRecordUsedTime(DIALED_REC,
                    PhoneItem[i].Calls_Info.PhoneNum,
                    &PhoneItem[i].TimeInDial,&PhoneItem[i].TimeUsed);
            }
            else if (PhoneItem[i].Calls_Info.Dir == CALL_DIR_TERMINATED)
            {
                SaveMBRecordUsedTime(ANSWERED_REC,
                    PhoneItem[i].Calls_Info.PhoneNum,
                    &PhoneItem[i].TimeInDial,&PhoneItem[i].TimeUsed);
            }
            sprintf(cTimer,pTimeEnd,IDS_CALLTIME,
                PhoneItem[i].TimeUsed.wHour,
                PhoneItem[i].TimeUsed.wMinute,
                PhoneItem[i].TimeUsed.wSecond);

            if (strlen(PhoneItem[i].cName) != 0)
                MBList_AddItem(PhoneItem[i].cName,cTimer,MBLIST_IPICNONE,0,0);
            else
                MBList_AddItem(PhoneItem[i].Calls_Info.PhoneNum,cTimer,MBLIST_IPICNONE,0,0);
        }
        PhoneItem[i].ItemStat = ITEMNOTUSED;
        memset(&PhoneItem[i],0x00,sizeof(PHONEITEM));
    }
    SaveRecord_Cost(&MultCostPara);//保存多方通话的通话计时

    MBList_Refresh();
    MBList_SetCurSel(0);
    MBList_SetStyle(MBLIST_WS_ENABLED);

    SendMessage(hCalling,MB_ACTIVE,0,0);//不必重列通话
}
/********************************************************************
* Function     MBCalling_Shortcut
* Purpose      多方通话中的快捷处理
* Params       iType:处理类型,pShortPhone:特殊号码
* Return       ME函数执行是否成功
* Remarks      
**********************************************************************/
BOOL    MBCalling_Shortcut(int iType,const char * pShortPhone)
{
    switch (iType)
    {
    case CUT_HELD://呼叫等待时忙音或者挂断保持的通话
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling_Shortcut挂断保持的通话\r\n");
#endif
        if (IsWindow_MBRing())//当前是来电界面
        {   //由于来电界面选择0,将会挂断来电,而不会挂断当前保持的通话,所以要分别处理
            if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_REPEAT,CUT_HELD,0))
                return FALSE;
        }
        else
        {
            if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_CUT_HELD,CUT_HELD,0))
                return FALSE;
        }
        break;

    case CUT_ACTIVED://挂断激活的通话并且直接接入一个等待的电话或者激活一个保持的电话
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling_Shortcut挂断激活的通话\r\n");
#endif
        if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_CUT_ACTIVED,CUT_ACTIVED,0))
            return FALSE;

        break;

    case ACTIVE_SWITCH://在保持和激活的通话状态间切换并且接入其它等待的电话
        switch (CallingType)
        {
        case MBCALLINNONE://没有通话
            break;
        case MBCALLINGONE://单方通话
        case MBCALLINGMULT://多方通话
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling_Shortcut 通话切换:%d\r\n",CallingType);
#endif
            if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_SHORTCUTSWITCH,ACTIVE_SWITCH,0))
                return FALSE;

            break;
        }
        break;

    case ADD_TO_ACTIVE://将保持的通话加入激活通话
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling_Shortcut 将保持的通话加入激活通话\r\n");
#endif
        if (-1 == ME_SwitchCallStatus(hCalling,IDC_MULTME_ALLACTIVE,ADD_TO_ACTIVE,0))
            return FALSE;

        break;

    case ACTIVE_SWITCH_X://将除指定序号外的所有通话变为保持
        if (NULL == pShortPhone)
            return FALSE;
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling_Shortcut 将除指定序号外的所有通话变为保持,%s\r\n",pShortPhone);
#endif
        if (-1 == ME_VoiceDial(hCalling,IDC_ME_ACTIVE_SWITCH_X,(char *)pShortPhone,NULL))
            return FALSE;

        break;

    case CUT_ACTIVED_X://挂断指定序号的激活通话
        if (NULL == pShortPhone)
            return FALSE;
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling_Shortcut 挂断指定序号的激活通话,%s\r\n",pShortPhone);
#endif
        if (-1 == ME_VoiceDial(hCalling,IDC_ME_CUT_ACTIVED_X,(char *)pShortPhone,NULL))
            return FALSE;

        break;

    case CALL_TRANSFER://通话中当已有两路电话时,使这两路电话连起,用户从通话中退出
        if (NULL == pShortPhone)
            return FALSE;
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling_Shortcut 转换,PhoneGroupNum:%d,%s\r\n",PhoneGroupNum,pShortPhone);
#endif
        if (-1 == ME_VoiceDial(hCalling,IDC_ME_CALL_TRANSFER,(char *)pShortPhone,NULL))
            return FALSE;
        break;

    case CUT_ALL:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling_Shortcut 挂断所有通话\r\n");
#endif
        if (-1 == ME_SwitchCallStatus(hCalling,IDC_MULTME_ALLHANGUP,CUT_ALL,0))
            return FALSE;

        break;
    default://其它的快捷操作
        if (NULL == pShortPhone)
            return FALSE;
        if (-1 == ME_VoiceDial(hCalling,IDC_ME_SHORTOTHER,(char *)pShortPhone,NULL))
            return FALSE;

        break;
    }
    return TRUE;
}
//////////////////////////////////////////////////////////////////////////
static  BOOL    MBTalkingWindow(void)//显示通话窗口
{
    WNDCLASS    wc;
    HMENU       hMenu;

    if (IsWindow(hCalling))
    {
        //MoveWindowToTop(hCalling);
        ShowWindow(hCalling,SW_SHOW);
        UpdateWindow(hCalling);
        SetFocus(hCalling);

        MBListCurCalls(LISTCURCALLS_REPEAT,0,0);
        PostMessage(hCalling,MB_ACTIVE,0,0);
        return TRUE;
    }
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

    hMenu = LoadMenuIndirect((PMENUTEMPLATE)&MBCallingMenuTemplate);
    InsertMenu(hMenu,IDM_MUTE,MF_BYCOMMAND,IDM_ONE_HOLD,TXT_HOLD);//保持
    SetChangelessMenu(hMenu);

    hCalling = CreateWindow(pClassName,TITLECAPTION, 
        WS_CAPTION|WS_BORDER |PWS_STATICBAR, 
        PLX_WIN_POSITION,NULL,(HMENU)hMenu,NULL,NULL);

    if (NULL == hCalling)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }

    ShowWindow(hCalling, SW_SHOW);
    UpdateWindow(hCalling); 

    return (TRUE);
}
static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static BOOL bKeyDown;//当前是否有按键按下
    LRESULT     lResult;
    HDC         hdc;
    int         nUsedItemNum;//获得有效的Item个数
    int         iItemData;
    HMENU       hMenu;
    DWORD       dAcmMax = 0;

    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case WM_CREATE :
#if defined(MBCALLING_RECORD)
        {
            int iSW,iSH,iFontHeight;
            GetScreenUsableWH2(&iSW,&iSH);
            iFontHeight = GetCharactorHeight(NULL);//文字高度
            hBRecord    = CreateWindow("BUTTON",TXT_RECORD,//创建录音按钮
                WS_BORDER|WS_VISIBLE|WS_CHILD|WS_BITMAP,
                iPortraitX * 2,iSH - iFontHeight * 3,iSW - iPortraitX * 4,GetButtonControlHeight(),
                hWnd,(HMENU)IDC_BUTTON_RECORD,NULL,NULL);

            if (!IsSpareTimeLeft())//录音时间已满
                EnableWindow(hBRecord,FALSE);

            bRecord = FALSE;//是否正在录音
        }        
#endif
#if defined(MBCALLING_TIMEACCURATE)
        {
            SYSTEMTIME sCurtime;
            GetLocalTime(&sCurtime);
            iInitSecond = sCurtime.wSecond;//协助通话计时(秒)的变量
        }
        SetTimer(hWnd,IDC_TIMER_CALLING,TIMER_CALLING,NULL);//设置通话计时的计时器
#endif
#if defined(MBCALLING_TIMESIMPLE)
        SetTimer(hWnd,IDC_TIMER_CALLING,TIMER_CALLING * 10,NULL);//设置通话计时的计时器
#endif
        SetTimer(hWnd,IDC_TIMER_INIT,TIMER_INIT,NULL);//设置初始化的计时器
        SetTimer(hWnd,IDC_TIMER_ERROR,TIMER_ERROR,NULL);//设置出错处理的计时器

        hPortrait           = NULL;         //头像为空
        bAOCInfo            = FALSE;        //不显示通话计费信息
        InterfaceType       = INTERFACE_ONE;//当前界面类型
        bListCurCalling     = FALSE;        //尚未开始列当前通话
        bMute               = FALSE;        //当前静音关闭
        bHolding            = FALSE;        //是否正在被Hold
        bKeyDown            = FALSE;        //当前没有按键按下
        bCanListCurCalling  = TRUE;         //是否可以在IDC_TIMER_ERROR消息中列当前通话
        //**************************************************************
        hMBList = CreateMBListControl(hWnd,IDC_MBLIST,0,0,
            GetScreenUsableWH1(SM_CXSCREEN),GetScreenUsableWH1(SM_CYSCREEN),
            WS_BORDER | WS_CHILD);//创建多方通话控件

        PostMessage(hWnd,MB_ACTIVE,0,0);

        //SaveLog("通话开始\r\n");
        break;

    case MB_ACTIVE://通话个数有变化的时候,需要发送这个消息,提示界面有变化
        SendDtmf(hWnd);//调用界面发送分机号

        nUsedItemNum = GetUsedItemNum();//得已用Item个数
        if (1 == nUsedItemNum)//单方通话
            CallingType = MBCALLINGONE;
        else if ((1 < nUsedItemNum) && (nUsedItemNum <= iMaxCount))//多方通话
            CallingType = MBCALLINGMULT;
        else//没有通话
            CallingType = MBCALLINNONE;

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("\nMB MB_ACTIVE,nUsedItemNum:%d\n",nUsedItemNum);
#endif
        DestroyViewList();
        MoveWindowToTop(hWnd);
        SetMenuSoftkey();//修改通话界面的按钮以及菜单
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("MB MB_ACTIVE,消息结束\r\n");
#endif
        break;

    case WM_ACTIVATE:
        if (WA_INACTIVE != LOWORD(wParam))
        {
            if (IsWindow(hMBList) && (IsWindowVisible(hMBList)))
                SetFocus(hMBList);
            else
                SetFocus(hWnd);//激活后设置焦点
        }
        break;

    case WM_TIMER:
        switch (wParam)
        {
        case IDC_TIMER_INIT://初始化信息的计时器
            KillTimer(hWnd,IDC_TIMER_INIT);
            ME_GetACMmax(hWnd,IDC_ME_MAXPRICE);//获取最大累计Call Meter值
            break;

        case IDC_TIMER_CALLING://通话计时的计时器
            ChangeTimeOnTimer();//改变时间
            break;

        case IDC_TIMER_COST://通话计费的计时器
            if (!bCanGetCurrentCCM)
                break;
            bCanGetCurrentCCM = FALSE;//不可以发送消息读取当前计费信息
            ME_GetCCM(hWnd,IDC_ME_GETCCM);//获取当前Call Meter值
            break;

#if defined(KEYSWITCHTYPE_LONG)
        case IDC_TIMER_LONGKEY://长按数字键0的计时器
            KillTimer(hWnd,IDC_TIMER_LONGKEY);
            SuperAddDialNumber(pPlus,TRUE,TRUE);//输入加号
            break;
#endif

        case IDC_TIMER_ERROR://出错处理的计时器
            if (bListCurCalling)//正在列当前通话
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
                printf("mb MBCalling 正在列当前通话\r\n");
#endif
                break;
            }
            if (!bCanListCurCalling)//不可以在IDC_TIMER_ERROR消息中列当前通话
                break;
            if (IsWindow_DialLink() || IsWindow_MBRing())
                break;
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling 定时列电话操作\r\n");
#endif
            if (-1 == ME_ListCurCalls(hCalling,IDC_ERROR_LISTCALLING))//列当前通话
            {
                bListCurCalling  = FALSE;
            }
            else
            {
                bListCurCalling  = TRUE;//正在列当前通话
                ListCurCallsType = LISTCURCALLS_REPEAT;
            }
            break;
        }
        break;

    case WM_DESTROY ://通知程序管理器,此程序退出;
        //SaveLog("通话结束\r\n");

#if defined(MBCALLING_RECORD)
        if (bRecord)//当前正在录音
            EndRecorderUnvisualable();//停止录音
#endif
        hCalling      = NULL;
        hMBList       = NULL;
        PhoneGroupNum = 0;//设置电话路数
        CallingType   = MBCALLINNONE;//没有通话
        bCostLimit    = FALSE;//设置当前通话是否已到话费限制
        RemovePortraitBitmap();//删除单方通话显示的头像

        KillTimer(hWnd,IDC_TIMER_CALLING);
        MBList_Destroy();//销毁多方通话的控件
        UnregisterClass(pClassName,NULL);
        DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)hInstance);
        EnableState();
        PhoneSetChannelCalling(FALSE);
        break;

    case WM_PAINT :
        hdc = BeginPaint( hWnd, NULL);
        CallingDisplay(hdc);
        EndPaint(hWnd, NULL);
        break;

    case WM_TODESKTOP:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("\nmb MBCalling WM_TODESKTOP,wParam:%ld\n",wParam);
#endif
        if (0 == wParam)//程序管理器自动发送
            return FALSE;
        if (!IsWindow_MBRing() &&   //来电界面
            !IsWindow_DialLink() && //连接界面
            !IsWindow_Dialup())     //拨号界面
            SendMessage(hWnd,WM_KEYDOWN,VK_F2,0);

        return FALSE;

#if defined(KEYSWITCHTYPE_LONG)
    case WM_KEYUP:
        switch(LOWORD(wParam))
        {
        case VK_0:  //按了数字键0
            KillTimer(hWnd,IDC_TIMER_LONGKEY);
            bKeyDown = FALSE;//当前没有按键按下
            SuperAddDialNumber(pNum0,TRUE,TRUE);//进行追加拨号
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
        case VK_UP://VK_PAGEUP://增大音量
            CallheadphoneWindow(1);//调整通话音量
            break;
        case VK_DOWN://VK_PAGEDOWN://减小音量
            CallheadphoneWindow(-1);//调整通话音量
            break;
        case VK_LEFT:
            if (MBCALLINNONE == CallingType)//当前是通话结束界面则退出
                DestroyWindow(hWnd);
            break;
        case VK_RIGHT:
            if (MBCALLINNONE == CallingType)//当前是通话结束界面则退出
                DestroyWindow(hWnd);
            break;

#if defined(KEYSWITCHTYPE_LONG)
        case VK_0:  //按了数字键0,长按数字键0的时候,需要输入加号
            if (MBCALLINNONE == CallingType)//当前是通话结束界面则退出
            {
                DestroyWindow(hWnd);
                break;
            }
            //由于长按零键要求输入加号,所以0键按下的时候需要启动计时器,当计时器到达
            //的时候,认为用于需要输入加号.当计时器没有到达的时候,输入0
            if (bKeyDown)//当前有按键按下
                break;
            bKeyDown = TRUE;//当前有按键按下
            KillTimer(hWnd,IDC_TIMER_LONGKEY);
            SetTimer(hWnd,IDC_TIMER_LONGKEY,TIMER_LONGKEY,NULL);//启动长按键0的计时器
            break;
#endif

#if defined(KEYSWITCHTYPE_SHORT)
        case VK_0:
#endif
        case VK_1:  //按了数字键1
        case VK_2:  //按了数字键2
        case VK_3:  //按了数字键3
        case VK_4:  //按了数字键4
        case VK_5:  //按了数字键5
        case VK_6:  //按了数字键6
        case VK_7:  //按了数字键7
        case VK_8:  //按了数字键8
        case VK_9:  //按了数字键9
            if (MBCALLINNONE == CallingType)//当前是通话结束界面则退出
            {
                DestroyWindow(hWnd);
            }
            else
            {
                char chSend[2] = "";//追加拨号所需数组
                chSend[0] = (LOWORD(wParam) - VK_0 + 48);
                SuperAddDialNumber(chSend,TRUE,TRUE);//进行追加拨号
            }
            break;
        case KEYASTERICK: //*
            if (MBCALLINNONE == CallingType)//当前是通话结束界面则退出
                DestroyWindow(hWnd);
            else
                SuperAddDialNumber(pAsterisk,TRUE,TRUE);//进行追加拨号"*"
            break;
        case KEYWELL: //#
            if (MBCALLINNONE == CallingType)//当前是通话结束界面则退出
                DestroyWindow(hWnd);
            else
                SuperAddDialNumber(pWell,TRUE,TRUE);//进行追加拨号"#"
            break;

        case VK_F1://拨号键
            switch (CallingType)
            {
            case MBCALLINNONE:
                DestroyWindow(hWnd);
                break;
            case MBCALLINGONE:
                SendMessage(hWnd,WM_COMMAND,IDM_ONE_HOLD,0);
                break;
            case MBCALLINGMULT:
                SendMessage(hWnd,WM_COMMAND,IDM_SWITCH2,0);
                break;
            }
            break;

#if defined(MBCALLING_RECORD)
        case VK_RETURN:
            SendMessage(hWnd,WM_COMMAND,IDM_RECORD,0);
            break;
#endif

        case VK_FLIP_OFF://合盖挂断
            if (MBCongig_Calling.bModeTurn)//合盖挂断
                ExecuteHangup();//处理挂断键的操作
            break;
        case VK_F10:
        case VK_F2://挂断
        case VK_EARPHONE_KEY://耳机上的按键
            ExecuteHangup();//处理挂断键的操作
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        //***********按钮部分开始****************
#if defined(MBCALLING_RECORD)
        case IDC_BUTTON_RECORD://录音按钮
            switch(HIWORD(wParam))
            {
            case BN_CLICKED:
                SendMessage(hWnd,WM_COMMAND,IDM_RECORD,0);
                break;
            }
            break;
#endif
        case IDC_BUTTON_MIDDLE://中键,拨号
            SuperAddDialNumber(NULL,TRUE,TRUE);
            break;
        case IDC_RBUTTON://挂断
            SendMessage(hWnd,WM_KEYDOWN,VK_F2,0);
            break;
        //***********按钮部分结束****************
        //***********菜单部分开始****************
        case IDM_MUTE://静音
            //bMute=FALSE表示当前静音关闭需要打开静音
            //bMute=TRUE表示当前静音打开需要关闭静音
            if (-1 == ME_MuteControl(hWnd,IDC_ME_MUTE,!bMute))
            {
                AppMessageBox(hWnd,bMute ? MUTEFAIL : UNMUTEFAIL,TITLECAPTION,WAITTIMEOUT);
            }
            else
            {
                bCanListCurCalling = FALSE;//不可以在IDC_TIMER_ERROR消息中列当前通话
                WaitWindowState(hWnd,TRUE);//MBWaitWindowState(hWnd,TRUE,NULL,FALSE);
            }
            break;

#if defined(MBCALLING_RECORD)
        case IDM_RECORD://录音
            if (bRecord)//正在录音,按钮上文字应该为"停止录音",点击按钮之后变为录音
            {
                SetWindowText(hBRecord,TXT_RECORD);
                hMenu = PDAGetMenu(hWnd);
                ModifyMenu(hMenu,IDM_RECORD,MF_BYCOMMAND|MF_ENABLED,IDM_RECORD,TXT_RECORD);
                EndRecorderUnvisualable();
                bRecord = FALSE;
                if (!IsSpareTimeLeft())//录音时间已满
                    EnableWindow(hBRecord,FALSE);
            }
            else//尚未录音,按钮上文字应该为"录音",点击按钮之后变为停止录音
            {
                if (StartRecorderUnvisualable(hWnd,IDC_RECORD_OVER))
                {
                    SetWindowText(hBRecord,TXT_STOPRECORD);
                    hMenu = PDAGetMenu(hWnd);
                    ModifyMenu(hMenu,IDM_RECORD,MF_BYCOMMAND|MF_ENABLED,IDM_RECORD,TXT_STOPRECORD);
                    bRecord = TRUE;
                }
                else
                {
                    AppMessageBox(hWnd,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);
                }
            }
            break;
#endif
        case IDM_VIEW://查看
            MBViewList(hWnd);
            break;
        //*********************************************************************
        case IDM_ONE_HOLD://保持/激活(菜单中,单方通话)
            if (-1 == ME_SwitchCallStatus(hCalling,IDC_ONE_SWITCH,ACTIVE_SWITCH,0))
                AppMessageBox(hCalling,SWITCHFAIL,TITLECAPTION,WAITTIMEOUT);
            else
                WaitWindowState(hWnd,TRUE);
            break;

        //*********************************************************************
        case IDM_SWITCH1://切换(电话会议1,1个电话激活,1个电话保持)
            if (-1 == ME_SwitchCallStatus(hCalling,IDC_MULTME_SWITCH1,ACTIVE_SWITCH,0))
                AppMessageBox(hCalling,SWITCHFAIL,TITLECAPTION,WAITTIMEOUT);
            else
                WaitWindowState(hWnd,TRUE);
            break;

        case IDM_SWITCH2://保持(电话会议2,全部激活/全部保持的情况)
            if (-1 == ME_SwitchCallStatus(hCalling,IDC_MULTME_SWITCH2,ACTIVE_SWITCH,0))
                AppMessageBox(hCalling,SWITCHFAIL,TITLECAPTION,WAITTIMEOUT);
            else
                WaitWindowState(hWnd,TRUE);
            break;

        //**********************************************************************
        case IDM_HANGUPACTIVE://挂断激活的电话
            if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_CUT_ACTIVED,CUT_ACTIVED,0))
                AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
            break;

        case IDM_HANGUPHOLD://挂断保持的通话
            if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_CUT_HELD,CUT_HELD,0))
                AppMessageBox(hWnd,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
            break;

        case IDM_ALLHANGUP://全部挂断
            if (-1 == ME_SwitchCallStatus(hCalling,IDC_MULTME_ALLHANGUP,CUT_ALL,0))
                AppMessageBox(hWnd,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
            break;

        case IDM_ALLACTIVE://全部激活
            if (-1 == ME_SwitchCallStatus(hCalling,IDC_MULTME_ALLACTIVE,ADD_TO_ACTIVE,0))
                AppMessageBox(hCalling,ACTIVEFAIL,TITLECAPTION,WAITTIMEOUT);
            break;

        case IDM_SHIFT://转移
            if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_CALL_TRANSFER,CALL_TRANSFER,0))
                AppMessageBox(hCalling,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//请求失败
            break;
        //*******************************************************************
        case IDM_SINGLECALL://单独通话
            bSelectSingleConnect = FALSE;//尚未选择单独通话的按钮
            SetMBCallingMenu(INTERFACE_MULT5);
            AddMultCallInfo();
            MBList_Show(SW_SHOW);
            MBList_SetStyle(MBLIST_WS_ENABLED);
            MBList_SetCurSel(0);
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_LBUTTON_CONNECT,1),(LPARAM)TXT_CONNECT);
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_MIDDLE,2),(LPARAM)"");
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_RBUTTON_CANCEL,(LPARAM)TXT_CANCEL);
            SetWindowText(hWnd,TXT_SINGLECALL);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling 单独通话 界面完成\r\n");
#endif
            break;

        case IDM_SINGLEHANGUP://单独挂断
            SetMBCallingMenu(INTERFACE_MULT6);
            AddMultCallInfo();
            MBList_Show(SW_SHOW);
            MBList_SetStyle(MBLIST_WS_ENABLED);
            MBList_SetCurSel(0);
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_LBUTTON_HANGUP,1),(LPARAM)TXT_HANGUP);
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_MIDDLE,2),(LPARAM)"");
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_RBUTTON_CANCEL,(LPARAM)TXT_CANCEL);
            SetWindowText(hWnd,TXT_SINGLEHANGUP);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling 单独挂断 界面完成\r\n");
#endif
            break;

        case IDC_LBUTTON_CONNECT://单独通话
            if (bSelectSingleConnect)//已经选择了单独通话的按钮
            {
                AppMessageBox(hWnd,DIALLINKINGWAIT,TITLECAPTION, WAITTIMEOUT);
                break;
            }
            iItemData = MBList_GetItem(MBList_GetCurSel());
            //获得当前控件中选中项目的索引在结构数组中的位置
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling 单独通话 iItemData:%d\r\n",iItemData);
#endif
            if ((iItemData < 0) || (iItemData > iMaxCount - 1))
                iItemData = 0;
            if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_ACTIVE_SWITCH_X,
                            ACTIVE_SWITCH_X,PhoneItem[iItemData].Calls_Info.index))
                AppMessageBox(hWnd,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling 单独通话 iItemData:%d,index:%d\r\n",
                iItemData,PhoneItem[iItemData].Calls_Info.index);
#endif
            bSelectSingleConnect = TRUE;//选择了单独通话的按钮
            break;

        case IDC_LBUTTON_HANGUP://单独挂断
            iItemData = MBList_GetItem(MBList_GetCurSel());
            //获得当前控件中选中项目的索引在结构数组中的位置
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling 单独挂断 iItemData:%d\r\n",iItemData);
#endif
            if ((iItemData < 0) || (iItemData > iMaxCount - 1))
                iItemData = 0;
            if (-1 == ME_SwitchCallStatus(hWnd,IDC_ME_CUT_ACTIVED_X,//挂断选中的电话
                            CUT_ACTIVED_X,PhoneItem[iItemData].Calls_Info.index))
                AppMessageBox(hWnd,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling 单独挂断 iItemData:%d,index:%d\r\n",
                iItemData,PhoneItem[iItemData].Calls_Info.index);
#endif
            break;

        case IDC_RBUTTON_CANCEL://单独通话/单独挂断的取消按钮
            SendMessage(hWnd,MB_ACTIVE,0,0);
            break;

        case IDC_LBUTTON_NONE://显示通话结束界面时候的确定按钮
        case IDC_RBUTTON_BACK://显示通话结束界面时候的退出按钮
            DestroyWindow(hWnd);
            break;
        }
        break;
    //*****************ME回调消息**********************
    //********************开始处理通话计费信息**********************
    case IDC_ME_MAXPRICE://查询最大累计Call Meter值的消息
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            ME_GetResult(&dAcmMax,sizeof(DWORD));
            if ((dAcmMax > 0) && (dAcmMax < 0xffffff))//设置通话限额
            {
                if (-1 == ME_GetPricePerUnit(hWnd,IDC_ME_GETPRICEUNIT))//获取计价信息
                    bAOCInfo = FALSE;
            }
            break;
        default:
            bAOCInfo = FALSE;
            break;
        }
        break;

    case IDC_ME_GETPRICEUNIT://获取计价信息
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling IDC_ME_GETPRICEUNIT wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            GetPriceUnit();//获取计价信息之后的处理过程
            break;
        default:
            bAOCInfo = FALSE;
            break;
        }
        break;

    case IDC_ME_GETCCM://获取当前Call Meter值
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling IDC_ME_GETCCM wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            if (-1 == ME_GetResult(&dCostInfo,sizeof(DWORD)))
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
                printf("mb MBCalling IDC_ME_GETCCM ME_GetResult==-1\r\n");
#endif
            }
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling dCostInfo:%ld\r\n",dCostInfo);
#endif
            bCanGetCurrentCCM = TRUE;//可以发送消息读取当前计费信息
            InvalidateRect(hWnd,&rCostInfo,TRUE);
            break;
        default:
            SendAbortCommand(wParam,lParam);//发送取消操作的at命令
            dCostInfo = 0;
            break;
        }        
        break;
    //********************通话计费信息处理完毕**********************    
    case IDC_ME_MUTE://静音的回调消息(单方通话/多方通话)
        bCanListCurCalling = TRUE;//可以在IDC_TIMER_ERROR消息中列当前通话
        WaitWindowState(hWnd,FALSE);//MBWaitWindowState(hWnd,FALSE,NULL,FALSE);
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            bMute = !bMute;
            hMenu = PDAGetMenu(hWnd);
            ModifyMenu(hMenu,IDM_MUTE,MF_BYCOMMAND|MF_ENABLED,IDM_MUTE,
                bMute ? TXT_UNMUTE : TXT_MUTE);

            InvalidateRect(hWnd,NULL,TRUE);
            AppMessageBox(hWnd,TXT_FINISH,TITLECAPTION,WAITTIMEOUT);
            break;
        default:
            SendAbortCommand(wParam,lParam);//发送取消操作的at命令
            AppMessageBox(hWnd,bMute ? UNMUTEFAIL : MUTEFAIL,TITLECAPTION,WAITTIMEOUT);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling 执行静音失败,bMute:%d,wParam:%ld,lParam:%ld\r\n",
                bMute,wParam,lParam);
#endif
            break;
        }
        break;

    case IDC_ME_LISTCURCALLS://列当前通话的回调消息
        MBWaitWindowState(hWnd,FALSE,NULL,FALSE);
        bListCurCalling = FALSE;
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            if (OperateListCurCalls())//列当前通话之后执行的操作
                SendMessage(hWnd,MB_ACTIVE,0,0);//重列电话之后,需要更新界面

            break;
        default:
            SendAbortCommand(wParam,lParam);//发送取消操作的at命令
            AppMessageBox(hCalling,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);
            break;
        }
        break;

    case IDC_ERROR_LISTCALLING://出错处理的回调消息
        bListCurCalling = FALSE;
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            OperateListCurCalls();
            break;
        }
        break;

    //*************************有关切换的操作开始*************
    case IDC_ME_SHORTCUTSWITCH://切换的回调消息,用于快捷操作
        switch (wParam)
        {
        case ME_RS_SUCCESS://由于切换之后可能会接入新的电话,所以需要重列当前通话
            MBListCurCalls(LISTCURCALLS_SWITCH,wParam,lParam);
            break;
        default:
            SendAbortCommand(wParam,lParam);//发送取消操作的at命令
            AppMessageBox(hWnd,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);
            break;
        }
        break;

    case IDC_ONE_SWITCH://在保持和激活的通话状态间切换(单方通话)
        WaitWindowState(hWnd,FALSE);
        switch (wParam)
        {
        case ME_RS_SUCCESS://单方通话中执行切换操作不会导致界面改变
            bHolding = !bHolding;
            hMenu = PDAGetMenu(hWnd);//设置菜单
            ModifyMenu(hMenu,IDM_ONE_HOLD,MF_BYCOMMAND|MF_ENABLED,IDM_ONE_HOLD,
                bHolding ? TXT_RESTORE : TXT_HOLD);

            InvertItemStateOne(iCallingOneNum);//结构数组中进行状态切换
            AppMessageBox(hWnd,TXT_FINISH,TITLECAPTION,WAITTIMEOUT);
            break;
        default:
            SendAbortCommand(wParam,lParam);//发送取消操作的at命令            
            AppMessageBox(hWnd,SWITCHFAIL,TITLECAPTION,WAITTIMEOUT);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling 单方通话切换失败,bHolding:%d,wParam:%ld,lParam:%ld\r\n",
                bHolding,wParam,lParam);
#endif
            break;
        }
        break;

    case IDC_MULTME_SWITCH1://多方通话的切换消息,1个电话激活,1个电话保持        
        WaitWindowState(hWnd,FALSE);
        switch (wParam)
        {
        case ME_RS_SUCCESS://1个电话激活,1个电话保持的时候执行切换操作不会导致界面改变
            InvertItemStateMult();//激活与保持交换
            AppMessageBox(hWnd,TXT_FINISH,TITLECAPTION,WAITTIMEOUT);
            break;
        default:
            SendAbortCommand(wParam,lParam);//发送取消操作的at命令
            AppMessageBox(hWnd,SWITCHFAIL,TITLECAPTION,WAITTIMEOUT);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling 多方通话切换失败,wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
            break;
        }
        break;

    case IDC_MULTME_SWITCH2://多方通话的切换消息(全部为激活通话/全部为保持通话)
        WaitWindowState(hWnd,FALSE);
        switch (wParam)
        {
        case ME_RS_SUCCESS://导致界面改变
            InvertItemStateMult();//激活与保持交换
            SendMessage(hWnd,MB_ACTIVE,0,0);
            AppMessageBox(hWnd,TXT_FINISH,TITLECAPTION,WAITTIMEOUT);
            break;
        default:
            SendAbortCommand(wParam,lParam);//发送取消操作的at命令
            AppMessageBox(hWnd,SWITCHFAIL,TITLECAPTION,WAITTIMEOUT);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling 多方通话切换失败,wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
            break;
        }
        break;
    //*************************有关切换的操作结束*************
    //*************************有关挂断的操作开始*************
    case WM_VOICE_HANGUPONE://单方通话的挂断
        MBListCurCalls(LISTCURCALLS_HANGUP_ONE,wParam,lParam);//列当前通话
        break;
    case IDC_MULTME_ALLHANGUP://多方通话的全部挂断消息
        MBListCurCalls(LISTCURCALLS_HANGUP_ALL,wParam,lParam);//列当前通话
        break;
    case IDC_ME_CUT_HELD://挂断保持的通话(多方通话)
        MBListCurCalls(LISTCURCALLS_HANGUP_HELD,wParam,lParam);//列当前通话
        break;
    case IDC_ME_CUT_ACTIVED://挂断激活的通话(多方通话)
        MBListCurCalls(LISTCURCALLS_HANGUP_ACTIVE,wParam,lParam);//列当前通话
        break;
    case IDC_ME_CUT_ACTIVED_X://挂断指定序号的激活通话
        MBListCurCalls(LISTCURCALLS_HANGUP_CUT_X,wParam,lParam);//列当前通话
        break;
    //*************************有关挂断的操作结束*************
    case IDC_MULTME_ALLACTIVE://多方通话的全部激活消息
        switch (wParam)
        {
        case ME_RS_SUCCESS://导致界面改变
            PhoneGroupNum = 1;
            SetAllType(CALL_STAT_ACTIVE);
            SendMessage(hWnd,MB_ACTIVE,0,0);
            AppMessageBox(hWnd,TXT_FINISH,TITLECAPTION,WAITTIMEOUT);
            break;
        default:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling 方通话的全部激活失败,wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
            SendAbortCommand(wParam,lParam);//发送取消操作的at命令
            AppMessageBox(hWnd,ACTIVEFAIL,TITLECAPTION,WAITTIMEOUT);
            break;
        }
        break;

    case IDC_ME_ACTIVE_SWITCH_X://将除指定序号外的所有通话变为保持
        bSelectSingleConnect = FALSE;//尚未选择单独通话的按钮
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            MBListCurCalls(LISTCURCALLS_ACTIVE_X,wParam,lParam);//重列当前通话
            break;
        default:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling 将除指定序号外的所有通话变为保持失败,wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
            SendAbortCommand(wParam,lParam);//发送取消操作的at命令
            AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//请求失败
            break;
        }
        break;

    case IDC_ME_CALL_TRANSFER://通话中当已有两路电话时,使这两路电话连起,用户从通话中退出
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            MBListCurCalls(LISTCURCALLS_TRANSFER,wParam,lParam);//列当前通话
            break;
        default:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling 转移失败wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
            SendAbortCommand(wParam,lParam);//发送取消操作的at命令
            AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//请求失败
            break;
        }
        break;

    case IDC_ME_REPEAT://重列当前通话
        MBListCurCalls(LISTCURCALLS_REPEAT,wParam,lParam);
        break;

    case IDC_ME_SHORTOTHER://其它的快捷操作
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            break;
        default:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling IDC_ME_SHORTOTHER失败wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
            SendAbortCommand(wParam,lParam);//发送取消操作的at命令
            AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//请求失败
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
* Function     InitMBCalling
* Purpose      初始化通话界面所需图片信息
* Params       
* Return           
* Remarks      单方通话初始化数据
**********************************************************************/
void    InitMBCalling(void)
{
    int iFontHeight,iSW;

    iFontHeight = GetCharactorHeight(NULL);//文字高度
    iSW         = GetScreenUsableWH1(SM_CXSCREEN);

    SetRect(&rPhoto,iPortraitX,iPortraitY,
        iPortraitX + iPortraitW,iPortraitY + iPortraitH);

    SetRect(&rName      ,rPhoto.right,rPhoto.top,
        iSW,rPhoto.top + iFontHeight * 2);
    SetRect(&rPhoneCode ,rPhoto.right,rName.bottom + iFontHeight ,
        iSW,rName.bottom + iFontHeight * 3);
    SetRect(&rTime      ,rPhoto.left ,rPhoto.bottom + iFontHeight,
        iSW,rPhoto.bottom + iFontHeight * 2);
    SetRect(&rCostInfo  ,rPhoto.left ,rTime.bottom + iFontHeight ,
        iSW,rTime.bottom + iFontHeight * 2);
}
/********************************************************************
* Function     GetPriceUnit
* Purpose      获取计价信息之后的处理过程
* Params       
* Return           
* Remarks      
**********************************************************************/
static  void    GetPriceUnit(void)
{
    if (-1 == ME_GetResult(&me_pucinfo,sizeof(ME_PUCINFO)))
    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling IDC_ME_GETPRICEUNIT ME_GetResult==-1\r\n");
#endif
        bAOCInfo = FALSE;
    }
    else
    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling,me_pucinfo.currency:%s,me_pucinfo.ppu:%s\r\n",
            me_pucinfo.currency,me_pucinfo.ppu);
#endif
        TrimString(me_pucinfo.currency);
        TrimString(me_pucinfo.ppu);
        if ((strlen(me_pucinfo.currency) == 0) && (strlen(me_pucinfo.ppu) == 0))
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling,没有设置计价信息\r\n");
#endif
            bAOCInfo = FALSE;
        }
        else
        {
            bAOCInfo          = TRUE;//显示通话计费信息
            bCanGetCurrentCCM = TRUE;//可以发送消息读取当前计费信息
            SetTimer(hCalling,IDC_TIMER_COST,TIMER_CALLING_COST,NULL);//启动通话计费的计时器
            if (strlen(me_pucinfo.currency) > 0)//设置了计价信息,ppu 乘以从ME_GetCCM中获得的值
            {
                CostType = COSTTYPE_PRICE;
                if (StrIsDouble(me_pucinfo.ppu))
                    dCost = MB_atof(me_pucinfo.ppu);
                else
                    dCost = dZero;//0.0
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
                printf("mb MBCalling,设置了计价信息,%f\r\n",dCost);
#endif
            }
            else//显示,用ME_GetCCM函数读取,直接显示
            {
                CostType = COSTTYPE_UNIT;
                dCost    = dZero;//0.0
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
                printf("mb MBCalling,从ME_GetCCM函数读取\r\n");
#endif
            }
        }
    }
}
/********************************************************************
* Function     ChangeTimeOnTimer
* Purpose      单方通话响应TIMER改变时间
* Params       
* Return           
* Remarks      
**********************************************************************/
static  void    ChangeTimeOnTimer(void)
{
    int i,iTimeMargin;
#if defined(MBCALLING_TIMEACCURATE)
    SYSTEMTIME sCurtime;
    GetLocalTime(&sCurtime);//首先获得当前时间
    if (sCurtime.wSecond == iInitSecond)//时间没有改变
        return;

    if (sCurtime.wSecond > iInitSecond)//计算时间间隔
        iTimeMargin = sCurtime.wSecond - iInitSecond;
    else
        iTimeMargin = iSecond60 - iInitSecond + sCurtime.wSecond;

    iInitSecond = sCurtime.wSecond;//重新保存开始计时的时间
#endif
#if defined(MBCALLING_TIMESIMPLE)
    iTimeMargin = 1;
#endif

    switch (CallingType)
    {
    case MBCALLINGONE://单方通话
        PhoneItem[iCallingOneNum].TimeUsed.wSecond += iTimeMargin;
        if (PhoneItem[iCallingOneNum].TimeUsed.wSecond >= iSecond60)
        {
            PhoneItem[iCallingOneNum].TimeUsed.wSecond = 0;
            PhoneItem[iCallingOneNum].TimeUsed.wMinute++;
            if (PhoneItem[iCallingOneNum].TimeUsed.wMinute >= iSecond60)
            {
                PhoneItem[iCallingOneNum].TimeUsed.wHour++;
                PhoneItem[iCallingOneNum].TimeUsed.wMinute = 0;
            }
        }
        if (MBCongig_Calling.b50Tip)//播放50秒提示音
        {
            if (((iTime_Second + 10) % iSecond60)== 0)//播放50秒提示音
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
                printf("mb MBCalling 播放50秒提示音,iTime_Second:%d\r\n",iTime_Second);
#endif
                PlayClewMusic(1);
            }
            iTime_Second++;
        }
        InvalidateRect(hCalling,&rTime ,TRUE);//刷新通话界面的时间
        InvalidateRect(hCalling,&rPhoto,TRUE);//刷新通话界面的图标
        break;
    case MBCALLINGMULT://多方通话
        for (i = 0;i < iMaxCount;i++)
        {
            if ((ITEMUSED == PhoneItem[i].ItemStat) && (0 != PhoneItem[i].Calls_Info.index))
            {
                PhoneItem[i].TimeUsed.wSecond += iTimeMargin;
                if (PhoneItem[i].TimeUsed.wSecond >= iSecond60)
                {
                    PhoneItem[i].TimeUsed.wSecond = 0;
                    PhoneItem[i].TimeUsed.wMinute++;
                    if (PhoneItem[i].TimeUsed.wMinute >= iSecond60)
                    {
                        PhoneItem[i].TimeUsed.wHour++;
                        PhoneItem[i].TimeUsed.wMinute = 0;
                    }
                }
            }
        }
        switch (InterfaceType)
        {
        case INTERFACE_MULT1://多方通话菜单1(1个电话激活,1个电话保持)
        case INTERFACE_MULT31://多方通话菜单3(1个激活通话,其余保持)
        case INTERFACE_MULT32://多方通话菜单3(1个保持通话,其余激活)
        case INTERFACE_MULT5://单独通话
        case INTERFACE_MULT6://单独挂断
            InvalidateRect(hCalling,NULL,TRUE);
            break;
        case INTERFACE_MULT21://多方通话菜单2(全部为激活通话)
        case INTERFACE_MULT22://多方通话菜单2(全部为保持通话)
            break;
        }
        break;
    }
}
/********************************************************************
* Function   RemovePortraitBitmap  
* Purpose    删除单方通话显示的头像
* Params    
* Return    
* Remarks   
**********************************************************************/
static  void    RemovePortraitBitmap(void)
{
    if (hPortrait != NULL)
    {
        DeleteObject(hPortrait);
        hPortrait = NULL;
    }
}
/********************************************************************
* Function   SetMenuSoftkey  
* Purpose    修改通话界面的按钮以及菜单
* Params    
* Return    
* Remarks   
**********************************************************************/
static  BOOL    SetMenuSoftkey(void)
{
    HMENU       hMenu;
    int         iActiveNum,iHoldNum;
    HDC         hdc;
    PBINFO      pbNameIcon;
    COLORREF    Color;
    BOOL        bTran;

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling SetMenuSoftkey:%d\r\n",CallingType);
#endif
    switch (CallingType)
    {
    case MBCALLINNONE://没有通话
#if defined(MBCALLING_RECORD)
        ShowWindow(hBRecord,FALSE);
#endif
        SetMBCallingMenu(INTERFACE_NONE);//没有通话
        SendMessage(hCalling,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_LBUTTON_NONE,1),(LPARAM)"");
        SendMessage(hCalling,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_MIDDLE,2),(LPARAM)"");
        SendMessage(hCalling,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_RBUTTON_BACK,(LPARAM)TXT_BACK);
        SetWindowText(hCalling,CALLEND);//修改标题
        break;

    case MBCALLINGONE://单方通话
#if defined(MBCALLING_RECORD)
        ShowWindow(hBRecord,TRUE);
#endif
        MBList_ResetItem();//删除多方通话所有节点
        MBList_Show(SW_HIDE);//隐藏多方通话控件
#if defined(SUPPORTMIDDLEBUTTON)
        SendMessage(hCalling,PWM_CREATECAPTIONBUTTON,//拨号
            MAKEWPARAM(IDC_BUTTON_MIDDLE,2),(LPARAM)TXT_DIALNUMBER);
#endif
        SendMessage(hCalling,PWM_CREATECAPTIONBUTTON,//挂断
            (WPARAM)IDC_RBUTTON,(LPARAM)TXT_HANGUP);

        SetWindowText(hCalling,TITLECAPTION);//通话

        SetMBCallingMenu(INTERFACE_ONE);//单方通话

        iCallingOneNum = GetTheNUsedItem(1);
        if (-1 == iCallingOneNum)
            break;

        hMenu = PDAGetMenu(hCalling);
        if (PhoneItem[iCallingOneNum].Calls_Info.Stat == CALL_STAT_ACTIVE)
        {   //正在激活,文字变为"保持"
            ModifyMenu(hMenu,IDM_ONE_HOLD,MF_BYCOMMAND|MF_ENABLED,IDM_ONE_HOLD,TXT_HOLD);
            bHolding = FALSE;
        }
        else if (PhoneItem[iCallingOneNum].Calls_Info.Stat == CALL_STAT_HELD)//正在保持,文字变为"激活"
        {
            ModifyMenu(hMenu,IDM_ONE_HOLD,MF_BYCOMMAND|MF_ENABLED,IDM_ONE_HOLD,TXT_RESTORE);
            bHolding = TRUE;
        }
        PDADrawMenuBar(hCalling);

        hdc = GetDC(hCalling);
        if (MB_GetInfoFromPhonebook(PhoneItem[iCallingOneNum].Calls_Info.PhoneNum,&pbNameIcon))
        {   //不是陌生人
            if (FileIfExist(pbNameIcon.Icon))//设置头像,设置组图标就显示头像
            {
                RemovePortraitBitmap();//删除单方通话显示的头像
                GetImageDimensionFromFile(pbNameIcon.Icon,&sPortrait);
                hPortrait = CreateBitmapFromImageFile(hdc,pbNameIcon.Icon,&Color,&bTran);
            }
            else
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
                printf("mb MBCalling 头像:%s没有找到\r\n",pbNameIcon.Icon);
#endif
                RemovePortraitBitmap();//删除单方通话显示的头像
            }
        }
        else
        {
            RemovePortraitBitmap();//删除单方通话显示的头像
        }
        memset(cPhone,0x00,sizeof(cPhone));
        memset(cName ,0x00,sizeof(cName ));
        StrAutoNewLine(hdc,PhoneItem[iCallingOneNum].cName,cName,&rName,DT_HCENTER|DT_CENTER);
        StrAutoNewLine(hdc,PhoneItem[iCallingOneNum].Calls_Info.PhoneNum,cPhone,&rPhoneCode,DT_HCENTER|DT_CENTER);

        ReleaseDC(hCalling,hdc);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling 单方通话生成号码:%s,原号码:%s\r\n",cPhone,PhoneItem[iCallingOneNum].Calls_Info.PhoneNum);
#endif
        break;

    case MBCALLINGMULT://多方通话
#if defined(MBCALLING_RECORD)
        ShowWindow(hBRecord,FALSE);
#endif
        SetWindowText(hCalling,TXT_MEETING);//电话会议

#if defined(SUPPORTMIDDLEBUTTON)
        SendMessage(hCalling,PWM_CREATECAPTIONBUTTON,//拨号
            MAKEWPARAM(IDC_BUTTON_MIDDLE,2),(LPARAM)TXT_DIALNUMBER);
#endif

        SendMessage(hCalling,PWM_CREATECAPTIONBUTTON,//挂断
            (WPARAM)IDC_RBUTTON,(LPARAM)TXT_HANGUP);

        iActiveNum  = GetItemTypeNum(CALL_STAT_ACTIVE);//激活的通话个数
        iHoldNum    = GetItemTypeNum(CALL_STAT_HELD);//保持的通话个数

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling 多方通话,iActiveNum:%d,iHoldNum:%d\r\n",iActiveNum,iHoldNum);
#endif
        if ((1 == iActiveNum) && (1 == iHoldNum))//电话会议1(1个电话激活,1个电话保持)
        {
            SetMBCallingMenu(INTERFACE_MULT1);
            if (AddMultCallInfo())//添加多方通话的数据到控件中
            {
                MBList_Show(SW_SHOW);//显示多方通话控件
                MBList_SetStyle(MBLIST_WS_DISABLE);//设置控件风格:禁止选择
            }
        }
        else if ((iActiveNum > 1) && (0 == iHoldNum))//电话会议2(全部是激活通话)
        {
            SetMBCallingMenu(INTERFACE_MULT21);
            if (AddMultCallInfo())
            {
                MBList_Show(SW_SHOW);
                MBList_SetStyle(MBLIST_WS_DISABLE);//设置控件风格:禁止选择
            }
        }
        else if ((0 == iActiveNum) && (iHoldNum > 1))//电话会议2(全部是保持通话)
        {
            SetMBCallingMenu(INTERFACE_MULT22);
            if (AddMultCallInfo())
            {
                MBList_Show(SW_SHOW);
                MBList_SetStyle(MBLIST_WS_DISABLE);//设置控件风格:禁止选择
            }
        }
        else if ((1 == iActiveNum) && (iHoldNum > 1))//电话会议3(1个激活通话,其余保持)
        {
            SetMBCallingMenu(INTERFACE_MULT31);//电话会议为保持
            if (AddMultCallInfo())
            {
                MBList_Show(SW_SHOW);
                MBList_SetStyle(MBLIST_WS_DISABLE);//设置控件风格:禁止选择
            }
        }
        else if ((iActiveNum > 1) && (1 == iHoldNum))//电话会议3(1个保持通话,其余激活)
        {
            SetMBCallingMenu(INTERFACE_MULT32);//电话会议为激活
            if (AddMultCallInfo())
            {
                MBList_Show(SW_SHOW);
                MBList_SetStyle(MBLIST_WS_DISABLE);//设置控件风格:禁止选择
            }
        }
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling 多方通话 SetMenuSoftkey函数\r\n");
#endif
        break;
    }
    InvalidateRect(hCalling,NULL,TRUE);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling SetMenuSoftkey函数正常退出\r\n");
#endif
    return TRUE;
}
/********************************************************************
* Function   SetMBCallingMenu  
* Purpose    设置菜单
* Params    
* Return    
* Remarks   
**********************************************************************/
static  void    SetMBCallingMenu(INTERFACE iMenuType)
{
    HMENU hMenu,hSubMenu,hSubMenu2;

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling SetMBCallingMenu,iMenuType:%d,InterfaceType:%d\r\n",iMenuType,InterfaceType);
#endif
    if (InterfaceType == iMenuType)//菜单类型没有改变
        return;

    hMenu = PDAGetMenu(hCalling);
    switch (iMenuType)
    {
    case INTERFACE_NONE://没有通话  
    case INTERFACE_MULT5://单独通话
    case INTERFACE_MULT6://单独挂断
        SendMessage(GetCapture(), WM_CANCELMODE, NULL, NULL);
        DestroyMenu(hMenu);
        PDASetMenu(hCalling,NULL);
        break;
    case INTERFACE_ONE://单方通话
        SendMessage(hCalling,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_LBUTTON_MENU,1),(LPARAM)TXT_MENU);
        if (NULL == hMenu)
            hMenu = LoadMenuIndirect((PMENUTEMPLATE)&MBCallingMenuTemplate);
        else
            MB_RemoveAllMenu(hMenu);//删除所有菜单

        if (NULL == hMenu)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling SetMBCallingMenu LoadMenuIndirect fail,hMenu == NULL,iMenuType:%d\r\n",iMenuType);
#endif
            break;
        }
        InsertMenu(hMenu,IDM_MUTE,MF_BYCOMMAND,IDM_ONE_HOLD,TXT_HOLD);//保持
        SetChangelessMenu(hMenu);
        PDASetMenu(hCalling,hMenu);
        break;
    case INTERFACE_MULT1://多方通话菜单1(1个电话激活,1个电话保持)
        SendMessage(hCalling,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_LBUTTON_MENU,1),(LPARAM)TXT_MENU);
        if (NULL == hMenu)
            hMenu = LoadMenuIndirect((PMENUTEMPLATE)&MBCallingMenuTemplate);
        else
            MB_RemoveAllMenu(hMenu);

        if (NULL == hMenu)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling SetMBCallingMenu LoadMenuIndirect fail,hMenu == NULL,iMenuType:%d\r\n",iMenuType);
#endif
            break;
        }
        hSubMenu = CreateMenu();
        InsertMenu(hMenu   ,IDM_MUTE,MF_BYCOMMAND         ,IDM_SWITCH1     ,TXT_SWITCH);       //切换
        InsertMenu(hMenu   ,IDM_MUTE,MF_BYCOMMAND|MF_POPUP,(DWORD)hSubMenu ,TXT_HANGUPMENU);   //挂断
        InsertMenu(hSubMenu,0       ,MF_BYPOSITION        ,IDM_HANGUPACTIVE,TXT_HANGUPACTIVE); //挂断激活的电话
        InsertMenu(hSubMenu,1       ,MF_BYPOSITION        ,IDM_HANGUPHOLD  ,TXT_HANGUPHOLD);   //结束保持通话
        InsertMenu(hSubMenu,2       ,MF_BYPOSITION        ,IDM_ALLHANGUP   ,TXT_ALLHANGUP);    //全部挂断
        InsertMenu(hMenu   ,IDM_MUTE,MF_BYCOMMAND         ,IDM_ALLACTIVE   ,TXT_ALLACTIVE);    //加入电话会议
        InsertMenu(hMenu   ,IDM_MUTE,MF_BYCOMMAND         ,IDM_SHIFT       ,TXT_SHIFT);        //转移
        SetChangelessMenu(hMenu);
        PDASetMenu(hCalling,hMenu);
        break;
    case INTERFACE_MULT21://多方通话菜单2(全部为激活通话)
        SendMessage(hCalling,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_LBUTTON_MENU,1),(LPARAM)TXT_MENU);
        if (NULL == hMenu)
            hMenu = LoadMenuIndirect((PMENUTEMPLATE)&MBCallingMenuTemplate);
        else
            MB_RemoveAllMenu(hMenu);

        if (NULL == hMenu)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling SetMBCallingMenu LoadMenuIndirect fail,hMenu == NULL,iMenuType:%d\r\n",iMenuType);
#endif
            break;
        }
        hSubMenu = CreateMenu();
        InsertMenu(hMenu    ,IDM_MUTE,MF_BYCOMMAND         ,IDM_SWITCH2     ,TXT_HOLD);         //保持
        InsertMenu(hMenu    ,IDM_MUTE,MF_BYCOMMAND|MF_POPUP,(DWORD)hSubMenu ,TXT_MEETING);      //电话会议
        InsertMenu(hSubMenu ,0       ,MF_BYPOSITION        ,IDM_SINGLECALL  ,TXT_SINGLECALL);   //单独通话
        InsertMenu(hSubMenu ,1       ,MF_BYPOSITION        ,IDM_SINGLEHANGUP,TXT_SINGLEHANGUP); //单独挂断
        SetChangelessMenu(hMenu);
        PDASetMenu(hCalling,hMenu);
        break;
    case INTERFACE_MULT22://多方通话菜单2(全部为保持通话)
        SendMessage(hCalling,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_LBUTTON_MENU,1),(LPARAM)TXT_MENU);
        if (NULL == hMenu)
            hMenu = LoadMenuIndirect((PMENUTEMPLATE)&MBCallingMenuTemplate);
        else
            MB_RemoveAllMenu(hMenu);

        if (NULL == hMenu)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling SetMBCallingMenu LoadMenuIndirect fail,hMenu == NULL,iMenuType:%d\r\n",iMenuType);
#endif
            break;
        }
        InsertMenu(hMenu,IDM_MUTE,MF_BYCOMMAND,IDM_SWITCH2,TXT_RESTORE);//激活
        SetChangelessMenu(hMenu);
        PDASetMenu(hCalling,hMenu);
        break;
    case INTERFACE_MULT31://多方通话菜单3(1个激活通话,其余保持)
        SendMessage(hCalling,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_LBUTTON_MENU,1),(LPARAM)TXT_MENU);
        if (NULL == hMenu)
            hMenu = LoadMenuIndirect((PMENUTEMPLATE)&MBCallingMenuTemplate);
        else
            MB_RemoveAllMenu(hMenu);

        if (NULL == hMenu)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling SetMBCallingMenu LoadMenuIndirect fail,hMenu == NULL,iMenuType:%d\r\n",iMenuType);
#endif
            break;
        }
        hSubMenu = CreateMenu();
        InsertMenu(hMenu   ,IDM_MUTE,MF_BYCOMMAND         ,IDM_SWITCH2     ,TXT_SWITCH);       //切换
        InsertMenu(hMenu   ,IDM_MUTE,MF_BYCOMMAND|MF_POPUP,(DWORD)hSubMenu ,TXT_HANGUPMENU);   //挂断
        InsertMenu(hSubMenu,0       ,MF_BYPOSITION        ,IDM_HANGUPACTIVE,TXT_HANGUPACTIVE); //挂断激活的电话
        InsertMenu(hSubMenu,1       ,MF_BYPOSITION        ,IDM_HANGUPHOLD  ,TXT_HANGUPHOLD);   //结束保持通话
        InsertMenu(hSubMenu,2       ,MF_BYPOSITION        ,IDM_ALLHANGUP   ,TXT_ALLHANGUP);    //全部挂断
        InsertMenu(hMenu   ,IDM_MUTE,MF_BYCOMMAND         ,IDM_ALLACTIVE   ,TXT_ALLACTIVE);    //全部激活
        InsertMenu(hMenu   ,IDM_MUTE,MF_BYCOMMAND         ,IDM_SHIFT       ,TXT_SHIFT);        //转移
        SetChangelessMenu(hMenu);
        PDASetMenu(hCalling,hMenu);
        break;
    case INTERFACE_MULT32://多方通话菜单3(1个保持通话,其余激活)
        SendMessage(hCalling,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_LBUTTON_MENU,1),(LPARAM)TXT_MENU);
        if (NULL == hMenu)
            hMenu = LoadMenuIndirect((PMENUTEMPLATE)&MBCallingMenuTemplate);
        else
            MB_RemoveAllMenu(hMenu);

        if (NULL == hMenu)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling SetMBCallingMenu LoadMenuIndirect fail,hMenu == NULL,iMenuType:%d\r\n",iMenuType);
#endif
            break;
        }
        hSubMenu  = CreateMenu();
        hSubMenu2 = CreateMenu();
        InsertMenu(hMenu   ,IDM_MUTE,MF_BYCOMMAND         ,IDM_SWITCH2     ,TXT_SWITCH);        //切换
        InsertMenu(hMenu   ,IDM_MUTE,MF_BYCOMMAND|MF_POPUP,(DWORD)hSubMenu ,TXT_HANGUPMENU);    //挂断
        InsertMenu(hSubMenu,0       ,MF_BYPOSITION        ,IDM_HANGUPACTIVE,TXT_HANGUPACTIVE);  //挂断激活的电话
        InsertMenu(hSubMenu,1       ,MF_BYPOSITION        ,IDM_HANGUPHOLD  ,TXT_HANGUPHOLD);    //结束保持通话
        InsertMenu(hSubMenu,2       ,MF_BYPOSITION        ,IDM_ALLHANGUP   ,TXT_ALLHANGUP);     //全部挂断

        InsertMenu(hMenu    ,IDM_MUTE,MF_BYCOMMAND|MF_POPUP,(DWORD)hSubMenu2,TXT_MEETING);       //电话会议
        InsertMenu(hSubMenu2,0       ,MF_BYPOSITION        ,IDM_ALLACTIVE   ,TXT_ALLACTIVE);     //全部激活
        InsertMenu(hSubMenu2,1       ,MF_BYPOSITION        ,IDM_SINGLEHANGUP,TXT_SINGLEHANGUP);  //单独挂断
        InsertMenu(hMenu    ,IDM_MUTE,MF_BYCOMMAND         ,IDM_SHIFT       ,TXT_SHIFT);         //转移
        SetChangelessMenu(hMenu);
        PDASetMenu(hCalling,hMenu);
        break;
    }
    PDADrawMenuBar(hCalling);
    InterfaceType = iMenuType;

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling SetMBCallingMenu 函数正常结束\r\n");
#endif
}
/********************************************************************
* Function   MB_RemoveAllMenu  
* Purpose    删除所有菜单
* Params    
* Return    
* Remarks   
**********************************************************************/
static  void    MB_RemoveAllMenu(HMENU hMenu)
{
    int iCount,i,j,iSubCount,iMenuID;
    HMENU hSubMenu;

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling MB_RemoveAllMenu开始删除菜单\r\n");
#endif
    iCount = GetMenuItemCount(hMenu);
    for (i = iCount - 1;i >= 0 ;i--)
    {
        hSubMenu = GetSubMenu(hMenu,i);
        if (hSubMenu != NULL)
        {
            iSubCount = GetMenuItemCount(hSubMenu);
            for (j = iSubCount - 1;j >= 0 ;j--)
            {
                iMenuID = GetMenuItemID(hSubMenu,j);
                if ((iMenuID == IDM_MUTE) || (iMenuID == IDM_VIEW) ||
#if defined(MBCALLING_RECORD)
                    (iMenuID == IDM_RECORD) || 
#endif
                    (iMenuID == -1))
                    continue;
                RemoveMenu(hSubMenu,j,MF_BYPOSITION);
            }
            RemoveMenu(hMenu,i,MF_BYPOSITION);
        }
        else
        {
            iMenuID = GetMenuItemID(hMenu,i);
            if ((iMenuID == IDM_MUTE) || (iMenuID == IDM_VIEW) ||
#if defined(MBCALLING_RECORD)
                (iMenuID == IDM_RECORD) || 
#endif
                (iMenuID == -1))
                continue;
            RemoveMenu(hMenu,i,MF_BYPOSITION);
        }
    }
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling MB_RemoveAllMenu菜单删除完毕\r\n");
#endif
}
/********************************************************************
* Function   SetChangelessMenu  
* Purpose    设置菜单的固定项
* Params    
* Return    
* Remarks   
**********************************************************************/
static  void    SetChangelessMenu(HMENU hMenu)
{
    ModifyMenu(hMenu,IDM_MUTE,MF_BYCOMMAND|MF_ENABLED,IDM_MUTE,
        bMute ? TXT_UNMUTE : TXT_MUTE);//静音
#if defined(MBCALLING_RECORD)
    ModifyMenu(hMenu,IDM_RECORD,MF_BYCOMMAND|MF_ENABLED,IDM_RECORD,
        bRecord ? TXT_STOPRECORD : TXT_RECORD);//录音
#endif
    ModifyMenu(hMenu,IDM_VIEW,MF_BYCOMMAND|MF_ENABLED,IDM_VIEW,TXT_VIEW);//查看
}
/********************************************************************
* Function   CallingDisplay  
* Purpose    通话界面的显示部分
* Params    
* Return    
* Remarks   
**********************************************************************/
static  void    CallingDisplay(HDC hdc)
{
    char    cLastTime[10] = "";
    char    cCostInfo[50] = "";
    int     i,iCalllingNum,iPicState;
    int     nDestX,nDestY,nDestW,nDestH;

    switch (CallingType)
    {
    case MBCALLINGONE://单方通话
        DrawText(hdc,cName ,strlen(cName) ,&rName     ,DT_VCENTER|DT_CENTER);//显示姓名
        DrawText(hdc,cPhone,strlen(cPhone),&rPhoneCode,DT_VCENTER|DT_CENTER);//显示号码

        if (hPortrait != NULL)//显示图片
        {
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
            StretchBlt(hdc,nDestX,nDestY,nDestW,nDestH,
                (HDC)hPortrait,0,0,sPortrait.cx,sPortrait.cy,SRCCOPY);
        }
        sprintf(cLastTime,pTime,
            PhoneItem[iCallingOneNum].TimeUsed.wHour,
            PhoneItem[iCallingOneNum].TimeUsed.wMinute,
            PhoneItem[iCallingOneNum].TimeUsed.wSecond);
        DrawText(hdc,cLastTime,strlen(cLastTime),&rTime,DT_VCENTER|DT_CENTER);//显示时间

        if (bAOCInfo)
        {
            switch (CostType)
            {
            case COSTTYPE_UNIT://单位
                sprintf(cCostInfo,pCostInfo2,dCostInfo);//"%ld"
                break;
            case COSTTYPE_PRICE://单价
                sprintf(cCostInfo,pCostInfo1,me_pucinfo.currency,dCostInfo * dCost);//"%s:%f"
                MB_DealDouble(cCostInfo,3);
                break;
            }
            DrawText(hdc,cCostInfo,strlen(cCostInfo),&rCostInfo,DT_VCENTER|DT_CENTER);
        }
        break;
    case MBCALLINGMULT://多方通话
        switch (InterfaceType)
        {
        case INTERFACE_MULT5://单独通话
        case INTERFACE_MULT6://单独挂断
        case INTERFACE_MULT1://多方通话菜单1(1个电话激活,1个电话保持)
            iCalllingNum = 0;//列表项的计数
            for (i = 0;i < iMaxCount;i++)
            {
                if ((ITEMUSED == PhoneItem[i].ItemStat) && (0 != PhoneItem[i].Calls_Info.index))
                {
                    if (CALL_STAT_ACTIVE == PhoneItem[i].Calls_Info.Stat)
                        iPicState = (bMute ? MBLIST_IPICSTATE_AM : MBLIST_IPICSTATE_A);
                    else if (CALL_STAT_HELD == PhoneItem[i].Calls_Info.Stat)
                        iPicState = (bMute ? MBLIST_IPICSTATE_HM : MBLIST_IPICSTATE_H);
                    else
                        iPicState = MBLIST_IPIC_UNCHANGE;//图片状态:没有改变

                    sprintf(cLastTime,pTime,
                        PhoneItem[i].TimeUsed.wHour, 
                        PhoneItem[i].TimeUsed.wMinute, 
                        PhoneItem[i].TimeUsed.wSecond);
                    MBList_SetItem(iCalllingNum,NULL,cLastTime,iPicState,-1,-1,FALSE);//设置通话计时
                    iCalllingNum++;
                }
            }
            break;
        case INTERFACE_MULT21://多方通话菜单2(全部为激活通话)
            iPicState = (bMute ? MBLIST_IPICSTATE_AM : MBLIST_IPICSTATE_A);
            MBList_SetItem(0,NULL,NULL,iPicState,-1,-1,FALSE);
            break;
        case INTERFACE_MULT22://多方通话菜单2(全部为保持通话)
            iPicState = (bMute ? MBLIST_IPICSTATE_HM : MBLIST_IPICSTATE_H);
            MBList_SetItem(0,NULL,NULL,iPicState,-1,-1,FALSE);
            break;
        case INTERFACE_MULT31://多方通话菜单3(1个激活通话,其余保持)
            for (i = 0;i < iMaxCount;i++)
            {
                if ((ITEMUSED == PhoneItem[i].ItemStat) && (0 != PhoneItem[i].Calls_Info.index))
                {
                    if (CALL_STAT_ACTIVE == PhoneItem[i].Calls_Info.Stat)
                    {
                        iPicState = (bMute ? MBLIST_IPICSTATE_AM : MBLIST_IPICSTATE_A);
                        sprintf(cLastTime,pTime,
                            PhoneItem[i].TimeUsed.wHour, 
                            PhoneItem[i].TimeUsed.wMinute, 
                            PhoneItem[i].TimeUsed.wSecond);
                        MBList_SetItem(0,NULL,cLastTime,iPicState,-1,-1,FALSE);//设置通话计时
                        break;
                    }
                }
            }
            iPicState = (bMute ? MBLIST_IPICSTATE_HM : MBLIST_IPICSTATE_H);
            MBList_SetItem(1,NULL,NULL,iPicState,-1,-1,FALSE);//电话会议为保持
            break;
        case INTERFACE_MULT32://多方通话菜单3(1个保持通话,其余激活)
            for (i = 0;i < iMaxCount;i++)
            {
                if ((ITEMUSED == PhoneItem[i].ItemStat) && (0 != PhoneItem[i].Calls_Info.index))
                {
                    if (CALL_STAT_HELD == PhoneItem[i].Calls_Info.Stat)
                    {
                        iPicState = (bMute ? MBLIST_IPICSTATE_HM : MBLIST_IPICSTATE_H);
                        sprintf(cLastTime,pTime,
                            PhoneItem[i].TimeUsed.wHour, 
                            PhoneItem[i].TimeUsed.wMinute, 
                            PhoneItem[i].TimeUsed.wSecond);
                        MBList_SetItem(0,NULL,cLastTime,iPicState,-1,-1,FALSE);//设置通话计时
                        break;
                    }
                }
            }
            iPicState = (bMute ? MBLIST_IPICSTATE_AM : MBLIST_IPICSTATE_A);
            MBList_SetItem(1,NULL,NULL,iPicState,-1,-1,FALSE);//电话会议为激活
            break;
        }
        MBList_Refresh();
        break;
    }
}
/********************************************************************
* Function   AddMultCallInfo  
* Purpose    添加多方通话的数据到控件中
* Params    
* Return    
* Remarks   
**********************************************************************/
static  BOOL    AddMultCallInfo(void)
{
    int     nItemNum,i,iPicState;
    char    cLastTime[10] = "";

    nItemNum = GetUsedItemNum();
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling AddMultCallInfo,nItemNum:%d,InterfaceType:%d\r\n",nItemNum,InterfaceType);
#endif

    if (0 == nItemNum)
        return FALSE;

    MBList_ResetItem();//清空列表
    switch (InterfaceType)
    {
    case INTERFACE_MULT1://多方通话菜单1(1个电话激活,1个电话保持)
        for (i = 0;i < iMaxCount;i++)
        {
            if ((ITEMUSED == PhoneItem[i].ItemStat) && (0 != PhoneItem[i].Calls_Info.index))
            {
                if (CALL_STAT_ACTIVE == PhoneItem[i].Calls_Info.Stat)
                    iPicState = (bMute ? MBLIST_IPICSTATE_AM : MBLIST_IPICSTATE_A);
                else if (CALL_STAT_HELD == PhoneItem[i].Calls_Info.Stat)
                    iPicState = (bMute ? MBLIST_IPICSTATE_HM : MBLIST_IPICSTATE_H);
                else
                    iPicState = MBLIST_IPIC_UNCHANGE;//图片状态:没有改变

                sprintf(cLastTime, pTime, 
                    PhoneItem[i].TimeUsed.wHour, 
                    PhoneItem[i].TimeUsed.wMinute, 
                    PhoneItem[i].TimeUsed.wSecond);
                if (strlen(PhoneItem[i].cName) != 0)
                    MBList_AddItem(PhoneItem[i].cName,cLastTime,iPicState,i,
                    PhoneItem[i].Calls_Info.index);
                else
                    MBList_AddItem(PhoneItem[i].Calls_Info.PhoneNum,cLastTime,iPicState,i,
                    PhoneItem[i].Calls_Info.index);
                //保存该电话在结构数组中的位置
            }
        }
        break;
    case INTERFACE_MULT21://多方通话菜单2(全部为激活通话)
        iPicState = (bMute ? MBLIST_IPICSTATE_AM : MBLIST_IPICSTATE_A);
        MBList_AddItem(TXT_MEETING,NULL,iPicState,-1,-1);
        break;

    case INTERFACE_MULT22://多方通话菜单2(全部为保持通话)
        iPicState = (bMute ? MBLIST_IPICSTATE_HM : MBLIST_IPICSTATE_H);
        MBList_AddItem(TXT_MEETING,NULL,iPicState,-1,-1);
        break;

    case INTERFACE_MULT31://多方通话菜单3(1个激活通话,其余保持)
        for (i = 0;i < iMaxCount;i++)
        {
            if ((ITEMUSED == PhoneItem[i].ItemStat) && (0 != PhoneItem[i].Calls_Info.index))
            {
                if (CALL_STAT_ACTIVE == PhoneItem[i].Calls_Info.Stat)//添加激活项目
                {
                    iPicState = (bMute ? MBLIST_IPICSTATE_AM : MBLIST_IPICSTATE_A);
                    sprintf(cLastTime, pTime, 
                        PhoneItem[i].TimeUsed.wHour, 
                        PhoneItem[i].TimeUsed.wMinute, 
                        PhoneItem[i].TimeUsed.wSecond);
                    if (strlen(PhoneItem[i].cName) != 0)
                        MBList_AddItem(PhoneItem[i].cName,cLastTime,iPicState,i,
                        PhoneItem[i].Calls_Info.index);
                    else
                        MBList_AddItem(PhoneItem[i].Calls_Info.PhoneNum,cLastTime,iPicState,i,
                        PhoneItem[i].Calls_Info.index);
                    //保存该电话在结构数组中的位置
                    break;
                }
            }
        }
        //添加保持项目
        iPicState = (bMute ? MBLIST_IPICSTATE_HM : MBLIST_IPICSTATE_H);
        MBList_AddItem(TXT_MEETING,NULL,iPicState,-1,-1);
        break;

    case INTERFACE_MULT32://多方通话菜单3(1个保持通话,其余激活)
        for (i = 0;i < iMaxCount;i++)
        {
            if ((ITEMUSED == PhoneItem[i].ItemStat) && (0 != PhoneItem[i].Calls_Info.index))
            {
                if (CALL_STAT_HELD == PhoneItem[i].Calls_Info.Stat)//添加保持项目
                {
                    iPicState = (bMute ? MBLIST_IPICSTATE_HM : MBLIST_IPICSTATE_H);
                    sprintf(cLastTime, pTime, 
                        PhoneItem[i].TimeUsed.wHour, 
                        PhoneItem[i].TimeUsed.wMinute, 
                        PhoneItem[i].TimeUsed.wSecond);
                    if (strlen(PhoneItem[i].cName) != 0)
                        MBList_AddItem(PhoneItem[i].cName,cLastTime,iPicState,i,
                        PhoneItem[i].Calls_Info.index);
                    else
                        MBList_AddItem(PhoneItem[i].Calls_Info.PhoneNum,cLastTime,iPicState,i,
                        PhoneItem[i].Calls_Info.index);
                    //保存该电话在结构数组中的位置
                    break;
                }
            }
        }
        //添加激活项目
        iPicState = (bMute ? MBLIST_IPICSTATE_AM : MBLIST_IPICSTATE_A);
        MBList_AddItem(TXT_MEETING,NULL,iPicState,-1,-1);
        break;
    case INTERFACE_MULT5://单独通话
    case INTERFACE_MULT6://单独挂断
        for (i = 0;i < iMaxCount;i++)
        {
            if ((ITEMUSED == PhoneItem[i].ItemStat) && (0 != PhoneItem[i].Calls_Info.index))
            {
                if (CALL_STAT_ACTIVE == PhoneItem[i].Calls_Info.Stat)//添加激活项目
                {
                    iPicState = (bMute ? MBLIST_IPICSTATE_AM : MBLIST_IPICSTATE_A);
                    sprintf(cLastTime,pTime, 
                        PhoneItem[i].TimeUsed.wHour, 
                        PhoneItem[i].TimeUsed.wMinute, 
                        PhoneItem[i].TimeUsed.wSecond);
                    if (strlen(PhoneItem[i].cName) != 0)
                        MBList_AddItem(PhoneItem[i].cName,cLastTime,iPicState,i,
                        PhoneItem[i].Calls_Info.index);
                    else
                        MBList_AddItem(PhoneItem[i].Calls_Info.PhoneNum,cLastTime,iPicState,i,
                        PhoneItem[i].Calls_Info.index);
                    //保存该电话在结构数组中的位置
                }
            }
        }
        break;
    }
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling AddMultCallInfo函数正常结束!\r\n");
#endif
    return TRUE;
}
/********************************************************************
* Function     IsAllType_Specific
* Purpose      是否全部为iCallType类型,CALL_STAT_ACTIVE,CALL_STAT_HELD
* Params       iCallType:CALL_STAT_ACTIVE,CALL_STAT_HELD激活或者保持
* Return       全部为iCallType类型返回TRUE,否则返回FALSE
* Remarks      
**********************************************************************/
BOOL    IsAllType_Specific(int iCallType)
{
    int i;

    if ((iCallType != CALL_STAT_ACTIVE) && (iCallType != CALL_STAT_HELD))
        return FALSE;

    if (0 == GetUsedItemNum())
        return FALSE;
    for (i = 0;i < iMaxCount;i++)
    {
        if ((ITEMUSED == PhoneItem[i].ItemStat) && (0 != PhoneItem[i].Calls_Info.index))
        {
            if (iCallType != PhoneItem[i].Calls_Info.Stat)
                return FALSE;
        }
    }
    return TRUE;
}
/********************************************************************
* Function     InvertItemStateOne
* Purpose      激活与保持交换(单方通话)
* Params       index:需要更改的结构数组中的位置
* Return           
* Remarks      
**********************************************************************/
static  BOOL    InvertItemStateOne(int index)
{
    if ((ITEMUSED == PhoneItem[index].ItemStat) && (0 != PhoneItem[index].Calls_Info.index))
    {
        if (CALL_STAT_ACTIVE == PhoneItem[index].Calls_Info.Stat)
            PhoneItem[index].Calls_Info.Stat = CALL_STAT_HELD;
        else if (CALL_STAT_HELD == PhoneItem[index].Calls_Info.Stat)
            PhoneItem[index].Calls_Info.Stat = CALL_STAT_ACTIVE;

        return TRUE;
    }
    return FALSE;
}
/********************************************************************
* Function     InvertItemStateMult
* Purpose      激活与保持交换(多方通话)
* Params       
* Return           
* Remarks      
**********************************************************************/
static  void    InvertItemStateMult(void)
{
    int i;
    for (i = 0;i < iMaxCount;i++)
    {
        if ((ITEMUSED == PhoneItem[i].ItemStat) && (0 != PhoneItem[i].Calls_Info.index))
        {
            if (CALL_STAT_ACTIVE == PhoneItem[i].Calls_Info.Stat)
                PhoneItem[i].Calls_Info.Stat = CALL_STAT_HELD;
            else if (CALL_STAT_HELD == PhoneItem[i].Calls_Info.Stat)
                PhoneItem[i].Calls_Info.Stat = CALL_STAT_ACTIVE;
        }
    }
}
/********************************************************************
* Function     SetAllType
* Purpose      全部设为指定类型
* Params       
* Return           
* Remarks      
**********************************************************************/
static  BOOL    SetAllType(int nType)
{
    int i;
    for (i = 0;i < iMaxCount;i++)
    {
        if ((ITEMUSED == PhoneItem[i].ItemStat) && (0 != PhoneItem[i].Calls_Info.index))
            PhoneItem[i].Calls_Info.Stat = nType;
    }
    return TRUE;
}
/********************************************************************
* Function     RemTime
* Purpose      计算通话时间
* Params       Cost:返回计时结果,nCall:但前电话在结构数组中的位置
* Return           
* Remarks      
**********************************************************************/
static  void    RemTime(MBRecordCostTime * Cost,int nCall)
{
    Cost->nLastHour   = PhoneItem[nCall].TimeUsed.wHour;      
    Cost->nLastMin    = PhoneItem[nCall].TimeUsed.wMinute;        
    Cost->nLastSecond = PhoneItem[nCall].TimeUsed.wSecond;    

    if (CALL_DIR_ORIGINATED == PhoneItem[nCall].Calls_Info.Dir)
    {
        Cost->nOutSecond += PhoneItem[nCall].TimeUsed.wSecond;
        if (Cost->nOutSecond >= iSecond60)
        {
            Cost->nOutSecond -= iSecond60;
            Cost->nOutMin = Cost->nOutMin + PhoneItem[nCall].TimeUsed.wMinute + 1;
        }
        else
        {
            Cost->nOutMin += PhoneItem[nCall].TimeUsed.wMinute;
        }
        if (Cost->nOutMin >= iSecond60)
        {
            Cost->nOutMin -= iSecond60;
            Cost->nOutHour = Cost->nOutHour + PhoneItem[nCall].TimeUsed.wHour + 1;
        }
        else
        {
            Cost->nOutHour += PhoneItem[nCall].TimeUsed.wHour;
        }
    }
    else if (CALL_DIR_TERMINATED == PhoneItem[nCall].Calls_Info.Dir)
    {
        Cost->nInSecond += PhoneItem[nCall].TimeUsed.wSecond;
        if (Cost->nInSecond >= iSecond60)
        {
            Cost->nInSecond -= iSecond60;
            Cost->nInMin = Cost->nInMin + PhoneItem[nCall].TimeUsed.wMinute + 1;
        }
        else
        {
            Cost->nInMin += PhoneItem[nCall].TimeUsed.wMinute;
        }
        if (Cost->nInMin >= iSecond60)
        {
            Cost->nInMin -= iSecond60;
            Cost->nInHour = Cost->nInHour + PhoneItem[nCall].TimeUsed.wHour + 1;
        }
        else
        {
            Cost->nInHour += PhoneItem[nCall].TimeUsed.wHour;
        }
    }
}
/********************************************************************
* Function     MBListCurCalls
* Purpose      列当前通话
* Params       OperateType:列通话之前的操作
* Return       列当前通话的消息是否发送出去
* Remarks      
**********************************************************************/
BOOL    MBListCurCalls(OPERATETYPE OperateType,WPARAM wParam,LPARAM lParam)
{
    if (!IsWindow(hCalling))
        return FALSE;

    MBWaitWindowState(hCalling,TRUE,NULL,FALSE);
    if (-1 == ME_ListCurCalls(hCalling,IDC_ME_LISTCURCALLS))//列当前通话
    {
        AppMessageBox(hCalling,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);
        bListCurCalling = FALSE;
        return FALSE;
    }
    ListCurCallswParam = wParam;
    ListCurCallslParam = lParam;
    ListCurCallsType   = OperateType;
    bListCurCalling    = TRUE;//正在列当前通话
    return TRUE;
}
/********************************************************************
* Function     OperateListCurCalls
* Purpose      列当前通话之后执行的操作
* Params       
* Return    是否还需要调用方发送MB_ACTIVE消息,FALSE:不需要,TRUE:需要   
* Remarks      
**********************************************************************/
static  BOOL    OperateListCurCalls(void)
{
    CALLS_INFO calls_info[MAX_COUNT];
    PBINFO pbNameIcon;
    int i,j,iDataLen,isour;
    int iHangup = 0;        //电话被挂断的个数
    BOOL bAnswer = FALSE;   //是否有电话被接进来
    BOOL bWaiting = FALSE;  //是否有电话正在等待
    int iHeldNums;          //当前保持通话的个数
    int iActiveNums;        //当前激活通话的个数

    memset(calls_info,0x00,sizeof(calls_info));
    iDataLen = ME_GetResult(calls_info,sizeof(calls_info));

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling 列当前通话之后执行的操作,iDataLen:%d,CallingType:%d\r\n",iDataLen,CallingType);
#endif

    if (iDataLen <= 0)//当前已经没有通话了
    {
        switch (CallingType)//考察当前界面类型
        {
        case MBCALLINNONE:
            return TRUE;
        case MBCALLINGONE:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling OperateListCurCalls-当前是单方通话界面\r\n");
#endif
            AfterHangup_Single();//该函数会销毁当前窗口,所以返回FALSE
            return FALSE;

        case MBCALLINGMULT:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling OperateListCurCalls-当前是多方通话界面\r\n");
#endif
            AfterHangup_Mult_All();//在该函数中已经发送了MB_ACTIVE消息,所以返回FALSE
            return FALSE;
        }
    }
#if 0
    {//保存通话记录
        char cCall[100] = "";
        char cCallsResult[800] = "";
        int iDataCount;

        if (iDataLen != -1)
        {
            iDataCount = iDataLen / sizeof(CALLS_INFO);
            for (i = 0;i < iDataCount;i++)
            {
                sprintf(cCall,"CI:i=%d,index=%d,Stat=%d,Dir=%d,PhoneNum=%s\r\n",
                    i,calls_info[i].index,calls_info[i].Stat,calls_info[i].Dir,calls_info[i].PhoneNum);
                strcat(cCallsResult,cCall);
            }
            strcat(cCallsResult,"\r\n");
            SaveLog(cCallsResult);

            memset(cCallsResult,0x00,sizeof(cCallsResult));
            for (i = 0;i < iMaxCount;i++)
            {
                if (PhoneItem[i].ItemStat != ITEMUSED)
                    continue;

                memset(cCall,0x00,sizeof(cCall));
                sprintf(cCall,"PI:i=%d,Name=%s,index=%d,Stat=%d,PhoneNum=%s\r\n",
                    i,PhoneItem[i].cName,PhoneItem[i].Calls_Info.index,
                    PhoneItem[i].Calls_Info.Stat,PhoneItem[i].Calls_Info.PhoneNum);
                strcat(cCallsResult,cCall);
            }
            strcat(cCallsResult,"\r\n");
            SaveLog(cCallsResult);
        }
    }
#endif
    //当前有通话存在(有可能其中的某个通话还没有被接进来,正处于等待状态)
    iHeldNums   = GetItemTypeNum(CALL_STAT_HELD);   //当前保持通话的个数
    iActiveNums = GetItemTypeNum(CALL_STAT_ACTIVE); //当前激活通话的个数

    iDataLen /= sizeof(CALLS_INFO);//当前通话个数

    //首先检查一下是否有通话已经挂断
    for (i = 0;i < iMaxCount;i++)
    {
        for (j = 0;j < iDataLen;j++)
        {
            if ((PhoneItem[i].Calls_Info.index == calls_info[j].index) &&
                (PhoneItem[i].ItemStat == ITEMUSED))
            {
                PhoneItem[i].Calls_Info.Stat = calls_info[j].Stat;//更新激活/保持的状态
                if (strcmp(PhoneItem[i].Calls_Info.PhoneNum,calls_info[j].PhoneNum) != 0)
                {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
                    printf("mb MBCalling OperateListCurCalls PhoneItem数组中的第%d个电话的号码为:%s,calls_info数组中第%d个电话的号码为:%s\r\n",
                        i,PhoneItem[i].Calls_Info.PhoneNum,j,calls_info[j].PhoneNum);
//                    {
//                        char cCall[100] = "";
//                        sprintf(cCall,"PhoneItem第%d个电话:%s,calls_info第%d个电话:%s\r\n",
//                            i,PhoneItem[i].Calls_Info.PhoneNum,j,calls_info[j].PhoneNum);
//                        SaveLog(cCall);
//                    }
#endif
                    strcpy(PhoneItem[i].Calls_Info.PhoneNum,calls_info[j].PhoneNum);
                    if (MB_GetInfoFromPhonebook(PhoneItem[i].Calls_Info.PhoneNum,&pbNameIcon))
                    {
                        isour = strlen(pbNameIcon.Name);
                        strncpy(PhoneItem[i].cName,pbNameIcon.Name,
                            sizeof(PhoneItem[i].cName) > isour ? isour + 1 : sizeof(PhoneItem[i].cName) - 1);
                    }
                }
                break;
            }
        }
        //PhoneItem中的第i个电话没有在calls_info结构中找到,这个电话可能已经挂断了
        if ((j == iDataLen) && (PhoneItem[i].ItemStat == ITEMUSED))
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling OperateListCurCalls,i:%d,号码:%s,该电话已经挂断\r\n",i,PhoneItem[i].Calls_Info.PhoneNum);
#endif
            AfterHangup_MultOne(i);//多方通话中的被动挂断处理
            iHangup++;
        }
    }

    if ((iDataLen < iHeldNums + iActiveNums) && (iHangup == 0))
    {
        //出现这种情况表示PhoneItem数组中有两个以上的结构的index相等,表示出错了
        for (i = 0;i < iMaxCount - 1;i++)
        {
            for (j = i + 1;j < iMaxCount;j++)
            {
                if ((i != j) && (PhoneItem[i].Calls_Info.index == PhoneItem[j].Calls_Info.index))
                {
                    PhoneItem[j].ItemStat = ITEMNOTUSED;
                }
            }
        }
    }
    //因为接入电话的时候需要考察电话的路数,所以重新设置电话的路数
    SetPhoneGroupNum();

    //然后检查一下是否有通话已经被接入或者正在等待
    for (i = 0;i < iDataLen;i++)
    {
        for (j = 0;j < iMaxCount;j++)
        {
            if ((PhoneItem[j].Calls_Info.index == calls_info[i].index) &&
                (PhoneItem[j].ItemStat == ITEMUSED))
                break;
        }
        if (iMaxCount == j)//有一个(只可能是一个)电话已经被接入或者正在等待
        {
            if ((calls_info[i].Stat == CALL_STAT_ACTIVE) || //电话已经接通
                (calls_info[i].Stat == CALL_STAT_HELD))
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
                printf("mb MBCalling OperateListCurCalls,i:%d,号码:%s,stat:%d,该电话已经被接入\r\n",
                    i,calls_info[i].PhoneNum,calls_info[i].Stat);
#endif
                if (IsWindow_MBRing())
                {
                    MBRingAnswerCall();//接入电话
                    bAnswer = TRUE;
                }
                break;
            }
            else if ((calls_info[i].Stat == CALL_STAT_INCOMING) || //电话正在等待
                     (calls_info[i].Stat == CALL_STAT_WAITING))
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
                printf("mb MBCalling OperateListCurCalls 电话正在等待,%s,i:%d,index:%d\r\n",
                    calls_info[i].PhoneNum,i,calls_info[i].index);
#endif
                MessageMBRing(IDC_RING_HANGUP,0,0);//通知接听界面做相应的处理
                bWaiting = TRUE;//有通话正在等待
                break;
            }
        }
    }

    //没有电话被接进来,且有通话被挂断,这时候如果通话全是保持的时候,可能需要将通话全部激活
    if ((!bAnswer) && (iHangup > 0))
    {
        if (GetUsedItemNum() == 0)//当前没有通话
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling OperateListCurCalls 当前没有通话\r\n");
#endif
            //出现这种情况表示,当前有一个通话正在等待,其余的通话全部自行挂断了
            DestroyWindow(hCalling);//退出通话窗口,且不显示通话结束的窗口
            return FALSE;
        }
        if ((!bWaiting) &&                          //没有通话正在等待
            (GetItemTypeNum(CALL_STAT_ACTIVE) == 0))//当前所有通话都是保持的
        {   //如果有通话正在等待的话,执行切换操作有可能会将电话接进来
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling OperateListCurCalls当前所有通话都是保持的,执行切换操作\r\n");
#endif
            MBCalling_Shortcut(ACTIVE_SWITCH,NULL);//执行切换操作
            return FALSE;//不必发送MB_ACTIVE
        }
    }

    switch (ListCurCallsType)
    {
    case LISTCURCALLS_HANGUP_ONE://列通话之前的操作是挂断单方通话
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling OperateListCurCalls 单独挂断失败\r\n");
#endif
        SendAbortCommand(ListCurCallswParam,ListCurCallslParam);//发送取消操作的at命令
        AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
        break;

    case LISTCURCALLS_HANGUP_ALL://列通话之前的操作是全部挂断
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling OperateListCurCalls 全部挂断失败\r\n");
#endif
        SendAbortCommand(ListCurCallswParam,ListCurCallslParam);//发送取消操作的at命令
        AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
        break;

    case LISTCURCALLS_HANGUP_HELD://列通话之前的操作是挂断保持的通话
        //原来有保持的通话,但是被挂断的通话个数不等于保持通话的个数
        if ((iHeldNums > 0) && (iHangup != iHeldNums))
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling OperateListCurCalls 原有保持通话个数:%d,挂断个数:%d\r\n",iHeldNums,iHangup);
#endif
            SendAbortCommand(ListCurCallswParam,ListCurCallslParam);//发送取消操作的at命令
            AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
        }
        break;

    case LISTCURCALLS_HANGUP_ACTIVE://列通话之前的操作是挂断激活的通话
        //原来有激活的通话,但是被挂断的通话个数不等于激活通话的个数
        if ((iActiveNums > 0) && (iHangup != iActiveNums))
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling OperateListCurCalls 原有激活通话个数:%d,挂断个数:%d\r\n",iHeldNums,iHangup);
#endif
            SendAbortCommand(ListCurCallswParam,ListCurCallslParam);//发送取消操作的at命令
            AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
        }
        break;

    case LISTCURCALLS_HANGUP_CUT_X://列通话之前的操作是挂断指定序号的激活通话
        if (1 != iHangup)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling OperateListCurCalls 挂断指定序号的激活通话失败\r\n");
#endif
            SendAbortCommand(ListCurCallswParam,ListCurCallslParam);//发送取消操作的at命令
            AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
        }
        break;

    case LISTCURCALLS_ACTIVE_X://列通话之前的操作是密谈
        if (GetItemTypeNum(CALL_STAT_ACTIVE) != 1)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling 密谈失败,iActiveNums:%d,iHeldNums:%d\r\n",iActiveNums,iHeldNums);
#endif
            SendAbortCommand(ListCurCallswParam,ListCurCallslParam);//发送取消操作的at命令
            AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//请求失败
        }
        break;

    case LISTCURCALLS_TRANSFER://列通话之前的操作是转移
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling OperateListCurCalls 转移失败\r\n");
#endif
        SendAbortCommand(ListCurCallswParam,ListCurCallslParam);//发送取消操作的at命令
        AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//请求失败
        break;
    }
    //调用MBRingAnswerCall函数之后,会主动发送MB_ACTIVE消息,所以返回FALSE

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling OperateListCurCalls 返回:%d\r\n",bAnswer);
#endif
    return (bAnswer ? FALSE : TRUE);
}
/********************************************************************
* Function     SendAbortCommand
* Purpose      发送取消操作的at命令
* Params       
* Return           
* Remarks      
**********************************************************************/
BOOL    SendAbortCommand(WPARAM wParam,LPARAM lParam)
{
#if defined(WAVECOM_2C)
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling SendAbortCommand,wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
    if ((wParam != ME_RS_SUCCESS) && (lParam == iAbortValue))
    {
        ME_SendATCmd(hCalling,IDC_ERROR_MESSAGE,(char *)pAbortAT,iTimeOut);
        return TRUE;
    }
    return FALSE;
#else
    return TRUE;
#endif
}
/********************************************************************
* Function     SetCostLimit
* Purpose      设置当前通话是否已到话费限制
* Params       bLimit:TRUE是,FALSE否
* Return           
* Remarks      
**********************************************************************/
void    SetCostLimit(BOOL bLimit)
{
    bCostLimit = bLimit;
}
//////////////////////////////////////////////////////////////////////////
/********************************************************************
* Function     SendDtmf
* Purpose      发送分机号
* Params       
* Return           
* Remarks      
**********************************************************************/
static BOOL    SendDtmf(HWND hwndCall)
{
    HWND        hWnd;
    WNDCLASS    wc;

    if (Dtmf_Type == DTMF_NONE)
        return FALSE;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AppWndProcDtmf;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pDTMFClass;

    if (!RegisterClass(&wc))
        return FALSE;

    hWnd = CreateWindow(pDTMFClass,TITLECAPTION,WS_CAPTION|WS_BORDER|PWS_STATICBAR,  
        PLX_WIN_POSITION,hwndCall,NULL,NULL,NULL);

    if (NULL == hWnd)
    {
        UnregisterClass(pDTMFClass,NULL);
        return FALSE;
    }
    return TRUE;
}
static LRESULT AppWndProcDtmf ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static  HWND    hFocus = 0;
    static  int     iExtension;//发送第几个分机号
    static  RECT    rect;
    static  char    cDTMFClew[70] = "";
            char    cDTMF[DTMFMAXLEN + 1] = "";
            HWND    hEdit;
            LRESULT lResult;
            HDC     hdc;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        hFocus      = hWnd;
        iExtension  = 0;//发送第0个分机号
        memset(cDTMFClew,0x00,sizeof(cDTMFClew));
        SetRect(&rect,iDTMFEditX,iDTMFEditY,
            GetScreenUsableWH1(SM_CXSCREEN) - iDTMFEditX * 2,
            iDTMFEditY + GetCharactorHeight(hWnd) * 2);

        SetTimer(hWnd,IDC_TIMER_DTMF,1000,NULL);//1秒之后显示窗口
        break;

    case WM_TIMER:
        switch (wParam)
        {
        case IDC_TIMER_DTMF:
            ShowWindow(hWnd, SW_SHOW);
            UpdateWindow(hWnd); 

            KillTimer(hWnd,IDC_TIMER_DTMF);
            if (Dtmf_Type == DTMF_PAUSE)
            {
                PostMessage(hWnd,IDC_ME_SUPERADDDIAL,0,0);
                sprintf(cDTMFClew,TXT_SENDINGDTMF,cPhoneNumberExtension);
            }
            else
            {
                if (IfValidDTMFNumber(cPhoneNumberExtension))//合法的分机号
                {
                    sprintf(cDTMFClew,TXT_SENDDTMFCONFIRM,cPhoneNumberExtension);
                    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, //左键确定
                        MAKEWPARAM(IDC_BUTTON_SET,1),(LPARAM)TXT_LEFTBUTTON);
                }
                else//非法的分机号,提示用户重新输入
                {
                    hEdit = CreateWindow ("DINPUTEDIT","",
                        WS_VISIBLE|WS_CHILD|WS_TABSTOP|WS_BORDER|ES_NUMBER,
                        iDTMFEditX,rect.bottom,GetScreenUsableWH1(SM_CXSCREEN) - iDTMFEditX * 2,
                        GetEditControlHeight(),hWnd,(HMENU)IDC_DTMF_EDIT,NULL,NULL);

                    if (hEdit == NULL)
                    {
                        DestroyWindow(hWnd);
                        return FALSE;
                    }
                    hFocus = hEdit;

                    SetFocus(hFocus);
                    SendMessage(hEdit,EM_LIMITTEXT,DTMFMAXLEN,0);//限制字符个数
                    strcpy(cDTMFClew,TEXT_DTMF_INPUT);
                }
                InvalidateRect(hWnd,&rect,TRUE);
            }
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,
                (WPARAM)IDC_RBUTTON,(LPARAM)TEXT_WINDOWCANCEL);
            break;
        }
        break;

    case WM_SETFOCUS:
        SetFocus(hFocus);
        break;

    case WM_SETRBTNTEXT:
        SetRightButtonText(hWnd,wParam,lParam);
        break;

    case WM_ACTIVATE://去掉这个消息，单击右键可以退出，否则退出失败
        if (WA_INACTIVE != LOWORD(wParam))
            SetFocus(hFocus);//激活后设置焦点
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;

    case WM_CLOSE://点击右上角的退出按钮时为保存退出;
        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_RBUTTON, 0 );
        break;

    case WM_DESTROY ://通知程序管理器,此程序退出;
        memset(cPhoneNumberExtension,0x00,sizeof(cPhoneNumberExtension));
        Dtmf_Type = DTMF_NONE;//没有分机号
        UnregisterClass(pDTMFClass,NULL);
        DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
        break;

    case WM_PAINT :
        hdc = BeginPaint( hWnd, NULL);
        StrAutoNewLine(hdc,cDTMFClew,NULL,&rect,DT_LEFT);
        EndPaint(hWnd, NULL);
        break;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_RETURN:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SET,0);
            break;
        case VK_F2:
        case VK_F10:
            SendMessage(hWnd,WM_COMMAND,IDC_RBUTTON,0);
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;
    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        case IDC_DTMF_EDIT:
            switch(HIWORD(wParam))
            {
            case EN_CHANGE:
                hEdit = GetDlgItem(hWnd, IDC_DTMF_EDIT);
                if (hEdit != NULL)
                {
                    if (GetWindowTextLength(hEdit) < 1)
                        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,
                        MAKEWPARAM(IDC_BUTTON_SET,1),(LPARAM)"");
                    else
                        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, //左键确定
                        MAKEWPARAM(IDC_BUTTON_SET,1),(LPARAM)TXT_LEFTBUTTON);
                }
                break;
            }
            break;

        case IDC_BUTTON_SET://设置
            hEdit = GetDlgItem(hWnd,IDC_DTMF_EDIT);
            if (hEdit == NULL)//合法的分机号
            {
                PostMessage(hWnd,IDC_ME_SUPERADDDIAL,ME_RS_SUCCESS,0);
            }
            else//获取用户的输入
            {
                GetWindowText(hEdit,cDTMF,GetWindowTextLength(hEdit) + 1);
                if (strlen(cDTMF) > 0)
                {
                    strcpy(cPhoneNumberExtension,cDTMF);
                    ShowWindow(hEdit,SW_HIDE);
                    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,
                        MAKEWPARAM(IDC_BUTTON_SET,1),(LPARAM)"");
                    hFocus = hWnd;
                    SetFocus(hWnd);
                    PostMessage(hWnd,IDC_ME_SUPERADDDIAL,ME_RS_SUCCESS,0);
                }
            }
            break;
        case IDC_RBUTTON://退出
            DealWithRightButton(hWnd);
            break;
        }
        break;
    //***********************ME回调消息**************************
    case IDC_ME_SUPERADDDIAL://追加拨号的回调消息
        if ((cPhoneNumberExtension[iExtension] != ch_End) && //分机号尚未发送完毕
            (!GetListCurCalling()) &&                   //没有列当前通话
            (!IsAllType_Specific(CALL_STAT_HELD)))      //当前不是全部保持
        {
            while ((cPhoneNumberExtension[iExtension] == ch_Plus) || 
                (cPhoneNumberExtension[iExtension] == EXTENSIONCHAR) ||
                (cPhoneNumberExtension[iExtension] == EXTENSIONCHARW))
                iExtension++;

            if (cPhoneNumberExtension[iExtension] == ch_End)
            {
                PostMessage(hWnd,IDC_ME_SUPERADDDIAL,ME_RS_SUCCESS,0);
                break;
            }

            if (-1 == ME_SendDTMF(hWnd,IDC_ME_SUPERADDDIAL,cPhoneNumberExtension[iExtension]))
            {
                PostMessage(hWnd,IDC_ME_SUPERADDDIAL,ME_RS_SUCCESS,0);
            }
            else
            {
                sprintf(cDTMFClew,TXT_SENDINGDTMF,cPhoneNumberExtension + iExtension);
                InvalidateRect(hWnd,&rect,TRUE);
            }
            iExtension++;
        }
        else//分机号发送完毕
        {
            DestroyWindow(hWnd);
        }
        break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
