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
#ifndef _MBLIST_
#define _MBLIST_
//控件消息
#define MBLIST_SELCHANGE        1//改变滚动条选项的消息
#define MBLIST_DBLCLK           2//双击操作

//控件风格
#define MBLIST_WS_DISABLE       1//禁止选择
#define MBLIST_WS_ENABLED       2//允许选择

#define MBLIST_IPIC_UNCHANGE    -1//图片状态:没有改变
#define MBLIST_IPICNONE         0//图片状态:没有
#define MBLIST_IPICSTATE_A      1//图片状态:激活
#define MBLIST_IPICSTATE_H      2//图片状态:保持
#define MBLIST_IPICSTATE_AM     4//图片状态:激活静音
#define MBLIST_IPICSTATE_HM     8//图片状态:保持静音
//创建MBList控件
HWND    CreateMBListControl(HWND hwndParent,int iId,int x,int y,int nWidth,int nHeight,DWORD dwFlags);
//添加数据
int     MBList_AddItem(const char * pFirstStr,const char * pSecondStr,
                       int iPicState,int iData,int iCallId);

//删除第iIndex个节点
int     MBList_DeleteItem(int iIndex);
//销毁窗口
void    MBList_Destroy(void);

//iCallId是否在链表里的节点中存在
BOOL    MBList_ExistCallId(int iCallId);

//获得链表节点数
int     MBList_GetCount(void);
//返回选中的节点的序号
int     MBList_GetCurSel(void);
//获得第iIndex个节点的数据
int     MBList_GetItem(int iIndex);
//获得第iIndex个节点的第一行第二行数据
BOOL    MBList_GetString(int iIndex,char * pFirstStr,char * pSecondStr);
//刷新当前列表
void    MBList_Refresh(void);
//删除所有节点
void    MBList_ResetItem(void);

//设置选中第index个节点的数据
void    MBList_SetCurSel(int index);
//设置第iIndex个节点的数据
void    MBList_SetItem(int iIndex,const char * pFirstStr,const char * pSecondStr,
                       int iPicState,int iData,int iCallId,BOOL bRefresh);
//设置控件风格
void    MBList_SetStyle(DWORD dwStyle);
//出现或者隐藏控件
BOOL    MBList_Show(int nCmdShow);
//是否显示通话状态的图标
BOOL    MBList_ShowListBmp(BOOL bShow);

#endif
