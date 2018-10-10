#ifndef _ME_COMMON_H_
#define _ME_COMMON_H_

void	ME_MemInitial   (int id);
int		ME_MemAssign    (int id, char* data, int datalen);
BOOL	ME_Malloc       (int id, int len);
void	ME_MemEmpty     (int id);
char*	ME_MemGetPntr   (int id);
int		ME_MemGetOffset (int id);

int	ME_GetIntSection(char*, char*, int );
int ME_GetStrSection(char*, char*, int, char*, int );
int String2Bytes (char* dst, const char* src, int srclen);
int Bytes2String (char* dst, const char* src, int srclen);
char * AOCHexEncode (unsigned long value);
unsigned long AOCHexDecode (char * str);
int	UCS2Decode(char * dst, char * src, int len);
int UCS2Encode (char * dst, char * src, int len);
int ME_NetOpratorDec( NETOPRATOR* pNetOp, char* pBuf, int BufCount );
int ME_PhoneBook_Dec( char *Scr, PPHONEBOOK pDesPB );
int ME_CurCalls_Dec( char *Scr, PCALLS_INFO pCallInfo );
void PhoneText_Decode( char* pDst, int nDstLen, char* pSrc );
int PhoneText_Encode( char* pDesStr, char* pSrcStr, unsigned int DestLen );
int ServCell_Decode( PSCELL_INFO pDest, char* pSource, int Status );
int NeibCell_Decode( PNCELL_INFO pDest, char* pSource );


#endif