#define ME_SMS_PDU_LEN			660

int	Analyse_PDUCode2WideChar(  char* pDesStr, char* pSrcStr, int BufLen );
int	Analyse_WideChar2PDUCode(  char* pDesStr, char* pSrcStr, int DesLen, int SrcLen );
int Analyse_UCS2PDUCode(char* pPDU, char* pUCS, int nBufLen);

void Analyse_MultiBytes2PDUCode( char* pDesStr, char* pSrcStr, int BufLen );

int	ME_TranStr_Code( PSMS_SEND pSMS_Send, char* pDesStr, int len );
int	ME_RecStr_Code( PSMS_INFO pDesSMS_Read, char* pPDUStr, int stat );
int	ME_RecStr_Flag( PSMS_INFO pSMSRead, char* SMS_Prompt );
int ME_RecLstStr_Code( PSMS_INFO pSMSInfo, char* pSmsContext, int Count );

struct SMS_FO
{
	BYTE MTI;
	BYTE RD;
	BYTE MMS;
	BYTE VPF;
	BYTE SRR;
	BYTE SRI;
	BYTE SRQ;
	BYTE UDHI;
	BYTE RP;
};

//ME_TranStr_Code
int	BinStr2String(char *pSrcStr, char *pDstStr, int Length);
static int	Byte2Str(BYTE code,char *str);
static int	gsm8Bytes_7Bytes(char * dst, char * src, int srclen, int offset);

//ME_RecStr_Code
static int FOData_Decode( BYTE src, struct SMS_FO* dst );
static int	String2BinStr(char *pSrcStr, char *pDstStr, int srclen);
static BYTE	HexStr2BYTE(char *str);
static int	gsm7Bytes_8Bytes(char * dst, char * src, int srclen, int offset);
static int	HexString2DWORD(char *str);
static int	DecNum2HexStr(int vp, char* vpstr);
static int	Analyse_PhoneNum( char *pContext, char *pPDUStr, int type );

#define ANALYSE_CODE				0
#define ANALYSE_DECODE				1

#define ME_CODELEN_STRLEN			4
