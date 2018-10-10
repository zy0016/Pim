#ifndef _ME_FUNCTION_H_
#define _ME_FUNCTION_H_

#define _ME_DEBUG
//#define _ME_LOG

#ifdef _ME_DEBUG
extern void debugex_nonl(char *fmt, ...);
#define MsgOut debugex_nonl
#else
#define MsgOut
#endif

#define WM_MAT_MESSAGE		(WM_USER+1001)
#define WM_FSM_START		(WM_USER+1002)

struct tagMEREQUEST;
struct ME_INDICATION;
struct ME_REGISTER;

#define	MAX_QUEUE_REQUESTS  16
#define MAX_QUEUE_INDICATS	26
#define MAX_QUEUE_REGISTER	16
#define CMD_DATALEN			512
#define DFT_TIMEOUT			8000

#define FRC_OK				0
#define FRC_ERROR			1
#define FRC_CMEERROR		2
#define FRC_CMSERROR		3
#define FRC_BUSY			4
#define FRC_NODIALTONE		5
#define FRC_NOCARRIER		6
#define FRC_CONNECT			7

enum me_fsm_event { START, DATAIN, TIMEOUT, USERSTOP, REQUEST, RESPONSE };

enum me_cmd_type
{
    ME_INIT = 0,
	ME_INIT_EQUIP,
	ME_RESET_EQUIP,
	ME_INIT_EVERYTIME,

	ME_VOICE_DIAL,
	ME_VOICE_HANGUP,
	ME_VOICE_ANSWER,
	ME_DATA_DIAL,
	ME_DATA_HANGUP,
	ME_GPRS_DIAL,
	ME_GPRS_HANGUP,
	ME_CURRENT_CALLS,
	ME_SWITCH_CALLLINE,
	ME_SET_AUTOANSWER,
	ME_GET_AUTOANSWER,
	ME_SET_CALLWAITING,
	ME_GET_CALLWAITING,
	ME_SET_CALLFWDING,
	ME_GET_CALLFWDING,
	ME_SET_CALLBARRING,
	ME_GET_CALLBARRING,

	ME_SMS_SEND,
	ME_SMS_READ,
	ME_SMS_DELETE,
	ME_SMS_LIST,
	ME_SMS_SET_SCA,
	ME_SMS_GET_SCA,
	ME_SMS_GETINDSTATUS,
	ME_SMS_SETINDSTATUS,
	ME_SET_CELLBROADCAST,
	ME_GET_CELLBROADCAST,
	ME_SMS_MEMCOUNT,
	ME_SMS_ACK,

	ME_PBOOK_READ,
	ME_PBOOK_WRITE,
	ME_PBOOK_MEMCOUNT,

	ME_SET_LOCK,
	ME_GET_LOCK,
	ME_PASS_CHANGE,
	ME_PASS_VALID,
	ME_GET_PASS,
	ME_PASS_REMAIN,

	ME_SET_OPRATOR,
	ME_GET_OPRATOR,
	ME_SRCH_OPRATOR,
	ME_GET_POPRATOR,
	ME_SET_POPRATOR,

	ME_GET_IMEI,
	ME_GET_IMSI,
	ME_SET_CLOCK,
	ME_GET_CLOCK,
	ME_MUTE_CONTROL,
	ME_SEND_DTMF,
	ME_FAC_SETTING,
	ME_RESET,
	ME_SHUTDOWN,
	ME_GET_SIGNAL,
	ME_GET_BATTERY,
	ME_STK_SEND,
	ME_STK_GETSAT,
	ME_MONI_SERV,
	ME_MONI_NEIB,
	ME_SET_VOLUME,
	ME_GET_VOLUME,
    ME_SETALARM,
    ME_GETALARM,
    ME_AUDIO,
    ME_SSCALL,
    ME_GETCCM,
    ME_GETACM,
    ME_GETACMAX,
    ME_RESETACM,
    ME_SETACMAX,
    ME_GETPUC,
    ME_SETPUC,
    ME_SLEEP,
    ME_USSD,
    ME_GETLASTERROR
};
enum ussdop
{
    ME_USSD_REQ,
    ME_USSD_RSP,
    ME_USSD_ABT
};

typedef void (*ME_ReqCallback) (struct tagMEREQUEST * pRequest);
typedef void (*ME_IndCallback) (struct ME_REGISTER *, unsigned long, unsigned long);

typedef struct tagMEREQUEST 
{
	struct tagMEREQUEST * next;	/* 连接指针 */
	int			cmd;			/* 命令 */
    int         queue;          
	unsigned long arg1;			/* 第一个参数 */
	int			arg2;			/* 第二个参数 */
	int     	arg3;			/* 第三个参数 */
    int         stat;
	void *		cmddata;		/* 命令附带的数据 */
	int			cmddata_len;	/* 命令附带数据的长度 */
	ME_ReqCallback reply;		/* 异步执行的回调函数 */
	unsigned long	error;		/* 返回结果 */
	unsigned long	errcode;	/* 错误码 */
	int			index;			/* 请求索引 */
	void *		result;			/* 返回数据 */
	int			result_len;		/* 返回数据的长度 */
}MEREQUEST, *PMEREQUEST; 

typedef int (*fsm_entry) (MEREQUEST * req, int event, char * data, int datalen);
typedef struct tagReqGlobal {
    UINT TimerID;
    fsm_entry entry;
    MEREQUEST * Head, * Tail;
}REQGLOBAL;

struct ME_REQUEST_WIN 
{
    MEREQUEST request;
	char	cmddata[CMD_DATALEN];
    int	    used_flag;
    HWND    hWnd;
    unsigned message;
};

struct ME_REGISTER
{
	struct ME_REGISTER * pNext;
	HWND hwnd;
	UINT msg;
	BOOL used;
	int  mode;
	ME_IndCallback notify;	/* 消息回调函数 */
};

struct ME_INDICATION 
{
    int		msgclass;		/* 消息类别 */
    void*	msgdata;		/* 消息数据 */
    int		msgdata_len;	/* 消息数据的长度 */
	int		msgresult_len;
	struct ME_REGISTER * reg_head;
	struct ME_REGISTER regs[MAX_QUEUE_REGISTER];
	int		reg_count;
};

struct ME_INDTABLE
{
	struct ME_INDICATION Inds[MAX_QUEUE_INDICATS];
	int Count;
};

#ifdef __cplusplus
extern "C" {
#endif

int ME_Request (MEREQUEST * pRequest);
int ME_CancelRequest (MEREQUEST * pRequest);
int ME_RegisterInd (int msg_class, int mode, ME_IndCallback notify);
int ME_UnRegisterInd ( int Handle );
int ME_GetCurrentResult (void* result, int result_len);
int USSD_Init (MEREQUEST * pRequest);
int USSD_Request (int mode, char * data, int datalen);
#ifdef __cplusplus
};
#endif

#endif
