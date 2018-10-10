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
#ifndef APP_MBPUBLIC_APP
#define APP_MBPUBLIC_APP

#include "setting.h"
#include "window.h"
#include "me_wnd.h"
#include "phonebookext.h"
#include "pubapp.h"
#include "hpime.h"
#include "string.h"
#include "../../public/setup/setup.h"

#define printf printf

#define VK_EARPHONE_KEY             VK_EARPHONE_BUTTON

#define KEYWELL                     VK_F4//井号键的键值
#define KEYASTERICK                 VK_F3//星号键的键值

//#define KEYSWITCHTYPE_LONG  //选择星号,0号键的时候,切换成其它字符的方式:长按键方式
#define KEYSWITCHTYPE_SHORT //选择星号键的时候,切换成其它字符的方式:短按键方式

#define SHOWDEBUGINFO   //是否显示调试语句
//#define SUPPORTMIDDLEBUTTON//支持中间键

#define PHONENUMMAXLEN              40  //电话号码最大长度
#define MAX_COUNT                   5   //电话的最大路数
#define EXTENSIONCHAR               'p' //分机的分隔符p
#define EXTENSIONCHARW              'w' //分机的分隔符w
#define EXTENSIONSTRING             "p"

typedef enum
{
    DTMF_NONE,  //没有分机号
    DTMF_PAUSE, //p
    DTMF_WAIT   //w
}DTMF_TYPE;//分机号的类别
//列当前通话之前的操作
typedef enum
{
    LISTCURCALLS_NONE         ,//没有操作
    LISTCURCALLS_HANGUP_ONE   ,//单方通话的挂断
    LISTCURCALLS_HANGUP_ALL   ,//挂断所有通话
    LISTCURCALLS_HANGUP_HELD  ,//挂断保持的通话
    LISTCURCALLS_HANGUP_ACTIVE,//挂断激活的通话
    LISTCURCALLS_HANGUP_CUT_X ,//挂断某个通话
    LISTCURCALLS_SWITCH       ,//切换
    LISTCURCALLS_REPEAT       ,//重列当前通话
    LISTCURCALLS_TRANSFER     ,//转移
    LISTCURCALLS_ACTIVE_X      //密谈
}OPERATETYPE;

typedef struct tagPhoneItem
{
    char        cName[PHONENUMMAXLEN + 1];  //显示的来电姓名
    CALLS_INFO  Calls_Info;
#define ITEMUSED    1                   //该节点正在通话中
#define ITEMNOTUSED 0                   //该节点已经挂断
    int         ItemStat;               //节点状态,是否正在通话
    SYSTEMTIME  TimeUsed;               //通话计时
    SYSTEMTIME  TimeInDial;             //保存来电时间/拨出时间
}PHONEITEM, *PPHONEITEM;

#define WM_VOICE_HANGUPONE          WM_USER + 999//通话界面中单方通话的挂断消息
#define WM_CALLING_HANGUP           WM_USER + 998//收到对方挂断消息
#define IDC_RING_HANGUP             WM_USER + 997
//接听界面收到这个消息表示当前有部分通话被挂断,需要马上进行相应的处理

#define IDC_FORWARDING              WM_USER + 996
//接听界面收到这个消息表示当前来电是被别的手机转移过来的
//常用宏定义
#define MESSAGEBOXTITLE             MBGetString(STR_MBPHONE_CLEW)
#define CONFIRM_OK                  MBGetString(STR_WINDOW_OK)
#define CONFIRM_CANCEL              MBGetString(STR_WINDOW_CANCEL)

//////////图片部分////////////////////////////////////////////////////////////////
#define MBPIC_VIEWLIST1             "Rom:/notepad.ico"
#define MBPIC_VIEWLIST2             "Rom:/dateapp.ico"
#define MBPIC_VIEWLIST3             "Rom:/contact.ico"
#define MBPIC_VIEWLIST4             "Rom:/mu_sms.ico"
#define MBPIC_VIEWLIST5             "Rom:/mu_mms.ico"
#define MBPIC_VIEWLIST6             "Rom:/mail_mal.ico"

#define MBPIC_DIALLINK              "ROM:/mb_linking.gif"//正在连接动画的文件名

#define MBPIC_DIALUP_NUMBER         "ROM:/dialnums.gif"//数字图片
#define MBPIC_DIALUP_NUMBAR         "ROM:/diabar.gif"//输入数字的区域图片
#define MBPIC_DIALUP_BSUP           "ROM:/dialarr.gif"//退格键的图片(正常)
#define MBPIC_DIALUP_BSDOWN         "ROM:/backdown.gif"//退格键的图片(按下)

#define MBPIC_DIALUP_BUTTONUP0      "ROM:/gsmkeyu0.gif"
#define MBPIC_DIALUP_BUTTONUP1      "ROM:/gsmkeyu1.gif"
#define MBPIC_DIALUP_BUTTONUP2      "ROM:/gsmkeyu2.gif"
#define MBPIC_DIALUP_BUTTONUP3      "ROM:/gsmkeyu3.gif"
#define MBPIC_DIALUP_BUTTONUP4      "ROM:/gsmkeyu4.gif"
#define MBPIC_DIALUP_BUTTONUP5      "ROM:/gsmkeyu5.gif"
#define MBPIC_DIALUP_BUTTONUP6      "ROM:/gsmkeyu6.gif"
#define MBPIC_DIALUP_BUTTONUP7      "ROM:/gsmkeyu7.gif"
#define MBPIC_DIALUP_BUTTONUP8      "ROM:/gsmkeyu8.gif"
#define MBPIC_DIALUP_BUTTONUP9      "ROM:/gsmkeyu9.gif"
#define MBPIC_DIALUP_BUTTONUPX      "ROM:/gsmkeyux.gif"
#define MBPIC_DIALUP_BUTTONUPJ      "ROM:/gsmkeyuj.gif"

#define MBPIC_DIALUP_BUTTONDOWN0    "ROM:/gsmkeyd0.gif"
#define MBPIC_DIALUP_BUTTONDOWN1    "ROM:/gsmkeyd1.gif"
#define MBPIC_DIALUP_BUTTONDOWN2    "ROM:/gsmkeyd2.gif"
#define MBPIC_DIALUP_BUTTONDOWN3    "ROM:/gsmkeyd3.gif"
#define MBPIC_DIALUP_BUTTONDOWN4    "ROM:/gsmkeyd4.gif"
#define MBPIC_DIALUP_BUTTONDOWN5    "ROM:/gsmkeyd5.gif"
#define MBPIC_DIALUP_BUTTONDOWN6    "ROM:/gsmkeyd6.gif"
#define MBPIC_DIALUP_BUTTONDOWN7    "ROM:/gsmkeyd7.gif"
#define MBPIC_DIALUP_BUTTONDOWN8    "ROM:/gsmkeyd8.gif"
#define MBPIC_DIALUP_BUTTONDOWN9    "ROM:/gsmkeyd9.gif"
#define MBPIC_DIALUP_BUTTONDOWNX    "ROM:/gsmkeydx.gif"
#define MBPIC_DIALUP_BUTTONDOWNJ    "ROM:/gsmkeydj.gif"

#define MBPIC_MBLIST_ACTIVE         "ROM:/mbincall.gif"//正在激活的图标
#define MBPIC_MBLIST_HOLD           "ROM:/mbhold.gif"  //正在保持的图标
#define MBPIC_MBLIST_AM             "ROM:/mbam.gif"    //激活静音
#define MBPIC_MBLIST_HM             "ROM:/mbhm.gif"    //保持静音

#define MBPIC_RECORD_UNANSWER       "Rom:/mbunan.ico"//未接电话
#define MBPIC_RECORD_ANSWER         "Rom:/mbansw.ico"//已接电话
#define MBPIC_RECORD_DIALED         "Rom:/mbdialed.ico"//已拨电话
#define MBPIC_RECORD_CALLTIME       "Rom:/mbtime.ico"//通话记时
#define MBPIC_RECORD_CALLCOST       "Rom:/callcost.ico"//通话计费
#define MBPIC_RECORD_COSTTYPE       "Rom:/costtype.ico"//计费方式
#define MBPIC_RECORD_SETCOST        "Rom:/costlimit.ico"//话费限定

#define MBPIC_RECORDLIST_IN         "ROM:mb_in.gif" //呼入图标
#define MBPIC_RECORDLIST_OUT        "ROM:mb_out.gif"//呼出图标
#define MBPIC_RECORDLIST_UN         "ROM:mb_unan.gif"//未接图标
//函数声明
#if defined(_EMULATE_)
void    printf( char * format, ... );
#endif
//通话部分专用函数
BOOL    EarphoneStatus(void);//是否有耳机
void    bInitMBDialup(void);//初始化拨号
void    bInitMBLink(void);//初始化连接
void    InitMBCalling(void);//初始化通话界面所需图片信息
BOOL    APP_CallPhone(const char * pPhoneNumber);//调用拨号界面
BOOL    APP_CallPhoneNumber(const char * pPhoneNumber);//调用拨号界面并且直接拨出去
BOOL    MBCallEndWindow(const char * pName,const char * pPhoneCode,const char * pDuration,const char * pCost);//通话结束界面
BOOL    SuperAddDialNumber(const char * pPhoneNumber,BOOL bSuperAdd,BOOL bCanSend);//调用追加拨号界面
void    SetCostLimit(BOOL bLimit);//设置当前通话中断是否是因为话费限制

BOOL    MBCallingWindow(const PHONEITEM * pPhoneItem,const char * pExtension,DTMF_TYPE dtmf_type);//由接入/拨出功能调用
BOOL    IsWindow_MBCalling(void);//当前是否存在通话界面
BOOL    IsWindow_MBRing(void);//当前是否是接听界面
BOOL    IsWindow_DialLink(void);//当前是否是连接界面
BOOL    IsWindow_Dialup(void);//当前是否是拨号界面
BOOL    MBPhoneRing(const CALLS_INFO * pNewItemInfo);//显示接电话界面的函数
void    GetNewItemInfo(CALLS_INFO * pNewItemInfo);//获得来电的信息
void    SetnSelItem(int n);//设置当前选中项
int     GetUsedItemNum(void);//得已用Item个数
int     GetPhoneGroupNum(void);//获得电话的路的个数
int     AfterHangup_MultOne(int iIndex);//多方通话中的挂断处理
BOOL    MBCalling_Shortcut(int iType,const char * pShortPhone);//多方通话中的快捷处理
int     GsmSpecific(const char * pNumber);//当前号码的前缀是否是一个GSM序列标志
BOOL    IsAllType_Specific(int iCallType);//是否全部为激活或者保持类型
void    SetListCurCalls(BOOL bListCur);//设置是否正在列当前通话
BOOL    GetListCurCalling(void);//获得是否正在列当前通话
BOOL    MBListCurCalls(OPERATETYPE OperateType,WPARAM wParam,LPARAM lParam);//列当前通话
void    MBRingAnswerCall(void);//接听电话之后的处理
BOOL    GetInitState(void);//获得打电话部分初始化是否完成
void    SendMBDialLinkDialInfo(int iType);//发送连接结果给拨号回调消息

void    DestroyViewList(void);//销毁显示列表窗口
void    MessageMBRing( UINT wMsgCmd, WPARAM wParam, LPARAM lParam);//向接听界面发送消息
void    MessageMBDialUp(UINT wMsgCmd,WPARAM wParam,LPARAM lParam);//向拨号界面发送消息
void    MessageMBCallingOne(UINT wMsgCmd, WPARAM wParam, LPARAM lParam);//向单方通话界面发送消息
void    MessageMBPostMsg(int iCallMsg,WPARAM wParam, LPARAM lParam);//给底层消息分发窗口发送消息
void    MessageMBPostMsgCallForward(void);//重新开始查询有关呼叫转移的信息

BOOL    SendAbortCommand(WPARAM wParam,LPARAM lParam);//发送取消操作的at命令
BOOL    CreateMBGSMWindow(void);//创建一个窗口用于GSM操作
BOOL    CallMBDialLinkingWindow(const char * pPhoneCode,const SYSTEMTIME * pDialupTime);//调用正在连接的界面
void    GetCurrentPhoneNum(char * pPhoneNum,unsigned int iBuflen);//获得当前来电的电话号码
BOOL    GetSceneModeCall(void);//获得当前是否是免打扰呼机模式
BOOL    ReadMobileConfigFile(MBConfig * pConfig);//读取通话设置的信息
void    ReadMobileConfig(MBConfig * pConfig);//读取通话设置的信息
BOOL    MB_SetHeadphone(HWND hwndCall,int iDir);//通话过程中调整音量
BOOL    GSMDealWith(const char * cPhoneNumber,BOOL bExecuteAtOnce);//在GSM列表中查找字符串,执行相应的函数
BOOL    USSDDealWith(const char * pString,BOOL bCalling);//USSD处理
BOOL    GSM_PhoneBookDealWith(const char * pPhoneNumber);//处理数字键+#调用联系人记录的功能

BOOL    MB_GetInfoFromPhonebook(const char* pPhone, PBINFO* pInfo);//从联系人处获得数据的函数
int     MB_GetDirectDial(WORD wDDIdx, char* pNo);//从联系人处获得快速拨号所需数据的函数
int     MB_GetSIMRcdByOder(int iOrder,PPHONEBOOK pRtnRcd);//据输入序号（非空间号）取SIM卡记录
BOOL    MB_SaveToPhoneBook(HWND hWnd,const char * pNumber);//保存电话号码到联系人
void    MB_SendSms(const char * pNumber);//发送短信
void    MB_SendMms(const char * pNumber);//发送彩信
//////////////////公共函数////////////////////////////////////////////////////////
void    MBPlayMusic(int iMult);//播放来电音乐

void    SetForward_All(BOOL bForward);//设置是否存在所有转移
void    SetForward_Uncdtional(BOOL bForward);//设置是否存在无条件转移
void    SetForward_Condition(BOOL bForward);//设置是否存在所有条件转移
void    SetForward_Busy(BOOL bForward);//设置是否存在遇忙转移
void    SetForward_NoReply(BOOL bForward);//设置是否存在无应答转移
void    SetForward_NoReach(BOOL bForward);//设置是否存在不在服务区或关机转移
BOOL    GetForward_Condition(void);//考查是否存在条件转移
BOOL    GetForward_Uncdtional(void);//考查是否存在无条件转移

void    EnableState(void);//通话之后执行的操作
void    DisableState(void);//通话之前执行的操作

void    DealWithRightButton(HWND hWnd);//如果窗口中有文本框控件,选择右键按钮之后的操作
void    SetRightButtonText(HWND hWnd,WPARAM wParam,LPARAM lParam);//如果窗口中有文本框控件,设置右键按钮上的文字
int     GetIntArrayMaxValue(int * pArray,size_t count);//获得数字数组中元素的最大值
int     TimeCmp(const SYSTEMTIME * pTime1,const SYSTEMTIME * pTime2);//比较时间
//char  * ecvt(double value,int ndigit,int * dec,int * sign);
char  * MB_Mtoa(double dNumber,char * pNumber);//浮点数转换成字符串
double  MB_atof(const char * pString);//字符串转换成浮点数
void    MB_DealDouble(char * pSour,unsigned int iBit);//将字符串中的数字零,清除
BOOL    FileIfExist(const char * pFileName);//考查当前文件是否存在
BOOL    StrIsNumber(const char * pStr);//当前字符串是否可以完全转化成数字
BOOL    StrIsDouble(const char * pStr);//当前字符串是否可以完全转化成double数字
BOOL    IfValidPhoneNumber(const char * pPhoneNumber);//当前字符串是否是合法的电话号码
BOOL    IfValidDTMFNumber(const char * pStr);//当前字符串是否是合法的分机号
void    PlayClewMusic(unsigned int iPlayCount);//播放提示音
BOOL    GetPhoneItem(PHONEITEM * pPhoneItem,const char * pNumber);//为通话信息结构中的姓名,号码赋值
int     InStrArray(const char * pSearchStr,const char ** pArray);//在指针数组中查找字符串pSearchStr
int     InIntArray(int iSearchInt,const int * pArray,size_t count);//在指针数组中查找数字iSearchStr
int     InCharArray(char ch,const char * pArray,size_t count);//在指针数组中查找字符ch
BOOL    NumberIsEmergent(const char * pNumber,BOOL bPureNumber);//当前号码是否紧急电话号码
DTMF_TYPE   IfExistExtension(const char * pPhoneNumber,char * pPureNumber,
                                    char * pExtension);//电话号码中是否包含分机号
void    SetLibrateIfSwitch(void);//
int     FindSIMState(BOOL bShow);//查找SIM卡函数
BOOL    MBWaitWindowState(HWND hWnd,BOOL bState,int icallmsg,BOOL bRButton);//显示提示框
BOOL    CountControslYaxisExt(int * piConY,unsigned int iControlH,size_t count,int iDirect,unsigned int iSpace);//计算控件纵坐标的函数
BOOL    CountControlsYaxis(int * piConY,unsigned int iControlH,size_t count);//计算控件纵坐标的函数
BOOL    CountControlsAbscissa(int * pConX,unsigned int iControlW,size_t count);//计算控件横坐标的函数
int     StrAutoNewLine(HDC hdc,const char * cSourstr,char * cDesstr,const RECT * pRect,UINT uFormat);
int     GetCharactorHeight(HWND hWnd);//获得单个字符的高度
int     GetStringWidth(HDC hdc,const char * pString);//获得一个字符串的宽度
int     GetScreenUsableWH1(int nIndex);//获得屏幕中可以利用的宽度或者高度
void    GetScreenUsableWH2(int * pwidth,int * pheight);//获得屏幕中可以利用的宽度和高度
void    AppMessageBox(HWND hwnd,const char * szPrompt,const char * szCaption,int nTimeout);
BOOL    AppConfirmWin(HWND hwnd,const char * szPrompt,const char * szTipsPic,
                      const char * szCaption,const char * szOk,const char * szCancel);
char *  TrimString(char * pStr);//删除字符串开头结尾的空格字符
HMENULIST   LoadMenuList(MENULISTTEMPLATE * MenuListElement,BOOL bLostkeyNotify);//创建HMENULIST
void    MenuList_AddString(HWND hMenuList,char ** p,int * id,char ** pic);//重新加载数据到Menulist中
void    MenuList_AddStringOne(HWND hMenuList,const char * pItemName,int id,const char * pIconName);//加载一项数据到Menulist中
BOOL    ResetMenuList(HWND hMenuList,int * id);//清除MENULIST中的全部数据
BOOL    CallReplyControlWindow(HWND hwndCall,const char * pTitle,
                               unsigned int iEditMaxLen,char * pResult,
                               const char * pBLeftTxt,const char * pBRightTxt,
                               DWORD dwFlags);
int     GetEditControlHeight(void);//获得EDIT控件合适的高度
int     GetButtonControlHeight(void);//获得Button控件合适的高度
int     GetRadioButtonControlWidth(const char * pString);//获得单选按钮控件需要的宽度
int     GetInputSoftKeyboardPlace(void);//获得屏幕半屏输入法的位置
void    GetFactFileNameFromPath(const char * pFilePathName,char * pFileName,unsigned int iBuflen,BOOL bExName);//从带路径的文件名当中,获得纯文件名
void    EndPicBrowser();//来电关闭图片预览
BOOL    IsSpareTimeLeft(void);//录音时间是否已满
BOOL    EndRecorderUnvisualable(void);
BOOL    StartRecorderUnvisualable(HWND hWnd, UINT uRetMsg);
BOOL    MBViewList(HWND hwndCall);
void    PhoneSetChannel(BOOL bSet);//设置通道
void    PhoneSetChannelCalling(BOOL bSet);//设置通道

void    SaveLog(const char * pString);
DWORD   ReadLog(char * pString);
void    RemoveLog(void);
//////////////////////////////////////////////////////////////////////////
#endif
