#ifndef _ME_FUNCTION_H_
#define _ME_FUNCTION_H_

#define WM_MAT_MESSAGE		(WM_USER+1001)

struct ME_REQUEST;
struct ME_INDICATION;
struct ME_REGISTER;

#define DFT_TIMEOUT			8000

#define FRC_OK				0
#define FRC_ERROR			1
#define FRC_CMEERROR		2
#define FRC_CMSERROR		3
#define FRC_BUSY			4
#define FRC_NODIALTONE		5
#define FRC_NOCARRIER		6
#define FRC_CONNECT			7

enum me_fsm_event { START, DATAIN, TIMEOUT, USERSTOP };

struct ME_FSM 
{
    int	    state;	/* fsm state */
};


typedef void (*ME_ReqCallback) (struct ME_REQUEST * pRequest);
typedef void (*ME_IndCallback) (struct ME_REGISTER * pRegister);

struct ME_REQUEST 
{
	int			cmd;			/* ���� */
	int			arg1;			/* ��һ������ */
	int			arg2;			/* �ڶ������� */
	int			arg3;			/* ���������� */
	void *		cmddata;		/* ����������� */
	int			cmddata_len;	/* ��������ݵĳ��� */
	ME_ReqCallback reply;			/* �첽ִ�еĻص����� */
	int			error;			/* ���ؽ�� */
	int			errcode;		/* ������ */
	int			index;			/* �������� */
	void *		result;			/* �������� */
	int			result_len;		/* �������ݵĳ��� */
	struct ME_REQUEST * next;	/* ����ָ�� */
};


#define	MAX_QUEUE_REQUESTS  20
#define MAX_QUEUE_INDICATS	23
#define MAX_QUEUE_REGISTER	10
#define CMD_DATALEN			280
#define URC_DATALEN			20

struct ME_REGISTER
{
	HWND hwnd;
	UINT msg;
	BOOL used;
	int  mode;
	ME_IndCallback notify;	/* ��Ϣ�ص����� */
	struct ME_REGISTER * pNext;
};

struct ME_INDICATION 
{
    int		msgclass;		/* ��Ϣ��� */
    void*	msgdata;		/* ��Ϣ���� */
    int		msgdata_len;	/* ��Ϣ���ݵĳ��� */
	struct ME_REGISTER * reg_head;
	struct ME_REGISTER regs[MAX_QUEUE_REGISTER];
	int		reg_count;
};

struct ME_INDTABLE
{
	struct ME_INDICATION Inds[MAX_QUEUE_INDICATS];
	int Count;
};

extern struct ME_INDTABLE IndTable;
/* Define request command value */
#define	ME_INIT			1

#ifdef __cplusplus
extern "C" {
#endif

int ME_Request (struct ME_REQUEST * pRequest);
int ME_CancelRequest (struct ME_REQUEST * pRequest);
int ME_RegisterInd (int msg_class, int mode, ME_IndCallback notify );
int ME_UnRegisterInd ( int Handle );
int ME_GetCurrentResult( void* result, int result_len );

enum me_cmd_type
{
	ME_INIT_WAVE_EQUIP_SET = 0,
	ME_INIT_WAVE_EQUIP_RESET,
	ME_INIT_WAVE_EVERYTIME,

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
	ME_MONI_NEIB
};


struct ME_REQUEST_WIN 
{
    struct ME_REQUEST request;
	char	cmddata[CMD_DATALEN];
    int	    used_flag;
    HWND    hWnd;
    unsigned message;
};


int InsertIndQueue( int index, int Mode, ME_IndCallback notify );
int FindFreeIndPosition( struct ME_INDICATION* pNode );



#ifdef __cplusplus
};
#endif

#endif
