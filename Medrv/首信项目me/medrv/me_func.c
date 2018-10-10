#if defined MULTI_THREAD
#include "..\tcpip\include\tilib.h"
#endif
#include "string.h"
#include "stdlib.h"
#include "window.h"
#include "string.h"
#include "stdio.h"
#include "me_func.h"
#include "me_at.h"
#include "me_wnd.h"
#include "me_comn.h"
#include "smspdu.h"

#define BC_WARNING		20
#define BC_SHUTDOWN		10

static	char FRC_String[][20] = 
{
	"OK",
	"ERROR",
	"+CME ERROR:",
	"+CMS ERROR:",
	"BUSY",
	"NO DIALTONE",
	"NO ANSWER",
	"NO CARRIER",
	"CONNECT",
	NULL
};

static	char URC_String[][20] = 
{
	"+CBM:",
	"+CBMI:",
	"+CDS:",
	"+CDSI:",
	"+CMT:",
	"+CMTI:",
	"+CCCM:",
	"+CCWA:",
	"+CCWV",
	"+CDEV:",
	"+CIEV:",
	"+CKEV:",
	"+CLAV:",
	"+CLIP:",
	"+CREG:",
	"+CGREG:",
	"+CRING:",
	"+CSSU:",
	"+CUSD:",
	"%SATI:",
	"RING",
	"NO CARRIER",
	"%SATN:",
	NULL
};

#ifndef NULL
#define NULL	(void *)0
#endif

#define ATCMD_BUFSIZE		180
#define RESULT_BUFSIZE		1000
#define MAX_LINE_CHARS		1000

#define ME_FSM_INIT			0
#define ME_FSM_DONE			7
#define ME_FSM_WTRSPNS_1	1
#define ME_FSM_WTRSPNS_2	2
#define ME_FSM_WTRSPNS_3	3
#define ME_FSM_WTRSPNS_4	4
#define ME_FSM_WTRSPNS_5	5
#define ME_FSM_WTRSPNS_6	6

#define URC_FSM_INIT		0
#define URC_FSM_WT_1		1
#define URC_FSM_WT_2		2
#define URC_FSM_DONE		3

HWND ME_hWnd;
static int init_done;
static char linebuf[MAX_LINE_CHARS];
static int request_id = 0;
UINT	TimerId, TimerId2;

static void ME_MatIndication(int event, void * pdata, int datalen);
void WINAPI ME_TimerProc( HWND, UINT, UINT, DWORD );

struct ME_REQUEST * request_head;
static struct ME_REQUEST * request_tail;
static struct ME_INDICATION * current_ind; 

extern int Me_GetInitValue( PME_INIT_GETVALUE pMeValue );
LRESULT CALLBACK ME_WndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
void DispatchIndicator( int index, char* data, int datalen );

int	FSM_SetCallFwding( int event, char* data, int datalen );
int FSM_GetCallFwding( int event, char* data, int datalen );
int FSM_SetCallBarring( int event, char* data, int datalen );
int FSM_GetCallBarring( int event, char* data, int datalen );
int FSM_SMSSend( int event, char* data, int datalen );
int FSM_SMSRead( int event, char* data, int datalen );
int FSM_SMSList( int event, char* data, int datalen );
int FSM_SMSDelete( int event, char* data, int datalen );
int FSM_PBookRead( int event, char* data, int datalen );
int FSM_SetPhoneLock( int event, char* data, int datalen );
int FSM_GetPhoneLock( int event, char* data, int datalen );
int FSM_ChangePassword( int event, char* data, int datalen );
int Me_Fsm_Init ( int event, int param, char * data, int datalen );

int IsUslctResultCode( char* buf, unsigned int buflen );
int IsFinalResultCode( char* buf, unsigned int buflen );
BOOL IsTwoLineIndicatior( int index );

int GetResultErrorCode( char* data, int datalen );
void GetResultLength( struct ME_REQUEST* pRequest );
int FillResultField( void* result, int result_len, struct ME_REQUEST* pRequest );

void GetURCLength( struct ME_INDICATION* pInd, char* pdata, int datalen );
int FillURCField( void* result, int result_len, struct ME_INDICATION* pInd );

int	AnaURC_CRING( char *SrcStr, int* Type );
int	AnaURC_CLIP( char *SrcStr,  PME_PHONE_ITEM pItem );
int	AnaURC_CCWA( char * SrcStr, PME_PHONE_ITEM pItem );
int	AnaURC_CSSU( char *SrcStr, PME_CSSU_ITEM pItem );
int	AnaURC_SMGO( char *SrcStr, int* Type);
int	AnaURC_CMTI( char *SrcStr, PME_CMTI_ITEM pItem );
int	AnaURC_STK( char *SrcStr, char* result );
int	AnaURC_CREG( char *SrcStr, PME_CREG_URC pItem );
int	AnaURC_CGREG( char *SrcStr, PME_CGREG_URC pItem );
int	AnaURC_CMT( char* SrcStr, struct tagSMS_INFO* info );
int	AnaURC_CBM( char* SrcStr, struct tagSMS_INFO* info );
int	AnaURC_CDS( char* SrcStr, struct tagSMS_INFO* info );

static int MakeSTKMenuResponse (char * dst, char * src);
static int String2Bytes (char* dst, const char* src, int srclen);
static int Bytes2String (char* dst, const char* src, int srclen);

#define MAX_MENULEN     1024
#define MAX_RESPLEN     256
char STKMenu[MAX_MENULEN];
int STK_Stat = 0;
struct ME_REQUEST* STKRequest;

int GetSTKMainMenu( char* buf, int buflen )
{
	if ( STK_Stat != 1 )
		return -1;

	if ( buflen < (int)strlen(STKMenu) )
		return -1;

	memcpy( buf, STKMenu, strlen(STKMenu) );
	STK_Stat = 2;

	return  strlen(STKMenu);
}

/*
 * Test if a command is async command.
 */
static int IsAsyncCmd (int cmd);

/*
 * For none async command.
 */
static int me_do_cmd (struct ME_REQUEST * request);

/*
 * ME finit state machine.
 */
static int me_finite_statemachine (int event, char * data, int datalen);


struct ME_FSM	me_fsm_data, urc_fsm_data;

int InsertIndQueue( int index, int Mode, ME_IndCallback notify )
{
	struct ME_REGISTER *pFree, *pMove;
	struct ME_INDICATION *pNode = &(IndTable.Inds[index]);
	int	retval,index2;

	if ( pNode->reg_head == NULL )
	{
		pNode->regs[0].notify = notify;
		pNode->regs[0].mode = Mode;
		pNode->regs[0].used = TRUE;
		pNode->regs[0].pNext = NULL;
		pNode->reg_head = &(pNode->regs[0]);

		retval = 0;
	}
	else
	{
		index2 = FindFreeIndPosition( pNode );

		if ( index2 < 0 )
			return -1;

		pFree = &(pNode->regs[index2]);

		pFree->notify = notify;
		pFree->used = TRUE;
		pFree->mode = Mode;
		pFree->pNext = NULL;

		if ( IND_PRIORITY == Mode )
		{
			pFree->pNext = pNode->reg_head;
			pNode->reg_head = pFree;
		}
		else
		{
			pMove = pNode->reg_head;

			while( pMove->pNext != NULL)
				pMove = pMove->pNext;

			pMove->pNext = pFree;
		}

		retval = index2;
	}

	pNode->reg_count++;

	return retval;
}


int FindFreeIndPosition( struct ME_INDICATION* pNode )
{
	int i;
	if ( pNode->reg_count >= MAX_QUEUE_REGISTER )
		return -1;

	for ( i = 0; i < MAX_QUEUE_REGISTER; i++ )
	{
		if ( !(pNode->regs[i].used) )
			break;

		if ( pNode->regs[i].mode == IND_EXCLUSIVE )
			return -1;
	}

	return i;
}

#define INIT_RETRY		3
#define INIT_EXPIRE		3000
#define INIT_TIMERID	101
static int me_init(struct ME_REQUEST * pRequest)
{
    WNDCLASS wc;
	int i = 0;
	MSG msg;

	request_head = request_tail = NULL;

 	wc.style         = 0;
	wc.lpfnWndProc   = ME_WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "MEWndClass";

	if (!RegisterClass(&wc))
		return -1;

	ME_hWnd = CreateWindow( "MEWndClass" ,"", 
							0, 0, 0, 0, 0,
							NULL, NULL, NULL, NULL );

	if ( ME_hWnd == NULL)
		return -1;

	if ( MAT_Init ( pRequest->arg1, 0, 0, ME_MatIndication ) < 0 )
		return -1;

	while ( i < INIT_RETRY )
	{
		SetTimer( ME_hWnd, INIT_TIMERID, INIT_EXPIRE, NULL );
		MAT_Send( 0, "AT\r", -1 );

		while ( GetMessage(&msg, NULL, 0, 0) )
		{
			if ( msg.message == WM_TIMER && msg.hwnd == ME_hWnd
				&& msg.wParam == INIT_TIMERID )
			{
				KillTimer(ME_hWnd, INIT_TIMERID );
				i++;
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if ( init_done )
				return 0;
		}
	}

//    init_done = 1;

    return 0;
}

int ME_Request (struct ME_REQUEST * pRequest)
{
    int retval = 0;

    if ( ! init_done )
	{
		/* Only init can be called */
		if ( pRequest->cmd != ME_INIT )
			return -1;
		me_init( pRequest );

		return init_done ? 1 : -1;
    }
    WAIT_FOR_MUTEX();

    pRequest->next = NULL;

    if ( ! IsAsyncCmd (pRequest->cmd) )
	{
		/* This is not async command */
		retval = me_do_cmd (pRequest);
    }
	else if ( request_head )
	{
		/* We are in command exec, add command into list tail */
		request_tail->next = pRequest;
		request_tail = pRequest;
		pRequest->index = request_id;
	}
	else
	{
		/* We are not in command exec */
		request_head = request_tail = pRequest;
		/* Start do this command */
		me_fsm_data.state = ME_FSM_INIT;
		PostMessage (ME_hWnd, WM_FSM_START, 0, 0);
		pRequest->index = request_id;
    }

    RELEASE_MUTEX();

    return request_id++;
}

int ME_CancelRequest ( struct ME_REQUEST * pRequest )
{
    WAIT_FOR_MUTEX();

	if ( pRequest == request_head )
	{
		me_finite_statemachine( USERSTOP, 0, 0 );
		if ( request_head != NULL && me_fsm_data.state == ME_FSM_DONE )
		{
			if ( request_head->reply)
				request_head->reply(request_head);

			request_head = request_head->next;
			if ( request_head )
			{
				me_fsm_data.state = ME_FSM_INIT;
				PostMessage (ME_hWnd, WM_FSM_START, 0, 0);
			}
		}
	}
	else
	{
		struct ME_REQUEST *cur_req, *pri_req;
		pri_req = request_head;
		cur_req = request_head->next;
		while ( cur_req != NULL )
		{
			if ( cur_req == pRequest )
			{
				if ( cur_req->reply )
				{
					cur_req->error = ME_RS_USERSTOP;
					cur_req->reply (cur_req);
				}
				pri_req->next = cur_req->next;
				break;
			}
			pri_req = pri_req->next;
			cur_req = cur_req->next;
		}
		if ( cur_req == NULL)
			goto ERROR_TAG;
	}

	RELEASE_MUTEX();
    return 0;

ERROR_TAG:
	RELEASE_MUTEX();
	return -1;
}

int ME_GetCurrentResult( void* result, int result_len )
{
	// 当前结果为请求返回结果
	if ( request_head != NULL && me_fsm_data.state == ME_FSM_DONE )
	{
		if ( NULL == result )
			return request_head->result_len;

		if ( result_len < 0 || request_head->result_len < 0 )
			return -1;

		if ( result_len < request_head->result_len )
			return -1;

		return FillResultField( result, result_len, request_head );
	}

	// 当前结果为URC数据
	if ( current_ind != NULL )
	{
		if ( NULL == result )
			return current_ind->msgresult_len;

		if ( result_len < 0 || current_ind->msgresult_len < 0 )
			return -1;

		if ( result_len < current_ind->msgresult_len )
			return-1;

		return FillURCField( result, result_len, current_ind );
	}

	return -1;
}

int ME_RegisterInd (int msg_class, int mode, ME_IndCallback notify )
{
	int i, index, count, first;

    WAIT_FOR_MUTEX();

	count = IndTable.Count;
	first = MAX_QUEUE_INDICATS;

	if ( count == MAX_QUEUE_INDICATS )
		goto ERROR_TAG;

	for( i=0; i < MAX_QUEUE_INDICATS; i++ )
	{
		if ( IndTable.Inds[i].reg_head != NULL )
		{
			if ( msg_class == IndTable.Inds[i].msgclass )
				break;
		}
		else
		{
			first = ( i < first )?i:first;
		}
	}

	if ( i < MAX_QUEUE_INDICATS )
	{
		if ( IND_EXCLUSIVE == mode )
			goto ERROR_TAG;
		else
		{
			index = InsertIndQueue( i, mode, notify );
			if ( index < 0 )
				goto ERROR_TAG;
		}

	    RELEASE_MUTEX();
		return i*100+index;
	}
	else
	{
		if ( first == MAX_QUEUE_INDICATS )
			goto ERROR_TAG;

		index = InsertIndQueue( first, mode, notify );
		if ( index < 0 )
			goto ERROR_TAG;

		IndTable.Inds[first].msgclass = msg_class;
		IndTable.Count++;

		RELEASE_MUTEX();

		return first*100+index;
	}

ERROR_TAG:
    RELEASE_MUTEX();
	return -1;
}

int ME_UnRegisterInd( int Handle )
{
	int id1, id2;
	struct ME_REGISTER *pInd, *pPrior;
	int count;

    WAIT_FOR_MUTEX();

	count = IndTable.Count;

	id1 = (int)Handle%100;
	id2 = (int)Handle/100;

	if ( id2 < 0 || id2 >= MAX_QUEUE_INDICATS )
		goto ERROR_TAG;

	if ( id1 < 0 || id1 >= MAX_QUEUE_REGISTER )
		goto ERROR_TAG;

	if ( IndTable.Inds[id2].reg_count == 0 )
		goto ERROR_TAG;

	pInd = pPrior = IndTable.Inds[id2].reg_head;

	while ( pInd->pNext != NULL )
	{
		if ( pInd == &(IndTable.Inds[id2].regs[id1]) )
		{
			pInd->used = FALSE;

			if ( pInd == IndTable.Inds[id2].reg_head )
				IndTable.Inds[id2].reg_head = pInd->pNext;
			else
				pPrior->pNext = pInd->pNext;

			break;
		}
		pPrior = pInd;
		pInd = pInd->pNext;
	}

	if ( pInd->pNext == NULL )
	{
		if ( pInd == &(IndTable.Inds[id2].regs[id1]) )
		{
			pInd->used = FALSE;

			if ( pInd == IndTable.Inds[id2].reg_head )
				IndTable.Inds[id2].reg_head = NULL;
			else
				pPrior->pNext = NULL;
		}
	}

	IndTable.Inds[id2].reg_count --;

	if ( IndTable.Inds[id2].reg_count == 0 )
		IndTable.Count--;

    RELEASE_MUTEX();
	return 0;

ERROR_TAG:
    RELEASE_MUTEX();
	return -1;
}

void ME_MatIndication(int event, void * pdata, int datalen)
{
	int i;
	static BOOL bMultiLine = FALSE;
	static int offset = 0, old_offset, ret = -1;

	if ( event != MAT_DATAIN ) 
		return;

    WAIT_FOR_MUTEX();

	for ( i = 0; i < datalen; i++ )
	{
		if ( ((char*)pdata)[i]  == '\0' )
			continue;

		if ( offset == MAX_LINE_CHARS )
		{
			MAT_Reset( 0 );
			break;
		}
		linebuf[offset++] = ((char*)pdata)[i];

		if ( ((char*)pdata)[i] == '\n' )
		{
			if ( offset <= 2 )
			{
				offset = 0;
				continue;
			}

			if ( bMultiLine )
			{
				if ( offset - old_offset > 2 )
				{
					bMultiLine = FALSE;
					DispatchIndicator( ret, linebuf, offset );
					offset = 0;
				}
				else
				{
					old_offset = offset;
				}
				continue;
			}

			if (request_head != NULL || !init_done)
			{
				ret = me_finite_statemachine ( DATAIN, linebuf, offset );

				if ( ret >= 0 )
				{
					offset = 0;
					goto check_cmdqueue;
				}
			}

			/* Dispatch the indication message */
			ret = IsUslctResultCode( linebuf, offset );
			if ( ret == MAX_QUEUE_INDICATS+1 )
			{
				DispatchIndicator( ret, linebuf, offset );
				offset = 0;
				continue;
			}

			if ( ret < 0 )
			{
				offset = 0;
				continue;
			}

			if ( IsTwoLineIndicatior(ret) )
			{
				bMultiLine = TRUE;
				old_offset = offset;
				continue;
			}
			else
			{
				DispatchIndicator( ret, linebuf, offset );
				offset = 0;
			}
		}

		if ( ((char*)pdata)[i] == '>' && offset == 1 )
		{
		    if ( request_head )
			{
				ret = me_finite_statemachine ( DATAIN, linebuf, offset ); 

				if ( ret >= 0 )
				{
					offset = 0;
					goto check_cmdqueue;
				}
			}
		}

check_cmdqueue:
		if ( request_head != NULL && me_fsm_data.state == ME_FSM_DONE )
		{
            if ( request_head->cmd == ME_STK_RESPONSE ) {
                request_head = STKRequest->next;
                free (STKRequest->cmddata);
                free (STKRequest);
            }
            else {
			    /* Call reply function */
			    if ( request_head->reply )
			    {
				    request_head->reply (request_head);
				    ME_MemInitial();
			    }
    			/* Unlink it from list */
	    		request_head = request_head->next;
            }

			/* Check for start next command */
			if ( request_head )
			{
				/* Start do this command */
				me_fsm_data.state = ME_FSM_INIT;
				PostMessage (ME_hWnd, WM_FSM_START, 0, 0);
			}
		}
	}

    RELEASE_MUTEX();
}

void DispatchIndicator( int index, char* data, int datalen )
{
	struct ME_REGISTER* preg;

	if ( index == MAX_QUEUE_INDICATS+1 )
	{
		char buf[MAX_RESPLEN];
        memcpy( STKMenu, data+7, datalen-9 );
		STKMenu[datalen] = '\0';
        STKRequest = (struct ME_REQUEST *) malloc (sizeof(struct ME_REQUEST));
        STKRequest->cmd = ME_STK_RESPONSE;
        STKRequest->cmddata = malloc (MAX_RESPLEN);
        MakeSTKMenuResponse ((char*)STKRequest->cmddata, STKMenu);
		STK_Stat = 1;
	    if ( request_head )
	    {
		    /* We are in command exec, add command into list tail */
            STKRequest->next = request_head->next;
            request_head->next = STKRequest;
	    }
	    else
	    {
		    /* We are not in command exec */
		    request_head = request_tail = STKRequest;
		    /* Start do this command */
		    me_fsm_data.state = ME_FSM_INIT;
		    PostMessage (ME_hWnd, WM_FSM_START, 0, 0);
        }

		return;
	}

	current_ind = &IndTable.Inds[index];

	GetURCLength( current_ind, data, datalen );

	preg = current_ind->reg_head;

	while ( preg )
	{
		preg->notify( preg );
		preg = preg->pNext;
	}
	current_ind = NULL;
}

LRESULT CALLBACK ME_WndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	if ( wMsgCmd == WM_MAT_MESSAGE )
		MAT_Callback();
	else if ( wMsgCmd == WM_FSM_START )
	{
		while ( request_head != NULL )
		{
			me_finite_statemachine ( START, 0, 0 );
			if ( me_fsm_data.state != ME_FSM_DONE )
				break;
			{
				struct ME_REQUEST * request = request_head;
				if ( request->reply )
				{
					request->reply (request);
					ME_MemInitial();
				}
				request_head = request->next;
			}
		}
	}
	else
		return DefWindowProc( hWnd, wMsgCmd, wParam, lParam);

	return 0;
}

void WINAPI ME_TimerProc( HWND hWnd, UINT Msg, UINT id, DWORD Time )
{

	KillTimer( NULL, id );
#if defined _EMULATE
	MsgOut( "\r\nME Request Timeout!" );
#endif

	me_finite_statemachine( TIMEOUT, 0, 0 );

	if ( request_head != NULL && me_fsm_data.state == ME_FSM_DONE )
	{
		/* Call reply function */
		if ( request_head->reply )
		{
			request_head->reply (request_head);
			ME_MemInitial();
		}
		/* Unlink it from list */
		request_head = request_head->next;

		/* Check for start next command */
		if ( request_head )
		{
			/* Start do this command */
			me_fsm_data.state = ME_FSM_INIT;
			PostMessage (ME_hWnd, WM_FSM_START, 0, 0);
		}
	}
}

/******************************************************************************
 *
 * Module dependent.
 *
 ******************************************************************************
 */

int IsAsyncCmd ( int cmd )
{
    return 1;
}

int me_do_cmd ( struct ME_REQUEST * request )
{
    int retval = 0;
    switch ( request->cmd )
	{
    }
    return retval;
}

int me_urc_fsm( int event, char* data, int datalen )
{
	int ret;
	static int offset;

	switch ( current_ind->msgclass )
	{
	case ME_URC_CMT:
	case ME_URC_CDS:

		if ( urc_fsm_data.state == URC_FSM_INIT )
			urc_fsm_data.state = URC_FSM_WT_1;
		else if ( urc_fsm_data.state == URC_FSM_WT_1)
		{
			offset = datalen;

			ret = MAT_Send( 0, "AT+CNMA\r", -1 );

			if ( ret != -1 )
			{
				TimerId2 = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
				urc_fsm_data.state = URC_FSM_WT_2;
			}
			else
			{
				offset = 0;
				urc_fsm_data.state = URC_FSM_DONE;
			}
		}
		else if ( urc_fsm_data.state == URC_FSM_WT_2 )
		{
			if ( event == DATAIN )
			{
				ret = IsFinalResultCode( &data[offset], datalen-offset );

				if ( ret < 0 )
					return ret;

				KillTimer( NULL, TimerId2 );
				offset = 0;
				urc_fsm_data.state = URC_FSM_DONE;
			}
			else if ( event == TIMEOUT )
			{
				offset = 0;
				urc_fsm_data.state = URC_FSM_DONE;
			}
		}
		break;
	
	default:
		urc_fsm_data.state = URC_FSM_DONE;
		break;
	}

	return 0;
}

int me_finite_statemachine ( int event, char * data, int datalen )
{
	char ATCmd[ATCMD_BUFSIZE];
    int retval = 0;

	if ( !init_done )
	{
		if ( event != DATAIN )
			return 0;

		if ( memcmp( data, "OK", 2 ) == 0 )
			init_done = 1;
		return 0;
	}

	if ( event == DATAIN && me_fsm_data.state == ME_FSM_INIT )
		return -1;

	request_head->result_len = -1;

    switch ( request_head->cmd )
	{
	case ME_INIT_WAVE_EQUIP_SET:
	case ME_INIT_WAVE_EQUIP_RESET:
	case ME_INIT_WAVE_SIM_SET:
	case ME_INIT_WAVE_EVERYTIME:

		if ( event == DATAIN )
		{
			retval = IsUslctResultCode( data, datalen );
			if ( retval >= 0 )
				return -1;

			retval = IsFinalResultCode( data, datalen );
		}
		retval = Me_Fsm_Init ( event, retval, data, datalen );
		break;

    case ME_STK_RESPONSE:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				retval = MAT_Send( 0, request_head->cmddata, -1 );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );
				
				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_STK_SEND:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				char* cmd = ((STK_PARAM*)(request_head->cmddata))->cmd;
				int  timeout = ((STK_PARAM*)(request_head->cmddata))->timeout;

				if ( cmd == NULL )
					goto ERROR_HANDLER;

				retval = MAT_Send( 0, cmd, strlen(cmd) );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, timeout, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{

			if ( DATAIN == event )
			{

		  		retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;


				KillTimer(NULL, TimerId);
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if  ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
			
		}

		break;

	case ME_AT_SEND:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				char* cmd = request_head->cmddata;
				int  timeout = request_head->arg1;

				retval = MAT_Send( 0, cmd, strlen(cmd) );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, timeout, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = IsUslctResultCode( data, datalen );

				if ( retval >= 0 )
					return retval;

				retval = IsFinalResultCode( data, datalen );

				if ( retval < 0 )
				{
					ME_MemAssign( data, datalen );
					return 0;
				}

				KillTimer(NULL, TimerId);
				ME_MemAssign( data, datalen );
				GetResultLength( request_head );
				request_head->error = ME_RS_SUCCESS;
				request_head->errcode = 0;
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if  ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_VOICE_DIAL:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				retval = MAT_Send( 0, "AT+COLP=1\r", -1 );

				if ( retval > 0 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);

				if ( retval == FRC_OK )
				{
					char* cmddata = request_head->cmddata;

					if ( request_head->cmddata_len == (int)strlen(cmddata) )
						sprintf( ATCmd, "ATD%s;\r", cmddata );
					else
						sprintf( ATCmd, "ATD%s+%s;\r", cmddata, &cmddata[strlen(cmddata)+1] );

					retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );

					if ( retval != -1 )
					{
						TimerId = SetTimer( NULL, 0, 90000, ME_TimerProc );
						me_fsm_data.state = ME_FSM_WTRSPNS_2;
					}
					else
						goto ERROR_HANDLER;
				}
				else
					goto ERROR_HANDLER;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}
		else if ( ME_FSM_WTRSPNS_2 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( USERSTOP == event )
			{
				retval = MAT_Send( 0, "ATH1\r", -1 );

				KillTimer( NULL, TimerId );
				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_3;
				}
				else
					goto ERROR_HANDLER;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}
		else if ( ME_FSM_WTRSPNS_3 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );
				
				if ( retval < 0 )
					return retval;

				KillTimer( NULL, TimerId );
				if ( retval == FRC_OK )
					request_head->error = ME_RS_USERSTOP;
				else
					request_head->error = ME_RS_FAILURE;

				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_VOICE_HANGUP:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				retval = MAT_Send( 0, "ATH\r", -1 );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );
				
				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_VOICE_ANSWER:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				retval = MAT_Send( 0, "ATA\r", strlen("ATA\r") );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );
				
				if ( retval < 0 )
					return retval;
		
				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_CURRENT_CALLS:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				retval = MAT_Send( 0, "AT+CLCC\r", -1 );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				data[datalen] = '\0';

				if ( strstr( data, "+CLCC:") == data )
				{
					ME_MemAssign( data, datalen );
					return 0;
				}
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_SWITCH_CALLLINE:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				switch( request_head->arg1 )
				{
				case CUT_HELD:				// 呼叫等待时忙音或者挂断保持的通话

					strcpy( ATCmd, "AT+CHLD=0\r" );

					break;

				case CUT_ACTIVED:			// 挂断激活的通话

					strcpy( ATCmd, "AT+CHLD=1\r" );

					break;

				case ACTIVE_SWITCH:			// 在保持和激活的通话状态间切换

					strcpy( ATCmd, "AT+CHLD=2\r" );

					break;

				case ADD_TO_ACTIVE:			// 将保持的通话加入激活通话

					strcpy( ATCmd, "AT+CHLD=3\r" );

					break;

				case CUT_ALL:				// 挂断所有保持和激活的通话

					strcpy( ATCmd, "AT+CHUP\r" );

					break;

				case CUT_ACTIVED_X:			// 挂断指定序号的激活通话

					if ( request_head->arg2 < 8 && request_head->arg2 > 0 )
						sprintf( ATCmd, "AT+CHLD=1%d\r", request_head->arg2 );
					else
						goto ERROR_HANDLER;

					break;

				case ACTIVE_SWITCH_X:		// 将除指定序号外的所有通话变为保持

					if ( request_head->arg2 < 8 && request_head->arg2 > 0 )
						sprintf( ATCmd, "AT+CHLD=2%d\r", request_head->arg2 );
					else
						goto ERROR_HANDLER;

					break;

				default:

					goto ERROR_HANDLER;
				}

				retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_SET_AUTOANSWER:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				if ( request_head->arg1 < 0 )
					goto ERROR_HANDLER;

				sprintf( ATCmd, "ATS0=%d\r", request_head->arg1 );

				retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_GET_AUTOANSWER:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				retval = MAT_Send( 0, "ATS0?\r", strlen("ATS0?\r") );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
				{
					ME_MemAssign( data, datalen );
					return 0;
				}

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;
		
	case ME_SET_CALLWAITING:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				switch ( request_head->arg2 )
				{
				case CLASS_VOICE:

					if ( (BOOL)(request_head->arg1) )
						strcpy( ATCmd, "AT+CCWA=1,1,1\r" );
					else
						strcpy( ATCmd, "AT+CCWA=0,0,1\r" );

					break;
				case CLASS_DATA:

					if ( (BOOL)(request_head->arg1) )
						strcpy( ATCmd, "AT+CCWA=1,1,2\r" );
					else
						strcpy( ATCmd, "AT+CCWA=0,0,2\r" );

					break;

				case CLASS_ALL:

					if ( (BOOL)(request_head->arg1) )
						strcpy( ATCmd, "AT+CCWA=1,1,7\r" );
					else
						strcpy( ATCmd, "AT+CCWA=0,0,7\r" );

					break;

				default:
					goto ERROR_HANDLER;
				}

				retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_GET_CALLWAITING:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				switch ( request_head->arg1 )
				{
				case CLASS_VOICE:

					strcpy( ATCmd, "AT+CCWA=,2,1\r" );

					break;
				case CLASS_DATA:

					strcpy( ATCmd, "AT+CCWA=,2,2\r" );

					break;

				default:
					goto ERROR_HANDLER;
				}

				retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				data[datalen] = '\0';

				if ( strstr( data, "+CCWA:") == data && ME_MemGetOffset() == 0 )
				{
					ME_MemAssign( data, datalen );
					return 0;
				}
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_SET_CALLFWDING:

		return FSM_SetCallFwding( event, data, datalen );

	case ME_GET_CALLFWDING:
		
		return FSM_GetCallFwding( event, data, datalen );

	case ME_SET_CALLBARRING:
		return FSM_SetCallBarring( event, data, datalen );

	case ME_GET_CALLBARRING:
		return FSM_GetCallBarring( event, data, datalen );

	case ME_SMS_SEND:
		return FSM_SMSSend( event, data, datalen );

	case ME_SMS_READ:
		return FSM_SMSRead( event, data, datalen );

	case ME_SMS_DELETE:
		return FSM_SMSDelete( event, data, datalen );

	case ME_SMS_LIST:
		return FSM_SMSList( event, data, datalen );

	case ME_SMS_ACK:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				retval = MAT_Send( 0, "AT+CMGF=1\r", -1 );
				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer( NULL, TimerId );

				if ( retval == FRC_OK)
				{
					retval = MAT_Send( 0, "AT+CNMA\r", -1 );
					if ( retval != -1 )
					{
						TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
						me_fsm_data.state = ME_FSM_WTRSPNS_2;
					}
					else
					{
						retval = MAT_Send( 0, "AT+CMGF=0\r", -1 );
						if ( retval != -1 )
						{
							TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
							me_fsm_data.state = ME_FSM_WTRSPNS_3;
						}
						else
							goto ERROR_HANDLER;
					}
				}
				else
				{
					GetResultLength( request_head );
					me_fsm_data.state = ME_FSM_DONE;
				}
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}
		else if ( ME_FSM_WTRSPNS_2 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				retval = MAT_Send( 0, "AT+CMGF=0\r", -1 );
				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_3;
				}
				else
					goto ERROR_HANDLER;
			}
			else if ( TIMEOUT == event )
			{
				retval = MAT_Send( 0, "AT+CMGF=0\r", -1 );
				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_3;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_3 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				static int err,errcode;
				err = request_head->error;
				errcode = request_head->errcode;

				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;
				
				request_head->errcode = errcode;
				request_head->error = err;

				KillTimer( NULL, TimerId );
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_SMS_FORMAT:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				if ( request_head->arg1 )
					strcpy( ATCmd, "AT+CMGF=0\r" );
				else
					strcpy( ATCmd, "AT+CMGF=1\r" );

				retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);

				if ( retval == FRC_OK )
				{
					retval = MAT_Send( 0, "AT+CSAS\r", -1 );

					if ( retval != -1 )
					{
						TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
						me_fsm_data.state = ME_FSM_WTRSPNS_2;
					}
					else
						goto ERROR_HANDLER;
				}
				else
				{
					GetResultLength( request_head );
					me_fsm_data.state = ME_FSM_DONE;
				}
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}
		else if ( ME_FSM_WTRSPNS_2 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);

				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}
		break;

	case ME_SMS_PHASE:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				if ( request_head->arg1 )
					strcpy(ATCmd, "AT+CSMS=1\r" );
				else
					strcpy(ATCmd, "AT+CSMS=0\r" );

				retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				static int tag = 0;
				if ( memcmp(data, "+CSMS:", 6) == 0)
					tag =1;

				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);

				if ( retval == FRC_OK && tag)
				{
					retval = MAT_Send( 0, "AT+CSAS\r", -1 );

					if ( retval != -1 )
					{
						TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
						me_fsm_data.state = ME_FSM_WTRSPNS_2;
					}
					else
						goto ERROR_HANDLER;
				}
				else 
				{
					if (!tag)
					{
						request_head->error = FRC_ERROR;
						request_head->errcode = 1;
					}
					GetResultLength( request_head );
					me_fsm_data.state = ME_FSM_DONE;
				}
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}
		else if ( ME_FSM_WTRSPNS_2 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);

				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}
		break;

	case ME_SMS_SET_SCA:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				sprintf( ATCmd, "AT+CSCA=\"%s\"\r", request_head->cmddata );

				retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_SMS_GET_SCA:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
					retval = MAT_Send( 0, "AT+CSCA?\r", -1 );

					if ( retval != -1 )
					{
						TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
						me_fsm_data.state = ME_FSM_WTRSPNS_1;
					}
					else
						goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				data[datalen] = '\0';

				if ( strstr( data, "+CSCA:") == data && ME_MemGetOffset() == 0 )
				{
					ME_MemAssign( data, datalen );
					return 0;
				}
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}
		break;

	case ME_SMS_SETINDSTATUS:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				int mt, bm, ds;

				mt = ((PIND_STATUS)(request_head->cmddata))->Mt;
				bm = ((PIND_STATUS)(request_head->cmddata))->Bm;
				ds = ((PIND_STATUS)(request_head->cmddata))->Ds;

				strcpy( ATCmd, "AT+CNMI=2," );

				if ( mt == IND_NONE )
					strcat( ATCmd, "0," );
				else if ( mt == IND_BUFFER )
					strcat( ATCmd, "1," );
				else if ( mt == IND_DIRECT )
					strcat( ATCmd, "2," );
				else
					goto ERROR_HANDLER;

				if ( bm == IND_NONE )
					strcat( ATCmd, "0," );
				else if ( bm == IND_DIRECT )
					strcat( ATCmd, "2,");
				else
					goto ERROR_HANDLER;

				if ( ds == IND_NONE )
					strcat( ATCmd, "0," );
				else if ( ds == IND_DIRECT )
					strcat( ATCmd, "1," );
				else
					goto ERROR_HANDLER;

				strcat( ATCmd, "1\r" );

				retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);

				if ( retval == FRC_OK )
				{
					retval = MAT_Send( 0, "AT+CSAS\r", -1 );

					if ( retval != -1 )
					{
						TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
						me_fsm_data.state = ME_FSM_WTRSPNS_2;
					}
					else
						goto ERROR_HANDLER;
				}
				else
				{
					GetResultLength( request_head );
					me_fsm_data.state = ME_FSM_DONE;
				}
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}
		else if ( ME_FSM_WTRSPNS_2 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);

				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}
		break;

	case ME_SMS_GETINDSTATUS:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				retval = MAT_Send( 0, "AT+CNMI?\r", -1 );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				data[datalen] = '\0';

				if ( strstr( data, "+CNMI:") == data && ME_MemGetOffset() == 0 )
				{
					ME_MemAssign( data, datalen );
					return 0;
				}
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}
		break;

	case ME_SET_CELLBROADCAST:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				BOOL mode;
				char *ids, *code;

				mode = ((PCELL_BROADCAST)(request_head->cmddata))->mode;
				ids = ((PCELL_BROADCAST)(request_head->cmddata))->ids;
				code = ((PCELL_BROADCAST)(request_head->cmddata))->code;

				if ( mode )
					sprintf( ATCmd, "AT+CSCB=0,\"%s\",\"%s\"\r", ids, code );
				else
					sprintf( ATCmd, "AT+CSCB=1,\"%s\",\"%s\"\r", ids, code);

				retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_GET_CELLBROADCAST:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				retval = MAT_Send( 0, "AT+CSCB?\r", -1 );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				data[datalen] = '\0';

				if ( strstr( data, "+CSCB:") == data && ME_MemGetOffset() == 0 )
				{
					ME_MemAssign( data, datalen );
					return 0;
				}
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}
		break;

	case ME_SMS_MEMCOUNT:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				switch( request_head->arg1 )
				{
				case SMS_MEM_SM:

					strcpy( ATCmd, "AT+CPMS=\"SM\"\r" );

					break;

				case SMS_MEM_ME:

					strcpy( ATCmd, "AT+CPMS=\"ME\"\r" );

					break;

				default:
					goto ERROR_HANDLER;
				}

				retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				data[datalen] = '\0';

				if ( strstr( data, "+CPMS:") == data && ME_MemGetOffset() == 0 )
				{
					ME_MemAssign( data, datalen );
					return 0;
				}
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}
		break;

	case ME_PBOOK_READ:
		return FSM_PBookRead( event, data, datalen );

	case ME_PBOOK_WRITE:

		{
			static int index, ret;
			static char *PhoneNum, *Text;
			static char phonetext[100];
			if ( ME_FSM_INIT == me_fsm_data.state )
			{
				if ( START == event )
				{
					index = ((PPHONEBOOK)(request_head->cmddata))->Index;
					PhoneNum = ((PPHONEBOOK)(request_head->cmddata))->PhoneNum;
					Text = ((PPHONEBOOK)(request_head->cmddata))->Text;

					if ( strlen(PhoneNum)==0 && strlen(Text)==0 )
					{
						sprintf( ATCmd, "AT+CPBW=%d\r", index );
						retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );

						if ( retval != -1 )
						{
							TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
							me_fsm_data.state = ME_FSM_WTRSPNS_2;
						}
						else
							goto ERROR_HANDLER;
					}
					else
					{
						ret = PhoneText_Encode( phonetext, Text, 100 );

						if ( strlen(phonetext) == 0 )
							goto ERROR_HANDLER;

						strcpy( ATCmd, "AT+CSCS=\"HEX\"\r" );
						retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );

						if ( retval != -1 )
						{
							TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
							me_fsm_data.state = ME_FSM_WTRSPNS_1;
						}
						else
							goto ERROR_HANDLER;
					}
				}
			}
			else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
			{
				if ( DATAIN == event )
				{
					retval = GetResultErrorCode( data, datalen );

					if ( retval < 0 )
						return retval;

					KillTimer(NULL, TimerId);
					if ( retval == FRC_OK )
					{
						sprintf( ATCmd, "AT+CPBW=%d,\"%s\",,\"%s\"\r", index, PhoneNum, phonetext );

						retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );

						if ( retval != -1 )
						{
							TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
							me_fsm_data.state = ME_FSM_WTRSPNS_2;
						}
						else
							goto ERROR_HANDLER;
					}
					else
						goto ERROR_HANDLER;
				}
				else if ( TIMEOUT == event )
					goto TIMEOUT_HANDLER;
			}
			else if ( ME_FSM_WTRSPNS_2 == me_fsm_data.state )
			{
				if ( DATAIN == event )
				{
					retval = GetResultErrorCode( data, datalen );

					if ( retval < 0 )
						return retval;

					KillTimer(NULL, TimerId);
					GetResultLength( request_head );
					me_fsm_data.state = ME_FSM_DONE;
				}
				else if ( TIMEOUT == event )
					goto TIMEOUT_HANDLER;
			}
		}
		break;
	
	case ME_PBOOK_MEMCOUNT:
		
		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				switch ( request_head->arg1 )
				{
				case PBOOK_SM:
					retval = MAT_Send( 0, "AT+CPBS=\"AD\"\r", -1 );
					break;
				case PBOOK_LD:
					retval = MAT_Send( 0, "AT+CPBS=\"LD\"\r", -1 );
					break;
				case PBOOK_MC:
					retval = MAT_Send( 0, "AT+CPBS=\"LM\"\r", -1 );
					break;
				case PBOOK_RC:
					retval = MAT_Send( 0, "AT+CPBS=\"LR\"\r", -1 );
					break;
				default:
					goto ERROR_HANDLER;
				}

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer( NULL, TimerId );

				if ( retval == FRC_OK )
				{
					retval = MAT_Send( 0, "AT+CPBS?\r", -1 );
					if ( retval != -1 )
					{
						TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
						me_fsm_data.state = ME_FSM_WTRSPNS_2;
					}
					else
						goto ERROR_HANDLER;
				}
				else
				{
					GetResultLength( request_head );
					me_fsm_data.state = ME_FSM_DONE;
				}
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}
		else if ( ME_FSM_WTRSPNS_2 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				data[datalen] = '\0';

				if ( strstr( data, "+CPBS:") == data && ME_MemGetOffset() == 0 )
				{
					ME_MemAssign( data, datalen );
					return 0;
				}
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer( NULL, TimerId );
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_SET_LOCK:

		return FSM_SetPhoneLock( event, data, datalen );

	case ME_GET_LOCK:
		
		return FSM_GetPhoneLock( event, data, datalen );

	case ME_PASS_CHANGE:

		return FSM_ChangePassword( event, data, datalen );

	case ME_PASS_VALID:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				char* password, *newpass;

				password = request_head->cmddata;
				
				if ( request_head->cmddata_len == (int)strlen(password) )
					newpass = NULL;
				else
					newpass = &password[strlen(password)+1];

				if ( request_head->arg1 == PUK1 || request_head->arg1 == PUK2 )
				{
					if ( newpass == NULL )
						goto ERROR_HANDLER;
				}

				strcpy( ATCmd, "AT+CPIN=");

				strcat( ATCmd, "\"" );
				strcat( ATCmd, password );
				strcat( ATCmd, "\"" );

				if ( newpass != NULL )
				{
					strcat( ATCmd, "," );
					strcat( ATCmd, "\"" );
					strcat( ATCmd, newpass );
					strcat( ATCmd, "\"" );
				}
				strcat( ATCmd, "\r" );

				retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, 10000, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_GET_PASS:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				retval = MAT_Send( 0, "AT+CPIN?\r", -1 );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				data[datalen] = '\0';

				if ( strstr( data, "+CPIN:") == data && ME_MemGetOffset() == 0 )
				{
					ME_MemAssign( data, datalen );
					return 0;
				}

				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_PASS_REMAIN:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				retval = MAT_Send( 0, "AT#CPINC?\r", -1 );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				if ( memchr( data, '+', datalen) != NULL )
					return -1;

				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
				{
					ME_MemAssign( data, datalen );
					return 0;
				}

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}
		
		break;

	case ME_SET_OPRATOR:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				switch ( request_head->arg1 )
				{
				case MODE_AUTO:

					strcpy( ATCmd, "AT+COPS=0\r" );
					break;

				case MODE_UNREGIST:

					strcpy( ATCmd, "AT+COPS=2\r" );
					break;

				case MODE_MANUAL:

					if ( request_head->cmddata == NULL )
						goto ERROR_HANDLER;

					sprintf( ATCmd, "AT+COPS=4,2,%s\r", request_head->cmddata );
					break;

				default:
					goto ERROR_HANDLER;
				}

				retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_GET_OPRATOR:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				switch ( request_head->arg1 )
				{
				case FORMAT_LONGALPHA:
					retval = MAT_Send( 0, "AT+COPS=3,0\r", -1 );
					break;
				case FORMAT_SHORTALPHA:
					retval = MAT_Send( 0, "AT+COPS=3,1\r", -1 );
					break;
				case FORMAT_NUMERIC:
					retval = MAT_Send( 0, "AT+COPS=3,2\r", -1 );
					break;
				default:
					goto ERROR_HANDLER;
				}
			
				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer( NULL, TimerId );

				if ( retval == FRC_OK)
				{
					retval = MAT_Send( 0, "AT+COPS?\r", -1 );

					if ( retval != -1 )
					{
						TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
						me_fsm_data.state = ME_FSM_WTRSPNS_2;
					}
					else
						goto ERROR_HANDLER;
				}
				else
				{
					GetResultLength( request_head );
					me_fsm_data.state = ME_FSM_DONE;
				}
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}
		else if ( ME_FSM_WTRSPNS_2 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				data[datalen] = '\0';

				if ( strstr( data, "+COPS:") == data && ME_MemGetOffset() == 0 )
				{
					ME_MemAssign( data, datalen );
					return 0;
				}
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer( NULL, TimerId );
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}
		break;

	case ME_SRCH_OPRATOR:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				retval = MAT_Send( 0, "AT+COPS=?\r", -1 );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, 60000, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				data[datalen] = '\0';

				if ( strstr( data, "+COPS:") == data && ME_MemGetOffset() == 0 )
				{
					ME_MemAssign( data, datalen );
					return 0;
				}
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( USERSTOP == event )
			{
				retval = MAT_Send( 0, "AT+WAC\r", -1 );

				KillTimer( NULL, TimerId );
				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_2;
				}
				else
					goto ERROR_HANDLER;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}
		else if ( ME_FSM_WTRSPNS_2 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );
				
				if ( retval < 0 )
					return retval;

				KillTimer( NULL, TimerId );
				if ( retval == FRC_OK )
					request_head->error = ME_RS_USERSTOP;

				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}
		break;

	case ME_GET_POPRATOR:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				retval = MAT_Send( 0, "AT+CPOL=,2\r", -1 );
			
				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer( NULL, TimerId );

				if ( retval == FRC_OK)
				{
					retval = MAT_Send( 0, "AT+CPOL?\r", -1 );

					if ( retval != -1 )
					{
						TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
						me_fsm_data.state = ME_FSM_WTRSPNS_2;
					}
					else
						goto ERROR_HANDLER;
				}
				else
				{
					GetResultLength( request_head );
					me_fsm_data.state = ME_FSM_DONE;
				}
			}
			else if ( TIMEOUT == event )
				goto ERROR_HANDLER;
		}
		else if ( ME_FSM_WTRSPNS_2 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				data[datalen] = '\0';

				if ( strstr( data, "+CPOL:") == data )
				{
					ME_MemAssign( data, datalen );
					return 0;
				}
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer( NULL, TimerId );
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}
		break;

	case ME_SET_POPRATOR:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				int index;
				char *oper;

				index = request_head->arg1;
				oper = request_head->cmddata;

				if ( index == 0 )
					goto ERROR_HANDLER;

				if ( oper == NULL )
					sprintf( ATCmd, "AT+CPOL=%d\r", index );
				else
					sprintf( ATCmd, "AT+CPOL=%d,2,\"%s\"\r", index, oper );

				retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_GET_IMEI:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				retval = MAT_Send( 0, "AT+CGSN\r", -1 );
			
				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				if ( memchr( data, '+', datalen) != NULL )
					return -1;

				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
				{
					ME_MemAssign( data, datalen );
					return 0;
				}

				KillTimer( NULL, TimerId );
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_GET_IMSI:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				retval = MAT_Send( 0, "AT+CIMI\r", -1 );
			
				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				if ( memchr( data, '+', datalen) != NULL )
					return -1;

				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
				{
					ME_MemAssign( data, datalen );
					return 0;
				}

				KillTimer( NULL, TimerId );
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_SET_CLOCK:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				sprintf( ATCmd, "AT#CCLK=\"%s\"\r", request_head->cmddata );

				retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_GET_CLOCK:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				retval = MAT_Send( 0, "AT#CCLK?\r", -1 );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				if ( strstr( data, "#CCLK:") == data && ME_MemGetOffset() == 0 )
				{
					ME_MemAssign( data, datalen );
					return 0;
				}

				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_MUTE_CONTROL:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				if ( request_head->arg1 )
					strcpy( ATCmd, "AT+CMUT=1\r" );
				else
					strcpy( ATCmd, "AT+CMUT=0\r" );

				retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_SEND_DTMF:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				sprintf( ATCmd, "AT+VTS=%c\r", request_head->arg1 );

				retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_FAC_SETTING:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				retval = MAT_Send( 0, "AT&F\r", -1 );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_RESET:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				retval = MAT_Send( 0, "AT+CFUN=0\r", -1 );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_SHUTDOWN:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				retval = MAT_Send( 0, "AT+COPS=2\r", -1 );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);

				retval = MAT_Send( 0, "AT#PWROFF\r", -1 );

				if ( retval != -1 )
				{
					GetResultLength( request_head );
					me_fsm_data.state = ME_FSM_DONE;
				}
				else
					goto ERROR_HANDLER;
			}
			else if ( TIMEOUT == event )
			{
				retval = MAT_Send( 0, "AT#PWROFF\r", -1 );

				if ( retval != -1 )
				{
					GetResultLength( request_head );
					me_fsm_data.state = ME_FSM_DONE;
				}
				else
					goto ERROR_HANDLER;
			}
		}

		break;

	case ME_SET_VBDR:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				retval = MAT_Send( 0, "AT#SETVBDR\r", -1 );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				if (memcmp(data, "#SETVBDR:", 9)!=0)
					return -1;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_EAR_AUDIO:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				if ( request_head->arg1 )
					retval = MAT_Send( 0, "AT#AUDIO=1\r", -1 );
				else
					retval = MAT_Send( 0, "AT#AUDIO=0\r", -1 );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				if (memcmp(data, "#AUDIO:", 7)!=0)
					return -1;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_SET_VOLUME:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				if ( request_head->arg1 < 0 || request_head->arg1 > 255 )
					goto ERROR_HANDLER;

				sprintf( ATCmd, "AT+CLVL=%d\r", request_head->arg1 );

				retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_GET_VOLUME:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				retval = MAT_Send( 0, "AT+CLVL?\r", -1 );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				data[datalen] = '\0';

				if ( strstr( data, "+CLVL:") == data && ME_MemGetOffset() == 0 )
				{
					ME_MemAssign( data, datalen );
					return 0;
				}
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_SET_CLIR:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				switch( request_head->arg1) {
				case CLIR_DEFAULT:
					strcpy( ATCmd, "AT+CLIR=0\r" );
					break;
				case CLIR_ENABLE:
					strcpy( ATCmd, "AT+CLIR=1\r" );
					break;
				case CLIR_DISABLE:
					strcpy( ATCmd, "AT+CLIR=2\r" );
					break;
				}

				retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_GET_CLIR:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				retval = MAT_Send( 0, "AT+CLIR?\r", -1 );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				data[datalen] = '\0';

				if ( strstr( data, "+CLIR:") == data && ME_MemGetOffset() == 0 )
				{
					ME_MemAssign( data, datalen );
					return 0;
				}
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_GET_SIGNAL:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				retval = MAT_Send( 0, "AT+CSQ\r", -1 );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				data[datalen] = '\0';

				if ( strstr( data, "+CSQ:") == data && ME_MemGetOffset() == 0 )
				{
					ME_MemAssign( data, datalen );
					return 0;
				}
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_GET_BATTERY:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				retval = MAT_Send( 0, "AT#CBC\r", -1 );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				data[datalen] = '\0';

				if ( strstr( data, "#CBC:") == data && ME_MemGetOffset() == 0 )
				{
					ME_MemAssign( data, datalen );
					return 0;
				}
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_PB_READY:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				retval = MAT_Send( 0, "AT#PHBSTA\r", -1 );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				data[datalen] = '\0';

				if ( strstr( data, "#PHBSTA:") == data && ME_MemGetOffset() == 0 )
				{
					ME_MemAssign( data, datalen );
					return 0;
				}
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;

	case ME_DISABLE_SIM:

		if ( ME_FSM_INIT == me_fsm_data.state )
		{
			if ( START == event )
			{
				retval = MAT_Send( 0, "AT+CFUN=0\r", -1 );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_1;
				}
				else
					goto ERROR_HANDLER;
			}
		}
		else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
		{
			if ( DATAIN == event )
			{
				retval = GetResultErrorCode( data, datalen );

				if ( retval < 0 )
					return retval;

				KillTimer(NULL, TimerId);
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
			else if ( TIMEOUT == event )
				goto TIMEOUT_HANDLER;
		}

		break;
    }

    return 0;

ERROR_HANDLER:
	request_head->error = ME_RS_FAILURE;
	GetResultLength( request_head );
	me_fsm_data.state = ME_FSM_DONE;

	return 0;

TIMEOUT_HANDLER:
	request_head->error = ME_RS_TIMEOUT;
	request_head->errcode = 0;
	GetResultLength( request_head );
	me_fsm_data.state = ME_FSM_DONE;

	return 0;
}

int FSM_SetCallFwding( int event, char* data, int datalen )
{
	char ATCmd[ATCMD_BUFSIZE];
	int retval;
	char* PhoneNum;
	int reason, Class, time;
	BOOL enable;

	if ( ME_FSM_INIT == me_fsm_data.state )
	{
		if ( START == event )
		{
			reason = ((PCALL_FWD)request_head->cmddata)->Reason;
			enable = ((PCALL_FWD)request_head->cmddata)->bEnable;
			Class = ((PCALL_FWD)request_head->cmddata)->Class;
			PhoneNum = ((PCALL_FWD)request_head->cmddata)->PhoneNum;
			time = ((PCALL_FWD)request_head->cmddata)->Time;

			switch ( Class )
			{
			case CLASS_VOICE:

				if ( FWD_UNCDTIONAL == reason )
				{
					if ( enable )
						sprintf( ATCmd, "AT+CCFC=0,3,\"%s\",,1\r", PhoneNum );
					else
						sprintf( ATCmd, "AT+CCFC=0,0,,,1\r" );
				}
				else if ( FWD_BUSY == reason )
				{
					if ( enable )
						sprintf( ATCmd, "AT+CCFC=1,3,\"%s\",,1\r", PhoneNum );
					else
						sprintf( ATCmd, "AT+CCFC=1,0,,,1\r" );
				}
				else if ( FWD_NOREPLY == reason )
				{
					if ( enable )
					{
						if ( time > 30 || (time > 0 && time%5 != 0))
							goto ERROR_HANDLER;

						if ( time < 0 )
							sprintf( ATCmd, "AT+CCFC=2,3,\"%s\",,1\r", PhoneNum );
						else
							sprintf( ATCmd, "AT+CCFC=2,3,\"%s\",,1,%d\r", PhoneNum, time );
					}
					else
						sprintf( ATCmd, "AT+CCFC=2,0,,,1\r" );
				}
				else if ( FWD_NOREACH == reason )
				{
					if ( enable )
						sprintf( ATCmd, "AT+CCFC=3,3,\"%s\",,1\r", PhoneNum );
					else
						sprintf( ATCmd, "AT+CCFC=3,0,,,1\r" );
				}
				else if ( FWD_ALLCDTIONAL == reason )
				{
					if ( enable )
						sprintf( ATCmd, "AT+CCFC=5,3,\"%s\",,1\r", PhoneNum );
					else
						sprintf( ATCmd, "AT+CCFC=5,0,,,1\r" );
				}
				else if ( FWD_ALL == reason )
				{
					if ( enable )
						sprintf( ATCmd, "AT+CCFC=4,3,\"%s\",,1\r", PhoneNum );
					else
						sprintf( ATCmd, "AT+CCFC=4,0,,,1\r" );
				}
				else
					goto ERROR_HANDLER;

				break;

			case CLASS_DATA:

				if ( FWD_UNCDTIONAL == reason )
				{
					if ( enable )
						sprintf( ATCmd, "AT+CCFC=0,3,\"%s\",,2\r", PhoneNum );
					else
						sprintf( ATCmd, "AT+CCFC=0,0,,,2\r" );
				}
				else if ( FWD_BUSY == reason )
				{
					if ( enable )
						sprintf( ATCmd, "AT+CCFC=1,3,\"%s\",,2\r", PhoneNum );
					else
						sprintf( ATCmd, "AT+CCFC=1,0,,,2\r" );
				}
				else if ( FWD_NOREPLY == reason )
				{
					if ( enable )
					{
						if ( time > 30 || (time > 0 && time%5 != 0))
							goto ERROR_HANDLER;

						if ( time < 0 )
							sprintf( ATCmd, "AT+CCFC=2,3,\"%s\",,2\r", PhoneNum );
						else
							sprintf( ATCmd, "AT+CCFC=2,3,\"%s\",,2,%d\r", PhoneNum, time );
					}
					else
						sprintf( ATCmd, "AT+CCFC=2,0,,,2\r" );
				}
				else if ( FWD_NOREACH == reason )
				{
					if ( enable )
						sprintf( ATCmd, "AT+CCFC=3,3,\"%s\",,2\r", PhoneNum );
					else
						sprintf( ATCmd, "AT+CCFC=3,0,,,2\r" );
				}
				else if ( FWD_ALLCDTIONAL == reason )
				{
					if ( enable )
						sprintf( ATCmd, "AT+CCFC=5,3,\"%s\",,2\r", PhoneNum );
					else
						sprintf( ATCmd, "AT+CCFC=5,0,,,2\r" );
				}
				else if ( FWD_ALL == reason )
				{
					if ( enable )
						sprintf( ATCmd, "AT+CCFC=4,3,\"%s\",,2\r", PhoneNum );
					else
						sprintf( ATCmd, "AT+CCFC=4,0,,,2\r" );
				}
				else
					goto ERROR_HANDLER;

				break;

			case CLASS_ALL:

				if ( FWD_UNCDTIONAL == reason )
				{
					if ( enable )
						sprintf( ATCmd, "AT+CCFC=0,3,\"%s\",,5\r", PhoneNum );
					else
						sprintf( ATCmd, "AT+CCFC=0,0,,,5\r" );
				}
				else if ( FWD_BUSY == reason )
				{
					if ( enable )
						sprintf( ATCmd, "AT+CCFC=1,3,\"%s\",,5\r", PhoneNum );
					else
						sprintf( ATCmd, "AT+CCFC=1,0,,,5\r" );
				}
				else if ( FWD_NOREPLY == reason )
				{
					if ( enable )
					{
						if ( time > 30 || (time > 0 && time%5 != 0))
							goto ERROR_HANDLER;

						if ( time < 0 )
							sprintf( ATCmd, "AT+CCFC=2,3,\"%s\",,5\r", PhoneNum );
						else
							sprintf( ATCmd, "AT+CCFC=2,3,\"%s\",,5,%d\r", PhoneNum, time );
					}
					else
						sprintf( ATCmd, "AT+CCFC=2,0,,,5\r" );
				}
				else if ( FWD_NOREACH == reason )
				{
					if ( enable )
						sprintf( ATCmd, "AT+CCFC=3,3,\"%s\",,5\r", PhoneNum );
					else
						sprintf( ATCmd, "AT+CCFC=3,0,,,5\r" );
				}
				else if ( FWD_ALLCDTIONAL == reason )
				{
					if ( enable )
						sprintf( ATCmd, "AT+CCFC=5,3,\"%s\",,5\r", PhoneNum );
					else
						sprintf( ATCmd, "AT+CCFC=5,0,,,5\r" );
				}
				else if ( FWD_ALL == reason )
				{
					if ( enable )
						sprintf( ATCmd, "AT+CCFC=4,3,\"%s\",,5\r", PhoneNum );
					else
						sprintf( ATCmd, "AT+CCFC=4,0,,,5\r" );
				}
				else
					goto ERROR_HANDLER;

				break;

			default:
				goto ERROR_HANDLER;
			}

			retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );

			if ( retval != -1 )
			{
				TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
				me_fsm_data.state = ME_FSM_WTRSPNS_1;
			}
			else
				goto ERROR_HANDLER;
		}
	}
	else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			retval = GetResultErrorCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer(NULL, TimerId);
			GetResultLength( request_head );
			me_fsm_data.state = ME_FSM_DONE;
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}

	return 0;

ERROR_HANDLER:
	request_head->error = ME_RS_FAILURE;
	request_head->errcode = 1;
	GetResultLength( request_head );
	me_fsm_data.state = ME_FSM_DONE;

	return 0;

TIMEOUT_HANDLER:
	request_head->error = ME_RS_TIMEOUT;
	request_head->errcode = 0;
	GetResultLength( request_head );
	me_fsm_data.state = ME_FSM_DONE;

	return 0;
}

int FSM_GetCallFwding( int event, char* data, int datalen )
{
	char ATCmd[ATCMD_BUFSIZE];
	int retval;
	int reason, Class;

	if ( ME_FSM_INIT == me_fsm_data.state )
	{
		if ( START == event )
		{
			reason = request_head->arg1;
			Class = request_head->arg2;

			switch ( Class )
			{
			case CLASS_VOICE:

				if ( FWD_UNCDTIONAL == reason )
					strcpy( ATCmd, "AT+CCFC=0,2,,,1\r" );
				else if ( FWD_BUSY == reason )
					strcpy( ATCmd, "AT+CCFC=1,2,,,1\r" );
				else if ( FWD_NOREPLY == reason )
					strcpy( ATCmd, "AT+CCFC=2,2,,,1\r" );
				else if ( FWD_NOREACH == reason )
					strcpy( ATCmd, "AT+CCFC=3,2,,,1\r" );
				else
					goto ERROR_HANDLER;

				break;

			case CLASS_DATA:

				if ( FWD_UNCDTIONAL == reason )
					strcpy( ATCmd, "AT+CCFC=0,2,,,2\r" );
				else if ( FWD_BUSY == reason )
					strcpy( ATCmd, "AT+CCFC=1,2,,,2\r" );
				else if ( FWD_NOREPLY == reason )
					strcpy( ATCmd, "AT+CCFC=2,2,,,2\r" );
				else if ( FWD_NOREACH == reason )
					strcpy( ATCmd, "AT+CCFC=3,2,,,2\r" );
				else
					goto ERROR_HANDLER;

				break;

			default:

				goto ERROR_HANDLER;
			}

			retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );

			if ( retval != -1 )
			{
				TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
				me_fsm_data.state = ME_FSM_WTRSPNS_1;
			}
			else
				goto ERROR_HANDLER;
		}
	}
	else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			data[datalen] = '\0';

			if ( strstr( data, "+CCFC:") == data && ME_MemGetOffset() == 0 )
			{
				ME_MemAssign( data, datalen );
				return 0;
			}
			retval = GetResultErrorCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer(NULL, TimerId);
			GetResultLength( request_head );
			me_fsm_data.state = ME_FSM_DONE;
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}

	return 0;

ERROR_HANDLER:
	request_head->error = ME_RS_FAILURE;
	request_head->errcode = 1;
	GetResultLength( request_head );
	me_fsm_data.state = ME_FSM_DONE;

	return 0;

TIMEOUT_HANDLER:
	request_head->error = ME_RS_TIMEOUT;
	request_head->errcode = 0;
	GetResultLength( request_head );
	me_fsm_data.state = ME_FSM_DONE;

	return 0;
}

int FSM_SetCallBarring( int event, char* data, int datalen )
{
	char ATCmd[ATCMD_BUFSIZE];
	int retval;
	char* Password;
	int fac, Class;
	BOOL enable;

	if ( ME_FSM_INIT == me_fsm_data.state )
	{
		if ( START == event )
		{
			fac = request_head->arg1;
			enable = request_head->arg2;
			Class = request_head->arg3;
			Password = request_head->cmddata;

			strcpy( ATCmd, "AT+CLCK=" );

			switch ( fac )
			{
			case CALLBAR_BAOC:

				strcat( ATCmd, "\"AO\"," );

				break;

			case CALLBAR_BOIC:

				strcat( ATCmd, "\"OI\"," );

				break;

			case CALLBAR_BOIC_EXHC:

				strcat( ATCmd, "\"OX\"," );

				break;

			case CALLBAR_BAIC:

				strcat( ATCmd, "\"AI\"," );

				break;

			case CALLBAR_BIC_ROAM:

				strcat( ATCmd, "\"IR\"," );

				break;

			case CALLBAR_UAB:

				if ( enable )
					goto ERROR_HANDLER;
				strcat( ATCmd, "\"AB\"," );
				break;

			case CALLBAR_UAG:

				if ( enable )
					goto ERROR_HANDLER;
				strcat( ATCmd, "\"AG\"," );
				break;

			case CALLBAR_UAC:

				if ( enable )
					return FALSE;
				strcat( ATCmd, "\"AC\"," );
				break;

			default:
				goto ERROR_HANDLER;
			}

			if ( enable )
				strcat( ATCmd, "1," );
			else
				strcat( ATCmd, "0," );

			if ( Password != NULL )
			{
				strcat( ATCmd, "\"" );
				strcat( ATCmd, Password );
				strcat( ATCmd, "\"" );
			}

			strcat( ATCmd, "," );

			switch ( Class )
			{
			case CLASS_VOICE:

				strcat( ATCmd, "1" );

				break;

			case CLASS_DATA:

				strcat( ATCmd, "2" );

				break;

			case CLASS_ALL:

				strcat( ATCmd, "5" );

				break;

			default:
				goto ERROR_HANDLER;
			}

			strcat( ATCmd, "\r" );

			retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );

			if ( retval != -1 )
			{
				TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
				me_fsm_data.state = ME_FSM_WTRSPNS_1;
			}
			else
				goto ERROR_HANDLER;
		}
	}
	else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			retval = GetResultErrorCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer(NULL, TimerId);
			GetResultLength( request_head );
			me_fsm_data.state = ME_FSM_DONE;
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}

	return 0;

ERROR_HANDLER:
	request_head->error = ME_RS_FAILURE;
	request_head->errcode = 1;
	GetResultLength( request_head );
	me_fsm_data.state = ME_FSM_DONE;

	return 0;

TIMEOUT_HANDLER:
	request_head->error = ME_RS_TIMEOUT;
	request_head->errcode = 0;
	GetResultLength( request_head );
	me_fsm_data.state = ME_FSM_DONE;

	return 0;
}

int FSM_GetCallBarring( int event, char* data, int datalen )
{
	char ATCmd[ATCMD_BUFSIZE];
	int retval;
	char* Password;
	int fac, Class;

	if ( ME_FSM_INIT == me_fsm_data.state )
	{
		if ( START == event )
		{
			fac = request_head->arg1;
			Class = request_head->arg2;
			Password = request_head->cmddata;

			strcpy( ATCmd, "AT+CLCK=" );

			switch ( fac )
			{
			case CALLBAR_BAOC:

				strcat( ATCmd, "\"AO\"," );

				break;

			case CALLBAR_BOIC:

				strcat( ATCmd, "\"OI\"," );

				break;

			case CALLBAR_BOIC_EXHC:

				strcat( ATCmd, "\"OX\"," );

				break;

			case CALLBAR_BAIC:

				strcat( ATCmd, "\"AI\"," );

				break;

			case CALLBAR_BIC_ROAM:

				strcat( ATCmd, "\"IR\"," );

				break;

			default:
				goto ERROR_HANDLER;
			}

			strcat( ATCmd, "2," );

			if ( Password != NULL )
			{
				strcat( ATCmd, "\"" );
				strcat( ATCmd, Password );
				strcat( ATCmd, "\"" );
			}

			strcat( ATCmd, "," );

			switch ( Class )
			{
			case CLASS_VOICE:

				strcat( ATCmd, "1" );

				break;

			case CLASS_DATA:

				strcat( ATCmd, "2" );

				break;

			default:
				goto ERROR_HANDLER;

			}

			strcat( ATCmd, "\r" );

			retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );

			if ( retval != -1 )
			{
				TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
				me_fsm_data.state = ME_FSM_WTRSPNS_1;
			}
			else
				goto ERROR_HANDLER;
		}
	}
	else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			data[datalen] = '\0';

			if ( strstr( data, "+CLCK:") == data && ME_MemGetOffset() == 0 )
			{
				ME_MemAssign( data, datalen );
				return 0;
			}
			retval = GetResultErrorCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer(NULL, TimerId);
			GetResultLength( request_head );
			me_fsm_data.state = ME_FSM_DONE;
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}

	return 0;

ERROR_HANDLER:
	request_head->error = ME_RS_FAILURE;
	request_head->errcode = 1;
	GetResultLength( request_head );
	me_fsm_data.state = ME_FSM_DONE;

	return 0;

TIMEOUT_HANDLER:
	request_head->error = ME_RS_TIMEOUT;
	request_head->errcode = 0;
	GetResultLength( request_head );
	me_fsm_data.state = ME_FSM_DONE;

	return 0;
}

#define ME_SMS_PDU_LEN			660
int FSM_SMSSend( int event, char* data, int datalen )
{
	char ATCmd[ATCMD_BUFSIZE];
	char PDUCode[ME_SMS_PDU_LEN];
	int retval;
	char* temp;

	if ( ME_FSM_INIT == me_fsm_data.state )
	{
		if ( START == event )
		{
			retval = MAT_Send( 0, "AT+CMGF?\r", -1 );

			if ( retval != -1 )
			{
				TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
				me_fsm_data.state = ME_FSM_WTRSPNS_1;
			}
			else
				goto ERROR_HANDLER;
		}
	}
	else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			data[datalen] = '\0';

			if ( strstr( data, "+CMGF:") == data && ME_MemGetOffset() == 0 )
			{
				ME_MemAssign( data, datalen );
				return 0;
			}
			retval = IsFinalResultCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer( NULL, TimerId );
			temp = ME_MemGetPntr();

			retval = ME_GetIntSection( temp, "+CMGF:", 1 );

			if ( retval == 1 )
			{
				retval = MAT_Send( 0, "AT+CMGF=0\r", -1 );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_2;
				}
				else
					goto ERROR_HANDLER;
			}
			else if ( retval == 0 )
			{
				int len;
				PSMS_SEND pSend = request_head->cmddata;

				memset( PDUCode, 0, ME_SMS_PDU_LEN );
				len = ME_TranStr_Code( pSend, PDUCode, ME_SMS_PDU_LEN );

				if ( len > 0 )
				{
					sprintf( ATCmd, "AT+CMGS=%d\r", len );
					request_head->cmddata = malloc( strlen(PDUCode)+1 );
					strcpy( request_head->cmddata, PDUCode );
					request_head->cmddata_len = strlen(PDUCode);

					retval = MAT_Send( 0, ATCmd, -1 );

					if ( retval != -1 )
					{
						TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
						me_fsm_data.state = ME_FSM_WTRSPNS_3;
					}
					else
						goto ERROR_HANDLER;
				}
				else
					goto ERROR_HANDLER;
			}
			else
				goto ERROR_HANDLER;
		}
		else if ( TIMEOUT == event )
			goto ERROR_HANDLER;
	}
	else if ( ME_FSM_WTRSPNS_2 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			retval = GetResultErrorCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer( NULL, TimerId );

			if ( retval == FRC_OK )
			{
				int len;
				PSMS_SEND pSend = request_head->cmddata;

				memset( PDUCode, 0, ME_SMS_PDU_LEN );
				len = ME_TranStr_Code( pSend, PDUCode, ME_SMS_PDU_LEN );

				if ( len > 0 )
				{
					sprintf( ATCmd, "AT+CMGS=%d\r", len );
					request_head->cmddata = malloc( strlen(PDUCode)+1 );
					strcpy( request_head->cmddata, PDUCode );
					request_head->cmddata_len = strlen(PDUCode);

					retval = MAT_Send( 0, ATCmd, -1 );

					if ( retval != -1 )
					{
						TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
						me_fsm_data.state = ME_FSM_WTRSPNS_3;
					}
					else
					{
						free( request_head->cmddata);
						goto ERROR_HANDLER;
					}
				}
				else
					goto ERROR_HANDLER;
			}
			else
			{
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}
	else if ( ME_FSM_WTRSPNS_3 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			if ( datalen == 1 && data[0] == '>' )
			{
				KillTimer( NULL, TimerId );
				retval = MAT_Send( 0, request_head->cmddata, request_head->cmddata_len );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_4;
				}
				else
				{
					free( request_head->cmddata);
					goto ERROR_HANDLER;
				}
			}
			else
			{
				retval = GetResultErrorCode( data, datalen );
		
				if ( retval < 0 )
					return retval;

				free( request_head->cmddata);
				KillTimer( NULL, TimerId );
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
		}
		else if ( TIMEOUT == event )
		{
			free( request_head->cmddata);
			goto TIMEOUT_HANDLER;
		}
	}
	else if ( ME_FSM_WTRSPNS_4 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			retval = GetResultErrorCode( data, datalen );

			if ( retval < 0 )
				return retval;

			free( request_head->cmddata);
			KillTimer( NULL, TimerId );
			GetResultLength( request_head );
			me_fsm_data.state = ME_FSM_DONE;
		}
		else if ( TIMEOUT == event )
		{
			char escape;
			escape = 0x1B;
			free( request_head->cmddata);
            retval = MAT_Send( 0, &escape, 1 );
			if ( retval != -1 )
			{
				TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
				me_fsm_data.state = ME_FSM_WTRSPNS_5;
			}
            else
                goto TIMEOUT_HANDLER;
		}
	}
	else if ( ME_FSM_WTRSPNS_5 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			retval = GetResultErrorCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer( NULL, TimerId );
            goto TIMEOUT_HANDLER;
		}
		else if ( TIMEOUT == event )
            goto TIMEOUT_HANDLER;
    }

	return 0;

ERROR_HANDLER:
	request_head->error = ME_RS_FAILURE;
	request_head->errcode = 1;
	GetResultLength( request_head );
	me_fsm_data.state = ME_FSM_DONE;

	return 0;

TIMEOUT_HANDLER:
	request_head->error = ME_RS_TIMEOUT;
	request_head->errcode = 0;
	GetResultLength( request_head );
	me_fsm_data.state = ME_FSM_DONE;

	return 0;
}

int FSM_SMSRead( int event, char* data, int datalen )
{
	char ATCmd[ATCMD_BUFSIZE];
	int retval;
	char* temp;

#if defined CAP_SMS
	if ( ME_FSM_INIT == me_fsm_data.state )
	{
		if ( START == event )
		{
			switch ( request_head->arg2 )
			{
			case SMS_MEM_SM:
				retval = MAT_Send( 0, "AT+CPMS=\"SM\"\r", -1 );
				break;
			case SMS_MEM_ME:
				retval = MAT_Send( 0, "AT+CPMS=\"ME\"\r", -1 );
				break;
			default:
				goto ERROR_HANDLER;
			}
			if ( retval != -1 )
			{
				TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
				me_fsm_data.state = ME_FSM_WTRSPNS_1;
			}
			else
				goto ERROR_HANDLER;
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}
	else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			retval = GetResultErrorCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer( NULL, TimerId );

			if ( retval == FRC_OK)
			{
				retval = MAT_Send( 0, "AT+CMGF?\r", -1 );
				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_2;
				}
				else
					goto ERROR_HANDLER;
			}
			else
			{
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}
	else if ( ME_FSM_WTRSPNS_2 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			data[datalen] = '\0';

			if ( strstr( data, "+CMGF:") == data && ME_MemGetOffset() == 0 )
			{
				ME_MemAssign( data, datalen );
				return 0;
			}
			retval = IsFinalResultCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer( NULL, TimerId );

			if ( retval == FRC_OK )
			{
				temp = ME_MemGetPntr();

				retval = ME_GetIntSection( temp, "+CMGF:", 1 );
				
				if ( retval == 1 )
				{
					retval = MAT_Send( 0, "AT+CMGF=0\r", -1 );
					
					if ( retval != -1 )
					{
						TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
						me_fsm_data.state = ME_FSM_WTRSPNS_3;
					}
					else
						goto ERROR_HANDLER;
				}
				else if ( retval == 0 )
				{
					sprintf( ATCmd, "AT+CMGR=%d\r", request_head->arg3 );
					
					retval = MAT_Send( 0, ATCmd, -1 );
					
					if ( retval != -1 )
					{
						TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
						me_fsm_data.state = ME_FSM_WTRSPNS_4;
					}
					else
						goto ERROR_HANDLER;
				}
				else
					goto ERROR_HANDLER;
			}
			else
				goto ERROR_HANDLER;
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}
	else if ( ME_FSM_WTRSPNS_3 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			retval = GetResultErrorCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer( NULL, TimerId );

			if ( retval == FRC_OK )
			{
				sprintf( ATCmd, "AT+CMGR=%d\r", request_head->arg3 );

				retval = MAT_Send( 0, ATCmd, -1 );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_4;
				}
				else
					goto ERROR_HANDLER;
			}
			else
			{
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}
	else if ( ME_FSM_WTRSPNS_4 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			data[datalen] = '\0';

			if ( strstr( data, "+CMGR:") == data )
			{
				ME_MemAssign(data, datalen );
				me_fsm_data.state = ME_FSM_WTRSPNS_5;
				return 0;
			}
			retval = GetResultErrorCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer( NULL, TimerId );
			GetResultLength( request_head );
			me_fsm_data.state = ME_FSM_DONE;
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}
	else if ( ME_FSM_WTRSPNS_5 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			ME_MemAssign( data, datalen );
			me_fsm_data.state = ME_FSM_WTRSPNS_4;
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}

	return 0;
#else
	if ( ME_FSM_INIT == me_fsm_data.state )
	{
		if ( START == event )
		{
			switch ( request_head->arg2 )
			{
			case SMS_MEM_SM:
				retval = MAT_Send( 0, "AT+CPMS=\"SM\"\r", -1 );
				break;
			case SMS_MEM_ME:
				retval = MAT_Send( 0, "AT+CPMS=\"ME\"\r", -1 );
				break;
			default:
				goto ERROR_HANDLER;
			}
			if ( retval != -1 )
			{
				TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
				me_fsm_data.state = ME_FSM_WTRSPNS_1;
			}
			else
				goto ERROR_HANDLER;
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}
	else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			retval = GetResultErrorCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer( NULL, TimerId );

			if ( retval == FRC_OK)
			{
				retval = MAT_Send( 0, "AT+CMGF?\r", -1 );
				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_2;
				}
				else
					goto ERROR_HANDLER;
			}
			else
			{
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}
	else if ( ME_FSM_WTRSPNS_2 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			data[datalen] = '\0';

			if ( strstr( data, "+CMGF:") == data && ME_MemGetOffset() == 0 )
			{
				ME_MemAssign( data, datalen );
				return 0;
			}
			retval = IsFinalResultCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer( NULL, TimerId );

			if ( retval == FRC_OK )
			{
				temp = ME_MemGetPntr();

				retval = ME_GetIntSection( temp, "+CMGF:", 1 );
				
				if ( retval == 1 )
				{
					retval = MAT_Send( 0, "AT+CMGF=0\r", -1 );
					
					if ( retval != -1 )
					{
						TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
						me_fsm_data.state = ME_FSM_WTRSPNS_3;
					}
					else
						goto ERROR_HANDLER;
				}
				else if ( retval == 0 )
				{
					if ( request_head->arg1 )
						sprintf( ATCmd, "AT#CMGR=%d\r", request_head->arg3 );
					else
						sprintf( ATCmd, "AT+CMGR=%d\r", request_head->arg3 );

					retval = MAT_Send( 0, ATCmd, -1 );

					if ( retval != -1 )
					{
						TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
						me_fsm_data.state = ME_FSM_WTRSPNS_4;
					}
					else
						goto ERROR_HANDLER;
				}
				else
					goto ERROR_HANDLER;
			}
			else
				goto ERROR_HANDLER;
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}
	else if ( ME_FSM_WTRSPNS_3 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			retval = GetResultErrorCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer( NULL, TimerId );

			if ( retval == FRC_OK )
			{
				if ( request_head->arg1 )
					sprintf( ATCmd, "AT#CMGR=%d\r", request_head->arg3 );
				else
					sprintf( ATCmd, "AT+CMGR=%d\r", request_head->arg3 );

				retval = MAT_Send( 0, ATCmd, -1 );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_4;
				}
				else
					goto ERROR_HANDLER;
			}
			else
			{
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}
	else if ( ME_FSM_WTRSPNS_4 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			static BOOL once = TRUE;
			data[datalen] = '\0';

			if ( strstr( data, "+CMGR:") == data )
			{
				ME_MemAssign(data, datalen );
				me_fsm_data.state = ME_FSM_WTRSPNS_5;
				return 0;
			}
			retval = GetResultErrorCode( data, datalen );

			if ( retval < 0 )
				return retval;

			if ( request_head->arg1 && retval == FRC_OK && once)
			{
				once = FALSE;
				return 0;
			}

			once = TRUE;
			KillTimer( NULL, TimerId );
			GetResultLength( request_head );
			me_fsm_data.state = ME_FSM_DONE;
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}
	else if ( ME_FSM_WTRSPNS_5 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			ME_MemAssign( data, datalen );
			me_fsm_data.state = ME_FSM_WTRSPNS_4;
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}

	return 0;
#endif


ERROR_HANDLER:
	request_head->error = ME_RS_FAILURE;
	request_head->errcode = 1;
	GetResultLength( request_head );
	me_fsm_data.state = ME_FSM_DONE;

	return 0;

TIMEOUT_HANDLER:
	request_head->error = ME_RS_TIMEOUT;
	request_head->errcode = 0;
	GetResultLength( request_head );
	me_fsm_data.state = ME_FSM_DONE;

	return 0;
}

int FSM_SMSList( int event, char* data, int datalen )
{
	int retval;
	char* temp;

#if defined CAP_SMS
	if ( ME_FSM_INIT == me_fsm_data.state )
	{
		if ( START == event )
		{
			switch ( request_head->arg2 )
			{
			case SMS_MEM_SM:
				retval = MAT_Send( 0, "AT+CPMS=\"SM\"\r", -1 );
				break;
			case SMS_MEM_ME:
				retval = MAT_Send( 0, "AT+CPMS=\"ME\"\r", -1 );
				break;
			default:
				goto ERROR_HANDLER;
			}
			if ( retval != -1 )
			{
				TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
				me_fsm_data.state = ME_FSM_WTRSPNS_1;
			}
			else
				goto ERROR_HANDLER;
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}
	else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			retval = GetResultErrorCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer( NULL, TimerId );

			if ( retval == FRC_OK )
			{
				retval = MAT_Send( 0, "AT+CMGF?\r", -1 );
				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_2;
				}
				else
					goto ERROR_HANDLER;
			}
			else
			{
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}
	else if ( ME_FSM_WTRSPNS_2 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			data[datalen] = '\0';

			if ( strstr( data, "+CMGF:") == data && ME_MemGetOffset() == 0 )
			{
				ME_MemAssign( data, datalen );
				return 0;
			}
			retval = IsFinalResultCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer( NULL, TimerId );
			temp = ME_MemGetPntr();

			retval = ME_GetIntSection( temp, "+CMGF:", 1 );
			
			if ( retval == 1 )
			{
				retval = MAT_Send( 0, "AT+CMGF=0\r", -1 );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_3;
				}
				else
					goto ERROR_HANDLER;
			}
			else if ( retval == 0 )
			{
				switch ( request_head->arg3 )
				{
				case SMS_UNREAD:
					retval = MAT_Send( 0, "AT+CMGL=0\r", -1 );
					break;

				case SMS_READED:
					retval = MAT_Send( 0, "AT+CMGL=1\r", -1 );
					break;

				case SMS_UNSEND:
					retval = MAT_Send( 0, "AT+CMGL=2\r", -1 );
					break;

				case SMS_SENDED:
					retval = MAT_Send( 0, "AT+CMGL=3\r", -1 );
					break;

				case SMS_ALL:
					retval = MAT_Send( 0, "AT+CMGL=4\r", -1 );
					break;

				default:
					goto ERROR_HANDLER;
				}

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, 10000, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_4;
				}
				else
					goto ERROR_HANDLER;
			}
			else
				goto ERROR_HANDLER;
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}
	else if ( ME_FSM_WTRSPNS_3 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			retval = GetResultErrorCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer( NULL, TimerId );

			if ( retval == FRC_OK )
			{
				switch ( request_head->arg3 )
				{
				case SMS_UNREAD:
					retval = MAT_Send( 0, "AT+CMGL=0\r", -1 );
					break;

				case SMS_READED:
					retval = MAT_Send( 0, "AT+CMGL=1\r", -1 );
					break;

				case SMS_UNSEND:
					retval = MAT_Send( 0, "AT+CMGL=2\r", -1 );
					break;

				case SMS_SENDED:
					retval = MAT_Send( 0, "AT+CMGL=3\r", -1 );
					break;

				case SMS_ALL:
					retval = MAT_Send( 0, "AT+CMGL=4\r", -1 );
					break;

				default:
					goto ERROR_HANDLER;
				}

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, 10000, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_4;
				}
				else
					goto ERROR_HANDLER;
			}
			else
			{
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}
	else if ( ME_FSM_WTRSPNS_4 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			data[datalen] = '\0';

			if ( strstr( data, "+CMGL:") == data )
			{
				ME_MemAssign(data, datalen );
				me_fsm_data.state = ME_FSM_WTRSPNS_5;
				return 0;
			}
			retval = GetResultErrorCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer( NULL, TimerId );
			GetResultLength( request_head );
			me_fsm_data.state = ME_FSM_DONE;
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}
	else if ( ME_FSM_WTRSPNS_5 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			ME_MemAssign( data, datalen );
			me_fsm_data.state = ME_FSM_WTRSPNS_4;
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}

	return 0;
#else
	if ( ME_FSM_INIT == me_fsm_data.state )
	{
		if ( START == event )
		{
			switch ( request_head->arg2 )
			{
			case SMS_MEM_SM:
				retval = MAT_Send( 0, "AT+CPMS=\"SM\"\r", -1 );
				break;
			case SMS_MEM_ME:
				retval = MAT_Send( 0, "AT+CPMS=\"ME\"\r", -1 );
				break;
			default:
				goto ERROR_HANDLER;
			}
			if ( retval != -1 )
			{
				TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
				me_fsm_data.state = ME_FSM_WTRSPNS_1;
			}
			else
				goto ERROR_HANDLER;
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}
	else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			retval = GetResultErrorCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer( NULL, TimerId );

			if ( retval == FRC_OK )
			{
				retval = MAT_Send( 0, "AT+CMGF?\r", -1 );
				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_2;
				}
				else
					goto ERROR_HANDLER;
			}
			else
			{
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}
	else if ( ME_FSM_WTRSPNS_2 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			data[datalen] = '\0';

			if ( strstr( data, "+CMGF:") == data && ME_MemGetOffset() == 0 )
			{
				ME_MemAssign( data, datalen );
				return 0;
			}
			retval = IsFinalResultCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer( NULL, TimerId );
			temp = ME_MemGetPntr();

			retval = ME_GetIntSection( temp, "+CMGF:", 1 );
			
			if ( retval == 1 )
			{
				retval = MAT_Send( 0, "AT+CMGF=0\r", -1 );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_3;
				}
				else
					goto ERROR_HANDLER;
			}
			else if ( retval == 0 )
			{
				switch ( request_head->arg3 )
				{
				case SMS_UNREAD:
					if ( request_head->arg1 )
						retval = MAT_Send( 0, "AT#CMGL=0\r", -1 );
					else
						retval = MAT_Send( 0, "AT+CMGL=0\r", -1 );
					break;

				case SMS_READED:
					if ( request_head->arg1 )
						retval = MAT_Send( 0, "AT#CMGL=1\r", -1 );
					else
						retval = MAT_Send( 0, "AT+CMGL=1\r", -1 );
					break;

				case SMS_UNSEND:
					if ( request_head->arg1 )
						retval = MAT_Send( 0, "AT#CMGL=2\r", -1 );
					else
						retval = MAT_Send( 0, "AT+CMGL=2\r", -1 );
					break;

				case SMS_SENDED:
					if ( request_head->arg1 )
						retval = MAT_Send( 0, "AT#CMGL=3\r", -1 );
					else
						retval = MAT_Send( 0, "AT+CMGL=3\r", -1 );
					break;

				case SMS_ALL:
					if ( request_head->arg1 )
						retval = MAT_Send( 0, "AT#CMGL=4\r", -1 );
					else
						retval = MAT_Send( 0, "AT+CMGL=4\r", -1 );
					break;

				default:
					goto ERROR_HANDLER;
				}

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, 15000, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_4;
				}
				else
					goto ERROR_HANDLER;
			}
			else
				goto ERROR_HANDLER;
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}
	else if ( ME_FSM_WTRSPNS_3 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			retval = GetResultErrorCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer( NULL, TimerId );

			if ( retval == FRC_OK )
			{
				switch ( request_head->arg3 )
				{
				case SMS_UNREAD:
					if ( request_head->arg1 )
						retval = MAT_Send( 0, "AT#CMGL=0\r", -1 );
					else
						retval = MAT_Send( 0, "AT+CMGL=0\r", -1 );
					break;

				case SMS_READED:
					if ( request_head->arg1 )
						retval = MAT_Send( 0, "AT#CMGL=1\r", -1 );
					else
						retval = MAT_Send( 0, "AT+CMGL=1\r", -1 );
					break;

				case SMS_UNSEND:
					if ( request_head->arg1 )
						retval = MAT_Send( 0, "AT#CMGL=2\r", -1 );
					else
						retval = MAT_Send( 0, "AT+CMGL=2\r", -1 );
					break;

				case SMS_SENDED:
					if ( request_head->arg1 )
						retval = MAT_Send( 0, "AT#CMGL=3\r", -1 );
					else
						retval = MAT_Send( 0, "AT+CMGL=3\r", -1 );
					break;

				case SMS_ALL:
					if ( request_head->arg1 )
						retval = MAT_Send( 0, "AT#CMGL=4\r", -1 );
					else
						retval = MAT_Send( 0, "AT+CMGL=4\r", -1 );
					break;

				default:
					goto ERROR_HANDLER;
				}

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, 15000, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_4;
				}
				else
					goto ERROR_HANDLER;
			}
			else
			{
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}
	else if ( ME_FSM_WTRSPNS_4 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			static BOOL once = TRUE;
			data[datalen] = '\0';

			if ( strstr( data, "+CMGL:") == data )
			{
				ME_MemAssign(data, datalen );
				me_fsm_data.state = ME_FSM_WTRSPNS_5;
				return 0;
			}
			retval = GetResultErrorCode( data, datalen );

			if ( retval < 0 )
				return retval;

			if ( request_head->arg1 && retval == FRC_OK && once)
			{
				once = FALSE;
				return 0;
			}

			once = TRUE;
			KillTimer( NULL, TimerId );
			GetResultLength( request_head );
			me_fsm_data.state = ME_FSM_DONE;
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}
	else if ( ME_FSM_WTRSPNS_5 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			ME_MemAssign( data, datalen );
			me_fsm_data.state = ME_FSM_WTRSPNS_4;
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}

	return 0;
#endif


ERROR_HANDLER:
	request_head->error = ME_RS_FAILURE;
	request_head->errcode = 1;
	GetResultLength( request_head );
	me_fsm_data.state = ME_FSM_DONE;

	return 0;

TIMEOUT_HANDLER:
	request_head->error = ME_RS_TIMEOUT;
	request_head->errcode = 0;
	GetResultLength( request_head );
	me_fsm_data.state = ME_FSM_DONE;

	return 0;
}

int FSM_SMSDelete( int event, char* data, int datalen )
{
	char ATCmd[ATCMD_BUFSIZE];
	int retval;

	if ( ME_FSM_INIT == me_fsm_data.state )
	{
		if ( START == event )
		{
			switch ( request_head->arg1 )
			{
			case SMS_MEM_SM:
				retval = MAT_Send( 0, "AT+CPMS=\"SM\"\r", -1 );
				break;
			case SMS_MEM_ME:
				retval = MAT_Send( 0, "AT+CPMS=\"ME\"\r", -1 );
				break;
			default:
				goto ERROR_HANDLER;
			}
			if ( retval != -1 )
			{
				TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
				me_fsm_data.state = ME_FSM_WTRSPNS_1;
			}
			else
				goto ERROR_HANDLER;
		}
	}
	else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			retval = GetResultErrorCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer( NULL, TimerId );

			if ( retval == FRC_OK)
			{
				sprintf( ATCmd, "AT+CMGD=%d\r", request_head->arg2 );
				retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );
				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_2;
				}
				else
					goto ERROR_HANDLER;
			}
			else
			{
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}
	else if ( ME_FSM_WTRSPNS_2 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			retval = GetResultErrorCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer( NULL, TimerId );
			GetResultLength( request_head );
			me_fsm_data.state = ME_FSM_DONE;
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}

	return 0;

ERROR_HANDLER:
	request_head->error = ME_RS_FAILURE;
	request_head->errcode = 1;
	GetResultLength( request_head );
	me_fsm_data.state = ME_FSM_DONE;

	return 0;

TIMEOUT_HANDLER:
	request_head->error = ME_RS_TIMEOUT;
	request_head->errcode = 0;
	GetResultLength( request_head );
	me_fsm_data.state = ME_FSM_DONE;

	return 0;
}

int FSM_PBookRead( int event, char* data, int datalen )
{
	char ATCmd[ATCMD_BUFSIZE];
	int retval;
	char* temp;

	if ( ME_FSM_INIT == me_fsm_data.state )
	{
		if ( START == event )
		{
			switch ( request_head->arg1 )
			{
			case PBOOK_SM:
				retval = MAT_Send( 0, "AT+CPBS=\"AD\"\r", -1 );
				break;
			case PBOOK_LD:
				retval = MAT_Send( 0, "AT+CPBS=\"LD\"\r", -1 );
				break;
			case PBOOK_MC:
				retval = MAT_Send( 0, "AT+CPBS=\"LM\"\r", -1 );
				break;
			case PBOOK_RC:
				retval = MAT_Send( 0, "AT+CPBS=\"LR\"\r", -1 );
				break;
			default:
				goto ERROR_HANDLER;
			}
			if ( retval != -1 )
			{
				TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
				me_fsm_data.state = ME_FSM_WTRSPNS_1;
			}
			else
				goto ERROR_HANDLER;
		}
	}
	else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			retval = GetResultErrorCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer( NULL, TimerId );

			if ( retval == FRC_OK)
			{
				retval = MAT_Send( 0, "AT+CSCS?\r", -1 );
				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_2;
				}
				else
					goto ERROR_HANDLER;
			}
			else
			{
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}
	else if ( ME_FSM_WTRSPNS_2 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			char result[20];
			data[datalen] = '\0';

			if ( strstr( data, "+CSCS:") == data && ME_MemGetOffset() == 0 )
			{
				ME_MemAssign( data, datalen );
				return 0;
			}
			retval = IsFinalResultCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer( NULL, TimerId );

			if ( retval == FRC_OK )
			{
				temp = ME_MemGetPntr();
				
				retval = ME_GetStrSection( temp, result, 20, "+CSCS:", 1 );
				
				if ( strcmp(result, "UCS2") != 0 )
				{
					retval = MAT_Send( 0, "AT+CSCS=\"UCS2\"\r", -1 );

					if ( retval != -1 )
					{
						TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
						me_fsm_data.state = ME_FSM_WTRSPNS_3;
					}
					else
						goto ERROR_HANDLER;
				}
				else
				{
					if ( request_head->arg3 == 0 )
						sprintf( ATCmd, "AT+CPBR=%d\r", request_head->arg2 );
					else if ( request_head->arg3 > request_head->arg2 )
						sprintf( ATCmd, "AT+CPBR=%d,%d\r", request_head->arg2, request_head->arg3 );
					else
						goto ERROR_HANDLER;
					
					retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );
					
					if ( retval != -1 )
					{
						TimerId = SetTimer( NULL, 0, 30000, ME_TimerProc );
						me_fsm_data.state = ME_FSM_WTRSPNS_4;
					}
					else
						goto ERROR_HANDLER;
				}
			}
			else
				goto ERROR_HANDLER;
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}
	else if ( ME_FSM_WTRSPNS_3 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			retval = GetResultErrorCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer( NULL, TimerId );

			if ( retval == FRC_OK )
			{
				if ( request_head->arg3 == 0 )
					sprintf( ATCmd, "AT+CPBR=%d\r", request_head->arg2 );
				else if ( request_head->arg3 > request_head->arg2 )
					sprintf( ATCmd, "AT+CPBR=%d,%d\r", request_head->arg2, request_head->arg3 );
				else
					goto ERROR_HANDLER;

				retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );

				if ( retval != -1 )
				{
					TimerId = SetTimer( NULL, 0, 30000, ME_TimerProc );
					me_fsm_data.state = ME_FSM_WTRSPNS_4;
				}
				else
					goto ERROR_HANDLER;
			}
			else
			{
				GetResultLength( request_head );
				me_fsm_data.state = ME_FSM_DONE;
			}
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}
	else if ( ME_FSM_WTRSPNS_4 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			data[datalen] = '\0';

			if ( strstr( data, "+CPBR:") == data )
			{
				ME_MemAssign(data, datalen );
				return 0;
			}
			retval = GetResultErrorCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer( NULL, TimerId );
			GetResultLength( request_head );
			me_fsm_data.state = ME_FSM_DONE;
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}

	return 0;

ERROR_HANDLER:
	request_head->error = ME_RS_FAILURE;
	request_head->errcode = 1;
	GetResultLength( request_head );
	me_fsm_data.state = ME_FSM_DONE;

	return 0;
TIMEOUT_HANDLER:
	request_head->error = ME_RS_TIMEOUT;
	request_head->errcode = 0;
	GetResultLength( request_head );
	me_fsm_data.state = ME_FSM_DONE;

	return 0;
}

int FSM_SetPhoneLock( int event, char* data, int datalen )
{
	char ATCmd[ATCMD_BUFSIZE];
	int retval;
	char* Password;
	int fac;
	BOOL enable;

	if ( ME_FSM_INIT == me_fsm_data.state )
	{
		if ( START == event )
		{
			fac = request_head->arg1;
			enable = request_head->arg2;
			Password = request_head->cmddata;

			strcpy( ATCmd, "AT+CLCK=" );

			switch ( fac )
			{
			case LOCK_PS:

				strcat( ATCmd, "\"PS\"," );

				break;

			case LOCK_SC:

				strcat( ATCmd, "\"SC\"," );

				break;

			case LOCK_PN:

				strcat( ATCmd, "\"PN\"," );

				break;

			case LOCK_PU:

				strcat( ATCmd, "\"PU\"," );

				break;

			case LOCK_PP:

				strcat( ATCmd, "\"PP\"," );

				break;

			case LOCK_PC:

				strcat( ATCmd, "\"PC\"," );

				break;

			case LOCK_FD:

				strcat( ATCmd, "\"FD\"," );

				break;

			default:
				goto ERROR_HANDLER;
			}

			if ( enable )
				strcat( ATCmd, "1" );
			else
				strcat( ATCmd, "0" );

			if ( Password != NULL )
			{
				strcat( ATCmd, ",");
				strcat( ATCmd, "\"" );
				strcat( ATCmd, Password );
				strcat( ATCmd, "\"" );
			}

			strcat( ATCmd, "\r" );

			retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );

			if ( retval != -1 )
			{
				TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
				me_fsm_data.state = ME_FSM_WTRSPNS_1;
			}
			else
				goto ERROR_HANDLER;
		}
	}
	else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			retval = GetResultErrorCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer(NULL, TimerId);
			GetResultLength( request_head );
			me_fsm_data.state = ME_FSM_DONE;
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}

	return 0;

ERROR_HANDLER:
	request_head->error = ME_RS_FAILURE;
	request_head->errcode = 1;
	GetResultLength( request_head );
	me_fsm_data.state = ME_FSM_DONE;

	return 0;
TIMEOUT_HANDLER:
	request_head->error = ME_RS_TIMEOUT;
	request_head->errcode = 0;
	GetResultLength( request_head );
	me_fsm_data.state = ME_FSM_DONE;

	return 0;
}

int FSM_GetPhoneLock( int event, char* data, int datalen )
{
	char ATCmd[ATCMD_BUFSIZE];
	int retval;
	char* Password;
	int fac;

	if ( ME_FSM_INIT == me_fsm_data.state )
	{
		if ( START == event )
		{
			fac = request_head->arg1;
			Password = request_head->cmddata;

			strcpy( ATCmd, "AT+CLCK=" );

			switch ( fac )
			{
			case LOCK_PS:

				strcat( ATCmd, "\"PS\"," );

				break;

			case LOCK_SC:

				strcat( ATCmd, "\"SC\"," );

				break;

			case LOCK_PN:

				strcat( ATCmd, "\"PN\"," );

				break;

			case LOCK_PU:

				strcat( ATCmd, "\"PU\"," );

				break;

			case LOCK_PP:

				strcat( ATCmd, "\"PP\"," );

				break;

			case LOCK_PC:

				strcat( ATCmd, "\"PC\"," );

				break;

			case LOCK_FD:

				strcat( ATCmd, "\"FD\"," );

				break;

			default:
				goto ERROR_HANDLER;
			}

			strcat( ATCmd, "2" );

			if ( Password != NULL )
			{
				strcat( ATCmd, "," );
				strcat( ATCmd, "\"" );
				strcat( ATCmd, Password );
				strcat( ATCmd, "\"" );
			}

			strcat( ATCmd, "\r" );

			retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );

			if ( retval != -1 )
			{
				TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
				me_fsm_data.state = ME_FSM_WTRSPNS_1;
			}
			else
				goto ERROR_HANDLER;
		}
	}
	else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			data[datalen] = '\0';

			if ( strstr( data, "+CLCK:") == data && ME_MemGetOffset() == 0 )
			{
				ME_MemAssign( data, datalen );
				return 0;
			}
			retval = GetResultErrorCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer(NULL, TimerId);
			GetResultLength( request_head );
			me_fsm_data.state = ME_FSM_DONE;
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}

	return 0;

ERROR_HANDLER:
	request_head->error = ME_RS_FAILURE;
	request_head->errcode = 1;
	GetResultLength( request_head );
	me_fsm_data.state = ME_FSM_DONE;

	return 0;
TIMEOUT_HANDLER:
	request_head->error = ME_RS_TIMEOUT;
	request_head->errcode = 0;
	GetResultLength( request_head );
	me_fsm_data.state = ME_FSM_DONE;

	return 0;
}

int FSM_ChangePassword( int event, char* data, int datalen )
{
	char ATCmd[ATCMD_BUFSIZE];
	int retval;
	char *OldPass, *NewPass;
	int fac;

	if ( ME_FSM_INIT == me_fsm_data.state )
	{
		if ( START == event )
		{
			fac = request_head->arg1;

			OldPass = request_head->cmddata;
			NewPass = &OldPass[strlen(OldPass)+1];

			strcpy( ATCmd, "AT+CPWD=" );

			switch ( fac )
			{
			case CALLBAR_BAOC:

				strcat( ATCmd, "\"AO\"," );

				break;

			case CALLBAR_BOIC:

				strcat( ATCmd, "\"OI\"," );

				break;

			case CALLBAR_BOIC_EXHC:

				strcat( ATCmd, "\"OX\"," );

				break;

			case CALLBAR_BAIC:

				strcat( ATCmd, "\"AI\"," );

				break;

			case CALLBAR_BIC_ROAM:

				strcat( ATCmd, "\"IX\"," );

				break;

			case CALLBAR_UAB:

				strcat( ATCmd, "\"AB\"," );

				break;

			case CALLBAR_UAG:

				strcat( ATCmd, "\"AG\"," );

				break;

			case CALLBAR_UAC:

				strcat( ATCmd, "\"AC\"," );

				break;

			case LOCK_PS:

				strcat( ATCmd, "\"PS\"," );

				break;

			case LOCK_SC:

				strcat( ATCmd, "\"SC\"," );

				break;

			case LOCK_PN:

				strcat( ATCmd, "\"PN\"," );

				break;

			case LOCK_PU:

				strcat( ATCmd, "\"PU\"," );

				break;

			case LOCK_PP:

				strcat( ATCmd, "\"PP\"," );

				break;

			case LOCK_PC:

				strcat( ATCmd, "\"PC\"," );

				break;

			case LOCK_P2:

				strcat( ATCmd, "\"P2\"," );

				break;

			default:
				goto ERROR_HANDLER;
			}

			strcat( ATCmd, "\"" );
			strcat( ATCmd, OldPass );
			strcat( ATCmd, "\"" );
			strcat( ATCmd, ",");
			strcat( ATCmd, "\"" );
			strcat( ATCmd, NewPass );
			strcat( ATCmd, "\"" );
			strcat( ATCmd, "\r" );

			retval = MAT_Send( 0, ATCmd, strlen(ATCmd) );

			if ( retval != -1 )
			{
				TimerId = SetTimer( NULL, 0, DFT_TIMEOUT, ME_TimerProc );
				me_fsm_data.state = ME_FSM_WTRSPNS_1;
			}
			else
				goto ERROR_HANDLER;
		}
	}
	else if ( ME_FSM_WTRSPNS_1 == me_fsm_data.state )
	{
		if ( DATAIN == event )
		{
			retval = GetResultErrorCode( data, datalen );

			if ( retval < 0 )
				return retval;

			KillTimer(NULL, TimerId);
			GetResultLength( request_head );
			me_fsm_data.state = ME_FSM_DONE;
		}
		else if ( TIMEOUT == event )
			goto TIMEOUT_HANDLER;
	}

	return 0;

ERROR_HANDLER:
	request_head->error = ME_RS_FAILURE;
	request_head->errcode = 1;
	GetResultLength( request_head );
	me_fsm_data.state = ME_FSM_DONE;

	return 0;
TIMEOUT_HANDLER:
	request_head->error = ME_RS_TIMEOUT;
	request_head->errcode = 0;
	GetResultLength( request_head );
	me_fsm_data.state = ME_FSM_DONE;

	return 0;
}

BOOL IsTwoLineIndicatior( int index )
{
	if ( IndTable.Inds[index].msgclass == ME_URC_CMT )
		return TRUE;

	if ( IndTable.Inds[index].msgclass == ME_URC_CDS )
		return TRUE;

	if ( IndTable.Inds[index].msgclass == ME_URC_CBM )
		return TRUE;

	return FALSE;
}
/*********************************************************************\
* Function	 IsUslctResultCode  
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int IsUslctResultCode( char* buf, unsigned int buflen )
{
	int i;
	char* keyword;
	unsigned int offset = 0;

	if ( buflen == 0 )
		return -1;

	while ( buf[offset] == '\r' || buf[offset] == '\n' )
		offset++;

	if ( offset >= buflen )
		return -1;

	buflen -= offset;

	if ( memcmp( URC_String[ME_URC_SATI], &buf[offset], strlen( URC_String[ME_URC_SATI] ) ) == 0 && STK_Stat == 0 )
		return MAX_QUEUE_INDICATS+1;

	for ( i = 0; i < MAX_QUEUE_INDICATS; i++ )
	{
		keyword = URC_String[IndTable.Inds[i].msgclass];

		if ( IndTable.Inds[i].reg_count == 0 )
			continue;

		if ( buflen < strlen( keyword ) )
			continue;

		if ( memcmp( keyword, &buf[offset], strlen( keyword ) ) == 0 )
			return i;
	}

	return -1;
}

/*********************************************************************\
* Function	 IsFinalResultCode 
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int IsFinalResultCode( char* buf, unsigned int buflen )
{
	int i = 0;
	unsigned int offset = 0;

	if ( buflen == 0 )
		return -1;

	while ( buf[offset] == '\r' || buf[offset] == '\n' )
		offset++;

	if ( offset >= buflen )
		return -1;

	buflen -= offset;

	while ( strlen( FRC_String[i]) > 0 )
	{
		if ( buflen < strlen( FRC_String[i] ) )
		{
			i++;
			continue;
		}

		if ( memcmp( FRC_String[i], &buf[offset], strlen( FRC_String[i]) ) == 0 )
 			return i;

		i++;
	}

	return -1;
}

/*********************************************************************\
* Function	   GetResultErrorCode
* Purpose
* Params
* Return
* Remarks
**********************************************************************/
int GetResultErrorCode( char* data, int datalen )
{
	int retval;

	retval = IsFinalResultCode( data, datalen );

	switch ( retval )
	{
	case FRC_OK:
		request_head->error = ME_RS_SUCCESS;
		request_head->errcode = 0;
		break;
	
	case FRC_ERROR:
		request_head->error = ME_RS_FAILURE;
		request_head->errcode = 1;
		break;

	case FRC_CMEERROR:
		request_head->error = ME_RS_FAILURE;
		if ( data[datalen-1] == '\n' && data[datalen-2] == '\r' )
			data[datalen-2] = '\0';

		request_head->errcode = atoi( &data[strlen(FRC_String[FRC_CMEERROR])] );
		break;

	case FRC_CMSERROR:
		request_head->error = ME_RS_FAILURE;
		if ( data[datalen-1] == '\n' && data[datalen-2] == '\r' )
			data[datalen-2] = '\0';

		request_head->errcode = atoi( &data[strlen(FRC_String[FRC_CMSERROR])] );
		break;

	case FRC_BUSY:
		request_head->error = ME_RS_BUSY;
		request_head->errcode = 0;
		break;

	case FRC_NOANSWER:
		request_head->error = ME_RS_NOANSWER;
		request_head->errcode = 0;
		break;

	case FRC_NODIALTONE:
		request_head->error = ME_RS_NODIALTONE;
		request_head->errcode = 0;
		break;

	case FRC_NOCARRIER:
		request_head->error = ME_RS_NOCARRIER;
		request_head->errcode = 0;
		break;

	case FRC_CONNECT:
		request_head->error = ME_RS_CONNECT;
		request_head->errcode = 0;
		break;
	}

	return retval;
}

void GetResultLength( struct ME_REQUEST* pRequest )
{
	unsigned int len = ME_MemGetOffset();
	char *str, *begin, *end;

	pRequest->result_len = -1;
	pRequest->result = NULL;

	if ( pRequest->error != ME_RS_SUCCESS )
		return;

	if ( pRequest->cmd == ME_INIT_WAVE_EVERYTIME )
	{
		pRequest->result_len = sizeof( ME_INIT_GETVALUE );
		return;
	}

	str = ME_MemGetPntr();

	if ( str == NULL )
		return;

	//去掉字符串开头的换行回车
	while( *str == '\r' || *str == '\n' )
	{
		str++;
	}

	len = strlen( str );

	if ( len == 0 )
		return;

	switch( pRequest->cmd )
	{

	case ME_AT_SEND:

		pRequest->result_len = len;
		pRequest->result = str;

		break;

	case ME_GET_AUTOANSWER:

		pRequest->result_len = sizeof( int );
		pRequest->result = str;

		break;

	case ME_SMS_READ:		//短信读取
	{
		if ( (str = strstr( str, "+CMGR:")) == NULL )
			return;

		pRequest->result_len = sizeof( SMS_INFO );
		pRequest->result = str;

		return;
	}

	case ME_SMS_MEMCOUNT:		//短信存储空间 +CPMS: "SM",3, 10,"SM",3,10
		
	{
		if ( (str = strstr( str, "+CPMS:")) == NULL )
			return;

		pRequest->result_len = sizeof( SMSMEM_INFO );
		pRequest->result = str;

		return;
	}

	case ME_PBOOK_MEMCOUNT:		//电话本存储空间 +CPBS: "SM",10, 20
		
	{
		if ( (str = strstr( str, "+CPBS:")) == NULL )
			return;

		pRequest->result_len = sizeof( PBMEM_INFO );
		pRequest->result = str;

		return;
	}

	case ME_SMS_GETINDSTATUS:

	{
		if ( (str = strstr( str, "+CNMI:")) == NULL )
			return;

		pRequest->result_len = sizeof( IND_STATUS );
		pRequest->result = str;

		return;
	}

	case ME_GET_CELLBROADCAST:

	{
		if ( (str = strstr( str, "+CSCB:")) == NULL )
			return;
		
		pRequest->result_len = sizeof( CELL_BROADCAST );
		pRequest->result = str;

		return;
	}

	case ME_SMS_LIST:		//短信列表读取

	{
		int i = 0;
		if ( (str = strstr( str, "+CMGL:")) == NULL )
			return;

		begin = str;

		while ( begin != NULL )
		{
			i++;
			end = strchr( begin, '\r' );

			if ( end == NULL )
				break;

			begin = strstr( end, "+CMGL:" );
		}

		pRequest->result_len = i*sizeof( SMS_INFO );
		pRequest->result = str;

		return;
	}

	case ME_GET_CALLFWDING:		//呼叫转移查询

	{
		if ( (str = strstr( str, "+CCFC:")) == NULL )
			return;

		pRequest->result_len = sizeof( CALLFWD_INFO );
		pRequest->result = str;

		return;
	}

	case ME_PBOOK_READ:			//电话本读取

	{
		int i = 0;
		if ( (str = strstr( str, "+CPBR:")) == NULL )
			return;

		begin = str;

		while ( begin != NULL )
		{
			i++;
			end = strchr( begin, '\r' );

			if ( end == NULL )
				break;

			begin = strstr( end, "+CPBR:" );
		}

		pRequest->result_len = i*sizeof( PHONEBOOK );
		pRequest->result = str;

		return;
	}

	case ME_GET_CALLBARRING:

	{
		if ( (str = strstr( str, "+CLCK:")) == NULL )
			return;
		
		pRequest->result_len = sizeof( BOOL );
		pRequest->result = str;

		return;
	}

	case ME_PASS_REMAIN:			//PIN码可输入次数查询

	{
		if ( (str = strstr( str, "EXT: I")) == NULL )
			return;

		pRequest->result_len = sizeof( int );
		pRequest->result = str;

		return;
	}

	case ME_GET_LOCK:			//PIN码激活查询

	{
		if ( (str = strstr( str, "+CLCK:")) == NULL )
			return;

		pRequest->result_len = sizeof( BOOL );
		pRequest->result = str;

		return;
	}

	case ME_GET_PASS:

	{
		if ( (str = strstr( str, "+CPIN:")) == NULL )
			return;

		pRequest->result_len = sizeof( int );
		pRequest->result = str;

		return;
	}

	case ME_GET_IMEI:		//获取ME标识
	case ME_GET_IMSI:		//获取ME标识

	{
		end = strchr( str, '\r' );

		if ( end == NULL )
			return;

		pRequest->result_len = ME_MBID_LEN;
		pRequest->result = str;

		return;
	}

	case ME_GET_CALLWAITING:

	{
		if ( (str = strstr( str, "+CCWA:")) == NULL )
			return;
		
		pRequest->result_len = sizeof( BOOL );
		pRequest->result = str;

		return;
	}

	case ME_GET_OPRATOR:		//当前运营商查询

	{
		if ( (str = strstr( str, "+COPS:")) == NULL )
			return;

		pRequest->result_len = sizeof( CUROPRATOR );
		pRequest->result = str;

		return;
	}

	case ME_SRCH_OPRATOR:		//搜索运营商列表

	{
		int i = 0;
		if ( (str = strstr( str, "+COPS:")) == NULL )
			return;

		begin = strchr( str, '(' );

		if ( begin == NULL )
			return;

		while ( begin != NULL )
		{
			i++;

			end = strchr( begin, ')' );

			if ( end == NULL )
				break;

			begin = strchr( end, '(' );
		}

		pRequest->result_len = i*sizeof( NETOPRATOR );
		pRequest->result = str;

		return;
	}

	case ME_GET_POPRATOR:
	{
		int i = 0;

		if ( (str = strstr( str, "+CPOL:")) == NULL )
			return;

		begin = str;

		while ( begin != NULL )
		{
			i++;
			end = strchr( begin, '\r' );

			if ( end == NULL )
				break;

			begin = strstr( end, "+CPOL:" );
		}

		pRequest->result_len = i*sizeof( PREFER_OPRATOR );
		pRequest->result = str;

		return;
	}

	case ME_GET_CLOCK:		//时钟设置

	{
		if ( (str = strstr( str, "#CCLK:")) == NULL )
			return;
		pRequest->result_len = ME_SMS_TIME_LEN;
		pRequest->result = str;

		return;
	}

	case ME_SMS_GET_SCA:			//短信中心号码

	{
		if ( (str = strstr( str, "+CSCA:")) == NULL )
			return;

		pRequest->result_len = ME_PHONENUM_LEN;
		pRequest->result = str;

		return;
	}

	case ME_CURRENT_CALLS:		//当前通话列表

	{
		int i = 0;
		if ( (str = strstr( str, "+CLCC:")) == NULL )
			return;

		begin = str;

		while ( begin != NULL )
		{
			i++;
			end = strchr( begin, '\r' );

			if ( end == NULL )
				break;

			begin = strstr( end, "+CLCC:" );
		}

		pRequest->result_len = i*sizeof( CALLS_INFO );
		pRequest->result = str;

		return;
	}

	case ME_GET_BATTERY:

	{
		if ( (str = strstr( str, "#CBC:")) == NULL )
			return;

		pRequest->result_len = sizeof( BATTERY_INFO );
		pRequest->result = str;

		return;
	}

	case ME_PB_READY:
	{
		if ( (str = strstr( str, "#PHBSTA:")) == NULL )
			return;

		pRequest->result_len = sizeof(int);
		pRequest->result = str;

		return;
	}

	case ME_GET_VOLUME:

	{
		if ( (str = strstr( str, "+CLVL:")) == NULL )
			return;

		pRequest->result_len = sizeof( int );
		pRequest->result = str;

		return;
	}

	case ME_GET_CLIR:

	{
		if ( (str = strstr( str, "+CLIR:")) == NULL )
			return;

		pRequest->result_len = sizeof( CLIR_STATUS );
		pRequest->result = str;

		return;
	}

	case ME_GET_SIGNAL:

	// +CSQ: 24,99
	//	Receive level: (first param)
	//		0			-113 dBm or less
	//		1			-111 dBm
	//		2...30		-109... -53 dBm
	//		31			-51 dBm or greater
	//		99			not known
	//	signal quality range:0~31, 4 steps


	//		0:	0, 99
	//		1:	1~7
	//		2:	8~15
	//		3:	16~23
	//		4:	24~31
	{
		if ( (str = strstr( str, "+CSQ:")) == NULL )
			return;

		pRequest->result_len = sizeof( int );
		pRequest->result = str;

		return;
	}

	case ME_DATA_DIAL:
	case ME_GPRS_DIAL:

	{
		pRequest->result_len = strlen( str );
		pRequest->result = str;

		return;
	}

	}
}

void GetURCLength( struct ME_INDICATION* pInd, char* pdata, int datalen )
{
	pInd->msgdata = pdata;
	pInd->msgdata_len = datalen;

	switch( pInd->msgclass )
	{
	case ME_URC_CBM:
		pInd->msgresult_len = sizeof(SMS_INFO);
		break;

	case ME_URC_CBMI:
		pInd->msgresult_len = sizeof(ME_CMTI_ITEM);
		break;

	case ME_URC_CCWA:
		pInd->msgresult_len = sizeof(ME_PHONE_ITEM);
		break;

	case ME_URC_CDS:
		pInd->msgresult_len = sizeof(SMS_INFO);
		break;

	case ME_URC_CDSI:
		pInd->msgresult_len = sizeof(ME_CMTI_ITEM);
		break;

	case ME_URC_CLIP:
		pInd->msgresult_len = sizeof(ME_PHONE_ITEM);
		break;

	case ME_URC_CMT:
		pInd->msgresult_len = sizeof(SMS_INFO);
		break;

	case ME_URC_CMTI:
		pInd->msgresult_len = sizeof(ME_CMTI_ITEM);
		break;

	case ME_URC_CREG:
		pInd->msgresult_len = sizeof(ME_CREG_URC);
		break;

	case ME_URC_CGREG:
		pInd->msgresult_len = sizeof(ME_CGREG_URC);
		break;

	case ME_URC_CRING:
		pInd->msgresult_len = sizeof(int);
		break;

	case ME_URC_CSSU:
		pInd->msgresult_len = sizeof(ME_CSSU_ITEM);
		break;

	case ME_URC_SATI:
	case ME_URC_SATN:
		pInd->msgresult_len = datalen;
		break;
	default:
		pInd->msgresult_len = -1;
		break;
	}
}

int FillURCField( void* result, int result_len, struct ME_INDICATION* pInd )
{
	char data[600];

	memcpy( data, pInd->msgdata, pInd->msgdata_len );
	data[pInd->msgdata_len] = '\0';

	switch( pInd->msgclass )
	{
	case ME_URC_CBM:
		return AnaURC_CBM( data, result );

	case ME_URC_CBMI:
		return AnaURC_CMTI( data, result );

	case ME_URC_CCWA:
		return AnaURC_CCWA( data, result );

	case ME_URC_CDS:
		return AnaURC_CDS( data, result );

	case ME_URC_CDSI:
		return AnaURC_CMTI( data, result );

	case ME_URC_CLIP:
		return AnaURC_CLIP( data, result );

	case ME_URC_CMT:
		return AnaURC_CMT( data, result );		

	case ME_URC_CMTI:
		return AnaURC_CMTI( data, result );

	case ME_URC_CREG:
		return AnaURC_CREG( data, result );

	case ME_URC_CGREG:
		return AnaURC_CGREG( data, result );

	case ME_URC_CRING:
		return AnaURC_CRING( data, result );

	case ME_URC_CSSU:
		return AnaURC_CSSU( data, result );

	case ME_URC_SATI:
	case ME_URC_SATN:
		return AnaURC_STK( data, result );

	}

	memcpy( result, pInd->msgdata, pInd->msgdata_len );
	
	return pInd->msgdata_len;
}

int FillResultField( void* result, int result_len, struct ME_REQUEST* pRequest )
{
	int len;
	char *str, *begin, *end;

	str = pRequest->result;
	len = pRequest->result_len;

	switch( pRequest->cmd )
	{
		case ME_INIT_WAVE_EVERYTIME:
		{
			len = Me_GetInitValue( (PME_INIT_GETVALUE)result );
			return len;
		}
		break;

		case ME_AT_SEND:
		{
			if ( result_len < len )
			{
				memcpy( result, str, result_len );
				return result_len;
			}
			else
			{
				memcpy( result, str, len );
				return len;
			}
		}

		case ME_GET_AUTOANSWER:
		{
			int ret;
			ret = atoi( str );
			memcpy( result, &ret, sizeof(int) );

			return len;
		}

		break;

		case ME_SMS_READ:		//短信读取

		{
			memset( result, 0, result_len );

			((PSMS_INFO)result)->Index = pRequest->arg1;

			end = strchr( str, '\r' );

			if ( end == NULL )
				return -1;

			*end = '\0';

			if ( ME_RecStr_Flag( (PSMS_INFO)result, str ) == -1 )
				return -1;

			begin = end + 2;
			end = strchr( begin, '\r' );

			if ( end == NULL )
				return -1;

			*end = '\0';

			ME_RecStr_Code( (PSMS_INFO)result, begin, ((PSMS_INFO)result)->Stat);

			if ( ((PSMS_INFO)result)->ConLen == -1 )
				return -1;

			return len;
		}

		case ME_SMS_MEMCOUNT:		//短信存储空间 +CPMS: "SM",3, 10,"SM",3,10

		{
			memset( result, 0, result_len );

			((PSMSMEM_INFO)result)->nMem1Used = ME_GetIntSection( str, "+CPMS:",  1 );
			((PSMSMEM_INFO)result)->nMem1Total = ME_GetIntSection( str, "+CPMS:", 2 );
			((PSMSMEM_INFO)result)->nMem2Used = ME_GetIntSection( str, "+CPMS:",  3 );
			((PSMSMEM_INFO)result)->nMem2Total = ME_GetIntSection( str, "+CPMS:", 4 );
			((PSMSMEM_INFO)result)->nMem3Used = ME_GetIntSection( str, "+CPMS:",  5 );
			((PSMSMEM_INFO)result)->nMem3Total = ME_GetIntSection( str, "+CPMS:", 6 );

			return len;
		}

		case ME_PBOOK_MEMCOUNT:		//电话本存储空间 +CPBS: "SM",10, 20

		{
			char memtype[3];
			memset( result, 0, result_len );

			((PPBMEM_INFO)result)->Used = ME_GetIntSection( str, "+CPBS:", 1 );
			((PPBMEM_INFO)result)->Total = ME_GetIntSection( str, "+CPBS:", 2 );

			ME_GetStrSection( str, memtype, 3, "+CPBS:", 1);

			if (memcmp( memtype, "AD", 2 ) == 0)
				((PPBMEM_INFO)result)->Type = PBOOK_SM;
			else if (memcmp( memtype, "FD", 2 ) == 0)
				((PPBMEM_INFO)result)->Type = PBOOK_FD;
			else if (memcmp( memtype, "LD", 2 ) == 0)
				((PPBMEM_INFO)result)->Type = PBOOK_LD;
			else if (memcmp( memtype, "LM", 2 ) == 0)
				((PPBMEM_INFO)result)->Type = PBOOK_MC;
			else if (memcmp( memtype, "LR", 2 ) == 0)
				((PPBMEM_INFO)result)->Type = PBOOK_RC;
			else
				return -1;

			return len;
		}

		case ME_SMS_GETINDSTATUS:

		{
			PIND_STATUS pStatus;
			
			pStatus = (PIND_STATUS)result;

			pStatus->Mt = ME_GetIntSection( str, "+CNMI:", 2 );
			if ( pStatus->Mt == -1 )
				return -1;

			pStatus->Bm = ME_GetIntSection( str, "+CNMI:", 3 );
			if ( pStatus->Bm == -1 )
				return -1;

			pStatus->Ds = ME_GetIntSection( str, "+CNMI:", 4 );
			if ( pStatus->Ds == -1 )
				return -1;

			if ( pStatus->Mt == 0 )
				pStatus->Mt = IND_NONE;
			else if ( pStatus->Mt == 1 )
				pStatus->Mt = IND_BUFFER;
			else
				pStatus->Mt = IND_DIRECT;

			if ( pStatus->Bm == 0 )
				pStatus->Bm = IND_NONE;
			else if ( pStatus->Bm == 1 )
				pStatus->Bm = IND_BUFFER;
			else
				pStatus->Bm = IND_DIRECT;

			if ( pStatus->Ds == 0 )
				pStatus->Ds = IND_NONE;
			else if ( pStatus->Mt == 1 )
				pStatus->Ds = IND_DIRECT;
			else
				pStatus->Ds = IND_BUFFER;

			return len;
		}

		case ME_GET_CELLBROADCAST:

		{
			PCELL_BROADCAST pcb;
		
			pcb = (PCELL_BROADCAST)result;

			if ( ME_GetIntSection( str, "+CSCB:", 1 ) == 1 )
				pcb->mode = FALSE;
			else
				pcb->mode = TRUE;

			ME_GetStrSection( str, pcb->ids, 40, "+CSCB:", 1 );
			ME_GetStrSection( str, pcb->code, 20, "+CSCB:", 2 );

			return len;
		}

		case ME_SMS_LIST:		//短信列表读取

		{
			int retval;

			memset( result, 0, result_len );
			retval = ME_RecLstStr_Code( result, str, result_len/sizeof(SMS_INFO) );

			return retval*sizeof(SMS_INFO);
		}

		case ME_GET_CALLFWDING:		//呼叫转移查询

		{
			int ret;

			memset( result, 0, result_len );

			end = strchr( str, '\r' );
			if ( end == NULL )
				return -1;

			*end = '\0';

			ret = ME_GetIntSection( str, "+CCFC:", 1 );

			if ( ret == -1 )
				return -1;

			if ( ret == 1 )
			{
				((PCALLFWD_INFO)result)->Enable = TRUE;

				ME_GetStrSection( str, ((PCALLFWD_INFO)result)->PhoneNum, ME_PHONENUM_LEN, "+CCFC:", 1 );
			}
			else
				((PCALLFWD_INFO)result)->Enable = FALSE;

			((PCALLFWD_INFO)result)->Time = ME_GetIntSection( str, "+CCFC:", 4 );

			return len;
		}

		case ME_PBOOK_READ:			//电话本读取

		{
			unsigned int i;

			end = str-2;

			for ( i=0; i < result_len/sizeof(PHONEBOOK); i++ )
			{
				begin = end + 2;

				while ( *begin == '\r' )
					begin += 2;

				if ( strlen (begin) == 0 )
					return i*sizeof(PHONEBOOK);

				end = strchr( begin, '\r' );

				if (end == NULL)
					return i*sizeof(PHONEBOOK);
		
				*end = '\0';

				ME_PhoneBook_Dec( begin, (PPHONEBOOK)((char*)result+i*sizeof(PHONEBOOK)) );
			}

			return i*sizeof(PHONEBOOK);
		}

		case ME_GET_CALLBARRING:

		{
			BOOL ret;

			ret = ME_GetIntSection( str, "+CLCK:", 1 );

			if ( ret == -1 )
				return -1;

			memcpy( result, &ret, sizeof( BOOL ) );

			return len;
		}

		case ME_PASS_REMAIN:			//PIN码可输入次数查询

		{
			int ret = -1;

			memset( result, 0, result_len );

			begin = strchr( str, '\n' );

			if ( begin == NULL )
				return -1;

			begin++;
			while ( *begin == '\r' || *begin == '\n' )
				begin++;

			if ( begin == NULL )
				return -1;

			end = strchr( begin, '\r' );

			if ( end == NULL )
				return -1;

			if ( request_head->arg1 == PIN1 )
				ret = ME_GetIntSection( begin, "", 1 );
			if ( request_head->arg1 == PIN2 )
				ret = ME_GetIntSection( begin, "", 2 );
			if ( request_head->arg1 == PUK1 )
				ret = ME_GetIntSection( begin, "", 3 );
			if ( request_head->arg1 == PUK2 )
				ret = ME_GetIntSection( begin, "", 4 );

			if ( ret == -1 )
				return -1;

			memcpy( result, &ret, sizeof( int ) );

			return len;
		}

		case ME_GET_LOCK:			//PIN码激活查询

		{
			int ret;

			ret = ME_GetIntSection( str, "+CLCK:", 1 );

			if ( ret == -1 )
				return -1;

			memcpy( result, &ret, sizeof( int ) );

			return len;
		}

		case ME_GET_PASS:

		{
			int ret;

			if ( strstr( str, "READY") != NULL )
				ret = NOPIN;
			else if ( strstr( str, "SIM PIN2" ) != NULL )
				ret = PIN2;
			else if ( strstr( str, "SIM PUK2" ) != NULL )
				ret = PUK2;
			else if ( strstr( str, "SIM PIN" ) != NULL )
				ret = PIN1;
			else if ( strstr( str, "SIM PUK" ) != NULL )
				ret = PUK1;
			else if ( strstr( str, "PH-SIM PIN" ) != NULL )
				ret = SIMPHLOCK;
			else if ( strstr( str, "PH-NET PIN" ) != NULL )
				ret = SIMNTLOCK;
			else
				return -1;

			memcpy( result, &ret, sizeof( int ) );

			return len;
		}

		case ME_GET_IMEI:		//获取ME标识

		{
			char imei[16];
			int i;	
			
			for (i=0; i<sizeof(imei); i+=2)
			{
				imei[i] = (str[i/2] & 0x0F)+0x30;
				imei[i+1] = ((str[i/2] >> 4) & 0x0F)+0x30;
			}
			imei[15] = '\0';

			strcpy (result, str);

			return strlen(imei);
		}

		case ME_GET_IMSI:

		{
			end = strchr( str, '\r' );

			if ( end == NULL )
				return -1;

			*end = '\0';

			memcpy( result, str, len );

			return len;
		}

		case ME_GET_CALLWAITING:

		{
			BOOL bEnable;

			if ( ME_GetIntSection( str, "+CCWA:", 1 ) == 1 )
				bEnable = TRUE;
			else
				bEnable = FALSE;

			memcpy( result, &bEnable, sizeof(BOOL) );

			return len;
		}

		case ME_GET_OPRATOR:		//当前运营商查询

		{
			((PCUROPRATOR)result)->Mode = ME_GetIntSection( str, "+COPS:", 1 );
			((PCUROPRATOR)result)->Format = ME_GetIntSection( str, "+COPS:", 2 );

			if ( ((PCUROPRATOR)result)->Mode == -1 || ((PCUROPRATOR)result)->Format == -1 )
				return -1;

			ME_GetStrSection( str, ((PCUROPRATOR)result)->Name, ME_OPRATOR_LEN, "COPS:", 1 );
			return len;
		}

		case ME_SRCH_OPRATOR:		//搜索运营商列表

		{
			int retval;

			memset( result, 0, result_len );

			retval = ME_NetOpratorDec( result, str, result_len/sizeof(NETOPRATOR) );

			return retval*sizeof(NETOPRATOR);
		}

		case ME_GET_POPRATOR:

		{
			int count;
			PPREFER_OPRATOR po;

			count = 0;
			po = result;
			begin = str;

			while ( (end = strstr( begin, "\r\n" )) != NULL )
			{
				*end = '\0';

				po[count].index = ME_GetIntSection( begin, "+CPOL:", 1 );

				ME_GetStrSection( begin, po[count].oper_id, ME_OPRATOR_LEN, "+CPOL:", 1 );

				count++;

				if ( count*sizeof(PREFER_OPRATOR) >= (unsigned int)result_len )
					break;

				begin = end+2;

				while ( *begin == '\r' )
					begin += 2;

				if ( begin == NULL )
					break;
			}

			return count*sizeof( PREFER_OPRATOR );
		}

		case ME_GET_CLOCK:		//时钟设置

		{
			str += strlen ("#CCLK:");
			while (*str == ' ' )
				str ++;

			memcpy (result, str, strlen(str));
            return strlen(str);
		}

		case ME_SMS_GET_SCA:			//短信中心号码

		{
			int retval;
			int type;

			memset( result, 0, result_len );

			type = ME_GetIntSection( str, "+CSCA:", 1 );

			if ( type == 145 )
			{
				*((char*)result) = '+';
				retval = ME_GetStrSection( str, (char*)(result)+1, result_len-1, "+CSCA:", 1 );
				retval += 1;
			}
			else
				retval = ME_GetStrSection( str, (char*)result, result_len, "+CSCA:", 1 );

			if ( retval <= 0 )
				return -1;

			return retval;
		}

		case ME_CURRENT_CALLS:		//当前通话列表

		{
			unsigned int i;

			memset( result, 0, result_len );

			end = str-2;

			for ( i = 0; i < result_len/sizeof(CALLS_INFO); i++ )
			{
				begin = end + 2;

				while ( *begin == '\r' )
					begin += 2;

				if ( strlen (begin) == 0 )
					return i*sizeof(CALLS_INFO);

				end = strchr( begin, '\r' );

				if ( end == NULL || end == begin )
					return i*sizeof(CALLS_INFO);
		
				*end = '\0';

				ME_CurCalls_Dec( begin, (PCALLS_INFO)((char*)result+i*sizeof(CALLS_INFO)) );
			}

			return i* sizeof( CALLS_INFO );
		}

		case ME_GET_VOLUME:

		{
			int level;

			level = ME_GetIntSection( str, "+CLVL:", 1 );

			if ( level == -1 )
				return -1;

			memcpy( result, &level, sizeof(int) );

			return sizeof(int);
		}

		case ME_GET_CLIR:

		{
			int status;

			status = ME_GetIntSection( str, "+CLIR:", 2 );

			if ( status == -1 )
				return -1;

			switch ( status ) {
			case 0:
				status = CLIR_NPROVIS;
				break;
			case 1:
				status = CLIR_PERMANENT;
				break;
			case 2:
				status = CLIR_UNKNOWN;
				break;
			case 3:
				status = CLIR_TEMP_RES;
				break;
			case 4:
				status = CLIR_TEMP_ALLOW;
				break;
			}

			memcpy( result, &status, sizeof(CLIR_STATUS) );

			return sizeof(CLIR_STATUS);
		}

		case ME_GET_BATTERY:

		{
			int level;

			level = ME_GetIntSection( str, "#CBC:", 1 );

			if ( level == -1 )
				return -1;

//			level = (level-3400)*100/(4300-3400);
			if ( level > 100 )
				level = 100;
			if ( level < 0 )
				level = 0;

			if ( level < BC_SHUTDOWN )
				((PBATTERY_INFO)result)->BCNotify = ME_BATTERY_SHUTDOWN;
			else if ( level < BC_WARNING )
				((PBATTERY_INFO)result)->BCNotify = ME_BATTERY_SHUTDOWN;
			else
				((PBATTERY_INFO)result)->BCNotify = ME_BATTERY_NORMAL;

			((PBATTERY_INFO)result)->BCValue = level;

			return len;
		}

		case ME_PB_READY:
		{
			str += strlen ("#PHBSTA:");
			if ( strstr (str, "OK") != NULL )
				*((int*)result) = 1;
			else
				*((int*)result) = 0;
			return sizeof(int);
		}

		case ME_GET_SIGNAL:

		// +CSQ: 24,99
		//	Receive level: (first param)
		//		0			-113 dBm or less
		//		1			-111 dBm
		//		2...30		-109... -53 dBm
		//		31			-51 dBm or greater
		//		99			not known
		//	signal quality range:0~31, 4 steps


		//		0:	0, 99
		//		1:	1~7
		//		2:	8~15
		//		3:	16~23
		//		4:	24~31
		{
			int value;

			value = ME_GetIntSection( str, "+CSQ:", 1 );

			if ( value == -1 )
				return -1;

			if ( value == 99 || value == 0 )
				value = 0;
			else
				value = value/8 + 1;

			*((int*)result) = value;

			return len;
		}

		case ME_DATA_DIAL:
		case ME_GPRS_DIAL:

		{
			if ( strlen( str ) > (unsigned int)result_len )
			{
				memcpy( result, str, result_len );
				return result_len;
			}
			else
			{
				strcpy( result, str );
				return len;
			}
		}

		default:

			return -1;
	}
}

/*---------------------------------------------------
 *	+CRING: <type>
 *	"VOICE"		for normal speech
 *	"FAX"		for fax calls
 *	"ASYNC"		for asynchronous transparent
 *	"REL ASYNC"	for asynchronous non-transparent
 *---------------------------------------------------*/
int AnaURC_CRING( char *SrcStr, int* Type )
{
	char *pCRINGStr = SrcStr;

	if ( memcmp( pCRINGStr, "+CRING:", 7 ) != 0 )
		return -1;

	if( strcmp( pCRINGStr, "VOICE" ) ==0 )
		*Type = ME_CRING_VOICE;

	else if( strcmp( pCRINGStr, "FAX" ) ==0 )
		*Type = ME_CRING_FAX;

	else if( strcmp( pCRINGStr, "ASYNC" ) ==0 )
		*Type = ME_CRING_ASYNC;

	else if( strcmp( pCRINGStr, "REL ASYNC" ) ==0 )
		*Type = ME_CRING_REL_ASYNC;

	else
		return -1;

	return sizeof(int);
}

/*-------------------------------------------------------------
 *	+CLIP: <number>, <type>[ ,<subaddr>, <satype>, <alpha> ]
 *	+CLIP: "13910218504",161,,,"TEST2"
 *	+CLIP: "13910218504",161
 *
 *----------------------------------------------------------*/
int	AnaURC_CLIP( char *SrcStr,  PME_PHONE_ITEM pItem )
{
	int len;
	int type;
	char PhoneNum[ME_PHONENUM_LEN], PhoneStr[ME_PHONETXT_LEN];
	char PhoneStr1[ME_PHONETXT_LEN];
	char *pClIPStr, *pBegin, *pEnd;

	pClIPStr = SrcStr;

	if ( memcmp( pClIPStr, "+CLIP:", 6 ) != 0 )
		return -1;

	// Analyse number
	if( (pBegin = strstr( pClIPStr, "\"" )) == NULL )
		return -1;

	if( (pEnd = strstr( pBegin+1, "\"" )) == NULL )
		return -1;

	len = pEnd - pBegin;

	type = ME_GetIntSection( pClIPStr, "+CLIP:", 1 );

	if ( type == 145 )
	{
		*PhoneNum = '+';
		memcpy( PhoneNum+1, pBegin+1, len-1 );
		PhoneNum[len]=0;
	}
	else
	{
		memcpy( PhoneNum, pBegin+1, len-1 );
		PhoneNum[len-1]=0;
	}

	// Analyse alpha
	if( (pBegin = strstr( pEnd+1, "\"" )) == NULL )
	{
		PhoneStr[0]=0;
		goto monitor_clip_report;
	}

	if( (pEnd = strstr( pBegin+1, "\"" )) == NULL )
	{
		PhoneStr[0]=0;
		goto monitor_clip_report;
	}

	len = pEnd - pBegin;
	memcpy( PhoneStr, pBegin+1, len-1 );
	PhoneStr[len-1]=0;

	PhoneText_Decode( PhoneStr1,  ME_PHONETXT_LEN, PhoneStr );

monitor_clip_report:
	strcpy( pItem->PhoneNum, PhoneNum );
	strcpy( pItem->PhoneStr, PhoneStr1 );

	return sizeof(ME_PHONE_ITEM);
}


/*-------------------------------------------------------------
 *	+CCWA: "13910218504",161,1,"TEST2"
 *	+CCWA: "13910218504",161,1
 *	+CCWA:,,1
 *
 *----------------------------------------------------------*/
int	AnaURC_CCWA( char * SrcStr, PME_PHONE_ITEM pItem )
{
	int len;
	int type;
	char PhoneNum[ME_PHONENUM_LEN], PhoneStr[ME_PHONETXT_LEN];
	char PhoneStr1[ME_PHONETXT_LEN];
	char *pCCWAStr, *pBegin, *pEnd;

	pCCWAStr = SrcStr;

	if ( memcmp( pCCWAStr, "+CCWA:", 6 ) != 0 )
		return -1;
	// Analyse number
	if( (pBegin = strstr( pCCWAStr, "\"" )) == NULL )
	{
		PhoneNum[0]=0;
		PhoneStr[0]=0;
		goto monitor_ccwa_report;
	}

	if( (pEnd = strstr( pBegin+1, "\"" )) == NULL )
		return -1;

	len = pEnd - pBegin;

	type = ME_GetIntSection( pCCWAStr, "+CCWA:", 1 );

	if ( type == 145 )
	{
		*PhoneNum = '+';
		memcpy( PhoneNum+1, pBegin+1, len-1 );
		PhoneNum[len]=0;
	}
	else
	{
		memcpy( PhoneNum, pBegin+1, len-1 );
		PhoneNum[len-1]=0;
	}


	// Analyse alpha
	if( (pBegin = strstr( pEnd+1, "\"" )) == NULL )
	{
		PhoneStr[0]=0;
		goto monitor_ccwa_report;
	}

	if( (pEnd = strstr( pBegin+1, "\"" )) == NULL )
	{
		PhoneStr[0]=0;
		goto monitor_ccwa_report;
	}

	len = pEnd - pBegin;
	memcpy( PhoneStr, pBegin+1, len-1 );
	PhoneStr[len-1]=0;

	PhoneText_Decode( PhoneStr1,  ME_PHONETXT_LEN, PhoneStr );

monitor_ccwa_report:
	strcpy( pItem->PhoneNum, PhoneNum );
	strcpy( pItem->PhoneStr, PhoneStr1 );

	return sizeof(ME_PHONE_ITEM);
}

/*-------------------------------------------------------------
 *	+CSSU:<code2>[,<index>[,<number>,<type>]]
 *	+CSSU: 2,,"13910218504",161
 *	+CSSU: 3,,"13910218504",161
 *	+CSSU: 133
 *
 *----------------------------------------------------------*/
int	AnaURC_CSSU( char *SrcStr, PME_CSSU_ITEM pItem )
{
	int		Index, i, len;
	char	*pCSSUStr, *pBegin, *pEnd;
	char	Code2[8], PhoneNum[ME_PHONENUM_LEN];

	pCSSUStr = SrcStr;

	if ( memcmp( pCSSUStr, "+CSSU:", 6 ) != 0 )
		return -1;

	// Analyse number
	if( (pBegin = strstr( pCSSUStr, "\"" )) == NULL )
	{
		PhoneNum[0]=0;
		goto monitor_cssu_report;
	}

	if( (pEnd = strstr( pBegin+1, "\"" )) == NULL )
		return -1;

	len = pEnd - pBegin;
	memcpy( PhoneNum, pBegin+1, len-1 );
	PhoneNum[len-1]=0;


monitor_cssu_report:

	// Analyse Code2
	for( i=0; ; i++ )
	{
		pCSSUStr += strlen("+CSSU:" );
		if( *(pCSSUStr+i) == ',' || *(pCSSUStr+i) == 0 )
			break;
		Code2[i] = *(pCSSUStr+i);
	}
	Code2[i]=0;
	Index = atoi( Code2 );

	if( Index != 2 && Index != 3 && Index != 133 )
		return -1;

	pItem->Action = Index;
	strcpy( pItem->PhoneNum, PhoneNum );

	return sizeof(ME_CSSU_ITEM);;
}


/*-------------------------------------------------------------
 *	+CMTI:<mem>,<index>
 *	+CMTI: "MT",3
 *
 *----------------------------------------------------------*/
int	AnaURC_CMTI( char *SrcStr, PME_CMTI_ITEM pItem )
{
	char *CMTIStr, *p;

	CMTIStr = SrcStr;

	if ( memcmp( CMTIStr, "+CMTI:", 6 ) != 0
		&& memcmp( CMTIStr, "+CBMI:", 6 ) != 0
		&& memcmp( CMTIStr, "+CSDI:", 6 ) != 0 )
		return -1;

	if( (p = strstr( CMTIStr, "\"SM\"" )) != NULL )
		pItem->DevType = SMS_MEM_SM;

	else if( (p = strstr( CMTIStr, "\"ME\"" )) != NULL )
		pItem->DevType = SMS_MEM_ME;

	else
		return -1;

	p += strlen( "\"SM\"") +1;	// skip"<mem>,", point to <index>
	pItem->Index = atoi( p );

	return sizeof(ME_CMTI_ITEM);
}

/*-------------------------------------------------------------
 *	+CREG:<mode>,<stat>
 *	
 *
 *----------------------------------------------------------*/
int	AnaURC_CREG( char *SrcStr, PME_CREG_URC pItem )
{
	char *CREGStr;

	CREGStr = SrcStr;

	if ( memcmp( CREGStr, "+CREG:", 6 ) != 0 )
		return -1;

	pItem->Stat = ME_GetIntSection( CREGStr, "+CREG:", 1 );
	ME_GetStrSection( CREGStr, pItem->lac, 6, "CREG:", 1 );
	ME_GetStrSection( CREGStr, pItem->ci, 6, "CREG:", 2 );

	return sizeof(ME_CREG_URC);
}

/*-------------------------------------------------------------
 *	+CGREG:<stat>
 *
 *
 *----------------------------------------------------------*/
int	AnaURC_CGREG( char *SrcStr, PME_CGREG_URC pItem )
{
	char *CREGStr;

	CREGStr = SrcStr;

	if ( memcmp( CREGStr, "+CGREG:", 7 ) != 0 )
		return -1;

	pItem->Stat = ME_GetIntSection( CREGStr, "+CGREG:", 1 );
	ME_GetStrSection( CREGStr, pItem->lac, 6, "CGREG:", 1 );
	ME_GetStrSection( CREGStr, pItem->ci, 6, "CGREG:", 2 );

	return sizeof(ME_CGREG_URC);
}

/*-------------------------------------------------------------
 *	+STIN: <CmdType>
 *	+STIN: 6
 *
 *----------------------------------------------------------*/
int	AnaURC_STK( char *SrcStr, char* result )
{
	char *STKStr;
	int i;

	STKStr = SrcStr;

	if ( memcmp( STKStr, "%SATI: ", 7 ) != 0 && memcmp( STKStr, "%SATN: ", 7 ) != 0)
		return -1;

	STKStr += 7;
	
	for ( i = strlen(STKStr)-1; i >= 0; i-- )
	{
		if ( STKStr[i] == '\r' || STKStr[i] == '\n' )
			STKStr[i] = '\0';
		else
			break;
	}
	if ( strlen(STKStr) > 0 )
		memcpy( result, STKStr, strlen(STKStr) );

	return strlen( STKStr );
}

int AnaURC_CMT( char* SrcStr, PSMS_INFO info )
{
	char* CMTStr = SrcStr, *begin, *end;
	if ( memcmp( CMTStr, "+CMT:", 5 ) != 0 )
		return -1;

	end = strchr( CMTStr, '\r' );

	if ( end == NULL )
		return -1;

	begin = end + 2;
	if ( begin == NULL )
		return -1;

	while ( *begin == '\r' || *begin == '\n' )
		begin++;

	if ( begin == NULL )
		return -1;

	end = strchr( begin, '\r' );

	if ( end == NULL )
		return -1;

	*end = '\0';

	ME_RecStr_Code( info, begin, SMS_UNREAD );

	if ( info->ConLen == -1 )
		return -1;

	return sizeof(SMS_INFO);
}

int AnaURC_CBM( char* SrcStr, PSMS_INFO info )
{
	char* CBMStr = SrcStr, *begin, *end;
	if ( memcmp( CBMStr, "+CBM:", 5 ) != 0 )
		return -1;

	end = strchr( CBMStr, '\r' );

	if ( end == NULL )
		return -1;

	begin = end + 2;
	if ( begin == NULL )
		return -1;

	end = strchr( begin, '\r' );

	if ( end == NULL )
		return -1;

	*end = '\0';

	ME_RecStr_Code( info, begin, SMS_UNREAD );

	if ( info->ConLen == -1 )
		return -1;

	return sizeof(SMS_INFO);
}

int AnaURC_CDS( char* SrcStr, PSMS_INFO info )
{
	char* CDSStr = SrcStr, *begin, *end;
	if ( memcmp( CDSStr, "+CDS:", 5 ) != 0 )
		return -1;

	end = strchr( CDSStr, '\r' );

	if ( end == NULL )
		return -1;

	begin = end + 2;
	if ( begin == NULL )
		return -1;

	while ( *begin == '\r' || *begin == '\n' )
		begin++;

	if ( begin == NULL )
		return -1;

	end = strchr( begin, '\r' );

	if ( end == NULL )
		return -1;

	*end = '\0';

	ME_RecStr_Code( info, begin, SMS_UNREAD );

	if ( info->ConLen == -1 )
		return -1;

	return sizeof(SMS_INFO);
}

static int MakeSTKMenuResponse (char * dst, char * src)
{
    int l;
    char * src1 = malloc (strlen(src)/2), *ptr1, *ptr2, t, dst1[MAX_RESPLEN];
    const char end[] = "830100";
    const char prefix[] = "AT%SATR=";

    String2Bytes (src1, src, strlen(src));
    ptr1 = &src1[2];
    ptr2 = dst1;

    l = (int)(*(ptr1+1));
    memcpy (ptr2, ptr1, l+2);
    ptr1 += l+2;
    ptr2 += l+2;
    
    l = (int)(*(ptr1+1));
    memcpy (ptr2, ptr1, l+2);

    t = ptr2[l];
    ptr2[l] = ptr2[l+1];
    ptr2[l+1]= t;

    ptr2 += l+2;

    free (src1);

    strcpy (dst, prefix);
    l = Bytes2String ( &dst[strlen(prefix)], dst1, ptr2-dst1);
    l += strlen(prefix);

    memcpy (&dst[l], end, strlen(end));
    dst[l+strlen(end)] = '\r';
    dst[l+strlen(end)+1] = '\0';

    return l+strlen(end)+1;
}

static int String2Bytes (char* dst, const char* src, int srclen)
{
    unsigned char b;
	int i;

	for ( i = 0; i < srclen; i += 2 ) {
		if ( *src >= '0' && *src <= '9' )
			b = (*src - '0') << 4;
		else
			b = (*src - 'A' + 10) << 4;

		src++;
		if( *src >= '0' && *src <= '9' )
			b |= *src - '0';
		else
			b |= *src - 'A' + 10;
		src++;
		*dst++ = b;
	}
	return srclen/2;
}

static int Bytes2String (char* dst, const char* src, int srclen)
{
	static const char tab[] = "0123456789ABCDEF";
    unsigned char b;
	int i;

	for ( i = 0; i < srclen; i++ ) {
        b = *src ++;
		*dst++ = tab[(b >> 4) & 0x0f];
		*dst++ = tab[b & 0x0f];
	}
	return srclen * 2;
}
