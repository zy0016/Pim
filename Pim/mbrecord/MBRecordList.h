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
#ifndef _MBRECORDLIST_
#define _MBRECORDLIST_

typedef enum
{
    MBL_WS_DEFAULT                  = 0x00, //Ĭ�Ϸ��
    MBL_WS_COUNT                    = 0x01, //�ڱ�������ʾ�绰����
    MBL_WS_NOVSCROLL                = 0x02, //����ʾ������
    MBL_WS_MIDDLEBUTTON             = 0x04, //��ʾ�м��
    MBL_WS_NODATA_SHOWMESSAGEBOX    = 0x08, //û�����ݵ�ʱ��ֻ��ʾ��ʾ��,�����봰��,��ʾ������ʾ"�޺����¼"
    MBL_WS_NODATA_NOTSHOWLIST       = 0x16, //û�����ݵ�ʱ����ʾ�б��,ֻ�ڽ�������ʾ"�޺����¼"
    MBL_WS_NOSHOWREPEATDATA         = 0x32  //�б��в���ʾ�����ظ�������(�����ļ��б����к����ظ��ļ�¼)
}MBRECORDLIST_STYLE;//�б�����

//hwndCall:�����ھ��
//dtype:ͨ����¼������(δ��,�ѽ�,�Ѳ�),����ʹ��"|",֧����������,��ȫ������Ļ��ϵ
//MBRecordList_Style:�б�����
BOOL    CallMBLogListExtWindow(HWND hwndCall,DWORD dtype,DWORD MBRecordList_Style);

#endif
