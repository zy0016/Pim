/**************************************************************************\
 *
 *                      PDAOS Window System
 *
 * Copyright (c) 1998 by CASS Corporation & Software Engineering Center
 *                       All Rights Reserved
 *
 *
 * Filename     ctype.h
 *
 * Purpose      Character type header.
 *  
 * Author       WeiZhong
 *
\**************************************************************************/

#ifndef __CTYPE_H
#define __CTYPE_H

#if __cplusplus
extern "C" {
#endif

int isalnum(int c);
int isalpha(int c);
int iscntrl(int c);
int isdigit(int c);
int isgraph(int c);
int islower(int c);
int isprint(int c);
int ispunct(int c);
int isspace(int c);
int isupper(int c);
int isxdigit(int c);
int tolower(int c);
int toupper(int c);

int isascii(int c);
int toascii(int c);

#define isascii(c)	((unsigned int)(c)<=0x7f)
#define toascii(c)	((c) & 0x7f)

#if __cplusplus
}
#endif

#endif
