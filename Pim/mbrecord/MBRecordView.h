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

typedef enum//窗口返回值
{
    MBRECORD_RS_MODIFY      = 2,    //本电话已经修改了姓名
    MBRECORD_RS_DIALUP      = 1,    //本电话已经拨出
    MBRECORD_RS_NONE        = 0,    //没有删除或者没有拨出
    MBRECORD_RS_DELETE      = -1,   //本条数据已经删除
    MBRECORD_RS_CREATEFAIL  = -2    //窗口创建失败
}MBRECORD_RS;

#define MBRECORD_TITLEMAXLEN            20//窗口标题最大长度
#define MBRECORD_CLEW_NUMMAXLEN         10//号码标题最大长度
#define MBRECORD_CLEW_NAMEMAXLEN        10//姓名标题最大长度
#define MBRECORD_CLEW_TIMEMAXLEN        10//时间标题最大长度
#define MBRECORD_CLEW_DURATIONMAXLEN    10//计时标题最大长度
#define MBRECORD_CLEW_UNKNOWPEOPLE      10//未知姓名的显示文字的最大长度
typedef struct//标题内容信息
{
    char cTitle[MBRECORD_TITLEMAXLEN + 1];              //窗口标题
    char cNumber[MBRECORD_CLEW_NAMEMAXLEN + 1];         //号码标题
    char cName[MBRECORD_CLEW_NAMEMAXLEN + 1];           //姓名标题
    char cTime[MBRECORD_CLEW_TIMEMAXLEN + 1];           //时间标题
    char cDuration[MBRECORD_CLEW_DURATIONMAXLEN + 1];   //计时标题
}TITLEINFO;

typedef enum//标题风格
{
    MBV_WS_NUMBER   = 0x01,//是否显示号码标题
    MBV_WS_NAME     = 0x02,//是否显示姓名标题
    MBV_WS_TIME     = 0x04,//是否显示时间标题
    MBV_WS_DURATION = 0x08 //是否显示通话计时标题
}ITEMSTYLE;

typedef enum//时间显示风格
{
    TS_NORMAL,          //年月日时分
    TS_NORMAL_WEEK,     //年月日星期时分
    TS_NORMAL_WEEK_M,   //年月日星期时分+上下午
    TS_NORMAL_M         //年月日时分+上下午
}TS_TIMESTYLE;

typedef enum
{
    MBV_WS_DEFAULT       = 0x00,//默认风格
    MBV_WS_BUTTON_MIDDLE = 0x01 //包含中间键
}MBRECORDVIEW_STYLE;//窗口风格

typedef struct
{
    HWND            hWnd;           //父窗口句柄
    unsigned int    index;          //查看的数据在文本框中的索引(也就是文件中的位置)
    MBRECORDTYPE    MBRecordType;   //需要查看的类型(未接电话,已接电话,已拨电话)
    TITLEINFO       TitleInfo;      //标题信息
    DWORD           ItemStyle;      //标题风格(可以使用"|")
    TS_TIMESTYLE    TimeStyle;      //时间风格(不可以使用"|")
    char            cUnknow[MBRECORD_CLEW_UNKNOWPEOPLE + 1];//未知电话的显示文字
    DWORD           MBRecordView_Style;//窗口风格(可以使用"|")
}MBRECORD_VIEW;

MBRECORD_RS CallMBLogViewWindow(const MBRECORD_VIEW * pMBRecord_Type);

#endif
