/**************************************************************************\
 *
 *                      PDAOS Window System
 *
 * Copyright (c) 1998 by CASS Corporation & Software Engineering Center
 *                       All Rights Reserved
 *
 *
 * Filename     display.h
 *
 * Purpose      Header file for display device driver.
 *  
 * Author       WeiZhong
 *
 *-------------------------------------------------------------------------
 *
 * $Archive:: /hopenpda/include/display.h                       $
 * $Revision:: 7  $     $Date:: 99-10-10 8:46a                  $
 *
 * $History:: display.h                                         $
 * 
 * *****************  Version 7  *****************
 * User: Zwei         Date: 99-10-10   Time: 8:46a
 * Updated in $/hopenpda/include
 * 1. Adds DIStretchBlt function in DISPLAYDRVINFO.
 * 2. Adds some new futures for new display driver(RGB16_24).
 * 
 * *****************  Version 6  *****************
 * User: Zwei         Date: 99-06-24   Time: 11:46a
 * Updated in $/hopenpda/include
 * 
 * *****************  Version 5  *****************
 * User: Zwei         Date: 99-06-15   Time: 8:02p
 * Updated in $/hopenpda/include
 * 
 * *****************  Version 3  *****************
 * User: Zwei         Date: 99-04-08   Time: 4:13p
 * Updated in $/hopenpda/include
 * 
 * *****************  Version 3  *****************
 * User: Zwei         Date: 98-10-30   Time: 11:34a
 * Updated in $/PDAOS/OS/include
 * Adds two new brush hatched style HS_GRAY and HS_LIGHTGRAY.
 * 
 * *****************  Version 2  *****************
 * User: Zwei         Date: 98-10-30   Time: 9:37a
 * Updated in $/PDAOS/OS/include
 * Includes types.h unconditionally, then needn't include osapi.h
 * in display.c.
 * 
 * *****************  Version 1  *****************
 * User: Zwei         Date: 98-10-30   Time: 8:57a
 * Created in $/PDAOS/OS/include
 * 
\**************************************************************************/

#ifndef __DISPLAY_H
#define __DISPLAY_H

#define _DISPLAY_VER 0x0200

#include "hpdef.h"

#ifndef _TYPEDEF_
#define _TYPEDEF_

#ifdef GUI_CALL
typedef void*   PDEV;  /* Device pointer type for window system */
#endif

#endif

typedef int32   RGB;

/* Define struct for display device infomation */
typedef struct DisplayDevInfo 
{
    int32   attrib;
    int32   width;
    int32   height;
    int8    planes;
    int8    bits_pixel;
    int8    mode;
    int8    unused;
    int32   phys_width;
    int32   phys_height;
    int16   pitch;
    int16   vesa_mode;
    int16   tv_mode;
    int16   frames;
} DISPLAYDEVINFO, *PDISPLAYDEVINFO;

/* Define struct for bitmap */
typedef struct DIBitmap 
{
    int16   type;               /* Bitmap type                  */
    int16   width;              /* Bitmap width                 */
    int16   height;             /* Bitmap height                */
    int16   palette_entries;    /* number of palette entries    */
    int32   bmp_size;           /* Size of bmpBuffer            */
    void*   pBuffer;            /* Buffer point for bitmap data */
    void*   pPalette;           /* Palette pointer              */
} DIBITMAP, *PDIBITMAP;

/* Define constant for BITMAP type field */
#define BMP_MONO    0   /* ��ɫλͼ����ɫ�������Ϊ2��ÿ����1λ     */
#define BMP_GRAY4   1   /* ��ɫλͼ����ɫ�������Ϊ4��ÿ����2λ     */
#define BMP_4       1   /* ��ɫλͼ����ɫ�������Ϊ4��ÿ����2λ     */
#define BMP_16      2   /* 16ɫλͼ����ɫ�������Ϊ16��ÿ����4λ    */
#define BMP_256     3   /* 256ɫλͼ����ɫ�������Ϊ256��ÿ����8λ  */
#define BMP_RGB16   4   /* 16λ�߲�ɫλͼ���޵�ɫ�壬ÿ����16λ��   */
#define BMP_RGB24   5   /* 24λ���ɫλͼ���޵�ɫ�壬ÿ����24λ��   */
#define BMP_RGB32   6   /* 32λ���ɫλͼ���޵�ɫ�壬ÿ����32λ��   */
#define BMP_RGB12   7   /* 16λ�߲�ɫλͼ���޵�ɫ�壬ÿ����16λ��   */

#ifndef TEXTBMP_STRUCT
#define TEXTBMP_STRUCT

/* Define struct for text bitmap */
typedef struct TextBmp
{
    int16   width;          /* Mono bitmap width       */
    int16   height;         /* Mono bitmap height      */
    int32   data[1];        /* Mono bitmap data buffer */
} TEXTBMP, *PTEXTBMP;

/* Defines text bitmap type */
#define TB_NORMAL   0
#define TB_TVFONT   1

typedef struct
{
    int16 flag;             /* must be 0                */
    int16 type;             /* text bitmap type         */
    int16 width;            /* text bitmap width        */
    int16 height;           /* text bitmap height       */
    int32 data[1];          /* text bitmap data buffer  */
} TEXTBMPEX, *PTEXTBMPEX;

#endif  /* TEXTBMP_STRUCT */

#ifdef GUI_CALL

typedef void* PMEMDEV;
typedef void* PDDBITMAP;
typedef void* PPHYSBMP;
typedef void* PPHYSPEN;
typedef void* PPHYSBRUSH;
typedef void* PPHYSPALETTE;
typedef void* PPHYSCURSOR;
typedef void* PSCREENSAVE;

#else

typedef struct PhysDev*     PDEV;
typedef struct MemDev*      PMEMDEV;
typedef struct MemDev*      PDDBITMAP;
typedef struct MemDev*      PPHYSBMP;
typedef struct PhysPen*     PPHYSPEN;
typedef struct PhysBrush*   PPHYSBRUSH;
typedef struct PhysPalette* PPHYSPALETTE;
typedef struct PhysCursor*  PPHYSCURSOR;
typedef struct ScreenSave*  PSCREENSAVE;

/* Define struct for point */
typedef struct tagPOINT
{
    int32 x;
    int32 y;
} POINT, *PPOINT;

/* Define struct for rect */
typedef struct tagRECT
{
    int32 x1;
    int32 y1;
    int32 x2;
    int32 y2;
}RECT, *PRECT;

#endif  /* GUI_CALL */

/* Define struct for region */
typedef struct Region
{
    int32 count;                /* rectangle count */
    RECT rect[1];               /* rectangle array */
} REGION, *PREGION;

/* Define struct for log pen */
typedef struct DrvLogPen
{
    int8    style;              /* Pen style        */
    int8    width;              /* Pen width        */
    int8    end_style;          /* Pen end style    */
    int8    frame_mode;         /* Pen frame mode   */
    RGB     color;              /* Pen color        */
}DRVLOGPEN, *PDRVLOGPEN;

/* Define constant for pen style */
#define PS_SOLID        0       /* ʵ�߱�           */
#define PS_DASH         1       /* ���߱�           */
#define PS_DOT          2       /* ���߱�           */
#define PS_DASHDOT      3       /* �㻮�߱�         */
#define PS_DASHDOTDOT   4       /* ˫�㻮�߱�       */
#define PS_NULL         5       /* �ձ�             */

/* Define constant for pen end style */
#define PES_DEFAULT     0       /* ϵͳĬ����       */
#define PES_RECTANGLE   1       /* ����             */
#define PES_TRIANGLE    2       /* ������           */
#define PES_CIRCLE      3       /* Բ��             */

/* Define constant for pen frame mode */
#define PFM_INTERNAL    0       /* �߱����ڷ��ͼ�ε��ڲ�           */
#define PFM_CENTER      1       /* �Է��ͼ�εı߽���Ϊ�ʼ�������   */

typedef struct DrvLogBrush
{
    int16   width;              /* Brush width      */
    int16   height;             /* Brush height     */
    int16   style;              /* Brush style      */       
    int16   hatch_style;        /* Hatch style      */
    RGB     color;              /* Brush color      */
    void*   pPattern;           /* Bitmap pattern   */
} DRVLOGBRUSH, *PDRVLOGBRUSH;


/* Define constant for brush style */
#define BS_HOLLOW       0       /* ��ˢ��               */
#define BS_NULL         0       /* ��ˢ��               */
#define BS_SOLID        1       /* ʵ��ˢ��             */
#define BS_BITMAP       2       /* �豸�޹�λͼˢ��     */
#define BS_HATCHED      3       /* ��Ӱˢ��             */
#define BS_PATTERN      4       /* �ڴ浥ɫ��λͼˢ��   */

/* Define constant for hatch style */
#define HS_BDIAGONAL    0       /* 45��������Ӱ�ߣ������ң�*/
#define HS_CROSS        1       /* ˮƽ����ֱ��ʮ����Ӱ��    */
#define HS_DIAGCROSS    2       /* 45��ʮ����Ӱ�ߣ�          */
#define HS_FDIAGONAL    3       /* 45��������Ӱ�ߣ������ң�*/
#define HS_HORIZONTAL   4       /* ˮƽ��Ӱ��                */
#define HS_VERTICAL     5       /* ��ֱ��Ӱ��                */
#define HS_SIEVE        6       /* �ڰ׵�����ɸ��          */
#define HS_GRAY         6       /* ��ɫЧ����ɸ��            */
#define HS_LIGHTGRAY    7       /* ǳ��ɫЧ����ɸ��          */
#define HS_WALL         8       /* ש��ǽͼ                  */

/* Defines struct for log cursor */
typedef struct DrvLogCursor 
{
    int16       type;           /* cursor type              */
    int16       rop;            /* cursor rop               */
    int32       width;          /* cursor width             */
    int32       height;         /* cursor height            */
    int32       hot_x;          /* cursor hot point x       */
    int32       hot_y;          /* cursor hot point y       */
    int32       fgcolor;
    int32       bkcolor;
    const void* pANDPlane;      /* curosr mask pattern      */
    const void* pXORPlane;      /* curosr image data        */
} DRVLOGCURSOR, *PDRVLOGCURSOR;

/* Define constant for type filed */
#define CUR_BW          0       /* ��ɫ���                 */
#define CUR_COLOR       1       /* ��ɫ���                 */
#define CUR_ANIMATE     2       /* ��ɫ�������             */

#define CUR_SHOW        1910        //�������ʾ 0x0776
#define CUR_HIDE        1911        //��������� 0x0777

typedef struct DrvLogPalette
{
    int16   entries;
    int16   unused;
    RGB     color_map[1];
} DRVLOGPALETTE, *PDRVLOGPALETTE;

/* Define struct for graph drawing parameter */
typedef struct GraphParam
{
    int16       flags;          /* flags                        */
    int16       arcDirection;    /* arc direction, wwh 02/08/02  */
    int16       bk_mode;        /* background mode              */
    int32       bk_color;       /* Realized background color    */
    int32       text_color;     /* Realized text color         */    
    PPHYSPEN    pPen;           /* Pen to use                   */
    PPHYSBRUSH  pBrush;         /* Brush to use                 */
    int16       brush_org_x;    /* brush offset X               */
    int16       brush_org_y;    /* brush offset Y               */
    int32       rop;            /* ROP2 or ROP3                 */
    RECT        clip_rect;      /* Clip rectangle               */
    PREGION     pRegion;        /* Complicate clip region       */
}GRAPHPARAM, *PGRAPHPARAM;

/* Define constant for flags field in GraphPara struct */
#define GPF_PEN         0x0001  /* ʹ�ñ�pPen                   */
#define GPF_BRUSH       0x0002  /* ʹ��ˢ��pBrush               */
#define GPF_ROP2        0x0004  /* ʹ��ROP2                     */
#define GPF_ROP3        0x0008  /* ʹ��ROP3                     */
#define GPF_CLIP_RECT   0x0010  /* ʹ�ü򵥲ü�����clip_rect    */
#define GPF_CLIP_REGION 0x0020  /* ʹ�ø��Ӳü�����pClipRegion  */

// Define constant for bk_mode field in GraphParam struct
#define BM_TRANSPARENT     1    /* ͸����ʽ                     */
#define BM_OPAQUE          2    /* ��͸����ʽ                   */
#define BM_NEWTRANSPARENT  3    /* ����λͼ��͸����ͼ           */

/* Define constant for rop field in GraphParam struct           */
#define ROP_SET_DST_0       1   /* ȫ0                                  */
#define ROP_NSRC_AND_NDST   2   /* Դ��������ķ���Ŀ�Ĳ�������ķǵ��� */
#define ROP_NSRC_AND_DST    3   /* Դ��������ķ���Ŀ�Ĳ����������     */
#define ROP_NSRC            4   /* Դ��������ķ�                       */
#define ROP_SRC_AND_NDST    5   /* Դ����������Ŀ�Ĳ�������ķǵ���     */
#define ROP_NDST            6   /* Ŀ�Ĳ�������ķ�                     */
#define ROP_SRC_XOR_DST     7   /* Դ����������Ŀ�Ĳ�����������       */
#define ROP_NSRC_OR_NDST    8   /* Դ��������ķ���Ŀ�Ĳ�������ķǵĻ� */
#define ROP_SRC_AND_DST     9   /* Դ����������Ŀ�Ĳ����������         */
#define ROP_NSRC_XOR_DST    10  /* Դ��������ķ���Ŀ�Ĳ�����������   */
#define ROP_DST             11   /* Ŀ�Ĳ�������                         */
#define ROP_NSRC_OR_DST     12  /* Դ��������ķ���Ŀ�Ĳ�������Ļ�     */
#define ROP_SRC             13  /* Դ��������                           */
#define ROP_SRC_OR_NDST     14  /* Դ����������Ŀ�Ĳ�������ķǵĻ�     */
#define ROP_SRC_OR_DST      15  /* Դ����������Ŀ�Ĳ�������Ļ�         */
#define ROP_SET_DST_1       16  /* ȫ1                                  */

/* Define struct for rectangle drawing */
typedef struct RectData
{
    int32 x1;
    int32 y1;
    int32 x2;
    int32 y2;
} RECTDATA, *PRECTDATA;

/* Define struct for line drawing */
typedef struct LineData
{
    int32 x1;       /* start x  */
    int32 y1;       /* start y  */
    int32 x2;       /* end x    */
    int32 y2;       /* end y    */
} LINEDATA, *PLINEDATA;

/* Define struct for point drawing */
typedef struct PointData
{
    int32 x;
    int32 y;
    uint32 color;
} POINTDATA, *PPOINTDATA;

/* Define struct for circle drawing */
typedef struct CircleData
{
    int32 x;        /* x coordinates of center  */
    int32 y;        /* y coordinates of center  */
    int32 r;        /* length of radius         */
} CIRCLEDATA, *PCIRCLEDATA;

/* Define struct for polygon drawing */
typedef struct PolygonData
{
    int16 count;    /* number of the points     */
    int16 mode;     /* close mode               */
    POINT points[1];/* points array             */
} POLYLINEDATA, *PPOLYLINEDATA, POLYGONDATA, *PPOLYGONDATA;

/* Define struct for Ellipse drawing */
typedef struct EllipseData
{
    int32 x;        /* x coordinates of center          */
    int32 y;        /* y coordinates of center          */
    int32 d1;       /* length of x axes                 */
    int32 d2;       /* length of y axes                 */
    int32 rotate;   /* rotate degree. Unsupported now   */
} ELLIPSEDATA, *PELLIPSEDATA;

/* Define struct for arc drawing */
typedef struct ArcData
{
    int32 x;
    int32 y;
    int32 r;
    int32 mode;
    int32 start;
    int32 end;
} ARCDATA, *PARCDATA;

/* Define struct for ellipse arc drawing */
/*
typedef struct EllipseArcData
{
    int32 x;
    int32 y;
    int32 d1;
    int32 d2;
    int32 rotate;
    int32 mode;
    int32 start;
    int32 end;
} ELLIPSEARCDATA, *PELLIPSEARCDATA;
*/
typedef struct EllipseArcData
{
    int32 x;        /* x coordinates of center          */
    int32 y;        /* y coordinates of center          */
    int32 d1;       /* length of x axes                 */
    int32 d2;       /* length of y axes                 */
    int32 mode;     /* clockwise or counter clockwise   */
    int32 rotate;   /* rotate degree                    */
    int32 start_x;  
    int32 start_y;  /* link this point to the center and intersect 
                    ** the ellipse. The result intersect point is 
                    ** the start point of the ellipse arc.
                    ** This method is as same as MS Windows.
                    */
    int32 end_x;    
    int32 end_y;    /* link this point to the center and intersect 
                    ** the ellipse. The result intersect point is 
                    ** the end point of the ellipse arc
                    ** This method is as same as MS Windows.
                    */
} ELLIPSEARCDATA, *PELLIPSEARCDATA;

/* Define constant for mode field in ArcData and EllipsArcData struct */
#define AM_ARC      0       /* ���պϣ���Ϊ��   */
#define AM_SECTOR   1       /* �պϳ�����       */
#define AM_LUNE     2       /* �պϳɹ���       */

#define AD_COUNTERCLOCKWISE 1
#define AD_CLOCKWISE        2

typedef struct tagROUNDRECTDATA
{
    int32 x1;       /* The left of boundary rect    */
    int32 y1;       /* The top of boundary rect     */
    int32 x2;       /* The right of boundary rect   */
    int32 y2;       /* The bottom of boundary rect  */
    int32 width;    /* The width of the ellipse arc */
    int32 height;   /* The height of ellipse arc    */
} ROUNDRECTDATA, *PROUNDRECT;

typedef struct PolyPoint
{
    uint32 number;
    PPOINT pPoint;
}POLYPOINTDATA, *PPOLYPOINTDATA; 

typedef struct PolyPolyLine
{
    POINT  *pPoints; // address of an array of points 
    uint32 *pPolyPoints; // address of an array of values 
    uint32 count; 
}POLYPOLYLINEDATA, *PPOLYPOLYLINEDATA; 

/* field selection bits */
#define DM_BITSPERPEL       0x00040000L
#define DM_PELSWIDTH        0x00080000L
#define DM_PELSHEIGHT       0x00100000L
#define DM_DISPLAYFLAGS     0x00200000L
#define DM_DISPLAYFREQUENCY 0x00400000L

typedef struct _dispmode 
{
    uint32  dmBitsPerPel;
    uint32  dmPelsWidth;
    uint32  dmPelsHeight;
    uint32  dmDisplayFlags;
    uint32  dmDisplayFrequency;
} DISPMODE, *PDISPMODE;

/* Flags for ChangeDisplaySettings */

/* Return values for ChangeDisplaySettings */
#define DISP_CHANGE_SUCCESSFUL       0
#define DISP_CHANGE_RESTART          1
#define DISP_CHANGE_FAILED          -1
#define DISP_CHANGE_BADMODE         -2
#define DISP_CHANGE_NOTUPDATED      -3
#define DISP_CHANGE_BADFLAGS        -4
#define DISP_CHANGE_BADPARAM        -5

/* Define struct for display driver infomation */
typedef struct DisplayDrvInfo
{
    int32 devices;
    int32 func_mask1;
    int32 func_mask2;
    int   (*OpenDevice)(PDEV pDev, int dev);
    int   (*CloseDevice)(PDEV pDev);
    int   (*SuspendDevice)(PDEV pDev);
    int   (*ResumeDevice)(PDEV pDev);
    int   (*GetDevInfo)(PDEV pDev, PDISPLAYDEVINFO pInfo);
    int   (*EnumMode)(PDEV pDev, int nModeNum, PDISPMODE pDispMode);
    int   (*ChangeMode)(PDEV pDev, PDISPMODE pDispMode, uint32 flags);
    int   (*CreateMemoryDevice)(PDEV pDev, int width, int height,
                                PMEMDEV pMemDev);
    int   (*DestroyMemoryDevice)(PDEV pMemDev);
    int   (*CreateBitmap)(PDEV pDev, int width, int height, 
                          PDDBITMAP pBitmap);
    int   (*DestroyBitmap)(PDEV pDev, PDDBITMAP pBitmap);
    int   (*SetBitmapBuffer)(PDEV pDev, PDDBITMAP pBitmap, void* pBuffer);
    int   (*RealizePen)(PDEV pDev, PDRVLOGPEN pLogPen, PPHYSPEN pPhysPen);
    int   (*UnrealizePen)(PDEV pDev, PPHYSPEN pPen);
    int   (*RealizeBrush)(PDEV pDev, PDRVLOGBRUSH pLogBrush, 
                                    PPHYSBRUSH pPhysBrush);
    int   (*UnrealizeBrush)(PDEV pDev, PPHYSBRUSH pBrush);
    int   (*RealizePalette)(PDEV pDev, PDRVLOGPALETTE pLogPalette,
                            PPHYSPALETTE pPhysPalette);
    int   (*UnrealizePalette)(PDEV pDev, PPHYSPALETTE pPalette);
    int   (*RealizeColor)(PDEV pDev, PPHYSPALETTE pPalette, int32 color);
    int   (*SelectPalette)(PDEV pDev, PPHYSPALETTE pPalette);
    int   (*RealizeCursor)(PDEV pDev, PDRVLOGCURSOR pLogCursor,
                           PPHYSCURSOR pPhysCursor);
    int   (*UnrealizeCursor)(PDEV pDev, PPHYSCURSOR pCursor);
    int   (*ShowCursor)(PDEV pDev, PPHYSCURSOR pCursor, int mode);
    int   (*SetCursorPos)(PDEV pDev, int x, int y);
    int   (*CheckCursor)(PDEV pDev);
    int   (*UpdateScreen)(PDEV pDev);
    int   (*DrawGraph)(PDEV pDev, int func, PGRAPHPARAM pGraphPara, 
                       void* pDrawData);
    int   (*BitBlt)(PDEV pDev, PRECT pDstRect, PDEV pSrcDev,
                    int x, int y, PGRAPHPARAM pGraphParam);
    int   (*PatBlt)(PDEV pDev, PRECT pRect, PGRAPHPARAM pGraphParam);
    int   (*DIBitBlt)(PDEV pDev, PRECT pDestRect, PDIBITMAP pBitmap,
                      int x, int y, PGRAPHPARAM pGraphParam);
    int   (*TextBlt)(PDEV pDev, int x, int y, PTEXTBMP pTextBmp, 
                     PGRAPHPARAM pGraphParam);
    int   (*StretchBlt)(PDEV pDev, PRECT pDstRect, PDEV pSrcDev, 
                        PRECT pSrcRect, PGRAPHPARAM pGraphParam);
    int   (*DIStretchBlt)(PDEV pDev, PRECT pDstRect, PDIBITMAP pBitMap, 
                        PRECT pSrcRect, PGRAPHPARAM pGraphParam);
    int   (*SaveScreen)(PDEV pDev, PSCREENSAVE pScreenSave, PRECT pRect);
    int   (*RestoreScreen)(PDEV pDev, PSCREENSAVE pScreenSave, PRECT pRect);
    int   (*ScrollScreen)(PDEV pDev, PRECT pRect, PRECT pClipRect, 
                          int x, int y);
} DISPLAYDRVINFO, *PDISPLAYDRVINFO;

/* Define constant for function mask in DisplayDrvInfo struct */
#define FM_SUSPENDDEVICE    0x00000001  /* ��SuspendDevice����  */
#define FM_RESUMEDEVICE     0x00000002  /* ��ResumeDevice����   */
#define FM_CHECKCURSOR      0x00000004  /* ��CheckCursor����    */
#define FM_UPDATESCREEN     0x00000008  /* ��UpdateScreen ����  */
#define FM_STRETCHBLT       0x00000010  /* ��StretchBlt����     */
#define FM_SCROLLSCREEN     0x00000020  /* ��ScrollScreen����   */

/* Define constant for cursor type */
#define CUS_BW          0       /* ��ɫ���                 */
#define CUS_COLOR       1       /* ��ɫ���(�豸���)       */
#define CUS_ANIMATE     2       /* ��ɫ�������             */

/* Define constant for func parameter of DrawGraph function */
#define DG_POINT            0           /* ����                 */
#define DG_LINE             1           /* ����                 */
#define DG_RECT             2           /* ������               */
#define DG_CIRCLE           3           /* ��Բ                 */
#define DG_POLYGON          4           /* �������             */
#define DG_ELLIPSE          5           /* ����Բ               */
#define DG_ARC              6           /* ����                 */
#define DG_ELLIPSEARC       7           /* ����Բ��             */

#define DG_ROUNDRECT        8           /* ��Բ�Ǿ���           */
#define DG_POLYPOINT        9           /* �������             */
#define DG_POLYPOLYLINE     10          /* �������             */

/* The entry function for display driver */
PDISPLAYDRVINFO EnableDisplayDev(PDISPLAYDRVINFO pDisplayDrvInfo);

/* the display mode define */
#define MAX_SCREEN 3

#define	DDM_MONO		0x00000001
#define DDM_GRAY4       0x00000002
#define DDM_GRAY16      0x00000004
#define DDM_COLOR16     0x00000104
#define DDM_PAL256      0x00010008

#define DDM_RGB16_565   0x00020010
#define DDM_RGB16_555   0x0002010F
#define	DDM_RGB12_4440	0x0002020C

#define	DDM_RGB24_RGB   0x00030016
#define	DDM_RGB24_BGR   0x00030116
#define	DDM_RGB18_RGB   0x00030212
#define	DDM_RGB18_BGR   0x00030312
#define	DDM_YUV24		0x00030416

#define	DDM_RGB32_0RGB  0x00040020
#define	DDM_RGB32_RGB0  0x00040120
#define	DDM_RGB32_0BGR  0x00040220
#define	DDM_RGB32_BGR0  0x00040320
#define	DDM_RGB18_0RGB  0x00040412
#define	DDM_RGB18_0BGR  0x00040512
#define	DDM_RGB18_RGB0  0x00040612
#define	DDM_RGB18_BGR0  0x00040712
#define	DDM_YUV32		0x00040820

#define	DDM_VGA			DDM_COLOR16
#define	DDM_VGA256		DDM_PAL256
#define	DDM_VGA32K		DDM_RGB16_555
#define	DDM_VGA64K		DDM_RGB16_565

#define	DDM_VERTICAL    0x01000000
#define DDM_HORIZONTAL  0x02000000
#define	DDM_OTHER	    0xFFFFFFFF  /* the screen is not supported */

#define GETPIXBYTES(mode)   (((mode) & 0x000F0000) >> 16)
#define GETPIXBITS(mode)   ((((mode) & 0x000F0000) >> 16) * 8)
#define GETPIXCOLORBITS(mode)   (((mode) & 0xFF))

// Define constant for attrib field
#define DDA_PALETTE         0x00000001  /* �е�ɫ��     */
#define DDA_DIRECT_DRAW     0x00000002  /* ֧��ֱ��д�� */
#define DDA_ENERGY_SAVE     0x00000004  /* ���нڵ繦�� */
#define DDA_ANIMATE_CURSOR  0x00000008  /* ֧�ֶ������ */
#define DDA_HARDWARE_CURSOR 0x00000010  /* ֧��Ӳ����� */
#define DDA_ROP3_SUPPORT    0x00000020  /* ֧��ROP3���� */
#define DDA_REGION_CLIP     0x00000040  /* ֧�ָ��Ӳü� */
#define DDA_VESA            0x00040000  /* �÷�ʽ��һ�� VESA��ʽ */
#define DDA_TV              0x00080000  /* �÷�ʽ��һ�� TV ��ʽ */
#define DDA_LCD             0x00100000  /* �÷�ʽ��һ�� LCD ��ʽ  */
#define DDA_VGAOUT          0x00200000  /* �÷�ʽ������ VGA ��� */
#define DDA_AVOUT           0x00400000  /* �÷�ʽ������ AV  ��� */
#define DDA_SVIDEO          0x00800000  /* �÷�ʽ������ S ������� */
#define DDA_RGB             0x01000000  /* �÷�ʽ������RGB������� */

typedef struct LDD_ModeInfo
{
    DWORD dwModeFlag;   /* the mode flag defined upon */
    BYTE pix_bits;      /* the pixel bits of this mode */
    BYTE bRotate;       /* rotate screen flag */
    BYTE bReserved1;    /* reserved */
    BYTE bReserved2;    /* reserved */
}LDD_MODEINFO, *PLDD_MODEINFO;

/*********************************************************************\
* Function	   LDD_GetDisplayDriverInfo
* Purpose      Get the logical display driver info.
* Params	   
        nDev:   (in)the number of screens
        pDisplayDrvInfo: (out)the display driver info should be returned.
* Return	 	   
        Success return TRUE and failed return FALSE;
* Remarks	   
**********************************************************************/
#ifdef _MSC_VER
_declspec(dllexport) 
#endif
BOOL LDD_GetDisplayDriverInfo(int nDev, PLDD_MODEINFO pDisplayModeInfo);

typedef struct OS_DisplayMode
{
    int    dsp_mode;            //Display mode.
    int    dsp_width;           //srceen width in pix.
    int    dsp_height;          //srceen height in pix.
    int    dsp_planes;          //number of planes.
    int    dsp_scanline_bytes;  //bytes per scan line.
    void*  dsp_screen_buffer;   //Address of emulate screen buffer.
    int    dsp_updatescreen;    //forced update screen flag.
}OS_DISPLAYMODE, *POS_DISPLAYMODE;

#define SCREEN_MAIN     0
#define SCREEN_FIRST    SCREEN_MAIN
#define SCREEN_SECOND   1
#define SCREEN_THIRD    2

/*********************************************************************\
* Function	   PDD_EnumDisplayMode
* Purpose      Enum the display modes of the given device.
* Params	   
        dev:    (in)the display device index
        index:  (in)the display mode index
        pDisplayMode: (out)the display mode returned
* Return	 	   
        Success return TRUE and failed return FALSE;
* Remarks	  
        Realized by physical display driver. 
**********************************************************************/
BOOL PDD_EnumDisplayMode(int dev, int index, POS_DISPLAYMODE pDisplayMode);

/*********************************************************************\
* Function	   PDD_SetDisplayMode
* Purpose      Set the display mode of the given device.
* Params	   
        dev:    (in)the display device index
        index:  (in)the display mode index
* Return	 	   
        Success return TRUE and failed return FALSE;
* Remarks	   
        Realized by physical display driver. 
**********************************************************************/
BOOL PDD_SetDisplayMode(int dev, int index);

/*********************************************************************\
* Function	   PDD_GetDisplayMode
* Purpose      Get the current mode of the given device.
* Params	   
        dev:    (in)the display device index
        pDisplayMode: (out)the display mode returned
* Return	 	   
* Remarks	   
        Realized by physical display driver. 
**********************************************************************/
BOOL PDD_GetDisplayMode(int dev, POS_DISPLAYMODE pDisplayMode);

/*********************************************************************\
* Function	   PDD_UpdateScreen
* Purpose      Update the given screen.
* Params	   
        dev:    (in)the display device index
        left,top,right,bottom: (in)the update rect
* Return	 	   
* Remarks	   
        Realized by physical display driver. 
**********************************************************************/
void PDD_UpdateScreen(int dev, int left, int top, int right, int bottom);

#endif // __DISPLAY_H
