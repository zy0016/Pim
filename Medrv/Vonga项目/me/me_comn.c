#include "stdlib.h"
#include "malloc.h"
#include "window.h"
#include "string.h"
#include "me_func.h"
#include "me_wnd.h"
#include "me_comn.h"
#include "smspdu.h"

#ifndef NULL
#define NULL	(void *)0
#endif

extern void debugex_nonl(char *fmt, ...);
#define	RESULT_BUFSIZE	    1000
static char me_result_buf[RESULT_BUFSIZE];

typedef struct tagME_MemBlock
{
	char*	p;
	int		len;
	int		offset;

}ME_MemBlock, *PME_MemBlock;

ME_MemBlock	ME_RcvBuf;

//extern void *realloc(void *memblock, size_t newsize);
/*********************************************************************\
* Function
* Purpose
* Params
* Return
* Remarks
**********************************************************************/
void ME_MemInitial( void )
{
	if ( ME_RcvBuf.len > RESULT_BUFSIZE )
		free( ME_RcvBuf.p );

	ME_RcvBuf.p = me_result_buf;
	memset( ME_RcvBuf.p, 0, RESULT_BUFSIZE );
	ME_RcvBuf.len = RESULT_BUFSIZE;
	ME_RcvBuf.offset = 0;

	return;
}

/*********************************************************************\
* Function
* Purpose
* Params
* Return
* Remarks
**********************************************************************/
BOOL ME_Malloc( int len )
{
	char* current;

#if defined VME_DEBUG
    debugex_nonl( "\r\nME malloc memory: %d", len );
#endif
	if ( ME_RcvBuf.len == RESULT_BUFSIZE )
	{
		current = ME_RcvBuf.p;

		ME_RcvBuf.p = malloc( len );

		if ( NULL == ME_RcvBuf.p )
		{
			ME_RcvBuf.p = current;
			return FALSE;
		}

		memset( ME_RcvBuf.p, 0, len );
		memcpy( ME_RcvBuf.p, current, ME_RcvBuf.offset );
		memset( current, 0, RESULT_BUFSIZE );
	}
	else
	{
		current = ME_RcvBuf.p;

		ME_RcvBuf.p = realloc( ME_RcvBuf.p, len );

		if ( NULL == ME_RcvBuf.p )
		{
			ME_RcvBuf.p = current;
			return FALSE;
		}
		memset( &ME_RcvBuf.p[ME_RcvBuf.len], 0, len-ME_RcvBuf.len );
	}

	ME_RcvBuf.len = len;

	return TRUE;
}

/*********************************************************************\
* Function
* Purpose
* Params
* Return
* Remarks
**********************************************************************/
int ME_MemAssign( char* data, int datalen )
{
#if defined VME_DEBUG
    debugex_nonl( "\r\nME Memory offset = %d", ME_RcvBuf.offset );
    debugex_nonl( "\r\nME Memory datalen = %d", datalen );
    debugex_nonl( "\r\nME Memory len = %d", ME_RcvBuf.len );
#endif
    if ( ME_RcvBuf.offset + datalen >=  ME_RcvBuf.len )
	{
#if defined VME_DEBUG
        debugex_nonl( "\r\nME request new memory: %d", 
            ME_RcvBuf.len + RESULT_BUFSIZE );
#endif
		if ( !ME_Malloc( ME_RcvBuf.len + RESULT_BUFSIZE ) )
			return -1;
	}

#if defined VME_DEBUG
    debugex_nonl( "\r\ncall memcpy in ME_MemAssign begin" );
#endif
	memcpy( &ME_RcvBuf.p[ME_RcvBuf.offset], data, datalen );
#if defined VME_DEBUG
    debugex_nonl( "\r\ncall memcpy in ME_MemAssign end" );
#endif

	ME_RcvBuf.offset += datalen;
#if defined VME_DEBUG
    debugex_nonl( "\r\nleave ME_MemAssign" );
#endif

	return 0;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
char* ME_MemGetPntr(void)
{
	return ME_RcvBuf.p;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int ME_MemGetOffset(void)
{
	return ME_RcvBuf.offset;
}

/***************************************************************
* Purpose: 从给定字符串中得到指定的整型值
* 
* Parameter:
*	pSrc:		给定字符串
*	pnResult:	得到的整型值
*	Identf:		验证给定字符串是否合法的标识符，如"+CPBR:"
*	index:		需要得到的变量索引，字符串型变量不参与计算，索引从1开始
*  
* Return Value:
*	!=-1:  解析成功
*	-1: 解析失败
*****************************************************************/
int ME_GetIntSection( char* pSrc, char* Identf, int index )
{
	char* pBegin = pSrc;
	char *pEnd = NULL;
	char pResult[20];
	int nResult = 0;
	int nSectionNum = 0;
	int nSecLen;

	if (strlen(Identf) > 0 )
	{
		pBegin = strstr(pSrc, Identf);
		if ( pBegin == NULL )
			return -1;
		pBegin += (strlen(Identf));
	}

	while ( (pEnd = strchr(pBegin, ',')) != NULL )
	{
		while (*pBegin == ' ')
			pBegin++;
		if ((pEnd == pBegin))
		{
			pBegin++;
			continue;
		}
		if (*pBegin == '"')
		{
			pBegin = pEnd+1;
			continue;
		}
		nSectionNum++;
		if (nSectionNum == index)
		{
			nSecLen = pEnd-pBegin;
			memcpy(pResult, pBegin, nSecLen);
			pResult[nSecLen] = '\0';
			nResult = atoi(pResult);
			return nResult;
		}
		pBegin = pEnd+1;
	}

	if (strlen(pBegin) > 0 )
	{
		while (*pBegin == ' ')
			pBegin++;

		if ( *pBegin != '"' && nSectionNum == index-1)
		{
			nSectionNum++;
			nSecLen = strlen(pBegin);
			memcpy(pResult, pBegin, nSecLen);
			pResult[nSecLen] = '\0';
			nResult = atoi(pResult);
			
			return (nResult);
		}
	}

	return -1;
}

/***************************************************************
* Purpose: 从给定字符串中得到指定的字符串
* 
* Parameter:
*	pSrc:		给定字符串
*	pResult:	得到的字符串
*	Identf:		验证给定字符串是否合法的标识符，如"+CPBR:"
*	index:		需要得到的变量索引，整型变量不参与计算，索引从1开始
*  
* Return Value:
*	TRUE:  解析成功
*	FALSE: 解析失败
*****************************************************************/
int ME_GetStrSection(char* pSrc, char* pResult, int nLen, char* Identf, int index)
{
	char* pBegin = pSrc;
	char* pEnd = NULL;
	char *pFirstQuota, *pSecondQuota;
	int nSectionNum = 0;
	int nSecLen;

	if (strlen(Identf) > 0 )
	{
		pBegin = strstr(pSrc, Identf);
		if (!pBegin)
			return -1;
		pBegin += (strlen(Identf));
	}

	pFirstQuota = strchr(pBegin, '"');
	if (!pFirstQuota)
		return -1;
	pSecondQuota = strchr(pFirstQuota+1, '"');
	if (!pSecondQuota)
		return -1;

	while ( (pEnd=strchr(pBegin, ',')) != NULL )
	{
		if (pEnd < pSecondQuota)
		{
			pBegin = pEnd+1;
			continue;
		}
		if ((pEnd == pBegin))
		{
			pBegin++;
			continue;
		}

		pBegin = pEnd+1;
		nSectionNum++;
		if (nSectionNum == index)
		{
			nSecLen = pSecondQuota-pFirstQuota-1;
			if (nLen < nSecLen +1)
				return -1;
			if (nSecLen < 0)
			{
				memset(pResult, 0, nLen);
				return -1;
			}
			else
			{
				memcpy(pResult, pFirstQuota+1, nSecLen);
				pResult[nSecLen] = 0;
				return nSecLen;
			}
		}
		pFirstQuota = strchr(pBegin, '"');
		if (!pFirstQuota)
			break;
		pSecondQuota = strchr(pFirstQuota+1, '"');
		if (!pSecondQuota)
			break;
	}

	if (pFirstQuota && pSecondQuota && nSectionNum == index-1)
	{
		nSecLen = pSecondQuota-pFirstQuota-1;

		if (nLen < nSecLen +1)
			return -1;

		if (nSecLen < 0)
		{
			memset(pResult, 0, nLen);
			return -1;
		}
		else
		{
			memcpy(pResult, pFirstQuota+1, nSecLen);
			pResult[nSecLen] = 0;
			return nSecLen;
		}
	}

	return -1;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int ME_CurCalls_Dec( char *Scr, PCALLS_INFO pCallInfo )
{
	//+CLCC: 1,0,0,0,0,"01062629970",129,"01062629970+507"
	//+CLCC: 2,1,5,0,0,"13641190277",161

	char	*pBegin, *pEnd, *pSpe;
	int		len;
	char	TempText[100];
	char	pTemp[4];

	if (strstr(Scr, "+CLCC: ") != Scr)
		return -1;

	pBegin	= Scr + strlen( "+CLCC: ");
	pEnd	= strstr( pBegin, "," );
	len = pEnd - pBegin;

	memcpy( pTemp, pBegin, len );
	pTemp[len] = 0;
	pCallInfo->index = atoi(pTemp);

	pBegin	= strstr( Scr, "," );		// dir
	pCallInfo->Dir = (int)(*(pBegin+1));

	pBegin	= strstr( pBegin+1, "," );	// stat
	pCallInfo->Stat = (int)(*(pBegin+1));

	pBegin	= strstr( pBegin+1, "," );	// mode
	pCallInfo->Mode = (int)(*(pBegin+1));

	pBegin	= strstr( pBegin+1, "," );	// mpty
	pCallInfo->Mpty = (int)(*(pBegin+1));

	pBegin	= strstr( pBegin, "\"" );	// "telephone_number"
	pEnd	= strstr( pBegin+1, "\"" );
	len = pEnd - pBegin;
	memcpy( pCallInfo->PhoneNum, pBegin+1, len-1 );
	pCallInfo->PhoneNum[len-1]=0;

	pBegin = strstr( pEnd+1, "\"" );
	if( pBegin == NULL )
	{
		strcpy( pCallInfo->Text, "" );
		return 0;
	}

	while( NULL != (pSpe =strstr( pEnd+1, "\"" )) )
		pEnd = pSpe;

	if (pEnd != pBegin)
		len = pEnd - pBegin;
	else
		len = strlen(pBegin);
	memcpy( TempText, pBegin+1, len-1 );
	TempText[len-1] = 0;

	PhoneText_Decode( pCallInfo->Text, ME_PHONETXT_LEN, TempText );

	return 0;
}


/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int ME_PhoneBook_Dec( char *Scr, PPHONEBOOK pDesPB )
{
	//+CPBR: 4,"+32498",145,"dsafa"
	char	*pBegin, *pEnd, *pSpe;
	int		len;
	char	TempText[100];
	char	PDUText[100];
	char	WideText[100];

	if (strstr(Scr, "+CPBR: ") != Scr && strstr(Scr, "+CPBP: ") != Scr
		&& strstr(Scr, "+CPBF: ") != Scr && strstr(Scr, "+CPBN: ") != Scr)
		return -1;

	memset(TempText, 0, 100);
	memset(PDUText, 0, 100);
	memset(WideText, 0, 100);

	pBegin	= Scr + strlen( "+CPBR: ");
	pEnd	= strstr( pBegin, "," );
	len = pEnd - pBegin;

	*pEnd = '\0';

	pDesPB->Index = atoi( pBegin );

	pBegin	= strstr( pEnd+1, "\"" );
	pEnd	= strstr( pBegin+1, "\"" );
	len = pEnd - pBegin;
	memcpy( pDesPB->PhoneNum, pBegin+1, len-1 );
	pDesPB->PhoneNum[len-1]=0;

	pBegin	= strstr( pEnd+1, "\"" );
	while( NULL != (pSpe =strstr( pEnd+1, "\"" )) )
		pEnd = pSpe;

	if (pEnd != pBegin)
		len = pEnd - pBegin;
	else
		len = strlen(pBegin);
	memcpy(TempText, pBegin+1, len-1);
	TempText[len-1] = 0;

	PhoneText_Decode( pDesPB->Text, ME_PHONETXT_LEN, TempText );

	return 0;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int ME_NetOpratorDec( NETOPRATOR* pNetOp, char* pBuf, int BufCount )
{
	char Section[100];
	char *pBegin, *pEnd;
	int nCount;

	if ( BufCount <= 0 )
		return 0;

	nCount = 0;

	pBegin = strchr(pBuf, '(');

	if (pBegin != NULL )
	{
		while ( (pEnd = strchr(pBegin, ')')) != NULL )
		{
			memcpy(Section, pBegin, pEnd-pBegin+1);

			Section[pEnd-pBegin+1] = 0;
			if ( ME_GetStrSection(Section, pNetOp[nCount].alpha_oper, ME_OPRATOR_LEN, "(", 1 ) == -1 )
				break;

			ME_GetStrSection(Section, pNetOp[nCount].num_oper, ME_OPRATOR_LEN, "(", 2);
			pNetOp[nCount].stat = ME_GetIntSection(Section, "(", 1);

			nCount++;

			if ( nCount >= BufCount )
				return nCount;

			pBegin = strchr(pEnd, '(');

			if (pBegin == NULL)
				break;
		}
	}

	return nCount;
}

void PhoneText_Decode( char* pDst, int nDstLen, char* pSrc )
{
	char PDUStr[100], WideStr[100];

	memset( PDUStr, 0, 100 );
	memset( WideStr, 0, 100 );

/*	if ( Analyse_UCS2PDUCode(PDUStr, pSrc, 100) < 0 )
	{
		strcpy( pDst, pSrc );
		return;
	}*/
	Analyse_PDUCode2WideChar(  WideStr,pSrc, 100 );
	WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)WideStr, -1,
			(LPSTR)pDst, nDstLen, 0, 0);

	return;
}

/*********************************************************************\
* Function
* Purpose
* Params
* Return
* Remarks
**********************************************************************/
int PhoneText_Encode( char* pDesStr, char* pSrcStr, unsigned int DestLen )
{
	char WideText[100];
	char PDUText[100];
	int len;
	unsigned int i = 0;

	while ( pSrcStr[i] != '\0' )
	{
		if ( (BYTE)(pSrcStr[i]) >= 0xA0 )
			break;

		i++;
	}

	if ( i == strlen(pSrcStr) )
	{
		if ( strlen(pSrcStr) >= DestLen )
		{
			strcpy( pDesStr, "" );
			return 0;
		}
		strcpy( pDesStr, pSrcStr );
		return 0;
	}
	else
	{
		len = MultiByteToWideChar( CP_ACP, 0, pSrcStr, strlen(pSrcStr), (LPWSTR)WideText, 100 );

		Analyse_WideChar2PDUCode( PDUText, WideText, 100, len );

		if ( strlen( PDUText ) >= DestLen )
		{
			strcpy( pDesStr, "" );
			return 0;
		}
//		strcpy( pDesStr, "80" );
//		strcat( pDesStr, PDUText );
		strcpy( pDesStr, PDUText );
		return 1;
	}
}

int ServCell_Decode( PSCELL_INFO pDest, char* pSource, int Status )
{
	char* p, *q;
	char* token[20];
	int i = 0;

	if ( Status == ME_RS_SEARCHING )
		return -1;

	q = strstr( pSource, "^SMONC:" );
	p = strchr( pSource, '\r');

	if ( p == NULL )
		return -1;

	p += 2;

	if ( p == NULL )
		return -1;

	token[0] = strtok( p, " " );

	while ( token[i] != NULL && i < 20 )
	{
		i++;
		token[i] = strtok( NULL, " " );
	}

	strcpy( pDest->BCH, token[0] );
	strcpy( pDest->RSSI, token[1] );
	strcpy( pDest->BRXL, token[2] );
	strcpy( pDest->MCC, token[3] );
	strcpy( pDest->MNC, token[4] );
	strcpy( pDest->LAC, token[5] );

	strcpy( pDest->CELLID, token[6] );
	strcpy( pDest->NCC, token[7] );
	strcpy( pDest->BCC, token[8] );
	strcpy( pDest->PWR, token[9] );
	strcpy( pDest->RXLev, token[10] );
	strcpy( pDest->C1, token[11] );
	
	
	if ( Status == ME_RS_SUCCESS )
	{
		strcpy( pDest->TCH, token[13] );
		strcpy( pDest->TS, token[14] );
		strcpy( pDest->TA, token[15] );
		strcpy( pDest->PWRLev, token[16] );
		strcpy( pDest->TRXL, token[17] );
		strcpy( pDest->Q, token[18] );
		strcpy( pDest->Chmod, token[19] );
	}

	q += strlen( "^SMONC:" )+1;

	p = q;

	token[0] = strtok( p, "," );
	i = 0;

	while ( token[i] != NULL && i < 9 )
	{
		i++;
		token[i] = strtok( NULL, "," );
	}

	strcpy( pDest->BSIC, token[4] );
	strcpy( pDest->C2, token[8] );

	return sizeof( SCELL_INFO );
}

int NeibCell_Decode( PNCELL_INFO pDest, char* pSource )
{
	char* p;
	char* token[8];
	int i = 0, j = 0;

	p = strchr( pSource, '\r');

	if ( p == NULL )
		return -1;

	p += 2;

	if ( p == NULL )
		return -1;

	token[0] = strtok( p, " \r\n" );
	while ( j < 6 )
	{
		if ( j > 0 )
			token[0] = strtok( NULL, " \r\n" );
		i = 0;

		while ( token[i] != NULL && i < 7 )
		{
			i++;
			token[i] = strtok( NULL, " \r\n" );
		}

		strcpy( pDest[j].BCH, token[0] );
		strcpy( pDest[j].BRXL, token[2] );
		strcpy( pDest[j].C1, token[6] );
		strcpy( pDest[j].C2, token[7] );
		j++;
	}

	return 6*sizeof( NCELL_INFO );
}