#ifndef _ME_COMMON_H_
#define _ME_COMMON_H_

void	ME_MemInitial( void );
int		ME_MemAssign( char* data, int datalen );
BOOL	ME_Malloc( int len );
void	ME_MemEmpty(void);
char*	ME_MemGetPntr(void);
int		ME_MemGetOffset(void);

int	ME_GetIntSection(char*, char*, int );
int ME_GetStrSection(char*, char*, int, char*, int );
int ME_NetOpratorDec( NETOPRATOR* pNetOp, char* pBuf, int BufCount );
int ME_PhoneBook_Dec( char *Scr, PPHONEBOOK pDesPB );
int ME_CurCalls_Dec( char *Scr, PCALLS_INFO pCallInfo );
void PhoneText_Decode( char* pDst, int nDstLen, char* pSrc );
int PhoneText_Encode( char* pDesStr, char* pSrcStr, unsigned int DestLen );
int ServCell_Decode( PSCELL_INFO pDest, char* pSource, int Status );
int NeibCell_Decode( PNCELL_INFO pDest, char* pSource );


#endif