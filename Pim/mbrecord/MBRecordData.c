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

//#define MBRECORD_SAVEDATA_UNREPEAT//���������֮��,����ͨ����¼��ʱ��Ͳ��ᱣ���ظ��ĺ���

static const int    iMaxPhoneRecord = MAX_PHONE_RECORD;
static const char * pUNANSWER_LOG   = "FLASH2:/UnAnswerLog.dat";
static const char * pANSWERED_LOG   = "FLASH2:/AnsweredLog.dat";
static const char * pDIALED_LOG     = "FLASH2:/DialedLong.dat";
static const char * pRECCOST_LOG    = "FLASH2:/RecCostLog.dat";

static CallRecord * MBRecord;   //��дͨ����¼��ʱ���õ����м����(����1480�ֽ�)
static CallRecord * pRecordUnAn;//����δ����������(����1480�ֽ�)
static CallRecord * pRecordAns; //�����ѽ���������(����1480�ֽ�)
static CallRecord * pRecordDial;//�����Ѳ���������(����1480�ֽ�)

static MBRecordCostTime sRecordCostTime;//��ͨ����Ϣ�������ڴ���
static int              nUnanswered = 0;//δӦ�������ĸ���

static  BOOL    SaveMBRecord(const CallRecord * pCallRecNode,MBRECORDTYPE CallRecType);
static  int     InitGetMBRecord(CallRecord * pInitMBRecord,MBRECORDTYPE CallRecType);
/********************************************************************
* Function   GetFileName  
* Purpose    ������Ҫ��ͨ����¼���ļ���
* Params    pFileName:�����ļ���, iRectTypeͨ����¼������(δ��,�ѽ�,�Ѳ�)
* Return     �Ƿ��ǺϷ���ͨ����¼������
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
* Purpose    ������Ҫ��ͨ����¼��ָ��
* Params    iRectTypeͨ����¼������(δ��,�ѽ�,�Ѳ�)
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
* Purpose    ��ʼ��ͨ����¼��������
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
        printf("mb MBRecord mallocʧ��\r\n");
#endif
        return FALSE;
    }

    pRecordUnAn = (CallRecord*)malloc(sizeof(CallRecord) * iMaxPhoneRecord);
    if (NULL == pRecordUnAn)
    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRECORDDATA)
        printf("mb pRecordUnAn mallocʧ��\r\n");
#endif
        return FALSE;
    }

    pRecordAns = (CallRecord*)malloc(sizeof(CallRecord) * iMaxPhoneRecord);
    if (NULL == pRecordAns)
    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRECORDDATA)
        printf("mb pRecordAns mallocʧ��\r\n");
#endif
        return FALSE;
    }

    pRecordDial = (CallRecord*)malloc(sizeof(CallRecord) * iMaxPhoneRecord);
    if (NULL == pRecordDial)
    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRECORDDATA)
        printf("mb pRecordDial mallocʧ��\r\n");
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
* Purpose    ��ʼ����ʱ���ȡͨ����¼������
* Params    pFileName:�����ļ���, iRectTypeͨ����¼������(δ��,�ѽ�,�Ѳ�)
* Return    ��ǰ��������,-1��ʾ��ȡʧ�� 
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
    {//δ�ҵ��ļ�
        CloseFile(hf);
        if (INVALID_HANDLE_VALUE == //�����ļ�ʧ��
            (hf = CreateFile(pFileName,ACCESS_WRITE|ACCESS_READ|CREATE_NEW|CREATE_OVERWRITE,0)))
            goto Error;

        if (-1 == SetFilePointer(hf, 0L, SEEK_SET))//�����ļ�ָ��
            goto Error;

        if (-1 == WriteFile(hf,MBRecord,sizeof(CallRecord) * iMaxPhoneRecord))
            goto Error;

        CloseFile(hf);
        memcpy(pInitMBRecord,MBRecord,sizeof(CallRecord) * iMaxPhoneRecord);
        return 0;
    }
    if (sizeof(CallRecord) * iMaxPhoneRecord != GetFileSize(hf))//�ļ����ȷǷ�
        goto Error;

    if (-1 == ReadFile(hf,MBRecord,sizeof(CallRecord) * iMaxPhoneRecord))//������������
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
* Purpose    ����ͨ����¼����Ϣ
* Params     CallRecType:���������(δ��,�ѽ�,�Ѳ�)
            pPhoneNum:�绰����
            pCurTime:���ص�ǰ�����ʱ��
* Return     �����Ƿ�ɹ�
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
* Purpose    ����ͨ����ʱ����
* Params     CallRecType:���������(δ��,�ѽ�,�Ѳ�)
            pPhoneNum:�绰����
            pInOutTime:��ͨʱ��
            pUsedTime:��Ҫ�����ͨ����ʱ
* Return     �����Ƿ�ɹ�
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
    {//δ�ҵ��ļ�������һ���ļ�
        if (INVALID_HANDLE_VALUE == 
                    (hf = CreateFile(pFileName, ACCESS_WRITE|ACCESS_READ|CREATE_NEW|CREATE_OVERWRITE, 0)))
         {//�����ļ�ʧ��
            CloseFile(hf);
            return FALSE;
         }
    }
    memset(MBRecord,0x00      ,sizeof(CallRecord) * iMaxPhoneRecord);
    memcpy(MBRecord,pCurRecord,sizeof(CallRecord) * iMaxPhoneRecord);
    //���Ƚ��������ݶ���MBRecord��

    if (-1 == SetFilePointer(hf, 0, SEEK_SET))//�����ļ�ָ��
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
            memcpy(&MBRecord[i].TimeUsed,pUsedTime,sizeof(SYSTEMTIME));//����ͨ��ʱ��
            if (-1 == SetFilePointer(hf, sizeof(CallRecord) * i, SEEK_CUR))//�����ļ�ָ��
                goto Error;

            if (-1 == WriteFile(hf,&MBRecord[i],sizeof(CallRecord)))//������Ҫ�洢����Ϣ
            {
                goto Error;
            }
            else
            {
                CloseFile(hf);
                memcpy(&pCurRecord[i],&MBRecord[i],sizeof(CallRecord));//������д���ڴ�
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
* Purpose    ����ͨ����¼����Ϣ
* Params     pCallRecNode:����Դλ��,
            CallRecType:��Ҫ��ȡ������(δ��,�ѽ�,�Ѳ�)
* Return     �����Ƿ�ɹ�
* Remarks      �ú�������һ����λ��ͨ����¼
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
    {//δ�ҵ��ļ�������һ���ļ�
        if (INVALID_HANDLE_VALUE == 
                    (hf = CreateFile(pFileName, ACCESS_WRITE|ACCESS_READ|CREATE_NEW|CREATE_OVERWRITE, 0)))
         {//�����ļ�ʧ��
            CloseFile(hf);
            return FALSE;
         }
    }
    memset(MBRecord,0x00,sizeof(CallRecord) * iMaxPhoneRecord);
    //���Ƚ����ݶ���MBRecord��
#if defined(MBRECORD_SAVEDATA_UNREPEAT)//if (CallRecType != UNANSWER_REC)//�ѽӵ绰,�Ѳ��绰���ᱣ���ظ��ĺ���
    {
        int i,j;
        for (i = 0,j = 0;i < iMaxPhoneRecord;i++)
        {
            if (strlen(pCurRecord[i].PhoneNo) != 0)
                break;
            if (strcmp(pCurRecord[i].PhoneNo,pCallRecNode->PhoneNo) != 0)//���벻��ȵĻ��Ž��п���
                memcpy(&MBRecord[j++],&pCurRecord[i],sizeof(CallRecord));
        }
    }
#else//else//δ�ӵ绰�����ظ��ĺ���
    {
        memcpy(MBRecord,pCurRecord,sizeof(CallRecord) * iMaxPhoneRecord);
    }
#endif

    memcpy(pCurRecord,pCallRecNode,sizeof(CallRecord));//������Ҫ�洢����Ϣ
    memcpy(pCurRecord + 1,MBRecord,(iMaxPhoneRecord - 1) * sizeof(CallRecord));//����ʣ����Ϣ

    if (-1 == SetFilePointer(hf, 0, SEEK_SET))//�����ļ�ָ��
        goto Error;

    if (-1 == WriteFile(hf,(PVOID)pCallRecNode,iMaxPhoneRecord * sizeof(CallRecord)))//������Ҫ�洢����Ϣ
        goto Error;

    CloseFile(hf);
    return TRUE;
Error:
    CloseFile(hf);
    return FALSE;
}
/********************************************************************
* Function   DeleteMBRecord  
* Purpose    ɾ���ڼ���ͨ����¼����Ϣ
* Params     CallRecType:��Ҫ��ȡ������(δ��,�ѽ�,�Ѳ�)
            index:��Ҫɾ���ĵڼ���ͨ����¼,-1��ʾȫ��ɾ��
* Return     ɾ���Ƿ�ɹ�
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
    {//δ�ҵ��ļ�������һ���ļ�
        if (INVALID_HANDLE_VALUE == 
                    (hf = CreateFile(pFileName, ACCESS_WRITE|ACCESS_READ|CREATE_NEW|CREATE_OVERWRITE, 0)))
         {//�����ļ�ʧ��
            CloseFile(hf);
            return FALSE;
         }
    }
    memset(MBRecord,0x00      ,sizeof(CallRecord) * iMaxPhoneRecord);
    memcpy(MBRecord,pCurRecord,sizeof(CallRecord) * iMaxPhoneRecord);
    //���Ƚ����ݶ���MBRecord��

    if (-1 == SetFilePointer(hf, 0, SEEK_SET))//�����ļ�ָ��
        goto Error;

    if (-1 == index)//ȫ��ɾ��
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
            if (i == index)//����Ҫɾ������������
            {
                memset(&MBRecord[i],0x00,sizeof(CallRecord));
                continue;
            }
            if (-1 == WriteFile(hf,&MBRecord[i],sizeof(CallRecord)))
                goto Error;

            memcpy(pCurRecord++,&MBRecord[i],sizeof(CallRecord));
        }
        if (-1 == WriteFile(hf,&MBRecord[index],sizeof(CallRecord)))//������֮�������д���ļ�
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
* Purpose    ɾ��ͨ����¼����ͬ��������е���Ϣ
* Params     CallRecType:��Ҫ��ȡ������(δ��,�ѽ�,�Ѳ�)
            pNumber:�绰����
* Return     �Ƿ�ɹ�
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
    {//δ�ҵ��ļ�������һ���ļ�
        if (INVALID_HANDLE_VALUE == 
                    (hf = CreateFile(pFileName, ACCESS_WRITE|ACCESS_READ|CREATE_NEW|CREATE_OVERWRITE, 0)))
         {//�����ļ�ʧ��
            CloseFile(hf);
            return FALSE;
         }
    }
    memset(MBRecord,0x00,sizeof(CallRecord) * iMaxPhoneRecord);
    for (i = 0,j = 0;i < iMaxPhoneRecord;i++)//���Ƚ����ݶ���MBRecord��
    {
        if (strlen(pCurRecord[i].PhoneNo) == 0)
            break;
        if (strcmp(pNumber,pCurRecord[i].PhoneNo) != 0)
            memcpy(&MBRecord[j++],&pCurRecord[i],sizeof(CallRecord));
    }
    if (-1 == SetFilePointer(hf, 0, SEEK_SET))//�����ļ�ָ��
        goto Error;
    //������д���ڴ���ȥ
    memcpy(pCurRecord,MBRecord,sizeof(CallRecord) * iMaxPhoneRecord);
    //������֮�������д���ļ�
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
* Purpose    ɾ��ĳ����ͨ����¼����Ϣ
* Params     CallRecType:��Ҫ��ȡ������(δ��,�ѽ�,�Ѳ�)
            pIndex:ָ������,ָ������ĳ���
* Return     ɾ���Ƿ�ɹ�
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

    iArrayNum = count / sizeof(int);//�������
    for (i = 0,l = 0;i < iMaxPhoneRecord;i++)
    {
        bDelete = FALSE;
        if (strlen(pCurRecord[i].PhoneNo) == 0)
            break;
        for (j = 0;j < iArrayNum;j++)
        {
            if (pIndex[j] == i)//�ҵ���Ҫɾ��������
            {
                bDelete = TRUE;
                break;
            }
        }
        if (!bDelete)
            memcpy(&MBRecord[l++],&pCurRecord[i],sizeof(CallRecord));
    }
    memcpy(pCurRecord,MBRecord,sizeof(CallRecord) * iMaxPhoneRecord);//������д���ڴ�
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
* Purpose    ��ȡͨ����¼����Ϣ
* Params     pCallRecNode:���ݶ�ȡ��Ŀ��λ��,
            count:Ŀ�껺�����Ĵ�С
            CallRecType:��Ҫ��ȡ������(δ��,�ѽ�,�Ѳ�)
            iDataIndex:��Ҫ��ȡ�ڼ���ͨ����¼,ȡֵΪ-1��ʱ��,��������
* Return     ����-1,���ݶ�ȡʧ��;��ȡ�ɹ����ص�ǰ�ж���������
* Remarks      
**********************************************************************/
int     GetMBRecord(CallRecord * pCallRecNode,size_t count,MBRECORDTYPE CallRecType,int iDataIndex)
{
    CallRecord * pCurRecord;
    int i;

    if (NULL == pCallRecNode)
        return -1;

    //Ŀ�껺�����Ĵ�С���ǵ�����¼Ҳ����ȫ����¼
    if ((count != sizeof(CallRecord)) && (count != (sizeof(CallRecord) * iMaxPhoneRecord)))
        return -1;

    if ((iDataIndex > iMaxPhoneRecord - 1) || (iDataIndex < -1))//��Ҫ��ȡ��ͨ����¼λ�÷Ƿ�
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
* Purpose    ����ͨ����¼������
* Params     CallRecType:��Ҫ��ȡ������(δ��,�ѽ�,�Ѳ�)
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
* Purpose      ����ͨ����Ϣ
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

    memcpy(&sRecordCostTime,pCost,sizeof(MBRecordCostTime));//��ͨ����Ϣ�������ڴ���
    return TRUE;
}
/********************************************************************
* Function     ReadCostParaFile
* Purpose      ��ȡͨ����Ϣ
* Params       
* Return           
* Remarks      
**********************************************************************/
BOOL    ReadRecord_Cost(MBRecordCostTime * pCost)
{
    HANDLE  hFile;
    static BOOL bInit = FALSE;//�Ƿ��Ѿ����ļ��ж�ȡͨ����Ϣ

    if (NULL == pCost)
        return FALSE;

    if (bInit)
    {
        memcpy(pCost,&sRecordCostTime,sizeof(MBRecordCostTime));//���ڴ��ж�ȡ����
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
        memcpy(&sRecordCostTime,pCost,sizeof(MBRecordCostTime));//��ͨ����Ϣ�������ڴ���
        return TRUE;
    }
    if (sizeof(MBRecordCostTime) != GetFileSize(hFile))//�ļ����ȷǷ�
        goto Error;

    if (-1 == ReadFile(hFile, pCost, sizeof(MBRecordCostTime)))
        goto Error;

    CloseFile(hFile);

    memcpy(&sRecordCostTime,pCost,sizeof(MBRecordCostTime));//��ͨ����Ϣ�������ڴ���
    bInit = TRUE;//�Ѿ����ļ��ж�ȡͨ����Ϣ

    return TRUE;

Error:
    CloseFile(hFile);
    DeleteFile(pRECCOST_LOG);
    return FALSE;
}
/********************************************************************
* Function     ResetnUnanswered
* Purpose      δ�ӵ绰������λ
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
