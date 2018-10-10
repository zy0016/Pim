/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : ͨ���Ʒ�
 *            
\**************************************************************************/
#include    "window.h"
#include    "string.h"
#include    "stdlib.h"
#include    "stdio.h" 
#include    "winpda.h"
#include    "str_public.h"
#include    "hp_pdaex.h"
#include    "../mbphone/str_mb_plx.h"
#include    "RcCommon.h"

#define DEBUG_MBRECORFARE

#define     IDC_BUTTON_QUIT         3//�˳���ť
#define     IDC_BUTTON_SET          4//ȷ����ť
#define     IDC_BUTTON_TOTAL        6//�ۼƷ������㰴ť

#define     IDC_ME_RESETACM         WM_USER + 100
#define     IDC_ME_GETACM           WM_USER + 110//��ѯ�ۼ�ͨ������
#define     IDC_ME_GETLAST          WM_USER + 120//��ѯ�ϴ�ͨ������
#define     IDC_ME_GETPRICEUNIT     WM_USER + 130//��ȡ�Ƽ���Ϣ

#define     TITLECAPTION            MBGetString(STR_MBRECORD_COST)//ͨ���Ʒ�
#define     BUTTONOK                (LPARAM)MBGetString(STR_WINDOW_OK)    //ȷ��
#define     BUTTONQUIT              (LPARAM)MBGetString(STR_WINDOW_EXIT)  //�˳�
#define     ERROR1                  MBGetString(STR_MBRECORD_ERROR1)      //������ʾ
#define     BUTTONTXT_TOTAL         MBGetString(STR_MBRECORD_BUTTONTXT_TOTAL)//�ۼƷ�������
#define     CLEWTXT_LAST            MBGetString(STR_MBRECORD_CLEWTXT_LAST)//���ͨ��
#define     CLEWTXT_TOTAL           MBGetString(STR_MBRECORD_CLEWTXT_TOTAL)//�ۼƷ���
#define     SETSUCC                 MBGetString(STR_MBRECORD_FARE_SETSUCC)//"���óɹ�"
#define     SETFAIL                 MBGetString(STR_MBRECORD_FARE_SETFAIL)//"����ʧ��"
#define     UNIT                    MBGetString(STR_MBRECORD_FACE_UNIT)//"��λ"

static HINSTANCE    hInstance;
static const char * pClassName   = "MBFareWndClass";//��������
static const char * pButtonClass = "BUTTON";
static const char * pClewFormat  = "%7ld %s";
static const char * pUnitFormat  = "%7lf %s";
static const int    iRectX       = 5;

static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  BOOL    CreateControl(HWND hWnd);//�����ؼ�

BOOL    CallMBRecordFare(HWND hwndCall)
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

    hWnd = CreateWindow(pClassName,TITLECAPTION, WS_CAPTION|WS_BORDER |PWS_STATICBAR,
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
    static  ME_PUCINFO      me_pucinfo;
    static  unsigned long   lTotal,lLast;
            char            cPin2[PIN2MAXLEN + 1] = "",cTotal[50] = "",cLast[50] = "";
            LRESULT         lResult;
            HDC             hdc;
            int             iy[2],ish,iFontHeight;
            RECT            rTotal,rLast;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        if (!CreateControl(hWnd))
        {
            AppMessageBox(NULL,ERROR1, TITLECAPTION, WAITTIMEOUT);
            return -1;
        }
        lTotal = 0;
        lLast  = 0;
        memset(&me_pucinfo,0x00,sizeof(ME_PUCINFO));
        ME_GetCCM(hWnd,IDC_ME_GETLAST);//��ѯ�ϴ�ͨ������
        break;

    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd,IDC_BUTTON_TOTAL));
        break;

    case WM_ACTIVATE://ȥ�������Ϣ�������Ҽ������˳��������˳�ʧ��
        if (WA_INACTIVE != LOWORD(wParam))
            SetFocus(GetDlgItem(hWnd,IDC_BUTTON_TOTAL));//��������ý���
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
        
    case WM_CLOSE://������Ͻǵ��˳���ťʱΪ�����˳�;
        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_BUTTON_QUIT, 0 );   
        break;

    case WM_DESTROY ://֪ͨ���������,�˳����˳�;
        UnregisterClass(pClassName,NULL);
        DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
        break;

    case WM_PAINT :
        hdc         = BeginPaint( hWnd, NULL);
        ish         = GetScreenUsableWH1(SM_CYSCREEN);
        iFontHeight = GetCharactorHeight(hWnd);
        CountControslYaxisExt(iy,iFontHeight,sizeof(iy),-1,ish / 2);
        SetRect(&rLast ,iRectX,iy[0],iRectX + GetStringWidth(hdc,CLEWTXT_LAST) ,iy[0] + iFontHeight);
        SetRect(&rTotal,iRectX,iy[1],iRectX + GetStringWidth(hdc,CLEWTXT_TOTAL),iy[1] + iFontHeight);

        DrawText(hdc,CLEWTXT_LAST,strlen(CLEWTXT_LAST),&rLast,DT_LEFT);
        DrawText(hdc,CLEWTXT_TOTAL,strlen(CLEWTXT_TOTAL),&rTotal,DT_LEFT);

        if ((strlen(me_pucinfo.currency) == 0) && (strlen(me_pucinfo.ppu) == 0))//���յ�λ��ʾ
        {
            sprintf(cLast ,pClewFormat,lLast ,UNIT);
            sprintf(cTotal,pClewFormat,lTotal,UNIT);
        }
        else
        {
            sprintf(cLast ,pUnitFormat,lLast * MB_atof(me_pucinfo.ppu) ,me_pucinfo.currency);
            sprintf(cTotal,pUnitFormat,lTotal * MB_atof(me_pucinfo.ppu),me_pucinfo.currency);
        }
        TextOut(hdc,rLast.right  + iRectX,iy[0],cLast ,strlen(cLast ));
        TextOut(hdc,rTotal.right + iRectX,iy[1],cTotal,strlen(cTotal));
        EndPaint(hWnd, NULL);
        break;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_F10:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
            break;
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
        case IDC_BUTTON_TOTAL://�ۼƷ������㰴ť
            switch(HIWORD(wParam))
            {
            case BN_CLICKED:
                if (CallValidPin2(hWnd))//���ȼ��PIN2��
                {
                    GetValidPin2(cPin2);
                    if (-1 == ME_ResetACM(hWnd,IDC_ME_RESETACM,cPin2))
                        AppMessageBox(hWnd,SETFAIL,TITLECAPTION, WAITTIMEOUT);
                }
                break;
            }
            break;

        case IDC_BUTTON_SET://����
            SendKeyDown(GetFocus(), VK_F5, 0);
            break;
        case IDC_BUTTON_QUIT://�˳�
            DestroyWindow(hWnd);
            break;
        }
        break;

    case IDC_ME_RESETACM://�ۼƷ�������
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            AppMessageBox(hWnd,SETSUCC,TITLECAPTION, WAITTIMEOUT);
            DestroyWindow(hWnd);
            break;
        default:
            AppMessageBox(hWnd,SETFAIL,TITLECAPTION, WAITTIMEOUT);
            SetFocus(GetDlgItem(hWnd,IDC_BUTTON_TOTAL));
            break;
        }
        break;

    case IDC_ME_GETACM://��ѯ�ۼ�ͨ������
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            ME_GetResult(&lTotal,sizeof(lTotal));
            break;
        default:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRECORFARE)
            printf("mb MBRecordFare ��ѯ�ۼ�ͨ������ʧ��,wParam:%ld,lParam:%d\r\n",wParam,lParam);
#endif            
            break;
        }
        ME_GetPricePerUnit(hWnd,IDC_ME_GETPRICEUNIT);//��ȡ�Ƽ���Ϣ
        break;

    case IDC_ME_GETLAST://��ѯ�ϴ�ͨ������
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            ME_GetResult(&lLast,sizeof(lLast));
            break;
        default:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRECORFARE)
            printf("mb MBRecordFare ��ѯ�ϴ�ͨ������ʧ��,wParam:%ld,lParam:%d\r\n",wParam,lParam);
#endif
            break;
        }
        ME_GetACM(hWnd,IDC_ME_GETACM);//��ѯ�ۼ�ͨ������
        break;

    case IDC_ME_GETPRICEUNIT://��ȡ�Ƽ���Ϣ
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            ME_GetResult(&me_pucinfo,sizeof(ME_PUCINFO));
            TrimString(me_pucinfo.currency);
            TrimString(me_pucinfo.ppu);
            break;
        default:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBRECORFARE)
            printf("mb MBRecordFare ��ȡ�Ƽ���Ϣʧ��,wParam:%ld,lParam:%d\r\n",wParam,lParam);
#endif
            break;
        }
        InvalidateRect(hWnd,NULL,TRUE);
        break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
/********************************************************************
* Function   CreateControl  
* Purpose    �����ؼ� 
* Params     hwnd�����ڹ��̾��
* Return     
* Remarks     ��������������Ľ���Ԫ�أ�����"ȷ��"��"ȡ��"��ť��Ϊ���ڼ�ͼ�꣬���ָʾ��ͷ. 
**********************************************************************/
static  BOOL    CreateControl(HWND hWnd)
{
    int isw,ish,icontrolx = 5;
    SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, // �Ҽ��˳�
                (WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT );

    SendMessage(hWnd , PWM_CREATECAPTIONBUTTON, //���ȷ��
                MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK );

    SendMessage( hWnd, PWM_SETSCROLLSTATE,
                SCROLLLEFT|SCROLLRIGHT|SCROLLDOWN|SCROLLUP, MASKALL );

    GetScreenUsableWH2(&isw,&ish);

    return (NULL != CreateWindow(pButtonClass,BUTTONTXT_TOTAL,
        BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE|WS_BITMAP|WS_TABSTOP,
        icontrolx,ish / 2 + 10,isw - icontrolx * 2,GetButtonControlHeight(),
        hWnd,(HMENU)IDC_BUTTON_TOTAL,hInstance,NULL));
}
