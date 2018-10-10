/**************************************************************************\
 *
 *                      Hopen TCP/IP System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 *
 * Filename		attsock.h
 *
 * Purpose      Define some global const and structure about SOCKET and Dial.
 *				Those items only open to Socket Programmer.				
 *  
 * Author       chriszhao
 *
 *-------------------------------------------------------------------------
 *
 * $Archive:: /tcpipsec include/attsock.h                           $
 * $Revision:: 3  $     $Date:: 01-11-05 16:20                      $
 *
 * $History:: attsock.h                                             $
 * 
 * *****************  Version 3  *****************
 * User: Zhaozheng    Date: 01-11-05   Time: 16:20
 * Updated in $/tcpipsec include
 * 
\**************************************************************************/


#ifndef __ATTSOCKET_H
#define __ATTSOCKET_H

#include "hp_kern.h"

/* Ĭ�ϱ������ */
#define DEFAULT_FLAGS		0

/* TCP port numbers */
#define	IPPORT_ECHO			7	/* Echo data port 			*/
#define	IPPORT_DISCARD		9	/* Discard data port 		*/
#define	IPPORT_FTPD			20	/* FTP Data port 			*/
#define	IPPORT_FTP			21	/* FTP Control port 		*/
#define	IPPORT_TELNET		23	/* Telnet port 				*/
#define IPPORT_SMTP			25	/* Mail port 				*/
#define	IPPORT_MTP			57	/* Secondary telnet protocol 	*/
#define	IPPORT_FINGER		79	/* Finger port 				*/
#define IPPORT_HTTP			80	/* Http Browser Port		*/	
#define	IPPORT_TTYLINK		87	/* Chat port 				*/
#define IPPORT_POP			109	/* pop2 port 				*/
#define	IPPORT_NNTP			119	/* Netnews port 			*/
#define	IPPORT_LOGINSERVER	513	/* BSD rlogin port 			*/
#define	IPPORT_TERM			5000	/* Serial interface server port	*/

/* UDP port numbers */
#define IPPROT_DAYTIME		13
#define	IPPORT_DOMAIN		53
#define	IPPORT_BOOTPS		67
#define	IPPORT_BOOTPC		68
#define	IPPORT_PHOTURIS		468	/* Photuris Key management 	*/
#define	IPPORT_EFSSERVER	520
#define	IPPORT_REMOTE		1234	/* Pulled out of the air 		*/
#define	IPPORT_BSR			5000	/* BSR X10 interface server port (UDP) */

/*SOCKET�м�ı���ֶ�*/
#define	P_PERMENTLENT		0

/*SOCKET�м��family����*/
/*���������ֵ��֤����*/
#define AF_UNSPEC       0               /* unspecified */
#define AF_UNIX         1               /* local to host (pipes, portals) */
#define AF_INET         2               /* internetwork: UDP, TCP, etc. */
#define AF_IMPLINK      3               /* arpanet imp addresses */
#define AF_PUP          4               /* pup protocols: e.g. BSP */
#define AF_CHAOS        5               /* mit CHAOS protocols */
#define AF_IPX          6               /* IPX and SPX */
#define AF_NS           6               /* XEROX NS protocols */
#define AF_ISO          7               /* ISO protocols */
#define AF_OSI          AF_ISO          /* OSI is ISO */
#define AF_ECMA         8               /* european computer manufacturers */
#define AF_DATAKIT      9               /* datakit protocols */
#define AF_CCITT        10              /* CCITT protocols, X.25 etc */
#define AF_SNA          11              /* IBM SNA */
#define AF_DECnet       12              /* DECnet */
#define AF_DLI          13              /* Direct data link interface */
#define AF_LAT          14              /* LAT */
#define AF_HYLINK       15              /* NSC Hyperchannel */
#define AF_APPLETALK    16              /* AppleTalk */
#define AF_NETBIOS      17              /* NetBios-style addresses */
#define AF_VOICEVIEW    18              /* VoiceView */
#define AF_FIREFOX      19              /* FireFox */
#define AF_UNKNOWN1     20              /* Somebody is using this! */
#define AF_BAN          21              /* Banyan */

#define AF_MAX          22

/*Э������*/
#define DEFAULT_PROTOCAL	0

/*SOCKET�м��mode����*/
#define SOCK_STREAM     1               /* stream socket */
#define SOCK_DGRAM      2               /* datagram socket */
#define SOCK_RAW        3               /* raw-protocol interface */
#define SOCK_RDM        4               /* reliably-delivered message */
#define SOCK_SEQPACKET  5               /* sequenced packet stream */

/*listenָ���������г���*/
#define SOMAXCONN           15


/*�첽��Ϣѡ���壨asyncselect��*/
#define	FD_READ				0x01
#define	FD_WRITE			0x02
#define	FD_OOB				0x04
#define	FD_ACCEPT			0x08
#define	FD_CONNECT			0x10
#define	FD_CLOSE			0x20


/***************************************\
		SOCKET�м�Ĵ�����Ϣ����

	-1				: global error num
	-100 ���� -200	: Socket Error Num
\***************************************/

#define INVALID_SOCKET		-1			/* ��ЧSOKCET */	
#define SOCKET_ERROR		-1			/* Gloabl Error */

#ifndef	EWOULDBLOCK
#define	EWOULDBLOCK			-100		/*��������			*/
#define	ENOTCONN			-101		/*δ���Ӵ���		*/
#define ESOCKTNOSUPPORT		-102		/*����ַ���в�֧�ָ������׽ӿ�*/
#define	EAFNOSUPPORT		-103		/*���岻֧��		*/
#define	EISCONN				-104		/*�����Ӵ���		*/
#define	EOPNOTSUPP			-105		/*ѡ�֧��		*/
#define	EALARM				-106		/*����				*/
#define	EABORT				-107		/*����				*/
#undef	EINTR
#define	EINTR				-108		/*�û����÷���		*/
#define	ECONNREFUSED		-109		/*���ӱ��ܾ�		*/
#define EMSGSIZE			-110		/*��Ϣ���ȴ���		*/
#define	EADDRINUSE			-111		/*��ַ��ռ��		*/
#define	EMIN				-112		/*��Сֵ���		*/
#define	EMAX				-113		/*���ֵ���		*/
#define	ESOCKTABLEFULL		-114		/*SOCKET����		*/
#define	ENOMEMORY			-115		/*�ڴ治��			*/
#define	ENOTSOCK			-116		/*SOCKET�����Ч	*/
#define	EINVALIDPARA		-117		/*������Ч			*/
#define	EADDRFORMAT			-118		/*��ַ��ʽ����		*/
#define	EPROTNOSUPPORT		-119		/*Э�鲻֧��		*/
#define ELOWSYSRESOURCE		-120		/*ϵͳ��Դ����		*/
#define ETIMEOUT			-121		/*��ʱ����			*/
#define ERESET				-122		/*���ӱ���λ		*/
#define EINPROGRESS			-123		/*һ�������ĵ������ڱ�ִ�� */
#define ECONNABORTED		-124		/*���ӱ�����		*/
#define ENETDOWN			-125		/*���粻ͨ			*/
#define ENOTINITIALISED		-126		/*δ��ʼ����SockStartupû�б����ã� */
#define EUNKNOWERROR		-199		/*δ֪����			*/
#define EMAXSOCKERRORNUM	-200		/*��������ֵ*/
#endif
/*Error code about name query:*/
#define	NO_ERROR			0
#define	FORMAT_ERROR		1
#define	SERVER_FAIL			2
#define	NAME_ERROR			3
#define	NOT_IMPL			4
#define	REFUSED				5
#define RUNING_ERROR		6
#define NOBUF_ERROR			7
#define TIMEOUT_ERROR		8
#define	USERCANCLE_ERROR	9

typedef int		SOCKET;		/*SOCKET����ṹ*/

/*����һ���ַ����滻�����ڱ�ϵͳ��WINSOCK�ļ���*/

#define WSAGetLastError				OS_GetError
#define	WSAAsyncSelect				asyncselect
#define	WSAAsyncGetHostByName		asyncGetHostByName
#define WSAStartup					SockStartup
#define WSACleanup					SockCleanup
#define WSADESCRIPTION_LEN			DESCRIPTION_LEN
#define WSASYS_STATUS_LEN			SYS_STATUS_LEN
#define WSACancelAsyncRequest		asyncCancelAsyncRequest

/*����ṹ��֤��WINSOCK����*/
struct linger
{
    unsigned short l_onoff;            // option on/off
    unsigned short l_linger;           // linger time
};
typedef struct linger LINGER;


//�û�Э������
#define	TCP_IP				2

/*ͳһ��ַ�ṹ		*/
struct sockaddr 
{
	unsigned short	sa_family;
    char    sa_data[22];
};
typedef struct sockaddr SOCKADDR;

struct in_addr 
{
    union 
	{
		struct { unsigned char s_b1,s_b2,s_b3,s_b4; } S_un_b;
		struct { unsigned short s_w1,s_w2; } S_un_w;
		unsigned long S_addr;
    } S_un;
};
typedef struct in_addr IN_ADDR;

/*INTERNET��ַ�ṹ	*/
struct sockaddr_in 
{
    unsigned short  sin_family;
    unsigned short  sin_port;
    struct	in_addr sin_addr;
	char unused[16];
};
typedef struct sockaddr_in SOCKADDR_IN;

/* �鲥��ַ */
struct ip_mreq
{
	struct in_addr imr_multiaddr;
	struct in_addr imr_interface;
};

#define INADDR_ANY              (unsigned long)0x00000000
#define INADDR_LOOPBACK         0x7f000001
#define INADDR_BROADCAST        (unsigned long)0xffffffff
#define INADDR_NONE             0xffffffff

SOCKET socket(int af, int type, int protocol);
int closesocket(SOCKET s);
int asyncselect(SOCKET s, HWND hWnd, unsigned int wMsg, long lEvent);
int getsockname(SOCKET s, struct sockaddr* addr, int* addrlen);
int gethostname( char* name, int namelen );
int getpeername(SOCKET s, struct sockaddr* name,int* namelen);
int getsockopt(SOCKET s, int level, int optname, char* optval, int* optlen);
int ioctlsocket(SOCKET s, long cmd, unsigned long* parg);
int setsockopt(SOCKET s, int level, int optname, char* optval, int optlen);

//Function in socketf.c
SOCKET accept( SOCKET s, struct sockaddr* addr, int* addrlen );
int bind( SOCKET s, const struct sockaddr* addr, int addrlen );
int connect( SOCKET s, const struct sockaddr* addr, int addrlen );
int listen( SOCKET s, int backlog );
int recv( SOCKET s, char* pbuf, int len, int flags );
int recvfrom( SOCKET s, char* pbuf, int len, int flags, 
			 struct sockaddr* from, int* fromlen );
int send( SOCKET s, const char* pbuf, int len, int flags);
int sendto( SOCKET s, const char* pbuf, int len, int flags,
		   const struct sockaddr* to, int tolen);
int shutdown( SOCKET s, int how );

/*SOCKETѡ����Լ�Э����ֵ�����ã�getsockopt��setsockopt��*/
#define	SOL_SOCKET      0xffff          /*Socket ����ѡ�	*/
#define IPPROTO_IP              0               /* dummy for IP */
#define IPPROTO_ICMP            1               /* control message protocol */
#define IPPROTO_IGMP            2               /* group management protocol */
#define IPPROTO_GGP             3               /* gateway^2 (deprecated) */
#define IPPROTO_TCP             6               /* tcp */
#define IPPROTO_PUP             12              /* pup */
#define IPPROTO_UDP             17              /* user datagram protocol */
#define IPPROTO_IDP             22              /* xns idp */
#define IPPROTO_ND              77              /* UNOFFICIAL net disk proto */

#define IPPROTO_RAW             255             /* raw IP packet */
#define IPPROTO_MAX             256

#define	IPPROTO_PPP				100				/*PPP ����ѡ�		*/
/*��socket�йصĻ�һЩȫ�����õ�ѡ�getsockopt��setsockopt��*/
#define SO_BROADCAST			1	/*�����͹㲥��Ϣ*/
#define SO_DEBUG				2	/*��¼������Ϣ*/
#define SO_DONTLINGER			3	/*����δ�������ݵ�socket��ִ��close����ʱ
										���ȴ����ݷ��ͳ�ȥ*/
#define SO_KEEPALIVE			4	/*�����Եķ���kick�����Ա�������*/
#define SO_LINGER				5	/*����δ�������ݵ�socket��ִ��close����ʱ
										����ȴ����ݷ��ͳ�ȥ*/
#define SO_OOBINLINE			6	/*���մ������ݣ���һ�治֧�֣�*/
#define SO_RCVBUF				7	/*���ý��ջ������ĳ���*/
#define SO_SENDBUF				8	/*���÷��ͻ������ĳ���*/
#define SO_RCVTIMEO				9	/*���ý������ݳ�ʱ��ʱ�䣬
									��һ�治֧��socket���ĳ�ʱ����*/
#define SO_SNDTIMEO				10	/*���÷������ݳ�ʱ��ʱ�䣬
									��һ�治֧��socket���ĳ�ʱ����*/
#define SO_RCVLOWAT				11	/*���ý������ݵĵ�ˮƽ��ֵ���ڵ�һ��������*/
#define SO_SNDLOWAT  			12	/*���÷������ݵĵ�ˮƽ��ֵ���ڵ�һ��������*/
#define TCP_NODELAY				13	/*��ֹ����coalescing��Nagle�㷨*/
#define SO_REUSEADDR			14	/* �����ַ���� */
#define SO_ACCEPTCONN			15	
#define SO_ERROR				16
#define SO_MAX_MSG_SIZE			17	

#define IP_MULTICAST_IF			20	/* �����鲥��ַ */
#define IP_ADD_MEMBERSHIP		21	/* �����鲥�� */
#define IP_DROP_MEMBERSHIP		22	/* �뿪�鲥�� */
#define IP_MULTICAST_TTL		23	/* �����鲥����ʱ�� */
#define IP_MULTICAST_LOOP		24	/* �����鲥���ݵĻ��� */

/*���ں�����ioctlsocket���Ŀ�������*/
#define	FIONBIO					1
#define	FIONREAD				2
#define	SIOCATMASK				3

/*���ں���shutdown()�Ķ������Ʒ�ʽ*/
#define SD_RECEIVE				1 /*�������̵�recv����	*/
#define	SD_SEND					2 /* �������̵�send����	*/
#define SD_BOTH					3 /* �Ͽ����Ӳ���ֹ����д���� */	

/* ���ں������պͷ��� */
#define MSG_OOB				0x01	/* �������� */
#define MSG_PEEK			0x02	/* ������� */

/*******************************************************\
	Define those function of socket lib
\*******************************************************/
/*���ڶ���������ѯ�Ļ��泤�ȡ�*/
#define MAXGETHOSTSTRUCT	100

struct hostent 
{
	char*	h_name;
	char**	h_aliases;
	short	h_addrtype;
	short	h_length;
	char**	h_addr_list;
};
typedef struct hostent HOSTENT;
/*�õ���Ϣ�м�Param�Ĳ�������*/

//#define WSAGETEVENT(lParam)			loword(lParam)
//#define WSAGETSELECTERROR(lParam)	hiword(lParam)

/*SOCKET������ʼ�����˳�����*/
/*����ṹ*/
#define DESCRIPTION_LEN      256
#define SYS_STATUS_LEN       128
typedef struct tagSOCKData 
{
        WORD                    wVersion;
        WORD                    wHighVersion;
        char                    szDescription[DESCRIPTION_LEN+1];
        char                    szSystemStatus[SYS_STATUS_LEN+1];
        unsigned short          iMaxSockets;
        unsigned short          iMaxUdpDg;
        char*					lpVendorInfo;
} SOCKDATA;
typedef SOCKDATA	WSADATA;
typedef SOCKDATA*	PSOCKDATA;
/* ����ṹ�Ͷ���֧��ͬ����⺯��select */
#define FD_SETSIZE	64
typedef struct tagfd_set 
{
	unsigned long fd_count;			/* how many are SET? */
	SOCKET fd_array[FD_SETSIZE];	/* an array of SOCKETs */
}fd_set;
 
struct timeval 
{
	long tv_sec;         /* seconds */
	long tv_usec;        /* and microseconds */
};
/*����*/
int SockStartup( DWORD wVersionRequested, PSOCKDATA lpSOCKData );
int SockCleanup (void); 
/*SOCKET������ʼ�����˳��������˽���*/
/* �������� */
struct hostent* gethostbyname(char* sname);
struct hostent* gethostbyaddr( char* addr, int len, int type );

HANDLE asyncGetHostByName( HWND hWnd, unsigned int wMsg, 
							  const char* name, char* buf, 
							  int buflen ); 
int asyncCancelAsyncRequest(HANDLE hAsyncTaskHandle);
char* inet_ntoa ( struct in_addr in);
unsigned long inet_addr( const char* cp );
unsigned short htons(unsigned int port);
unsigned long htonl(unsigned long port);
unsigned short ntohs(unsigned int port);
unsigned long ntohl(unsigned long port);
int MakeIPAddr( struct sockaddr * paddr, char * ipaddr, int port );

int SL_FDClr( SOCKET s, fd_set* pset );
int SL_FDisSet( SOCKET s, fd_set* pset );
int SL_FDSet( SOCKET s, fd_set* pset );
int SL_FDZero( fd_set* pset );

#define FD_CLR		SL_FDClr
#define FD_ISSET	SL_FDisSet
#define FD_SET		SL_FDSet
#define FD_ZERO		SL_FDZero

int select( int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, 
		   const struct timeval* timeout );
int scancleblock( SOCKET s );
/*����һ�������봦����������Ӧ���������ַ�����ʽ����*/
char* dealerror();

#define MAX_HOST_NAME	60		/* �������ֵ���󳤶� */
typedef struct ping_param
{
	char	dest[MAX_HOST_NAME];/* ��������			*/
	int		len;				/* ������ַ����		*/
	short	resolve;			/* �Ƿ����������ַ */
	short	times;				/* ���͵Ĵ���		*/
	short	buflen;				/* ���ͻ�������С	*/
	short	frag;				/* ���ò�Ҫ��Ƭ		*/
	short	route;				/* ��¼·��			*/
	short	loose;				/* ��ɢ·��			*/
	short	strict;				/* �ϸ�·��			*/
	short	timestamp;			/* ��¼ʱ���		*/
	char	host[MAX_HOST_NAME];/* ��������			*/
	unsigned long addr;
	unsigned char ttl;			/* time to live		*/
	unsigned char tos;			/* ���÷�������		*/
	unsigned char unused[2];
}PING_PARAM;

int ping_execute(PING_PARAM params, void *call_back);

void TCPIP_Init( void* hWnd );

#endif