/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : �����趨
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
#include    "hpime.h"

#define     ITEMNUMS                4

#define     IDC_BUTTON_QUIT         3//�˳���ť
#define     IDC_BUTTON_SET          4//ȷ����ť
#define     IDC_UNITCHARGING        5//��λ����
#define     IDC_COINUNIT            6//���ҵ�λ

#define     IDC_ME_GETPRICE         WM_USER + 110
#define     IDC_ME_SETPRICE         WM_USER + 100

#define     UNITCHARGINGMAXLEN      20//������󳤶�
#define     COINUNITMAXLEN          3//���ҵ�λ��󳤶�

#define     TITLECAPTION            MBGetString(STR_MBRECORD_SETUPCOST)//�����趨
#define     BUTTONOK                (LPARAM)MBGetString(STR_WINDOW_OK)    //ȷ��
#define     BUTTONQUIT              (LPARAM)MBGetString(STR_WINDOW_EXIT)  //�˳�
#define     ERROR1                  MBGetString(STR_MBRECORD_ERROR1)      //������ʾ
#define     PRICE                   MBGetString(STR_MBRECORD_PRICE)//����%s%s
#define     PRICESET                MBGetString(STR_MBRECORD_SET)//����
#define     PRICESEARCH             MBGetString(STR_MBPHONE_GSM_FIND)       //��ѯ
#define     PRICESUCC               MBGetString(STR_MBPHONE_GSM_SUCC)       //�ɹ�
#define     PRICEFAIL               MBGetString(STR_MBPHONE_GSM_FAIL)       //ʧ��
//#define     INPUTPRICEUNIT          MBGetString(STR_MBRECORD_INPUTUNIT)//�����뵥λ����
#define     PRICEUNITERROR          MBGetString(STR_MBRECORD_UNITERROR)//�Ƿ��ĵ�λ����

#define     TXT_UNITCHARGING        MBGetString(STR_MBRECORD_UNITMONEY)//��λ����
#define     TXT_COINUNIT            MBGetString(STR_MBRECORD_MONEYUNIT)//���ҵ�λ
#define     IMETXT                  "ȫ����д"//MBGetString(STR_IMEASP_ENGID)
#define     CLEW_INPUTCOINUNIT      MBGetString(STR_MBRECORD_AOC_INPUTCOINUNIT)//"��������ҵ�λ"

static const char * pClassName           = "MBSetCostsWndClass";//��������
static const char * pUnitChargingDefault = "0.001";//��λ����Ĭ��ֵ
static const char * pEditClass           = "INPUTEDIT";
static const int    iTxtX                = 5;
static HINSTANCE    hInstance;
static InputEdit    IUnitCharging;
static InputEdit    ICoinUnit;

static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  BOOL    CreateControl(HWND hWnd,HWND * hUnitCharging);//�����ؼ�
static  BOOL    ValidPricePerUnit(HWND hWnd);

BOOL    CallSetCost(HWND hwndCall)//�����趨
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
    static  HWND        hFocus = 0;
            ME_PUCINFO  me_pubinfo;
            LRESULT     lResult;
            int         iy[ITEMNUMS];
            HDC         hdc;
            char        cClew[40] = "";

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :        
        ME_GetPricePerUnit(hWnd,IDC_ME_GETPRICE);
        if (!CreateControl(hWnd,&hFocus))
        {
            AppMessageBox(NULL,ERROR1, TITLECAPTION, WAITTIMEOUT);
            return -1;
        }
        break;

    case WM_SETFOCUS:
        SetFocus(hFocus);
        break;

    case WM_SETRBTNTEXT:
        SetRightButtonText(hWnd,wParam,lParam);
        break;

    case WM_ACTIVATE://ȥ�������Ϣ�������Ҽ������˳��������˳�ʧ��
        if (WA_INACTIVE != LOWORD(wParam))
            SetFocus(hFocus);//��������ý���
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;

    case WM_CLOSE://������Ͻǵ��˳���ťʱΪ�����˳�;
        SendMessage(hWnd,WM_COMMAND,(WPARAM)IDC_BUTTON_QUIT,0);   
        break;

    case WM_DESTROY ://֪ͨ���������,�˳����˳�;
        UnregisterClass(pClassName,NULL);
        DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)hInstance);
        break;

    case WM_PAINT :
        hdc = BeginPaint( hWnd, NULL);
        CountControlsYaxis(iy,GetCharactorHeight(hWnd),sizeof(iy));
        TextOut(hdc,iTxtX,iy[0],TXT_UNITCHARGING,strlen(TXT_UNITCHARGING));
        TextOut(hdc,iTxtX,iy[2],TXT_COINUNIT    ,strlen(TXT_COINUNIT    ));
        EndPaint(hWnd, NULL);
        break;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_F10:
            DestroyWindow( hWnd );  
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;  

    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        case IDC_UNITCHARGING://��λ����
            switch(HIWORD(wParam))
            {
            case EN_SETFOCUS:
                hFocus = GetDlgItem(hWnd,IDC_UNITCHARGING);
                break;
            }
            break;

        case IDC_COINUNIT://���ҵ�λ
            switch(HIWORD(wParam))
            {
            case EN_SETFOCUS:
                hFocus = GetDlgItem(hWnd,IDC_COINUNIT);
                break;
            }
            break;

        case IDC_BUTTON_SET://����
            ValidPricePerUnit(hWnd);
            break;
        case IDC_BUTTON_QUIT://�˳�
            DealWithRightButton(hWnd);
            break;
        }
        break;

    case IDC_ME_GETPRICE://��ѯ������Ϣ
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            memset(&me_pubinfo,0x00,sizeof(ME_PUCINFO));
            ME_GetResult(&me_pubinfo,sizeof(ME_PUCINFO));
            TrimString(me_pubinfo.currency);
            SetWindowText(GetDlgItem(hWnd,IDC_UNITCHARGING),me_pubinfo.ppu);
            SetWindowText(GetDlgItem(hWnd,IDC_COINUNIT),me_pubinfo.currency);
            break;
        default:
            sprintf(cClew,PRICE,PRICESEARCH,PRICEFAIL);
            AppMessageBox(hWnd,cClew,TITLECAPTION, WAITTIMEOUT);
            break;
        }
        break;

    case IDC_ME_SETPRICE://���û���
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            sprintf(cClew,PRICE,PRICESET,PRICESUCC);
            AppMessageBox(hWnd,cClew,TITLECAPTION, WAITTIMEOUT);
            DestroyWindow(hWnd);
            break;
        default:
            sprintf(cClew,PRICE,PRICESET,PRICEFAIL);
            AppMessageBox(hWnd,cClew,TITLECAPTION, WAITTIMEOUT);
            SetFocus(GetDlgItem(hWnd,IDC_UNITCHARGING));
            break;
        }
        break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
/********************************************************************
* Function   ValidPricePerUnit  
* Purpose    ��������������Ƿ�Ϸ�
* Params     hUnitCharging:��λ����,hCoinUnit:���ҵ�λ
* Return     
* Remarks     
**********************************************************************/
static  BOOL    ValidPricePerUnit(HWND hWnd)
{
    char cUnit[UNITCHARGINGMAXLEN + 1] = "",cCoin[COINUNITMAXLEN + 1] = "";
    char cPin2[PIN2MAXLEN + 1] = "",cClew[40] = "";
    ME_PUCINFO  me_pubinfo;
    HWND hUnitCharging,hCoinUnit;

    hUnitCharging = GetDlgItem(hWnd,IDC_UNITCHARGING);
    hCoinUnit     = GetDlgItem(hWnd,IDC_COINUNIT);

    GetWindowText(hUnitCharging,cUnit,GetWindowTextLength(hUnitCharging) + 1);
    GetWindowText(hCoinUnit    ,cCoin,GetWindowTextLength(hCoinUnit    ) + 1);

    if (strlen(cUnit) == 0)
    {
        strcpy(cUnit,pUnitChargingDefault);
    }
    else
    {
        if (!StrIsDouble(cUnit))
        {
            AppMessageBox(hWnd,PRICEUNITERROR,TITLECAPTION, WAITTIMEOUT);
            SetFocus(hUnitCharging);
            SendMessage(hUnitCharging,EM_SETSEL,0,-1);
            return FALSE;
        }
        if (atoi(cUnit) == 0)
            strcpy(cUnit,pUnitChargingDefault);
    }
    TrimString(cCoin);
    if (strlen(cCoin) == 0)
    {
        AppMessageBox(hWnd,CLEW_INPUTCOINUNIT,TITLECAPTION, WAITTIMEOUT);
        SetFocus(hCoinUnit);
        SendMessage(hCoinUnit,EM_SETSEL,0,-1);
        return FALSE;
    }
    memset(&me_pubinfo,0x00,sizeof(ME_PUCINFO));
    strcpy(me_pubinfo.ppu,cUnit);
    strcpy(me_pubinfo.currency,cCoin);
    GetValidPin2(cPin2);

    if (-1 == ME_SetPricePerUnit(hWnd,IDC_ME_SETPRICE,&me_pubinfo,cPin2))
    {
        sprintf(cClew,PRICE,PRICESET,PRICEFAIL);
        AppMessageBox(hWnd,cClew,TITLECAPTION, WAITTIMEOUT);
        SetFocus(hUnitCharging);
        return FALSE;
    }
    return TRUE;
}
/********************************************************************
* Function   CreateControl  
* Purpose    �����ؼ� 
* Params     hwnd�����ڹ��̾��
* Return     
* Remarks     ��������������Ľ���Ԫ�أ�����"ȷ��"��"ȡ��"��ť��Ϊ���ڼ�ͼ�꣬���ָʾ��ͷ. 
**********************************************************************/
static  BOOL    CreateControl(HWND hWnd,HWND * hUnitCharging)
{
    int icontrolx = 10,iControlHeight,iy[ITEMNUMS],icontrolw;
    HWND hCoinUnit;

    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT);
    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK);
    SendMessage(hWnd,PWM_SETSCROLLSTATE,SCROLLLEFT|SCROLLRIGHT|SCROLLDOWN|SCROLLUP,MASKALL);

    iControlHeight = GetEditControlHeight();
    icontrolw      = GetScreenUsableWH1(SM_CXSCREEN) - icontrolx * 2;
    CountControlsYaxis(iy,iControlHeight,sizeof(iy));

    * hUnitCharging = CreateWindow (pEditClass,"",   //��λ����
        WS_VISIBLE | WS_CHILD | WS_TABSTOP|WS_BORDER,
        icontrolx,iy[1],icontrolw,iControlHeight,
        hWnd,(HMENU)IDC_UNITCHARGING,NULL,NULL);

    if (* hUnitCharging == NULL)
        return FALSE;

    hCoinUnit = CreateWindow (pEditClass,"",   //���ҵ�λ
        WS_VISIBLE | WS_CHILD | WS_TABSTOP|WS_BORDER,
        icontrolx,iy[3],icontrolw,iControlHeight,
        hWnd,(HMENU)IDC_COINUNIT,NULL,NULL);

    if (hCoinUnit == NULL)
        return FALSE;

    SendMessage(* hUnitCharging,EM_LIMITTEXT,UNITCHARGINGMAXLEN,0);
    SendMessage(hCoinUnit      ,EM_LIMITTEXT,COINUNITMAXLEN    ,0);

    IUnitCharging.lStyle = GetWindowLong(* hUnitCharging,GWL_STYLE);
    IUnitCharging.nKeyBoardType = GetImeHandle(IMETXT);
    strcpy(IUnitCharging.szName,TXT_UNITCHARGING);
    SetWindowLong(* hUnitCharging,GWL_USERDATA,(LONG)&IUnitCharging);

    ICoinUnit.lStyle = GetWindowLong(hCoinUnit,GWL_STYLE);
    ICoinUnit.nKeyBoardType = GetImeHandle(IMETXT);
    strcpy(ICoinUnit.szName,TXT_COINUNIT);
    SetWindowLong(hCoinUnit,GWL_USERDATA,(LONG)&ICoinUnit);

    return TRUE;
}
