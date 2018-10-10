 
#ifndef _ME_WIN_H_
#define _ME_WIN_H_
#define VME_DEBUG  1
#ifdef __cplusplus
extern "C" {
#endif

#include "hp_modcfg.h"
#define MEINIT_ATRET_COPSLEN	16

typedef struct tag_ME_Init_GetValue{
	BYTE	CREG;
	BYTE	CGREG;
	char	COPS_Numeric[MEINIT_ATRET_COPSLEN];

}ME_INIT_GETVALUE, *PME_INIT_GETVALUE;


// Define GSM Registration Status
#define ME_CREG_ABANDON			0
#define ME_CREG_HOMENET			1
#define ME_CREG_SEARCHING		2
#define ME_CREG_DENIED			3
#define ME_CREG_UNKNOWN			4
#define ME_CREG_ROAMING			5
#define ME_CREG_INVALID			-1

// Define GPRS Registration Status
#define ME_CGREG_ABANDON		0
#define ME_CGREG_HOMENET		1
#define ME_CGREG_SEARCHING		2
#define ME_CGREG_DENIED			3
#define ME_CGREG_UNKNOWN		4
#define ME_CGREG_ROAMING		5
#define ME_CGREG_INVALID		-1

typedef struct tagSTK_PARAM
{
    char  cmd[64];			//AT命令
    int   timeout;		//超时
    int   line_count;    //行数
    int	  line_len;       //行长度
    void* revbuf;       //缓冲区指针
}STK_PARAM, *PSTK_PARAM;


enum me_result_code
{
#if defined SIEMENS_TC35
	ME_RS_SUCCESS,			//"OK"
	ME_RS_FAILURE,			//"ERROR","CME ERROR","CMS ERROR"
	ME_RS_BUSY,				//"BUSY"
	ME_RS_NODIALTONE,
	ME_RS_NOCARRIER,		//"NO CARRIER"
	ME_RS_CONNECT,			//"CONNECT"
	ME_RS_USERSTOP,			
	ME_RS_TIMEOUT,			//TIMEOUT
	ME_RS_ERRORSTATUS,
	ME_RS_NOSIMCARD,
	ME_RS_NOCONNECTION,
	ME_RS_LIMITSERVICE,
	ME_RS_RESELECTION,
	ME_RS_SEARCHING
#endif
#if defined WAVECOM_2C
	ME_RS_SUCCESS,			//"OK"
	ME_RS_FAILURE,			//"ERROR","CME ERROR","CMS ERROR"
	ME_RS_BUSY,				//"BUSY"
	ME_RS_NOANSWER,			//"NO ANSWER"
	ME_RS_NOCARRIER,		//"NO CARRIER"
	ME_RS_CONNECT,			//"CONNECT"
	ME_RS_USERSTOP,			
	ME_RS_TIMEOUT,			//TIMEOUT
	ME_RS_ERRORSTATUS,
	ME_RS_NOSIMCARD
#endif
#if defined TI_MODULE
	ME_RS_SUCCESS,			//"OK"
	ME_RS_FAILURE,			//"ERROR","CME ERROR","CMS ERROR"
	ME_RS_BUSY,				//"BUSY"
	ME_RS_NODIALTONE,
	ME_RS_NOANSWER,
	ME_RS_NOCARRIER,		//"NO CARRIER"
	ME_RS_CONNECT,			//"CONNECT"
	ME_RS_USERSTOP,			
	ME_RS_TIMEOUT,			//TIMEOUT
	ME_RS_ERRORSTATUS,
	ME_RS_NOSIMCARD
#endif
};

enum me_ur_code
{
#if defined WAVECOM_2C
	ME_URC_CALA,			//"+CALA: <timestring>,<index>"
	ME_URC_CBM,				//"+CBM: <length><pdu>	
	ME_URC_CBMI,			//"+CBMI: "BM",<index>"
	ME_URC_CCCM,			//"+CCCM: <ccm>"
	ME_URC_CCED,			//"+CCED: <values>"
	ME_URC_CCWA,			//"+CCWA: <number>,<type>,<class>[,<alpha>]"
	ME_URC_CDS,				//"+CDS: <length><pdu>"
	ME_URC_CDSI,			//"+CDSI: <mem>,<index>"
	ME_URC_CKEV,			//"+CKEV: <keynb>"
	ME_URC_CLIP,			//"+CLIP: <number>,<type>[,,,<alpha>]"
	ME_URC_CMT,				//"+CMT: <length><pdu>"
	ME_URC_CMTI,			//"+CMTI: <mem>,<index>
	ME_URC_CREG,			//"+CREG: <stat>[,<lac>,<ci>]"
	ME_URC_CGREG,
	ME_URC_CRING,			//"+CRING: <type>"
	ME_URC_CSQ,				//"+CSQ: <RxLev>,99"
	ME_URC_CSSU,			//"+CSSU: <code2>[,<number>,<type>]"
	ME_URC_STIN,			//"+STIN: <ind>"
	ME_URC_WIND,			//"+WIND: <IndicationNb>[,<CallId>]"
	ME_URC_WVMI,			//"+WVMI: <LineId>,<Status>
	ME_URC_RUI,				//"+RUI: <UserInfo>"
	ME_URC_RING,			//"RING"
	ME_URC_NOCARRIER		//"NO CARRIER"
#endif
#if defined SIEMENS_TC35
	ME_URC_CALA,			//"+CALA: <timestring>,<index>"
	ME_URC_CBM,				//"+CBM: <length><pdu>	
	ME_URC_CBMI,			//"+CBMI: "BM",<index>"
	ME_URC_CCCM,			//"+CCCM: <ccm>"
	ME_URC_CCED,			//"+CCED: <values>"
	ME_URC_CCWA,			//"+CCWA: <number>,<type>,<class>[,<alpha>]"
	ME_URC_CDS,				//"+CDS: <length><pdu>"
	ME_URC_CDSI,			//"+CDSI: <mem>,<index>"
	ME_URC_CKEV,			//"+CKEV: <keynb>"
	ME_URC_CLIP,			//"+CLIP: <number>,<type>[,,,<alpha>]"
	ME_URC_CMT,				//"+CMT: <length><pdu>"
	ME_URC_CMTI,			//"+CMTI: <mem>,<index>
	ME_URC_CREG,			//"+CREG: <stat>[,<lac>,<ci>]"
	ME_URC_CGREG,
	ME_URC_CRING,			//"+CRING: <type>"
	ME_URC_CSQ,				//"+CSQ: <RxLev>,99"
	ME_URC_CSSU,			//"+CSSU: <code2>[,<number>,<type>]"
	ME_URC_SSTN,
	ME_URC_SMGO,
	ME_URC_RUI,				//"+RUI: <UserInfo>"
	ME_URC_RING,			//"RING"
	ME_URC_NOCARRIER		//"NO CARRIER"
#endif
#if defined TI_MODULE
	ME_URC_CBM,
	ME_URC_CBMI,
	ME_URC_CDS,
	ME_URC_CDSI,
	ME_URC_CMT,
	ME_URC_CMTI,
	ME_URC_CCCM,
	ME_URC_CCWA,
	ME_URC_CCWV,
	ME_URC_CDEV,
	ME_URC_CIEV,
	ME_URC_CKEV,
	ME_URC_CLAV,
	ME_URC_CLIP,
	ME_URC_CREG,
	ME_URC_CGREG,
	ME_URC_CRING,
	ME_URC_CSSU,
	ME_URC_CUSD,
	ME_URC_STIN,
	ME_URC_RING,
	ME_URC_NOCARRIER
#endif
};

enum ind_mod {IND_EXCLUSIVE, IND_PRIORITY, IND_ORDINAL};
enum line_switch { 
	CUT_HELD,		// 呼叫等待时忙音或者挂断保持的通话
	CUT_ACTIVED,	// 挂断激活的通话
	ACTIVE_SWITCH,	// 在保持和激活的通话状态间切换
	ADD_TO_ACTIVE,	// 将保持的通话加入激活通话
	CUT_ALL,		// 挂断所有保持和激活的通话
	ACTIVE_SWITCH_X,	// 将除指定序号外的所有通话变为保持
	CUT_ACTIVED_X		// 挂断指定序号的激活通话
};
enum call_class 
{ 
	CLASS_VOICE,
	CLASS_DATA,
#if !defined TI_MODULE
	CLASS_FAX,
	CLASS_SMS,
#endif
	CLASS_ALL
};

enum callfwd_style { 
	FWD_UNCDTIONAL, 
	FWD_BUSY,
	FWD_NOREPLY,
	FWD_NOREACH,
	FWD_ALLCDTIONAL,
	FWD_ALL
};

enum lock_fac {
	CALLBAR_BAOC,			//限制所有拨出电话
	CALLBAR_BOIC,			//限制所有拨出国际长途
	CALLBAR_BOIC_EXHC,		//限制所有拨出国际长途，除了本国
	CALLBAR_BAIC,			//限制所有拨入电话
	CALLBAR_BIC_ROAM,		//限制国际漫游时来电
	CALLBAR_UAB,			//解除所有通话限制
	CALLBAR_UAG,			//解除所有拨出限制
	CALLBAR_UAC,			//解除所有拨入限制
	LOCK_PS,
	LOCK_SC,
	LOCK_PN,
	LOCK_PU,
	LOCK_PP,
	LOCK_PC,
	LOCK_FD,
	LOCK_P2
};

enum sms_memtype {
	SMS_MEM_SM,
#if defined TI_MODULE
	SMS_MEM_ME
#endif
#if defined WAVECOM_2C
	SMS_MEM_SR,
	SMS_MEM_BM
#endif
#if defined SIEMENS_TC35
	SMS_MEM_ME,
	SMS_MEM_MT
#endif
};

#if defined SIEMENS_TC35
enum sms_bufstat
{
	SMS_BUF_FREE,
	SMS_BUF_OVERFLOW,
	SMS_BUF_WAITING
};
#endif

enum sms_stat {
	SMS_UNREAD,
	SMS_READED,
	SMS_UNSEND,
	SMS_SENDED,
	SMS_ALL
};

enum pin_type 
{
	NOPIN,			//"+CPIN READY"
	PIN1,			//"+CPIN SIM PIN"
	PUK1,			//"+CPIN SIM PUK"
	PIN2,			//"+CPIN SIM PIN2"
	PUK2,			//"+CPIN SIM PUK2"
	SIMPHLOCK,		//"+CPIN PH-SIM PIN"
	SIMNTLOCK		//"+CPIN PH-NET PIN"
};

#define ME_PHONENUM_LEN			24
#define ME_PHONETXT_LEN			16
#define ME_SMS_TIME_LEN			40
#define ME_SMS_CONTEXT_LEN		164
#define ME_OPRATOR_LEN			20
#define ME_MBID_LEN				40

enum cring_type
{
	ME_CRING_VOICE,
	ME_CRING_FAX,
	ME_CRING_ASYNC,
	ME_CRING_REL_ASYNC
};

typedef struct tagME_PHONE_ITEM
{
	char	PhoneNum[ME_PHONENUM_LEN];
	char	PhoneStr[ME_PHONETXT_LEN];

}ME_PHONE_ITEM, *PME_PHONE_ITEM;

typedef struct tagME_CSSU_ITEM{
	int		Action;
#define	ME_CSSU_CALL_ON_HOLD 			2
#define	ME_CSSU_CALL_RETRIEVED			3
#define	ME_CSSU_HELDCALL_TERMINATED		133

	char	PhoneNum[ME_PHONENUM_LEN];
}ME_CSSU_ITEM, *PME_CSSU_ITEM;

typedef struct tagME_CMTI_ITEM{
	int		DevType;
	int		Index;
}ME_CMTI_ITEM, *PME_CMTI_ITEM;

typedef struct tagME_CREG_URC
{
	int		Stat;		//same to result
#define		CREG_NOREG			0
#define		CREG_HOMEREG		1
#define		CREG_SEARCHING		2
#define		CREG_DENIED			3
#define		CREG_UNKNOWN		4
#define		CREG_ROAMING		5
	char	lac[6];		//location area code
	char	ci[6];		//cell id
}ME_CREG_URC, ME_CGREG_URC, *PME_CREG_URC, *PME_CGREG_URC;

#if defined WAVECOM_2C
typedef struct tagME_WIND_ITEM{
	int		Event;

#define ME_WIND_INIT_COMPLETE	4
#define	ME_WIND_CALL_CREATED	5
#define	ME_WIND_CALL_RELEASED	6
	int		CallID;
}ME_WIND_ITEM, *PME_WIND_ITEM;
#endif


typedef struct tagCALL_FWD
{
	int		Reason;
	BOOL	bEnable;
	char	PhoneNum[ME_PHONENUM_LEN];
	int		Class;
	int		Time;
}CALL_FWD, *PCALL_FWD;

typedef struct tagCALLS_INFO
{
	int     index;								// call id
#define CALL_STAT_ACTIVE		'0'
#define CALL_STAT_HELD			'1'
#define CALL_STAT_DIALING		'2'
#define CALL_STAT_ALERTING		'3'
#define CALL_STAT_INCOMING		'4'
#define CALL_STAT_WAITING		'5'
	int		Stat;								// call state

#define CALL_MODE_VOICE			'0'
#define CALL_MODE_DATA			'1'
#define CALL_MODE_FAX			'2'
#define CALL_MODE_UNKNOWN		'9'
	int		Mode;								// call mode

#define	CALL_DIR_ORIGINATED		'0'
#define CALL_DIR_TERMINATED		'1'
	int		Dir;								// call direction

	int		Mpty;								// whether one of multiparty
	char	PhoneNum[ME_PHONENUM_LEN];			// responding call
	char	Text[ME_PHONETXT_LEN];				// content of call, maybe ""

}CALLS_INFO, *PCALLS_INFO;

typedef struct tagCallFwd_Info
{
	BOOL	Enable;		// FALSE:Disable, TRUE:Enable
	char	PhoneNum[ME_PHONENUM_LEN];
	int		Time;

} CALLFWD_INFO, *PCALLFWD_INFO;

// ME_SMS_SEND
enum sms_type { TEXT_SMS, DATA_SMS, STATUS_REPORT, SUBMIT_REPORT, SMS_DELIVER };
typedef struct tagSMS_Send
{
	char	SCA[ME_PHONENUM_LEN];
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

	char	PhoneNum[ME_PHONENUM_LEN];
	char	Context[ME_SMS_CONTEXT_LEN];

}SMS_SEND, *PSMS_SEND;

typedef struct tagSMS_INFO
{
	int		Index;
	int		Stat;
	int		Type;
	int		Status;
	int		ConLen;
	int		Class;
#define SMS_CLASS_NONE	0
#define SMS_CLASS_0		1
#define SMS_CLASS_1		2
#define SMS_CLASS_2		3
#define SMS_CLASS_3		4
	char	SCA[ME_PHONENUM_LEN];
	char	SenderNum[ME_PHONENUM_LEN];
	char	SendTime[ME_SMS_TIME_LEN];
	char	Context[ME_SMS_CONTEXT_LEN];

}SMS_INFO, *PSMS_INFO;

typedef struct tagSMSMem_Info
{
	int		nMem1Used;
	int		nMem1Total;
	int		nMem2Used;
	int		nMem2Total;
#if defined SIEMENS_TC35 || defined TI_MODULE
	int		nMem3Used;
	int		nMem3Total;
#endif

}SMSMEM_INFO, *PSMSMEM_INFO;

typedef struct tagIND_STATUS
{
	int		Mt;			//短信通知方式
#define IND_NONE		0	//新短信到来时不通知
#define IND_BUFFER		1	//新短信到来时先存储后通知
#define IND_DIRECT		2	//新短信到来时直接通知
	int		Bm;			//小区广播通知方式
	int		Ds;			//状态报告通知方式
}IND_STATUS, *PIND_STATUS;

typedef struct tagCELL_BROADCAST
{
	BOOL mode;
	char ids[40];
	char code[20];

}CELL_BROADCAST, *PCELL_BROADCAST;

typedef struct tagPHONEBOOK
{
	int		Index;
	char	PhoneNum[ME_PHONENUM_LEN];
	char	Text[ME_PHONETXT_LEN];

}PHONEBOOK, *PPHONEBOOK;

enum pbmem_type { PBOOK_SM, PBOOK_ME, PBOOK_FD, PBOOK_LD, PBOOK_MC, PBOOK_RC, PBOOK_ON };
typedef struct tagPBMEM_INFO
{
	int		Type;
	int		Used;
	int 	Total;

}PBMEM_INFO, *PPBMEM_INFO;

enum oper_mode	{ MODE_AUTO, MODE_UNREGIST, MODE_MANUAL };

enum oper_format 
{
	FORMAT_LONGALPHA,
#if defined WAVECOM_2C || defined TI_MODULE
	FORMAT_SHORTALPHA,
#endif
	FORMAT_NUMERIC
};

typedef struct tagCurOprator
{
	int Mode;
	int Format;
	char Name[ME_OPRATOR_LEN];

}CUROPRATOR, *PCUROPRATOR;

enum oper_stat { STAT_UNKNOWN, STAT_AVAILABLE, STAT_CURRENT, STAT_FORBIDDEN };
typedef struct tagNETOPREATOR
{
	int		stat;
	char	alpha_oper[ME_OPRATOR_LEN];
	char	num_oper[ME_OPRATOR_LEN];

}NETOPRATOR, *PNETOPRATOR;

typedef struct tagPREFER_OPRATOR
{
	int		index;
	char	oper_id[ME_OPRATOR_LEN];

}PREFER_OPRATOR, *PPREFER_OPRATOR;

#if defined WAVECOM_2C

enum charge_mode
{
	MIN_LEVEL,		//最小电量，需要充电
	MAX_LEVEL,		//最大电量，充电满
	CHG_IN,			//充电过程中
	CHG_OUT			//充电结束
};
#endif

#if defined SIEMENS_TC35
enum charge_mode
{
	CHG_OFF,		//未连接充电器
	CHG_CONNECT,	//连接充电器
	CHG_INPROG,		//充电进行中
	CHG_FINISH,		//充电结束
	CHG_ERROR,		//充电错误
	CHG_HIGHTMP		//电池温度过高
};
#endif
#if defined TI_MODULE
enum
{
	BATTERY_INUSE,
	BATTERY_NOUSE,
	BATTERY_NOCON,
	POWER_FAULT
};
#endif

typedef struct tagBATTERY_INFO
{
	int BCMode;
	int BCValue;
	int Consuming;

}BATTERY_INFO, *PBATTERY_INFO;
#if defined SIEMENS_TC35
typedef struct tagSAT_Param
{
	int state;
#define SAT_RESET		0
#define SAT_OFF			1
#define SAT_IDLE		2
#define SAT_PAC			3
#define SAT_WAIT		4
	
	int alphabet;
#define ALPHA_GSM		0
#define ALPHA_UCS2		1
	int instance;
#define INS_OTHER		0
#define INS_THIS		1
	char profile[40];
}SAT_PARAM, *PSAT_PARAM;
#endif

#if defined SIEMENS_TC35
typedef struct tagSERV_CELL
{
	char BCH[8];		//Broadcast Channel
	char BRXL[8];		//Receiving level of BCCH carrier
	char BCC[8];		//Base station Color Code
	char RXLev[8];		//minimal level to allow registration
	char MCC[8];		//Mobile Country Code
	char MNC[8];		//Mobile Network Code
	char BSIC[8];		//Base Station Identity Code
	char NCC[8];		//PLMN color code
	char RSSI[8];		//Received Signal Strength Indication
	char LAC[8];		//Location Area Code
	char CELLID[8];		//Cell ID
	char C1[8];
	char C2[8];
	char PWR[8];		//maximal power level used on RACH level
	char TA[8];			//Time Advance
	char TCH[8];		//Traffic Channel
	char TRXL[8];		//Receiving level of TCH carrier
	char TS[8];			//Time Slot
	char PWRLev[8];		//Current Power Level
	char Q[8];			//Receiving Quality
	char Chmod[8];		//Channel Mode
}SCELL_INFO, *PSCELL_INFO;

typedef struct tagNEIB_CELL
{
	char BCH[8];		//Broadcast Channel
	char BRXL[8];		//Receiving level of BCCH carrier
	char C1[8];
	char C2[8];
}NCELL_INFO, *PNCELL_INFO;

#endif

int ME_Initialize( int Mode );
enum me_init_mode
{
	MOD_COM,
	MOD_MUX
};

int ME_GSMInit_Module( HWND hWnd, UINT Msg );
int ME_GSMInit_Module_2( HWND hWnd, UINT Msg );
int ME_GSMInit_Module_3( HWND hWnd, UINT Msg );
#if defined WAVECOM_2C
int ME_GSMInit_SIM( HWND hWnd, UINT Msg );
#endif

int ME_RegisterIndication( int MsgClass, int Mode, HWND hwnd, UINT msg );
BOOL ME_UnRegisterIndication( int Handle );

int ME_CancelFunction( int Handle );
int ME_GetResult( void* result, int result_len );

int ME_VoiceDial( HWND hWnd, int Msg, char* PhoneNum, char* Extention );
int ME_VoiceHangup( HWND hWnd, int Msg );
int ME_CallAnswer( HWND hWnd, int Msg );
int ME_ListCurCalls( HWND hWnd, int Msg );
int ME_SwitchCallStatus( HWND hWnd, int Msg, int Style, int Index );
int ME_SetAutoAnswerStatus( HWND hWnd, int Msg, int Times );
int ME_GetAutoAnswerStatus( HWND hWnd, int Msg );
int ME_SetCallWaitingStatus( HWND hWnd, int Msg, BOOL bEnable, int Class );
int ME_GetCallWaitingStatus( HWND hWnd, int Msg, int Class );
int ME_SetCallForwardStatus( HWND hWnd, int Msg, PCALL_FWD pCallFwd );
int ME_GetCallForwardStatus( HWND hWnd, int Msg, int Style, int Class );
int ME_SetCallBarringStatus( HWND hWnd, int Msg, int Style,BOOL bEnable, char* Password, int Class );
int ME_GetCallBarringStatus( HWND hWnd, int Msg, int Style, int Class, char* Password );

int ME_SMS_Send( HWND hWnd, int Msg, PSMS_SEND pSend );

#if defined TI_MODULE
int ME_SMS_Read( HWND hWnd, int Msg, int MemType, int Index );
int ME_SMS_List( HWND hWnd, int Msg, int Type, int MemType );
#else
int ME_SMS_Read( HWND hWnd, int Msg, BOOL bPreRead, int MemType, int Index );
int ME_SMS_List( HWND hWnd, int Msg, int Type, BOOL bPreRead, int MemType );
#endif
int ME_SMS_Delete( HWND hWnd, int Msg, int MemType, int Index );
int ME_SMS_SetSCA( HWND hWnd, int Msg, char* SCA );
int ME_SMS_GetSCA( HWND hWnd, int Msg );
int ME_SMS_SetIndStatus( HWND hWnd, int Msg, PIND_STATUS pStatus );
int ME_SMS_GetIndStatus( HWND hWnd, int Msg );
int ME_SMS_SetCellBroadcast( HWND hWnd, int Msg, PCELL_BROADCAST pStatus );
int ME_SMS_GetCellBroadcast( HWND hWnd, int Msg );
int ME_SMS_GetMemStatus( HWND hWnd, int Msg, int MemType );

int ME_ReadPhonebook(HWND hWnd, int Msg, int Type, int IndexBegin, int IndexEnd);
int ME_WritePhonebook( HWND hWnd, int Msg, PPHONEBOOK Record );
int ME_GetPhonebookMemStatus( HWND hWnd, int Msg, int Type );

int ME_SetPhoneLockStatus( HWND hWnd, int Msg, int Type, char* Password, BOOL bEnable );
int ME_GetPhoneLockStatus( HWND hWnd, int Msg, int Type, char* Password );
int ME_ChangeLockPassword( HWND hWnd, int Msg, int Type, char* OldPass, char* NewPass );
int ME_PasswordValidation( HWND hWnd, int Msg, int Type, char* Password, char* NewPass );
int ME_GetCurWaitingPassword( HWND hWnd, int Msg );
int ME_GetPassInputLimit( HWND hWnd, int Msg, int Type );

int ME_SetOpratorMode( HWND hWnd, int Msg, int Mode, char* OperId );
int ME_GetCurOprator ( HWND hWnd, int Msg, int Format );
int ME_SearchNetOprator ( HWND hWnd, int Msg );
#if defined WAVECOM_2C
int ME_ReadPrefOperList( HWND hWnd, int Msg );
#endif
#if defined SIEMENS_TC35
int ME_ReadPrefOperList( HWND hWnd, int Msg, int IndexBegin, int IndexEnd );
#endif

int ME_WritePrefOperList( HWND hWnd, int Msg, int index, char* oper_id );

int ME_GetEquipmentId ( HWND hWnd, int Msg );
int ME_GetSuscriberId ( HWND hWnd, int Msg );
int ME_SetClockTime ( HWND hWnd, int Msg, char* DateTime );
int ME_GetClockTime ( HWND hWnd, int Msg );
int ME_MuteControl( HWND hWnd, int Msg, BOOL bEnable );
int ME_SendDTMF ( HWND hWnd, int Msg, char SendChar );
int ME_FactoryDefaultSetting( HWND hWnd, int Msg );
int ME_ResetModule( HWND hWnd, int Msg );
int ME_ShutdownModule( HWND hWnd, int Msg );
int ME_GetBatteryInfo( HWND hWnd, int Msg );
int ME_GetSignalInfo( HWND hWnd, int Msg );
int ME_STK_Send( HWND hwnd, int msg, STK_PARAM* param );
#if defined SIEMENS_TC35
int ME_SMS_Acknowledge( HWND hwnd, int msg );
int ME_STK_GetRemoteSAT( HWND hwnd, int msg );
int ME_GetServCellInfo( HWND hwnd, int msg );
int ME_GetNeibCellInfo( HWND hwnd, int msg );
#endif
#if defined WAVECOM_2C
int ME_DataDial( HWND hwnd, int msg, char* PhoneNum );
int ME_DataHangup( HWND hwnd, int msg );
int ME_GPRSDial( HWND hwnd, int msg, char* PhoneNum );
int ME_GPRSHangup( HWND hwnd, int msg );
#endif

#ifdef __cplusplus
};
#endif

#endif
