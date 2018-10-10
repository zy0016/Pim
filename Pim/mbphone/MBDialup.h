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
#include    "hpimage.h"
#include    "MBPublic.h"
#include    "../mbrecord/RcCommon.h"

#define DEBUG_MBDIALUP          //支持debug状态打印输出

#define MBDIALUPSTYLE_KEYBORD   //拨号界面是用键盘拨号
//#define MBDIALUPSTYLE_TOUCH     //拨号界面是用触摸屏拨号
//#define MBDIALUPSTYLE_EDIT      //拨号界面是用特殊的edit控件拨号(没有实现)

#define MBDIAL_HAVEPROJECTMODE//定义这个宏表示在拨号界面需要支持调用工程模式

#define     IDC_BUTTON_MIDDLE       7//中键
#define     IDC_BUTTON_QUIT         3//退出按钮
#define     IDC_BUTTON_SET          4//确定按钮

#define     IDC_TIMER_BACKSPACE     5//长按退格键的计时器
#define     IDC_TIMER_LONGKEY       6//长按*,0键的计时器
#define     IDC_TIMER_SHORTKEY      9//短按星号键的计时器
#define     IDC_TIMER_SPECIAL       8//专用拨号界面等待时间的计时器

#define     IDC_ME_SUPERADDDIAL     WM_USER + 100//追加拨号的回调消息
#define     IDC_ME_SHORTCUT         WM_USER + 110//快捷操作
#define     IDC_INIT112             WM_USER + 90//无SIM卡的时候发送的消息

#define     TIMER_SHORTKEY          1000//短按星号键计时时间
#define     TIMER_LONGKEY           1000//长按*,0键计时时间
#define     TIMER_BACKSPACE         1000//长按退格键计时器的计时时间

#define     TITLECAPTION            MBGetString(STR_MBPHONE_DIAL_CAPTION)//呼叫
#define     BUTTONOK                (LPARAM)MBGetString(STR_WINDOW_OK)  //确定
#define     BUTTONQUIT              (LPARAM)MBGetString(STR_WINDOW_EXIT)//退出
#define     BUTTONSAVE              (LPARAM)MBGetString(STR_MBPHONE_SAVE)//保存
#define     BUTTONDELETE            (LPARAM)MBGetString(STR_MBPHONE_DELETE)//删除
#define     BUTTONBACK              (LPARAM)MBGetString(STR_WINDOW_BACK)//返回
#define     BUTTON_MIDDLE_TEXT      MBGetString(STR_MBRECORD_LIST_LINKMAN)//联系人

#define     EMERGENT                MBGetString(STR_MBPHONE_ENERGENT)//只能进行紧急呼叫
#define     MULTME_DIAL             MBGetString(STR_MBPHONE_DIALCLEW)//不能再拨号了
#define     ME_APPLY_FAIL           MBGetString(STR_MBPHONE_APPLY_FAIL)//请求失败
#define     QUICKDIAL_FAIL          MBGetString(STR_MBPHONE_QUICKDIAL_FAIL)//请输入号码
#define     LEFTBUTTONTEXT          MBGetString(STR_MBPHONE_DIALUP)//"拨打"
#define     INVALIDATENUM           MBGetString(STR_MBPHONE_INVALIDATENUMBER)//无效电话号码

#define     MBBTN_NUM               15//按键总个数

typedef enum    //按键显示定义
{
    BTN_UP,     //抬起
    BTN_DOWN    //按下
}BtnState;

typedef enum                //触摸笔触屏的区域定义
{
    AREAINFO_NONE   = -2,   //拨号界面的其它区域,就是不会引起刷新的部分
    AREAINFO_BS     = -1    //退格键区域
}AreaInfo;

#if defined(MBDIALUPSTYLE_TOUCH)
typedef struct tagDialItemBtn
{
    RECT            rPicNum;        //pPicNumber图片中相关数字所在的区域(1234567890p+#*)
    RECT            rPicBtn;        //按钮图片显示位置
    SIZE            sBtnNum;        //按钮图片区域
    HBITMAP         hBtnUp;         //按钮图片句柄(正常)
    HBITMAP         hBtnDown;       //按钮图片句柄(按下)
    unsigned int    iVKey;          //该字符对应的键值
    char            ch_short;       //字符
    BtnState        iState;         //按键状态
}DIALITEMBTN,PDIALITEMBTN;
#elif defined(MBDIALUPSTYLE_KEYBORD)
typedef struct tagDialItemBtn
{
    RECT            rBtnPicLarge;   //PICNUMBER图片中相关数字所在的区域(1234567890*#+)大号
    RECT            rBtnPicSmall;   //PICNUMBER图片中相关数字所在的区域(1234567890*#+)小号
    unsigned int    iVKey;          //该字符对应的键值
    char            ch_short;       //字符
    int             iState;         //按键状态
}DIALITEMBTN,PDIALITEMBTN;
#endif

typedef enum//选择拨号键之后执行的操作类型
{
    RESULT_NOINIT,      //初始化尚未完成
    RESULT_SUCC,        //完成拨号
    RESULT_ERRORNUMBER, //号码非法
    RESULT_GSM,         //该号码进行了GSM处理
    RESUTL_USSD,        //该号码进行了USSD处理
    RESULT_PHONENUM,    //通话路数或者通话个数不合适
    RESULT_QUICKDIAL,   //快速拨号部分没有相应的记录
    RESULT_NONESIM,     //没有SIM卡的时候,不是特殊号码
    RESUTL_PROJECTMODE, //当前号码是工程模式的启动号码
    RESULT_LINKING,     //当前正在进行拨号连接
    RESULT_END          //结束标志
}RESULT_BYSEND;
//////////////////////////////////////////////////////////////////////////
#if defined(MBDIALUPSTYLE_TOUCH)
static  int     GetPenAreaInfo(HWND hWnd,POINT Point,BtnState BState);//根据触摸笔点击的位置,获得这个点在哪个区域 
static  BOOL    GetOldAndNewRefreshArea(HWND hWnd,LPARAM lParam,int iOldPenArea);//触摸笔移动时候刷新必要的区域
static  void    SetRefreshNumbar(HWND hWnd);//输入电话号码的时候,刷新相应的区域
static  void    MBDialup_PenUp(HWND hWnd,LPARAM lParam);//处理触摸笔抬起的情况
static  void    MBDialup_PenDown(HWND hWnd,LPARAM lParam);//处理触摸笔按下的情况
#endif
#if defined(MBDIALUPSTYLE_KEYBORD)
static  void    RefreshLastNumberRect(int iAdd_Del);//刷新字符区域 
static  void    MBDialup_BackSpaceKey(HWND hWnd);//拨号界面是用键盘拨号的话,选择退格键的操作
static  void    MBDialup_BackSpaceButton(HWND hWnd);//拨号界面是用键盘拨号的话,选择退格按钮的操作
static  void    MBDialup_Keyboard(HWND hWnd,WPARAM wParam);//拨号界面是用键盘拨号的话,选择按键的操作
static  int     GetDialitemBtnKey(unsigned int iVKey);//获得某个键值在dialitembtn数组中的位置
#endif

static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  void    ShowPhoneNumber(HDC hdc);//显示号码
static  BOOL    DialNumber(HWND hWnd);//执行拨号操作
static  int     GetDialitemBtnIndex(char ch);//返回某个字符在dialitembtn数组中的位置
static  BOOL    DealWithNumber(HWND hWnd);//根据GSM规则处理号码
static  BOOL    APP_CallPhoneWindow(const char * pPhoneNumber,BOOL bSuperAdd,BOOL bCanSend);//创建拨号窗口
static  void    MBDialupCreate(HWND hWnd);
static  RESULT_BYSEND   DealWithNumberBySend(const char * pPhoneNumber);//选择拨号键之后执行的操作
static  RESULT_BYSEND   DealWithNumberBySendLink(const char * pPhoneNumber);//由APP_CallPhoneNumber函数调用,连接专用接口
