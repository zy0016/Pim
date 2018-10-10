/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 2000-2002 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :	fast flash block store system $ version 1.0
 *
 * Filename:    BlockApi.h
 *
 * Purpose :    Implements record store, used inline.
 *  
 * Author  :    dwu
 *
 *-------------------------------------------------------------------------
 *
 * $Archive::                                                       $
 * $Revision::    $     $Date::                                     $
 *
 * $History::	Create on 2002/06/21                               $
 * 
\**************************************************************************/

#ifndef _BLOCK_API_H
#define _BLOCK_API_H

#define     RCD_NEXT          -1
//macro error
#define     FFB_BLOCKERROR        -1
#define     FFB_NOTENOUGHSPACE    -2
#define     FFB_RECORDNEEDCLEAN   -3


BOOL    InitFBlockSys();

#ifdef _EMULATE_
BOOL    CloseFBlockSys();
#endif

BOOL    RegisterAppBlock(BYTE BlockType, short nRecorderSize, BYTE nBlock);          
BOOL    UnRegisterAppBlock(BYTE BlockType);

HANDLE  OpenAppBlock(BYTE BlockType);           
void    CloseAppBlock(HANDLE hApp);

DWORD	AppRecorderWrite(HANDLE hApp, BYTE* pBuf, short nBuf);
DWORD	AppRecorderRead(HANDLE hApp, BYTE* pBuf, DWORD itemID, int* nRead);

BOOL	AppRecorderDelete(HANDLE hApp, DWORD itemID);
BOOL    CleanAppBlock(HANDLE hApp,BYTE nflag);  
long    GetFreeRecordNum(HANDLE hApp);
long    GetFreeMaxRecordSize(HANDLE hApp);                   
BOOL    SetRecordPointer(HANDLE hApp, DWORD index);

#endif