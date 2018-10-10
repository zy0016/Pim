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
#ifndef _RC_COMMON_H_
#define _RC_COMMON_H_

#include "winpda.h"
#include "stdio.h"
#include "stdlib.h"
#include "fapi.h"
#include "../../public/setup/setup.h"
#include "../mbphone/MBPublic.h"

#ifndef MMS_CALLEDIT_MOBIL
#define MMS_CALLEDIT_MOBIL 8 
#endif

typedef enum
{
    UNANSWER_REC = 0x01,   //δ�ӵ绰
    ANSWERED_REC = 0x02,   //�ѽӵ绰
    DIALED_REC   = 0x04    //�Ѳ��绰
}MBRECORDTYPE;

#define PIN2MAXLEN          10              //pin2����󳤶�
#define MAX_PHONE_RECORD    20              //�����ͨ����¼����
typedef struct
{
    SYSTEMTIME  Time;                       //��ͨʱ��
    SYSTEMTIME  TimeUsed;                   //ͨ��ʱ��
    char        PhoneNo[PHONENUMMAXLEN + 1];//ͨ������
}CallRecord;

typedef struct
{
    int nLastHour;
    int nLastMin;
    int nLastSecond;
    int nOutHour;
    int nOutMin;
    int nOutSecond;
    int nInHour;
    int nInMin;
    int nInSecond;
}MBRecordCostTime;
BOOL    InitMBRecordData(void);//��ʼ��ͨ����¼��������
BOOL    CallMBRecordCostWindow(HWND hwndCall);//ͨ����ʱ
BOOL    InitMBPostMsg(void);//��ʼ���绰����
BOOL    CallMBRecordFare(HWND hwndCall);//ͨ���Ʒ�
BOOL    CallValidPin2(HWND hwndCall);//��֤pin2��
BOOL    CallSetCost(HWND hwndCall);//�����趨
BOOL    CallChargingLimitWindow(HWND hwndCall);//�����޶�
BOOL    CallChargingTypeWindow(HWND hwndCall);//�Ʒѷ�ʽ
void    GetValidPin2(char * pPin2);//�����ȷ��pin2��
void    SetPin2Change(void);//��ʾPIN2���Ѿ��޸�
void    MBRecrodList_Refresh(MBRECORDTYPE MBRecord_Type);//����ˢ���б����Ϣ
void    ResetnUnanswered(void);//δ�ӵ绰������λ
BOOL    SaveMBRecordByPhoneNum(MBRECORDTYPE CallRecType,const char * pPhoneNum,SYSTEMTIME * pCurTime);//����ͨ����¼����Ϣ
BOOL    SaveMBRecordUsedTime(MBRECORDTYPE CallRecType,const char * pPhoneNum,//����ͨ����ʱ����
                             const SYSTEMTIME * pInOutTime,const SYSTEMTIME * pUsedTime);
BOOL    DeleteMBRecord(MBRECORDTYPE CallRecType,int index);//ɾ���ڼ���ͨ����¼����Ϣ
BOOL    DeleteSameNumber(MBRECORDTYPE CallRecType,const char * pNumber);//ɾ��ͨ����¼����ͬ��������е���Ϣ
int     GetMBRecord(CallRecord * pCallRecNode,size_t count,MBRECORDTYPE CallRecType,int iDataIndex);//��ȡͨ����¼����Ϣ
int     GetRecorderNumber(MBRECORDTYPE CallRecType);//����ͨ����¼������

BOOL    ReadRecord_Cost(MBRecordCostTime * pCost);//��ȡͨ����Ϣ
BOOL    SaveRecord_Cost(const MBRecordCostTime * pCost);//����ͨ����Ϣ

int     APP_EditSMS(const char* pszReciever, const char* pszContent);
void    CallMMSEditWnd(char *fileName, HWND hwnd, UINT msg, UINT nKind, PSTR name);

#endif //_RC_COMMON_H_
