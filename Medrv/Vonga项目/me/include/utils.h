
#define	ATOL_OK		0
#define	ATOL_EOL	1
#define	ATOL_FAIL	-1

#define	IsBlank( c )	(c == ' ' || c == '\t')
#define	IsLineEnd( s )	(*(s) == 0 || *(s) == '\r' || *(s) == '\n')
#define	SkipBlank( s )	while( *(s) == ' ' || *(s) == '\t' ) (s) ++

int s_atol_hex( unsigned char ** pStr, unsigned long * val );
int s_atol( unsigned char ** pStr, unsigned long * val );
