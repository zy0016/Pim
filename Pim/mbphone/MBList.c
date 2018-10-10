/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : ��ӵ绰����
 *
 * Purpose  : ֧����ʾ�������ֵ�listbox�ؼ�,ֻ�����ڶ෽ͨ��
 *            
\**************************************************************************/
#include    "winpda.h"
#include    "hp_pdaex.h"
#include    "hpimage.h"
#include    "MBList.h"
#include    "MBPublic.h"
#include    "stdio.h"
#include    "stdlib.h"

//#define DEBUG_MBLIST

static const char * pClassName      = "MBListControl";      //��������
static const char * pStr2Line       = "%s\n%s";
static const char * pStr1Line       = "%s";
static const char * pMBActive       = MBPIC_MBLIST_ACTIVE;//���ڼ����ͼ��
static const char * pMBHold         = MBPIC_MBLIST_HOLD;  //���ڱ��ֵ�ͼ��
static const char * pMBAM           = MBPIC_MBLIST_AM;    //�����
static const char * pMBHM           = MBPIC_MBLIST_HM;    //���־���

static const int iButtonHeight      = 2;                    //�������ĸ�
static const int iVscrollBarWidth   = 20;                   //�������Ŀ�

#define     IDC_VSCROLLBAR          2//������ID
#define     FIRSTMAXLEN             40//��һ��������󳤶�
#define     SECONDMAXLEN            40//�ڶ���������󳤶�
typedef struct tagMBList
{
    BOOL                bSelected;                   //�Ƿ�ѡ��
    int                 iItemData;                   //��Ҫ����ĸ�������
    int                 iCallId;                     //ÿ���绰��ID
    char                cFirstStr[FIRSTMAXLEN + 1];  //��һ������
    char                cSecondStr[SECONDMAXLEN + 1];//�ڶ�������
    int                 iPicState;//ͼ���״̬,TRUE:���ڼ���,FALSE:���ڱ���
    struct tagMBList *  pNext;
};
typedef struct tagMBList MBLIST;

typedef enum
{
    MBLIST_UP       = 1, //���������Ϲ���
    MBLIST_DOWN     = -1,//���������¹���
    MBLIST_MIDDLE   = 0  //����������
}MBLIST_DIRECT;//�����������־

static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
//��ʾ�б��е�����
static  void    DisplayMBList(HDC hdc);
//��õ�ǰ�������λ���ǿɼ���ĵڼ����б���
static  int     GetPtPosition(const POINT * pPoint);
//���ù�������λ��
static  void    SetVScrollBar(void);
//��ʼ���ؼ����������
static  BOOL    InitMBList(void);
//�����Ҫˢ�µ�����
static  BOOL    GetRefreshRectByPoint(RECT * pRefreshRect,int iRefreshIndex);
//����������Ļ�����ĳ���������Ƿ��Ѿ�����ͷ
static  BOOL    GetIfVScrollBarEnd(MBLIST_DIRECT MB_Direct);
//�����������ť��ʱ��,���ù�������λ��(����һ��)
static  void    SetVScrollBarLine(MBLIST_DIRECT MB_Direct);
//�����������ҳ��ť��ʱ��,���ù�������λ��(����һҳ)
static  void    SetVScrollBarPage(MBLIST_DIRECT MB_Direct);
//���ù������϶�ʱ��ĵ�λ��
static  int     SetVScrollBarThumbPosition(void);
//ˢ����Ҫˢ�µ�����
static  void    RefreshRectBySelectChange(int iOldSelectItem,int iCurSelectItem,int iOldTopIndex);
//ȫ��ˢ��
static  void    RefreshAllScreen(void);

static MBLIST     * pMBList;            //�ڵ�����

static BOOL         bShowBmp = TRUE;    //�Ƿ���ʾͨ��״̬��ͼ��
static HBITMAP      hBmpActive;         //���ڼ����ͼ��
static HBITMAP      hBmpHold;           //���ڱ��ֵ�ͼ��
static HBITMAP      hBmpAM;             //�����
static HBITMAP      hBmpHM;             //���־���
static SIZE         SizeActive;         //���ڼ����ͼ��ĳߴ�
static SIZE         SizeHold;           //���ڱ��ֵ�ͼ��ĳߴ�
static SIZE         SizeAM;             //�������ͼƬ�ߴ�
static SIZE         SizeHM;             //���־�����ͼƬ�ߴ�

static int          iMBListX;           //���ں�����
static int          iMBListY;           //����������
static int          iMBListWidth;       //���ڿ�
static int          iMBListHeight;      //���ڸ�
static int          iTextX;             //���ֺ�����

static int          iItemDataHeight;    //������һ����Ŀ�ĸ�
static int          iItemCount;         //��������ʾ��Ŀ��������

static int        * pNodeY;             //��������ڵ��������
static int          iTopIndex;          //���Ϸ��Ľڵ��ǵڼ�������

static HWND         hWndParent;         //�����ھ��
static HINSTANCE    hInstance;
static HWND         hWndMBList;         //��ǰ���ھ��
static int          iMBListID;          //��ǰ����ID

static DWORD        dwFlag;             //��ǰ�ؼ����
HWND    CreateMBListControl(HWND hwndParent,int iId,int x,int y,int nWidth,int nHeight,
                            DWORD dwFlags)
{
    HWND        hWnd;
    WNDCLASS    wc;

    if (hWndMBList != NULL)
        return (hWndMBList);

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
        return NULL;

    bShowBmp        = TRUE;
    hWndParent      = hwndParent;
    iMBListX        = x;
    iMBListY        = y;
    iMBListWidth    = nWidth;
    iMBListHeight   = nHeight;
    dwFlag          = dwFlags;

    if (dwFlags & WS_VSCROLL)
    {   //��������а����������Ļ�,��Ҫ��ȥ�������,����������������,
        //����Ļ�����Ҳ�ᴴ��һ��������,�ᵼ�³�ͻ
        dwFlags = dwFlags ^ WS_VSCROLL;
    }
    hWnd = CreateWindow(pClassName,"",dwFlags,
        x,y,iMBListWidth,iMBListHeight + iButtonHeight,hwndParent, NULL, NULL,NULL);//&iId

    if (NULL == hWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }

    hWndMBList  = hWnd;
    iMBListID   = iId;

    return (hWnd);
}
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    HWND        hVScrollBar;
    LRESULT     lResult;
    HDC         hdc;
    MBLIST      * p;
    POINT       point;
    int         iSelectItem;//��õ�ǰѡ�е���Ŀ
    int         iMBListCount;//�б���Ŀ������
    int         iOldTopIndex;//iTopIndex����ֵ
    SCROLLINFO  VSBInfo;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        //iMBListCount = *((int*)((CREATESTRUCT*)lParam)->lpCreateParams);
        if (!InitMBList())//��ʼ���ؼ����������
            return FALSE;
        //**************��ʼ��һ������****************************
        iTopIndex = 0;//���Ϸ��Ľڵ��ǵڼ�������
        pMBList   = NULL;
        //***************����������*******************************************
        if (dwFlag & WS_VSCROLL)
        {
            hVScrollBar = CreateWindow("SCROLLBAR",NULL,
                WS_VISIBLE|WS_CHILD|SBS_VERT,
                iMBListWidth - iVscrollBarWidth,0,iVscrollBarWidth,iMBListHeight,
                hWnd,(HMENU)IDC_VSCROLLBAR,NULL,NULL);
            if (hVScrollBar == NULL)
                return FALSE;

            memset(&VSBInfo,0x00,sizeof(SCROLLINFO));
            VSBInfo.cbSize = sizeof(SCROLLINFO);
            VSBInfo.fMask  = SIF_ALL;
            SetScrollInfo(hVScrollBar,SB_CTL,&VSBInfo,TRUE);
            ShowWindow(hVScrollBar,SW_HIDE);
        }
        break;

    case WM_DESTROY ://֪ͨ���������,�˳����˳�;
        while (pMBList != NULL)
        {
            p       = pMBList;
            pMBList = pMBList->pNext;
            free(p);
        }
        pMBList     = NULL;
        hWndMBList  = NULL;
        hWndParent  = NULL;

        UnregisterClass(pClassName,NULL);
        DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
        break;

    case WM_PAINT :
        hdc = BeginPaint( hWnd, NULL);
        DisplayMBList(hdc);
        EndPaint(hWnd, NULL);
        break;

    case WM_VSCROLL:
        switch( LOWORD( wParam ))
        {
        case SB_LINEUP:
            if (dwFlag & MBLIST_WS_DISABLE)//��ֹѡ��
                break;
            if (!GetIfVScrollBarEnd(MBLIST_UP))//��������û�е������Ϸ�
            {
                iTopIndex--;
                SetVScrollBarLine(MBLIST_UP);
                RefreshAllScreen();
            }
            SetFocus(hWnd);
            break;
        case SB_LINEDOWN:
            if (!GetIfVScrollBarEnd(MBLIST_DOWN))//��������û�е������·�
            {
                iTopIndex++;
                SetVScrollBarLine(MBLIST_DOWN);
                RefreshAllScreen();
            }
            SetFocus(hWnd);
            break;
        case SB_PAGEUP:
            iMBListCount = MBList_GetCount();
            if (iTopIndex - iItemCount >= 0)
                iTopIndex -= iItemCount;
            else
                iTopIndex = 0;

            SetVScrollBarPage(MBLIST_UP);
            RefreshAllScreen();
            SetFocus(hWnd);
            break;
        case SB_PAGEDOWN:
            iMBListCount = MBList_GetCount();
            if ((iTopIndex + iItemCount <= iMBListCount) && 
                (iMBListCount - iTopIndex - iItemCount > iItemCount))
                iTopIndex += iItemCount;
            else
                iTopIndex = iMBListCount - iItemCount;

            SetVScrollBarPage(MBLIST_DOWN);
            RefreshAllScreen();
            SetFocus(hWnd);
            break;
        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
            iTopIndex = iTopIndex + SetVScrollBarThumbPosition();
            RefreshAllScreen();
            SetFocus(hWnd);
            break;
        default:
            return PDADefWindowProc(hWnd, WM_VSCROLL, wParam, lParam);
        }
        break;

    case WM_PENDOWN:
        if (dwFlag & MBLIST_WS_DISABLE)//��ֹѡ��
            break;
        point.x = LOWORD (lParam);
        point.y = HIWORD (lParam);

        iSelectItem = GetPtPosition(&point);
        if (-1 != iSelectItem)
        {
            if (iSelectItem == MBList_GetCurSel())//˫������Ϣ���ظ�����
            {
                PostMessage(hWndParent,WM_COMMAND,MAKEWPARAM(iMBListID,MBLIST_DBLCLK),0);
            }
            else
            {
                RefreshRectBySelectChange(MBList_GetCurSel(),iSelectItem,iTopIndex);
                MBList_SetCurSel(iSelectItem);
            }
        }
        break;

    case WM_TODESKTOP:
        //PostMessage(hWndParent,WM_TODESKTOP,0,0);//���������ظ�����
        return TRUE;

    case WM_KEYUP:
        switch (LOWORD(wParam))
        {
        case VK_0:  //�������ּ�0
        case VK_1:  //�������ּ�1
        case VK_2:  //�������ּ�2
        case VK_3:  //�������ּ�3
        case VK_4:  //�������ּ�4
        case VK_5:  //�������ּ�5
        case VK_6:  //�������ּ�6
        case VK_7:  //�������ּ�7
        case VK_8:  //�������ּ�8
        case VK_9:  //�������ּ�9
        case VK_F1: //���ż�
        case VK_F2: //�Ҷϼ�
        case KEYASTERICK: //*
        case KEYWELL: //#
        case VK_F5:
        case VK_F10://��softkey
        case VK_RETURN:
        case VK_LEFT:
        case VK_RIGHT:
            PostMessage(hWndParent,WM_KEYUP,LOWORD(wParam),0);//�����ϰ������ظ�����
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_EARPHONE_KEY://�����ϵİ���
            SendMessage(hWndParent,WM_KEYDOWN,VK_F2,0);//�Ҷϵ绰
            break;

        case VK_UP:
            if (dwFlag & MBLIST_WS_DISABLE)//��ֹѡ��
                break;
            iMBListCount = MBList_GetCount();
            iSelectItem  = MBList_GetCurSel();//����ѡ�еĽڵ�����
            iOldTopIndex = iTopIndex;

            if (-1 == iSelectItem)//û��ѡ��ڵ�
            {
                iTopIndex = 0;
                MBList_SetCurSel(0);//ѡ�е�һ���ڵ�
                InvalidateRect(hWndMBList,NULL,TRUE);
                SetVScrollBar();
                return TRUE;
            }
            if (iSelectItem == 0)//��ǰѡ���˵�һ��
            {
                if (iMBListCount > iItemCount)
                    iTopIndex = iMBListCount - iItemCount;

                MBList_SetCurSel(iMBListCount - 1);//ѡ�����һ��
                RefreshRectBySelectChange(iSelectItem,iMBListCount - 1,iOldTopIndex);
            }
            else
            {
                if (iTopIndex > iSelectItem)//ѡ��Ľڵ��ڿ��ӽڵ�֮��
                    iTopIndex = iSelectItem - 1;
                else if (iSelectItem - iTopIndex + 1 > iItemCount)//ѡ��Ľڵ��ڿ��ӽڵ�֮��
                    iTopIndex = iSelectItem - iItemCount;
                else if (iTopIndex == iSelectItem)//ѡ�еĽڵ������ʾ�����Ϸ��Ľڵ�
                    iTopIndex--;

                MBList_SetCurSel(iSelectItem - 1);//ѡ����һ��
                RefreshRectBySelectChange(iSelectItem,iSelectItem - 1,iOldTopIndex);
            }
            SetVScrollBar();
            PostMessage(hWndParent,WM_COMMAND,MAKEWPARAM(iMBListID,MBLIST_SELCHANGE),0);

            PostMessage(hWndParent,WM_KEYDOWN,VK_UP,0);
            break;

        case VK_DOWN:
            if (dwFlag & MBLIST_WS_DISABLE)//��ֹѡ��
                break;
            iMBListCount = MBList_GetCount();
            iSelectItem  = MBList_GetCurSel();//����ѡ�еĽڵ�����
            iOldTopIndex = iTopIndex;
            if (-1 == iSelectItem)//û��ѡ��ڵ�
            {
                iTopIndex = 0;
                MBList_SetCurSel(0);//ѡ�е�һ���ڵ�
                InvalidateRect(hWndMBList,NULL,TRUE);
                SetVScrollBar();
                return TRUE;
            }
            if (iSelectItem == iMBListCount - 1)//ѡ�������һ��
            {
                iTopIndex = 0;
                MBList_SetCurSel(0);//ѡ�е�һ��
                RefreshRectBySelectChange(iSelectItem,0,iOldTopIndex);
            }
            else
            {
                if (iTopIndex > iSelectItem)//ѡ��Ľڵ��ڿ��ӽڵ�֮��
                    iTopIndex = iSelectItem + 1;
                else if (iSelectItem - iTopIndex + 1 > iItemCount)//ѡ��Ľڵ��ڿ��ӽڵ�֮��
                    iTopIndex = iSelectItem + 2 - iItemCount;
                else if (iSelectItem - iTopIndex + 1 == iItemCount)//���ڿ��ӷ�Χ���·����нڵ�����
                    iTopIndex++;//ѡ����һ��

                MBList_SetCurSel(iSelectItem + 1);//ѡ����һ��
                RefreshRectBySelectChange(iSelectItem,iSelectItem + 1,iOldTopIndex);
            }
            SetVScrollBar();
            PostMessage(hWndParent,WM_COMMAND,MAKEWPARAM(iMBListID,MBLIST_SELCHANGE),0);

            PostMessage(hWndParent,WM_KEYDOWN,VK_DOWN,0);
            break;
        case VK_PAGEUP:
            //PostMessage(hWnd,WM_VSCROLL,SB_PAGEUP,0);
            //break;
        case VK_PAGEDOWN:
            //PostMessage(hWnd,WM_VSCROLL,SB_PAGEDOWN,0);
            //break;
        case VK_0:  //�������ּ�0
        case VK_1:  //�������ּ�1
        case VK_2:  //�������ּ�2
        case VK_3:  //�������ּ�3
        case VK_4:  //�������ּ�4
        case VK_5:  //�������ּ�5
        case VK_6:  //�������ּ�6
        case VK_7:  //�������ּ�7
        case VK_8:  //�������ּ�8
        case VK_9:  //�������ּ�9
        case VK_F1: //���ż�
        case VK_F2: //�Ҷϼ�
        case KEYASTERICK: //*
        case KEYWELL: //#
        case VK_F5:
        case VK_F10://��softkey
        case VK_RETURN:
        case VK_LEFT:
        case VK_RIGHT:
        case VK_FLIP_OFF://�ϸǹҶ�
            PostMessage(hWndParent,WM_KEYDOWN,LOWORD(wParam),0);//�����ϰ������ظ�����
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
/********************************************************************
* Function     InitMBList
* Purpose      ��ʼ���ؼ����������
* Params       
* Return     
* Remarks      
**********************************************************************/
static  BOOL    InitMBList(void)
{
    static BOOL bInit = FALSE;//��ʶͼ���ʼ���Ƿ����
    BOOL        bTran;
    COLORREF    Color;
    HDC         hdc;
    int         iFontHeight,i;

    if (bInit)//ͼ���ʼ���Ѿ����
        return TRUE;
    //��ʼ��ͼ��
    memset(&SizeActive,0x00,sizeof(SIZE));
    memset(&SizeHold  ,0x00,sizeof(SIZE));
    memset(&SizeAM    ,0x00,sizeof(SIZE));
    memset(&SizeHM    ,0x00,sizeof(SIZE));

    GetImageDimensionFromFile(pMBActive,&SizeActive);//�õ�ͼ��Ĵ�С
    GetImageDimensionFromFile(pMBHold  ,&SizeHold  );//�õ�ͼ��Ĵ�С
    GetImageDimensionFromFile(pMBAM    ,&SizeAM    );//�õ�ͼ��Ĵ�С
    GetImageDimensionFromFile(pMBHM    ,&SizeHM    );//�õ�ͼ��Ĵ�С

    hdc        = CreateMemoryDC(SizeActive.cx,SizeActive.cy);
    hBmpActive = CreateBitmapFromImageFile(hdc,pMBActive,&Color,&bTran);
    hBmpHold   = CreateBitmapFromImageFile(hdc,pMBHold  ,&Color,&bTran);
    hBmpAM     = CreateBitmapFromImageFile(hdc,pMBAM    ,&Color,&bTran);
    hBmpHM     = CreateBitmapFromImageFile(hdc,pMBHM    ,&Color,&bTran);
    DeleteObject(hdc);
    //��ʼ��������һ����Ŀ�ĸ�
    iFontHeight = GetCharactorHeight(NULL);
    if (iFontHeight * 2 > SizeActive.cy)//��ô�����һ����Ŀ�ĸ�
        iItemDataHeight = iFontHeight * 2;
    else
        iItemDataHeight = SizeActive.cy;

    iItemCount = iMBListHeight / iItemDataHeight;//��������ʾ��Ŀ��������
    iTextX     = iMBListX + SizeActive.cx;//���ֺ�����
    //////////////��������ڵ��������////////////////////////////////////////////////////////////
    pNodeY = (int *)malloc(iItemCount * sizeof(int));
    if (pNodeY == NULL)
        return FALSE;
    for (i = 0;i < iItemCount;i++)
    {
        pNodeY[i] = iItemDataHeight * i;//��������ڵ��������
    }
    bInit = TRUE;
    return TRUE;
}
/********************************************************************
* Function     GetPtPosition
* Purpose      ��õ�ǰ�������λ���ǿɼ���ĵڼ����б���
* Params       ��ǰ�������λ��
* Return     ��ǰ�������λ���ǿɼ���ĵڼ����б���
* Remarks      
**********************************************************************/
static  int     GetPtPosition(const POINT * pPoint)
{
    int i,iMBListCount;

    if (pNodeY == NULL)
        return -1;

    iMBListCount = MBList_GetCount();

    if (iMBListCount > iItemCount)
        iMBListCount = iItemCount;

    for (i = 0;i < iMBListCount;i++)
    {
        if ((pNodeY[i] < pPoint->y) && (pPoint->y < pNodeY[i] + iItemDataHeight))
            return (i + iTopIndex);
    }
    return -1;
}
/********************************************************************
* Function     DisplayMBList
* Purpose      ��ʾ�б��е�����
* Params       iTopIndex:���Ϸ��Ľڵ��ǵڼ�������,���ӵڼ����ڵ㿪ʼ��ʾ
* Return           
* Remarks      
**********************************************************************/
static  void    DisplayMBList(HDC hdc)
{
    MBLIST * p = pMBList;
    RECT    rect;
    char    cDisplay[FIRSTMAXLEN + SECONDMAXLEN + 1] = "";
    int     i = 0;

    if (NULL == pNodeY)
        return;
    while (p != NULL)
    {
        if (i - iTopIndex > iItemCount)//�ýڵ�����ʾ����֮��
            return;

        if (i >= iTopIndex)
        {
            if (bShowBmp)//��ʾͨ��״̬��ͼ��
            {
                SetRect(&rect,iTextX,pNodeY[i - iTopIndex],
                    iMBListWidth,pNodeY[i - iTopIndex] + iItemDataHeight);
                switch (p->iPicState)
                {
                case MBLIST_IPICNONE://û��
                    SetRect(&rect,0,pNodeY[i - iTopIndex],//��ʾ���ֵ�������
                        iMBListWidth,pNodeY[i - iTopIndex] + iItemDataHeight);
                    break;
                case MBLIST_IPICSTATE_A://����
                    StretchBlt( hdc,
                        iMBListX, pNodeY[i - iTopIndex], SizeActive.cx,SizeActive.cy,//Ŀ�ľ���
                        (HDC)hBmpActive,0,0,SizeActive.cx,SizeActive.cy,//ͼ��ԭ����
                        SRCCOPY);//��λͼ��Դ���ο�����Ŀ�ľ��Σ���չ��ѹ��λͼ��ʹ���ʺ�Ŀ�ľ��εķ�Χ
                    break;
                case MBLIST_IPICSTATE_H://����
                    StretchBlt( hdc,
                        iMBListX, pNodeY[i - iTopIndex], SizeHold.cx,SizeHold.cy,//Ŀ�ľ���
                        (HDC)hBmpHold,0,0,SizeHold.cx,SizeHold.cy,//ͼ��ԭ����
                        SRCCOPY);//��λͼ��Դ���ο�����Ŀ�ľ��Σ���չ��ѹ��λͼ��ʹ���ʺ�Ŀ�ľ��εķ�Χ
                    break;
                case MBLIST_IPICSTATE_AM://�����
                    StretchBlt( hdc,
                        iMBListX, pNodeY[i - iTopIndex], SizeAM.cx,SizeAM.cy,//Ŀ�ľ���
                        (HDC)hBmpAM,0,0,SizeAM.cx,SizeAM.cy,//ͼ��ԭ����
                        SRCCOPY);//��λͼ��Դ���ο�����Ŀ�ľ��Σ���չ��ѹ��λͼ��ʹ���ʺ�Ŀ�ľ��εķ�Χ
                    break;
                case MBLIST_IPICSTATE_HM://���־���
                    StretchBlt( hdc,
                        iMBListX, pNodeY[i - iTopIndex], SizeHM.cx,SizeHM.cy,//Ŀ�ľ���
                        (HDC)hBmpHM,0,0,SizeHM.cx,SizeHM.cy,//ͼ��ԭ����
                        SRCCOPY);//��λͼ��Դ���ο�����Ŀ�ľ��Σ���չ��ѹ��λͼ��ʹ���ʺ�Ŀ�ľ��εķ�Χ
                    break;
                }
            }
            else//����ʾͨ��״̬��ͼ��
            {
                SetRect(&rect,0,pNodeY[i - iTopIndex],//��ʾ���ֵ�������
                    iMBListWidth,pNodeY[i - iTopIndex] + iItemDataHeight);
            }
            if (dwFlag & WS_VSCROLL)//�й�����
            {
                rect.right -= iVscrollBarWidth;//�޸�ˢ������
            }

            memset(cDisplay,0x00,sizeof(cDisplay));
            if ((strlen(p->cFirstStr) != 0) && (strlen(p->cSecondStr) != 0))
                sprintf(cDisplay,pStr2Line,p->cFirstStr,p->cSecondStr);
            else if ((strlen(p->cFirstStr) != 0) && (strlen(p->cSecondStr) == 0))
                sprintf(cDisplay,pStr1Line,p->cFirstStr);
            else if ((strlen(p->cFirstStr) == 0) && (strlen(p->cSecondStr) != 0))
                sprintf(cDisplay,pStr1Line,p->cSecondStr);

            DrawText(hdc,cDisplay,strlen(cDisplay),&rect,DT_LEFT|DT_VCENTER);//�������

            if (p->bSelected)//��ת��ɫ
            {
                BOOL bRes;
                bRes = InvertRect(hdc,&rect);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBLIST)
                printf("\nMB MBList,InvertRect:%d,top:%d,left:%d,right:%d,bottom:%d\n",
                    bRes,rect.top,rect.left,rect.right,rect.bottom);
#endif
            }
        }
        i++;
        p = p->pNext;
    }
}
/********************************************************************
* Function     GetIfVScrollBarEnd
* Purpose      ����������Ļ�����ĳ���������Ƿ��Ѿ�����ͷ
* Params       MB_Direct:��������
* Return       TRUE:��ù�������ĳ���������Ѿ�����ͷ,���򷵻�FALSE
* Remarks      
**********************************************************************/
static  BOOL    GetIfVScrollBarEnd(MBLIST_DIRECT MB_Direct)
{
    SCROLLINFO  VSBInfo;
    HWND        hVScrollBar;
    int         iCount;

    hVScrollBar = GetDlgItem(hWndMBList,IDC_VSCROLLBAR);
    if (NULL == hVScrollBar)
        return FALSE;

    iCount = MBList_GetCount();
    if (iCount > iItemCount)//��Ŀ����������Ļ�����ʾ����,��Ҫ��ʾ������
    {
        GetScrollInfo(hVScrollBar,SB_CTL,&VSBInfo);

        switch (MB_Direct)
        {
        case MBLIST_UP:
            if (VSBInfo.nPos == 0)
                return TRUE;
            break;
        case MBLIST_DOWN:
            if (VSBInfo.nPos == iCount - iItemCount)
                return TRUE;
            break;
        }
    }
    return FALSE;
}
/********************************************************************
* Function     SetVScrollBarThumbPosition
* Purpose      ���ù������϶�ʱ��ĵ�λ��
* Params       
* Return       ���ϻ��������ƶ��ĳ���    
* Remarks      
**********************************************************************/
static  int     SetVScrollBarThumbPosition()
{
    int         iCount,iStep;
    SCROLLINFO  VSBInfo;
    HWND        hVScrollBar;

    hVScrollBar = GetDlgItem(hWndMBList,IDC_VSCROLLBAR);
    if (NULL == hVScrollBar)
        return 0;

    iCount = MBList_GetCount();
    if (iCount > iItemCount)//��Ŀ����������Ļ�����ʾ����,��Ҫ��ʾ������
    {
        GetScrollInfo(hVScrollBar,SB_CTL,&VSBInfo);

        VSBInfo.cbSize  = sizeof(SCROLLINFO);
        VSBInfo.fMask   = SIF_ALL;
        VSBInfo.nMin    = 0;
        VSBInfo.nMax    = iCount - 1;
        VSBInfo.nPage   = iItemCount;

        iStep           = VSBInfo.nTrackPos - VSBInfo.nPos;
        VSBInfo.nPos    = VSBInfo.nTrackPos;

        SetScrollInfo( hVScrollBar, SB_CTL, &VSBInfo, TRUE );
        if (!IsWindowVisible(hVScrollBar))
            ShowWindow(hVScrollBar,SW_SHOW);

        return iStep;
    }
    return 0;
}
/********************************************************************
* Function     SetVScrollBarPage
* Purpose      �����������ҳ��ť��ʱ��,���ù�������λ��
* Params       MB_Direct,1:���Ϲ���,-1:���¹���,0λ�ò���
* Return       ��    
* Remarks      ���ڹ�����һҳ��ʱ��,���ù�����
**********************************************************************/
static  void    SetVScrollBarPage(MBLIST_DIRECT MB_Direct)
{
    int         iCount;
    SCROLLINFO  VSBInfo;
    HWND        hVScrollBar;

    hVScrollBar = GetDlgItem(hWndMBList,IDC_VSCROLLBAR);
    if (NULL == hVScrollBar)
        return;

    iCount = MBList_GetCount();
    if (iCount > iItemCount)//��Ŀ����������Ļ�����ʾ����,��Ҫ��ʾ������
    {
        GetScrollInfo(hVScrollBar,SB_CTL,&VSBInfo);

        VSBInfo.cbSize  = sizeof(SCROLLINFO);
        VSBInfo.fMask   = SIF_ALL;
        VSBInfo.nMin    = 0;
        VSBInfo.nMax    = iCount - 1;
        VSBInfo.nPage   = iItemCount;

        switch (MB_Direct)
        {
        case MBLIST_UP://up
            if (VSBInfo.nMax == VSBInfo.nPos)
                VSBInfo.nPos -= (iItemCount + 2);
            else
                VSBInfo.nPos -= iItemCount;
            break;
        case MBLIST_DOWN://down
            VSBInfo.nPos += iItemCount;
            break;
        default:
            return;
        }
        SetScrollInfo( hVScrollBar, SB_CTL, &VSBInfo, TRUE );
        if (!IsWindowVisible(hVScrollBar))
            ShowWindow(hVScrollBar,SW_SHOW);
    }
}
/********************************************************************
* Function     SetVScrollBarLine
* Purpose      �����������ť��ʱ��,���ù�������λ��
* Params       MB_Direct,1:���Ϲ���,-1:���¹���,0λ�ò���
* Return       ��    
* Remarks      ���ڹ���һ�е�ʱ��,���ù�����
**********************************************************************/
static  void    SetVScrollBarLine(MBLIST_DIRECT MB_Direct)
{
    int         iCount;
    SCROLLINFO  VSBInfo;
    HWND        hVScrollBar;

    hVScrollBar = GetDlgItem(hWndMBList,IDC_VSCROLLBAR);
    if (NULL == hVScrollBar)
        return;

    iCount = MBList_GetCount();
    if (iCount > iItemCount)//��Ŀ����������Ļ�����ʾ����,��Ҫ��ʾ������
    {
        GetScrollInfo(hVScrollBar,SB_CTL,&VSBInfo);

        VSBInfo.cbSize  = sizeof(SCROLLINFO);
        VSBInfo.fMask   = SIF_ALL;
        VSBInfo.nMin    = 0;
        VSBInfo.nMax    = iCount - 1;
        VSBInfo.nPage   = iItemCount;

        switch (MB_Direct)
        {
        case MBLIST_UP://up
            if (VSBInfo.nMax == VSBInfo.nPos)
                VSBInfo.nPos -= 2;
            else
                VSBInfo.nPos--;
            break;
        case MBLIST_DOWN://down
            VSBInfo.nPos++;
            break;
        default:
            return;
        }
        SetScrollInfo( hVScrollBar, SB_CTL, &VSBInfo, TRUE );
        if (!IsWindowVisible(hVScrollBar))
            ShowWindow(hVScrollBar,SW_SHOW);
    }
}
/********************************************************************
* Function     SetVScrollBar
* Purpose      ���ù�������λ��
* Params       
* Return       ��    
* Remarks      ����ѡ�����¼���ʱ��,���ù�����
**********************************************************************/
static  void    SetVScrollBar(void)
{
    int         iCount;
    SCROLLINFO  VSBInfo;
    HWND        hVScrollBar;

    hVScrollBar = GetDlgItem(hWndMBList,IDC_VSCROLLBAR);
    if (NULL == hVScrollBar)
        return;

    iCount = MBList_GetCount();
    if (iCount > iItemCount)//��Ŀ����������Ļ�����ʾ����,��Ҫ��ʾ������
    {
        GetScrollInfo(hVScrollBar,SB_CTL,&VSBInfo);

        VSBInfo.cbSize  = sizeof(SCROLLINFO);
        VSBInfo.fMask   = SIF_ALL;
        VSBInfo.nMin    = 0;
        VSBInfo.nMax    = iCount - 1;
        VSBInfo.nPage   = iItemCount;
        VSBInfo.nPos    = iTopIndex;

        SetScrollInfo( hVScrollBar, SB_CTL, &VSBInfo, TRUE );
        if (!IsWindowVisible(hVScrollBar))
            ShowWindow(hVScrollBar,SW_SHOW);
    }
    else
    {
        if (IsWindowVisible(hVScrollBar))
            ShowWindow(hVScrollBar,SW_HIDE);
    }
    SendMessage(hWndMBList,PWM_SETSCROLLSTATE,SCROLLDOWN|SCROLLUP,MASKALL);
}
/********************************************************************
* Function     RefreshAllScreen
* Purpose      ȫ��ˢ��
* Params       
* Return       
* Remarks      
**********************************************************************/
static  void    RefreshAllScreen(void)
{
    HWND hVScrollBar;
    RECT rectCur;

    hVScrollBar = GetDlgItem(hWndMBList,IDC_VSCROLLBAR);
    if (NULL != hVScrollBar)//�й�����
    {
        SetRect(&rectCur,iMBListX,iMBListY,iMBListWidth - iVscrollBarWidth,iMBListHeight);
        InvalidateRect(hWndMBList,&rectCur,TRUE);
    }
    else
    {
        InvalidateRect(hWndMBList,NULL,TRUE);
    }
}
/********************************************************************
* Function     RefreshRectBySelectChange
* Purpose      ˢ����Ҫˢ�µ�����
* Params       iOldSelectItem:ԭ��ѡ��Ľڵ�,iCurSelectItem:��ǰѡ��Ľڵ�
                iOldTopIndex:ԭiTopIndex
* Return       
* Remarks      
**********************************************************************/
static  void    RefreshRectBySelectChange(int iOldSelectItem,int iCurSelectItem,int iOldTopIndex)
{
    HWND hVScrollBar;
    RECT rectOld,rectCur;
    if (iOldTopIndex != iTopIndex)//��Ҫˢ��ȫ��
    {
        hVScrollBar = GetDlgItem(hWndMBList,IDC_VSCROLLBAR);
        if (NULL != hVScrollBar)//�й�����
        {
            SetRect(&rectCur,iMBListX,iMBListY,iMBListWidth - iVscrollBarWidth,iMBListHeight);
            InvalidateRect(hWndMBList,&rectCur,TRUE);
        }
        else
        {
            InvalidateRect(hWndMBList,NULL,TRUE);
        }
        return;
    }
    if (GetRefreshRectByPoint(&rectOld,iOldSelectItem))
        InvalidateRect(hWndMBList,&rectOld,TRUE);

    if (GetRefreshRectByPoint(&rectCur,iCurSelectItem))
        InvalidateRect(hWndMBList,&rectCur,TRUE);
}
/********************************************************************
* Function     GetRefreshRectByPoint
* Purpose      �����Ҫˢ�µ�����
* Params       pRefreshRect:����ˢ������,iRefreshIndex:��Ҫˢ�µĽڵ�
* Return       TRUE:��Ҫˢ��,FALSE:����Ҫˢ��
* Remarks      
**********************************************************************/
static  BOOL    GetRefreshRectByPoint(RECT * pRefreshRect,int iRefreshIndex)
{
    HWND hVScrollBar;

    if ((iRefreshIndex - iTopIndex >= iItemCount) ||//�ýڵ�����ʾ����֮��,����Ҫˢ��
        (iRefreshIndex < iTopIndex))//�ýڵ�����ʾ����֮��,����Ҫˢ��
        return FALSE;

    if (pNodeY == NULL)
        return FALSE;

    if (pRefreshRect != NULL)
    {
        hVScrollBar = GetDlgItem(hWndMBList,IDC_VSCROLLBAR);
        if (NULL != hVScrollBar)//�й�����
        {
            SetRect(pRefreshRect,0,pNodeY[iRefreshIndex - iTopIndex],
                iMBListWidth - iVscrollBarWidth,pNodeY[iRefreshIndex - iTopIndex] + iItemDataHeight);
        }
        else
        {
            SetRect(pRefreshRect,0,pNodeY[iRefreshIndex - iTopIndex],
                iMBListWidth,pNodeY[iRefreshIndex - iTopIndex] + iItemDataHeight);
        }
    }
    return TRUE;
}
//////////////////////////////////////////////////////////////////////////
/********************************************************************
* Function     MBList_AddItem
* Purpose      �������
* Params       iPicState:ͼƬ��־,iData:��������
* Return     �ڵ����б��е�λ��    
* Remarks      
**********************************************************************/
int     MBList_AddItem(const char * pFirstStr,const char * pSecondStr,
                       int iPicState,int iData,int iCallId)
{
    MBLIST * p = NULL,* pnext;
    int iSour,iCount;
    RECT rRefresh;

    p = (MBLIST*)malloc(sizeof(MBLIST));
    if (p == NULL)
        return -1;

    memset(p,0x00,sizeof(MBLIST));

    if (pFirstStr != NULL)
    {
        iSour = strlen(pFirstStr);
        strncpy(p->cFirstStr,pFirstStr,sizeof(p->cFirstStr) > iSour ? iSour + 1 : sizeof(p->cFirstStr) - 1);
    }
    if (pSecondStr != NULL)
    {
        iSour = strlen(pSecondStr);
        strncpy(p->cSecondStr,pSecondStr,sizeof(p->cSecondStr) > iSour ? iSour + 1 : sizeof(p->cSecondStr) - 1);
    }
    p->iPicState = iPicState;
    p->iItemData = iData;
    p->bSelected = FALSE;
    p->iCallId   = iCallId;

    if (MBList_GetCount() == 0)//����ͷ�ڵ�
    {
        iCount   = 0;
        pMBList  = p;
        p->pNext = NULL;
    }
    else
    {
        iCount = 1;
        pnext  = pMBList;
        while (pnext->pNext != NULL)//�����������β
        {
            pnext = pnext->pNext;
            iCount++;
        }
        pnext->pNext = p;//���½��Ľڵ���ӵ������β
        p->pNext     = NULL;
    }
    if (IsWindow(hWndMBList))
    {
        if (GetRefreshRectByPoint(&rRefresh,iCount))
            InvalidateRect(hWndMBList,&rRefresh,TRUE);
    }
    SetVScrollBar();
    return (iCount);
}
/********************************************************************
* Function     MBList_DeleteItem
* Purpose      ɾ����iIndex���ڵ�
* Params       
* Return     ʣ�������      
* Remarks      
**********************************************************************/
int     MBList_DeleteItem(int iIndex)
{
    MBLIST * pCur,* pCurNext;
    int i,iCount,iSelIndex;

    iCount = MBList_GetCount();

    if ((0 == iCount) || (iIndex < 0) || (iIndex > iCount - 1))
        return (iCount);

    iSelIndex = MBList_GetCurSel();//����ѡ�еĽڵ�����

    if (0 == iIndex)//ɾ��ͷ�ڵ�
    {
        pCur    = pMBList;
        pMBList = pMBList->pNext;
        free(pCur);
    }
    else if (iIndex == iCount - 1)//ɾ��β�ڵ�
    {
        pCur     = pMBList;
        pCurNext = pCur->pNext;
        while (pCurNext->pNext != NULL)//��������β�ڵ�,ʹpCurNextָ��β�ڵ�
        {
            pCur     = pCur->pNext;
            pCurNext = pCur->pNext;
        }
        free(pCurNext);
        pCur->pNext = NULL;
    }
    else//ɾ���м�ڵ�
    {
        i        = 0;
        pCur     = pMBList;
        pCurNext = pCur->pNext;
        while (i < iIndex - 1)
        {
            pCur     = pCur->pNext;
            pCurNext = pCur->pNext;
            i++;
        }
        pCur->pNext = pCurNext->pNext;
        free(pCurNext);
    }
    //�ýڵ����һ���ڵ�Ӧ��������
    if ((iCount - 1 - iTopIndex < iItemCount) && (iTopIndex > 0))
        iTopIndex--;

    if (iSelIndex == iIndex)//ѡ��Ľڵ���ǵ�ǰ��ɾ���Ľڵ�
    {
        if (iSelIndex == iCount - 1)//ԭ��ѡ��Ľڵ������һ���ڵ�,ѡ�����һ���ڵ�
            MBList_SetCurSel(iCount - 2);
        else//ԭ��ѡ��Ľڵ㲻�����һ���ڵ�,ѡ��ԭ��ѡ��ڵ����һ���ڵ�
            MBList_SetCurSel(iSelIndex + 1);
    }

    if (IsWindow(hWndMBList))
        InvalidateRect(hWndMBList,NULL,TRUE);

    SetVScrollBar();
    return (iCount - 1);
}
/********************************************************************
* Function     MBList_Destroy
* Purpose      ���ٴ���
* Params       
* Return           
* Remarks      
**********************************************************************/
void    MBList_Destroy(void)
{
    if (IsWindow(hWndMBList))
        DestroyWindow(hWndMBList);
}
/********************************************************************
* Function     MBList_ExistCallId
* Purpose      iCallId�Ƿ���������Ľڵ��д���
* Params       iIndex:��iIndex���ڵ�
* Return           
* Remarks      
**********************************************************************/
BOOL    MBList_ExistCallId(int iCallId)
{
    MBLIST * p = pMBList;

    while (p != NULL)
    {
        if (iCallId == p->iCallId)
            return TRUE;

        p = p->pNext;
    }
    return FALSE;
}
/********************************************************************
* Function     MBList_GetCount
* Purpose      �������ڵ���
* Params       
* Return           
* Remarks      
**********************************************************************/
int     MBList_GetCount(void)
{
    int iCount = 0;
    MBLIST * p = pMBList;

    while (p != NULL)
    {
        p = p->pNext;
        iCount++;
    }
    return (iCount);
}
/********************************************************************
* Function     MBList_GetCurSel
* Purpose      ����ѡ�еĽڵ�����
* Params       
* Return           
* Remarks      
**********************************************************************/
int     MBList_GetCurSel(void)
{
    MBLIST * p = pMBList;
    int i = 0;

    while (p != NULL)
    {
        if (p->bSelected)
            return (i);

        p = p->pNext;
        i++;
    }
    return (-1);
}
/********************************************************************
* Function     MBList_GetItem
* Purpose      ��õ�iIndex���ڵ�İ���������
* Params       iIndex:��iIndex���ڵ�
* Return           
* Remarks      
**********************************************************************/
int     MBList_GetItem(int iIndex)
{
    MBLIST * p = pMBList;
    int i = 0;

    if (iIndex < 0)
        return (-1);

    while (p != NULL)
    {
        if (i == iIndex)
            return (p->iItemData);

        p = p->pNext;
        i++;
    }
    return (-1);
}
/********************************************************************
* Function     MBList_GetString
* Purpose      ��õ�iIndex���ڵ�ĵ�һ�еڶ�������
* Params       iIndex:��iIndex���ڵ�
* Return           
* Remarks      
**********************************************************************/
BOOL    MBList_GetString(int iIndex,char * pFirstStr,char * pSecondStr)
{
    MBLIST * p = pMBList;
    int i = 0;

    if (iIndex < 0)
        return FALSE;

    while (p != NULL)
    {
        if (i == iIndex)
        {
            if (pFirstStr != NULL)
                strcpy(pFirstStr,p->cFirstStr);
            if (pSecondStr != NULL)
                strcpy(pSecondStr,p->cSecondStr);
            return TRUE;
        }
        p = p->pNext;
        i++;
    }
    return FALSE;
}
/********************************************************************
* Function     MBList_Refresh
* Purpose      ˢ�µ�ǰ�б�
* Params       
* Return           
* Remarks      
**********************************************************************/
void    MBList_Refresh(void)
{
    int iCount,i;
    int iFirstLen,iSecondLen,iTxtMaxLen;
    RECT rect;
    char cFirstStr[FIRSTMAXLEN + 1] = "",cSecondStr[SECONDMAXLEN + 1] = "";

    if (IsWindow(hWndMBList))
    {
        iCount = MBList_GetCount();
        for (i = 0 ;i < iCount;i++)
        {
            if (GetRefreshRectByPoint(&rect,i))
            {
                memset(cFirstStr ,0x00,sizeof(cFirstStr ));
                memset(cSecondStr,0x00,sizeof(cSecondStr));
                if (MBList_GetString(i,cFirstStr,cSecondStr))
                {
                    iFirstLen  = strlen(cFirstStr);
                    iSecondLen = strlen(cSecondStr);
                    iTxtMaxLen = GetStringWidth(NULL,
                        iFirstLen > iSecondLen ? cFirstStr : cSecondStr);

                    rect.right = iTextX + iTxtMaxLen;
                }
                InvalidateRect(hWndMBList,&rect,TRUE);
            }
        }
    }
}
/********************************************************************
* Function     MBList_ResetItem
* Purpose      ɾ�����нڵ�
* Params       
* Return           
* Remarks      
**********************************************************************/
void    MBList_ResetItem(void)
{
    MBLIST * p;

    iTopIndex = 0;
    while (pMBList != NULL)
    {
        p       = pMBList;
        pMBList = pMBList->pNext;
        free(p);
    }
    if (IsWindow(hWndMBList))
        InvalidateRect(hWndMBList,NULL,TRUE);
}
/********************************************************************
* Function     MBList_SetCurSel
* Purpose      ����ѡ�е�index���ڵ�
* Params       index:ѡ�е�index���ڵ�,index<0 ��ʱ��ѡ�е�һ���ڵ�
                index���ڽڵ������ʱ��ѡ�����һ���ڵ�
* Return           
* Remarks      
**********************************************************************/
void    MBList_SetCurSel(int index)
{
    MBLIST * p = pMBList;
    int i = 0,iListCount;

    iListCount = MBList_GetCount();//��ýڵ�����

    if (index < 0)//ѡ�е�һ���ڵ�
        index = 0;
    else if (index >= iListCount)//ѡ�����һ���ڵ�
        index = iListCount - 1;

    while (p != NULL)
    {
        p->bSelected = (i == index);

        p = p->pNext;
        i++;
    }
}
/********************************************************************
* Function     MBList_SetItem
* Purpose      ���õ�iIndex���ڵ������
* Params       iPicState/iData/iCallId:-1��ʾ���ı䵱ǰ�ڵ�״̬
                bRefresh:�Ƿ�����ˢ��
* Return           
* Remarks      
**********************************************************************/
void    MBList_SetItem(int iIndex,const char * pFirstStr,const char * pSecondStr,
                       int iPicState,int iData,int iCallId,BOOL bRefresh)
{
    MBLIST * p = pMBList;
    int i = 0,iSour;
    RECT rRefresh;

    while (p != NULL)
    {
        if (i == iIndex)
        {
            if (pFirstStr != NULL)
            {
                iSour = strlen(pFirstStr);
                strncpy(p->cFirstStr,pFirstStr,sizeof(p->cFirstStr) > iSour ? iSour + 1 : sizeof(p->cFirstStr) - 1);
            }
            if (pSecondStr != NULL)
            {
                iSour = strlen(pSecondStr);
                strncpy(p->cSecondStr,pSecondStr,sizeof(p->cSecondStr) > iSour ? iSour + 1 : sizeof(p->cSecondStr) - 1);
            }
            if (iPicState != MBLIST_IPIC_UNCHANGE)
                p->iPicState = iPicState;

            if (iData != -1)
                p->iItemData = iData;

            if (iCallId != -1)
                p->iCallId = iCallId;

            break;
        }
        p = p->pNext;
        i++;
    }
    if ((bRefresh) && (IsWindow(hWndMBList)))
    {
        if (GetRefreshRectByPoint(&rRefresh,iIndex))
        {
            InvalidateRect(hWndMBList,&rRefresh,TRUE);
        }
    }
}
/********************************************************************
* Function     MBList_SetStyle
* Purpose      ���ÿؼ����
* Params       
* Return           
* Remarks      
**********************************************************************/
void    MBList_SetStyle(DWORD dwStyle)
{
    dwFlag = dwStyle;
}
/********************************************************************
* Function     MBList_Show
* Purpose      ���ֻ������ؿؼ�
* Params       
* Return           
* Remarks      
**********************************************************************/
BOOL    MBList_Show(int nCmdShow)
{
    if (!IsWindow(hWndMBList))
        return FALSE;
    switch (nCmdShow)
    {
    case SW_HIDE:
        return ShowWindow(hWndMBList,SW_HIDE);
    case SW_SHOW:
        ShowWindow(hWndMBList,SW_SHOW);
        SetFocus(hWndMBList);
        return TRUE;
    }
    return FALSE;
}
/********************************************************************
* Function     MBList_ShowListBmp
* Purpose      �Ƿ���ʾͨ��״̬��ͼ��
* Params       
* Return           
* Remarks      
**********************************************************************/
BOOL    MBList_ShowListBmp(BOOL bShow)
{
    if (IsWindow(hWndMBList))
    {
        bShowBmp = bShow;
        MBList_Show(SW_SHOW);
        return TRUE;
    }
    return FALSE;
}
