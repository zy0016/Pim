/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : �����޶�
 *            
\**************************************************************************/
#include    "window.h"
#include    "../mbphone/str_mb_plx.h"
#include    "setting.h"
#include    "RcCommon.h"
#include    "hp_pdaex.h"
#include    "../mbphone/MBPublic.h"
#include    "me_wnd.h"

#define     IDC_BUTTON_RIGHT        3//�˳���ť
#define     IDC_BUTTON_LEFT         4//ȷ����ť
#define     IDC_CLOSE               5//��
#define     IDC_OPEN                6//��
#define     IDC_EDIT_LIMIT          7//��λ�޶�
#define     IDC_ME_GETACMMAX        WM_USER + 100//��ȡ����ۼ�Call Meterֵ
#define     IDC_ME_SETACMMAX        WM_USER + 110//��������ۼ�Call Meterֵ

#define     ACMMAXLEN               10

#define     TITLECAPTION            MBGetString(STR_MBRECORD_COSTTYPE)//�Ʒѷ�ʽ
#define     BUTTON_LEFT             (LPARAM)MBGetString(STR_WINDOW_OK)    //ȷ��
#define     BUTTON_RIGHT            (LPARAM)MBGetString(STR_WINDOW_BACK)  //�˳�
#define     ERROR1                  MBGetString(STR_SETUP_ERROR1)         //������ʾ
#define     COSTTYPE_FAIL           MBGetString(STR_MBRECORD_FARE_SETFAIL)//����ʧ��

#define     RADIO_CLOSE             MBGetString(STR_MBRECORD_CLOSE)//��
#define     RADIO_OPEN              MBGetString(STR_MBRECORD_OPEN)//��
#define     TXT_UNITLIMIT           MBGetString(STR_MBRECORD_UNITLIMIT)//��λ�޶�
#define     CLEW_INPUTUNITLIMIT     MBGetString(STR_MBRECORD_INPUTUNITLIMIT)//"�����뵥λ�޶�"
#define     CLEW_SETFAIL            MBGetString(STR_MBRECORD_FARE_SETFAIL)//����ʧ��

static const char * pClassName      = "MBChargingLimitndClass";//��������
static const char * pString         = "%ld";
static const char * pButtonClass    = "BUTTON";
static const char * pEditClass      = "DINPUTEDIT";
static const int    iControlY       = 20;
static HINSTANCE    hInstance;

static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  BOOL    CreateControl(HWND hWnd,HWND * phClose);//�����ؼ�

BOOL    CallChargingLimitWindow(HWND hwndCall)//�����޶�
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
            HWND        hEditLimit;
            LRESULT     lResult;
            DWORD       dAcmMax = 0;
            RECT        rect;
            char        cAcmMax[ACMMAXLEN + 1] = "",cPin2[PIN2MAXLEN + 1] = "";
            HDC         hdc;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        if (!CreateControl(hWnd,&hFocus))
        {
            AppMessageBox(NULL,ERROR1, TITLECAPTION, WAITTIMEOUT);
            return -1;
        }
        ME_GetACMmax(hWnd,IDC_ME_GETACMMAX);//��ȡ����ۼ�Call Meterֵ
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
        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_BUTTON_RIGHT, 0 );   
        break;

    case WM_DESTROY ://֪ͨ���������,�˳����˳�;
        UnregisterClass(pClassName,NULL);
        DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
        break;

    case WM_PAINT :
        hdc = BeginPaint( hWnd, NULL);
        SetRect(&rect,0,iControlY + GetButtonControlHeight(),
            GetScreenUsableWH1(SM_CXSCREEN),
            iControlY + GetButtonControlHeight() + GetCharactorHeight(hWnd));

        DrawText(hdc,TXT_UNITLIMIT,strlen(TXT_UNITLIMIT),&rect,DT_CENTER);
        EndPaint(hWnd, NULL);
        break;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_F10:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_RIGHT,0);
            break;
        case VK_RETURN:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_LEFT,0);
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;
    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        case IDC_CLOSE://��
            switch (HIWORD(wParam))
            {
            case BN_SETFOCUS:
                CheckRadioButton(hWnd,IDC_CLOSE,IDC_OPEN,IDC_CLOSE);
                hFocus = GetDlgItem(hWnd,IDC_CLOSE);
                break;
            }
            break;

        case IDC_OPEN://��
            switch (HIWORD(wParam))
            {
            case BN_SETFOCUS:
                CheckRadioButton(hWnd,IDC_CLOSE,IDC_OPEN,IDC_OPEN);
                hFocus = GetDlgItem(hWnd,IDC_CLOSE);
                break;
            }
            break;

        case IDC_EDIT_LIMIT:
            switch(HIWORD(wParam))
            {
            case EN_SETFOCUS:
                hFocus = GetDlgItem(hWnd,IDC_EDIT_LIMIT);
                break;
            }
            break;

        case IDC_BUTTON_LEFT://����
            GetValidPin2(cPin2);
            if (IsDlgButtonChecked(hWnd,IDC_CLOSE) == BST_CHECKED)//��
            {
                dAcmMax = 0;
            }
            else if (IsDlgButtonChecked(hWnd,IDC_OPEN) == BST_CHECKED)//��
            {
                hEditLimit = GetDlgItem(hWnd,IDC_EDIT_LIMIT);
                GetWindowText(hEditLimit,cAcmMax,GetWindowTextLength(hEditLimit) + 1);
                if (strlen(cAcmMax) == 0)
                {
                    AppMessageBox(hWnd,CLEW_INPUTUNITLIMIT,TITLECAPTION, WAITTIMEOUT);
                    SetFocus(hEditLimit);
                    break;
                }
                dAcmMax = atol(cAcmMax);
            }
            if (-1 == ME_SetACMmax(hWnd,IDC_ME_SETACMMAX,dAcmMax,cPin2))
                AppMessageBox(hWnd,CLEW_SETFAIL,TITLECAPTION, WAITTIMEOUT);
            break;
        case IDC_BUTTON_RIGHT://�˳�
            DealWithRightButton(hWnd);
            break;
        }
        break;

    case IDC_ME_GETACMMAX://��ȡ����ۼ�Call Meterֵ
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            ME_GetResult(&dAcmMax,sizeof(DWORD));
            hEditLimit = GetDlgItem(hWnd,IDC_EDIT_LIMIT);
            if (dAcmMax > 0)
            {
                sprintf(cAcmMax,pString,dAcmMax);//"%ld"
                SetWindowText(hEditLimit,cAcmMax);
                CheckRadioButton(hWnd,IDC_CLOSE,IDC_OPEN,IDC_OPEN);
            }
            else
            {
                SetWindowText(hEditLimit,"");
                CheckRadioButton(hWnd,IDC_CLOSE,IDC_OPEN,IDC_CLOSE);
            }
            break;
        default:
            break;
        }
        break;
    case IDC_ME_SETACMMAX:
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            DestroyWindow(hWnd);
            break;
        default:
            AppMessageBox(hWnd,CLEW_SETFAIL,TITLECAPTION, WAITTIMEOUT);
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
* Function   CreateControl  
* Purpose    �����ؼ� 
* Params     hwnd�����ڹ��̾��
* Return     
* Remarks     ��������������Ľ���Ԫ�أ�����"ȷ��"��"ȡ��"��ť��Ϊ���ڼ�ͼ�꣬���ָʾ��ͷ. 
**********************************************************************/
static  BOOL    CreateControl(HWND hWnd,HWND * phClose)
{
    int icontrolw,icontrolh,ix[2];
    int icontrolw1,icontrolw2;
    HWND HOn,hEditLimit;

    SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, // �Ҽ��˳�
                (WPARAM)IDC_BUTTON_RIGHT,BUTTON_RIGHT );

    SendMessage(hWnd , PWM_CREATECAPTIONBUTTON, //���ȷ��
                MAKEWPARAM(IDC_BUTTON_LEFT,1),BUTTON_LEFT );

    SendMessage( hWnd, PWM_SETSCROLLSTATE,
                SCROLLLEFT|SCROLLRIGHT|SCROLLDOWN|SCROLLUP, MASKALL );

    icontrolw1  = GetRadioButtonControlWidth(RADIO_CLOSE);
    icontrolw2  = GetRadioButtonControlWidth(RADIO_OPEN);

    icontrolw   = icontrolw1 >= icontrolw2 ? icontrolw1 : icontrolw2;

    CountControlsAbscissa(ix,icontrolw,sizeof(ix));
    icontrolh = GetButtonControlHeight();

    * phClose = CreateWindow(pButtonClass,RADIO_CLOSE,//��
        BS_AUTORADIOBUTTON|WS_CHILD|WS_VISIBLE|WS_TABSTOP,
        ix[0],iControlY,icontrolw,icontrolh,hWnd,(HMENU)IDC_CLOSE,hInstance,NULL);

    if (* phClose == NULL)
        return FALSE;

    HOn = CreateWindow(pButtonClass,RADIO_OPEN,//��
        BS_AUTORADIOBUTTON|WS_CHILD|WS_VISIBLE|WS_TABSTOP,
        ix[1],iControlY,icontrolw,icontrolh,hWnd,(HMENU)IDC_OPEN,hInstance,NULL);

    if (HOn == NULL)
        return FALSE;

    hEditLimit = CreateWindow (pEditClass,"", //��λ����
        WS_VISIBLE | WS_CHILD | ES_CENTER |WS_TABSTOP|ES_NUMBER|WS_BORDER,
        ix[0],iControlY + icontrolh * 2,GetScreenUsableWH1(SM_CXSCREEN) - ix[0] * 2,GetEditControlHeight(),
        hWnd,(HMENU)IDC_EDIT_LIMIT,NULL,NULL);

    if (hEditLimit == NULL)
        return FALSE;

    SendMessage(hEditLimit,EM_LIMITTEXT,ACMMAXLEN,0);

    return TRUE;
}
