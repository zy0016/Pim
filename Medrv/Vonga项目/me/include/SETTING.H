#ifndef _SETTING_H
#define _SETTING_H


//#include "app.h"
#include "ui.h"
/*Section name and Key name in .ini file*/

//owner infomation:

//Section name
#define SN_OWNER        "owner"
//Key name
#define KN_OWN_NAME     "name"
#define KN_OWN_CORP     "corp"
#define KN_OWN_ADDR     "addr"
#define KN_OWN_WKTEL    "work-tel"
#define KN_OWN_HMTEL    "home-tel"
#define KN_OWN_DISP     "DisplayOnStart"


//密码:
//Section name
#define SN_PASSWORD     "Password"
//Key name
#define KN_PWD_PIN1     "pin1"
#define KN_PWD_PIN2     "pin2"
#define KN_PWD_PUK1     "puk1"
#define KN_PWD_PUK2     "puk2"  
#define KN_PWD_PHONE    "phone"
#define KN_PWD_PINON	"pinon"
#define KN_PWD_PHONEON  "phoneon"

//输入密码类型
#define PWDTYPE_PIN1        300
#define PWDTYPE_PIN2        302
#define PWDTYPE_PUK1        303
#define PWDTYPE_PUK2        304
#define PWDTYPE_PHONE       305

//power auto-off time:
//Section name
#define SN_POWER        "power"
//Key name
#define KN_PWR_AUTOOFF  "AutoOff"
#define KN_PWR_LIGHT    "light"

//"对比度"程序中使用的节名.
#define SN_CONTRAST		"contrast"
//"对比度"程序中使用的关键字名.
#define KN_CONTRAST		"ctr_value"
#define KN_SLIPPER_XLEFT "SlipperXLeft"

//时间
#define SN_DATETIME     "datetime"
#define KN_DATEMODE     "datemode"
#define KN_TIMEMODE     "timemode"
  
//背景屏保
#define SN_BGSCREEN     "bgscreen"
#define KN_BG           "bkground"
#define KN_SS           "ssaver"
//方案
#define SN_COLORCASE    "case"
#define KN_CASENAME     "casename"
#define KN_BARCOLOR     "bar"
#define KN_WINCOLOR     "win"
#define KN_FOCUSCOLOR   "focus"

//声音
#define SN_SOUND        "ring"
#define KN_TEL          "tel"
#define KN_SMS          "sms"
#define KN_BTN          "btn"
#define KN_INITRING     "initring"
#define KN_RINGTYPE     "ringtype"
#define KN_RINGVOLUM    "ringvolu"

//笔校准
#define SN_PENCALIB     "pencalib"
#define KN_PEN_STATE    "penstate"
#define KN_PEN_PHY0X    "phy0x"
#define KN_PEN_PHY0Y    "phy0y"
#define KN_PEN_PHY1X    "phy1x"
#define KN_PEN_PHY1Y    "phy1y" 

//网络
#define SN_NET          "net"
#define KN_NET_MODE     "netmode"

//语言
#define SN_LANGUAGE     "language"
#define KN_LANGUAGEID   "id"

#define  SETUP_OWNER		1
#define  SETUP_PASSWORD		2
#define  SETUP_POWER		3
#define  SETUP_PENINPUT		4
#define  SETUP_SOUND		5

//external functions

/**********读取拨号设置******************************************/
#define    DIALDEF_WAP      1
#define    DIALDEF_MAIL     2
extern void IspReadInfo( UDB_ISPINFO * ,int );
extern int  IspIsNum(void);
extern BOOL Setup_ReadDefDial(UDB_ISPINFO  * ,int ); 
//第一次建立拨号设置文件
extern  void SetUp_SetDialCfg(void);
extern  int  Setup_GetDialType(int nType);
/**************************************************************/
//调用笔校准
extern BOOL Setup_ShowPenCalib();
//调用密码
int Password ( char* pBuffer )	;

#define  PWD_PASSED    301     //验证通过
#define  PWD_NEEDED    302     //验证失败
#define  SIM_DESTROY   303     //SIM卡毁坏  
/****************************************************************/
#define NAMELEN     20
#define WM_DATEWND_QUIT		WM_USER+12 

//setting file name
static char SetupFileName[] = "FLASH2:Setting.ini"; 
static char FlashSetupFile[] = "RAM:Setting.txt";
static char ColorCase[] = "FLASH2:colorcase.ini";


//颜色方案
typedef struct tagColor
{	
    COLORREF  colorwin;
    COLORREF  colorbar;
    COLORREF  colorfocus;
    char      cName[15];
}COLORCASE;

#define  SN_GETBARCOLOR    0x01
#define  SN_GETWINCOLOR    0x02
#define  SN_GETFOCUSCOLOR  0x03
extern COLORREF SetupGetDeskColor(int iIndex);

//声音播放
#define RINGID_TEL    0
#define RINGID_SMS    1
#define RINGID_INIT   2
extern void Setup_PlayRing(int, BOOL);
extern int Setup_RingGetMode(void);
//convert an integer ( < 10,000 ) to a string
void IntToStr(char *pStr, int num);

//PIN码
extern void SetupSetPinEquery(BOOL bAct);
extern BOOL SetupGetPinEquery(void);

//**********************************调整对比度所需部分*****************************************
#define		SN_DISPLAY_CONTRAST		"contrast"				//对比度的值
#define		KN_CONTRASTVAL			"contrast_val"			//对比度的值,默认为37(即0x25)范围[32,39]
#define		CONTRAST_DEFAULT		37						//对比度的值
//************************************密码设置所需部分**************************************************
#define		SN_PWS					SN_PASSWORD				//密码节点名
#define		KN_PWS_PHONE			KN_PWD_PHONE			//手机密码
#define		KN_PWS_AUTOLOCK			"pws_autolock"			//是否打开上锁功能(0为关闭,1为打开)
#define		PASSWORD_DEFAULT		"000000"				//手机密码默认值

#define		PWSV_LOCKING			0//设置部分中现在上锁时的手机密码验证
#define		PWSV_STARTUP			1//启动手机时的手机密码验证
extern		BOOL	CallPwsValidate(HWND hPasswordValidate,int itype);//itype:当前需要验证的密码类型
//本函数显示密码验证的窗口,窗口中显示一个EDIT控件和"确定","取消"按钮,提示用户输入手机密码.
//itype:当前需要验证的密码类型
//例如在开机时要验证手机密码,只需直接调用这个函数就可以了:CallPwsValidate(hwnd,PWSV_STARTUP),
//如果当前开机密码设置为关闭,函数会自动退出,不会提示输入密码,否则会提示输入密码,密码输入正确之后,才会退出函数.

//***********************************节电模式设置所需部分************************************************
#define		SN_THRIFT				"SN_Thrift"				//节电模式
#define		KN_THRIFT				"KN_Hhrift"				//0为关闭节点模式,1为启动节电模式,默认值为0

//***********************************情景模式设置所需部分************************************************
#define		SCENEMODENUM			6						//情景模式个数
#define		SCENEMODENUMDEF			0						//初始时选择的情景模式编号的默认值

#define		SN_SCENEMODE			"scenemode"				//情景模式的结点
#define		KN_SCENEMODENUM			"scenemodenum"			//用户选择的情景模式的关键字
//iscenemode:情景模式的编号
//iscenemode=0:标准模式
//iscenemode=1:会议模式
//iscenemode=2:户外模式
//iscenemode=3:办公模式
//iscenemode=4:静音模式
//iscenemode=5:自定义模式

#define		RINGNAMEMAXLEN			30						//铃声文件名数组的长度(包括路径)

/*typedef struct											//用于铃音选择的结构
{
	char		cShortInfo[RINGNAMEMAXLEN];					//短消息提示音的文件名(包括路径)
	char		cOn_Off[RINGNAMEMAXLEN];					//开关机铃音文件名(包括路径)
	char		cOnoff_Top[RINGNAMEMAXLEN];					//开关盖铃音文件名(包括路径)
	char		cAlarmClock[RINGNAMEMAXLEN];				//闹钟提示音文件名(包括路径)
}RINGSELECT;
*/
typedef	struct								//读取设置情景模式设置所需的结构
{
	int			iComeview;					//振铃模式(0:铃声;1:振动;2:振动铃声;3:静音)
	//int			iDiabolotype;				//响铃方式(0:持续;1:持续一次;2:持续两次)
	//RINGSELECT	rRingselect;				//铃声选择的结构
	int			iRingvolume;				//铃声音量(音量的数字,1为最小音量,6为最大音量,1,2,3,4,5,6)
	int			iHeadphones;				//听筒音量(音量的数字,1为最小音量,6为最大音量,1,2,3,4,5,6)
	//int			iShortInfoview;				//短消息提示音	(0:关闭;1:打开)
	//int			iKeypressvolume;			//按键音			(0:关闭;1:打开)
	//int			iWarningvolume;				//告警音			(0:关闭;1:打开)
	int			iGetvolume;					//接通提示音		(0:关闭;1:打开)
	char		cKeypress[RINGNAMEMAXLEN];	//按键音(""为关闭,否则为按键音的文件名,包括路径)
}SCENEMODE;

static char Setup_SceneModeFN[] = "FLASH2:smseting.ini";	//保存各个情景模式信息的文件
extern	BOOL	GetSM(SCENEMODE * scenemode,int iscenemode);//读取情景模式的各个参数值
extern	BOOL	SetSM(SCENEMODE   scenemode,int iscenemode);//设置情景模式的各个参数值
//iscenemode:情景模式的编号
//iscenemode=0:标准模式
//iscenemode=1:会议模式
//iscenemode=2:户外模式
//iscenemode=3:办公模式
//iscenemode=4:静音模式
//iscenemode=5:自定义模式

//***********************************铃声选择中组的选择所需部分***********************************************
#define		GROUPNUM				10		//组的个数
#define		GROUPNAMEMAXLEN			20		//组名的最大长度
#define		ICONAMEMAXLEN			20		//组的图标名的最大长度

typedef struct								//有关组的信息
{
	char	cGroupName[GROUPNAMEMAXLEN];	//组的名字
	char	cRingName[RINGNAMEMAXLEN];		//这个组的铃声文件名(包括路径)

#define		GROUPIMAGE				1
#if GROUPIMAGE
	char	cIcoName[ICONAMEMAXLEN];		//组的图标名
#else
	int		iIcoNameIndex;					//组的图标的索引
#endif
}GROUPINFO;

extern	BOOL	GetGroupInfo(GROUPINFO * groupinfo,int igroupnum);//读取相应组的信息
extern	BOOL	SetGroupInfo(GROUPINFO   groupinfo,int igroupnum);//设置单个组的信息
//igroupnum:组的编号,0 ～ GROUPNUM - 1,一共GROUPNUM个组.

typedef	struct								//铃声设置的所需的结构
{
	char	cRingsetSMS[RINGNAMEMAXLEN];	//短信/彩信铃声
	char	cRingsetOpen[RINGNAMEMAXLEN];	//开机铃声
	char	cRingsetAlarm[RINGNAMEMAXLEN];	//日程表闹铃的铃声
}RINGSETTING;
extern	void	GetRingSet(RINGSETTING * ringsetting);				//读取铃声信息

#define		CAPTIONMAXLEN			20		//窗口标题最大长度
//***********************************铃声选择部分***********************************************

typedef	struct								//存储ROM中的铃声的结构
{
	char pringname[RINGNAMEMAXLEN];			//铃声的实际文件名
	char pshowname[RINGNAMEMAXLEN];			//铃声的显示的名字
} RINGNAMES;

static RINGNAMES ringnames[] = 
{
	{""			,""},//无
	{"ROM:0.mmf",""},//单身情歌
	{"ROM:1.mmf",""},//男儿当自强
	{"ROM:2.mmf",""},//简单爱
	{"ROM:3.mmf",""},//上海滩
	{"ROM:4.mmf",""},//好心情
	{"ROM:5.mmf",""},//笑声
	{"NULL","NULL"}
};

typedef	struct
{
	HWND	hCallWnd;
	int		hCallMsg;
	char	titlecaption[CAPTIONMAXLEN];
	char	pSelRingname[RINGNAMEMAXLEN];
}FUNCRINGSELECT;
extern	BOOL	CallRingSelect(FUNCRINGSELECT * ringselect);//显示铃声列表
extern	void	GetReplyData(char * ringname,LPARAM lParam ,unsigned int buflen);
/*
CallRingSelect函数显示一个窗口,窗口中有一个列表框,当前ROM,FLASH2中的铃声文件显示在列表框中。
对于ROM的铃声,显示其对应的中文名称,对于FLASH2中的铃声,只显示其实际文件名.

用户选择窗口的"确定"按钮之后,由回调消息返回选中的铃声的实际文件名包括路径(需要调用GetReplyData函数).
用户选择窗口的"取消"按钮之后,回调消息中的LPARAM=0
函数执行成功WPARAM=1
函数执行失败WPARAM=0

hCallWnd	:回调模块的指定窗口句柄
hCallMsg	:回调模块的指定消息响应
titlecaption:窗口的标题
pSelRingname:需要选中铃声的文件名

返回说明：
接收到返回消息后需要调用GetReplyData(char * ringname,LPARAM lParam ,unsigned int buflen)返回结果。
ringname:返回选中的文件名字符串
LPARAM:回调消息的lParam
buflen:调用方定义的需要返回的字符串的长度.注意:没有边界检查.
*/
//*********************************显示选项窗口部分*************************************
typedef	struct 
{
	HWND	hCallWnd;					//回调模块的指定窗口句柄
	int		hCallMsg;					//回调模块的指定消息响应
	char	titlecaption[CAPTIONMAXLEN];//窗口的标题   
	int		iControlType;				//需要显示的控件类型
#define		RADIOBUTTON				1	//显示两个单选按钮(打开和关闭)
#define		CHECKBOXBUTTON			2	//显示一个复选按钮
#define		STRSPINBOX				3	//显示一个StrSpinBox控件(打开和关闭)
	int		iValue;						//使窗口中相关控件选中的值(非零或者0)
	char	CheckTxt[10];				//复选按钮的提示文字.
	char	Clew[30];					//显示在窗口中的提示文字(可有可无)
	int		iPosition;					//Clew相对于StrSpinBox控件和复选按钮的位置
#define		OPTION_TOP				1	//Clew在控件的上边
#define		OPTION_LEFT				2	//Clew在控件的左边
//(iPosition未指定则Clew在控件的左边,Clew的长度要减小,否则可能显示不全,这一点要程序员控制)
}OPTIONSELECT;
extern	BOOL	CallOptionWindow(OPTIONSELECT * optionselect);
/*
CallOptionWindow函数显示一个窗口,窗口中显示了3类控件,控件的类型由iControlType变量决定
iControlType = RADIOBUTTON:窗口中显示了两个单选按钮,对应的文字为"打开"和"关闭"
	当iValue = 1时,单选按钮的"打开"会被选中.
	当iValue = 0时,单选按钮的"关闭"会被选中.
	否则两个都不会选中
iControlType = CHECKBOXBUTTON:窗口会显示一个复选按钮,对应的文字包含在CheckTxt中.
	当iValue = 非0时,复选按钮会被选中.否则不会选中.
iControlType = STRSPINBOX:窗口会显示一个StrSpinBox控件,控件中包含两个选项,文字为"打开"和"关闭"
	当iValue = 非0时,StrSpinBox控件会显示"打开"选项.
	当iValue = 0时,StrSpinBox控件会显示"关闭"选项.

用户选择窗口的"确定"按钮之后
iControlType = RADIOBUTTON:
如果选中"打开"按钮,LPARAM = 1;如果选中"关闭"按钮,LPARAM = 0,如果两个按钮都没优选中,LPARAM = -1
iControlType = CHECKBOXBUTTON:
如果选择了复选按钮,LPARAM = 1;如果没有选择复选按钮,LPARAM = 0
iControlType = STRSPINBOX:
如果选择"打开"按钮,LPARAM = 1;如果选择"关闭"按钮,LPARAM = 0

用户选择窗口的"取消"按钮之后,回调消息中的LPARAM = -2
函数执行成功WPARAM=1
函数执行失败WPARAM=0
*/
//***************************************背光灯设置部分********************************
#define		SN_BGLIGHT				"SN_bglight"			//背灯光节点名
#define		KN_KEYBOARD				"KN_bglight_keyboard"	//背灯光-键盘关键字名,0表示"关闭",5表示"5秒",10表示"10秒",15表示"15秒".默认值为0
#define		KN_SCREEN				"KN_bglight_screen"		//背灯光-屏幕关键字名,20表示"20秒",20表示"30秒",60表示"60秒",90表示"90秒".默认值为20
//**************************************桌面短语部分************************
#define		SALUTATORY_LIMIT		12						//桌面短语字符串最大长度(6个汉字/12个英文)
extern		void	GetSalutatory(char * cSalutatory);		//获得桌面短语的函数(cSalutatory的长度应为SALUTATORY_LIMIT + 1)

extern		int		GetSystemUsableWH(int);					//获得屏幕中可以利用的宽度或者高度
#endif