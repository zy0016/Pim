#ifndef __WAP_H__
#define __WAP_H__
/*************************************************************************
                                                                          
   SEF CONFIDENTIAL AND PROPRIETARY                                       
                                                                          
   This source is the sole property of SEF Inc. Reproduction or           
   utilization of this source in whole or in part is forbidden without    
   the written consent of SEF Inc.                                        
                                                                          
   (c) Copyright SEF Inc.,         2002.  All Rights Reserved.            
                                                                          
**************************************************************************
                                                                          
   wap.h: Header file of wap.c									
                                                                          
**************************************************************************
   Date       Author                  Reference
    ========   =============    ==========================
    2002/6/29  |  ChenGuofang    |    CR - BJC00004   
	 ========   =============    ==========================
    2002/8/1  |  ChenGuofang    |    CR - BJC00063   
**************************************************************************/


#ifdef _cplusplus
extern "C" {
#endif

/************** INCLUDES ******************************************************/
#include "window.h"
#include "string.h"
#include "fapi.h"
#include "malloc.h"
#include "stdlib.h"
#include "winpda.h"
#include "stdio.h"
#include "TitleInd.h"
#include "hp_pdaex.h"
#include "MEdrv.h"
#include "Progman.h"
#include "atcmd_parseWAP.h"

/*---------- Dependencies-------- --------------------------------------*/\
/*---------- Global Definitions and Declarations -----------------------*/
/*---------- Constant / Macro Definitions ------------------------------*/

/*---------- Type Declarations -----------------------------------------*/
  typedef enum				/* shows the return  */
{   
	WAPR_HANGUP_ERROR=0,              /* 0:   �Ҷ�ʧ��          */
	WAPR_HANGUP_OK,                   /* 1:   �Ҷϳɹ�          */
    WAPR_DIALUPD_OK ,                 /* 2:   ���ųɹ�          */
	WAPR_DIALUPDT_OK,                 /* 3��  ��ATDT��ʽ�ɹ�      */
    WAPR_DIALUPD_BUSY,			      /* 4:   ����Dæ			  */
    WAPR_DIALUPDT_BUSY,			      /* 4:   ����DTæ			  */
    WAPR_DIALUPD_NOANSWER,		      /* 5:   ����Dû�л�Ӧ	      */
	WAPR_DIALUPDT_NOANSWER,		      /* 6:   ����DTû�л�Ӧ	      */
    WAPR_DIALUPD_NOCARRIER,	          /* 7:	  ����û���ź�		  */
	WAPR_DIALUPDT_NOCARRIER,	      /* 8:	  ����û���ź�		  */
    WAPR_DIALUPD_ERROR,	              /* 9:   ����ATD��ʽʧ��     */
    WAPR_DIALUPDT_ERROR,		      /* 10:  ����ATDT��ʽʧ��    */
	WAPR_ERROR,
} WAP_PARSERRESPONSE_RESULT;
/*---------- Variable Declarations -------------------------------------*/

/*---------- function Declarations -------------------------------------*/

/*---------- Global function Declarations ------------------------------*/
int  Init_WAP_Dialup();
void WAP_Hangup(HWND hWnd);
int  WAP_DialupModeD(HWND hWnd,char *pNumber);
int  WAP_DialupModeDT(HWND hWnd,char *pNumber);
WAP_PARSERRESPONSE_RESULT  ParserResponseWAP(LPARAM  lParam,WPARAM wParam);

#ifdef _cplusplus
}
#endif

#endif /* End of #ifndef __WAP_H__ */
