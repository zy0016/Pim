//#include "..\tcpip\include\tilib.h"
#include "stdlib.h"
#include "me_at.h"
#include "window.h"
#include "me_func.h"
#include "me_wnd.h"
#include "string.h"
#include "vme.h"
#include "error.h"
#include "vatc.h"

#define RD_BUFSIZE		4096
#define WR_BUFSIZE		4096
VME_VATCCHANID com_fd;


struct com_inbuf
{
	char* base;
	int	  head;
	int	  tail;
	int	  datalen;
};

struct com_inbuf RdBuffer;
char* WrBuffer;
BOOL bDataAvailToSend = 0;//FALSE;
char* SendPtr;
int   toSend;

extern HWND ME_hWnd;
static MAT_Indication mat_indication;
extern void debugex_nonl(char *fmt, ...);

vVOID vProcessVATC_Events(vT_EvtCode Event, vINT8 ChanId);
int Handle_vatcWrite( VME_VATCCHANID ChanId );
int Handle_vatcRead ( VME_VATCCHANID ChanId );

#define EVENT_DATAIN	1
#define EVENT_DATAOUT	2
#define EVENT_MODEM		3

int MAT_Callback( int datalen )
{
	char* head = RdBuffer.base+RdBuffer.head;
	char t;

	if ( datalen > 0 )
	{
		t = head[datalen];
		head[datalen] = '\0';
		debugex_nonl( "\r\nME recieve: %s, datalen = %d", head, datalen );
		head[datalen] = t;
	}
    
	mat_indication ( MAT_DATAIN, head, datalen );

	RdBuffer.head += datalen;

	if ( RdBuffer.head >= RD_BUFSIZE)
		RdBuffer.head = 0;

	if ( datalen > 0 )
		Handle_vatcRead( com_fd );

    return 0;
}

int Handle_vatcRead ( VME_VATCCHANID ChanId )
{
    vUINT16 count = VME_vatcGetReadBufferLength( ChanId );
    
#if defined VME_DEBUG
	debugex_nonl( "\r\n1RdBuffer.datalen = %d!", RdBuffer.datalen );
	debugex_nonl( "\r\nME Get Readbuffer Len = %d!", count );
#endif

    while (count > 0)
    {
        vINT16 len, l;

        if (RdBuffer.datalen >= RD_BUFSIZE)
        {
            MAT_Reset( 0 );
			RdBuffer.datalen = RdBuffer.head = RdBuffer.tail = 0;
            break;
        }

        if (RdBuffer.head > RdBuffer.tail)
          len = RdBuffer.head - RdBuffer.tail;
        else
          len = RD_BUFSIZE - RdBuffer.tail;

       if (len + RdBuffer.datalen > RD_BUFSIZE)
           len = RD_BUFSIZE - RdBuffer.datalen;

#if defined VME_DEBUG
		debugex_nonl( "\r\nME Want to Read %d Bytes!", len );
#endif

       l =  VME_vatcRead( ChanId, RdBuffer.base+RdBuffer.tail, len);

#if defined VME_DEBUG
		debugex_nonl( "\r\nME Really Readed %d Bytes!", l );
#endif
       RdBuffer.tail += l;
       RdBuffer.datalen += l;

       if (RdBuffer.tail >= RD_BUFSIZE)
	   {
			RdBuffer.tail = 0;
#if defined VME_DEBUG
		debugex_nonl( "\r\nME Read tail Greater than BUFSIZE!", l );
#endif
			goto NOTIFY_PROCESS;
	   }

       // cannot read more, wait for next event
       if (l != len)
       {
#if defined VME_DEBUG
		debugex_nonl( "\r\nME No more data to read!", l );
#endif

           goto NOTIFY_PROCESS;
       }

       count -= l;
    }

NOTIFY_PROCESS:
#if defined VME_DEBUG
	debugex_nonl( "\r\n2RdBuffer.datalen = %d!", RdBuffer.datalen );
#endif
    count = RdBuffer.datalen;
	RdBuffer.datalen = 0;
	MAT_Callback( count );

    return 0;
}


int MAT_Init ( int mode, int para1, int para2, MAT_Indication indication )
{
	mat_indication = indication;

	com_fd = VME_vatcOpen( eVATC_ATC1 );//2 );

#if defined VME_DEBUG
		debugex_nonl( "\r\nME Open Channel %d!", com_fd );
#endif

    if ( com_fd < 0 )
    {
        vT_StatusCode err = VME_GetLastError();
#if defined VME_DEBUG
	    if ( err == VME_VATC_INVALID_CHANNEL )
    		debugex_nonl( "\r\nME Open Channel Invalid Channel!" );
        else if ( err == VME_VATC_ERROR_ALREADY_OPEN )
    		debugex_nonl( "\r\nME Open Channel already open!" );
#endif
		return -1;
    }
	RdBuffer.base = malloc( RD_BUFSIZE);
	if ( RdBuffer.base == NULL )
	{
#if defined VME_DEBUG
		debugex_nonl( "\r\nME Read Buffer Malloc Fail!" );
#endif
		return -1;
	}
	
	WrBuffer = malloc( WR_BUFSIZE );
	if ( WrBuffer == NULL )
	{
#if defined VME_DEBUG
		debugex_nonl( "\r\nME Write Buffer Malloc Fail!" );
#endif
		return -1;
	}
	RdBuffer.head = 0;
	RdBuffer.tail = 0;
	RdBuffer.datalen = 0;

    return 0;
}

// transmit function, sends an event when all data are sent or an error occurred
int Handle_vatcWrite( VME_VATCCHANID ChanId )
{
	// try to send all data
	vINT16 sent = VME_vatcWrite( ChanId, SendPtr, toSend );

	if (sent >= 0)
	{
		// when not all data sent, a message will occur later
		bDataAvailToSend = (sent != toSend);
		toSend -= sent; // prepare next transmission
		SendPtr += sent;

		if (!bDataAvailToSend)
		{
			// all data are sent out, trigger ourself
#if defined VME_DEBUG
		debugex_nonl( "\r\nME Write %d bytes!", sent );
#endif
			return sent;
		}
#if defined VME_DEBUG
		debugex_nonl( "\r\nME Write %d bytes!", sent );
#endif
		return sent;
	}
	else
	{
        vT_StatusCode err = VME_GetLastError();
        bDataAvailToSend = FALSE;
		// error occurred! trigger ourself
        
#if defined VME_DEBUG
        if ( err == VME_VATC_INVALID_CHANNEL )
            debugex_nonl( "\r\n VATC Write Invalid channel");
        else if ( err == VME_VATC_TOO_MANY_DATA )
            debugex_nonl( "\r\n VATC Write Too Many Data");
		debugex_nonl( "\r\nME Write ATC FAIL!" );
#endif
		return -1;
	}
}

int MAT_Send ( int mode, void * pdata, int datalen )
{
    if ( datalen == -1 )
        datalen = strlen( (char*)pdata );

    if (!bDataAvailToSend)
	{
		bDataAvailToSend = TRUE;

		memcpy( WrBuffer, pdata, datalen );
		SendPtr = WrBuffer;
		toSend = datalen;

        debugex_nonl( "\r\nME Send: %s\r\n, datalen=%d", pdata, datalen );
		return Handle_vatcWrite( com_fd );	// data will sent
	}
	else
	{
#if defined VME_DEBUG
		debugex_nonl( "\r\nME No data avai to send!" );
#endif
		return -1;
	}
}



int MAT_Reset ( int mode )
{
    return 0;
}

vVOID vProcessVATC_Events(vT_EvtCode Event, vINT8 ChanId)
{
	switch( Event )
	{
	case VME_EVT_VATC_RECEIVED_DATA_AVAIL:

		Handle_vatcRead( ChanId );

		break;

	case VME_EVT_VATC_WRITE_CALLBACK:

		if ( bDataAvailToSend )
			Handle_vatcWrite( ChanId );

		break;

	default:
		break;
	}

	return;
}
