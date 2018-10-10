/*****************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Purpose :     
 * Author  :     
 *
 ******************************************************************************
 *
 * $Archive:: /Hopen/include/Pdb.h                                  $
 * $Revision:: 4  $     $Date:: 01-12-05 9:48                       $
 *
 * $History:: Pdb.h                                                 $
 * 
 * *****************  Version 4  *****************
 * User: Dling        Date: 01-12-05   Time: 9:48
 * Updated in $/Hopen/include
 * 
 * *****************  Version 3  *****************
 * User: Dling        Date: 01-11-09   Time: 13:59
 * Updated in $/Hopen/include
 * #ifdef __cplusplus
 * 
 * *****************  Version 2  *****************
 * User: Dling        Date: 01-04-23   Time: 18:25
 * Updated in $/Hopen/include
 * 
 * *****************  Version 1  *****************
 * User: Dling        Date: 01-03-30   Time: 10:30
 * Created in $/Hopen/include
 * 
\******************************************************************************/


#ifndef _DATABASE_H_
#define _DATABASE_H_

#include "hpdef.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// field type:
#define DB_FT_STRING	0
#define DB_FT_UI1		1
#define DB_FT_I2		2
#define DB_FT_I4		3
#define DB_FT_DATE		4
#define DB_FT_STRUCT	5
#define DB_FT_BOOL		DB_FT_UI1

// how to find:
#define DB_FINDFIRST	1
#define DB_FINDLAST		2
#define DB_FINDNEXT		3
#define DB_FINDPREV		4

// date:
typedef DWORD	DBDATE;


#define DB_FIELDNAMELEN	12

// field info:
typedef struct DBFIELDINFO{
	WORD	type;
	WORD	size;
	BYTE	name[DB_FIELDNAMELEN];
}DBFIELDINFO, *PDBFIELDINFO;

// field value:
typedef struct DBFIELDVALUE{
	WORD	type;
	WORD	size;
	union {
		BYTE	bVal;		// DB_FT_UI1
		short	iVal;		// DB_FT_I2
		long	lVal;		// DB_FT_I4
		DBDATE	date;		// DB_FT_DATE
		BYTE *	strVal;		// DB_FT_STRING
		void *	addrVal;	// DB_FT_STRUCT
	}val;
}DBFIELDVALUE, *PDBFIELDVALUE;

// record:
typedef struct DBRECORDITEM{
	WORD			nFields;
	DBFIELDVALUE *	fvArray;
}DBRECORDITEM, *PDBRECORDITEM;

// bookmark
typedef struct DBBOOKMARK{
	DWORD		index;
}DBBOOKMARK, *PDBBOOKMARK;

// create/open/close
HANDLE	DB_Create(LPCSTR lpszName, UINT nFilelds, DBFIELDINFO* fArray);
HANDLE	DB_Open(LPCSTR lpszName);
BOOL	DB_Close(HANDLE hDB);
BOOL	DB_Delete(LPCSTR lpszName);

// field count
int		DB_GetFieldCount(HANDLE hDB);

// record count
long	DB_GetRecordCount(HANDLE hDB);

// add/delete a record
BOOL	DB_AddNewRecord(HANDLE hDB, DBRECORDITEM *pRec);
BOOL	DB_DeleteRecord(HANDLE hDB);

// read current record
BOOL	DB_GetRecord(HANDLE hDB, DBRECORDITEM *pRec);

// set/get a field  value of current record:
BOOL	DB_SetFieldValue(HANDLE hDB, int nOrdinal, DBFIELDVALUE* pVal);
BOOL	DB_GetFieldValue(HANDLE hDB, int nOrdinal, DBFIELDVALUE* pVal);

// get field index:
int		DB_GetFieldIndex(HANDLE hDB, LPCSTR pFieldName);
BOOL	DB_GetFieldName(HANDLE hDB, int nOrdinal, LPSTR pBuffer, DWORD size);

// find:
BOOL	DB_SetQueryFilter(HANDLE hDB, LPCSTR lpszFilter);
BOOL	DB_BindParam(HANDLE hDB, LPCSTR lpParamName, void* pValue);
BOOL	DB_Find(HANDLE hDB, UINT uType);

// move:
BOOL	DB_MoveFirst(HANDLE hDB);
BOOL	DB_MoveLast(HANDLE hDB);
BOOL	DB_Move(HANDLE hDB, long lRows);

//得到当前记录的书签
BOOL	DB_GetBookmark(HANDLE hDB, DBBOOKMARK *pBookmark);

//根据书签设置当前记录指针
BOOL	DB_SetBookmark(HANDLE hDB, DBBOOKMARK *pBookmark);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif// _DATABASE_H_