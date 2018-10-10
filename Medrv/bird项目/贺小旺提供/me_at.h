/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Mobile Engine
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef _ME_AT_H_
#define _ME_AT_H_

typedef void (*MAT_Indication)(int event, void * pdata, int datalen);

enum mat_event { MAT_RESET, MAT_DATAIN };

int MAT_Init ( int mode, int para1, int para2, MAT_Indication indication );
int MAT_Send ( int mode, void * pdata, int datalen );
int MAT_Reset ( int mode );

#define WAIT_FOR_MUTEX()	
#define RELEASE_MUTEX()

#endif
