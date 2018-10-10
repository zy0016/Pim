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
#if defined MULTI_THREAD 

#include "..\tcpip\include\tilib.h"
#endif
#include "fapi.h"
#include "string.h"
#include "device.h"
#include "window.h"
#include "me_at.h"
#include "me_func.h"
#include "me_wnd.h"
#include "muxagent.h"


#define RD_BUFSIZE	1024
#define BUFSIZE		256

static int com_fd;
extern HWND ME_hWnd;
static MAT_Indication mat_indication;

typedef struct com_inbuf
{
	char	buf[RD_BUFSIZE];
	char*	data;
	char*	write;
	int		datalen;
}RD_BUFFER;

RD_BUFFER RdBuf;

#define MAX_MUXNAMELEN		50
#define MAX_CHNL			3

typedef struct tagMUXChnl
{
    int             hmux;
    int             enable;
    char            name[MAX_MUXNAMELEN];
    RD_BUFFER       rcvbuf;
    MAT_Indication  notify;
} MUXCHNL, *PMUXCHNL;

const char * devname[MAX_CHNL] = {
    MUX_CHANNEL1,
    MUX_CHANNEL2,
    MUX_CHANNEL3
};

MUXCHNL gMuxChnl[MAX_CHNL];


#if defined MULTI_THREAD
static OSHANDLE hEvent;
#if (_HOPEN_VER >= 200)
	MUTEX		mutex;
#else
	OSHANDLE	mutex;
#endif

static int BUF_Cleanup(RD_BUFFER* pbuf);
static int BUF_GetData(RD_BUFFER* pbuf, char* buf, int buflen);
static int BUF_PutData(RD_BUFFER* pbuf, char* data, int datalen);

#endif

typedef struct UART_Control
{
	_CALLBACK	Event_CallBack;
	LONG		para;
	DWORD		EventCode;
}UART_CONTROL, *PUART_CONTROL;

static int mux_status;

#define EVENT_DATAIN	1
#define EVENT_DATAOUT	2
#define EVENT_MODEM		3

UINT ME_EventProc(LONG lUserData, DWORD dwEventMask, DWORD param1, LONG param2);

int MAT_Callback (int chnl)
{
	char buf[BUFSIZE];
	int count;
    static int ready = 1;

	while (1)
	{
		count = MUX_ReadFile (gMuxChnl[chnl].hmux, buf, BUFSIZE);

		if ( count < 0 )
			break;

#ifdef _ME_DEBUG
	{
		int i;
		MsgOut ("\r\nME recieve: ");
		for ( i=0; i < count; i++ )
			MsgOut ("%c", buf[i]);
	}
#endif
        BUF_PutData (&gMuxChnl[chnl].rcvbuf, buf, count);
	}

   if ( !ready )
       return 0;

    while ( (count = BUF_GetData (&gMuxChnl[chnl].rcvbuf, buf, BUFSIZE)) > 0 ) {
        ready = 0;
        gMuxChnl[chnl].notify (MAT_DATAIN, buf, count);
        ready = 1;
    }

    return 0;
}

#if 0
int MAT_Callback(void)
{
#if defined MULTI_THREAD
	char buf[RD_BUFSIZE];
	int  l;

	LIB_WaitMutex(&mutex);

	while ( (l = BUF_GetData( &RdBuf, buf, RD_BUFSIZE )) > 0 )
	{
#if defined _ME_DEBUG
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
    static int ready = 1;

	while (1)
	{
		count = MUX_ReadFile (com_fd, buf, BUFSIZE);

		if ( count < 0 )
			break;

#if defined _ME_DEBUG
	{
		int i;
		MsgOut( "\r\nME recieve: " );
		for ( i=0; i<count; i++)
			MsgOut( "%c", buf[i] );
	}
#endif
        BUF_PutData (&RdBuf, buf, count);
	}

   if ( !ready )
       return 0;

    while ( (count = BUF_GetData (&RdBuf, buf, BUFSIZE)) > 0 ) {
        ready = 0;
        mat_indication (MAT_DATAIN, buf, count);
        ready = 1;
    }

#endif
    return 0;
}


#endif


#if defined MULTI_THREAD
void WINAPI MAT_Thread ( LPVOID lpParameter )
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

    return;
}

#endif

int MAT_Init (int mode, int para1, int para2, MAT_Indication indication)
{
	UART_CONTROL Uart_Control;

	if(para1 == 0){
		memset (gMuxChnl, 0, sizeof(MUXCHNL) * MAX_CHNL);
		
		//	mat_indication = indication;
		//	BUF_Cleanup(&RdBuf);
		
#if defined MULTI_THREAD
		LIB_CreateMutex(&mutex);
		
		hEvent = LIB_CreateEvent ("MEdrv");
		
		if ( hEvent == NULL )
			return -1;
		
		LIB_CreateTask("MEdrv", TS_REALTIME|32, MAT_Thread, 2048, NULL);
#endif
		
		com_fd = MUX_OpenFile("DEV:UART1", ACCESS_READ|ACCESS_WRITE, 0);
		
		if ( com_fd < 0 )
			return -1;
		
		Uart_Control.Event_CallBack = ME_EventProc;
		Uart_Control.para = 1;
		Uart_Control.EventCode = EV_DATAIN | EV_DATAOUT | EV_MODEM;
		
		MUX_FileIoctl( com_fd, IO_SETCALLBACK, &Uart_Control, sizeof(UART_CONTROL) );
		MUX_FileIoctl( com_fd, IO_DUMPBUF, NULL, 0 );
		
		if ( mode == MOD_MUX )
			MUX_FileIoctl(com_fd, IO_UART_SETMODEMCTL, MUX_ON, strlen(MUX_ON));
		
		gMuxChnl[1].hmux = com_fd;
		gMuxChnl[1].enable = 1;
		gMuxChnl[1].notify = indication;
    	BUF_Cleanup (&gMuxChnl[1].rcvbuf);
        strcpy (gMuxChnl[1].name, devname[1]);
	}

	else if(para1 == 1){	
    
		Uart_Control.Event_CallBack = ME_EventProc;
		Uart_Control.EventCode = EV_DATAIN | EV_DATAOUT | EV_MODEM;
        gMuxChnl[2].notify = indication;
    	BUF_Cleanup (&gMuxChnl[2].rcvbuf);
        strcpy (gMuxChnl[2].name, devname[2]);

        gMuxChnl[2].hmux = MUX_OpenFile (gMuxChnl[2].name, ACCESS_READ|ACCESS_WRITE, 0);
        gMuxChnl[2].enable = 1;
        Uart_Control.para = 2;
        MUX_FileIoctl (gMuxChnl[2].hmux, IO_SETCALLBACK, &Uart_Control, sizeof (Uart_Control));
    }

   
    return 0;
}

int MAT_Send (int mode, void * pdata, int datalen)
{
	int offset=0, len, sent;

	if ( datalen == -1 )
		len = strlen((char*)pdata);
	else
		len = datalen;

	while (offset < len)
	{
//		sent = MUX_WriteFile(com_fd, (char*)pdata+offset, len - offset);
		sent = MUX_WriteFile (gMuxChnl[mode].hmux, (char*)pdata+offset, len - offset);

		if (sent < 0)
			return sent;
		else
			offset += sent;
	}
#if defined _ME_DEBUG
	MsgOut( "\r\nME Send: %s", pdata );
#endif
    return offset;
}

int MAT_Reset (int mode)
{
    return 0;
}

int MAT_TurndownMux()
{
	mux_status = MUX_FileIoctl(com_fd, IO_UART_SETMODEMCTL, MUX_CHK, strlen(MUX_CHK));

	if ( mux_status == 1 )
		MUX_FileIoctl(com_fd, IO_UART_SETMODEMCTL, MUX_OFF, strlen(MUX_OFF));

	return 0;
}

int MAT_TurnOnMux()
{
	if ( mux_status != 1 )
		return 0;

	MUX_FileIoctl(com_fd, IO_UART_SETMODEMCTL, MUX_ON, strlen(MUX_ON));

	mux_status = 0;

	return 0;
}

UINT ME_EventProc(LONG lUserData, DWORD dwEventMask, DWORD param1, LONG param2)
{
	if ( dwEventMask == EV_DATAIN )
#if defined MULTI_THREAD
		LIB_SignalEvent( hEvent, 0 );
#else
		PostMessage( ME_hWnd, WM_MAT_MESSAGE, (WPARAM)lUserData, 0);
#endif

	return 0;
}

int ME_MuxPowerUp()
{
	if ( com_fd < 0 )
		return -1;

	return MUX_FileIoctl(com_fd, IO_UART_SETMODEMCTL, MUX_ON, strlen(MUX_ON));
}

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
