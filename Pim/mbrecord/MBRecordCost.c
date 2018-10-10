/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : ͨ����¼����
 *
 * Purpose  : ͨ����ʱ
 *            
\**************************************************************************/
#include    "window.h"
#include    "../mbphone/str_mb_plx.h"
#include    "setting.h"
#include    "RcCommon.h"
#include    "hp_pdaex.h"
#include    "../mbphone/MBPublic.h"

#define     TIMEMAXLEN              50
#define     IDC_BUTTON_QUIT         3//�˳���ť
#define     IDC_BUTTON_SET          4//ȷ����ť

#define     TITLECAPTION            MBGetString(STR_MBRECORD_DURATION)  //ͨ����ʱ
#define     BUTTONOK                (LPARAM)MBGetString(STR_MBRECORD_CLEAR)//ȷ��
#define     BUTTONQUIT              (LPARAM)MBGetString(STR_WINDOW_BACK)    //�˳�

#define     IDS_LASTCALLTIME        MBGetString(STR_MBRECORD_LASTTIME)//"�ϴ�"
#define     IDS_OUTSUM              MBGetString(STR_MBRECORD_OUTCOUNT)//"����"
#define     IDS_INSUM               MBGetString(STR_MBRECORD_INCOUNT)//"����"
#define     IDS_TOTAL               MBGetString(STR_MBRECORD_TOTAL)//"�ܼ�"

static HINSTANCE    hInstance;
static const char * pClassName  = "MBRecordCostWndClass";//��������
static const char * pTime       = "%s:%02d:%02d:%02d";
static const int    iRectX      = 10;
static const int    iNumber60   = 60;

static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
//*********************************************************
BOOL    CallMBRecordCostWindow(HWND hwndCall)
{
    HWND        hWnd;
    WNDCLASS    wc;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassName;

    if (!RegisterClass(&wc))
        return FALSE;

    hWnd = CreateWindow(pClassName,TITLECAPTION,WS_CAPTION|WS_BORDER |PWS_STATICBAR,  
        PLX_WIN_POSITION , hwndCall, NULL, NULL, NULL);

    if (NULL == hWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd); 
    return (TRUE);
}
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    MBRecordCostTime MRCostTime;
    RECT    rTitleTime,rLastTime,rOutTime,rInTime;
    int     iSW,iy[4],iFontHeight,TimeAllHour = 0,TimeAllMin = 0,TimeAllSec = 0;
    char    cTotalTime[TIMEMAXLEN] = "",cLastTime[TIMEMAXLEN] = "",cOutTime[TIMEMAXLEN] = "",cInTime[TIMEMAXLEN] = "";
    LRESULT lResult;
    HDC     hdc;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT);
        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK);
        break;

    case WM_SETFOCUS:
        SetFocus(hWnd);
        break;

    case WM_CLOSE://������Ͻǵ��˳���ťʱΪ�����˳�;
        SendMessage(hWnd,WM_COMMAND,(WPARAM)IDC_BUTTON_QUIT,0);
        break;

    case WM_DESTROY ://֪ͨ���������,�˳����˳�;
        UnregisterClass(pClassName,NULL);
        DlmNotify((WPARAM)PES_STCQUIT,(LPARAM)hInstance);
        break;

    case WM_PAINT :
        hdc = BeginPaint( hWnd, NULL);

        ReadRecord_Cost(&MRCostTime); 

        TimeAllSec = MRCostTime.nInSecond + MRCostTime.nOutSecond;
        if(TimeAllSec > iNumber60)
        {
            TimeAllSec -= iNumber60;
            TimeAllMin++;
        }
        TimeAllMin = TimeAllMin + MRCostTime.nInMin + MRCostTime.nOutMin;
        if(TimeAllMin > iNumber60)
        {
            TimeAllMin -= iNumber60;
            TimeAllHour++;
        }
        TimeAllHour = TimeAllHour + MRCostTime.nOutHour + MRCostTime.nInHour;

        sprintf(cLastTime, pTime,IDS_LASTCALLTIME, 
            MRCostTime.nLastHour, MRCostTime.nLastMin, MRCostTime.nLastSecond);
        sprintf(cOutTime, pTime, IDS_OUTSUM,
            MRCostTime.nOutHour, MRCostTime.nOutMin, MRCostTime.nOutSecond);
        sprintf(cInTime, pTime, IDS_INSUM,
            MRCostTime.nInHour, MRCostTime.nInMin, MRCostTime.nInSecond);
        sprintf(cTotalTime, pTime,IDS_TOTAL,//"��ʱ�ܼ�"
            TimeAllHour, TimeAllMin, TimeAllSec);

        iSW         = GetScreenUsableWH1(SM_CXSCREEN);
        iFontHeight = GetCharactorHeight(hWnd);
        CountControlsYaxis(iy,iFontHeight,sizeof(iy));

        SetRect(&rTitleTime ,iRectX,iy[0],iSW - iRectX * 2,iy[0] + iFontHeight);//��ʱ�ܼ�
        SetRect(&rLastTime  ,iRectX,iy[1],iSW - iRectX * 2,iy[1] + iFontHeight);//�ϴ�ͨ��
        SetRect(&rOutTime   ,iRectX,iy[2],iSW - iRectX * 2,iy[2] + iFontHeight);//�ۼƺ���
        SetRect(&rInTime    ,iRectX,iy[3],iSW - iRectX * 2,iy[3] + iFontHeight);//�ۼƽ���

        DrawText(hdc,cLastTime  ,-1,&rLastTime  ,DT_LEFT|DT_VCENTER);
        DrawText(hdc,cOutTime   ,-1,&rOutTime   ,DT_LEFT|DT_VCENTER);
        DrawText(hdc,cInTime    ,-1,&rInTime    ,DT_LEFT|DT_VCENTER);
        DrawText(hdc,cTotalTime ,-1,&rTitleTime ,DT_LEFT|DT_VCENTER);

        EndPaint(hWnd, NULL);
        break;

    case WM_TODESKTOP:
        SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
        return TRUE;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_F10:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
            break;
        case VK_F5:
        case VK_RETURN:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SET,0);
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        case IDC_BUTTON_SET://����
            memset(&MRCostTime,0x00,sizeof(MBRecordCostTime));
            SaveRecord_Cost(&MRCostTime);
            InvalidateRect(hWnd, NULL, NULL);
            break;
        case IDC_BUTTON_QUIT://�˳�
            DestroyWindow(hWnd);  
            break;
        }
        break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
