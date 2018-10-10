#include "string.h"
#include "window.h"
#include "me_func.h"
#include "me_wnd.h"
#include "me_at.h"

extern void debugex_nonl(char *fmt, ...);
struct ME_REQUEST_WIN	request_buf[MAX_QUEUE_REQUESTS];

struct ME_REQUEST_WIN * FindRequestStruct( int Handle )
{
    int i;

	if ( Handle == -1 )
	{
		for ( i = 0; i < MAX_QUEUE_REQUESTS; i ++ )
		{
			if ( request_buf[i].used_flag == 0 ) 
			{
				request_buf[i].used_flag = 1;
				
				return &request_buf[i];
			}
		}
	}
	else
	{
		for ( i = 0; i < MAX_QUEUE_REQUESTS; i ++ )
		{
			if ( request_buf[i].used_flag == 0 )
				continue;

			if ( request_buf[i].request.index == Handle )
				return &request_buf[i];
		}
	}

    return NULL;
}

void ME_WndCallback( struct ME_REQUEST * pRequest)
{
    struct ME_REQUEST_WIN * request = FindRequestStruct( pRequest->index );

	if ( request == NULL)
		return;

	SendMessage (request->hWnd, request->message, (WORD)pRequest->error, (DWORD)pRequest->errcode);

    /* Free this request structure */
    request->used_flag = 0;
}

void ME_URCCallback( struct ME_REGISTER * pRegister )
{
	SendMessage( pRegister->hwnd, pRegister->msg, 0, 0 );
}

int ME_SampleRequest (int arg1, int arg2, HWND hWnd, unsigned msg)
{
    int retval = 0;
    struct ME_REQUEST_WIN * request = FindRequestStruct( -1 );

    request->hWnd = hWnd;
    request->message = msg;

    /* Fill the request */
    request->request.reply = ME_WndCallback;

    /* Fill other fields */
    retval = ME_Request (&request->request);
    if ( retval != 0 ) 
	{
		if ( retval == 1 )
			SendMessage (request->hWnd, request->message,
				(WORD)request->request.result_len, (DWORD)request->request.result);
		/* Free this request structure */
		request->used_flag = 0;
    }

    return retval;
}

int ME_Initialize( int Mode )
{
	int retval = 0;

	struct ME_REQUEST_WIN * request = FindRequestStruct( -1 );

	request->request.cmd = ME_INIT;

	request->request.arg1 = Mode;

	retval = ME_Request( &request->request );

	request->used_flag = 0;

	return retval;
}

int ME_GSMInit_Module_3( HWND hWnd, UINT Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = FindRequestStruct( -1 );

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_INIT_WAVE_EVERYTIME;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_GSMInit_Module( HWND hWnd, UINT Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = FindRequestStruct( -1 );

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_INIT_WAVE_EQUIP_SET;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_GSMInit_Module_2( HWND hWnd, UINT Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = FindRequestStruct( -1 );

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_INIT_WAVE_EQUIP_RESET;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}


int ME_RegisterIndication( int MsgClass, int Mode, HWND hwnd, UINT msg )
{
	int ret, id1, id2;

	if ( hwnd == NULL )
		return -1;

	ret = ME_RegisterInd( MsgClass, Mode, ME_URCCallback );

	if ( ret < 0 )
		return -1;

	id1 = (int)ret/100;
	id2 = (int)ret%100;

	IndTable.Inds[id1].regs[id2].hwnd = hwnd;
	IndTable.Inds[id1].regs[id2].msg = msg;

	return ret;
}

BOOL ME_UnRegisterIndication( int Handle )
{
	int retval;

	retval = ME_UnRegisterInd( Handle );

	if ( retval < 0 )
		return FALSE;

	return TRUE;
}

int ME_CancelFunction( int Handle )
{
	struct ME_REQUEST_WIN * request;
	int retval;

	if ( Handle < 0 )
		return -1;

	request = FindRequestStruct( Handle );

	if ( request == NULL )
		return -1;

	retval = ME_CancelRequest( &(request->request) );

	if ( retval == -1 )
	{
		PostMessage( request->hWnd, request->message, ME_RS_USERSTOP, 0 );
		request->used_flag = 0;
	}

	return 0;
}

int ME_GetResult( void* result, int result_len )
{
	return ME_GetCurrentResult( result, result_len );
}

int ME_VoiceDial( HWND hWnd, int Msg, char* PhoneNum, char* Extention )
{
	int retval = 0;
	int	len;
	struct ME_REQUEST_WIN * request = FindRequestStruct( -1 );

	if ( NULL == request )
	{
#if defined VME_DEBUG
		debugex_nonl( "\r\nME Find Request Struct Fail!" );
#endif
		return -1;
	}
    request->hWnd = hWnd;
    request->message = Msg;

	len = strlen( PhoneNum );
	strcpy( request->cmddata, PhoneNum );
	if ( Extention != NULL )
	{
		memcpy( &request->cmddata[len+1], Extention, strlen(Extention) );
		request->request.cmddata_len = len + strlen(Extention) + 1;
	}
	else
		request->request.cmddata_len = len;

    /* Fill the request */
	request->request.cmd = ME_VOICE_DIAL;
	request->request.cmddata = request->cmddata;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );
#if defined VME_DEBUG
		debugex_nonl( "\r\nME Request return %d!", retval );
#endif
	return retval;
}

int ME_VoiceHangup( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = FindRequestStruct( -1 );

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_VOICE_HANGUP;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_CallAnswer( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = FindRequestStruct( -1 );

	if ( NULL == request )
	{
#if defined VME_DEBUG
		debugex_nonl( "\r\nME Find Request Struct Fail!" );
#endif		
		return -1;
	}

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_VOICE_ANSWER;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

#if defined VME_DEBUG
		debugex_nonl( "\r\nME Request return %d!", retval );
#endif
	return retval;
}

int ME_ListCurCalls( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = FindRequestStruct( -1 );

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_CURRENT_CALLS;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_SwitchCallStatus( HWND hWnd, int Msg, int Style, int Index )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = FindRequestStruct( -1 );

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

	return retval;
}

int ME_SetAutoAnswerStatus( HWND hWnd, int Msg, int Times )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = FindRequestStruct( -1 );

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.arg1 = Times;

	request->request.cmd = ME_SET_AUTOANSWER;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_GetAutoAnswerStatus( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = FindRequestStruct( -1 );

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_GET_AUTOANSWER;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_SetCallWaitingStatus( HWND hWnd, int Msg, BOOL bEnable, int Class )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = FindRequestStruct( -1 );

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

	return retval;
}

int ME_GetCallWaitingStatus( HWND hWnd, int Msg, int Class )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = FindRequestStruct( -1 );

	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.arg1 = Class;
	request->request.cmd = ME_GET_CALLWAITING;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_SetCallForwardStatus( HWND hWnd, int Msg, PCALL_FWD pCallFwd )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;;

	if ( pCallFwd == NULL )
		return -1;

	request = FindRequestStruct( -1 );
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

	return retval;
}

int ME_GetCallForwardStatus( HWND hWnd, int Msg, int Style, int Class )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = FindRequestStruct( -1 );

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

	return retval;
}

int ME_SetCallBarringStatus( HWND hWnd, int Msg, int Style,BOOL bEnable, char* Password, int Class )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = FindRequestStruct( -1 );

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

    /* Fill the request */
	request->request.arg1 = Style;
	request->request.arg2 = bEnable;
	request->request.arg3 = Class;

	request->request.cmd = ME_SET_CALLBARRING;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_GetCallBarringStatus( HWND hWnd, int Msg, int Style, int Class, char* Password )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request = FindRequestStruct( -1 );

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

    /* Fill the request */
	request->request.arg1 = Style;
	request->request.arg2 = Class;

	request->request.cmd = ME_GET_CALLBARRING;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_SMS_Send( HWND hWnd, int Msg, PSMS_SEND pSend )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	if ( pSend == NULL )
		return -1;

	request = FindRequestStruct( -1 );
	
	if ( NULL == request )
	{
#if defined VME_DEBUG
		debugex_nonl( "\r\nME Find Request Struct Fail!" );
#endif
		return -1;
	}

    request->hWnd = hWnd;
    request->message = Msg;
	memcpy( request->cmddata, pSend, sizeof(SMS_SEND) );
	
    /* Fill the request */
	request->request.cmddata = request->cmddata;
	request->request.cmddata_len = sizeof(SMS_SEND);

	request->request.cmd = ME_SMS_SEND;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

#if defined VME_DEBUG
		debugex_nonl( "\r\nME Request return %d!", retval );
#endif
		return retval;
}

int ME_SMS_Read( HWND hWnd, int Msg, BOOL bPreRead, int MemType, int Index )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = FindRequestStruct( -1 );
	
	if ( NULL == request )
	{
#if defined VME_DEBUG
		debugex_nonl( "\r\nME Find Request Struct Fail!" );
#endif
		return -1;
	}
    request->hWnd = hWnd;
    request->message = Msg;
	
    /* Fill the request */
	request->request.arg1 = bPreRead;
	request->request.arg2 = MemType;
	request->request.arg3 = Index;

	request->request.cmd = ME_SMS_READ;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

#if defined VME_DEBUG
		debugex_nonl( "\r\nME Request return %d!", retval );
#endif
		return retval;
}

int ME_SMS_Delete( HWND hWnd, int Msg, int MemType, int Index )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = FindRequestStruct( -1 );
	
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

	return retval;
}

int ME_SMS_List( HWND hWnd, int Msg, int Type, BOOL bPreRead, int MemType )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = FindRequestStruct( -1 );
	
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

	return retval;
}

int ME_SMS_SetSCA( HWND hWnd, int Msg, char* SCA )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	if ( SCA == NULL )
		return -1;

	request = FindRequestStruct( -1 );
	
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

	return retval;
}

int ME_SMS_GetSCA( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = FindRequestStruct( -1 );
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_SMS_GET_SCA;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_SMS_SetIndStatus( HWND hWnd, int Msg, PIND_STATUS pStatus )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	if ( pStatus == NULL )
		return -1;

	request = FindRequestStruct( -1 );
	
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

	return retval;
}

int ME_SMS_GetIndStatus( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = FindRequestStruct( -1 );
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_SMS_GETINDSTATUS;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_SMS_SetCellBroadcast( HWND hWnd, int Msg, PCELL_BROADCAST pStatus )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	if ( pStatus == NULL )
		return -1;

	request = FindRequestStruct( -1 );
	
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

	return retval;
}

int ME_SMS_GetCellBroadcast( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = FindRequestStruct( -1 );
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_GET_CELLBROADCAST;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_SMS_GetMemStatus( HWND hWnd, int Msg, int MemType )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = FindRequestStruct( -1 );
	
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;
	
    /* Fill the request */
	request->request.arg1 = MemType;

	request->request.cmd = ME_SMS_MEMCOUNT;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_ReadPhonebook(HWND hWnd, int Msg, int Type, int IndexBegin, int IndexEnd)
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = FindRequestStruct( -1 );
	
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

	return retval;
}

int ME_WritePhonebook( HWND hWnd, int Msg, PPHONEBOOK Record )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	if ( NULL == Record )
		return -1;

	request = FindRequestStruct( -1 );
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;	
	memcpy( request->cmddata, Record, sizeof(PHONEBOOK) );

    /* Fill the request */
	request->request.cmddata = request->cmddata;
	request->request.cmddata_len = sizeof( PHONEBOOK );

	request->request.cmd = ME_PBOOK_WRITE;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_GetPhonebookMemStatus( HWND hWnd, int Msg, int Type )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = FindRequestStruct( -1 );
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.arg1 = Type;

	request->request.cmd = ME_PBOOK_MEMCOUNT;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_SetPhoneLockStatus( HWND hWnd, int Msg, int Type, char* Password, BOOL bEnable )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = FindRequestStruct( -1 );
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

    /* Fill the request */
	request->request.arg1 = Type;
	request->request.arg2 = bEnable;

	request->request.cmd = ME_SET_LOCK;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_GetPhoneLockStatus( HWND hWnd, int Msg, int Type, char* Password )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = FindRequestStruct( -1 );
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

    /* Fill the request */
	request->request.arg1 = Type;

	request->request.cmd = ME_GET_LOCK;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_ChangeLockPassword( HWND hWnd, int Msg, int Type, char* OldPass, char* NewPass )
{
	int retval = 0;
	int	len;
	struct ME_REQUEST_WIN * request;

	if ( OldPass == NULL || NewPass == NULL )
		return -1;

	request = FindRequestStruct( -1 );
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

	return retval;
}


int ME_PasswordValidation( HWND hWnd, int Msg, int Type, char* Password, char* NewPass )
{
	int retval = 0, len;
	struct ME_REQUEST_WIN * request;

	if ( Password == NULL )
		return -1;

	request = FindRequestStruct( -1 );
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

	len = strlen( Password );
	strcpy( request->cmddata, Password );
	if ( NewPass != NULL )
		memcpy( &request->cmddata[len+1], NewPass, strlen(NewPass) );

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

	return retval;
}

int ME_GetCurWaitingPassword( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = FindRequestStruct( -1 );
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_GET_PASS;
    request->request.reply = ME_WndCallback;
	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_GetPassInputLimit( HWND hWnd, int Msg, int Type )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = FindRequestStruct( -1 );
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_PASS_REMAIN;
    request->request.reply = ME_WndCallback;
	request->request.arg1 = Type;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_SetOpratorMode( HWND hWnd, int Msg, int Mode, char* OperId )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = FindRequestStruct( -1 );
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

	return retval;
}

int ME_GetCurOprator ( HWND hWnd, int Msg, int Format )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = FindRequestStruct( -1 );
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.arg1 = Format;

	request->request.cmd = ME_GET_OPRATOR;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_SearchNetOprator ( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = FindRequestStruct( -1 );
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_SRCH_OPRATOR;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_ReadPrefOperList( HWND hWnd, int Msg, int BeginIndex, int EndIndex )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = FindRequestStruct( -1 );
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

	return retval;
}

int ME_WritePrefOperList( HWND hWnd, int Msg, int index, char* oper_id )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = FindRequestStruct( -1 );
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

	request->request.cmd = ME_SET_OPRATOR;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_GetEquipmentId ( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = FindRequestStruct( -1 );
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_GET_IMEI;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_GetSubscriberId ( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = FindRequestStruct( -1 );
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_GET_IMSI;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_SetClockTime ( HWND hWnd, int Msg, char* DateTime )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	if ( DateTime == NULL )
		return -1;

	request = FindRequestStruct( -1 );
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

	return retval;
}

int ME_GetClockTime ( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = FindRequestStruct( -1 );
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_GET_CLOCK;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_MuteControl( HWND hWnd, int Msg, BOOL bEnable )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = FindRequestStruct( -1 );
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.arg1 = bEnable;
	request->request.cmd = ME_MUTE_CONTROL;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_SendDTMF ( HWND hWnd, int Msg, char SendChar )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = FindRequestStruct( -1 );
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.arg1 = SendChar;
	request->request.cmd = ME_SEND_DTMF;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_FactoryDefaultSetting( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = FindRequestStruct( -1 );
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_FAC_SETTING;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_ResetModule( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = FindRequestStruct( -1 );
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_RESET;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_ShutdownModule( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = FindRequestStruct( -1 );
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_SHUTDOWN;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_GetBatteryInfo( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = FindRequestStruct( -1 );
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_GET_BATTERY;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_GetSignalInfo( HWND hWnd, int Msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN * request;

	request = FindRequestStruct( -1 );
	if ( NULL == request )
		return -1;

    request->hWnd = hWnd;
    request->message = Msg;

    /* Fill the request */
	request->request.cmd = ME_GET_SIGNAL;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_STK_Send( HWND hwnd, int msg, STK_PARAM* param )
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	if ( param == NULL )
		return -1;

	request = FindRequestStruct( -1 );
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

	return retval;
}

int ME_STK_GetRemoteSAT( HWND hwnd, int msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	request = FindRequestStruct( -1 );
	if ( NULL == request )
		return -1;

    request->hWnd = hwnd;
    request->message = msg;

    /* Fill the request */
	request->request.cmd = ME_STK_GETSAT;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_SMS_Acknowledge( HWND hwnd, int msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	request = FindRequestStruct( -1 );
	if ( NULL == request )
		return -1;

    request->hWnd = hwnd;
    request->message = msg;

    /* Fill the request */
	request->request.cmd = ME_SMS_ACK;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_GetServCellInfo( HWND hwnd, int msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	request = FindRequestStruct( -1 );
	if ( NULL == request )
		return -1;

    request->hWnd = hwnd;
    request->message = msg;

    /* Fill the request */
	request->request.cmd = ME_MONI_SERV;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}

int ME_GetNeibCellInfo( HWND hwnd, int msg )
{
	int retval = 0;
	struct ME_REQUEST_WIN* request;

	request = FindRequestStruct( -1 );
	if ( NULL == request )
		return -1;

    request->hWnd = hwnd;
    request->message = msg;

    /* Fill the request */
	request->request.cmd = ME_MONI_NEIB;
    request->request.reply = ME_WndCallback;

	retval = ME_Request( &(request->request) );

	return retval;
}