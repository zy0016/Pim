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
#include "hp_pdaex.h"
#include "me_wnd.h"
#include "str_mb_plx.h"
#include "hpimage.h"
#include "winpda.h"
#include "MBPublic.h"
#include "../mbrecord/RcCommon.h"
#if defined(EXCEPTION)
#include "../CAMERA/CameraHeader.h"
#include "../minivideo/MiniVideoPlay.h"
#endif

#define     IDC_BUTTON_SET          201
#define     IDC_BUTTON_QUIT         202
#define     IDC_REPLACE_CALL        203
#define     IDC_HANGUP              204
#define     IDC_BUTTON_MIDDLE       206
#define     WM_VOICER_ANSWER        WM_USER + 103
#define     WM_SWITCH_CALLLINE      WM_USER + 106
#define     WM_VOICER_HANGUP        WM_USER + 105
#define     IDC_TIMER_CF            205
#define     IDC_TIMER_BRINGTOTOP    210

#define     BUTTONOK                MBGetString(STR_WINDOW_OK)
#define     BUTTONQUIT              MBGetString(STR_WINDOW_BACK)
#define     IDS_HANGUP              MBGetString(STR_MBPHONE_HANGUP)//挂断
#define     TITLECAPTION            MBGetString(STR_MBPHONE_CALLCOMING)//来电
#define     IDS_VOICE_ANSWER        MBGetString(STR_MBPHONE_GET)//接听
#define     IDS_REPLACE             MBGetString(STR_MBPHONE_REPLACE)//替换
#define     IDS_PL_CONFIRM          MBGetString(STR_MBPHONE_CALLCOMING)
#define     IDS_CLOSECURCALL        MBGetString(STR_MBPHONE_HANGUP_CURRENT)//挂断当前通话?
#define     IDS_PROMPT              MBGetString(STR_MBPHONE_CLEW)//提示
#define     IDS_HANGUP_FAIL         MBGetString(STR_MBPHONE_HANGUP_FAIL)//挂断失败
#define     IDS_ANSWER_FAIL         MBGetString(STR_MBPHONE_ANSWER_FAIL)//接听失败
#define     SWITCHFAIL              MBGetString(STR_MBPHONE_SWITCH_FAIL)//切换失败
#define     CALLFORWARDING          MBGetString(STR_MBPHONE_CALLFORWARDING)//转接
#define     BUTTON_MIDDLE_TXT       MBGetString(STR_MBPHONE_DIALNUMBER)//拨号

static  LRESULT CALLBACK MBRingAppWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,LPARAM lParam);
static  LRESULT CALLBACK ConfirmWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,LPARAM lParam);
static  void    OnCommandMsg(HWND hWnd, WPARAM wParam, LPARAM lParam);
static  BOOL    GotoConfirmWindow(HWND hWnd);
static  void    MBRingHangup(void);//来电接听的挂断处理
static  void    MBRingInit(void);//接电话之前需要执行的函数的集合
static  void    DestroyConfirm(void);//销毁确认窗口
