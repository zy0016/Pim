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
#ifndef _MBRECORDVIEW_
#define _MBRECORDVIEW_

#include "RcCommon.h"

typedef enum//���ڷ���ֵ
{
    MBRECORD_RS_MODIFY      = 2,    //���绰�Ѿ��޸�������
    MBRECORD_RS_DIALUP      = 1,    //���绰�Ѿ�����
    MBRECORD_RS_NONE        = 0,    //û��ɾ������û�в���
    MBRECORD_RS_DELETE      = -1,   //���������Ѿ�ɾ��
    MBRECORD_RS_CREATEFAIL  = -2    //���ڴ���ʧ��
}MBRECORD_RS;

#define MBRECORD_TITLEMAXLEN            20//���ڱ�����󳤶�
#define MBRECORD_CLEW_NUMMAXLEN         10//���������󳤶�
#define MBRECORD_CLEW_NAMEMAXLEN        10//����������󳤶�
#define MBRECORD_CLEW_TIMEMAXLEN        10//ʱ�������󳤶�
#define MBRECORD_CLEW_DURATIONMAXLEN    10//��ʱ������󳤶�
#define MBRECORD_CLEW_UNKNOWPEOPLE      10//δ֪��������ʾ���ֵ���󳤶�
typedef struct//����������Ϣ
{
    char cTitle[MBRECORD_TITLEMAXLEN + 1];              //���ڱ���
    char cNumber[MBRECORD_CLEW_NAMEMAXLEN + 1];         //�������
    char cName[MBRECORD_CLEW_NAMEMAXLEN + 1];           //��������
    char cTime[MBRECORD_CLEW_TIMEMAXLEN + 1];           //ʱ�����
    char cDuration[MBRECORD_CLEW_DURATIONMAXLEN + 1];   //��ʱ����
}TITLEINFO;

typedef enum//������
{
    MBV_WS_NUMBER   = 0x01,//�Ƿ���ʾ�������
    MBV_WS_NAME     = 0x02,//�Ƿ���ʾ��������
    MBV_WS_TIME     = 0x04,//�Ƿ���ʾʱ�����
    MBV_WS_DURATION = 0x08 //�Ƿ���ʾͨ����ʱ����
}ITEMSTYLE;

typedef enum//ʱ����ʾ���
{
    TS_NORMAL,          //������ʱ��
    TS_NORMAL_WEEK,     //����������ʱ��
    TS_NORMAL_WEEK_M,   //����������ʱ��+������
    TS_NORMAL_M         //������ʱ��+������
}TS_TIMESTYLE;

typedef enum
{
    MBV_WS_DEFAULT       = 0x00,//Ĭ�Ϸ��
    MBV_WS_BUTTON_MIDDLE = 0x01 //�����м��
}MBRECORDVIEW_STYLE;//���ڷ��

typedef struct
{
    HWND            hWnd;           //�����ھ��
    unsigned int    index;          //�鿴���������ı����е�����(Ҳ�����ļ��е�λ��)
    MBRECORDTYPE    MBRecordType;   //��Ҫ�鿴������(δ�ӵ绰,�ѽӵ绰,�Ѳ��绰)
    TITLEINFO       TitleInfo;      //������Ϣ
    DWORD           ItemStyle;      //������(����ʹ��"|")
    TS_TIMESTYLE    TimeStyle;      //ʱ����(������ʹ��"|")
    char            cUnknow[MBRECORD_CLEW_UNKNOWPEOPLE + 1];//δ֪�绰����ʾ����
    DWORD           MBRecordView_Style;//���ڷ��(����ʹ��"|")
}MBRECORD_VIEW;

MBRECORD_RS CallMBLogViewWindow(const MBRECORD_VIEW * pMBRecord_Type);

#endif
