 
#ifndef _ME_WIN_H_
#define _ME_WIN_H_

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
    char  cmd[560];		//AT����
	char* data;			//���Ӳ���
	int	  datalen;		//��������
    int   timeout;		//��ʱ
    int   line_count;   //����
    int	  line_len;     //�г���
    void* revbuf;       //������ָ��
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
	ME_RS_NOSIMCARD,
	ME_RS_NOTREADY
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
#if defined INFINION_MODULE
	ME_RS_SUCCESS,			//"OK"
	ME_RS_FAILURE,			//"ERROR","CME ERROR","CMS ERROR"
	ME_RS_BUSY,				//"BUSY"
	ME_RS_NOANSWER,			//"NO ANSWER"
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
#if defined ADI_MODULE
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
    ME_URC_CSSI,            //"+CSSI:<code>[,<index>]"
	ME_URC_CSSU,			//"+CSSU: <code2>[,<index>,<number>,<type>]"
	ME_URC_SSTN,
	ME_URC_SMGO,
	ME_URC_RUI,				//"+RUI: <UserInfo>"
	ME_URC_RING,			//"RING"
	ME_URC_NOCARRIER,		//"NO CARRIER"
	ME_URC_SYSSTART,
	ME_URC_UNDERVOLTAGE
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
	ME_URC_SATI,
	ME_URC_RING,
	ME_URC_NOCARRIER,
	ME_URC_SATN
#endif
#if defined INFINION_MODULE
	ME_URC_CALL,			//"+XCALLSTAT:"
	ME_URC_CALA,			//"+CALA: <timestring>,<index>"
	ME_URC_CBM,				//"+CBM: <length><pdu>	
	ME_URC_CBMI,			//"+CBMI: "BM",<index>"
	ME_URC_CCCM,			//"+CCCM: <ccm>"
	ME_URC_CCWA,			//"+CCWA: <number>,<type>,<class>[,<alpha>]"
	ME_URC_CDS,				//"+CDS: <length><pdu>"
	ME_URC_CDSI,			//"+CDSI: <mem>,<index>"
	ME_URC_CLIP,			//"+CLIP: <number>,<type>[,,,<alpha>]"
	ME_URC_CMT,				//"+CMT: <length><pdu>"
	ME_URC_CMTI,			//"+CMTI: <mem>,<index>
	ME_URC_CREG,			//"+CREG: <stat>[,<lac>,<ci>]"
	ME_URC_CGREG,
	ME_URC_CRING,			//"+CRING: <type>"
	ME_URC_CSSU,			//"+CSSU: <code2>[,<number>,<type>]"
	ME_URC_STK,				//"+STKPRO:"
	ME_URC_RING,			//"RING"
	ME_URC_NOCARRIER		//"NO CARRIER"
#endif
#if defined ADI_MODULE
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
	ME_URC_NOCARRIER,		//"NO CARRIER"
	ME_URC_SYSSTART,
	ME_URC_UNDERVOLTAGE
#endif
};

enum ind_mod {IND_EXCLUSIVE, IND_PRIORITY, IND_ORDINAL};
enum line_switch { 
	CUT_HELD,			// ���еȴ�ʱæ�����߹Ҷϱ��ֵ�ͨ��
	CUT_ACTIVED,		// �Ҷϼ����ͨ��
	ACTIVE_SWITCH,		// �ڱ��ֺͼ����ͨ��״̬���л�
	ADD_TO_ACTIVE,		// �����ֵ�ͨ�����뼤��ͨ��
	CUT_ALL,			// �Ҷ����б��ֺͼ����ͨ��
	ACTIVE_SWITCH_X,	// ����ָ������������ͨ����Ϊ����
	CUT_ACTIVED_X,		// �Ҷ�ָ����ŵļ���ͨ��
    CALL_TRANSFER
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
	CALLBAR_BAOC,			//�������в����绰
	CALLBAR_BOIC,			//�������в������ʳ�;
	CALLBAR_BOIC_EXHC,		//�������в������ʳ�;�����˱���
	CALLBAR_BAIC,			//�������в���绰
	CALLBAR_BIC_ROAM,		//���ƹ�������ʱ����
	CALLBAR_UAB,			//�������ͨ������
	CALLBAR_UAG,			//������в�������
	CALLBAR_UAC,			//������в�������
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
#if defined INFINION_MODULE
	SMS_MEM_BM
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

#if defined INFINION_MODULE
#define ME_PHONENUM_LEN			44
#else
#define ME_PHONENUM_LEN			24
#endif

#define ME_PHONETXT_LEN			16
#define ME_SMS_TIME_LEN			40
#define ME_SMS_CONTEXT_LEN		164
#define ME_OPRATOR_LEN			20
#define ME_MBID_LEN				40
#if defined INFINION_MODULE
#define ME_STKIND_LEN			200
#endif

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

} ME_PHONE_ITEM, *PME_PHONE_ITEM;

typedef struct tagME_CSSI_ITEM
{
	int		Action;
    int     Index;
} ME_CSSI_ITEM, *PME_CSSI_ITEM;

typedef struct tagME_CSSU_ITEM
{
	int		Action;
    int     Index;
	char	PhoneNum[ME_PHONENUM_LEN];
}ME_CSSU_ITEM, *PME_CSSU_ITEM;

typedef struct tagME_CMTI_ITEM{
	int		DevType;
	int		Index;
} ME_CMTI_ITEM, *PME_CMTI_ITEM;

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

#if defined INFINION_MODULE

typedef struct tagME_CALL_STAT{
	int		id;
	int		stat;
#define CALL_STAT_DISCONNECT		6

}ME_CALL_STAT, *PME_CALL_STAT;

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
#define CALL_STAT_ACTIVE		0
#define CALL_STAT_HELD			1
#define CALL_STAT_DIALING		2
#define CALL_STAT_ALERTING		3
#define CALL_STAT_INCOMING		4
#define CALL_STAT_WAITING		5
	int		Stat;								// call state

#define CALL_MODE_VOICE			0
#define CALL_MODE_DATA			1
#define CALL_MODE_FAX			2
#define CALL_MODE_UNKNOWN		9
	int		Mode;								// call mode

#define	CALL_DIR_ORIGINATED		0
#define CALL_DIR_TERMINATED		1
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
enum sms_type { SMS_SUBMIT, SUBMIT_REPORT, STATUS_REPORT, SMS_DELIVER };
typedef struct tagSMS_Send
{
	char	SCA[ME_PHONENUM_LEN];
	int		Type;
	int		udhl;
	int		dcs;
#define DCS_GSM         0
#define DCS_8BIT        1
#define DCS_UCS2        2
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
#define SMS_1HOUR	0	//һСʱ
#define SMS_6HOUR	1	//��Сʱ
#define SMS_24HOUR	2	//��ʮ��Сʱ
#define SMS_72HOUR	3	//��ʮ��Сʱ
#define SMS_1WEEK	4	//һ����
#define SMS_MAXTIME	5	//�ʱ��

	char	PhoneNum[ME_PHONENUM_LEN];
	char	UDH[ME_SMS_CONTEXT_LEN];
	char	Context[ME_SMS_CONTEXT_LEN];

}SMS_SEND, *PSMS_SEND;

typedef struct tagSMS_INFO
{
	int		Index;
	int		Stat;
	int		Type;
	int		Status;
	int		dcs;
	int		udhl;
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
	char	UDH[ME_SMS_CONTEXT_LEN];
	char	Context[ME_SMS_CONTEXT_LEN];

}SMS_INFO, *PSMS_INFO;

enum cbs_code {
    CBS_ALPHA_GSM,
    CBS_ALPHA_DATA,
    CBS_ALPHA_UCS2
};

#define CBS_LANG_GERMAN     0x00000001
#define CBS_LANG_ENGLISH    0x00000002
#define CBS_LANG_ITALIAN    0x00000004
#define CBS_LANG_FRENCH     0x00000008
#define CBS_LANG_SPANISH    0x00000010
#define CBS_LANG_DUTCH      0x00000020
#define CBS_LANG_SWEDISH    0x00000040
#define CBS_LANG_DANISH     0x00000080
#define CBS_LANG_PORTUGUESE 0x00000100
#define CBS_LANG_FINNISH    0x00000200
#define CBS_LANG_NORWEGIAN  0x00000400
#define CBS_LANG_GREEK      0x00000800
#define CBS_LANG_TURKISH    0x00001000
#define CBS_LANG_HANGARIAN  0x00002000
#define CBS_LANG_POLISH     0x00004000
#define CBS_LANG_CZECH      0x00008000
#define CBS_LANG_RESERVE    0x00010000

enum cbs_class {
    CBS_CLASS0,
    CBS_CLASS1,
    CBS_CLASS2,
    CBS_CLASS3,
    CBS_NOCLASS
};
// Cell Broadcasting Message Info
typedef struct tagCBSInfo {
    unsigned short  SNum;       // Serial Number
    unsigned short  MId;        // Message Identifier
    int             Code;       // Alphabet
    int             Class;      // Msg Class
    unsigned long   Lang;       // Language
    int             PageNum;    // Total number of page
    int             PageRef;    // page reference
    int             DataLen;
    char            Data[94];   // Message Content
} CBS_INFO, *PCBS_INFO;

enum ussdresponse 
{
    ME_USSD_ABORT,
    ME_USSD_REFUSE,
    ME_USSD_NOTIFY,
    ME_USSD_REQUEST,
    ME_USSD_DISCONNECT
};

typedef struct tagUSSDInfo {
    int             Flag;
    int             Code;
    unsigned long   Lang;
    int             DataLen;
    char            Data[160];
} USSD_INFO, *PUSSD_INFO;

typedef struct tagSMSMem_Info
{
	int		nMem1Used;
	int		nMem1Total;
	int		nMem2Used;
	int		nMem2Total;
#if defined SIEMENS_TC35 || defined TI_MODULE || defined INFINION_MODULE || defined ADI_MODULE
	int		nMem3Used;
	int		nMem3Total;
#endif

}SMSMEM_INFO, *PSMSMEM_INFO;

typedef struct tagIND_STATUS
{
	int		Mt;			//����֪ͨ��ʽ
#define IND_NONE		0	//�¶��ŵ���ʱ��֪ͨ
#define IND_BUFFER		1	//�¶��ŵ���ʱ�ȴ洢��֪ͨ
#define IND_DIRECT		2	//�¶��ŵ���ʱֱ��֪ͨ
	int		Bm;			//С���㲥֪ͨ��ʽ
	int		Ds;			//״̬����֪ͨ��ʽ
}IND_STATUS, *PIND_STATUS;

typedef struct tagCELL_BROADCAST
{
	BOOL mode;
	char ids[40];
	unsigned long code;

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

#define MODE_AUTO     0
#define MODE_MANUAL   1
#define MODE_UNREGIST 2


#define	FORMAT_LONGALPHA	0
#if defined WAVECOM_2C || defined TI_MODULE || defined INFINION_MODULE || defined ADI_MODULE
#define FORMAT_SHORTALPHA	1
#endif
#define	FORMAT_NUMERIC		2

typedef struct tagCurOprator
{
	int Mode;
	int Format;
	char Name[ME_OPRATOR_LEN];

}CUROPRATOR, *PCUROPRATOR;

#define STAT_UNKNOWN	0
#define STAT_AVAILABLE	1
#define STAT_CURRENT	2
#define STAT_FORBIDDEN	3

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
	MIN_LEVEL,		//��С��������Ҫ���
	MAX_LEVEL,		//�������������
	CHG_IN,			//��������
	CHG_OUT			//������
};

#elif defined SIEMENS_TC35

enum charge_mode
{
	CHG_OFF,		//δ���ӳ����
	CHG_CONNECT,	//���ӳ����
	CHG_INPROG,		//��������
	CHG_FINISH,		//������
	CHG_ERROR,		//������
	CHG_HIGHTMP		//����¶ȹ���
};

#else

enum
{
	BATTERY_INUSE,
	BATTERY_NOUSE,
	BATTERY_NOCON,
	POWER_FAULT
};

#endif

#define ME_BATTERY_NORMAL		0
#define ME_BATTERY_WARNING		1
#define ME_BATTERY_SHUTDOWN		2

typedef struct tagBATTERY_INFO
{
	int BCMode;
	int BCValue;
	int BCNotify;
	int Consuming;

}BATTERY_INFO, *PBATTERY_INFO;

typedef enum { CLIR_DEFAULT, CLIR_ENABLE, CLIR_DISABLE } CLIR_MODE;
typedef enum { CLIR_NPROVIS, CLIR_PERMANENT, CLIR_UNKNOWN, CLIR_TEMP_RES, CLIR_TEMP_ALLOW } CLIR_STATUS;

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

#if defined INFINION_MODULE
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

enum SS {
    SSCD,       /* Call Deflection */
    SSCLIP,     /* Call Line Identification Presentation */
    SSCLIR,     /* Call Line Identification Restriction */
    SSCOLP,     /* Connected Line Identification Presentation */
    SSCOLR,     /* Connected Line Identification Restriction */
    SSCF,         /* Call Forwarding */
    SSCNAP,       /* Calling Name Presentation */
    SSCW,         /* Call Waiting */
    SSHOLD,       /* Call Hold */
    SSMPTY,       /* Multi Party Service */
    SSCUG,        /* Closed User Group */
    SSAOCI,       /* Advice of Charge Information */
    SSAOCC,       /* Advice of Charge Charging */
    SSUUS,        /* User to User Signalling */
    SSCB,         /* Call Barring */
    SSECT,        /* Explicit Call Transfer */
    SSeMLPP,      /* enhanced Multi-Level Precedence and Pre-emption */
    SSSPNP,       /* Support of Private Numbering Plan */
    SSCCBS,       /* Completion of calls to busy Subscribers */
    SSUNKNOWN      
};

typedef enum {
    SSReg,
    SSAct,
    SSInt,
    SSDeact,
    SSErase
} SSOp;

typedef enum {
    BSVOICE,    // Voice
    BSDATA,     // Data
    BSFAX,      // FAX
    BSSMS,      // Short Message
    BSDCS,      // Data circuit sync
    BSDCA,      // Data circuit async
    BSDPAC,     // Dedicated packet access
    BSDPAD      // Dedicated PAD access
} SSBS;

typedef enum {
    CFU,
    CFB,
    CFNRy,
    CFNRc
} CFTYPE;

typedef struct tagCFInfo {
    CFTYPE  Reason;
    BOOL    Active;
    SSBS    BS;
    int     Time;
    char    DN[ME_PHONENUM_LEN];
}CFINFO, *PCFINFO;

typedef enum {
    BAOC,
    BOIC,
    BOIC_exHC,
    BAIC,
    BIC_Roam
} CBTYPE;

typedef struct tagCBInfo {
    CBTYPE Fac;
    BOOL   Active;
    SSBS    BS;
} CBINFO, *PCBINFO;

typedef struct tagCWInfo {
    BOOL    Active;
    SSBS    BS; 
} CWINFO, *PCWINFO;

typedef enum {
    CLIPNONE,
    CLIPALLOW,
    CLIPUNKNOWN
} CLIPSTATUS;

typedef struct tagCLIPInfo {
    CLIPSTATUS Status;
}CLIPINFO, *PCLIPINFO;

typedef enum {
    CLIRNONE,
    CLIRPERMANENT,
    CLIRUNKNOWN,
    CLIRTEMPRES,
    CLIRTEMPALLOW
} CLIRSTATUS;

typedef struct tagCLIRInfo {
    CLIRSTATUS Status;
}CLIRINFO, *PCLIRINFO;


typedef struct tagCOLPInfo {
    BOOL Active;
} COLPINFO, *PCOLPINFO;

typedef struct tagCOLRInfo {
    BOOL Active;
} COLRINFO, *PCOLRINFO;

typedef struct tagME_PUC {
    char currency[4];
    char ppu[21];
    char unused1;
    char unused2;
    char unused3;
} ME_PUCINFO, *PME_PUCINFO;

typedef struct tagME_ALARMTIME
{
	unsigned char year;
	unsigned char month;
	unsigned char day;
	unsigned char hour;
	unsigned char minute;
	unsigned char second;
	unsigned char unused1;
	unsigned char unused2;
}ME_ALARMTIME, *PME_ALARMTIME;

int ME_SetAlarm             (HWND hwnd, int msg, ME_ALARMTIME* time, int index);
int ME_GetAlarm             (HWND hwnd, int msg, int index);
int ME_SetSpeakerVolume     (HWND hwnd, int msg, int level);
int ME_GetSpeakerVolume     (HWND hwnd, int msg);
int ME_GSMInit_Module       (HWND hWnd, UINT Msg);
int ME_GSMInit_Module_2     (HWND hWnd, UINT Msg);
int ME_GSMInit_Module_3     (HWND hWnd, UINT Msg);
#if defined WAVECOM_2C
int ME_GSMInit_SIM          (HWND hWnd, UINT Msg);
#endif

int ME_RegisterIndication   (int MsgClass, int Mode, HWND hwnd, UINT msg);
BOOL ME_UnRegisterIndication (int Handle);

int ME_CancelFunction       (int Handle);
int ME_GetResult            (void* result, int result_len);

int ME_SpecialDial          (HWND hWnd, int nMsg, char * ssCode, BOOL callControl);
int ME_VoiceDial            (HWND hWnd, int Msg, char* PhoneNum, char* Extention);
int ME_VoiceHangup          (HWND hWnd, int Msg);
int ME_CallAnswer           (HWND hWnd, int Msg);
int ME_ListCurCalls         (HWND hWnd, int Msg);
int ME_SwitchCallStatus     (HWND hWnd, int Msg, int Style, int Index);
int ME_SetAutoAnswerStatus  (HWND hWnd, int Msg, int Times);
int ME_GetAutoAnswerStatus  (HWND hWnd, int Msg);
int ME_SetCallWaitingStatus (HWND hWnd, int Msg, BOOL bEnable, int Class);
int ME_GetCallWaitingStatus (HWND hWnd, int Msg, int Class);
int ME_SetCallForwardStatus (HWND hWnd, int Msg, PCALL_FWD pCallFwd);
int ME_GetCallForwardStatus (HWND hWnd, int Msg, int Style, int Class);
int ME_SetCallBarringStatus (HWND hWnd, int Msg, int Style,BOOL bEnable, char* Password, int Class);
int ME_GetCallBarringStatus (HWND hWnd, int Msg, int Style, int Class, char* Password);

int ME_SMS_Send             (HWND hWnd, int Msg, PSMS_SEND pSend);
int ME_SMS_Read             (HWND hWnd, int Msg, BOOL bPreRead, int MemType, int Index);
int ME_SMS_List             (HWND hWnd, int Msg, int Type, BOOL bPreRead, int MemType);
int ME_SMS_Delete           (HWND hWnd, int Msg, int MemType, int Index);
int ME_SMS_SetSCA           (HWND hWnd, int Msg, char* SCA);
int ME_SMS_GetSCA           (HWND hWnd, int Msg);
int ME_SMS_SetIndStatus     (HWND hWnd, int Msg, PIND_STATUS pStatus);
int ME_SMS_GetIndStatus     (HWND hWnd, int Msg);
int ME_SMS_SetCellBroadcast (HWND hWnd, int Msg, PCELL_BROADCAST pStatus);
int ME_SMS_GetCellBroadcast (HWND hWnd, int Msg);
int ME_SMS_GetMemStatus     (HWND hWnd, int Msg, int MemType);
int ME_SMS_Acknowledge      (HWND hwnd, int msg);

int ME_ReadPhonebook        (HWND hWnd, int Msg, int Type, int IndexBegin, int IndexEnd);
int ME_WritePhonebook       (HWND hWnd, int Msg, int Type, PPHONEBOOK Record);
int ME_GetPhonebookMemStatus(HWND hWnd, int Msg, int Type);

int ME_SetPhoneLockStatus   (HWND hWnd, int Msg, int Type, char* Password, BOOL bEnable);
int ME_GetPhoneLockStatus   (HWND hWnd, int Msg, int Type, char* Password);
int ME_ChangeLockPassword   (HWND hWnd, int Msg, int Type, char* OldPass, char* NewPass);
int ME_PasswordValidation   (HWND hWnd, int Msg, int Type, char* Password, char* NewPass);
int ME_GetCurWaitingPassword(HWND hWnd, int Msg);
int ME_GetPassInputLimit    (HWND hWnd, int Msg, int Type);

int ME_SetOpratorMode       (HWND hWnd, int Msg, int Mode, char* OperId);
int ME_GetCurOprator        (HWND hWnd, int Msg, int Format);
int ME_SearchNetOprator     (HWND hWnd, int Msg);

#if defined SIEMENS_TC35
int ME_ReadPrefOperList     (HWND hWnd, int Msg, int IndexBegin, int IndexEnd);
#else
int ME_ReadPrefOperList     (HWND hWnd, int Msg);
#endif

int ME_WritePrefOperList    (HWND hWnd, int Msg, int index, char* oper_id);
int ME_GetEquipmentId       (HWND hWnd, int Msg);
int ME_GetSuscriberId       (HWND hWnd, int Msg);
int ME_SetClockTime         (HWND hWnd, int Msg, char* DateTime);
int ME_GetClockTime         (HWND hWnd, int Msg);
int ME_MuteControl          (HWND hWnd, int Msg, BOOL bEnable);
int ME_SendDTMF             (HWND hWnd, int Msg, char SendChar);
int ME_FactoryDefaultSetting(HWND hWnd, int Msg);
int ME_ResetModule          (HWND hWnd, int Msg);
int ME_ShutdownModule       (HWND hWnd, int Msg);
int ME_GetBatteryInfo       (HWND hWnd, int Msg);
int ME_GetSignalInfo        (HWND hWnd, int Msg);

int ME_GetCCM               (HWND hWnd, int nMsg);
int ME_GetACM               (HWND hWnd, int nMsg);
int ME_GetACMmax            (HWND hWnd, int nMsg);
int ME_ResetACM             (HWND hWnd, int nMsg, char * Password);
int ME_SetACMmax            (HWND hWnd, int nMsg, unsigned long ACMmax, char * Password);
int ME_GetPricePerUnit      (HWND hWnd, int nMsg);
int ME_SetPricePerUnit      (HWND hWnd, int nMsg, PME_PUCINFO Puc, char * Password);

int ME_GetUSSDInfo          (PUSSD_INFO info);
int ME_USSD_SetNotify       (HWND hWnd, int nMsg);
void * ME_USSD_Request      (char * str);
int ME_USSD_Response        (void * Obj, char * str);
int ME_USSD_Abort           (void * Obj);

int ME_GetLastError         (HWND hWnd, int Msg);
#define ME_E_MAXACM         (0x00080044L)
#define ME_E_CALLBARRED      (0x0000012CL)

#if defined WAVECOM_2C
int ME_STK_Send             (HWND hwnd, int msg, BOOL direct, STK_PARAM* param);
#else
int ME_STK_Send             (HWND hwnd, int msg, STK_PARAM* param);
#endif

#if defined SIEMENS_TC35
int ME_SleepModule          (HWND hwnd, int msg);
int ME_STK_GetRemoteSAT     (HWND hwnd, int msg);
int ME_GetServCellInfo      (HWND hwnd, int msg);
int ME_GetNeibCellInfo      (HWND hwnd, int msg);
#define     ME_AUDIO_HEADSET    1
#define     ME_AUDIO_HANDSET    2
int ME_SelectAudioMode      (HWND hwnd, int msg, int mode);

#endif
#if defined INFINION_MODULE
int ME_STK_GetRemoteSAT     (HWND hwnd, int msg);
int ME_GetServCellInfo      (HWND hwnd, int msg);
int ME_GetSoftwareVersion   (HWND hwnd, int msg);
int ME_DataDial             (HWND hwnd, int msg, char* PhoneNum);
int ME_DataHangup           (HWND hwnd, int msg);
int ME_GPRSDial             (HWND hwnd, int msg, char* PhoneNum);
int ME_GPRSHangup           (HWND hwnd, int msg);
#endif

#if defined WAVECOM_2C
int ME_SMS_SetFormat        (HWND hwnd, int msg, BOOL bPDU);
int ME_SMS_SetProtocol      (HWND hwnd, int msg, BOOL bPhasePlus);
int ME_DataDial             (HWND hwnd, int msg, char* PhoneNum);
int ME_DataHangup           (HWND hwnd, int msg);
int ME_GPRSDial             (HWND hwnd, int msg, char* PhoneNum);
int ME_GPRSHangup           (HWND hwnd, int msg);
int ME_SendATCmd            (HWND hwnd, int msg, char* cmd, int timeout);
int ME_SetCallLineIndRestriction(HWND hwnd, int msg, CLIR_MODE para);
int ME_GetCallLineIndRestriction(HWND hwnd, int msg);

#endif

#if defined TI_MODULE
int ME_SendATCmd            (HWND hwnd, int msg, char* cmd, int timeout);
int ME_SMS_SetFormat        (HWND hwnd, int msg, BOOL bPDU);
int ME_SMS_SetProtocol      (HWND hwnd, int msg, BOOL bPhasePlus);
int ME_GetSTKMainMenu       (char* buf, int buflen);

int ME_SetCallLineIndRestriction(HWND hwnd, int msg, CLIR_MODE para);
int ME_GetCallLineIndRestriction(HWND hwnd, int msg);
int ME_DisableSIMCard       (HWND hwnd, int msg);
int ME_SetVBDR              (HWND hwnd, int msg);
int ME_SetAudioMode         (HWND hwnd, int msg, BOOL bEarPhone);
int ME_PhonebookReady       (HWND hwnd, int msg);

#endif
int ME_MuxPowerUp();

#ifdef __cplusplus
};
#endif

#endif
