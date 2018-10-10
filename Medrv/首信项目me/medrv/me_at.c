#if defined MULTI_THREAD
#include "..\tcpip\include\tilib.h"
#endif
#include "string.h"
#include "window.h"
#include "fapi.h"
#include "stdlib.h"
#include "device.h"
#include "me_at.h"
#include "me_func.h"
#include "me_wnd.h"
#include "muxagent.h"

#define RD_BUFSIZE	1024
#define BUFSIZE		256

static int com_fd;

extern HWND ME_hWnd;
static MAT_Indication mat_indication;

#if defined MULTI_THREAD
typedef struct com_inbuf
{
	char	buf[RD_BUFSIZE];
	char*	data;
	char*	write;
	int		datalen;
}RD_BUFFER;

RD_BUFFER RdBuf;

static OSHANDLE hEvent;
#if (_HOPEN_VER >= 200)
	MUTEX		mutex;
#else
	OSHANDLE	mutex;
#endif

static int BUF_Cleanup(RD_BUFFER* pbuf);
static int BUF_GetData(RD_BUFFER* pbuf, char* buf, int buflen);
static int BUF_PutData(RD_BUFFER* pbuf, char* data, int datalen);
extern void vDelay_main(unsigned char local);
#endif
static int mux_status;


UINT ME_EventProc(LONG lUserData, DWORD dwEventMask, DWORD param1, LONG param2);

#define EVENT_DATAIN	1
#define EVENT_DATAOUT	2
#define EVENT_MODEM		3

int MAT_Callback(void)
{
#if defined MULTI_THREAD
	char buf[RD_BUFSIZE];
	int  l;

	LIB_WaitMutex(&mutex);

	while ( (l = BUF_GetData( &RdBuf, buf, RD_BUFSIZE )) > 0 )
	{
#if defined _EMULATE
	{
		int i;
		MsgOut( "\r\nME recieve: " );
		for ( i=0; i<l; i++)
			MsgOut( "%c", buf[i] );
	}
#endif
		mat_indication ( MAT_DATAIN, buf, l );
	}

	LIB_SignalMutex(&mutex);

#else
	char buf[BUFSIZE];
	int count;

	while (1)
	{
		count = MUX_ReadFile( com_fd, buf, BUFSIZE );

		if ( count < 0 )
			break;

#if defined _EMULATE
	{
		int i;
		MsgOut( "\r\nME recieve: " );
		for ( i=0; i<count; i++)
			MsgOut( "%c", buf[i] );
	}
#endif
		mat_indication( MAT_DATAIN, buf, count );
	}

#endif
    return 0;
}

#if defined MULTI_THREAD
DWORD WINAPI MAT_Thread ( LPVOID lpParameter )
{
    int len, l;
	char buf[BUFSIZE];

	for (;;)
	{
		while ( 1 )
		{
			len = MUX_ReadFile( com_fd, buf, BUFSIZE );

			if ( len == 0 )
				continue;

			if ( len < 0 )
				break;

			LIB_WaitMutex(&mutex);
			l = BUF_PutData(&RdBuf, buf, len );
			LIB_SignalMutex(&mutex);

			// buffer full
			if ( l <= 0 )
			{
				MAT_Reset( 0 );
				LIB_WaitMutex(&mutex);
				BUF_Cleanup(&RdBuf);
				LIB_SignalMutex(&mutex);
				LIB_WaitEvent( hEvent );
			}
		}

		PostMessage( ME_hWnd, WM_MAT_MESSAGE, 0, 0);
		LIB_WaitEvent( hEvent );
	}

    return 0;
}

#endif

int MAT_Init ( int mode, int para1, int para2, MAT_Indication indication )
{
	char pTemp[64];
	UART_CONTROL Uart_Control;

	mat_indication = indication;
#if defined MULTI_THREAD
	BUF_Cleanup(&RdBuf);

	LIB_CreateMutex(&mutex);

    hEvent = LIB_CreateEvent ("MEdrv");

	if ( hEvent == NULL )
		return -1;

	LIB_CreateTask("MEdrv", TS_REALTIME|32, MAT_Thread, 2048, NULL);
#endif

    com_fd = MUX_OpenFile("DEV:UART0", ACCESS_READ|ACCESS_WRITE, 0);

	if ( com_fd < 0 )
		return -1;

	Uart_Control.Event_CallBack = ME_EventProc;
	Uart_Control.para = NULL;
	Uart_Control.EventCode = EV_DATAIN | EV_DATAOUT | EV_MODEM;

	MUX_FileIoctl( com_fd, IO_SETCALLBACK, &Uart_Control, sizeof(UART_CONTROL) );
	MUX_FileIoctl( com_fd, IO_DUMPBUF, NULL, 0 );

	if ( mode == MOD_MUX )
		MUX_FileIoctl(com_fd, IO_UART_SETMODEMCTL, MUX_ON, strlen(MUX_ON));

	MUX_ReadFile( com_fd, pTemp, 64 );

    return 0;
}

int MAT_Send ( int mode, void * pdata, int datalen )
{
	int offset=0, len, sent;

	if ( datalen == -1 )
		len = strlen((char*)pdata);
	else
		len = datalen;

	while (offset < len)
	{
		sent = MUX_WriteFile(com_fd, (char*)pdata+offset, len - offset);

		if (sent < 0)
			return sent;
		else
			offset += sent;

        if (offset >= 256)
            OS_Sleep (100);
	}
//add by dcl for delay
	if (strcmp(pdata,"AT+CFUN=0\r") == 0)
		vDelay_main(110);

	if (strcmp(pdata,"AT%SATC=1,FFFFFFFFFFFFFFFFFF\r") == 0)
		vDelay_main(80);

	if (strcmp(pdata,"AT+CFUN=1\r") == 0)
		vDelay_main(110);
//end by dcl for delay
#if defined _EMULATE
	MsgOut( "\r\nME Send: %s", pdata );
#endif
    return offset;
}

int MAT_Reset ( int mode )
{
    return 0;
}

UINT ME_EventProc(LONG lUserData, DWORD dwEventMask, DWORD param1, LONG param2)
{
	if ( dwEventMask == EV_DATAIN )
#if defined MULTI_THREAD
		LIB_SignalEvent( hEvent, 0 );
#else
		PostMessage( ME_hWnd, WM_MAT_MESSAGE, 0, 0);
#endif
    else if (dwEventMask == EV_DATAOUT )
    {
        int i= 0;
    }

	return 0;
}

int ME_MuxPowerUp()
{
	if ( com_fd < 0 )
		return -1;

	return MUX_FileIoctl(com_fd, IO_UART_SETMODEMCTL, MUX_ON, strlen(MUX_ON));
}

#if defined MULTI_THREAD
/*========================================================================*\
*
*	FUNCTION: BUF_Cleanup
*
*	PURPOSE : clean up all the data in the specified buffer
*
*   PARAM   : pbuf		buffer object
*
*	RETURN  : 0		succuessful completion
*			  -1	failure completion
*
\*========================================================================*/

static int BUF_Cleanup(RD_BUFFER* pbuf)
{
	if (pbuf == NULL)
		return -1;
	memset(pbuf->buf, 0, sizeof(RD_BUFSIZE));
	pbuf->write = pbuf->data = pbuf->buf;
	pbuf->datalen = 0;
	return 0;
}

/*========================================================================*\
*
*	FUNCTION: BUF_GetData
*
*	PURPOSE : get some data from specified buffer
*
*   PARAM   : pbuf		buffer object
*			  buf		reception buffer
*			  buflen	the size of recept buffer
*
*	RETURN  : actual number of data from buffer
*
\*========================================================================*/

static int BUF_GetData(RD_BUFFER* pbuf, char* buf, int buflen)
{
	char *front, *tail;
	int count = 0, copyed;

	if (pbuf == NULL || buf == NULL || buflen <= 0)
		return 0;

	front = pbuf->data;
	while (buflen > 0 && (tail = pbuf->write) != front)
	{
		if (tail > front)
			copyed = tail - front;
		else
			copyed = pbuf->buf + RD_BUFSIZE - front;
		if (copyed > buflen)
			copyed = buflen;
		memcpy(buf + count, front, copyed);
		buflen -= copyed;
		count += copyed;
		front += copyed;

		if (front >= pbuf->buf + RD_BUFSIZE)
			front -= RD_BUFSIZE;
		pbuf->data = front;
	}
	pbuf->datalen -= count;
	return count;
}

/*========================================================================*\
*
*	FUNCTION: BUF_PutData
*
*	PURPOSE : put some data into specified buffer
*
*   PARAM   : pbuf		buffer object
*			  data		data pointer
*			  datalen	data size
*
*	RETURN  : the actual number which put into the buffer
*
\*========================================================================*/

static int BUF_PutData(RD_BUFFER* pbuf, char* data, int datalen)
{
	char *front, *tail;
	int copyed, count = 0;

	if (pbuf == NULL || data == NULL || datalen <= 0)
		return 0;

	tail = pbuf->write;
	while (datalen > 0)
	{
		front = pbuf->data;
		if (tail + 1 == front || (tail + 1 == pbuf->buf + RD_BUFSIZE 
			&& front == pbuf->buf))
			break;

		if (front > tail)
			copyed = front - tail - 1;
		else if (front == 0)
			copyed = pbuf->buf + RD_BUFSIZE - tail - 1;
		else
			copyed = pbuf->buf + RD_BUFSIZE - tail;

		if (copyed > datalen)
			copyed = datalen;
		memcpy(tail, data + count, copyed);
		datalen -= copyed;
		count += copyed;
		tail += copyed;

		if (tail >= pbuf->buf + RD_BUFSIZE)
			tail -= RD_BUFSIZE;
		pbuf->write = tail;
	}
	pbuf->datalen += count;
	return count;
}
#endif