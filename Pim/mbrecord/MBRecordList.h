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
    MBL_WS_DEFAULT                  = 0x00, //默认风格
    MBL_WS_COUNT                    = 0x01, //在标题栏显示电话个数
    MBL_WS_NOVSCROLL                = 0x02, //不显示滚动条
    MBL_WS_MIDDLEBUTTON             = 0x04, //显示中间键
    MBL_WS_NODATA_SHOWMESSAGEBOX    = 0x08, //没有数据的时候只显示提示框,不进入窗口,提示框中显示"无号码记录"
    MBL_WS_NODATA_NOTSHOWLIST       = 0x16, //没有数据的时候不显示列表框,只在界面上显示"无号码记录"
    MBL_WS_NOSHOWREPEATDATA         = 0x32  //列表中不显示号码重复的数据(但是文件中保存有号码重复的记录)
}MBRECORDLIST_STYLE;//列表项风格

//hwndCall:父窗口句柄
//dtype:通话记录的类型(未接,已接,已拨),可以使用"|",支持任意两项,和全部三项的或关系
//MBRecordList_Style:列表项风格
BOOL    CallMBLogListExtWindow(HWND hwndCall,DWORD dtype,DWORD MBRecordList_Style);

#endif
