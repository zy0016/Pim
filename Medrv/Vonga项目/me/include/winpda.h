/**************************************************************************\
 *
 *                      Hopen Window System
 *
 * Copyright (c) 1998 by CASS Corporation & Software Engineering Center
 *                       All Rights Reserved
 *
 *
 * Filename     winpda.h
 *
 * Purpose      Header file for default PDA main window proc.
 *  
 * Author       WeiZhong
 *
\**************************************************************************/

#ifndef __WINPDA_H
#define __WINPDA_H

#ifndef __WINDOW_H
#include "window.h"
#endif

// LOWORD(wParam) : ID of the listbox
// HIWORD(wParam) : unused
// LOWORD(lPARAM) : width of the list box
// HIWORD(lParam) : unused
#define PWM_CREATECAPTIONLISTBOX    0x0381      // For Legend860, Radiant

// wParam : 0 - hide, 1 - show
// lParam : unused. 
#define PWM_SHOWCAPTIONLISTBOX      0x0382      // For Legend860 only

// LOWORD(wParam) : ID of the listbox
// HIWORD(wParam) : Specifies the location of the button
                  //HIWORD(wParam) == 1 means button at the left
                  //HIWORE(wParam) == 0 means button at the right
// lParam : Specifies the button text
#define PWM_CREATECAPTIONBUTTON     0x0383      // For Radiant only

// lParam : handle of app icon bitmap
// return the old app icon bitmap
#define PWM_SETAPPICON              0x0384      // For Radiant only

#define PWM_SETSCROLLSTATE          0x0385      // For Mobile Phone
#define PWM_GETSCROLLSTATE          0x0389      // For Mobile Phone

// HIWORD(wParam) : unused
// LOWORD(wParam) : Specifies the location of the button
                  //HIWORD(wParam) == 1 means button at the left
                  //HIWORE(wParam) == 0 means button at the right
// lParam : Specifies new button text
#define PWM_SETBUTTONTEXT           0x0386      // For Mobile Phone, changed the text on 
                                                // the button
// HIWORD(wParam) : unused
// LOWORD(wParam) : Specifies the location of the button
                  //HIWORD(wParam) == 1 means button at the left
                  //HIWORE(wParam) == 0 means button at the right
// lParam : Pointer to the buffer that will receive the text on the button.
#define PWM_GETBUTTONTEXT           0x0387      // For Mobile Phone, get the text on 
                                                // the button

//wParam           : CAPTIONCOLOR
//lParam           : STATICBARCOLOR
#define PWM_SETDEFWINDOWCOLOR       0x0388      // For Mobile Phone, modify the color of
												// the default window's caption.

#define PWS_STATICBAR               0x0001     //For Mobile Phone, Specifies showing the
                                               // staticbar or not.
// Set Scroll State
#define SCROLLUP        0x0001
#define SCROLLDOWN      0x0002
#define SCROLLLEFT      0x0004
#define SCROLLRIGHT     0x0008
#define SCROLLMIDDLE    0x0010

#define MASKUP          0x0001
#define MASKDOWN        0x0002
#define MASKLEFT        0x0004
#define MASKRIGHT       0x0008
#define MASKMIDDLE      0x0010
#define MASKALL         0x001F

#define CAP_COLOR       0x0001
#define BAR_COLOR       0x0002
#define BTN_COLOR       0X0003
#define FOCUS_COLOR     0x0004
#define NORMAL_COLOR    0x0005

#define NORBMPFILE  "ROM:ss_1btn.bmp"
#define FOCBMPFILE  "ROM:ss_fbtn.bmp"
#define PUSBMPFILE  "ROM:ss_3btn.bmp"

/**************************************************************************/
/*              Default PDA main window proc                              */
/**************************************************************************/
LRESULT WINAPI PDADefWindowProc(HWND hWnd, UINT message, WPARAM wParam, 
                                LPARAM lParam);

/**************************************************************************/
/*              Menu Functions                                            */
/**************************************************************************/

#ifndef NOMENUS
#ifndef NOMENUAPI

HMENU   WINAPI PDAGetMenu(HWND hWnd);
BOOL    WINAPI PDASetMenu(HWND hWnd, HMENU hMenu);
BOOL    WINAPI PDADrawMenuBar(HWND hWnd);

#endif // NOMENUAPI
#endif // NOMENUS

COLORREF GetWindowColor(int index);
void SetWindowColor(int index, COLORREF color);
void SetCaptionBmp(PCSTR pFileName);
void SetBottomBmp(PCSTR pFileName);
/**************************************************************************/
/*              Scroll bar support                                        */
/**************************************************************************/

#ifndef NOSCROLL

int     WINAPI PDASetScrollInfo(HWND hWnd, int nBar, 
                                PCSCROLLINFO pScrollInfo, BOOL bRedraw);
BOOL    WINAPI PDAGetScrollInfo(HWND hWnd, int nBar, 
                                PSCROLLINFO pScrollInfo);
BOOL    WINAPI PDAEnableScrollBar(HWND hWnd, int nSBFlags, int nArrow);

#endif // NOSCROLL

#endif // __WINPDA_H
