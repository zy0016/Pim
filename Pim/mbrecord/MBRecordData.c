/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/
#include "RcCommon.h"
#include "setting.h"
#include "phonebookext.h"
#include "../mbphone/MBPublic.h"
#include "hp_pdaex.h"

#define DEBUG_MBRECORDDATA

//#define MBRECORD_SAVEDATA_UNREPEAT//定义这个宏之后,保存通话记录的时候就不会保存重复的号码

static const int    iMaxPhoneRecord = MAX_PHONE_RECORD;
static const char * pUNANSWER_LOG   = "FLASH2:/UnAnswerLog.dat";
static const char * pANSWERED_LOG   = "FLASH2:/AnsweredLog.dat";
static const char * pDIALED_LOG     = "FLASH2:/DialedLong.dat";
static const char * pRECCOST_LOG    = "FLASH2:/RecCostLog.dat";

static CallRecord * MBRecord;   //读写通话记录的时候用到的中间变量(分配1480字节)
static CallRecord * pRecordUnAn;//保存未接来电数据(分配1480字节)
static CallRecord * pRecordAns; //保存已接来电数据(分配1480字节)
static CallRecord * pRecordDial;//保存已拨来电数据(分配1480字节)

static MBRecordCostTime sRecordCostTime;//将通话信息保存在内存中
static int              nUnanswered = 0;//未应答的来电的个数

static  BOOL    SaveMBRecord(const CallRecord * pCallRecNode,MBRECORDTYPE CallRecType);
static  int     InitGetMBRecord(CallRecord * pInitMBRecord,MBRECORDTYPE CallRecType);
/********************************************************************
* Function   GetFileName  
* Purpose    返回需要的通话记录的文件名
* Params    pFileName:返回文件名, iRectType通话记录的类型(未接,已接,已拨)
* Return     是否是合法的通话记录的类型
* Remarks      
**********************************************************************/
static  const char *  GetFileName(MBRECORDTYPE iRectType)
{
    switch(iRectType)
    {
    case UNANSWER_REC:
        return pUNANSWER_LOG;
    case ANSWERED_REC:
        return pANSWERED_LOG;
    case DIALED_REC:
        return pDIALED_LOG;
    default:
        return NULL;
    }
}
/********************************************************************
* Function   GetPointMBRecord  
* Purpose    返回需要的通话记录的指针
* Params    iRectType通话记录的类型(未接,已接,已拨)
* Return     
* Remarks      
**********************************************************************/
static  CallRecord * GetPointMBRecord(MBRECORDTYPE iRectType)
{
    switch(iRectType)
    {
    case UNANSWER_REC:
        return pRecordUnAn;
    case ANSWERED_REC:
        return pRecordAns;
    case DIALED_REC:
        return pRecordDial;
    default:
        return NULL;
    }
}
/********************************************************************
* Function   InitMBRecordData  
* Purpose    初始化通话记录所需数据
* Params    
* Return     
* Remarks      
**********************************************************************/
BOOL    InitMBRecordData(void)
{
    static BOOL bInit = FALSE;

    if (bInit)
        return TRUE;

    MBRecord = (CallRecord*)malloc(sizeof(CallRecord) * iMaxPhoneRecord);
    if (NULL == MBRecord)
    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRECORDDATA)
        printf("mb MBRecord malloc失败\r\n");
#endif
        return FALSE;
    }

    pRecordUnAn = (CallRecord*)malloc(sizeof(CallRecord) * iMaxPhoneRecord);
    if (NULL == pRecordUnAn)
    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRECORDDATA)
        printf("mb pRecordUnAn malloc失败\r\n");
#endif
        return FALSE;
    }

    pRecordAns = (CallRecord*)malloc(sizeof(CallRecord) * iMaxPhoneRecord);
    if (NULL == pRecordAns)
    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRECORDDATA)
        printf("mb pRecordAns malloc失败\r\n");
#endif
        return FALSE;
    }

    pRecordDial = (CallRecord*)malloc(sizeof(CallRecord) * iMaxPhoneRecord);
    if (NULL == pRecordDial)
    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRECORDDATA)
        printf("mb pRecordDial malloc失败\r\n");
#endif
        return FALSE;
    }

    memset(MBRecord   ,0x00,sizeof(CallRecord) * iMaxPhoneRecord);
    memset(pRecordUnAn,0x00,sizeof(CallRecord) * iMaxPhoneRecord);
    memset(pRecordAns ,0x00,sizeof(CallRecord) * iMaxPhoneRecord);
    memset(pRecordDial,0x00,sizeof(CallRecord) * iMaxPhoneRecord);

    InitGetMBRecord(pRecordUnAn,UNANSWER_REC);
    InitGetMBRecord(pRecordAns ,ANSWERED_REC);
    InitGetMBRecord(pRecordDial,DIALED_REC  );

    bInit = TRUE;
    return TRUE;
}
/********************************************************************
* Function   InitGetMBRecord  
* Purpose    初始化的时候读取通话记录的数据
* Params    pFileName:返回文件名, iRectType通话记录的类型(未接,已接,已拨)
* Return    当前数据条数,-1表示读取失败 
* Remarks      
**********************************************************************/
static  int     InitGetMBRecord(CallRecord * pInitMBRecord,MBRECORDTYPE CallRecType)
{
    HANDLE  hf;
    char  * pFileName = NULL;
    int     i;

    pFileName = (char*)GetFileName(CallRecType);
    if (NULL == pFileName)
        return -1;

    memset(MBRecord,0x00,sizeof(CallRecord) * iMaxPhoneRecord);
    if(INVALID_HANDLE_VALUE == (hf = CreateFile(pFileName, ACCESS_READ, 0)))
    {//未找到文件
        CloseFile(hf);
        if (INVALID_HANDLE_VALUE == //创建文件失败
            (hf = CreateFile(pFileName,ACCESS_WRITE|ACCESS_READ|CREATE_NEW|CREATE_OVERWRITE,0)))
            goto Error;

        if (-1 == SetFilePointer(hf, 0L, SEEK_SET))//设置文件指针
            goto Error;

        if (-1 == WriteFile(hf,MBRecord,sizeof(CallRecord) * iMaxPhoneRecord))
            goto Error;

        CloseFile(hf);
        memcpy(pInitMBRecord,MBRecord,sizeof(CallRecord) * iMaxPhoneRecord);
        return 0;
    }
    if (sizeof(CallRecord) * iMaxPhoneRecord != GetFileSize(hf))//文件长度非法
        goto Error;

    if (-1 == ReadFile(hf,MBRecord,sizeof(CallRecord) * iMaxPhoneRecord))//读出所有数据
        goto Error;

    CloseFile(hf);

    memcpy(pInitMBRecord,MBRecord,sizeof(CallRecord) * iMaxPhoneRecord);

    for (i = 0;i < iMaxPhoneRecord;i++)
    {
        if (strlen(MBRecord[i].PhoneNo) == 0)
            break;
    }
    return i;
Error:
    CloseFile(hf);
    DeleteFile(pFileName);
    return -1;
}
/********************************************************************
* Function   SaveMBRecordByPhoneNum  
* Purpose    保存通话记录的信息
* Params     CallRecType:保存的类型(未接,已接,已拨)
            pPhoneNum:电话号码
            pCurTime:返回当前保存的时间
* Return     保存是否成功
* Remarks      
**********************************************************************/
BOOL    SaveMBRecordByPhoneNum(MBRECORDTYPE CallRecType,const char * pPhoneNum,SYSTEMTIME * pCurTime)
{
    CallRecord  callrecord;
    SYSTEMTIME  sCurTime;

    if (CallRecType == UNANSWER_REC)
        DlmNotify(PS_SETTEL,++nUnanswered);

    if (pPhoneNum == NULL)
        return FALSE;

    if (0 == strlen(pPhoneNum))
        return FALSE;

    memset(&callrecord,0x00,sizeof(CallRecord));

    GetLocalTime(&sCurTime);
    memcpy(&callrecord.Time,&sCurTime,sizeof(SYSTEMTIME));

    if (pCurTime != NULL)
        memcpy(pCurTime,&sCurTime,sizeof(SYSTEMTIME));

    strncpy(callrecord.PhoneNo,pPhoneNum,sizeof(callrecord.PhoneNo) - 1);

    return (SaveMBRecord(&callrecord,CallRecType));
}
/********************************************************************
* Function   SaveMBRecordUsedTime  
* Purpose    保存通话计时部分
* Params     CallRecType:保存的类型(未接,已接,已拨)
            pPhoneNum:电话号码
            pInOutTime:接通时间
            pUsedTime:需要保存的通话计时
* Return     保存是否成功
* Remarks      
**********************************************************************/
BOOL    SaveMBRecordUsedTime(MBRECORDTYPE CallRecType,const char * pPhoneNum,
                             const SYSTEMTIME * pInOutTime,const SYSTEMTIME * pUsedTime)
{
    HANDLE       hf;
    char       * pFileName = NULL;
    int          i;
    CallRecord * pCurRecord;

    if ((pPhoneNum == NULL) || (pInOutTime == NULL) || (pUsedTime == NULL))
        return FALSE;

    pFileName = (char*)GetFileName(CallRecType);
    if (NULL == pFileName)
        return FALSE;

    pCurRecord = GetPointMBRecord(CallRecType);
    if (pCurRecord == NULL)
        return FALSE;

    if(INVALID_HANDLE_VALUE == (hf = CreateFile(pFileName, ACCESS_WRITE|ACCESS_READ, 0)))
    {//未找到文件，创建一个文件
        if (INVALID_HANDLE_VALUE == 
                    (hf = CreateFile(pFileName, ACCESS_WRITE|ACCESS_READ|CREATE_NEW|CREATE_OVERWRITE, 0)))
         {//创建文件失败
            CloseFile(hf);
            return FALSE;
         }
    }
    memset(MBRecord,0x00      ,sizeof(CallRecord) * iMaxPhoneRecord);
    memcpy(MBRecord,pCurRecord,sizeof(CallRecord) * iMaxPhoneRecord);
    //首先将所有数据读到MBRecord中

    if (-1 == SetFilePointer(hf, 0, SEEK_SET))//设置文件指针
        goto Error;

    for (i = 0;i < iMaxPhoneRecord;i++)
    {
        if ((pInOutTime->wYear   == MBRecord[i].Time.wYear  ) &&
            (pInOutTime->wMonth  == MBRecord[i].Time.wMonth ) &&
            (pInOutTime->wDay    == MBRecord[i].Time.wDay   ) &&
            (pInOutTime->wHour   == MBRecord[i].Time.wHour  ) &&
            (pInOutTime->wMinute == MBRecord[i].Time.wMinute) &&
            (pInOutTime->wSecond == MBRecord[i].Time.wSecond))
        {
            memcpy(&MBRecord[i].TimeUsed,pUsedTime,sizeof(SYSTEMTIME));//更新通话时间
            if (-1 == SetFilePointer(hf, sizeof(CallRecord) * i, SEEK_CUR))//设置文件指针
                goto Error;

            if (-1 == WriteFile(hf,&MBRecord[i],sizeof(CallRecord)))//保存需要存储的信息
            {
                goto Error;
            }
            else
            {
                CloseFile(hf);
                memcpy(&pCurRecord[i],&MBRecord[i],sizeof(CallRecord));//将数据写入内存
                return TRUE;
            }
        }
    }
    CloseFile(hf);
    return FALSE;
Error:
    CloseFile(hf);
    return FALSE;
}
/********************************************************************
* Function   SaveMBRecord  
* Purpose    保存通话记录的信息
* Params     pCallRecNode:数据源位置,
            CallRecType:需要读取的类型(未接,已接,已拨)
* Return     保存是否成功
* Remarks      该函数保存一个单位的通话记录
**********************************************************************/
static  BOOL    SaveMBRecord(const CallRecord * pCallRecNode, MBRECORDTYPE CallRecType)
{
    HANDLE       hf;
    char       * pFileName = NULL;
    CallRecord * pCurRecord;

    if (pCallRecNode == NULL)
        return FALSE;

    pFileName = (char*)GetFileName(CallRecType);
    if (NULL == pFileName)
        return -1;

    pCurRecord = GetPointMBRecord(CallRecType);
    if (pCurRecord == NULL)
        return FALSE;

    if(INVALID_HANDLE_VALUE == (hf = CreateFile(pFileName, ACCESS_WRITE|ACCESS_READ, 0)))
    {//未找到文件，创建一个文件
        if (INVALID_HANDLE_VALUE == 
                    (hf = CreateFile(pFileName, ACCESS_WRITE|ACCESS_READ|CREATE_NEW|CREATE_OVERWRITE, 0)))
         {//创建文件失败
            CloseFile(hf);
            return FALSE;
         }
    }
    memset(MBRecord,0x00,sizeof(CallRecord) * iMaxPhoneRecord);
    //首先将数据读到MBRecord中
#if defined(MBRECORD_SAVEDATA_UNREPEAT)//if (CallRecType != UNANSWER_REC)//已接电话,已拨电话不会保存重复的号码
    {
        int i,j;
        for (i = 0,j = 0;i < iMaxPhoneRecord;i++)
        {
            if (strlen(pCurRecord[i].PhoneNo) != 0)
                break;
            if (strcmp(pCurRecord[i].PhoneNo,pCallRecNode->PhoneNo) != 0)//号码不相等的话才进行拷贝
                memcpy(&MBRecord[j++],&pCurRecord[i],sizeof(CallRecord));
        }
    }
#else//else//未接电话保存重复的号码
    {
        memcpy(MBRecord,pCurRecord,sizeof(CallRecord) * iMaxPhoneRecord);
    }
#endif

    memcpy(pCurRecord,pCallRecNode,sizeof(CallRecord));//保存需要存储的信息
    memcpy(pCurRecord + 1,MBRecord,(iMaxPhoneRecord - 1) * sizeof(CallRecord));//保存剩余信息

    if (-1 == SetFilePointer(hf, 0, SEEK_SET))//设置文件指针
        goto Error;

    if (-1 == WriteFile(hf,(PVOID)pCallRecNode,iMaxPhoneRecord * sizeof(CallRecord)))//保存需要存储的信息
        goto Error;

    CloseFile(hf);
    return TRUE;
Error:
    CloseFile(hf);
    return FALSE;
}
/********************************************************************
* Function   DeleteMBRecord  
* Purpose    删除第几个通话记录的信息
* Params     CallRecType:需要读取的类型(未接,已接,已拨)
            index:需要删除的第几个通话记录,-1表示全部删除
* Return     删除是否成功
* Remarks      
**********************************************************************/
BOOL    DeleteMBRecord(MBRECORDTYPE CallRecType,int index)
{
    HANDLE       hf;
    char       * pFileName = NULL;
    int          i;
    CallRecord * pCurRecord;

    pFileName = (char*)GetFileName(CallRecType);
    if (NULL == pFileName)
        return -1;

    pCurRecord = GetPointMBRecord(CallRecType);
    if (pCurRecord == NULL)
        return FALSE;

    if(INVALID_HANDLE_VALUE == (hf = CreateFile(pFileName, ACCESS_WRITE|ACCESS_READ, 0)))
    {//未找到文件，创建一个文件
        if (INVALID_HANDLE_VALUE == 
                    (hf = CreateFile(pFileName, ACCESS_WRITE|ACCESS_READ|CREATE_NEW|CREATE_OVERWRITE, 0)))
         {//创建文件失败
            CloseFile(hf);
            return FALSE;
         }
    }
    memset(MBRecord,0x00      ,sizeof(CallRecord) * iMaxPhoneRecord);
    memcpy(MBRecord,pCurRecord,sizeof(CallRecord) * iMaxPhoneRecord);
    //首先将数据读到MBRecord中

    if (-1 == SetFilePointer(hf, 0, SEEK_SET))//设置文件指针
        goto Error;

    if (-1 == index)//全部删除
    {
        memset(MBRecord  ,0x00,sizeof(CallRecord) * iMaxPhoneRecord);
        memset(pCurRecord,0x00,sizeof(CallRecord) * iMaxPhoneRecord);

        if (-1 == WriteFile(hf,MBRecord,sizeof(CallRecord) * iMaxPhoneRecord))
            goto Error;
    }
    else
    {
        for (i = 0;i < iMaxPhoneRecord;i++)
        {
            if (i == index)//将需要删除的数据清零
            {
                memset(&MBRecord[i],0x00,sizeof(CallRecord));
                continue;
            }
            if (-1 == WriteFile(hf,&MBRecord[i],sizeof(CallRecord)))
                goto Error;

            memcpy(pCurRecord++,&MBRecord[i],sizeof(CallRecord));
        }
        if (-1 == WriteFile(hf,&MBRecord[index],sizeof(CallRecord)))//将清零之后的数据写入文件
            goto Error;

        memcpy(pCurRecord,&MBRecord[index],sizeof(CallRecord));
    }
    CloseFile(hf);
    return TRUE;
Error:
    CloseFile(hf);
    return FALSE;
}
/********************************************************************
* Function   DeleteSameNumber  
* Purpose    删除通话记录中相同号码的所有的信息
* Params     CallRecType:需要读取的类型(未接,已接,已拨)
            pNumber:电话号码
* Return     是否成功
* Remarks      
**********************************************************************/
BOOL    DeleteSameNumber(MBRECORDTYPE CallRecType,const char * pNumber)
{
    HANDLE       hf;
    char       * pFileName = NULL;
    int          i,j;
    CallRecord * pCurRecord;

    if (pNumber == NULL)
        return FALSE;

    if (strlen(pNumber) == 0)
        return FALSE;

    pFileName = (char*)GetFileName(CallRecType);
    if (NULL == pFileName)
        return FALSE;

    pCurRecord = GetPointMBRecord(CallRecType);
    if (pCurRecord == NULL)
        return FALSE;

    if(INVALID_HANDLE_VALUE == (hf = CreateFile(pFileName, ACCESS_WRITE|ACCESS_READ, 0)))
    {//未找到文件，创建一个文件
        if (INVALID_HANDLE_VALUE == 
                    (hf = CreateFile(pFileName, ACCESS_WRITE|ACCESS_READ|CREATE_NEW|CREATE_OVERWRITE, 0)))
         {//创建文件失败
            CloseFile(hf);
            return FALSE;
         }
    }
    memset(MBRecord,0x00,sizeof(CallRecord) * iMaxPhoneRecord);
    for (i = 0,j = 0;i < iMaxPhoneRecord;i++)//首先将数据读到MBRecord中
    {
        if (strlen(pCurRecord[i].PhoneNo) == 0)
            break;
        if (strcmp(pNumber,pCurRecord[i].PhoneNo) != 0)
            memcpy(&MBRecord[j++],&pCurRecord[i],sizeof(CallRecord));
    }
    if (-1 == SetFilePointer(hf, 0, SEEK_SET))//设置文件指针
        goto Error;
    //将数据写回内存中去
    memcpy(pCurRecord,MBRecord,sizeof(CallRecord) * iMaxPhoneRecord);
    //将过滤之后的数据写入文件
    if (-1 == WriteFile(hf,pCurRecord,sizeof(CallRecord) * iMaxPhoneRecord))
        goto Error;

    CloseFile(hf);
    return TRUE;
Error:
    CloseFile(hf);
    return FALSE;
}
/********************************************************************
* Function   DeleteSpecifyMBRecord  
* Purpose    删除某几个通话记录的信息
* Params     CallRecType:需要读取的类型(未接,已接,已拨)
            pIndex:指针数组,指针数组的长度
* Return     删除是否成功
* Remarks      
**********************************************************************/
BOOL    DeleteSpecifyMBRecord(MBRECORDTYPE CallRecType,const int * pIndex,size_t count)
{
    HANDLE       hf;
    char       * pFileName = NULL;
    int          iArrayNum,i,j,l;
    CallRecord * pCurRecord;
    BOOL         bDelete;

    pFileName = (char*)GetFileName(CallRecType);
    if (NULL == pFileName)
        return -1;

    pCurRecord = GetPointMBRecord(CallRecType);
    if (pCurRecord == NULL)
        return FALSE;

    if (INVALID_HANDLE_VALUE == 
        (hf = CreateFile(pFileName, ACCESS_WRITE|ACCESS_READ, 0)))
        return FALSE;

    memset(MBRecord,0x00,sizeof(CallRecord) * iMaxPhoneRecord);

    iArrayNum = count / sizeof(int);//数组个数
    for (i = 0,l = 0;i < iMaxPhoneRecord;i++)
    {
        bDelete = FALSE;
        if (strlen(pCurRecord[i].PhoneNo) == 0)
            break;
        for (j = 0;j < iArrayNum;j++)
        {
            if (pIndex[j] == i)//找到需要删除的索引
            {
                bDelete = TRUE;
                break;
            }
        }
        if (!bDelete)
            memcpy(&MBRecord[l++],&pCurRecord[i],sizeof(CallRecord));
    }
    memcpy(pCurRecord,MBRecord,sizeof(CallRecord) * iMaxPhoneRecord);//将数据写回内存
    if (-1 == WriteFile(hf,pCurRecord,sizeof(CallRecord) * iMaxPhoneRecord))
        goto Error;

    CloseFile(hf);
    return TRUE;
Error:
    CloseFile(hf);
    return FALSE;
}
/********************************************************************
* Function   GetMBRecord  
* Purpose    读取通话记录的信息
* Params     pCallRecNode:数据读取的目的位置,
            count:目标缓冲区的大小
            CallRecType:需要读取的类型(未接,已接,已拨)
            iDataIndex:需要读取第几个通话记录,取值为-1的时候,返回所有
* Return     返回-1,数据读取失败;读取成功返回当前有多少条数据
* Remarks      
**********************************************************************/
int     GetMBRecord(CallRecord * pCallRecNode,size_t count,MBRECORDTYPE CallRecType,int iDataIndex)
{
    CallRecord * pCurRecord;
    int i;

    if (NULL == pCallRecNode)
        return -1;

    //目标缓冲区的大小不是单个记录也不是全部记录
    if ((count != sizeof(CallRecord)) && (count != (sizeof(CallRecord) * iMaxPhoneRecord)))
        return -1;

    if ((iDataIndex > iMaxPhoneRecord - 1) || (iDataIndex < -1))//需要读取的通话记录位置非法
        return -1;

    pCurRecord = GetPointMBRecord(CallRecType);
    if (pCurRecord == NULL)
        return -1;

    if (-1 == iDataIndex)
        memcpy(pCallRecNode,pCurRecord,count);
    else
        memcpy(pCallRecNode,&pCurRecord[iDataIndex],count);

    for (i = 0;i < iMaxPhoneRecord;i++)
    {
        if (strlen(pCurRecord[i].PhoneNo) == 0)
            break;
    }
    return i;
}
/********************************************************************
* Function   GetRecorderNumber  
* Purpose    返回通话记录的条数
* Params     CallRecType:需要读取的类型(未接,已接,已拨)
* Return     
* Remarks      
**********************************************************************/
int     GetRecorderNumber(MBRECORDTYPE CallRecType)
{
    int i,iRNum = 0;
    CallRecord * pCurRecord;

    pCurRecord = GetPointMBRecord(CallRecType);
    if (pCurRecord == NULL)
        return -1;

    for (i = 0;i < iMaxPhoneRecord;i++)
    {
        if (strlen(pCurRecord[i].PhoneNo) != 0)
            iRNum++;
        else
            break;
    }
    return (iRNum);
}
//////////////////////////////////////////////////////////////////////////
/********************************************************************
* Function     SaveRecord_Cost
* Purpose      保存通话信息
* Params       
* Return           
* Remarks      
**********************************************************************/
BOOL    SaveRecord_Cost(const MBRecordCostTime * pCost)
{
    HANDLE hFile;

    if (NULL == pCost)
        return FALSE;

    hFile = CreateFile(pRECCOST_LOG, ACCESS_WRITE, 0);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        hFile = CreateFile(pRECCOST_LOG, CREATE_NEW|ACCESS_WRITE, 0);
        if (hFile == INVALID_HANDLE_VALUE)
            return FALSE;
    }

    if (-1 == WriteFile(hFile, ( PVOID)pCost, sizeof(MBRecordCostTime)))
    {
        CloseFile(hFile);
        return FALSE;
    }
    CloseFile(hFile);

    memcpy(&sRecordCostTime,pCost,sizeof(MBRecordCostTime));//将通话信息保存在内存中
    return TRUE;
}
/********************************************************************
* Function     ReadCostParaFile
* Purpose      读取通话信息
* Params       
* Return           
* Remarks      
**********************************************************************/
BOOL    ReadRecord_Cost(MBRecordCostTime * pCost)
{
    HANDLE  hFile;
    static BOOL bInit = FALSE;//是否已经从文件中读取通话信息

    if (NULL == pCost)
        return FALSE;

    if (bInit)
    {
        memcpy(pCost,&sRecordCostTime,sizeof(MBRecordCostTime));//从内存中读取数据
        return TRUE;
    }
    hFile = CreateFile(pRECCOST_LOG, ACCESS_READ, 0);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        hFile = CreateFile(pRECCOST_LOG, CREATE_NEW|ACCESS_WRITE, 0);
        if (hFile == INVALID_HANDLE_VALUE)
            return FALSE;

        memset(pCost, 0, sizeof(MBRecordCostTime));

        if (-1 == WriteFile(hFile, pCost, sizeof(MBRecordCostTime)))
            goto Error;

        CloseFile(hFile);
        memcpy(&sRecordCostTime,pCost,sizeof(MBRecordCostTime));//将通话信息保存在内存中
        return TRUE;
    }
    if (sizeof(MBRecordCostTime) != GetFileSize(hFile))//文件长度非法
        goto Error;

    if (-1 == ReadFile(hFile, pCost, sizeof(MBRecordCostTime)))
        goto Error;

    CloseFile(hFile);

    memcpy(&sRecordCostTime,pCost,sizeof(MBRecordCostTime));//将通话信息保存在内存中
    bInit = TRUE;//已经从文件中读取通话信息

    return TRUE;

Error:
    CloseFile(hFile);
    DeleteFile(pRECCOST_LOG);
    return FALSE;
}
/********************************************************************
* Function     ResetnUnanswered
* Purpose      未接电话个数复位
* Params      
* Return           
* Remarks      
**********************************************************************/
void    ResetnUnanswered(void)
{
    nUnanswered = 0;
    DlmNotify(PS_SETTEL,nUnanswered);
}
//////////////////////////////////////////////////////////////////////////
int     APP_GetFrqntNo(char* pFrqntNo)
{
    int  i,iSour;
    char * pNo = NULL;

    memset(MBRecord,0x00,sizeof(CallRecord) * iMaxPhoneRecord);
    GetMBRecord(MBRecord,sizeof(CallRecord) * iMaxPhoneRecord,DIALED_REC,-1);

    pNo = pFrqntNo;
    for (i = 0;i < iMaxPhoneRecord;i++)
    {
        iSour = strlen(MBRecord[i].PhoneNo);
        if (0 == iSour)
            break;
        strncat(pNo,MBRecord[i].PhoneNo,DB_PNOLEN > iSour ? iSour + 1 : DB_PNOLEN - 1);
        pNo += DB_PNOLEN;
    }
    return (i);
}
