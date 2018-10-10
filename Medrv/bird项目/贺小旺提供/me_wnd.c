/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Mobile Engine
 *
 * Purpose  : 
 *
\**************************************************************************/
#include "string.h"

#include "window.h"
#include "me_func.h"
#include "me_wnd.h"
#include "me_at.h"


static struct ME_REQUEST_WIN * FindRequestNode (int idx);
static struct ME_REQUEST_WIN * GetRequestNode (void);
static int gInit;
struct ME_REQUEST_WIN	request_buf[MAX_QUEUE_REQUESTS];
struct ME_REQUEST_WIN * gUssdObj;
extern struct ME_INDTABLE IndTable;

static struct ME_REQUEST_WIN * GetRequestNode (void)
{
    int i;
    for ( i = 0; i < MAX_QUEUE_REQUESTS; i ++ )
    {
        if ( request_buf[i].used_flag == 0 ) {
            memset (&request_buf[i], 0, sizeof(struct ME_REQUEST_WIN));
            request_buf[i].request.index = -1;
            request_buf[i].used_flag = 1;
            return &request_buf[i];
        }
    }
    return NULL;
}

static struct ME_REQUEST_WIN * FindRequestNode (int idx)
{
    int i;

    if ( idx < 0 )
        return NULL;
    
    for ( i = 0; i < MAX_QUEUE_REQUESTS; i ++ ) {
        if ( request_buf[i].used_flag == 0 )
            continue;

        if ( request_buf[i].request.index == idx )
            return &request_buf[i];
    }

    return NULL;
}

void ME_WndCallback (MEREQUEST * pRequest)
{
    struct ME_REQUEST_WIN * request = FindRequestNode (pRequest->index);

	if ( request == NULL)
		return;

    SendMessage (request->hWnd, request->message, (WORD)pRequest->error, (DWORD)pRequest->errcode);

    /* Free this request structure */
    request->used_flag = 0;
}

void ME_UssdCallback (MEREQUEST * pRequest)
{
    if ( gUssdObj == NULL )
        return;

    SendMessage (gUssdObj->hWnd, gUssdObj->message, (DWORD)gUssdObj->request.error, (DWORD)gUssdObj);
}

void ME_URCCallback (struct ME_REGISTER * pRegister, unsigned long param1, unsigned long param2)
{
    SendMessage (pRegister->hwnd, pRegister->msg, param1, param2);
}

int ME_Initialize (int Mode)
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;
    
    if ( gInit )
        return gInit;

    request = GetRequestNode ();

    if ( request == NULL )
        return -1;

	request->request.cmd = ME_INIT;

	request->request.arg1 = Mode;

	retval = ME_Request( &request->request );

	request->used_flag = 0;

    request = GetRequestNode ();
    retval = USSD_Init (&(request->request));

    if ( retval < 0 )
        return retval;

    request->request.reply = ME_UssdCallback;
    gUssdObj = request;

    gInit = TRUE;

	return retval;
}

int ME_GSMInit_Module_3( HWND hWnd, UINT Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_INIT_EVERYTIME;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GSMInit_Module( HWND hWnd, UINT Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_INIT_EQUIP;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GSMInit_Module_2( HWND hWnd, UINT Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_RESET_EQUIP;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}


int ME_RegisterIndication( int MsgClass, int Mode, HWND hwnd, UINT msg )
{
	int ret, id1, id2;

	if ( hwnd == NULL )
		return -1;

	ret = ME_RegisterInd (MsgClass, Mode, ME_URCCallback);

	if ( ret < 0 )
		return -1;

	id1 = (int)ret/100;
	id2 = (int)ret%100;

	IndTable.Inds[id1].regs[id2].hwnd = hwnd;
	IndTable.Inds[id1].regs[id2].msg = msg;

	return ret;
}

BOOL ME_UnRegisterIndication (int Handle)
{
	int retval;

	retval = ME_UnRegisterInd (Handle);

	if ( retval < 0 )
		return FALSE;

	return TRUE;
}

int ME_CancelFunction (int Handle)
{
	struct ME_REQUEST_WIN * request;
	int retval;

	if ( Handle < 0 )
		return -1;

	request = FindRequestNode (Handle);

	if ( request == NULL )
		return -1;

	retval = ME_CancelRequest (&(request->request));

	if ( retval < 0 ) {
		PostMessage (request->hWnd, request->message, ME_RS_USERSTOP, 0);
		request->used_flag = 0;
	}

	return 0;
}

int ME_GetResult (void* result, int result_len)
{
	return ME_GetCurrentResult (result, result_len);
}

int ME_VoiceDial (HWND hWnd, int Msg, char* PhoneNum, char* Extention)
{
	int retval = 0;
	int	len;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

	len = strlen (PhoneNum);
	strcpy (request->cmddata, PhoneNum);
	if ( Extention != NULL )
	{
		memcpy (&request->cmddata[len+1], Extention, strlen(Extention));
		request->request.cmddata_len = len + strlen(Extention) + 1;
	}
	else
		request->request.cmddata_len = len;

    /* Fill the request */
	request->request.cmd = ME_VOICE_DIAL;
	request->request.cmddata = request->cmddata;
    request->request.reply = ME_WndCallback;

	retval = ME_Request (&(request->request));

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_VoiceHangup( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_VOICE_HANGUP;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_CallAnswer( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_VOICE_ANSWER;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_ListCurCalls( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_CURRENT_CALLS;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SwitchCallStatus( HWND hWnd, int Msg, int Style, int Index )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.arg1 = Style;
	request->request.arg2 = Index;
	request->request.cmd = ME_SWITCH_CALLLINE;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SetAutoAnswerStatus( HWND hWnd, int Msg, int Times )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.arg1 = Times;

	request->request.cmd = ME_SET_AUTOANSWER;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetAutoAnswerStatus( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_GET_AUTOANSWER;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SetCallWaitingStatus( HWND hWnd, int Msg, BOOL bEnable, int Class )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.arg1 = bEnable;
	request->request.arg2 = Class;
	request->request.cmd = ME_SET_CALLWAITING;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetCallWaitingStatus( HWND hWnd, int Msg, int Class )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.arg1 = Class;
	request->request.cmd = ME_GET_CALLWAITING;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SetCallForwardStatus( HWND hWnd, int Msg, PCALL_FWD pCallFwd )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;;

	if ( pCallFwd == NULL )
		return -1;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

	memcpy( request->cmddata, pCallFwd, sizeof(CALL_FWD));

    /* Fill the request */
	request->request.cmd = ME_SET_CALLFWDING;

	request->request.cmddata = request->cmddata;
	request->request.cmddata_len = sizeof(CALL_FWD);
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetCallForwardStatus( HWND hWnd, int Msg, int Style, int Class )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.arg1 = Style;
	request->request.arg2 = Class;

	request->request.cmd = ME_GET_CALLFWDING;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SetCallBarringStatus( HWND hWnd, int Msg, int Style,BOOL bEnable, char* Password, int Class )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	
	if ( Password != NULL )
	{
		strcpy( request->cmddata, Password );
		request->request.cmddata = request->cmddata;
		request->request.cmddata_len = strlen( Password );
	}
    else
        request->request.cmddata = NULL;

    /* Fill the request */
	request->request.arg1 = Style;
	request->request.arg2 = bEnable;
	request->request.arg3 = Class;

	request->request.cmd = ME_SET_CALLBARRING;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetCallBarringStatus (HWND hWnd, int Msg, int Style, int Class, char* Password)
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = GetRequestNode ();

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	
	request->request.cmddata = NULL;

    /* Fill the request */
	request->request.arg1 = Style;
	request->request.arg2 = Class;

	request->request.cmd = ME_GET_CALLBARRING;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SMS_Send( HWND hWnd, int Msg, PSMS_SEND pSend )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	if ( pSend == NULL )
		return -1;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	memcpy( request->cmddata, pSend, sizeof(SMS_SEND) );
	
    /* Fill the request */
    request->request.arg1 = 0;
	request->request.cmddata = request->cmddata;
	request->request.cmddata_len = sizeof(SMS_SEND);

	request->request.cmd = ME_SMS_SEND;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SMS_Read( HWND hWnd, int Msg, BOOL bPreRead, int MemType, int Index )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	
    /* Fill the request */
	request->request.arg1 = bPreRead;
	request->request.arg2 = MemType;
	request->request.arg3 = Index;

	request->request.cmd = ME_SMS_READ;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SMS_Delete( HWND hWnd, int Msg, int MemType, int Index )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	
    /* Fill the request */
	request->request.arg1 = MemType;
	request->request.arg2 = Index;

	request->request.cmd = ME_SMS_DELETE;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SMS_List( HWND hWnd, int Msg, int Type, BOOL bPreRead, int MemType )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.arg1 = bPreRead;
	request->request.arg2 = MemType;
	request->request.arg3 = Type;

	request->request.cmd = ME_SMS_LIST;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SMS_SetSCA( HWND hWnd, int Msg, char* SCA )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	if ( SCA == NULL )
		return -1;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	strcpy( request->cmddata, SCA );

    /* Fill the request */
	request->request.cmddata = request->cmddata;
	request->request.cmddata_len = strlen( SCA );

	request->request.cmd = ME_SMS_SET_SCA;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SMS_GetSCA( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_SMS_GET_SCA;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SMS_SetIndStatus( HWND hWnd, int Msg, PIND_STATUS pStatus )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	if ( pStatus == NULL )
		return -1;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	memcpy( request->cmddata, pStatus, sizeof(IND_STATUS) );

    /* Fill the request */
	request->request.cmddata = request->cmddata;
	request->request.cmddata_len = sizeof(IND_STATUS);

	request->request.cmd = ME_SMS_SETINDSTATUS;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SMS_GetIndStatus( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_SMS_GETINDSTATUS;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SMS_SetCellBroadcast( HWND hWnd, int Msg, PCELL_BROADCAST pStatus )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	if ( pStatus == NULL )
		return -1;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	memcpy( request->cmddata, pStatus, sizeof(CELL_BROADCAST) );

    /* Fill the request */
	request->request.cmddata = request->cmddata;
	request->request.cmddata_len = sizeof(CELL_BROADCAST);

	request->request.cmd = ME_SET_CELLBROADCAST;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SMS_GetCellBroadcast( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_GET_CELLBROADCAST;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SMS_GetMemStatus( HWND hWnd, int Msg, int MemType )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	
    /* Fill the request */
	request->request.arg1 = MemType;

	request->request.cmd = ME_SMS_MEMCOUNT;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_ReadPhonebook(HWND hWnd, int Msg, int Type, int IndexBegin, int IndexEnd)
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.arg1 = Type;
	request->request.arg2 = IndexBegin;
	request->request.arg3 = IndexEnd;

	request->request.cmd = ME_PBOOK_READ;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_WritePhonebook( HWND hWnd, int Msg, int type, PPHONEBOOK Record )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	if ( NULL == Record )
		return -1;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;	
	memcpy( request->cmddata, Record, sizeof(PHONEBOOK) );

    /* Fill the request */
    request->request.arg1 = type;
	request->request.cmddata = request->cmddata;
	request->request.cmddata_len = sizeof( PHONEBOOK );

	request->request.cmd = ME_PBOOK_WRITE;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetPhonebookMemStatus( HWND hWnd, int Msg, int Type )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.arg1 = Type;

	request->request.cmd = ME_PBOOK_MEMCOUNT;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SetPhoneLockStatus( HWND hWnd, int Msg, int Type, char* Password, BOOL bEnable )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

	if ( Password != NULL )
	{
		strcpy( request->cmddata, Password );
		request->request.cmddata = request->cmddata;
		request->request.cmddata_len = strlen( Password );
	}
    else {
        request->request.cmddata = NULL;
    }

    /* Fill the request */
	request->request.arg1 = Type;
	request->request.arg2 = bEnable;

	request->request.cmd = ME_SET_LOCK;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetPhoneLockStatus (HWND hWnd, int Msg, int Type, char* Password)
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
    request->request.cmddata = NULL;

    /* Fill the request */
	request->request.arg1 = Type;

	request->request.cmd = ME_GET_LOCK;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_ChangeLockPassword( HWND hWnd, int Msg, int Type, char* OldPass, char* NewPass )
{
	int retval = 0;
	int	len;
	struct ME_REQUEST_WIN * request;

	if ( OldPass == NULL || NewPass == NULL )
		return -1;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

	len = strlen( OldPass );
	strcpy( request->cmddata, OldPass );
	memcpy( &request->cmddata[len+1], NewPass, strlen(NewPass) );

    /* Fill the request */
	request->request.cmd = ME_PASS_CHANGE;
	request->request.cmddata = request->cmddata;
	request->request.cmddata_len = len + strlen(NewPass) + 1;
	request->request.arg1 = Type;

    request->request.reply = ME_WndCallback;
	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}


int ME_PasswordValidation( HWND hWnd, int Msg, int Type, char* Password, char* NewPass )
{
	int retval = 0, len;
	struct ME_REQUEST_WIN * request;

	if ( Password == NULL )
		return -1;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

	len = strlen( Password );
	strcpy( request->cmddata, Password );
	if ( NewPass != NULL )
		memcpy (&request->cmddata[len+1], NewPass, strlen(NewPass));

    /* Fill the request */
	request->request.cmddata = request->cmddata;
	if ( NewPass != NULL )
		request->request.cmddata_len = len + strlen(NewPass) + 1;
	else
		request->request.cmddata_len = len;

	request->request.arg1 = Type;

	request->request.cmd = ME_PASS_VALID;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetCurWaitingPassword( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_GET_PASS;
    request->request.reply = ME_WndCallback;
	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetPassInputLimit( HWND hWnd, int Msg, int Type )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_PASS_REMAIN;
    request->request.reply = ME_WndCallback;
	request->request.arg1 = Type;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SetOpratorMode( HWND hWnd, int Msg, int Mode, char* OperId )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

	if ( OperId != NULL )
	{
		strcpy( request->cmddata, OperId );
		request->request.cmddata = request->cmddata;
		request->request.cmddata_len = strlen( OperId);
	}
	else
		request->request.cmddata = NULL;

    /* Fill the request */
	request->request.arg1 = Mode;

	request->request.cmd = ME_SET_OPRATOR;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetCurOprator ( HWND hWnd, int Msg, int Format )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.arg1 = Format;

	request->request.cmd = ME_GET_OPRATOR;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SearchNetOprator ( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_SRCH_OPRATOR;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_ReadPrefOperList( HWND hWnd, int Msg, int BeginIndex, int EndIndex )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.arg1 = BeginIndex;
	request->request.arg2 = EndIndex;
	request->request.cmd = ME_GET_POPRATOR;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_WritePrefOperList( HWND hWnd, int Msg, int index, char* oper_id )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;


    /* Fill the request */
	if ( oper_id != NULL )
	{
		strcpy( request->cmddata, oper_id );
		request->request.cmddata = request->cmddata;
		request->request.cmddata_len = strlen(oper_id);
	}
	else
		request->request.cmddata = NULL;

	request->request.arg1 = index;

	request->request.cmd = ME_SET_POPRATOR;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetEquipmentId ( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_GET_IMEI;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetSubscriberId ( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_GET_IMSI;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SetClockTime ( HWND hWnd, int Msg, char* DateTime )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	if ( DateTime == NULL )
		return -1;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	strcpy( request->cmddata, DateTime );

    /* Fill the request */
	request->request.cmddata = request->cmddata;
	request->request.cmddata_len = strlen( DateTime );

	request->request.cmd = ME_SET_CLOCK;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetClockTime ( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_GET_CLOCK;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_MuteControl( HWND hWnd, int Msg, BOOL bEnable )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.arg1 = bEnable;
	request->request.cmd = ME_MUTE_CONTROL;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SendDTMF ( HWND hWnd, int Msg, char SendChar )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.arg1 = SendChar;
	request->request.cmd = ME_SEND_DTMF;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_FactoryDefaultSetting( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_FAC_SETTING;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_ResetModule( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_RESET;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SleepModule( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_SLEEP;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_ShutdownModule( HWND hWnd, int Msg )
{
	PostMessage (hWnd, Msg, ME_RS_SUCCESS, 0);
	return 0;
}

int ME_PowerOff( HWND hWnd, int Msg )
{
    int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_SHUTDOWN;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetBatteryInfo( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_GET_BATTERY;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetSignalInfo( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_GET_SIGNAL;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_STK_Send( HWND hwnd, int msg, STK_PARAM* param )
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	if ( param == NULL )
		return -1;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hwnd;
    request->message = msg;

    /* Fill the request */
	request->request.cmddata = param;
	request->request.cmddata_len = sizeof( STK_PARAM );

	request->request.cmd = ME_STK_SEND;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_STK_GetRemoteSAT( HWND hwnd, int msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hwnd;
    request->message = msg;

    /* Fill the request */
	request->request.cmd = ME_STK_GETSAT;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SMS_Acknowledge( HWND hwnd, int msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hwnd;
    request->message = msg;

    /* Fill the request */
	request->request.cmd = ME_SMS_ACK;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SetSpeakerVolume( HWND hwnd, int msg, int level )
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hwnd;
    request->message = msg;

    /* Fill the request */
	request->request.arg1 = level;
	request->request.cmd = ME_SET_VOLUME;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetSpeakerVolume( HWND hwnd, int msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hwnd;
    request->message = msg;

    /* Fill the request */
	request->request.cmd = ME_GET_VOLUME;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetServCellInfo( HWND hwnd, int msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hwnd;
    request->message = msg;

    /* Fill the request */
	request->request.cmd = ME_MONI_SERV;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetNeibCellInfo( HWND hwnd, int msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hwnd;
    request->message = msg;

    /* Fill the request */
	request->request.cmd = ME_MONI_NEIB;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SetAlarm (HWND hwnd, int msg, ME_ALARMTIME* time, int index)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hwnd;
    request->message = msg;

	if ( time == NULL )
		return -1;

	memcpy (request->cmddata, time, sizeof(ME_ALARMTIME));

	request->request.arg1 = index;
	request->request.cmddata = request->cmddata;
	request->request.cmddata_len = sizeof(ME_ALARMTIME);
	request->request.cmd = ME_SETALARM;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetAlarm (HWND hwnd, int msg, int index)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hwnd;
    request->message = msg;

	request->request.arg1 = index;
	request->request.cmd = ME_GETALARM;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SelectAudioMode (HWND hwnd, int msg, int mode)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;
	
	request = GetRequestNode ();
	if ( NULL == request )
		return -1;
	
    request->hWnd = hwnd;
    request->message = msg;
	
	request->request.arg1 = mode;
	request->request.cmd = ME_AUDIO;
    request->request.reply = ME_WndCallback;
	
	retval = ME_Request( &(request->request) );
	
	if ( retval < 0 )
		request->used_flag = 0;
	
	return retval;
}

int ME_SpecialDial (HWND hWnd, int nMsg, char * ssCode, BOOL callControl)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;
	
	if ( ssCode == NULL )
        return -1;

    request = GetRequestNode ();
	if ( NULL == request )
		return -1;
	
    request->hWnd = hWnd;
    request->message = nMsg;

    strcpy (request->cmddata, ssCode);
    request->request.cmddata = request->cmddata;
    request->request.cmddata_len = strlen (ssCode)+1;
    request->request.arg1 = callControl;
    request->request.arg2 = -1;
    request->request.arg3 = 0;
    request->request.result = NULL;
    request->request.result_len = -1;
	request->request.cmd = ME_SSCALL;
    request->request.reply = ME_WndCallback;
	
	retval = ME_Request( &(request->request) );
	
	if ( retval < 0 )
		request->used_flag = 0;
	
	return retval;
}

int ME_GetCCM (HWND hWnd, int nMsg)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;
	
    request = GetRequestNode ();
	if ( NULL == request )
		return -1;
	
    request->hWnd = hWnd;
    request->message = nMsg;

    request->request.result = NULL;
    request->request.result_len = -1;
	request->request.cmd = ME_GETCCM;
    request->request.reply = ME_WndCallback;
	
	retval = ME_Request( &(request->request) );
	
	if ( retval < 0 )
		request->used_flag = 0;
	
	return retval;
}

int ME_GetACM (HWND hWnd, int nMsg)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;
	
    request = GetRequestNode ();
	if ( NULL == request )
		return -1;
	
    request->hWnd = hWnd;
    request->message = nMsg;

    request->request.result = NULL;
    request->request.result_len = -1;
	request->request.cmd = ME_GETACM;
    request->request.reply = ME_WndCallback;
	
	retval = ME_Request( &(request->request) );
	
	if ( retval < 0 )
		request->used_flag = 0;
	
	return retval;
}

int ME_GetACMmax (HWND hWnd, int nMsg)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;
	
    request = GetRequestNode ();
	if ( NULL == request )
		return -1;
	
    request->hWnd = hWnd;
    request->message = nMsg;

    request->request.result = NULL;
    request->request.result_len = -1;
	request->request.cmd = ME_GETACMAX;
    request->request.reply = ME_WndCallback;
	
	retval = ME_Request( &(request->request) );
	
	if ( retval < 0 )
		request->used_flag = 0;
	
	return retval;
}

int ME_ResetACM (HWND hWnd, int nMsg, char * Password)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;
	
	if ( Password == NULL )
        return -1;

    request = GetRequestNode ();
	if ( NULL == request )
		return -1;
	
    request->hWnd = hWnd;
    request->message = nMsg;

    strcpy (request->cmddata, Password);
    request->request.cmddata = request->cmddata;
    request->request.cmddata_len = strlen (Password)+1;
    request->request.result = NULL;
    request->request.result_len = -1;
	request->request.cmd = ME_RESETACM;
    request->request.reply = ME_WndCallback;
	
	retval = ME_Request( &(request->request) );
	
	if ( retval < 0 )
		request->used_flag = 0;
	
	return retval;
}

int ME_SetACMmax (HWND hWnd, int nMsg, unsigned long ACMmax, char * Password)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;
	
	if ( Password == NULL )
        return -1;

    request = GetRequestNode ();
	if ( NULL == request )
		return -1;
	
    request->hWnd = hWnd;
    request->message = nMsg;

    strcpy (request->cmddata, Password);
    request->request.cmddata = request->cmddata;
    request->request.cmddata_len = strlen (Password)+1;
    request->request.arg1 = ACMmax;
    request->request.result = NULL;
    request->request.result_len = -1;
	request->request.cmd = ME_SETACMAX;
    request->request.reply = ME_WndCallback;
	
	retval = ME_Request( &(request->request) );
	
	if ( retval < 0 )
		request->used_flag = 0;
	
	return retval;
}

int ME_GetPricePerUnit (HWND hWnd, int nMsg)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;
	
    request = GetRequestNode ();
	if ( NULL == request )
		return -1;
	
    request->hWnd = hWnd;
    request->message = nMsg;

    request->request.result = NULL;
    request->request.result_len = -1;
	request->request.cmd = ME_GETPUC;
    request->request.reply = ME_WndCallback;
	
	retval = ME_Request( &(request->request) );
	
	if ( retval < 0 )
		request->used_flag = 0;
	
	return retval;
}

int ME_SetPricePerUnit (HWND hWnd, int nMsg, PME_PUCINFO Puc, char * Password)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;
	
	if ( Password == NULL || Puc == NULL )
        return -1;

    request = GetRequestNode ();
	if ( NULL == request )
		return -1;
	
    request->hWnd = hWnd;
    request->message = nMsg;

    memcpy (request->cmddata, Puc, sizeof(Puc));
    strcpy (&(request->cmddata[sizeof(Puc)]), Password);
    request->request.cmddata = request->cmddata;
    request->request.cmddata_len = sizeof(Puc) + strlen (Password)+1;
    request->request.result = NULL;
    request->request.result_len = -1;
	request->request.cmd = ME_SETPUC;
    request->request.reply = ME_WndCallback;
	
	retval = ME_Request( &(request->request) );
	
	if ( retval < 0 )
		request->used_flag = 0;
	
	return retval;
}

int ME_USSD_SetNotify (HWND hWnd, int nMsg)
{
    if ( gUssdObj == NULL )
        return -1;

    gUssdObj->hWnd = hWnd;
    gUssdObj->message = nMsg;

    return 0;
}

void * ME_USSD_Request (char * str)
{
    int retval;

    if ( str == NULL )
        return NULL;

    gUssdObj->request.result = NULL;
    gUssdObj->request.result_len = 0;

    retval = USSD_Request (ME_USSD_REQ, str, strlen(str));

    if ( retval < 0 )
        return NULL;

	return gUssdObj;
}

int ME_USSD_Response (void * Obj, char * str)
{
    int retval;

    if ( str == NULL || Obj != gUssdObj )
        return -1;

    gUssdObj->request.cmddata = gUssdObj->cmddata;
    retval = USSD_Request (ME_USSD_RSP, str, strlen (str));

	return retval;
}

int ME_USSD_Abort (void * Obj)
{
    int retval;

    if ( Obj != gUssdObj )
        return -1;

    retval = USSD_Request (ME_USSD_ABT, NULL, 0);

    return retval;
}

int ME_SMS_SetFormat (HWND hwnd, int msg, BOOL bPDU)
{
    if ( bPDU )
        PostMessage (hwnd, msg, ME_RS_SUCCESS, 0);
    else
        PostMessage (hwnd, msg, ME_RS_FAILURE, 0);

    return 0;
}

int ME_SMS_SetProtocol(HWND hwnd, int msg, BOOL bPhasePlus)
{
    PostMessage (hwnd, msg, ME_RS_SUCCESS, 0);
    return 0;
}

int ME_SendATCmd (HWND hwnd, int msg, char* cmd, int timeout)
{
	int retval = 0;
	int len;
	struct ME_REQUEST_WIN* request;

	if ( cmd == NULL || timeout <= 0 )
		return -1;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hwnd;
    request->message = msg;

	len = strlen( cmd );

	if ( len+1 >= CMD_DATALEN )
		return -1;

	strcpy( request->cmddata, cmd );
	strcat( request->cmddata, "\r" );

	request->request.cmddata = request->cmddata;
	request->request.cmddata_len = len + 2;

	request->request.arg1 = timeout;

	request->request.cmd = ME_AT_SEND;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetCallLineIndRestriction(HWND hwnd, int msg)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hwnd;
    request->message = msg;

    /* Fill the request */
	request->request.cmd = ME_GET_CLIR;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_SetCallLineIndRestriction(HWND hwnd, int msg, CLIR_MODE para)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	request = GetRequestNode ();
	if ( NULL == request )
		return -1;

    request->hWnd = hwnd;
    request->message = msg;

    /* Fill the request */
	request->request.arg1 = para;
	request->request.cmd = ME_SET_CLIR;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	if ( retval < 0 )
		request->used_flag = 0;

	return retval;
}

int ME_GetLastError (HWND hWnd, int Msg)
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;
	
    request = GetRequestNode ();
	if ( NULL == request )
		return -1;
	
    request->hWnd = hWnd;
    request->message = Msg;

    request->request.result = NULL;
    request->request.result_len = -1;
	request->request.cmd = ME_GETLASTERROR;
    request->request.reply = ME_WndCallback;
	
	retval = ME_Request( &(request->request) );
	
	if ( retval < 0 )
		request->used_flag = 0;
	
	return retval;
}

