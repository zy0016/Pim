/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 2000-2001 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :	// For Siemens MC388
 *
 * Filename:	Me_Init.c
 *
 * Purpose :
 *
 * Author  :	Jingjing Xu
\**************************************************************************/


#include "string.h"
#include "stdlib.h"
#include "window.h"
#include "me_func.h"
#include "me_at.h"
#include "me_wnd.h"


//------------------MEInit Public Define----------------------


int Me_Fsm_Init( int event, int param, char * data, int datalen );
int Me_GetInitValue( PME_INIT_GETVALUE pMeValue );
extern void GetResultLength( struct ME_REQUEST* pRequest );

//---------- Extern Variable and Function -------------

extern	struct	ME_REQUEST *	request_head;
extern	struct	ME_FSM	me_fsm_data;
extern	UINT	TimerId;
extern	UINT	WINAPI ME_TimerProc( HWND, UINT, UINT, DWORD );



//--------------- Init ATCmd String of Every Stadge ----------------

// Control by bWaveSetFlag, only once
char	WAT_INITSTRING_0[][50] = {
	"AT\r",
	"AT+CPAS\r",
	"AT^SCKS?\r",
	"",
};

// Control by bWaveReSetFlag, when &F
char	WAT_INITSTRING_1[][50] = {
	"AT\\Q3E0Q0V1X4+CRC=1;+CLIP=1;+COPS=3,2; +CREG=1;\r",
	"AT&C1&D0&S0\r",
	"AT+CMEE=1\r",
	"AT&W\r",
	"",
};


// Common At, Init every time
char	WAT_INITSTRING_2[][50] = {
	"AT+CSMS=1\r",
	"AT^SCTM=1\r",
	"AT+CGREG=1;+CSCS=\"UCS2\";+CSSN =1,1\r",
	"AT^SSTA=1,1\r",
	"AT+COPS?;+CREG?;+CGREG?\r",		// Get query result
	"",
};



void * WAT_STRING_GRP[3] = {
	(void*)WAT_INITSTRING_0,
	(void*)WAT_INITSTRING_1,
	(void*)WAT_INITSTRING_2,
};


// AT Command Group
char (*WAT_CurCommand)[50];


//---------------- Me Init Define ---------------------

#define MEINIT_TIMER_ID				1
#define MEINIT_MAXCOUNT_ERROR		100
#define MEINIT_MAXCOUNT_TIMEOUT		100

static int ErrorCount, TimeoutCount;

// Define FSM Step
#define ME_FSMINIT_START		0
#define ME_FSMINIT_WTRSPNS_1	1
#define ME_FSMINIT_WTRSPNS_2	2
#define ME_FSMINIT_WTRSPNS_3	3
#define ME_FSMINIT_WTRSPNS_4	4
#define ME_FSMINIT_WTRSPNS_5	5
#define ME_FSMINIT_COMPLETE		7	// Keep asME_FSM_DONE

// Define Execute Type
#define ME_EXECUTE_START		0
#define ME_EXECUTE_TEST			1
#define ME_EXECUTE_COMMAND		2
#define ME_EXECUTE_LAST			3
#define ME_EXECUTE_GETVALUE		4
#define ME_EXECUTE_STKACTIVE	5

// Define FSM Execution
//[ request_head->cmd: ME_INIT_WAVE_ ] [ ME_EXECUTE_ ]

int	Me_InitFsm_ExecTable[3][7] = {
	0,1,1,4,0,0,0,		//ME_INIT_WAVE_EQUIP_SET
	0,2,2,2,3,0,0,		//ME_INIT_WAVE_EQUIP_RESET
	0,2,2,2,5,4,0,		//ME_INIT_WAVE_EVERYTIME
};


// Function Declare

int Me_Fsm_Init( int event, int param, char * data, int datalen );
static int Me_Fsm_Init_End( int Error );
static int Me_Fsm_Init_ResetTimer( void );

int Me_GetInitValue( PME_INIT_GETVALUE pMeValue );
static char * MeInit_Filtrate( const char * SrcStr, int SrcLen );
static int MEInit_AnaCREG( char * Content, BYTE *stat );
static int MEInit_AnaCGREG( char * Content, BYTE *stat );
static int MEInit_AnaCOPS( char * Content, char * COPSNum, int Len );
int ME_Search_String( char *SrcStr, char *DestStr, int DestLen );
int ME_Search_integer( char *SrcStr, BYTE * DestInt, BOOL FirstSearch );
int ME_Search_integer_Alpha( char *SrcStr, char * DestInt, int DestLen, BOOL FirstSearch );



/*------------------------------------------------------------------
 *
 * Me_Fsm_Init
 *
 *------------------------------------------------------------------*/

int Me_Fsm_Init ( int event, int param, char * data, int datalen )
{
	static int count=0;

	switch( Me_InitFsm_ExecTable[request_head->cmd][me_fsm_data.state] )
	{

	case ME_EXECUTE_START:

		//Reset Timer
		Me_Fsm_Init_ResetTimer();

		WAT_CurCommand = WAT_STRING_GRP[request_head->cmd];
		count = 0;

		switch( event )
		{
		case START:
			if( -1 == MAT_Send( 0, WAT_CurCommand[count], strlen(WAT_CurCommand[count]) ) )
			{
				Me_Fsm_Init_End( ME_RS_FAILURE );
				break;
			}

			TimerId = SetTimer( NULL, MEINIT_TIMER_ID, DFT_TIMEOUT, ME_TimerProc );
			me_fsm_data.state = ME_FSMINIT_WTRSPNS_1;
			break;

		case DATAIN:
			break;

		case TIMEOUT:
		case USERSTOP:
			Me_Fsm_Init_End( ME_RS_ERRORSTATUS );
			break;
		}
		break;

	case ME_EXECUTE_TEST:
		switch( event )
		{
		case DATAIN:
			if( param == FRC_OK || param == FRC_ERROR || param == FRC_CMEERROR || param == FRC_CMSERROR )
				KillTimer(NULL, TimerId);

			if ( param == FRC_OK )
			{
				count++;
				if( -1 == MAT_Send( 0, WAT_CurCommand[count], strlen(WAT_CurCommand[count]) ) )
				{
					Me_Fsm_Init_End( ME_RS_FAILURE );
					break;
				}

				TimerId = SetTimer( NULL, MEINIT_TIMER_ID, DFT_TIMEOUT, ME_TimerProc );

				me_fsm_data.state ++;
				Me_Fsm_Init_ResetTimer();
				break;
			}

			else if( param == FRC_ERROR || param == FRC_CMEERROR || param == FRC_CMSERROR )	// repeat to send
			{
				if( ErrorCount >= MEINIT_MAXCOUNT_ERROR )
				{
					Me_Fsm_Init_End( ME_RS_FAILURE );
					break;
				}

				ErrorCount ++;

				if( -1 == MAT_Send( 0, WAT_CurCommand[count], strlen(WAT_CurCommand[count]) ) )
				{
					Me_Fsm_Init_End( ME_RS_FAILURE );
					break;
				}

				TimerId = SetTimer( NULL, MEINIT_TIMER_ID, DFT_TIMEOUT, ME_TimerProc );
				break;
			}
			break;

		case TIMEOUT:	// repeat to send
			if( TimeoutCount >= MEINIT_MAXCOUNT_TIMEOUT )
			{
				Me_Fsm_Init_End( ME_RS_TIMEOUT );
				break;
			}

			TimeoutCount ++;

			if( -1 == MAT_Send( 0, WAT_CurCommand[count], strlen(WAT_CurCommand[count]) ) )
			{
				Me_Fsm_Init_End( ME_RS_FAILURE );
				break;
			}

			TimerId = SetTimer( NULL, MEINIT_TIMER_ID, DFT_TIMEOUT, ME_TimerProc );
			break;

		case USERSTOP:
			Me_Fsm_Init_End( ME_RS_USERSTOP );
			break;
		}
		break;

	case ME_EXECUTE_COMMAND:
		switch( event )
		{
		case DATAIN:
			if( param == FRC_OK || param == FRC_ERROR || param == FRC_CMEERROR || param == FRC_CMSERROR )
				KillTimer(NULL, TimerId);

			if ( param == FRC_OK )
			{
				count++;
				if( -1 == MAT_Send( 0, WAT_CurCommand[count], strlen(WAT_CurCommand[count]) ) )
				{
					Me_Fsm_Init_End( ME_RS_FAILURE );
					break;
				}

				TimerId = SetTimer( NULL, MEINIT_TIMER_ID, DFT_TIMEOUT, ME_TimerProc );

				me_fsm_data.state ++;
				break;
			}

			else if( param == FRC_ERROR || param == FRC_CMEERROR || param == FRC_CMSERROR )
			{
				Me_Fsm_Init_End( ME_RS_FAILURE );
				break;
			}
			break;

		case TIMEOUT:
			Me_Fsm_Init_End( ME_RS_TIMEOUT );
			break;

		case USERSTOP:
			Me_Fsm_Init_End( ME_RS_USERSTOP );
			break;
		}
		break;

	case ME_EXECUTE_STKACTIVE:
		switch( event )
		{
		case DATAIN:
			if( param == FRC_OK || param == FRC_ERROR || param == FRC_CMEERROR || param == FRC_CMSERROR )
				KillTimer(NULL, TimerId);

			if( param == FRC_OK || param == FRC_ERROR || param == FRC_CMEERROR || param == FRC_CMSERROR )
			{
				count++;
				if( -1 == MAT_Send( 0, WAT_CurCommand[count], strlen(WAT_CurCommand[count]) ) )
				{
					Me_Fsm_Init_End( ME_RS_FAILURE );
					break;
				}

				TimerId = SetTimer( NULL, MEINIT_TIMER_ID, DFT_TIMEOUT, ME_TimerProc );

				me_fsm_data.state ++;
				break;
			}
			break;

		case TIMEOUT:
			Me_Fsm_Init_End( ME_RS_TIMEOUT );
			break;

		case USERSTOP:
			Me_Fsm_Init_End( ME_RS_USERSTOP );
			break;
		}
		break;

	case ME_EXECUTE_LAST:
		switch( event )
		{
		case DATAIN:
			if ( param == FRC_OK )
			{
				Me_Fsm_Init_End( ME_RS_SUCCESS );
				break;
			}

			else if( param == FRC_ERROR || param == FRC_CMEERROR || param == FRC_CMSERROR )
			{
				Me_Fsm_Init_End( ME_RS_FAILURE );
				break;
			}
			break;

		case TIMEOUT:
			Me_Fsm_Init_End( ME_RS_TIMEOUT );
			break;

		case USERSTOP:
			Me_Fsm_Init_End( ME_RS_USERSTOP );
			break;
		}
		break;

	case ME_EXECUTE_GETVALUE:
		switch( event )
		{
		case DATAIN:
			if( request_head->cmd == ME_INIT_WAVE_EQUIP_SET )
			{
				static int res = -1;
				if ( param == FRC_OK || param == FRC_ERROR || param == FRC_CMEERROR || param == FRC_CMSERROR )
				{
					if( res != -1 )
						Me_Fsm_Init_End( res );
					else
						Me_Fsm_Init_End( ME_RS_FAILURE );
				}
				else
				{
					if( strstr( data, "^SCKS: 1,1" ) != NULL 
						|| strstr( data, "^SCKS: 0,1" ) != NULL )
					{
						res = ME_RS_SUCCESS;
					}
					else if( strstr( data, "^SCKS: 1,0" ) != NULL 
						|| strstr( data, "^SCKS: 0,0" ) != NULL )
					{
						res = ME_RS_NOSIMCARD;
					}
				}
				break;
			}

			if ( param == FRC_OK || param == FRC_ERROR || param == FRC_CMEERROR || param == FRC_CMSERROR )
			{
				Me_Fsm_Init_End( ME_RS_SUCCESS );
			}
			else
			{
				// GetInfo from ME
				MeInit_Filtrate( (const char *)data, datalen );
			}
			break;

		case TIMEOUT:
			Me_Fsm_Init_End( ME_RS_TIMEOUT );
			break;

		case USERSTOP:
			Me_Fsm_Init_End( ME_RS_USERSTOP );
			break;
		}
		break;
	}

	return 0;
}

static int Me_Fsm_Init_End( int Error )
{
	KillTimer( NULL, TimerId );
	request_head->error = Error;
	me_fsm_data.state = ME_FSMINIT_COMPLETE;
	if ( request_head->cmd == ME_INIT_WAVE_EVERYTIME )
		GetResultLength( request_head );	
	return 0;
}

static int Me_Fsm_Init_ResetTimer( void )
{
	ErrorCount = 0; 
	TimeoutCount = 0;
	return 0;
}



/*------------------------------------------------------------------
 *
 * Me_GetInitValue
 *
 *------------------------------------------------------------------*/

ME_INIT_GETVALUE ME_Init_Value;

int Me_GetInitValue( PME_INIT_GETVALUE pMeValue )
{
	memcpy( pMeValue, &ME_Init_Value, sizeof( ME_INIT_GETVALUE ) );
	return 0;
}


#define	MEINIT_ATRET_STRLEN		48
static char * MeInit_Filtrate( const char * SrcStr, int SrcLen )
{
	int		i, x=0;
	char *	ptemp;
	char	NotifyMsg[MEINIT_ATRET_STRLEN];
	const char	WAT_QUERY_KEYWORD[4][8] = {	"+CREG:",
											"+CGREG:",
											"+COPS:", "", };		// KeyWord of GetInfo

	for( i =0; i<3; i++ )
	{
		if( (ptemp = strstr( SrcStr, WAT_QUERY_KEYWORD[i] ) ) == NULL )
			continue;

		x = 0;
		while( *(ptemp+x) != 10 && *(ptemp+x) != 13 && *(ptemp+x) != 0 && x < MEINIT_ATRET_STRLEN-1 )
		{
			NotifyMsg[x] = *(ptemp+x);
			x++;
		}
		NotifyMsg[x] = 0;

		// Analyse Info
		if( i == 0 )			// +CREG: <mode>,<stat>
		{
			MEInit_AnaCREG( NotifyMsg, &(ME_Init_Value.CREG) );
		}
		else if( i == 1 )		// +CGREG: <stat>
		{
			MEInit_AnaCGREG( NotifyMsg, &(ME_Init_Value.CGREG) );
		}
		else if( i == 2 )		// +COPS: 0<,2,46000>
		{
			MEInit_AnaCOPS( NotifyMsg, ME_Init_Value.COPS_Numeric, MEINIT_ATRET_COPSLEN );
		}
	}

	return ptemp +x-1;
}


static int MEInit_AnaCREG( char * Content, BYTE *stat )
{
	int offset;
	BYTE temp;

	offset = ME_Search_integer( Content, &temp,TRUE );
	ME_Search_integer( Content+offset, stat, FALSE );
	return 0;
}

static int MEInit_AnaCGREG( char * Content, BYTE *stat )
{
	ME_Search_integer( Content, stat,TRUE );
	return 0;
}

static int MEInit_AnaCOPS( char * Content, char * COPSNum, int Len )
{
	char * AnaContent;
	int offset;
	BYTE temp;

	offset = ME_Search_integer( Content, &temp,TRUE );

	AnaContent = Content+offset;
	offset = ME_Search_integer( AnaContent, &temp,FALSE );

	if( offset == -1 )
	{
		*COPSNum = 0;
		return -1;
	}

	AnaContent += offset;
	ME_Search_String( AnaContent, COPSNum, Len );
	return 0;
}

/*----------------------------------------------------------------
 *
 * FirstSearch:	Search begin :, match ,
 * other:		Search ','or end, calculate indirectly value
 *
 * Source Format :
 * integer		: ---> ',' or '\0'
 *				  ---> ',' or '\0'
 *
 *--------------------------------------------------------------*/

int ME_Search_integer( char *SrcStr, BYTE * DestInt, BOOL FirstSearch )
{
	int i, AnaCount = 0;
	char *pBegin;
	char CodeInt[8];

	if( FirstSearch )
	{
		if( (pBegin =  strstr( SrcStr, ":" )) == NULL )
			return -1;
		pBegin +=1;
	}
	else
		pBegin = SrcStr;

	for( i=0; i< 8; i++ )
	{
		if( *(pBegin+i) == ',' || *(pBegin+i) == 0 )
		{
			// Calculate the size of Analysed Str
			AnaCount = pBegin +i -SrcStr;
			if( *(pBegin+i) == ',' )
				AnaCount += 1;
			break;
		}
		CodeInt[i] = *(pBegin+i);
	}
	if( i >= 8 || i <= 0 )
		return -1;

	CodeInt[i]=0;
	*DestInt = atoi( CodeInt );

	return AnaCount ;
}

/*
int ME_Search_integer_Alpha( char *SrcStr, char * DestInt, int DestLen, BOOL FirstSearch )
{
	int i, AnaCount;
	char *pBegin;
	char CodeInt[MEINIT_ATRET_STRLEN];

	if( FirstSearch )
	{
		if( (pBegin =  strstr( SrcStr, ":" )) == NULL )
			return -1;
		pBegin +=1;
	}
	else
		pBegin = SrcStr;

	for( i=0; i< DestLen-1; i++ )
	{
		if( *(pBegin+i) == ',' || *(pBegin+i) == 0 )
		{
			// Calculate the size of Analysed Str
			AnaCount = pBegin +i -SrcStr;
			if( *(pBegin+i) == ',' )
				AnaCount += 1;
			break;
		}
		CodeInt[i] = *(pBegin+i);
	}
	if( i >= DestLen-1 || i <= 0 )
	{
		DestInt[0] = 0;
		return -1;
	}

	CodeInt[i]=0;
	strcpy( DestInt, CodeInt );

	return AnaCount ;
}
*/

/*----------------------------------------------------------------
 *
 * AllSearch:	Search begin ", match end ",
 * Source Format :
 * string		" ---> ", or "
 *
 *--------------------------------------------------------------*/

int ME_Search_String( char *SrcStr, char *DestStr, int DestLen )
{
	int		AnaCount;
	int		len, cpylen;
	char	*pBegin, *pEnd, *pTemp;
	char	MidStr[MEINIT_ATRET_STRLEN];

	pTemp = SrcStr;

	// Analyse number
	if( (pBegin = strstr( pTemp, "\"" )) == NULL )
		goto Search_String_Error;

	if( (pEnd = strstr( pBegin+1, "\"" )) == NULL )
		goto Search_String_Error;

	len = pEnd - pBegin -1;
	cpylen = ( MEINIT_ATRET_STRLEN > len )? len: (MEINIT_ATRET_STRLEN-1);

	memcpy( MidStr, pBegin+1, cpylen );
	MidStr[cpylen]=0;

	// Calculate the size of Analysed Str
	AnaCount = pEnd - SrcStr +1;
	// filtrate ',' that follows ':'
	if( *(pEnd+1) == ',')
		AnaCount +=1;

	// convert string format, USC2->ASCII
	//STK_Convert_USC22ASCII( MidStr, DestStr, DestLen, STK_ALPHA_80 );
	strncpy( DestStr, MidStr, DestLen );

	return AnaCount;

Search_String_Error:
	DestStr[0] = 0;
	return -1;
}



// End of Program


