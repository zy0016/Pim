/**************************************************************************\
 *
 *                      HopenOS Window System
 *
 * Copyright (c) 1998 by CASS Corporation & Software Engineering Center
 *                       All Rights Reserved
 *
 *
 * Filename     windowx.h
 *
 * Purpose      定义窗口系统提供的, 未包含在window.h中的宏、消息和函数, 这
 *              些消息和函数仅提供给内部使用, 对普通应用程序不公开.
 *  
 * Author       WeiZhong
 *
\**************************************************************************/

#ifndef __WINDOWX_H
#define __WINDOWX_H

/* include window.h if necessary */
#ifndef __WINDOW_H
#include "window.h"
#endif

/* Internal system message */
#define WM_SYSTIMER     0x03F1
#define WM_SYSWAKEUP    0x03F0

/* Resend the latest mouse message */
BOOL WINAPI ResendMouseMessage(HWND hWnd, UINT message, WPARAM wParam, 
                               LPARAM lParam);

/* Captures the mouse for non-client area */
HWND WINAPI SetNCCapture(HWND hWnd);

/* Callback function when caret position changing */
typedef void (CALLBACK *CARETPROC)(const RECT*);
CARETPROC WINAPI SetCaretProc(CARETPROC pNewProc);

#endif
