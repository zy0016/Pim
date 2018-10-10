/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 打接电话部分
 *
 * Purpose  : 支持显示两行文字的listbox控件,只适用于多方通话
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

static const char * pClassName      = "MBListControl";      //窗口类名
static const char * pStr2Line       = "%s\n%s";
static const char * pStr1Line       = "%s";
static const char * pMBActive       = MBPIC_MBLIST_ACTIVE;//正在激活的图标
static const char * pMBHold         = MBPIC_MBLIST_HOLD;  //正在保持的图标
static const char * pMBAM           = MBPIC_MBLIST_AM;    //激活静音
static const char * pMBHM           = MBPIC_MBLIST_HM;    //保持静音

static const int iButtonHeight      = 2;                    //按键区的高
static const int iVscrollBarWidth   = 20;                   //滚动条的宽

#define     IDC_VSCROLLBAR          2//滚动条ID
#define     FIRSTMAXLEN             40//第一行文字最大长度
#define     SECONDMAXLEN            40//第二行文字最大长度
typedef struct tagMBList
{
    BOOL                bSelected;                   //是否选中
    int                 iItemData;                   //需要保存的附加数据
    int                 iCallId;                     //每个电话的ID
    char                cFirstStr[FIRSTMAXLEN + 1];  //第一行文字
    char                cSecondStr[SECONDMAXLEN + 1];//第二行文字
    int                 iPicState;//图标的状态,TRUE:正在激活,FALSE:正在保持
    struct tagMBList *  pNext;
};
typedef struct tagMBList MBLIST;

typedef enum
{
    MBLIST_UP       = 1, //滚动条向上滚动
    MBLIST_DOWN     = -1,//滚动条向下滚动
    MBLIST_MIDDLE   = 0  //滚动条不变
}MBLIST_DIRECT;//滚动条方向标志

static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
//显示列表中的数据
static  void    DisplayMBList(HDC hdc);
//获得当前鼠标点击的位置是可见项的第几个列表项
static  int     GetPtPosition(const POINT * pPoint);
//设置滚动条的位置
static  void    SetVScrollBar(void);
//初始化控件所需的数据
static  BOOL    InitMBList(void);
//获得需要刷新的区域
static  BOOL    GetRefreshRectByPoint(RECT * pRefreshRect,int iRefreshIndex);
//考查滚动条的滑块在某个方向上是否已经滚到头
static  BOOL    GetIfVScrollBarEnd(MBLIST_DIRECT MB_Direct);
//点击滚动条按钮的时候,设置滚动条的位置(滚动一行)
static  void    SetVScrollBarLine(MBLIST_DIRECT MB_Direct);
//点击滚动条翻页按钮的时候,设置滚动条的位置(滚动一页)
static  void    SetVScrollBarPage(MBLIST_DIRECT MB_Direct);
//设置滚动条拖动时候的的位置
static  int     SetVScrollBarThumbPosition(void);
//刷新需要刷新的区域
static  void    RefreshRectBySelectChange(int iOldSelectItem,int iCurSelectItem,int iOldTopIndex);
//全屏刷新
static  void    RefreshAllScreen(void);

static MBLIST     * pMBList;            //节点链表

static BOOL         bShowBmp = TRUE;    //是否显示通话状态的图标
static HBITMAP      hBmpActive;         //正在激活的图标
static HBITMAP      hBmpHold;           //正在保持的图标
static HBITMAP      hBmpAM;             //激活静音
static HBITMAP      hBmpHM;             //保持静音
static SIZE         SizeActive;         //正在激活的图标的尺寸
static SIZE         SizeHold;           //正在保持的图标的尺寸
static SIZE         SizeAM;             //激活静音的图片尺寸
static SIZE         SizeHM;             //保持静音的图片尺寸

static int          iMBListX;           //窗口横坐标
static int          iMBListY;           //窗口纵坐标
static int          iMBListWidth;       //窗口宽
static int          iMBListHeight;      //窗口高
static int          iTextX;             //文字横坐标

static int          iItemDataHeight;    //窗口中一个项目的高
static int          iItemCount;         //窗口中显示项目的最大个数

static int        * pNodeY;             //保存各个节点的纵坐标
static int          iTopIndex;          //最上方的节点是第几个数据

static HWND         hWndParent;         //父窗口句柄
static HINSTANCE    hInstance;
static HWND         hWndMBList;         //当前窗口句柄
static int          iMBListID;          //当前窗口ID

static DWORD        dwFlag;             //当前控件风格
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
    {   //如果参数中包括滚动条的话,需要除去这个属性,再主动创建滚动条,
        //否则的话窗口也会创建一个滚动条,会导致冲突
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
    int         iSelectItem;//获得当前选中的项目
    int         iMBListCount;//列表项目的项数
    int         iOldTopIndex;//iTopIndex备份值
    SCROLLINFO  VSBInfo;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        //iMBListCount = *((int*)((CREATESTRUCT*)lParam)->lpCreateParams);
        if (!InitMBList())//初始化控件所需的数据
            return FALSE;
        //**************初始化一般数据****************************
        iTopIndex = 0;//最上方的节点是第几个数据
        pMBList   = NULL;
        //***************创建滚动条*******************************************
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

    case WM_DESTROY ://通知程序管理器,此程序退出;
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
            if (dwFlag & MBLIST_WS_DISABLE)//禁止选择
                break;
            if (!GetIfVScrollBarEnd(MBLIST_UP))//滚动条还没有到达最上方
            {
                iTopIndex--;
                SetVScrollBarLine(MBLIST_UP);
                RefreshAllScreen();
            }
            SetFocus(hWnd);
            break;
        case SB_LINEDOWN:
            if (!GetIfVScrollBarEnd(MBLIST_DOWN))//滚动条还没有到达最下方
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
        if (dwFlag & MBLIST_WS_DISABLE)//禁止选择
            break;
        point.x = LOWORD (lParam);
        point.y = HIWORD (lParam);

        iSelectItem = GetPtPosition(&point);
        if (-1 != iSelectItem)
        {
            if (iSelectItem == MBList_GetCurSel())//双击的消息发回父窗口
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
        //PostMessage(hWndParent,WM_TODESKTOP,0,0);//将按键发回父窗口
        return TRUE;

    case WM_KEYUP:
        switch (LOWORD(wParam))
        {
        case VK_0:  //按了数字键0
        case VK_1:  //按了数字键1
        case VK_2:  //按了数字键2
        case VK_3:  //按了数字键3
        case VK_4:  //按了数字键4
        case VK_5:  //按了数字键5
        case VK_6:  //按了数字键6
        case VK_7:  //按了数字键7
        case VK_8:  //按了数字键8
        case VK_9:  //按了数字键9
        case VK_F1: //拨号键
        case VK_F2: //挂断键
        case KEYASTERICK: //*
        case KEYWELL: //#
        case VK_F5:
        case VK_F10://右softkey
        case VK_RETURN:
        case VK_LEFT:
        case VK_RIGHT:
            PostMessage(hWndParent,WM_KEYUP,LOWORD(wParam),0);//将以上按键发回父窗口
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_EARPHONE_KEY://耳机上的按键
            SendMessage(hWndParent,WM_KEYDOWN,VK_F2,0);//挂断电话
            break;

        case VK_UP:
            if (dwFlag & MBLIST_WS_DISABLE)//禁止选择
                break;
            iMBListCount = MBList_GetCount();
            iSelectItem  = MBList_GetCurSel();//返回选中的节点的序号
            iOldTopIndex = iTopIndex;

            if (-1 == iSelectItem)//没有选择节点
            {
                iTopIndex = 0;
                MBList_SetCurSel(0);//选中第一个节点
                InvalidateRect(hWndMBList,NULL,TRUE);
                SetVScrollBar();
                return TRUE;
            }
            if (iSelectItem == 0)//当前选中了第一项
            {
                if (iMBListCount > iItemCount)
                    iTopIndex = iMBListCount - iItemCount;

                MBList_SetCurSel(iMBListCount - 1);//选中最后一项
                RefreshRectBySelectChange(iSelectItem,iMBListCount - 1,iOldTopIndex);
            }
            else
            {
                if (iTopIndex > iSelectItem)//选择的节点在可视节点之上
                    iTopIndex = iSelectItem - 1;
                else if (iSelectItem - iTopIndex + 1 > iItemCount)//选择的节点在可视节点之下
                    iTopIndex = iSelectItem - iItemCount;
                else if (iTopIndex == iSelectItem)//选中的节点就是显示在最上方的节点
                    iTopIndex--;

                MBList_SetCurSel(iSelectItem - 1);//选中上一项
                RefreshRectBySelectChange(iSelectItem,iSelectItem - 1,iOldTopIndex);
            }
            SetVScrollBar();
            PostMessage(hWndParent,WM_COMMAND,MAKEWPARAM(iMBListID,MBLIST_SELCHANGE),0);

            PostMessage(hWndParent,WM_KEYDOWN,VK_UP,0);
            break;

        case VK_DOWN:
            if (dwFlag & MBLIST_WS_DISABLE)//禁止选择
                break;
            iMBListCount = MBList_GetCount();
            iSelectItem  = MBList_GetCurSel();//返回选中的节点的序号
            iOldTopIndex = iTopIndex;
            if (-1 == iSelectItem)//没有选择节点
            {
                iTopIndex = 0;
                MBList_SetCurSel(0);//选中第一个节点
                InvalidateRect(hWndMBList,NULL,TRUE);
                SetVScrollBar();
                return TRUE;
            }
            if (iSelectItem == iMBListCount - 1)//选中了最后一项
            {
                iTopIndex = 0;
                MBList_SetCurSel(0);//选中第一项
                RefreshRectBySelectChange(iSelectItem,0,iOldTopIndex);
            }
            else
            {
                if (iTopIndex > iSelectItem)//选择的节点在可视节点之上
                    iTopIndex = iSelectItem + 1;
                else if (iSelectItem - iTopIndex + 1 > iItemCount)//选择的节点在可视节点之下
                    iTopIndex = iSelectItem + 2 - iItemCount;
                else if (iSelectItem - iTopIndex + 1 == iItemCount)//窗口可视范围的下方还有节点数据
                    iTopIndex++;//选中下一项

                MBList_SetCurSel(iSelectItem + 1);//选中下一项
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
        case VK_0:  //按了数字键0
        case VK_1:  //按了数字键1
        case VK_2:  //按了数字键2
        case VK_3:  //按了数字键3
        case VK_4:  //按了数字键4
        case VK_5:  //按了数字键5
        case VK_6:  //按了数字键6
        case VK_7:  //按了数字键7
        case VK_8:  //按了数字键8
        case VK_9:  //按了数字键9
        case VK_F1: //拨号键
        case VK_F2: //挂断键
        case KEYASTERICK: //*
        case KEYWELL: //#
        case VK_F5:
        case VK_F10://右softkey
        case VK_RETURN:
        case VK_LEFT:
        case VK_RIGHT:
        case VK_FLIP_OFF://合盖挂断
            PostMessage(hWndParent,WM_KEYDOWN,LOWORD(wParam),0);//将以上按键发回父窗口
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
* Purpose      初始化控件所需的数据
* Params       
* Return     
* Remarks      
**********************************************************************/
static  BOOL    InitMBList(void)
{
    static BOOL bInit = FALSE;//标识图标初始化是否完成
    BOOL        bTran;
    COLORREF    Color;
    HDC         hdc;
    int         iFontHeight,i;

    if (bInit)//图标初始化已经完成
        return TRUE;
    //初始化图标
    memset(&SizeActive,0x00,sizeof(SIZE));
    memset(&SizeHold  ,0x00,sizeof(SIZE));
    memset(&SizeAM    ,0x00,sizeof(SIZE));
    memset(&SizeHM    ,0x00,sizeof(SIZE));

    GetImageDimensionFromFile(pMBActive,&SizeActive);//得到图象的大小
    GetImageDimensionFromFile(pMBHold  ,&SizeHold  );//得到图象的大小
    GetImageDimensionFromFile(pMBAM    ,&SizeAM    );//得到图象的大小
    GetImageDimensionFromFile(pMBHM    ,&SizeHM    );//得到图象的大小

    hdc        = CreateMemoryDC(SizeActive.cx,SizeActive.cy);
    hBmpActive = CreateBitmapFromImageFile(hdc,pMBActive,&Color,&bTran);
    hBmpHold   = CreateBitmapFromImageFile(hdc,pMBHold  ,&Color,&bTran);
    hBmpAM     = CreateBitmapFromImageFile(hdc,pMBAM    ,&Color,&bTran);
    hBmpHM     = CreateBitmapFromImageFile(hdc,pMBHM    ,&Color,&bTran);
    DeleteObject(hdc);
    //初始化窗口中一个项目的高
    iFontHeight = GetCharactorHeight(NULL);
    if (iFontHeight * 2 > SizeActive.cy)//获得窗口中一个项目的高
        iItemDataHeight = iFontHeight * 2;
    else
        iItemDataHeight = SizeActive.cy;

    iItemCount = iMBListHeight / iItemDataHeight;//窗口中显示项目的最大个数
    iTextX     = iMBListX + SizeActive.cx;//文字横坐标
    //////////////计算各个节点的纵坐标////////////////////////////////////////////////////////////
    pNodeY = (int *)malloc(iItemCount * sizeof(int));
    if (pNodeY == NULL)
        return FALSE;
    for (i = 0;i < iItemCount;i++)
    {
        pNodeY[i] = iItemDataHeight * i;//保存各个节点的纵坐标
    }
    bInit = TRUE;
    return TRUE;
}
/********************************************************************
* Function     GetPtPosition
* Purpose      获得当前鼠标点击的位置是可见项的第几个列表项
* Params       当前鼠标点击的位置
* Return     当前鼠标点击的位置是可见项的第几个列表项
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
* Purpose      显示列表中的数据
* Params       iTopIndex:最上方的节点是第几个数据,即从第几个节点开始显示
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
        if (i - iTopIndex > iItemCount)//该节点在显示区域之外
            return;

        if (i >= iTopIndex)
        {
            if (bShowBmp)//显示通话状态的图标
            {
                SetRect(&rect,iTextX,pNodeY[i - iTopIndex],
                    iMBListWidth,pNodeY[i - iTopIndex] + iItemDataHeight);
                switch (p->iPicState)
                {
                case MBLIST_IPICNONE://没有
                    SetRect(&rect,0,pNodeY[i - iTopIndex],//显示文字的区域靠左
                        iMBListWidth,pNodeY[i - iTopIndex] + iItemDataHeight);
                    break;
                case MBLIST_IPICSTATE_A://激活
                    StretchBlt( hdc,
                        iMBListX, pNodeY[i - iTopIndex], SizeActive.cx,SizeActive.cy,//目的矩形
                        (HDC)hBmpActive,0,0,SizeActive.cx,SizeActive.cy,//图象原矩形
                        SRCCOPY);//把位图从源矩形拷贝到目的矩形，扩展或压缩位图以使其适合目的矩形的范围
                    break;
                case MBLIST_IPICSTATE_H://保持
                    StretchBlt( hdc,
                        iMBListX, pNodeY[i - iTopIndex], SizeHold.cx,SizeHold.cy,//目的矩形
                        (HDC)hBmpHold,0,0,SizeHold.cx,SizeHold.cy,//图象原矩形
                        SRCCOPY);//把位图从源矩形拷贝到目的矩形，扩展或压缩位图以使其适合目的矩形的范围
                    break;
                case MBLIST_IPICSTATE_AM://激活静音
                    StretchBlt( hdc,
                        iMBListX, pNodeY[i - iTopIndex], SizeAM.cx,SizeAM.cy,//目的矩形
                        (HDC)hBmpAM,0,0,SizeAM.cx,SizeAM.cy,//图象原矩形
                        SRCCOPY);//把位图从源矩形拷贝到目的矩形，扩展或压缩位图以使其适合目的矩形的范围
                    break;
                case MBLIST_IPICSTATE_HM://保持静音
                    StretchBlt( hdc,
                        iMBListX, pNodeY[i - iTopIndex], SizeHM.cx,SizeHM.cy,//目的矩形
                        (HDC)hBmpHM,0,0,SizeHM.cx,SizeHM.cy,//图象原矩形
                        SRCCOPY);//把位图从源矩形拷贝到目的矩形，扩展或压缩位图以使其适合目的矩形的范围
                    break;
                }
            }
            else//不显示通话状态的图标
            {
                SetRect(&rect,0,pNodeY[i - iTopIndex],//显示文字的区域靠左
                    iMBListWidth,pNodeY[i - iTopIndex] + iItemDataHeight);
            }
            if (dwFlag & WS_VSCROLL)//有滚动条
            {
                rect.right -= iVscrollBarWidth;//修改刷新区域
            }

            memset(cDisplay,0x00,sizeof(cDisplay));
            if ((strlen(p->cFirstStr) != 0) && (strlen(p->cSecondStr) != 0))
                sprintf(cDisplay,pStr2Line,p->cFirstStr,p->cSecondStr);
            else if ((strlen(p->cFirstStr) != 0) && (strlen(p->cSecondStr) == 0))
                sprintf(cDisplay,pStr1Line,p->cFirstStr);
            else if ((strlen(p->cFirstStr) == 0) && (strlen(p->cSecondStr) != 0))
                sprintf(cDisplay,pStr1Line,p->cSecondStr);

            DrawText(hdc,cDisplay,strlen(cDisplay),&rect,DT_LEFT|DT_VCENTER);//添加文字

            if (p->bSelected)//反转颜色
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
* Purpose      考查滚动条的滑块在某个方向上是否已经滚到头
* Params       MB_Direct:滚动方向
* Return       TRUE:获得滚动条在某个方向上已经滚到头,否则返回FALSE
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
    if (iCount > iItemCount)//项目个数大于屏幕最大显示个数,需要显示滚动条
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
* Purpose      设置滚动条拖动时候的的位置
* Params       
* Return       向上或者向下移动的长度    
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
    if (iCount > iItemCount)//项目个数大于屏幕最大显示个数,需要显示滚动条
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
* Purpose      点击滚动条翻页按钮的时候,设置滚动条的位置
* Params       MB_Direct,1:向上滚动,-1:向下滚动,0位置不变
* Return       无    
* Remarks      用于滚动条一页的时候,设置滚动条
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
    if (iCount > iItemCount)//项目个数大于屏幕最大显示个数,需要显示滚动条
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
* Purpose      点击滚动条按钮的时候,设置滚动条的位置
* Params       MB_Direct,1:向上滚动,-1:向下滚动,0位置不变
* Return       无    
* Remarks      用于滚动一行的时候,设置滚动条
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
    if (iCount > iItemCount)//项目个数大于屏幕最大显示个数,需要显示滚动条
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
* Purpose      设置滚动条的位置
* Params       
* Return       无    
* Remarks      用于选择上下键的时候,设置滚动条
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
    if (iCount > iItemCount)//项目个数大于屏幕最大显示个数,需要显示滚动条
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
* Purpose      全屏刷新
* Params       
* Return       
* Remarks      
**********************************************************************/
static  void    RefreshAllScreen(void)
{
    HWND hVScrollBar;
    RECT rectCur;

    hVScrollBar = GetDlgItem(hWndMBList,IDC_VSCROLLBAR);
    if (NULL != hVScrollBar)//有滚动条
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
* Purpose      刷新需要刷新的区域
* Params       iOldSelectItem:原来选择的节点,iCurSelectItem:当前选择的节点
                iOldTopIndex:原iTopIndex
* Return       
* Remarks      
**********************************************************************/
static  void    RefreshRectBySelectChange(int iOldSelectItem,int iCurSelectItem,int iOldTopIndex)
{
    HWND hVScrollBar;
    RECT rectOld,rectCur;
    if (iOldTopIndex != iTopIndex)//需要刷新全屏
    {
        hVScrollBar = GetDlgItem(hWndMBList,IDC_VSCROLLBAR);
        if (NULL != hVScrollBar)//有滚动条
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
* Purpose      获得需要刷新的区域
* Params       pRefreshRect:返回刷新区域,iRefreshIndex:需要刷新的节点
* Return       TRUE:需要刷新,FALSE:不需要刷新
* Remarks      
**********************************************************************/
static  BOOL    GetRefreshRectByPoint(RECT * pRefreshRect,int iRefreshIndex)
{
    HWND hVScrollBar;

    if ((iRefreshIndex - iTopIndex >= iItemCount) ||//该节点在显示区域之下,不需要刷新
        (iRefreshIndex < iTopIndex))//该节点在显示区域之上,不需要刷新
        return FALSE;

    if (pNodeY == NULL)
        return FALSE;

    if (pRefreshRect != NULL)
    {
        hVScrollBar = GetDlgItem(hWndMBList,IDC_VSCROLLBAR);
        if (NULL != hVScrollBar)//有滚动条
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
* Purpose      添加数据
* Params       iPicState:图片标志,iData:附加数据
* Return     节点在列表中的位置    
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

    if (MBList_GetCount() == 0)//加入头节点
    {
        iCount   = 0;
        pMBList  = p;
        p->pNext = NULL;
    }
    else
    {
        iCount = 1;
        pnext  = pMBList;
        while (pnext->pNext != NULL)//遍历到链表结尾
        {
            pnext = pnext->pNext;
            iCount++;
        }
        pnext->pNext = p;//将新建的节点添加到链表结尾
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
* Purpose      删除第iIndex个节点
* Params       
* Return     剩余结点个数      
* Remarks      
**********************************************************************/
int     MBList_DeleteItem(int iIndex)
{
    MBLIST * pCur,* pCurNext;
    int i,iCount,iSelIndex;

    iCount = MBList_GetCount();

    if ((0 == iCount) || (iIndex < 0) || (iIndex > iCount - 1))
        return (iCount);

    iSelIndex = MBList_GetCurSel();//返回选中的节点的序号

    if (0 == iIndex)//删除头节点
    {
        pCur    = pMBList;
        pMBList = pMBList->pNext;
        free(pCur);
    }
    else if (iIndex == iCount - 1)//删除尾节点
    {
        pCur     = pMBList;
        pCurNext = pCur->pNext;
        while (pCurNext->pNext != NULL)//遍历链表到尾节点,使pCurNext指向尾节点
        {
            pCur     = pCur->pNext;
            pCurNext = pCur->pNext;
        }
        free(pCurNext);
        pCur->pNext = NULL;
    }
    else//删除中间节点
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
    //该节点的下一个节点应该向上移
    if ((iCount - 1 - iTopIndex < iItemCount) && (iTopIndex > 0))
        iTopIndex--;

    if (iSelIndex == iIndex)//选择的节点就是当前被删除的节点
    {
        if (iSelIndex == iCount - 1)//原来选择的节点是最后一个节点,选中最后一个节点
            MBList_SetCurSel(iCount - 2);
        else//原来选择的节点不是最后一个节点,选中原来选择节点的下一个节点
            MBList_SetCurSel(iSelIndex + 1);
    }

    if (IsWindow(hWndMBList))
        InvalidateRect(hWndMBList,NULL,TRUE);

    SetVScrollBar();
    return (iCount - 1);
}
/********************************************************************
* Function     MBList_Destroy
* Purpose      销毁窗口
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
* Purpose      iCallId是否在链表里的节点中存在
* Params       iIndex:第iIndex个节点
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
* Purpose      获得链表节点数
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
* Purpose      返回选中的节点的序号
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
* Purpose      获得第iIndex个节点的包含的数据
* Params       iIndex:第iIndex个节点
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
* Purpose      获得第iIndex个节点的第一行第二行数据
* Params       iIndex:第iIndex个节点
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
* Purpose      刷新当前列表
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
* Purpose      删除所有节点
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
* Purpose      设置选中第index个节点
* Params       index:选中第index个节点,index<0 的时候选中第一个节点
                index大于节点个数的时候选中最后一个节点
* Return           
* Remarks      
**********************************************************************/
void    MBList_SetCurSel(int index)
{
    MBLIST * p = pMBList;
    int i = 0,iListCount;

    iListCount = MBList_GetCount();//获得节点总数

    if (index < 0)//选中第一个节点
        index = 0;
    else if (index >= iListCount)//选中最后一个节点
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
* Purpose      设置第iIndex个节点的数据
* Params       iPicState/iData/iCallId:-1表示不改变当前节点状态
                bRefresh:是否立即刷新
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
* Purpose      设置控件风格
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
* Purpose      出现或者隐藏控件
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
* Purpose      是否显示通话状态的图标
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
