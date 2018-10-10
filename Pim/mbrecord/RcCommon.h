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
    UNANSWER_REC = 0x01,   //未接电话
    ANSWERED_REC = 0x02,   //已接电话
    DIALED_REC   = 0x04    //已拨电话
}MBRECORDTYPE;

#define PIN2MAXLEN          10              //pin2码最大长度
#define MAX_PHONE_RECORD    20              //保存的通话记录个数
typedef struct
{
    SYSTEMTIME  Time;                       //接通时间
    SYSTEMTIME  TimeUsed;                   //通话时间
    char        PhoneNo[PHONENUMMAXLEN + 1];//通话号码
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
BOOL    InitMBRecordData(void);//初始化通话记录所需数据
BOOL    CallMBRecordCostWindow(HWND hwndCall);//通话记时
BOOL    InitMBPostMsg(void);//初始化电话部分
BOOL    CallMBRecordFare(HWND hwndCall);//通话计费
BOOL    CallValidPin2(HWND hwndCall);//验证pin2码
BOOL    CallSetCost(HWND hwndCall);//话费设定
BOOL    CallChargingLimitWindow(HWND hwndCall);//话费限定
BOOL    CallChargingTypeWindow(HWND hwndCall);//计费方式
void    GetValidPin2(char * pPin2);//获得正确的pin2码
void    SetPin2Change(void);//表示PIN2码已经修改
void    MBRecrodList_Refresh(MBRECORDTYPE MBRecord_Type);//发送刷新列表的消息
void    ResetnUnanswered(void);//未接电话个数复位
BOOL    SaveMBRecordByPhoneNum(MBRECORDTYPE CallRecType,const char * pPhoneNum,SYSTEMTIME * pCurTime);//保存通话记录的信息
BOOL    SaveMBRecordUsedTime(MBRECORDTYPE CallRecType,const char * pPhoneNum,//保存通话计时部分
                             const SYSTEMTIME * pInOutTime,const SYSTEMTIME * pUsedTime);
BOOL    DeleteMBRecord(MBRECORDTYPE CallRecType,int index);//删除第几个通话记录的信息
BOOL    DeleteSameNumber(MBRECORDTYPE CallRecType,const char * pNumber);//删除通话记录中相同号码的所有的信息
int     GetMBRecord(CallRecord * pCallRecNode,size_t count,MBRECORDTYPE CallRecType,int iDataIndex);//读取通话记录的信息
int     GetRecorderNumber(MBRECORDTYPE CallRecType);//返回通话记录的条数

BOOL    ReadRecord_Cost(MBRecordCostTime * pCost);//读取通话信息
BOOL    SaveRecord_Cost(const MBRecordCostTime * pCost);//保存通话信息

int     APP_EditSMS(const char* pszReciever, const char* pszContent);
void    CallMMSEditWnd(char *fileName, HWND hwnd, UINT msg, UINT nKind, PSTR name);

#endif //_RC_COMMON_H_
