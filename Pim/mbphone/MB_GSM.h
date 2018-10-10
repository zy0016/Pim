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
#include    "me_wnd.h"
#include    "MBPublic.h"
#include    "stdio.h"
#include    "stdlib.h"
#include    "../mbrecord/RcCommon.h"

#define     IDC_BUTTON_QUIT         3//退出按钮
#define     IDC_BUTTON_SET          4//确定
#define     IDC_USSD_LEFTBUTTON     6//用于USSD功能的左键
#define     IDC_USSD_RIGHTBUTTON    7//用于USSD功能的右键
#define     IDC_PHONE_LEFTBUTTON    8//用于查询联系人功能的左键
#define     IDC_PHONE_RIGHTBUTTON   9//用于查询联系人功能的右键
#define     IDC_GSMEDIT             5
#define     IDC_GSMOPERATE          WM_USER + 110//GSM操作的回调消息
#define     IDC_USSD_NOTIFY         WM_USER + 120//ussd注册消息
#define     IDC_ME_PHONE_SIM_AMX    WM_USER + 130//查询SIM卡中联系人的最大个数
#define     IDC_CANCELOPERATE       WM_USER + 140//标志取消操作的回调消息

#define     TITLECAPTION            MBGetString(STR_MBPHONE_GSM_CAPTION)    //GSM操作
#define     BUTTONOK                (LPARAM)MBGetString(STR_WINDOW_OK)      //确定
#define     BUTTONQUIT              (LPARAM)MBGetString(STR_WINDOW_EXIT)    //退出
#define     BUTTONTXTLEFT_REPLY     MBGetString(STR_MBPHONE_USSD_REPLY)     //回复
#define     BUTTONTXTRIGHT          MBGetString(STR_WINDOW_BACK)            //返回

#define     GSMOPERATESUCC_1        MBGetString(STR_MBPHONE_GSM_SUCC_1)     //"操作成功"
#define     GSMOPERATEFAIL_1        MBGetString(STR_MBPHONE_GSM_FAIL_2)     //"操作失败"
#define     GSMOPERATESEARCH_FAIL   MBGetString(STR_MBPHONE_GSM_FIND_FAIL)  //查询失败
#define     GSMOPERATECANCEL        MBGetString(STR_MBPHONE_GSM_OPERATECANCEL)//操作取消

#define     GSMOPERATESUCC          MBGetString(STR_MBPHONE_GSM_SUCC)       //成功
#define     GSMOPERATEFAIL          MBGetString(STR_MBPHONE_GSM_FAIL)       //失败
#define     GSMOPERATEACTIVATE      MBGetString(STR_MBRECORD_PUB_ACTIVATE)  //激活
#define     GSMOPERATEDEACTIVATE    MBGetString(STR_MBRECORD_PUB_DEACTIVATE)//撤销
#define     GSMOPERATE_RA           MBGetString(STR_MBRECORD_PUB_RA)        //注册激活
#define     GSMOPERATE_UD           MBGetString(STR_MBRECORD_PUB_UD)        //注销禁止
#define     GSMOPERATESEARCH        MBGetString(STR_MBPHONE_GSM_FIND)       //查询
#define     GSMOPERATEHAVED         MBGetString(STR_MBPHONE_GSM_HAVE)       //已 

#define     IMEIFAIL                MBGetString(STR_MBPHONE_GSM_IMEIFAIL)   //IMEI查询失败

#define     GSMPIN1MODIFY           MBGetString(STR_MBPHONE_GSM_PIN1_MODIFY)//pin1码修改%s
#define     GSMPIN2MODIFY           MBGetString(STR_MBPHONE_GSM_PIN2_MODIFY)//pin2码修改%s
#define     GSMPUK1MODIFY           MBGetString(STR_MBPHONE_GSM_PUK1_MODIFY)//PIN解锁%s
#define     GSMPUK2MODIFY           MBGetString(STR_MBPHONE_GSM_PUK2_MODIFY)//PIN2解锁%s
#define     GSMBARRINGMODIFY        MBGetString(STR_MBPHONE_GSM_BARRING_MODIFY)//注册网络密码%s
#define     GSMUNLOCKPS             MBGetString(STR_MBPHONE_GSM_UNLOCKPS)   //"手机锁解锁%s"
#define     GSM_WRONGPASSWORD       MBGetString(STR_MBPHONE_GSM_WRONGPASSWORD)//"错误的密码"
#define     GSMBARRING_ERROR1       MBGetString(STR_MBPHONE_GSM_BARRING_ERROR1)//"服务未预定"
#define     GSMBARRING_ERROR2       MBGetString(STR_MBPHONE_GSM_BARRING_ERROR2)//"密码错误"
#define     GSMBARRING_ERROR3       MBGetString(STR_MBPHONE_GSM_BARRING_ERROR3)//"非法的拨出号码"

#define     GSMCALLFORWARDING1      MBGetString(STR_MBPHONE_GSM_FORWARD1)//所有呼叫转移
#define     GSMCALLFORWARDING2      MBGetString(STR_MBPHONE_GSM_FORWARD2)//所有条件转移
#define     GSMCALLFORWARDING3      MBGetString(STR_MBPHONE_GSM_FORWARD3)//无条件转移
#define     GSMCALLFORWARDING4      MBGetString(STR_MBPHONE_GSM_FORWARD4)//无应答转移
#define     GSMCALLFORWARDING5      MBGetString(STR_MBPHONE_GSM_FORWARD5)//不在服务区或关机转移
#define     GSMCALLFORWARDING6      MBGetString(STR_MBPHONE_GSM_FORWARD6)//遇忙转移
#define     GSMCALLFORWARDING_NO    MBGetString(STR_MBPHONE_GSM_FORWARD_NO)//"号码:"

#define     GSMCALLBARRING33        MBGetString(STR_MBPHONE_GSM_BARRING1)//限制所有拨出电话
#define     GSMCALLBARRING330       MBGetString(STR_MBPHONE_GSM_BARRING2)//解除所有通话限制
#define     GSMCALLBARRING331       MBGetString(STR_MBPHONE_GSM_BARRING3)//限制所有拨出国际长途
#define     GSMCALLBARRING332       MBGetString(STR_MBPHONE_GSM_BARRING4)//限制所有拨出国际长途(除本国)
#define     GSMCALLBARRING333       MBGetString(STR_MBPHONE_GSM_BARRING5)//解除所有拨出限制
#define     GSMCALLBARRING35        MBGetString(STR_MBPHONE_GSM_BARRING6)//限制所有拨入电话
#define     GSMCALLBARRING351       MBGetString(STR_MBPHONE_GSM_BARRING7)//限制国际漫游时来电
#define     GSMCALLBARRING353       MBGetString(STR_MBPHONE_GSM_BARRING8)//解除所有拨入限制

#define     GSMCALLWAITING          MBGetString(STR_MBPHONE_GSM_CALLWAIT)//呼叫等待

#define     GSMCLIR                 MBGetString(STR_MBPHONE_GSM_CLIR) //CLIR%s%s
#define     GSMCLIR1                MBGetString(STR_MBPHONE_GSM_CLIR1)//"not provisioned"
#define     GSMCLIR2                MBGetString(STR_MBPHONE_GSM_CLIR2)//"provisioned in permanent mode"
#define     GSMCLIR3                MBGetString(STR_MBPHONE_GSM_CLIR3)//"unknown (e.g. no network, etc."
#define     GSMCLIR4                MBGetString(STR_MBPHONE_GSM_CLIR4)//"temporary mode presentation restricted"
#define     GSMCLIR5                MBGetString(STR_MBPHONE_GSM_CLIR5)//"temporary mode presentation allowed"

#define     GSMCLIP                 MBGetString(STR_MBPHONE_GSM_CLIP)//"CLIP%s%s"
#define     GSMCOLP                 MBGetString(STR_MBPHONE_GSM_COLP)//"COLP %s %s"
#define     GSMCOLR                 MBGetString(STR_MBPHONE_GSM_COLR)//"COLR %s %s"

#define     GSMCLIRINVOKE           MBGetString(STR_MBPHONE_GSM_CLIR_INVOKE)//"Activate CLIR %s"
#define     GSMCLIRSUPPRESS         MBGetString(STR_MBPHONE_GSM_CLIR_SUPPRESS)//"Suppress CLIR %s"

#define     GSMCLIP_NONE            MBGetString(STR_MBPHONE_GSM_CLIP_NONE)//"未提供CLIP服务"
#define     GSMCLIP_ALLOW           MBGetString(STR_MBPHONE_GSM_CLIP_ALLOW)//"提供CLIP服务"
#define     GSMCLIP_UNKNOWN         MBGetString(STR_MBPHONE_GSM_CLIP_UNKNOWN)//"结果未知"

#define     USSD_NOSERVICE          MBGetString(STR_MBPHONE_USSD_NOSERVICE)//此项服务无效

#define     WITHOUTDATA             MBGetString(STR_MBPHONE_PBLIST_WITHOUTDATA)//无号码
#define     INVALIDATENUM           MBGetString(STR_MBPHONE_INVALIDATENUMBER)//无效电话号码
typedef struct 
{
    HWND        hWnd;           //执行me函数所需句柄
    int         iCallMsg;       //执行的消息
    char        * pPhoneNumber; //电话号码
    char        ** pGsmPre;     //表前缀地址
    int         iStation;       //符合GSM前缀的号码在前缀表地址中的位置
    DWORD       wParam;         //附加参数
}GSMSTRUCT;
typedef struct 
{
    const char  **pGSMPre;      //前缀表地址
    int         (* pFunction)(GSMSTRUCT * pGsmStruct);//函数地址
    DWORD       wParam;         //附加参数
    BOOL        bExecuteAtOnce; 
    //执行方式,TRUE:检测到结束符的话立即执行,FALSE:选择拨号键之后再执行
    BOOL        bGSMLengthMatch;
    //是否进行GSM号码的长度匹配,有些GSM号码的长度是固定的,所以可以在最初的扫描过程中
    //协助匹配,避免出现*#43##的情况
}GSMOPERATE;
typedef enum                        //操作标志
{
    GSM_IMEI,                       //查IMEI号
    GSM_PIN1,                       //改变pin码
    GSM_PIN2,                       //改变pin2码
    GSM_PUK1,                       //解puk码
    GSM_PUK2,                       //解puk2码
    GSM_CALLBAR,                    //修改呼叫限制码
    GSM_UNLOCKPS,                   //手机锁,解锁

    GSM_CALLFORWARDING1,            //所有转移
    GSM_CALLFORWARDING2,            //所有条件转移
    GSM_CALLFORWARDING3,            //无条件转移
    GSM_CALLFORWARDING4,            //无应答转移
    GSM_CALLFORWARDING5,            //不在服务区或关机转移
    GSM_CALLFORWARDING6,            //遇忙转移

    GSM_CALLFORWARDING_CHECKBEGIN,  //开始查询呼叫转移部分
    GSM_CALLFORWARDING1_CHECK,      //查询所有转移
    GSM_CALLFORWARDING2_CHECK,      //查询所有条件转移
    GSM_CALLFORWARDING3_CHECK,      //查询无条件转移
    GSM_CALLFORWARDING4_CHECK,      //查询无应答转移
    GSM_CALLFORWARDING5_CHECK,      //查询不在服务区或关机转移
    GSM_CALLFORWARDING6_CHECK,      //查询遇忙转移
    GSM_CALLFORWARDING_CHECKEND,    //查询呼叫转移部分结束

    GSM_CALLBARRING33,              //限制所有拨出电话
    GSM_CALLBARRING331,             //限制所有拨出国际长途
    GSM_CALLBARRING332,             //限制所有拨出国际长途(除本国)    
    GSM_CALLBARRING35,              //限制所有拨入电话
    GSM_CALLBARRING351,             //限制国际漫游时来电
    GSM_CALLBARRING330,             //解除所有通话限制
    GSM_CALLBARRING333,             //解除所有拨出限制
    GSM_CALLBARRING353,             //解除所有拨入限制

    GSM_CALLBARRING_CHECKBEGIN,     //开始查询呼叫限制部分
    GSM_CALLBARRING33_CHECK,        //限制所有拨出电话
    GSM_CALLBARRING331_CHECK,       //解除所有通话限制
    GSM_CALLBARRING332_CHECK,       //限制所有拨出国际长途
    GSM_CALLBARRING35_CHECK,        //限制所有拨出国际长途(除本国)
    GSM_CALLBARRING351_CHECK,       //解除所有拨出限制
    GSM_CALLBARRING330_CHECK,       //限制所有拨入电话
    GSM_CALLBARRING333_CHECK,       //限制国际漫游时来电
    GSM_CALLBARRING353_CHECK,       //解除所有拨入限制
    GSM_CALLBARRING_CHECKEND,       //查询呼叫限制部分结束

    GSM_CALLWAIT_ACTIVE,            //激活呼叫等待
    GSM_CALLWAIT_CHECK,             //查询呼叫等待
    GSM_CALLWAIT_DEACTIVE,          //禁止呼叫等待

    GSM_CLIP_CHECK,                 //CLIP check status
    GSM_CLIR_CHECK,                 //CLIR check status
    GSM_COLP_CHECK,                 //COLP check status(Connected Line identification Presentation)
    GSM_COLR_CHECK,                 //Check status COLR(connected Line identification Restriction)

    GSM_CLIR_INVOKE,                //Invoke CLIR for a voice call
    GSM_CLIR_SUPPRESS,              //Suppress CLIR for a voice call

    GSM_FILELOG_DELETE,
    GSM_FILELOG_READ,
    GSM_END                         //GSM操作结束
}GSMTYPE;

static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  void    OperateGSMWindow(GSMTYPE iGSM_Type,BOOL bShow,BOOL bAutoClose);
static  BOOL    GSMClewText_Succ(HWND hWnd);
static  BOOL    GSMClewText_Fail(HWND hWnd,WPARAM wParam,WPARAM lParam);

static  int     GsmIMEI(GSMSTRUCT * pGsmStruct);
static  int     GsmSecurity(GSMSTRUCT * pGsmStruct);
static  int     GsmModifyCallBarring(GSMSTRUCT * pGsmStruct);
static  int     GsmUnlockPS(GSMSTRUCT * pGsmStruct);
static  int     GsmForwardingOperate(GSMSTRUCT * pGsmStruct);
static  int     GsmForwardingCheck(GSMSTRUCT * pGsmStruct);
static  int     GsmCallBarringOperate(GSMSTRUCT * pGsmStruct);
static  int     GsmCallBarringCheck(GSMSTRUCT * pGsmStruct);
static  int     GsmCallWaitingOperate(GSMSTRUCT * pGsmStruct);
static  int     GsmPNPCheck(GSMSTRUCT * pGsmStruct);
static  int     GsmCLIRDeal(GSMSTRUCT * pGsmStruct);
static  int     ReadFileLog_Delete(GSMSTRUCT * pGsmStruct);
static  int     ReadFileLog_Read(GSMSTRUCT * pGsmStruct);

static  int     GetGSMParam(const char * pGsmCode,char * param1,
                            char * param2,char * param3);
static  void    GetGSMSC(const char * pGsmCode,char * pSC);
static  void    SetWindowTextExt(HWND hEdit,const char * pString);
static  void    StringCombination(char * pResult,const char * p1,const char * p2,
                                  const char * p3,const char * p4);

static  BOOL    USSDDealRespose(HWND hWnd,WPARAM wParam,LPARAM lParam);
static  BOOL    SetUssdInfo(HWND hWnd);
