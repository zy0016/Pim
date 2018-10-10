 /**************************************************************************\
*
*                      Hopen Software System
*
* Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
*                       All Rights Reserved
*
* Model   :		MEdrv
*
* FileName:		MEdrv.h
*
* Purpose :		Provide extern interface
*  
* Author  :		Ding Yong
*
\**************************************************************************/
#ifndef _ME_DRV_H_
#define _ME_DRV_H_

#include "hp_modcfg.h"

extern BOOL		ME_Initialize();
enum ind_mod {IND_EXCLUSIVE, IND_PRIORITY, IND_ORDINAL};	
extern HANDLE	ME_AppRegister( char* keyword, int Mode, HWND hwnd, UINT msg );
extern BOOL		ME_AppUnregister( HANDLE hApp );
extern int		ME_GetURCData( HANDLE hApp, void* buf, unsigned int buflen );
extern int		ME_Function( DWORD type, void* info, unsigned int infolen );
extern int		ME_GetReplyData( DWORD at_type, void* buf, unsigned int buflen );
extern HANDLE	ME_SendATCommand( char* command, HWND hwnd, UINT msg, int timeout );
extern int		ME_RcvATResponse( HANDLE hSender, void* buf, unsigned int buflen );

#define ME_ATCMD_LEN		52
#define ME_URC_LEN			12

#define ME_TELEPHONE_STRLEN			24		// Telephone Number
#define ME_TELTEXT_STRLEN			16		// Telephone Text
#define ME_TIME_STRLEN				40		// Time
#define ME_SMS_CONTEXT				164		// SMS Context
#define ME_OPRT_NAMELEN				20		// Net Oprator
#define ME_MBID_LEN					40

// AT_Type
#define ME_BREAK_DIAL		1
#define ME_VOICE_DIAL		2
#define ME_VOICE_HANGUP		3
#define ME_VOICE_ANSWER		4
#define ME_CURRENT_CALLS	5
#define ME_SET_CALLWAITING	6
#define ME_SWITCH_CALLLINE	7
#define ME_SET_FWDCALL		8
#define ME_QRY_FWDCALL		9
#define ME_QRY_CALLWAITING	10

#define ME_SMS_PDU_SEND			11
#define ME_SMS_PDU_READ			12
#define ME_SMS_PDU_DELETE		13
#define ME_SMS_SCA				14
#define ME_SMS_PDU_LIST			15
#define ME_SMS_MEMCOUNT			16
#define ME_SMS_GETINDSTATUS		17
#define ME_SMS_SETINDSTATUS		18
#define ME_SET_CELLBROADCAST	19
#define ME_GET_CELLBROADCAST	20

#define ME_PBOOK_READ			21
#define ME_PBOOK_WRITE			22
#define ME_PBOOK_DELETE			23
#if defined WAVECOM_2C
#define ME_PB_PNOFIND			24
#define ME_PB_TEXTFIND			25
#define ME_PB_STEPREAD			26
#endif
#define ME_PB_MEMCOUNT			27
#if defined SIEMENS_TC35
#define ME_CALL_TIME			28
#endif

#define ME_PIN_ENABLE			31
#define ME_PIN_CONFIRM			32
#define ME_PIN_MODIFY			33
#define ME_PIN_QUERY			34
#define ME_PIN_REMAIN			35
#define ME_PIN_EQUERY			36
#define ME_SET_CALLBARRING		37
#define ME_QRY_CALLBARRING		38
#define ME_PIN2_CONFIRM			39

#define ME_SET_OPRATOR			41
#define ME_QRY_OPRATOR			42
#define ME_SRCH_OPRATOR			43
#define ME_QRY_SIGNAL			44
#define ME_QRY_BATTERY			45
#define ME_GET_POCOUNT			46
#define ME_GET_POPRATOR			47
#define ME_SET_POPRATOR			48

#define ME_CLOCK_CONFIG		51
#define ME_LOAD_PROFILE		52	
#define ME_GETIDENTIFIER	53
#define ME_MUTE_CONTROL		54
#define ME_RESET			55
#define ME_SHUTDOWN			56
#define ME_SEND_DTMF		57

#define ME_DATA_DIAL		61
#define ME_DATA_HANGUP		62
#define ME_GPRS_DIAL		63
#define ME_GPRS_HANGUP		64
#define ME_CALL_AUTOANSWER	65

// result code
#define ME_RS_SUCCESS				0		//"OK"
#define ME_RS_GNR_FAILURE			1		//"ERROR"
#define ME_RS_GSM_FAILURE			2		//"CME ERROR"
#define ME_RS_SMS_FAILURE			3		//"CMS ERROR"
#define ME_RS_BUSY					4		//"BUSY"
#if defined WAVECOM_2C
#define ME_RS_NOANSWER				5		//"NO ANSWER"
#endif
#if defined SIEMENS_TC35
#define ME_RS_NODIALTONE			5		//"NO DIALTONE"
#endif
#define ME_RS_NOCARRIER				6		//"NO CARRIER"
#define ME_RS_NOPIN					7		//"+CPIN READY"
#define ME_RS_PIN					8		//"+CPIN SIM PIN"
#define ME_RS_PUK					9		//"+CPIN SIM PUK"
#define ME_RS_PIN2					10		//"+CPIN SIM PIN2"
#define ME_RS_PUK2					11		//"+CPIN SIM PUK2"
#define ME_RS_SIMPHLOCK				12		//"+CPIN PH-SIM PIN"
#define ME_RS_SIMNTLOCK				13		//"+CPIN PH-NET PIN"
#define ME_RS_CONNECT				14		//"CONNECT"
#define ME_RS_TIMEOUT				15		//"+CPIN READY"

#define ME_CRING_VOICE		1
#define ME_CRING_FAX		2
#define ME_CRING_ASYNC		3
#define ME_CRING_REL_ASYNC	4

typedef struct tagME_PHONE_ITEM{
	char	PhoneNum[ME_TELEPHONE_STRLEN];
	char	PhoneStr[ME_TELTEXT_STRLEN];
}ME_PHONE_ITEM, *PME_PHONE_ITEM;

typedef struct tagME_CSSU_ITEM{
	int		Action;
//ME_CSSU_ITEM.Action
#define	ME_CSSU_CALL_ON_HOLD 			2
#define	ME_CSSU_CALL_RETRIEVED			3
#define	ME_CSSU_HELDCALL_TERMINATED		133

	char	PhoneNum[ME_TELEPHONE_STRLEN];
}ME_CSSU_ITEM, *PME_CSSU_ITEM;


typedef struct tagME_CMTI_ITEM{
	int		DevType;
//ME_CMTI_ITEM.DevType
#define ME_SMS_MEM_SM		1
#if defined SIEMENS_TC35
#define ME_SMS_MEM_ME		2
#define ME_SMS_MEM_MT		3
#endif
#if defined WAVECOM_2C
#define ME_SMS_MEM_BM		4
#define ME_SMS_MEM_SR		5
#endif

	int		Index;
}ME_CMTI_ITEM, *PME_CMTI_ITEM;

#if defined SIEMENS_TC35
#define ME_SMSBUF_FREE			0
#define ME_SMSBUF_OVERFLOW		1
#define ME_SMSBUF_WAITING		2
#endif

typedef struct tagME_CREG_RESULT
{
	int		Mode;
#define		CREG_DISABLE_URC	0
#define		CREG_ENABLE_URC		1
#define		CREG_ENABLE_LOC		2
	int		Stat;
#define		CREG_NOREG			0
#define		CREG_HOMEREG		1
#define		CREG_SEARCHING		2
#define		CREG_DENIED			3
#define		CREG_UNKNOWN		4
#define		CREG_ROAMING		5

}ME_CREG_RESULT, ME_CGREG_RESULT, *PME_CREG_RESULT, *PME_CGREG_RESULT;

typedef struct tagME_CREG_URC
{
	int		Stat;		//same to result
	char	lac[6];		//location area code
	char	ci[6];		//cell id
}ME_CREG_URC, ME_CGREG_URC, *PME_CREG_URC, *PME_CGREG_URC;

#if defined WAVECOM_2C
typedef struct tagME_WIND_ITEM{
	int		Event;
//ME_WIND_ITEM.Event
#define ME_WIND_INIT_COMPLETE	4
#define	ME_WIND_CALL_CREATED	5
#define	ME_WIND_CALL_RELEASED	6
	int		CallID;
}ME_WIND_ITEM, *PME_WIND_ITEM;
#endif

typedef struct tagNETOPREATOR{
	int		stat;
#define STAT_UNKNOWN	0
#define STAT_AVAILABLE	1
#define	STAT_CURRENT	2
#define STAT_FORBIDDEN	3

	char	alpha_oper[ME_OPRT_NAMELEN];
	char	num_oper[ME_OPRT_NAMELEN];
}NETOPRATOR, *PNETOPRATOR;

typedef struct tagQUERY_OPERATOR{
	HWND	hCallWnd;
	int		hCallMsg;
	int		Format;
#define LONG_ALPHA		0
#if defined WAVECOM_2C
#define SHORT_ALPHA		1
#endif
#define NUMERIC			2
}QUERY_OPRATOR, *PQUERY_OPRATOR;

typedef struct tagSET_OPERATOR
{
	HWND	hCallWnd;
	int		hCallMsg;

	int		Mode;
#define MOD_AUTO		0	// 自动模式
#define MOD_UNREGIST	2	// 注销网络
#define MOD_MANUAL		4	// 手动切换网络

	char	num_oper[ME_OPRT_NAMELEN];

}SET_OPRATOR, *PSET_OPRATOR;

typedef struct SEARCH_OPRATOR
{
	HWND	hCallWnd;
	int		hCallMsg;

}SEARCH_OPRATOR, *PSEARCH_OPRATOR;

typedef struct tagOpQuery_Reply
{
	int Mode;
	int Format;
	char OperName[ME_OPRT_NAMELEN];

}REPLY_OPQUERY, *PREPLY_OPQUERY;

typedef struct tagOpSearch_Reply
{
	int		GetCount;		// 读到的服务商个数
	int		BufCount;		// Content缓冲含有NETOPRATOR的个数
	NETOPRATOR*	Content;		// 指向NETOPRATOR的指针
}REPLY_OPSRCH, *PREPLY_OPSRCH;


extern int		ME_AnaURC_CRING( char *SrcStr );
extern int		ME_AnaURC_CLIP( char *SrcStr,  PME_PHONE_ITEM pItem );
extern int		ME_AnaURC_CCWA( char * SrcStr, PME_PHONE_ITEM pItem );
extern int		ME_AnaURC_CSSU( char *SrcStr, PME_CSSU_ITEM pItem );
#if defined WAVECOM_2C
extern int		ME_AnaURC_WIND( char *SrcStr, PME_WIND_ITEM pItem );
#endif
#if defined SIEMENS_TC35
extern int		ME_AnaURC_SMGO( char *SrcStr );
#endif
extern int		ME_AnaURC_CMTI( char *SrcStr, PME_CMTI_ITEM pItem );
extern int		ME_AnaURC_STK( char *SrcStr );
extern int		ME_AnaRES_Query_COPS( char *SrcStr, PREPLY_OPQUERY pItem );
extern int		ME_AnaRES_Search_COPS( char *SrcStr, PNETOPRATOR pItem, int Count );
extern int		ME_AnaRES_CREG( char *SrcStr, PME_CREG_RESULT pItem );
extern int		ME_AnaURC_CREG( char *SrcStr, PME_CREG_URC pItem );
extern int		ME_AnaRES_CGREG( char *SrcStr, PME_CGREG_RESULT pItem );
extern int		ME_AnaURC_CGREG( char *SrcStr, PME_CGREG_URC pItem );
extern int		ME_AnaRES_CCID( char *SrcStr, char* pCardId );
struct tagSMS_INFO;
extern int		ME_AnaURC_CMT( char* SrcStr, struct tagSMS_INFO* info );
extern int		ME_AnaURC_CBM( char* SrcStr, struct tagSMS_INFO* info );
extern int		ME_AnaURC_CDS( char* SrcStr, struct tagSMS_INFO* info );

// ME_BREAK_DIAL
typedef struct tagBreak_Dial
{
	HWND	hCallWnd;
	int		hCallMsg;
}BREAK_DIAL, *PBREAK_DIAL;

// ME_VOICE_DIAL
typedef struct tagVoice_Call
{
	HWND	hCallWnd;
	int		hCallMsg;

	char	PhoneNum[ME_TELEPHONE_STRLEN];
	char	Extension[ME_TELEPHONE_STRLEN];	// optional when dialing, set ""!

}VOICE_CALL, *PVOICE_CALL;

typedef struct tagData_Dial
{
	HWND	hCallWnd;
	int		hCallMsg;

	char	PhoneNum[ME_TELEPHONE_STRLEN];
}DATA_DIAL, GPRS_DIAL, *PDATA_DIAL, *PGPRS_DIAL;

// ME_VOICE_HANGUP
// ME_VOICE_ANSWER
typedef struct tagVoice_Hangup_Answer
{
	HWND	hCallWnd;
	int		hCallMsg;

}VOICE_HANGUP, VOICE_ANSWER, DATA_HANGUP, GPRS_HANGUP, *PVOICE_HANGUP, *PVOICE_ANSWER, *PDATA_HANGUP, *PGPRS_HANGUP;


// ME_SMS_PDU_SEND
typedef struct tagSMS_PduSend
{
	HWND	hCallWnd;
	int		hCallMsg;

	char	SCA[ME_TELEPHONE_STRLEN];
	int		Type;
	int		ConLen;
	BOOL	Report;
	BOOL	ReplyPath;
	int		PID;
#define		PID_IMPLICIT	0
#define		PID_TELEX		1
#define		PID_TELEFAX		2
#define		PID_VOICE		3
#define		PID_ERMES		4
#define		PID_NPAGING		5
#define		PID_X400		6
#define		PID_EMAIL		7

	int		VPvalue;
#define SMS_1HOUR	0	//一小时
#define SMS_6HOUR	1	//六小时
#define SMS_24HOUR	2	//二十四小时
#define SMS_72HOUR	3	//七十二小时
#define SMS_1WEEK	4	//一星期
#define SMS_MAXTIME	5	//最长时间

	char	PhoneNum[ME_TELEPHONE_STRLEN];
	char	Context[ME_SMS_CONTEXT];

}SMS_PDUSEND, *PSMS_PDUSEND;


// ME_SMS_PDU_READ
typedef struct tagSMS_PduRead{

	HWND	hCallWnd;
	int		hCallMsg;

	int		Action;			// 0:普通读； 1:预读
#define		NORMAL_READ		0
#define		PRE_READ		1
	char	Order[4];
	int		MemType;
}SMS_PDUREAD, *PSMS_PDUREAD;


// ME_SMS_PDU_DELETE
typedef struct tagSMS_PduDelete{

	HWND	hCallWnd;
	int		hCallMsg;

	char	Order[4];
	int		MemType;

}SMS_PDUDELETE, *PSMS_PDUDELETE;

typedef struct tagSMS_INFO
{
	int		Index;
	int		Stat;
	int		Type;
#define		TXT_SMS			1
#define		DATA_SMS		2
#define		STATUS_REPORT	3
#define		SUBMIT_REPORT	4
#define		SMS_DELIVER		5

	int		Status;

	int		ConLen;
	char	SCA[ME_TELEPHONE_STRLEN];
	char	SenderNum[ME_TELEPHONE_STRLEN];
	char	SendTime[ME_TIME_STRLEN];
	char	Context[ME_SMS_CONTEXT];
}SMS_INFO, *PSMS_INFO;

typedef struct tagSMS_LstRead
{
	HWND	hCallWnd;
	int		hCallMsg;

	int		Action;		//0 for normal read, 1 for pre read
	int		LstType;
#define UNREAD			0
#define READED			1
#define UNSEND			2
#define SENDED			3
#define ALL				4
	int		MemType;

}SMS_LSTREAD, *PSMS_LSTREAD;

typedef struct tagSMS_LSTREADREPLY
{
	int			GetCount;	// the count of info read
	int			BufCount;	
	SMS_INFO*	Content;	// pointer of SMS_INFO, According to Count
}REPLY_SMSLREAD, *PREPLY_SMSLREAD;

typedef struct tagSMSMemCount
{
	HWND hCallWnd;
	int hCallMsg;
	int MemType;

}SMS_MEMCOUNT, *PSMS_MEMCOUNT;

typedef struct tagSMSMemCountReply
{
	int nMem1Used;
	int nMem1Total;
	int nMem2Used;
	int nMem2Total;
}REPLY_SMSMCOUNT, *PREPLY_SMSMCOUNT;

typedef struct tagSMS_SCA{
	HWND	hCallWnd;
	int		hCallMsg;
	int		OpType;
#define GET_SCA			1
#define SET_SCA			2

	char	SCANo[ME_TELEPHONE_STRLEN];

}SMS_SCA, *PSMS_SCA;

typedef struct tagCELL_BROADCAST
{
	BOOL mode;
	char ids[40];
	char code[20];

}CELL_BROADCAST, *PCELL_BROADCAST;

typedef struct tagSET_CELLBROADCAST
{
	HWND	hCallWnd;
	int		hCallMsg;
	CELL_BROADCAST cb;

}SET_CELLBROADCAST, *PSET_CELLBROADCAST;

typedef struct tagGET_CELLBROADCAST
{
	HWND	hCallWnd;
	int		hCallMsg;

}GET_CELLBROADCAST, *PGET_CELLBROADCAST;

typedef struct tagIND_STATUS
{
	int		Mt;			//短信通知方式
#define IND_NONE		0	//新短信到来时不通知
#define IND_BUFFER		1	//新短信到来时先存储后通知
#define IND_DIRECT		2	//新短信到来时直接通知
	int		Bm;			//小区广播通知方式
	int		Ds;			//状态报告通知方式
}IND_STATUS, *PIND_STATUS;

// ME_SMS_GETINDSTATUS
typedef struct tagSET_IND_STATUS
{

	HWND	hCallWnd;
	int		hCallMsg;
	IND_STATUS Status;

}SET_IND_STATUS, *PSET_IND_STATUS;

// ME_SMS_SETINDSTATUS
typedef struct tagGET_IND_STATUS
{
	HWND	hCallWnd;
	int		hCallMsg;
}GET_IND_STATUS, *PGET_IND_STATUS;

// ME_FWD_XXXX
typedef struct tagSetCallFwd{
	HWND	hCallWnd;
	int		hCallMsg;
	BOOL	Enable;		// 0:Disable, 1:Enable
	char	Type[4];	// "0":unconditional, "1":busy, "2":no reply, "3":not reachable
#define ME_FWD_UNCDTIONAL	"0"
#define ME_FWD_BUSY			"1"
#define ME_FWD_NOREPLY		"2"
#define ME_FWD_NOREACH		"3"
#define ME_FWD_ALLCDTIONAL	"5"
#define ME_FWD_ALL			"4"
	int		Class;
#define ME_CLASS_VOICE		1
#define ME_CLASS_DATA		2
#define ME_CLASS_FAX		3
#define ME_CLASS_SMS		4
#define ME_CLASS_ALL		5

	char	PhoneNum[ME_TELEPHONE_STRLEN];

}SET_CALLFWD, *PSET_CALLFWD;

// ME_FWD_XXXX
typedef struct tagQueryCallFwd
{
	HWND	hCallWnd;
	int		hCallMsg;
	char	Type[4];	// "0":unconditional, "1":busy, "2":no reply, "3":not reachable
	int		Class;		//	Same to SET_CALLFWD

}QUERY_CALLFWD, *PQUERY_CALLFWD;

typedef struct tagQueryCallFwdReply
{
	BOOL	Enable;		// FALSE:Disable, TRUE:Enable
	char	PhoneNum[ME_TELEPHONE_STRLEN];

}REPLY_QUERYCALLFWD, *PREPLY_QUERYCALLFWD;

typedef struct tagPHONEBOOK{
	char	order[4];
	char	PhoneNum[ME_TELEPHONE_STRLEN];
	char	Text[ME_TELTEXT_STRLEN];
}PHONEBOOK, *PPHONEBOOK;

// ME_PBOOK_READ one or more
typedef struct tagPB_READ{
	HWND	hCallWnd;
	int		hCallMsg;

	char	MStype[8];		// Memory Storage Type
#define SIM			"\"SM\""
#define LASTDIAL	"\"LD\""
#define MISSCALL	"\"MC\""
#define RECCALL		"\"RC\""
	char	ReadBegin[4];	// read begin
	char	ReadEnd[4];		// read end
}PB_READ, *PPB_READ;

typedef struct tagPB_READREPLY
{
	int			GetCount;		// the count of info read
	int			BufCount;		// Number of Phonebook Content contains
	PHONEBOOK*	Content;		// pointer of PHONEBOOK[], According to Count
}REPLY_PBREAD, *PREPLY_PBREAD;

// ME_PBOOK_WRITE one
typedef struct tagPB_WRITE{
	HWND	hCallWnd;
	int		hCallMsg;

	char	Order[4];						// optional when write, set ""!
	char	PhoneNum[ME_TELEPHONE_STRLEN];
	char	Text[ME_TELTEXT_STRLEN];
}PB_WRITE, *PPB_WRITE;

// ME_PBOOK_DELETE one
typedef struct tagPB_DELETE{
	HWND	hCallWnd;
	int		hCallMsg;
	char	Order[4];						// optional when write, set ""!
}PB_DELETE, *PPB_DELETE;

// Check Phonebook Memory
typedef struct tagPB_MEMCOUNT
{
	HWND	hCallWnd;
	int		hCallMsg;
}PB_MEMCOUNT, *PPB_MEMCOUNT;

typedef struct tagPB_MEMCOUNTREPLY
{
	char	MemType[4];
	int		Used;
	int		Total;
}REPLY_PBMEMCOUNT, *PREPLY_PBMEMCOUNT;

// ME_SET_CALLBARRING
typedef struct tagSet_CallBarring
{
	HWND hCallWnd;
	int  hCallMsg;

	int	 Type;
#define BAOC		0		//限制所有拨出电话
#define BOIC		1		//限制所有拨出国际长途
#define BOIC_EXHC	2		//限制所有拨出国际长途，除了本国
#define BAIC		3		//限制所有拨入电话
#define BIC_ROAM	4		//限制国际漫游时来电
#define UAB			5		//解除所有通话限制
#define UAG			6		//解除所有拨出限制
#define UAC			7		//解除所有拨入限制

	BOOL Enable;
	int  Class;
	char Password[12];
}SET_CALLBARRING, *PSET_CALLBARRING;

// ME_QRY_CALLBARRING
typedef struct tagQry_CallBarring
{
	HWND hCallWnd;
	int  hCallMsg;

	int	 Type;				//same to set opration,except UAB,UAG,UAC
	int  Class;
	char Password[12];
}QRY_CALLBARRING, *PQRY_CALLBARRING;

// ME_Enable_PIN
typedef struct tagSIMPIN_SWITCH{
	HWND	hCallWnd;
	int		hCallMsg;

	int		Type;
	BOOL	bEnable;		//PIN Enable Or Not
	char	pPassword[12];	//PIN Password
}SIMPIN_SWITCH, *PSIMPIN_SWITCH;


typedef struct tagSIMPIN_CONFIRM{
	HWND	hCallWnd;
	int		hCallMsg;
	int		PassType;
#define SIM_PIN				1
#define SIM_PUK				2

	char	PINPass[12];	//PIN Password
	char	PUKPass[12];	//PUK Password
}SIMPIN_CONFIRM, *PSIMPIN_CONFIRM, SIMPIN2_CONFIRM, *PSIMPIN2_CONFIRM;

typedef struct tagSIMPIN_REMAIN{
	HWND	hCallWnd;
	int		hCallMsg;

}SIMPIN_REMAIN, *PSIMPIN_REMAIN;

typedef struct tagReply_SIMPINRemain
{
	int		PIN1Remain;
	int		PIN2Remain;
	int		PUK1Remain;
	int		PUK2Remain;
}REPLY_SIMPINREMAIN, *PREPLY_SIMPINREMAIN;

enum
{
	LOCK_PS,		//SIM LOCK
	LOCK_SC,		//PIN
	LOCK_AO,		//Barr all outgoing call
	LOCK_OI,		//Barr outgoing international call
	LOCK_OX,		//Barr international except to home country
	LOCK_AI,		//Barr all incoming call
	LOCK_IR,		//Barr inc. when roaming
	LOCK_AB,		//All barring service
	LOCK_AG,		//All outgoing barring service
	LOCK_AC,		//All incoming barring service
	LOCK_PN,		//Network lock
	LOCK_PU,		//Network subset lock
	LOCK_PP,		//Service Provider lock
	LOCK_PC,		//Corporate lock
	LOCK_P2			//PIN2
};
typedef struct tagSIMPIN_MODIFY{
	HWND	hCallWnd;
	int		hCallMsg;

	int		Type;
	char	pOldPass[12];	//Old PIN password
	char	pNewPass[12];	//New PIN password
}SIMPIN_MODIFY, *PSIMPIN_MODIFY;

typedef struct tagSIMPIN_QUERY{
	HWND	hCallWnd;
	int		hCallMsg;
}SIMPIN_QUERY, *PSIMPIN_QUERY;

typedef struct tagSIMPIN_EQUERY{
	HWND	hCallWnd;
	int		hCallMsg;
}SIMPIN_EQUERY, *PSIMPIN_EQUERY;

typedef struct tagMOBILE_IDENTIFIER{
	HWND	hCallWnd;
	int	    hCallMsg;
}MOBILE_IDENTIFIER, *PMOBILE_IDENTIFIER;

typedef struct tagSET_CALLWAITING{
	HWND	hCallWnd;
	int	    hCallMsg;
	BOOL	bEnable;			//是否开启呼叫等待

}SET_CALLWAITING, *PSET_CALLWAITING;

typedef struct tagQRY_CALLWAITING{
	HWND	hCallWnd;
	int	    hCallMsg;

}QRY_CALLWAITING, *PQRY_CALLWAITING;

typedef struct tagSWITCH_CALLLINE{
	HWND	hCallWnd;
	int	    hCallMsg;
	int		SwitchStyle;	// 切换方式
#define CUT_HELD		0	// 呼叫等待时忙音或者挂断保持的通话
#define CUT_ACTIVED		1	// 挂断激活的通话
#define ACTIVE_SWITCH	2	// 在保持和激活的通话状态间切换
#define ADD_TO_ACTIVE	3	// 将保持的通话加入激活通话
#define CUT_ALL			4	// 挂断所有保持和激活的通话
#define ACTIVE_SWITCH_X	6	// 将除指定序号外的所有通话变为保持
#define CUT_ACTIVED_X	5	// 挂断指定序号的激活通话
	int		index;			// 通话序号,只当SwitchStyle为ACTIVE_SWITCH_X和CUT_ACTIVE_X时有效

}SWITCH_CALLLINE, *PSWITCH_CALLLINE;

typedef struct tagCLOCK_CONFIG{
	HWND	hCallWnd;
	int		hCallMsg;
	int		OpType;
#define GET_CLOCK		0
#define SET_CLOCK		1
	char	DateTime[ME_TIME_STRLEN];

}CLOCK_CONFIG, *PCLOCK_CONFIG;

typedef struct tagLOAD_PROFILE{
	HWND	hCallWnd;
	int		hCallMsg;
}LOAD_PROFILE, *PLOAD_PROFILE;

typedef struct tagFUNC_RESET{
	HWND	hCallWnd;
	int		hCallMsg;
}FUNC_RESET, *PFUNC_RESET;

typedef struct tagFUNC_SHUTDOWN{
	HWND	hCallWnd;
	int		hCallMsg;
}FUNC_SHUTDOWN, *PFUNC_SHUTDOWN;

#if defined WAVECOM_2C
typedef struct tagPB_PNOFIND{
	HWND	hCallWnd;
	int		hCallMsg;
	char	PhoneNo[ME_TELEPHONE_STRLEN];
}PB_PNOFIND, *PPB_PNOFIND;

typedef struct tagPB_TEXTFIND{
	HWND	hCallWnd;
	int		hCallMsg;
	char	Text[12];

}PB_TEXTFIND, *PPB_TEXTFIND;

typedef struct tagPBTxtFind_Reply
{
	int			GetCount;
	int			BufCount;
	PHONEBOOK*	Content;
}REPLY_PBTXTFIND, *PREPLY_PBTXTFIND;

typedef struct tagPB_STEPREAD{
	HWND	hCallWnd;
	int		hCallMsg;
	int		ReadType;
#define FIRST_ITEM		0
#define LAST_ITEM		1
#define NEXT_ITEM		2
#define PREV_ITEM		3
#define LAST_READ		4
#define LAST_WRITE		5
}PB_STEPREAD, *PPB_STEPREAD;
#endif

typedef struct tagDTMF
{
	HWND	hCallWnd;
	int		hCallMsg;
	char	SendChars[30];

}DTMF_DATA, *PDTMF_DATA;

typedef struct tagMuteControl
{
	HWND	hCallWnd;
	int		hCallMsg;
	BOOL	bMute;

}MUTE_CONTROL, *PMUTE_CONTROL;


// ME_CURRENT_CALLS
typedef struct tagCALLS_INFO{
	int		index;
#define	ME_RS_ORIGINATED	'0'
#define ME_RS_TERMINATED	'1'
	int		Dir;		// call direction 

#define ME_RS_ACTIVE		'0'
#define ME_RS_HELD			'1'
#define ME_RS_DIALING		'2'
#define ME_RS_ALERTING		'3'
#define ME_RS_INCOMING		'4'
#define ME_RS_WAITING		'5'
	int		Stat;		// call state

#define ME_RS_VOICE			'0'
#define ME_RS_DATA			'1'
#define ME_RS_FAX			'2'
#define ME_RS_UNKNOWN		'9'
	int		Mode;		// call mode

#define ME_RS_NOTMULTIPARTY		'0'
#define ME_RS_MULTIPARTY		'1'
	int		Mpty;		// whether one of multiparty

	char	PhoneNum[ME_TELEPHONE_STRLEN];	// responding call
	char	Text[ME_TELTEXT_STRLEN];		// content of call, maybe ""

}CALLS_INFO, *PCALLS_INFO;

typedef struct tagCUR_CALLS{
	HWND	hCallWnd;
	int		hCallMsg;

}CUR_CALLS, *PCUR_CALLS;

typedef struct tagCURCALL_REPLY
{
	int			GetCount;		// the count of info read
	int			BufCount;
	CALLS_INFO*	Content;		// pointer of CALLS_INFO[], According to Count
}REPLY_CURCALLS, *PREPLY_CURCALLS;

typedef struct tagQRY_BATTERY
{
	HWND hCallWnd;
	int	 hCallMsg;
}QRY_BATTERY, *PQRY_BATTERY;

typedef struct tagBATTERY_INFO
{
	int BCMode;
#if defined WAVECOM_2C
	#define MIN_LEVEL	0		//最小电量，需要充电
	#define MAX_LEVEL	1		//最大电量，充电满
	#define CHG_IN		2		//充电过程中
	#define CHG_OUT		3		//充电结束
#endif

#if defined SIEMENS_TC35
	#define CHG_OFF			0		//未连接充电器
	#define CHG_CONNECT		1		//连接充电器
	#define CHG_INPROG		2		//充电进行中
	#define CHG_FINISH		3		//充电结束
	#define CHG_ERROR		4		//充电错误
	#define CHG_HIGHTMP		5		//电池温度过高

	int Consuming;
#endif
	
	
	int BCValue;
}BATTERY_INFO, *PBATTERY_INFO;

typedef struct tagQRY_SIGNAL
{
	HWND hCallWnd;
	int hCallMsg;
}QRY_SIGNAL, *PQRY_SIGNAL;

#if defined SIEMENS_TC35
typedef struct tagCALL_TIME
{
	HWND	hCallWnd;
	int		hCallMsg;
	int		Type;
#define		LAST_CALLTIME	0
#define		TOTAL_CALLTIME	1
}CALL_TIME, *PCALL_TIME;
#endif

typedef struct tagAUTO_ANSWER
{
	HWND hCallWnd;
	int hCallMsg;
	int	Times;
}AUTO_ANSWER, *PAUTO_ANSWER;

//GetPoCount
typedef struct tagPO_COUNT
{
	HWND	hCallWnd;
	int		hCallMsg;

}PO_COUNT, *PPO_COUNT;

//Preferd Oprator
typedef struct tagPREFER_OPRATOR
{
	int		index;
	char	oper_id[ME_OPRT_NAMELEN];

}PREFER_OPRATOR, *PPREFER_OPRATOR;

//GetPOprator
typedef struct tagGET_POPRATOR
{
	HWND	hCallWnd;
	int		hCallMsg;
#if defined SIEMENS_TC35
	int		begin;
	int		end;
#endif

}GET_POPRATER, *PGET_POPRATOR;

typedef struct tagReply_GetPOprator
{
	int GetCount;
	int BufCount;
	void* Content;

}REPLY_GETPOPRATOR, *PREPLY_GETPOPRATOR;

//SetPOprator
typedef struct tagSET_POPRATOR
{
	HWND	hCallWnd;
	int		hCallMsg;

	int		index;
	char	Oper_id[ME_OPRT_NAMELEN];

}SET_POPRATOR, *PSET_POPRATOR;

#endif

