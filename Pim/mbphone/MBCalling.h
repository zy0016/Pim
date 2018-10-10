/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/
#include    "winpda.h"
#include    "str_mb_plx.h"
#include    "hp_pdaex.h"
#include    "MBPublic.h"
#include    "hpimage.h"
#include    "MBList.h"
#include    "../mbrecord/RcCommon.h"

//#define MBCALLING_RECORD//定义这个宏表示存在录音功能
#define MBCALLING_TIMEACCURATE//通话过程中精确计时
//#define MBCALLING_TIMESIMPLE//通话过程中一般计时

#define     DTMFMAXLEN                  30

#define     TIMER_CALLING               100//计时器的毫秒数值
#define     TIMER_CALLING_COST          2000//通话计费的计时器的毫秒数值
#define     TIMER_LONGKEY               1000//判断长按键的计时器的毫秒数值
#define     TIMER_INIT                  2000//进行初始化所需的计时器的毫秒数值
#define     TIMER_ERROR                 5555//出错处理所需计时器的毫秒数值

#define     IDC_BUTTON_MIDDLE           20//中键
#define     IDC_RBUTTON                 30//单方通话的静音/解静音按钮(右softkey)
#define     IDC_LBUTTON_NONE            40//左softkey,用于没有通话时界面的显示
#define     IDC_RBUTTON_BACK            50//通话结束界面的返回按钮
#define     IDC_RBUTTON_CANCEL          51//单独通话/单独挂断的取消按钮
#define     IDC_LBUTTON_CONNECT         52//单独通话
#define     IDC_LBUTTON_HANGUP          53//单独挂断
#define     IDC_LBUTTON_MENU            54//有菜单的时候,左softkey的文字
#define     IDC_BUTTON_RECORD           55//录音按钮的id
#define     IDC_BUTTON_SET              56//左键
#define     IDC_DTMF_EDIT               57//输入DTMF的文本框
#define     MB_ACTIVE                   WM_USER + 55//通话个数有变化的时候,需要发送这个消息,提示界面有变化

#define     IDC_MBLIST                  60//多方通话的控件

#define     IDC_TIMER_CALLING           70//通话计时的计时器
#define     IDC_TIMER_COST              80//通话计费的计时器
#define     IDC_TIMER_INIT              90//初始化信息的计时器
#define     IDC_TIMER_LONGKEY           100//长按数字键0的计时器
#define     IDC_TIMER_ERROR             110//出错处理的计时器
#define     IDC_TIMER_DTMF              120//发送分机号的计时器

#define     IDC_ME_SHORTCUTSWITCH       WM_USER + 99//切换的回掉消息,用于快捷操作
#define     IDC_ONE_SWITCH              WM_USER + 100//保持/激活的回调消息
#define     IDC_MULTME_SWITCH1          WM_USER + 120//多方通话的切换消息(1个电话激活,1个电话保持)
#define     IDC_MULTME_SWITCH2          WM_USER + 121//多方通话的切换消息(全部为激活通话/全部为保持通话)
#define     IDC_MULTME_ALLACTIVE        WM_USER + 130//多方通话的全部激活消息
#define     IDC_MULTME_ALLHANGUP        WM_USER + 140//多方通话的全部挂断消息
#define     IDC_ME_LISTCURCALLS         WM_USER + 150//列当前通话的回调消息
//#define     IDC_PHONEBOOK               WM_USER + 160//选择联系人的时候所需之回掉消息
#define     IDC_ME_SUPERADDDIAL         WM_USER + 170//追加拨号的回调消息
#define     IDC_ME_MUTE                 WM_USER + 180//静音的回调消息

#define     IDC_ME_SHORTOTHER           WM_USER + 200//其它操作的回掉消息
#define     IDC_ME_CUT_HELD             WM_USER + 210//呼叫等待时忙音或者挂断保持的通话
#define     IDC_ME_CUT_ACTIVED          WM_USER + 220//挂断激活的通话
#define     IDC_ME_CUT_ACTIVED_X        WM_USER + 230//挂断指定序号的激活通话
#define     IDC_ME_ACTIVE_SWITCH_X      WM_USER + 250//将除指定序号外的所有通话变为保持
#define     IDC_ME_CALL_TRANSFER        WM_USER + 270//通话中当已有两路电话时,使这两路电话连起,用户从通话中退出
#define     IDC_ME_GETPRICEUNIT         WM_USER + 280//获取计价信息
#define     IDC_ME_GETCCM               WM_USER + 290//获取当前Call Meter值
#define     IDC_ME_REPEAT               WM_USER + 300//重列当前通话
#define     IDC_RECORD_OVER             WM_USER + 310//录音停止之后收到的消息
#define     IDC_ME_MAXPRICE             WM_USER + 320//查询最大累计Call Meter值的消息
#define     IDC_ERROR_MESSAGE           WM_USER + 500//出错处理的at消息
#define     IDC_ERROR_LISTCALLING       WM_USER + 510//出错处理的回调消息

#define     TITLECAPTION                MBGetString(STR_MBPHONE_CALLING)//通话

#define     TXT_MUTE                    MBGetString(STR_MBPHONE_MUTE)   //静音
#define     TXT_UNMUTE                  MBGetString(STR_MBPHONE_UNMUTE) //发音
#define     TXT_BACK                    MBGetString(STR_WINDOW_BACK)    //退出
#define     TXT_CONNECT                 MBGetString(STR_MBPHONE_CALLING)//通话
#define     TXT_HANGUP                  MBGetString(STR_MBPHONE_HANGUP)//挂断
#define     TXT_HANGUPMENU              MBGetString(STR_MBPHONE_HANGUPEXT)//挂断
#define     TXT_CANCEL                  MBGetString(STR_MBRECORD_PUB_CANCEL)//取消
#define     TXT_MENU                    MBGetString(STR_WINDOW_MENU)//菜单
#define     TXT_DIALNUMBER              MBGetString(STR_MBPHONE_DIALNUMBER)//拨号
#define     TEXT_WINDOWCANCEL           MBGetString(STR_WINDOW_CANCEL)//取消
#define     TXT_LEFTBUTTON              MBGetString(STR_WINDOW_OK) 
#define     TXT_FINISH                  MBGetString(STR_MBPHONE_DONE)//完成

#define     DIALLINKINGWAIT             MBGetString(STR_MBPHONE_WAIT)
#define     IDS_CALLTIME                MBGetString(STR_MBRECORD_DURATION)//通话计时
#define     SWITCHFAIL                  MBGetString(STR_MBPHONE_SWITCH_FAIL)//切换失败
#define     ACTIVEFAIL                  MBGetString(STR_MBPHONE_ACTIVE_FAIL)//激活失败
#define     HANGUPFAIL                  MBGetString(STR_MBPHONE_HANGUP_FAIL)//挂断失败
#define     HANGUPALLCLEW               MBGetString(STR_MBPHONE_HANGUP_ALL)//全部挂断?
#define     CALLEND                     MBGetString(STR_MBPHONE_CALLONEEND)//通话结束
#define     MUTEFAIL                    MBGetString(STR_MBPHONE_MUTEFAIL)//静音失败
#define     UNMUTEFAIL                  MBGetString(STR_MBPHONE_UNMUTEFAIL)//解静音失败
#define     ME_APPLY_FAIL               MBGetString(STR_MBPHONE_APPLY_FAIL)//请求失败
#define     DIALLING_FARE               MBGetString(STR_MBPHONE_DIALLING_FARE)//"已到话费限制"
#define     TXT_RECORD                  MBGetString(STR_MBPHONE_RECORD_BEGIN)//"录音"
#define     TXT_STOPRECORD              MBGetString(STR_MBPHONE_RECORD_END)//"停止录音"
#define     TXT_VIEW                    MBGetString(STR_MBPHONE_VIEW)//"查看"

#define     TXT_SENDINGDTMF             MBGetString(STR_MBPHONE_DTMFSENDING)//"正在发送%s..."
#define     TXT_SENDDTMFCONFIRM         MBGetString(STR_MBPHONE_DTMFCONFIRM)//"确定发送%s?"
#define     TEXT_DTMF_INPUT             MBGetString(STR_MBPHONE_INPUTDTMF)//"请输入分机号:"
//**********************菜单部分**********************
#define     TXT_HOLD                    MBGetString(STR_MBPHONE_HOLDING)//保持
#define     TXT_RESTORE                 MBGetString(STR_MBPHONE_RESTORE)//激活
#define     TXT_SWITCH                  MBGetString(STR_MBPHONE_SWITCH)//切换
#define     TXT_ALLACTIVE               MBGetString(STR_MBPHONE_ACTIVEALL)//加入电话会议
#define     TXT_ALLHANGUP               MBGetString(STR_MBPHONE_HANGUPALL)//全部挂断
#define     TXT_HANGUPHOLD              MBGetString(STR_MBPHONE_HANGUPHOLD)//结束保持通话
#define     TXT_SHIFT                   MBGetString(STR_MBPHONE_SHIFT)//转移
#define     TXT_MEETING                 MBGetString(STR_MBPHONE_MEETING)//电话会议
#define     TXT_SINGLECALL              MBGetString(STR_MBPHONE_SINGLECALL)//单独通话
#define     TXT_SINGLEHANGUP            MBGetString(STR_MBPHONE_SINGLEHANGUP)//单独挂断
#define     TXT_HANGUP_SINGLE           MBGetString(STR_MBPHONE_HANGUP_SINGLE)//"挂断单个通话"
#define     TXT_HANGUP_MEETING          MBGetString(STR_MBPHONE_HANGUP_MEETING)//"挂断电话会议"
#define     TXT_HANGUPACTIVE            MBGetString(STR_MBPHONE_HANGUP_CURRENTCALL)//"结束当前电话"

#define     IDM_ONE_HOLD                300//保持
#define     IDM_SWITCH2                 310//切换(全部为激活通话)
//#define     IDM_ADDRESS                 320//联系人
//#define     IDM_SMS                     330//短信
//#define     IDM_MMS                     340//彩信
#define     IDM_SWITCH1                 350//切换(1个电话激活,1个电话保持)
#define     IDM_ALLACTIVE               360//全部激活:激活全部通话
#define     IDM_ALLHANGUP               370//全部挂断:挂断全部电话
#define     IDM_HANGUPHOLD              380//挂断保持的通话
#define     IDM_HANGUPACTIVE            450//挂断激活的电话
#define     IDM_SHIFT                   400//转移
#define     IDM_SINGLECALL              410//单独通话
#define     IDM_SINGLEHANGUP            420//单独挂断
#define     IDM_MUTE                    480//静音
#if defined(MBCALLING_RECORD)
#define     IDM_RECORD                  460//录音
#endif
#define     IDM_VIEW                    470//查看

static MENUITEMTEMPLATE MBCallingMenu[] = //通话菜单
{
    {MF_STRING,IDM_MUTE  ,"",NULL},//静音
#if defined(MBCALLING_RECORD)
    {MF_STRING,IDM_RECORD,"",NULL},//录音
#endif
    {MF_STRING,IDM_VIEW  ,"",NULL},//查看
    {MF_END   ,0          , NULL,NULL}
};
static const MENUTEMPLATE MBCallingMenuTemplate =
{
    0,
    (PMENUITEMTEMPLATE)MBCallingMenu
};
typedef enum//当前通话类型
{
    MBCALLINNONE    = 0,//没有通话
    MBCALLINGONE    = 1,//单方通话
    MBCALLINGMULT   = 2 //多方通话
}MBCALLTYPE;
typedef enum//界面类型
{
    INTERFACE_NONE   = 0,//没有通话
    INTERFACE_ONE    = 1,//单方通话
    INTERFACE_MULT1  = 2,//多方通话菜单1(1个电话激活,1个电话保持)
    INTERFACE_MULT21 = 3,//多方通话菜单2(全部为激活通话)
    INTERFACE_MULT22 = 4,//多方通话菜单2(全部为保持通话)
    INTERFACE_MULT31 = 5,//多方通话菜单3(1个激活通话,其余保持)
    INTERFACE_MULT32 = 6,//多方通话菜单3(1个保持通话,其余激活)
    INTERFACE_MULT5  = 7,//单独通话
    INTERFACE_MULT6  = 8 //单独挂断
}INTERFACE;
////////////////////////////////////////////////////////////////
static  LRESULT AppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  void    ChangeTimeOnTimer(void);//单方通话响应TIMER改变时间

static  BOOL    SetAllType(int nType);//全部设为指定类型
static  int     GetTheNUsedItem(int nTheN);//得第n个使用的记录
static  BOOL    SetMenuSoftkey(void);//修改通话界面的按钮以及菜单
static  BOOL    MBTalkingWindow(void);//显示通话窗口
static  void    CallingDisplay(HDC hdc);//通话界面的显示部分
static  BOOL    AddMultCallInfo(void);//添加多放通话的数据到控件中
static  void    InvertItemStateMult(void);//激活与保持交换(多方通话)
static  BOOL    InvertItemStateOne(int index);//激活与保持交换(单方通话)
static  void    RemTime(MBRecordCostTime * Cost,int nCall);//计算通话时间
static  void    AfterHangup_Single(void);//单方通话中的主动挂断处理
static  int     GetFirstNotUsedItem(void);//得第一个未用ITEM
static  void    AfterHangup_Mult_All(void);//多方通话中的全部挂断处理
static  BOOL    OperateListCurCalls(void);//列当前通话之后执行的操作
static  int     GetItemTypeNum(int iType);//得已用激活或者保持的Item个数
static  void    SetMBCallingMenu(INTERFACE iMenuType);//设置菜单
static  void    SetChangelessMenu(HMENU hMenu);//设置菜单的固定项
static  void    ExecuteHangup(void);//处理挂断键的操作
static  void    SetPhoneGroupNum(void);//设置电话的路的个数
static  void    GetPriceUnit(void);//获取计价信息之后的处理过程
static  void    MB_RemoveAllMenu(HMENU hMenu);//删除所有菜单
static  void    RemovePortraitBitmap(void);//删除单方通话显示的头像

static  LRESULT AppWndProcDtmf( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  BOOL    SendDtmf(HWND hwndCall);
