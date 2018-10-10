/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 2000-2001 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :
 *
 * Filename:	SmsPDU.c
 *
 * Purpose :
 *
 * Author  :	Ding Yong
 *
\**************************************************************************/

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "malloc.h"
#include "window.h"
#include "me_func.h"
#include "me_wnd.h"
#include "me_comn.h"
#include "smspdu.h"

extern void debugex_nonl(char *fmt, ...);


/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
/*------------------------------------------------------
 *
 * Code Transmitted Context to PDU String
 * "0891683108100005F011000D91683146110972F70008000A4E2D658777ED6D88606F"
 *
 *----------------------------------------------------*/
int	ME_TranStr_Code( PSMS_SEND pSMS_Send, char* pDesStr, int Length )
{
	int		len, offset;
	int		SCALength, SMSLength;
	char	PhoneNum[ME_PHONENUM_LEN],TempPhoneNum[ME_PHONENUM_LEN];
	char	Context[ME_SMS_PDU_LEN],TempContext[ME_SMS_PDU_LEN];
	char	ContextLen[ME_CODELEN_STRLEN];
	char	EndCode[4];
	char	VP[4];

	switch ( pSMS_Send->VPvalue )
	{
	case SMS_1HOUR:
		pSMS_Send->VPvalue = 11;
		break;

	case SMS_6HOUR:
		pSMS_Send->VPvalue = 71;
		break;

	case SMS_24HOUR:
		pSMS_Send->VPvalue = 167;
		break;

	case SMS_1WEEK:
		pSMS_Send->VPvalue = 173;
		break;

	case SMS_MAXTIME:
		pSMS_Send->VPvalue = 255;
		break;

	default:
		return -1;
	}

	memset( pDesStr, 0, Length );

	// Create PDU "SCA"
	if( pSMS_Send->SCA[0] == '+' )
	{
		strcpy( TempPhoneNum, "91" );
		offset = 1;
	}
	else
	{
		strcpy( TempPhoneNum, "A1" );
		offset = 0;
	}

	Analyse_PhoneNum( (pSMS_Send->SCA)+offset, TempPhoneNum+2, ANALYSE_CODE );
	SCALength = strlen( TempPhoneNum )/2;

//	sprintf( PhoneNum, "%02x", SCALength );	// SCA Length
	strcpy( PhoneNum, "08");
	strcat( PhoneNum, TempPhoneNum );		// SCA PhoneNum
	strcpy( pDesStr, PhoneNum );


	// SMS Param
	if ( !pSMS_Send->ReplyPath)
	{
		if ( pSMS_Send->Report )				//Status Report
			strcat( pDesStr, "3100" );
		else
			strcat( pDesStr, "1100" );
	}
	else
	{
		if ( pSMS_Send->Report )				//Status Report
			strcat( pDesStr, "B100" );
		else
			strcat( pDesStr, "9100" );
	}

	// PhoneNum
	if( pSMS_Send->PhoneNum[0] == '+' )
	{
		strcpy( TempPhoneNum, "91" );
		offset = 1;
	}
	else
	{
		strcpy( TempPhoneNum, "A1" );
		offset = 0;
	}

	Analyse_PhoneNum( (pSMS_Send->PhoneNum)+offset, TempPhoneNum+2, ANALYSE_CODE );
	len = strlen( (pSMS_Send->PhoneNum)+offset );

//	sprintf( PhoneNum, "%02x", len );	// PhoneNum Length
	strcpy( PhoneNum, "0B");
	strcat( PhoneNum, TempPhoneNum );	// PhoneNum PhoneNum
	strcat( pDesStr, PhoneNum );

	// some settings
	switch ( pSMS_Send->PID )	//PID
	{
	case PID_TELEX:
		strcat( pDesStr, "21" );
		break;
	case PID_TELEFAX:
		strcat( pDesStr, "22" );
		break;
	case PID_VOICE:
		strcat( pDesStr, "24" );
		break;
	case PID_ERMES:
		strcat( pDesStr, "25" );
		break;
	case PID_NPAGING:
		strcat( pDesStr, "26" );
		break;
	case PID_X400:
		strcat( pDesStr, "31" );
		break;
	case PID_EMAIL:
		strcat( pDesStr, "32" );
		break;
	default:
		strcat( pDesStr, "00" );
		break;
	}
	len = pSMS_Send->ConLen;//strlen( pSMS_Send->Context );

	if ( len > 70 )
		strcat( pDesStr, "00" );
	else
		strcat( pDesStr, "08" );

	DecNum2HexStr( pSMS_Send->VPvalue, VP);
	strcat( pDesStr, VP);

	memset( TempContext, 0, ME_SMS_PDU_LEN );
	memset( Context, 0, ME_SMS_PDU_LEN );

	// Send Context length
	if ( len > 70 || pSMS_Send->Type == DATA_SMS )
	{
//		sprintf( ContextLen, "%02x", len*2 );
		strcpy( ContextLen, "C8");
		strcat( pDesStr, ContextLen );					// Context Length

		ASCIIstr2GSMstr( pSMS_Send->Context, TempContext );
		BinStr2String( TempContext, Context, strlen(TempContext) );
		strcat( pDesStr, Context );						// Context
	}
	else
	{
		len = MultiByteToWideChar( CP_ACP, 0, pSMS_Send->Context, strlen(pSMS_Send->Context),
						(LPWSTR)TempContext, ME_SMS_PDU_LEN );

		sprintf( ContextLen, "%02x", len*2 );
		strcat( pDesStr, ContextLen );					// Context Length

		Analyse_WideChar2PDUCode(  Context, TempContext, ME_SMS_PDU_LEN, len );
		strcat( pDesStr, Context );						// Context
	}

	SMSLength = strlen( pDesStr )/2 - SCALength-1;	// Calculate SMS Length

	EndCode[0]=0x1A;	EndCode[1]=0;
	strcat( pDesStr,EndCode );						// Send Command End_Code

	strupr( pDesStr );

	return SMSLength;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
/*------------------------------------------------------
 *
 * PDU String to Code Transmitted Context
 * +CMGR: 1,,38
 * "0891683108100005F0040D91683108317090F20000100180512510231441E19058341E9149E592D9743EA151E9940A"
 * "0891683108100005F04406A177848800F530202151016023550B05040B8423F000037102028A808E0400003D9D880681043E4D086983687474703A2F2F3231382E3230302E3234342E3139372F7761732F3032313231353136353739313030303030373030303030303330313400"
 *----------------------------------------------------*/
int	ME_RecStr_Code( PSMS_INFO pDesSMS_Read, char* pPDUStr )
{
	int		Len, Offset;
	int		PDU_Offset, ContextType;

	char	PhoneLen[ME_CODELEN_STRLEN], PhoneNum[ME_PHONENUM_LEN];
	char	SendTime[ME_SMS_TIME_LEN];
	char	ContextLen[ME_CODELEN_STRLEN];
	char	Context[ME_SMS_PDU_LEN],TempContext[ME_SMS_PDU_LEN];
	struct SMS_FO fo;
	BYTE FoData;

	if( strlen( pPDUStr)== 0 )
		return -1;

	memset(SendTime, 0, ME_SMS_TIME_LEN);
	memset(Context, 0, ME_SMS_PDU_LEN);
	memset(TempContext, 0, ME_SMS_PDU_LEN);

	memcpy( PhoneLen, pPDUStr, 2 );				// SCA Number
	PhoneLen[2] = 0;
	Len = HexString2DWORD( PhoneLen )*2 -2;

	PDU_Offset = 2;

	if ( Len > 0 )
	{
		if( memcmp( pPDUStr+PDU_Offset, "91", 2 ) ==0 )
		{
			strcpy( pDesSMS_Read->SCA, "+" );
			Offset = 1;
		}
		else
			Offset = 0;

		PDU_Offset += 2;
		memcpy( PhoneNum, pPDUStr+PDU_Offset, Len );
		PhoneNum[Len]=0;

		Analyse_PhoneNum( pDesSMS_Read->SCA+Offset, PhoneNum, ANALYSE_DECODE );
		PDU_Offset += Len;
	}

	memcpy(PhoneNum, pPDUStr+PDU_Offset, 2);
	PhoneNum[2] = 0;
	FoData = HexStr2BYTE( PhoneNum );

	FOData_Decode( FoData, &fo );

	if ( fo.UDHI )
		pDesSMS_Read->Type = DATA_SMS;
	else
		pDesSMS_Read->Type = TEXT_SMS;

	if ( fo.MTI == 2 )
		pDesSMS_Read->Type = STATUS_REPORT;
	else if ( fo.MTI == 1 )
		pDesSMS_Read->Type = SUBMIT_REPORT;

	PDU_Offset += 2;							// some settings

	if ( pDesSMS_Read->Type == STATUS_REPORT )
		PDU_Offset += 2;						//MR

	memcpy( PhoneLen, pPDUStr+PDU_Offset, 2 );	// Sender Number
	PhoneLen[2] = 0;
	Len = HexString2DWORD( PhoneLen );
	if( Len%2 != 0 )
		Len+=1;

	PDU_Offset += 2;
	if( memcmp( pPDUStr+PDU_Offset, "91", 2 ) ==0 )
	{
		strcpy( pDesSMS_Read->SenderNum, "+" );
		Offset =1;
	}
	else
		Offset =0;

	PDU_Offset +=2;
	memcpy( PhoneNum, pPDUStr+PDU_Offset, Len );
	PhoneNum[Len] = 0;
	Analyse_PhoneNum( pDesSMS_Read->SenderNum +Offset, PhoneNum, ANALYSE_DECODE );
	PDU_Offset += Len;

	if ( pDesSMS_Read->Type == STATUS_REPORT )
	{
		PDU_Offset += 14;

		memcpy( SendTime, pPDUStr+PDU_Offset, 14 );			// Send Time
		SendTime[14] = 0;
		Analyse_PhoneNum( pDesSMS_Read->SendTime, SendTime, ANALYSE_DECODE );

		PDU_Offset += 14;

		memcpy(PhoneNum, pPDUStr+PDU_Offset, 2);

		PhoneNum[2] = 0;

		pDesSMS_Read->Status = (int)HexStr2BYTE( PhoneNum );

		return 0;
	}

	PDU_Offset += 2;		// PID

							//DCS	
	if ( pPDUStr[PDU_Offset+1] >='0' && pPDUStr[PDU_Offset+1] <='3' )
		ContextType = 0;				// GSM 7 bits
	else if ( pPDUStr[PDU_Offset+1] >='4' && pPDUStr[PDU_Offset+1] <='7' )
		ContextType = 2;				// Ascii 8 bits
	else if ( pPDUStr[PDU_Offset+1] >='8' && pPDUStr[PDU_Offset+1] <='B')
		ContextType = 1;				// UCS2 16 bits
	else
		ContextType = 2;

	if ( pPDUStr[PDU_Offset] == 'F' || pPDUStr[PDU_Offset] == '1' || pPDUStr[PDU_Offset] == '3')
	{
		char dcs = pPDUStr[PDU_Offset+1];
		switch (dcs)
		{
		case '0':
		case '4':
		case '8':
		case 'C':
			pDesSMS_Read->Class = SMS_CLASS_0;
			break;
		case '1':
		case '5':
		case '9':
		case 'D':
			pDesSMS_Read->Class = SMS_CLASS_1;
			break;
		case '2':
		case '6':
		case 'A':
		case 'E':
			pDesSMS_Read->Class = SMS_CLASS_2;
			break;
		case '3':
		case '7':
		case 'B':
		case 'F':
			pDesSMS_Read->Class = SMS_CLASS_3;
			break;
		}
	}
	else
		pDesSMS_Read->Class = SMS_CLASS_NONE;

	PDU_Offset += 2;

	memcpy( SendTime, pPDUStr+PDU_Offset, 14 );			// Send Time
	SendTime[14] = 0;
	Analyse_PhoneNum( pDesSMS_Read->SendTime, SendTime, ANALYSE_DECODE );
	PDU_Offset += 14;

	memcpy( ContextLen, pPDUStr+PDU_Offset, 2 );		// Context length
	ContextLen[2]=0;
	Len = HexString2DWORD( ContextLen )*2;
	if( Len > ME_SMS_PDU_LEN )
		Len =ME_SMS_PDU_LEN;
	PDU_Offset += 2;

	strcpy( Context, pPDUStr+PDU_Offset );				// Context
	if( ContextType == 1 )
	{
		Analyse_PDUCode2WideChar( TempContext, Context ,ME_SMS_PDU_LEN);
		WideCharToMultiByte( 
				CP_ACP, 
				0,
				(LPCWSTR)TempContext,
				-1,
				(LPSTR)pDesSMS_Read->Context,
				ME_SMS_CONTEXT_LEN,
				0,
				0 );
		return Len;
	}
	if( ContextType == 0 )
	{
		String2BinStr(Context, TempContext  );
		GSMstr2ASCIIstr( TempContext, pDesSMS_Read->Context);
		return Len;
	}
	if (ContextType == 2)
	{
		String2BinStr(Context, pDesSMS_Read->Context);
		return Len/2;
	}

	return 0;
}


/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int Analyse_WideChar2PDUCode( char* pDesStr, char* pSrcStr, int DesLen, int SrcLen )
{
	int		i,j;
	char	TempContext[ME_SMS_PDU_LEN];

	memset( TempContext, 0, ME_SMS_PDU_LEN );
	BinStr2String( pSrcStr, pDesStr, SrcLen*2 );

	for( i=0,j=2; pDesStr[i] != 0; i+=4,j+=4 )
	{
		TempContext[j] =	pDesStr[i];
		TempContext[j+1] =	pDesStr[i+1];
		TempContext[i] =	pDesStr[j];
		TempContext[i+1] =	pDesStr[j+1];
	}
	TempContext[i] = 0;
	TempContext[i+1] = 0;

	memcpy( pDesStr, TempContext, DesLen );

	return 0;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int		BinStr2String(char *pSrcStr, char *pDstStr, int Length)
{
	int i;
	char *pHexStr;
#if defined VME_DEBUG
    debugex_nonl( "\r\nBinStr2String: malloc begin" );
#endif
	pHexStr = (char *)malloc(Length*2+1);
#if defined VME_DEBUG
    debugex_nonl( "\r\nBinStr2String: malloc end" );
#endif

	for(i=0;i<Length;i++)
	{
		Byte2Str(pSrcStr[i],pHexStr+2*i);
	}
	pHexStr[2*Length]=0;
	strcpy(pDstStr,pHexStr);
	
	free(pHexStr);
	return 0;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int Byte2Str(BYTE code,char *str)
{
	BYTE b1,b2,byte;
	
	char ch1,ch2;
	byte = code;
	b1=(byte&0xf0)>>4;
	b2=byte&0x0f;

	if(b1>9)
		ch1='A'+b1-10;
	else
		ch1='0'+b1;

	if(b2>9)
		ch2='A'+b2-10;
	else
		ch2='0'+b2;

	str[0]=ch1;
	str[1]=ch2;
	
	return 2;
	
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int ASCIIstr2GSMstr(char *pSrcStr, char *pDstStr)
{
	DWORD b[8], A, B;
	int i,j, str_len, LopI, ModI;
	char *pHexStr;

	str_len = strlen(pSrcStr);
#if defined VME_DEBUG
    debugex_nonl( "\r\nASCIIstr2GSMstr: malloc begin" );
    debugex_nonl( "\r\nSrcstr = \"%s\"", pSrcStr );
    debugex_nonl( "\r\nstr_len = %d", str_len );
#endif
	pHexStr = (char *)malloc(str_len+7);
#if defined VME_DEBUG
    debugex_nonl( "\r\nASCIIstr2GSMstr: malloc end" );
#endif

	LopI= str_len/8;

	ModI= str_len%8;
	if(ModI)
		LopI++;
	
	for(j=0; j<LopI; j++)
	{
		A=B=0;
		for(i=0;i<8;i++)
		{
			if(ModI && j==LopI-1 && i>= ModI)
				b[i]=0;
			else
				b[i]=(char)pSrcStr[8*j+i];
		}
		A = b[0] + (b[1]<<7) + (b[2]<<14) + (b[3]<<21) + (b[4]<<28);
		B = b[4] + (b[5]<<7) + (b[6]<<14) + (b[7]<<21);
		B = B>>4;
		pHexStr[7*j] = (char)A;
		pHexStr[7*j+1] = (char)(A>>8);
		pHexStr[7*j+2] = (char)(A>>16);
		pHexStr[7*j+3] = (char)(A>>24);
		pHexStr[7*j+4] = (char)B;
		pHexStr[7*j+5] = (char)(B>>8);
		pHexStr[7*j+6] = (char)(B>>16);
	}
	pHexStr[7*j+7]=0;
	strcpy(pDstStr,pHexStr);
	free(pHexStr);
	return 0;
}


/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int	Analyse_PDUCode2WideChar(  char* pDesStr, char* pSrcStr, int BufLen )
{
	char	TempContext[ME_SMS_PDU_LEN];
	char	TempStr[4];
	int		i,j,TempVal;

	memset(TempContext, 0, ME_SMS_PDU_LEN);
	for( i=0,j=2; pSrcStr[i] != 0; i+=4,j+=4 )
	{
		pDesStr[j] = pSrcStr[i];
		pDesStr[j+1] = pSrcStr[i+1];
		pDesStr[i] = pSrcStr[j];
		pDesStr[i+1] = pSrcStr[j+1];
	}
	pDesStr[i] = 0;

	for( i=0,j=0 ; pSrcStr[i] != 0; i+=2,j++ )
	{
		memset( TempStr, 0, 4 );
		strncpy( TempStr, pDesStr+i, 2 );
		TempVal = HexString2DWORD( TempStr );
		TempContext[j] = TempVal;
	}
	TempContext[j] = 0;		// Double '\0'
	TempContext[j+1] = 0;
	memcpy( pDesStr, TempContext, BufLen );
	return 0;
}

/*********************************************************************\
* Function
* Purpose
* Params
* Return
* Remarks
**********************************************************************/
int String2BinStr(char *pSrcStr, char *pDstStr)
{
	int i, str_len;
	str_len = strlen(pSrcStr);

	for(i=0;i<str_len;i+=2)
	{
		pDstStr[i/2] = HexStr2BYTE(pSrcStr+i);
	}
	pDstStr[i/2]=0;

	return 0;
}

/*********************************************************************\
* Function	   
* Purpose    将0到255的有效期数值转化为PDU字符串  
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int DecNum2HexStr(int vp, char* vpstr)
{
	int quot,rem;
	quot = vp/16;
	rem = vp%16;

	if(quot>=0 && quot<=9)
		vpstr[0] = quot+'0';
	else if(quot>=10 && quot<=15)
		vpstr[0] = quot+'A'-10;
	else 
		return -1;
	
	if(rem>=0 && rem<=9)
		vpstr[1] = rem+'0';
	else if(rem>=10 && rem<=15)
		vpstr[1] = rem+'A'-10;
	else 
		return -1;
	
	vpstr[2] = '\0';
	return 0;
}

/*------------------------------------------------------------
 *
 * Usage: decode a Hex value: char *str("1C")-> return BYTE=0X1C
 * rule: str lenght =2
 * example: String2BinStr()
 *
 *-------------------------------------------------------*/

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BYTE HexStr2BYTE(char *str)
{
	BYTE	b1='0', ch;
	BYTE	code;
	
	ch = str[0];
		
	if(ch>='A' && ch<='F')
	{
		b1=ch-'A'+10;
	}
	if(ch>='a' && ch<='f')
	{
		b1=ch-'a'+10;
	}
	if(ch>='0' && ch<='9')
	{
		b1=ch-'0';
	}

	code = (b1<<4);

	ch = str[1];
	
	if(ch>='A' && ch<='F')
	{
		b1=ch-'A'+10;
	}
	if(ch>='a' && ch<='f')
	{
		b1=ch-'a'+10;
	}
	if(ch>='0' && ch<='9')
	{
		b1=ch-'0';
	}
	code += b1;
		

	return code;
	
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int GSMstr2ASCIIstr(char *pSrcStr, char *pDstStr)
{
	DWORD b[8], A, B;
	int i,j, str_len, LopI, ModI;
	char *pHexStr;

	str_len = strlen(pSrcStr);
	

	LopI= str_len/7;

	ModI= str_len%7;
	if(ModI)
		LopI++;

#if defined VME_DEBUG
    debugex_nonl( "\r\nGSMstr2ASCIIstr: malloc begin" );
#endif
	pHexStr = (char *)malloc(LopI*8);
#if defined VME_DEBUG
    debugex_nonl( "\r\nGSMstr2ASCIIstr: malloc end" );
#endif
	
	for(j=0; j<LopI; j++)
	{
		A=B=0;
		for(i=0;i<7;i++)
		{
			if(ModI && j==LopI-1 && i>= ModI)
				b[i]=0;
			else
				b[i]=(BYTE)pSrcStr[7*j+i];
		}
		
		A = b[0] + (b[1]<<8) + (b[2]<<16) + (b[3]<<24);
		B = b[3] + (b[4]<<8) + (b[5]<<16) + (b[6]<<24);
		B = B>>4;

		pHexStr[8*j]   = (char)(A&0x7F);
		pHexStr[8*j+1] = (char)((A>>7 )&0x7F);
		pHexStr[8*j+2] = (char)((A>>14)&0x7F);
		pHexStr[8*j+3] = (char)((A>>21)&0x7F);
		pHexStr[8*j+4] = (char)(B&0x7F);
		pHexStr[8*j+5] = (char)((B>>7 )&0x7F);
		pHexStr[8*j+6] = (char)((B>>14)&0x7F);
		pHexStr[8*j+7] = (char)((B>>21)&0x7F);
	}
	strcpy(pDstStr,pHexStr);
	free(pHexStr);
	return 0;
}


/*--------------------------------------------------------------------
 *
 * Usage: decode a length value: char *str("0A")-> return int=10
 * rule: str lenght =2
 * example:telephone length
 *
 *-----------------------------------------------------------------*/

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int HexString2DWORD(char *str)
{
	BYTE	b1='0', ch;
	int		i;
	int		ret,code,len;
	
	ret = 0;

	len = strlen(str);

	if(len>2)
		return 0;

	i=len-1;
	while(i>=0)
	{
		ch = str[i];
		if(ch=='x'||ch=='X')  // no use in this condition, just ensure
			break;

		if(ch>='A' && ch<='F')
		{
			b1=ch-'A'+10;
		}
		if(ch>='a' && ch<='f')
		{
			b1=ch-'a'+10;
		}
		if(ch>='0' && ch<='9')
		{
			b1=ch-'0';
		}
		code = b1;
		ret+=code<<  ((len-1-i)*4);
		i--;
	}
	return ret;
	
}


/*--------------------------------------------------------
 *
 * Analyse telephone number decode/code
 *
 *------------------------------------------------------*/

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int	Analyse_PhoneNum( char *pContext, char *pPDUStr, int type )
{
	int i;
	switch( type )
	{
	case ANALYSE_CODE:		//pContext->pPDUStr
		for( i=0; ;i+=2 )
		{
			if( pContext[i] == 0 )
			{
				pPDUStr[i]=0;
				break;
			}
			if ( pContext[i] == '*' )
				pPDUStr[i+1] = 'A';
			else if ( pContext[i] == '#' )
				pPDUStr[i+1] = 'B';
			else if ( pContext[i] == 'a' )
				pPDUStr[i+1] = 'C';
			else if ( pContext[i] == 'b' )
				pPDUStr[i+1] = 'D';
			else if ( pContext[i] == 'c' )
				pPDUStr[i+1] = 'E';
			else
				pPDUStr[i+1] = pContext[i];

			// when strlen == odd len
			if( pContext[i+1] == 0 )
			{
				pPDUStr[i] = 'F';
				pPDUStr[i+2] =	0;
				break;
			}
			if ( pContext[i+1] == '*' )
				pPDUStr[i] = 'A';
			else if ( pContext[i+1] == '#' )
				pPDUStr[i] = 'B';
			else if ( pContext[i+1] == 'a' )
				pPDUStr[i] = 'C';
			else if ( pContext[i+1] == 'b' )
				pPDUStr[i] = 'D';
			else if ( pContext[i+1] == 'c' )
				pPDUStr[i] = 'E';
			else
				pPDUStr[i] = pContext[i+1];
		}
		break;

	case ANALYSE_DECODE:
		for( i=0; ;i+=2 )
		{
			if( pPDUStr[i] == 0 )
			{
				pContext[i] = 0;
				if( pContext[i-1]=='F')
					pContext[i-1] = 0;
				break;
			}
			if ( pPDUStr[i+1] == 'A' )
				pContext[i] = '*';
			else if ( pPDUStr[i+1] == 'B' )
				pContext[i] = '#';
			else if ( pPDUStr[i+1] == 'C' )
				pContext[i] = 'a';
			else if ( pPDUStr[i+1] == 'D' )
				pContext[i] = 'b';
			else if ( pPDUStr[i+1] == 'E' )
				pContext[i] = 'c';
			else
				pContext[i] = pPDUStr[i+1];

			if ( pPDUStr[i] == 'A' )
				pContext[i+1] = '*';
			else if ( pPDUStr[i] == 'B' )
				pContext[i+1] = '#';
			else if ( pPDUStr[i] == 'C' )
				pContext[i+1] = 'a';
			else if ( pPDUStr[i] == 'D' )
				pContext[i+1] = 'b';
			else if ( pPDUStr[i] == 'E' )
				pContext[i+1] = 'c';
			else
				pContext[i+1] = pPDUStr[i];
		}
		break;
	}
	return 0;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return 
* Remarks	   
**********************************************************************/
int FOData_Decode( BYTE src, struct SMS_FO* dst )
{
	int ret;

	memset( dst, 0, sizeof(struct SMS_FO) );

	dst->MTI = src&0x03;//00000011

	if ( dst->MTI == 2 )
	{
		ret = STATUS_REPORT;
		dst->SRQ = src&0x20;//00100000
		dst->MMS = src&0x04;//00000100
		dst->UDHI = src&0x40;//01000000
	}
	else if ( dst->MTI == 1 )
	{
		ret = SUBMIT_REPORT;
		dst->UDHI = src&0x40;//01000000
	}
	else
	{
		ret = SMS_DELIVER;
		dst->MMS = src&0x04;//00000100
		dst->SRI = src&0x20;//00100000
		dst->UDHI = src&0x40;//01000000
		dst->RP = src&0x80;//10000000
	}

	return ret;
}

/*********************************************************************\
* Function	   
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int ME_RecStr_Flag( PSMS_INFO pSMSRead, char* SMS_Prompt )
{
	// +CMGR: 1,,22
	int tag;

	if( strlen(SMS_Prompt)== 0 )
		return -1;

	if ( strstr( SMS_Prompt, "+CMGR:" ) != NULL )
		tag = 0;
	else if ( strstr( SMS_Prompt, "^SMGR:" ) != NULL )
		tag = 1;
	else
		return -1;

	if ( tag == 0 )
		pSMSRead->Stat = ME_GetIntSection( SMS_Prompt, "+CMGR:", 1 );
	else
		pSMSRead->Stat = ME_GetIntSection( SMS_Prompt, "^SMGR:", 1 );

	return pSMSRead->Stat;
}

/*********************************************************************\
* Function
* Purpose
* Params
* Return
* Remarks
**********************************************************************/
int ME_RecLstStr_Code( PSMS_INFO pSMSInfo, char* pSmsContext, int Count )
{
	int	 i, tag;
	char *begin, *end;

	if( strlen(pSmsContext)== 0 )
		return -1;

	if ( strstr( pSmsContext, "+CMGL:" ) != NULL )
		tag = 0;
	else if ( strstr( pSmsContext, "^SMGL:" ) != NULL )
		tag = 1;
	else
		return 0;

	end = (char*)pSmsContext-2;

	for ( i=0; i < Count; i++ )
	{
		begin = end + 2;

		while ( *begin == '\r')
			begin += 2;

		if ( strlen( begin ) == 0 )
			return i;

		end = strchr( begin, '\r' );

		if ( end == NULL )
			return i;

		*end = '\0';

		if ( tag == 0 )
		{
			pSMSInfo[i].Index = ME_GetIntSection( begin, "+CMGL:", 1);
			pSMSInfo[i].Stat = ME_GetIntSection( begin, "+CMGL:", 2);
		}
		else
		{
			pSMSInfo[i].Index = ME_GetIntSection( begin, "^SMGL:", 1);
			pSMSInfo[i].Stat = ME_GetIntSection( begin, "^SMGL:", 2);
		}

		if ( pSMSInfo[i].Index == -1 || pSMSInfo[i].Stat == -1 )
			return i;

		begin = end + 2;

		while ( *begin == '\r')
			begin += 2;

		if ( (strstr( begin, "+CMGL:" ) == begin && tag == 0)
			|| (strstr( begin, "^SMGL:" ) == begin && tag == 1))
		{
			end = begin-2;
			continue;
		}

		if ( strlen( begin ) == 0 )
			return i;

		end = strchr( begin, '\r' );

		if ( end == NULL )
			return i;

		*end = '\0';

		pSMSInfo[i].ConLen = ME_RecStr_Code( &pSMSInfo[i], begin );
	}

	return i;
}

int Analyse_UCS2PDUCode(char* pPDU, char* pUCS, int nBufLen)
{
	char *pBegin, *pEnd;
	char TempBuf[5];
	int nPDULen;
	char *endchar;
	long CharCount;
	int BaseNum, i, AddNum,j;

	// 不是UCS2编码
	if (pUCS[0]!='8' || (pUCS[0]=='8' && pUCS[1]!='0' && pUCS[1]!='1'))
		return -2;
	
	// 全部是UCS2编码
	if (pUCS[0]=='8' && pUCS[1]=='0')
	{
		pBegin = &pUCS[2];
		pEnd = pUCS+strlen(pUCS)-4;
		if (strcmp(pEnd, "FFFF") != 0)
			return -1;
		nPDULen = pEnd-pBegin;
		if (nPDULen >= nBufLen)
			return -1;
		memcpy(pPDU, pBegin, nPDULen);
		pPDU[nPDULen] = 0;
		return nPDULen;
	}

	// UCS2与GSM混合编码
	memcpy(TempBuf, &pUCS[2], 2);
	TempBuf[2] = 0;
	CharCount = strtol(TempBuf, &endchar, 16);
	memcpy(TempBuf, &pUCS[4], 2);
	TempBuf[2] = 0;
	BaseNum = strtol(TempBuf, &endchar, 16)<<7;
	pBegin = &pUCS[6];
	nPDULen = 0;
	for (i=0; i<CharCount; i++)
	{
		if (*pBegin < '8')
		{
			memcpy(&pPDU[nPDULen], "00", 2);
			nPDULen += 2;
			memcpy(&pPDU[nPDULen], pBegin, 2);
			nPDULen += 2;
			pBegin += 2;
		}
		else
		{
			memcpy(TempBuf, pBegin, 2);
			TempBuf[2] = 0;
			AddNum = strtol(TempBuf, &endchar, 16)-128;
			sprintf( TempBuf, "%X", BaseNum+AddNum );
//			itoa(BaseNum+AddNum, TempBuf, 16);
			if (strlen(TempBuf) == 4)
			{
				memcpy(&pPDU[nPDULen], TempBuf, 4);
				nPDULen += 4;
			}
			else
			{
				for (j=0; j<(int)(4-strlen(TempBuf)); j++)
				{
					pPDU[nPDULen++] = '0';
				}
				memcpy(&pPDU[nPDULen], TempBuf, strlen(TempBuf));
				nPDULen += strlen(TempBuf);
			}
			pBegin += 2;
		}
	}
	pPDU[nPDULen] = 0;
	strupr( pPDU );

	return nPDULen;
}