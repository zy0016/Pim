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
#define     IDS_HANGUP              MBGetString(STR_MBPHONE_HANGUP)//�Ҷ�
#define     TITLECAPTION            MBGetString(STR_MBPHONE_CALLCOMING)//����
#define     IDS_VOICE_ANSWER        MBGetString(STR_MBPHONE_GET)//����
#define     IDS_REPLACE             MBGetString(STR_MBPHONE_REPLACE)//�滻
#define     IDS_PL_CONFIRM          MBGetString(STR_MBPHONE_CALLCOMING)
#define     IDS_CLOSECURCALL        MBGetString(STR_MBPHONE_HANGUP_CURRENT)//�Ҷϵ�ǰͨ��?
#define     IDS_PROMPT              MBGetString(STR_MBPHONE_CLEW)//��ʾ
#define     IDS_HANGUP_FAIL         MBGetString(STR_MBPHONE_HANGUP_FAIL)//�Ҷ�ʧ��
#define     IDS_ANSWER_FAIL         MBGetString(STR_MBPHONE_ANSWER_FAIL)//����ʧ��
#define     SWITCHFAIL              MBGetString(STR_MBPHONE_SWITCH_FAIL)//�л�ʧ��
#define     CALLFORWARDING          MBGetString(STR_MBPHONE_CALLFORWARDING)//ת��
#define     BUTTON_MIDDLE_TXT       MBGetString(STR_MBPHONE_DIALNUMBER)//����

static  LRESULT CALLBACK MBRingAppWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,LPARAM lParam);
static  LRESULT CALLBACK ConfirmWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,LPARAM lParam);
static  void    OnCommandMsg(HWND hWnd, WPARAM wParam, LPARAM lParam);
static  BOOL    GotoConfirmWindow(HWND hWnd);
static  void    MBRingHangup(void);//��������ĹҶϴ���
static  void    MBRingInit(void);//�ӵ绰֮ǰ��Ҫִ�еĺ����ļ���
static  void    DestroyConfirm(void);//����ȷ�ϴ���
