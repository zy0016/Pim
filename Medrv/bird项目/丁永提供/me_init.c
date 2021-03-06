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


int ME_FSM_Init (MEREQUEST * req, int event, int param, char * data, int datalen);
int Me_GetInitValue (PME_INIT_GETVALUE pMeValue);
extern void GetResultLength (MEREQUEST* pRequest);

//---------- Extern Variable and Function -------------

extern	REQGLOBAL *	gQueue;
extern	UINT WINAPI ME_TimerProc (HWND, UINT, UINT, DWORD);


//--------------- Init ATCmd String of Every Stadge ----------------

// Control by bWaveSetFlag, only once
const char InitTab1[][128] = {
    "AT&F\r",
	"ATE0V1\r",
    "AT+CMEE=1;+CRC=1\r",
	"AT^SNFPT=0\r",
	"AT^SCKS?\r",
	"",
};

// Control by bWaveReSetFlag, when &F
const char InitTab2[][128] = {
	"AT\\Q3E0Q0V1X4+CRC=1;+CLIP=1;+COPS=3,2;+CREG=1;\r",
	"AT&C1&D0&S0\r",
	"AT+CMEE=1\r",
	"AT&W\r",
	"",
};


// Common At, Init every time
const char InitTab3[][128] = {
    "AT^SCTM=1\r",
    "AT^SSMSS=0\r",
    "AT^SACM=1\r",
	"AT+CMGF=0\r",
    "AT+CSCB=1\r",
    "AT+CSMS=1\r",
    "AT^SSDA=1\r",
	"AT+CGREG=1\r",
    "AT+CREG=1\r",
    "AT+CCWA=1\r",
    "AT+CSSN=1,1\r",
    "AT+CUSD=1\r",
    "AT+COPS=3,2\r",
	"AT^SSTA=1,1\r",
	"AT+COPS?;+CREG?;+CGREG?\r",		// Get query result
	"",
};

void * WAT_STRING_GRP[3] = {
	(void*)InitTab1,
	(void*)InitTab2,
	(void*)InitTab3,
};

// AT Command Group
char (*WAT_CurCommand)[128];


//---------------- Me Init Define ---------------------

#define MEINIT_TIMER_ID				1
#define MEINIT_MAXCOUNT_ERROR		64
#define MEINIT_MAXCOUNT_TIMEOUT		3

static int ErrorCount, TimeoutCount;

// Define FSM Step
#define FSMINIT_START		    0
#define FSMINIT_STEP1	        1
#define FSMINIT_COMPLETE	    20	// Keep same as FSM_STATE_DONE

// Define Execute Type
#define ME_EXECUTE_START		0
#define ME_EXECUTE_TEST			1
#define ME_EXECUTE_COMMAND		2
#define ME_EXECUTE_LAST			3
#define ME_EXECUTE_GETVALUE		4
#define ME_EXECUTE_STKACTIVE	5
#define ME_EXECUTE_ANYWAY       6

// Define FSM Execution
int	Me_InitFsm_ExecTable[3][16] = {
	0,1,1,1,1,4,0,0,0,0,0,0,0,0,0,0,
	0,2,2,2,3,0,0,0,0,0,0,0,0,0,0,0,
	0,2,1,2,1,1,1,1,2,2,2,2,2,2,5,4
};


// Function Declare

int ME_FSM_Init (MEREQUEST * req, int event, int param, char * data, int datalen);
static int FSM_Init_End (MEREQUEST * req, int Error);
static int Me_Fsm_Init_ResetTimer (void);

int Me_GetInitValue (PME_INIT_GETVALUE pMeValue);
static char * MeInit_Filtrate (const char * SrcStr, int SrcLen);
static int MEInit_AnaCREG (char * Content, BYTE *stat);
static int MEInit_AnaCGREG (char * Content, BYTE *stat);
static int MEInit_AnaCOPS (char * Content, char * COPSNum, int Len);
int ME_Search_String (char *SrcStr, char *DestStr, int DestLen);
int ME_Search_integer (char *SrcStr, BYTE * DestInt, BOOL FirstSearch);



/*------------------------------------------------------------------
 *
 * Me_Fsm_Init
 *
 *------------------------------------------------------------------*/

int ME_FSM_Init  (MEREQUEST * req, int event, int param, char * data, int datalen)
{
	static int count=0;
    int cmd = req->cmd - ME_INIT_EQUIP;
    
	switch( Me_InitFsm_ExecTable[cmd][req->stat] )
	{
	case ME_EXECUTE_START:

		//Reset Timer
		Me_Fsm_Init_ResetTimer ();

		WAT_CurCommand = WAT_STRING_GRP[cmd];
		count = 0;

		switch( event )
		{
		case START:
			if( -1 == MAT_Send (req->queue+1, WAT_CurCommand[count], strlen(WAT_CurCommand[count])) )
			{
				FSM_Init_End (req, ME_RS_FAILURE);
				break;
			}

			gQueue[req->queue].TimerID = SetTimer (NULL, MEINIT_TIMER_ID, DFT_TIMEOUT, ME_TimerProc);
			req->stat = FSMINIT_STEP1;
			break;

		case DATAIN:
			break;

		case TIMEOUT:
		case USERSTOP:
			FSM_Init_End (req, ME_RS_ERRORSTATUS);
			break;
		}
		break;

	case ME_EXECUTE_TEST:
		switch( event )
		{
		case DATAIN:
			if( param == FRC_OK || param == FRC_ERROR || param == FRC_CMEERROR || param == FRC_CMSERROR )
				KillTimer(NULL, gQueue[req->queue].TimerID);

			if ( param == FRC_OK )
			{
				count++;
				if ( -1 == MAT_Send (req->queue+1, WAT_CurCommand[count], strlen(WAT_CurCommand[count])))
				{
					FSM_Init_End (req, ME_RS_FAILURE);
					break;
				}

				gQueue[req->queue].TimerID = SetTimer (NULL, MEINIT_TIMER_ID, DFT_TIMEOUT, ME_TimerProc);

				req->stat ++;
				Me_Fsm_Init_ResetTimer();
				break;
			}

			else if( param == FRC_ERROR || param == FRC_CMEERROR || param == FRC_CMSERROR )	// repeat to send
			{
				if( ErrorCount >= MEINIT_MAXCOUNT_ERROR )
				{
					FSM_Init_End (req, ME_RS_FAILURE);
					break;
				}

				ErrorCount ++;

				if( -1 == MAT_Send (req->queue+1, WAT_CurCommand[count], strlen(WAT_CurCommand[count])) )
				{
					FSM_Init_End (req, ME_RS_FAILURE);
					break;
				}

				gQueue[req->queue].TimerID = SetTimer (NULL, MEINIT_TIMER_ID, DFT_TIMEOUT, ME_TimerProc);
				break;
			}
			break;

		case TIMEOUT:	// repeat to send
			if( TimeoutCount >= MEINIT_MAXCOUNT_TIMEOUT )
			{
				FSM_Init_End (req, ME_RS_TIMEOUT);
				break;
			}

			TimeoutCount ++;

			if( -1 == MAT_Send (req->queue+1, WAT_CurCommand[count], strlen(WAT_CurCommand[count])) )
			{
				FSM_Init_End (req, ME_RS_FAILURE);
				break;
			}

			gQueue[req->queue].TimerID = SetTimer (NULL, MEINIT_TIMER_ID, DFT_TIMEOUT, ME_TimerProc);
			break;

		case USERSTOP:
			FSM_Init_End (req, ME_RS_USERSTOP);
			break;
		}
		break;

	case ME_EXECUTE_COMMAND:
		switch( event )
		{
		case DATAIN:
			if( param == FRC_OK || param == FRC_ERROR || param == FRC_CMEERROR || param == FRC_CMSERROR )
				KillTimer(NULL, gQueue[req->queue].TimerID);

			if ( param == FRC_OK )
			{
				count++;
				if( -1 == MAT_Send (req->queue+1, WAT_CurCommand[count], strlen(WAT_CurCommand[count])) )
				{
					FSM_Init_End (req, ME_RS_FAILURE);
					break;
				}

				gQueue[req->queue].TimerID = SetTimer (NULL, MEINIT_TIMER_ID, DFT_TIMEOUT, ME_TimerProc);

				req->stat ++;
				break;
			}

			else if( param == FRC_ERROR || param == FRC_CMEERROR || param == FRC_CMSERROR )
			{
				FSM_Init_End (req, ME_RS_FAILURE);
				break;
			}
			break;

		case TIMEOUT:
			FSM_Init_End (req, ME_RS_TIMEOUT);
			break;

		case USERSTOP:
			FSM_Init_End (req, ME_RS_USERSTOP);
			break;
		}
		break;

	case ME_EXECUTE_STKACTIVE:
		switch( event )
		{
		case DATAIN:
			if( param == FRC_OK || param == FRC_ERROR || param == FRC_CMEERROR || param == FRC_CMSERROR )
				KillTimer(NULL, gQueue[req->queue].TimerID);

			if( param == FRC_OK || param == FRC_ERROR || param == FRC_CMEERROR || param == FRC_CMSERROR )
			{
				count++;
				if( -1 == MAT_Send (req->queue+1, WAT_CurCommand[count], strlen(WAT_CurCommand[count])) )
				{
					FSM_Init_End (req, ME_RS_FAILURE);
					break;
				}

				gQueue[req->queue].TimerID = SetTimer (NULL, MEINIT_TIMER_ID, DFT_TIMEOUT, ME_TimerProc);

				req->stat ++;
				break;
			}
			break;

		case TIMEOUT:
			FSM_Init_End (req, ME_RS_TIMEOUT);
			break;

		case USERSTOP:
			FSM_Init_End (req, ME_RS_USERSTOP);
			break;
		}
		break;

	case ME_EXECUTE_ANYWAY:
		switch( event )
		{
		case DATAIN:
			if( param == FRC_OK || param == FRC_ERROR || param == FRC_CMEERROR )
			{
				KillTimer(NULL, gQueue[req->queue].TimerID);

				count++;
				if( -1 == MAT_Send (req->queue+1, WAT_CurCommand[count], strlen(WAT_CurCommand[count])) )
				{
					FSM_Init_End (req, ME_RS_FAILURE);
					break;
				}

				gQueue[req->queue].TimerID = SetTimer (NULL, MEINIT_TIMER_ID, DFT_TIMEOUT, ME_TimerProc);

				req->stat ++;
			}
			break;

		case TIMEOUT:	// repeat to send
			FSM_Init_End (req, ME_RS_TIMEOUT);
			break;

		case USERSTOP:
			FSM_Init_End (req, ME_RS_USERSTOP);
			break;
		}
		break;

	case ME_EXECUTE_LAST:
		switch( event )
		{
		case DATAIN:
			if ( param == FRC_OK )
			{
				FSM_Init_End (req, ME_RS_SUCCESS);
				break;
			}

			else if( param == FRC_ERROR || param == FRC_CMEERROR || param == FRC_CMSERROR )
			{
				FSM_Init_End (req, ME_RS_FAILURE);
				break;
			}
			break;

		case TIMEOUT:
			FSM_Init_End (req, ME_RS_TIMEOUT);
			break;

		case USERSTOP:
			FSM_Init_End (req, ME_RS_USERSTOP);
			break;
		}
		break;

	case ME_EXECUTE_GETVALUE:
		switch( event )
		{
		case DATAIN:
			if ( req->cmd == ME_INIT_EQUIP )
			{
				static int res = -1;
				if ( param == FRC_OK || param == FRC_ERROR || param == FRC_CMEERROR || param == FRC_CMSERROR )
				{
					if( res != -1 )
						FSM_Init_End (req, res);
					else
						FSM_Init_End (req, ME_RS_FAILURE);
				}
				else
				{
					if( strstr (data, "^SCKS: 1,1") != NULL 
						|| strstr (data, "^SCKS: 0,1") != NULL )
					{
						res = ME_RS_SUCCESS;
					}
					else if( strstr (data, "^SCKS: 1,0") != NULL 
						|| strstr (data, "^SCKS: 0,0") != NULL )
					{
						res = ME_RS_NOSIMCARD;
					}
				}
				break;
			}

			if ( param == FRC_OK || param == FRC_ERROR || param == FRC_CMEERROR || param == FRC_CMSERROR )
			{
				FSM_Init_End (req, ME_RS_SUCCESS);
			}
			else
			{
				// GetInfo from ME
				MeInit_Filtrate ((const char *)data, datalen);
			}
			break;

		case TIMEOUT:
			FSM_Init_End (req, ME_RS_TIMEOUT);
			break;

		case USERSTOP:
			FSM_Init_End (req, ME_RS_USERSTOP);
			break;
		}
		break;
	}

	return 0;
}

static int FSM_Init_End (MEREQUEST * req, int Error)
{
	KillTimer (NULL, gQueue[req->queue].TimerID);
	req->error = Error;
	req->stat = FSMINIT_COMPLETE;
	if ( req->cmd == ME_INIT_EVERYTIME )
		GetResultLength (req);	
	return 0;
}

static int Me_Fsm_Init_ResetTimer (void)
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

int Me_GetInitValue (PME_INIT_GETVALUE pMeValue)
{
	memcpy (pMeValue, &ME_Init_Value, sizeof (ME_INIT_GETVALUE));
	return 0;
}


#define	MEINIT_ATRET_STRLEN		48
static char * MeInit_Filtrate (const char * SrcStr, int SrcLen)
{
	int		i, x = 0;
	char *	ptemp;
	char	NotifyMsg[MEINIT_ATRET_STRLEN];
	const char	WAT_QUERY_KEYWORD[4][8] = {	"+CREG:",
											"+CGREG:",
											"+COPS:", "", };		// KeyWord of GetInfo

	for ( i = 0; i < 3; i++ )
	{
		if( (ptemp = strstr (SrcStr, WAT_QUERY_KEYWORD[i]) ) == NULL )
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
			MEInit_AnaCREG (NotifyMsg, &(ME_Init_Value.CREG) );
		}
		else if( i == 1 )		// +CGREG: <stat>
		{
			MEInit_AnaCGREG (NotifyMsg, &(ME_Init_Value.CGREG));
		}
		else if( i == 2 )		// +COPS: 0<,2,46000>
		{
			MEInit_AnaCOPS (NotifyMsg, ME_Init_Value.COPS_Numeric, MEINIT_ATRET_COPSLEN);
		}
	}

	return ptemp + x - 1;
}


static int MEInit_AnaCREG (char * Content, BYTE *stat)
{
	int offset;
	BYTE temp;

	offset = ME_Search_integer (Content, &temp,TRUE);
	ME_Search_integer (Content+offset, stat, FALSE);
	return 0;
}

static int MEInit_AnaCGREG (char * Content, BYTE *stat)
{
	ME_Search_integer (Content, stat,TRUE);
	return 0;
}

static int MEInit_AnaCOPS (char * Content, char * COPSNum, int Len)
{
	char * AnaContent = NULL;
	int offset;
	BYTE temp;

	offset = ME_Search_integer (Content, &temp,TRUE);

	AnaContent = Content+offset;
	offset = ME_Search_integer (AnaContent, &temp,FALSE);

	if( offset == -1 )
	{
		*COPSNum = 0;
		return -1;
	}

	AnaContent += offset;
	ME_Search_String (AnaContent, COPSNum, Len);
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

int ME_Search_integer (char *SrcStr, BYTE * DestInt, BOOL FirstSearch)
{
	int i, AnaCount = 0;
	char *pBegin;
	char CodeInt[8];

	if( FirstSearch )
	{
		if( (pBegin =  strstr( SrcStr, ":")) == NULL )
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
	*DestInt = atoi (CodeInt);

	return AnaCount ;
}


/*----------------------------------------------------------------
 *
 * AllSearch:	Search begin ", match end ",
 * Source Format :
 * string		" ---> ", or "
 *
 *--------------------------------------------------------------*/

int ME_Search_String (char *SrcStr, char *DestStr, int DestLen)
{
	int		AnaCount;
	int		len, cpylen;
	char	*pBegin, *pEnd, *pTemp;
	char	MidStr[MEINIT_ATRET_STRLEN];

	pTemp = SrcStr;

	// Analyse number
	if( (pBegin = strstr (pTemp, "\"")) == NULL )
		goto Search_String_Error;

	if( (pEnd = strstr (pBegin+1, "\"")) == NULL )
		goto Search_String_Error;

	len = pEnd - pBegin -1;
	cpylen = ( MEINIT_ATRET_STRLEN > len )? len: (MEINIT_ATRET_STRLEN-1);

	memcpy (MidStr, pBegin+1, cpylen);
	MidStr[cpylen]=0;

	// Calculate the size of Analysed Str
	AnaCount = pEnd - SrcStr +1;
	// filtrate ',' that follows ':'
	if( *(pEnd+1) == ',')
		AnaCount +=1;

	// convert string format, USC2->ASCII
	//STK_Convert_USC22ASCII( MidStr, DestStr, DestLen, STK_ALPHA_80 );
	strncpy (DestStr, MidStr, DestLen);

	return AnaCount;

Search_String_Error:
	DestStr[0] = 0;
	return -1;
}



// End of Program


