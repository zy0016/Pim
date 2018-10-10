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
#include "stdio.h"
#include "stdlib.h"
#include "window.h"
#include "string.h"
#include "pubapp.h"
#include "pmi.h"
#include "MBPublic.h"
#include "../mbrecord/RcCommon.h"
#include "str_mb_plx.h"
#include "hpime.h"
#include "fapi.h"
#include "hpimage.h"
#include "winpda.h"
#include "hp_pdaex.h"

static const char   cCh_Well            = '#';
static const char   cCh_Plus            = '+';
static const char   cCh_Asterisk        = '*';
static const char   cCh_0               = '\0';
static const char   cCh_Num0            = '0';
static const char   cCh_Num9            = '9';
static const char   cCh_Point           = '.';
static const char   cCh_n               = '\n';
static const char   cCh_r               = '\r';
static const char   cCh_Space           = ' ';
static const char   cCh_Colon           = ':';
static const char   cCh_Slash1          = '\\';
static const char   cCh_Slash2          = '/';
static const char   cCh_ExtP            = EXTENSIONCHAR;
static const char   cCh_ExtW            = EXTENSIONCHARW;
static const int    pChinese            = 0x80;//中文ASCII码标志
static const char * pRomPath1           = "/rom";
static const char * pRomPath2           = "rom:";
static const char * pInputStr           = "英数";
static const char * pChStr              = "十";//中文字符串实例
static const char * pChEng              = "H";//英文字符串实例
static const char * pChNewLine          = "\n";//需要添加的换行符
static const char * pChPoint            = ".";
static const char * pChEnd              = "\0";
static const char * pString1            = "%ld";
static const char * pChNumer0           = "0";
static const char * pMinus              = "-";
static const char * pMenuClassName      = "MENULIST";
static const char * pListClassName      = "LISTBOX";
static const char * pEmergentNumber[]   = //特殊号码集合(没有SIM卡的时候也可以拨打的号码)
{
    "112",//紧急电话号码
    ""
};

static  int     StrAutoNewLine_chn(HDC hdc,const char * cSourstr,char * cDesstr,int iRectWidth);//返回行数
static  int     StrAutoNewLine_eng(HDC hdc,const char * cSourstr,char * cDesstr,int iRectWidth);//返回行数
static  int     GetCharactorWidth(HDC hdc,const char * ch); //获得单个字符或者单个汉字的宽度
static  int     GetStringLanguage(const char * pString);    //获得当前字符串的语言

//有关呼叫转移部分的函数
static BOOL bFWD_UNCDTIONAL;//无条件转移
static BOOL bFWD_BUSY;      //遇忙转移
static BOOL bFWD_NOREPLY;   //无应答转移
static BOOL bFWD_NOREACH;   //不在服务区或关机转移

static  void    SetCallForwardIcon(BOOL bForward)//设置呼叫转移的图标
{
    DlmNotify(PS_SETCALL,(LPARAM)(bForward ? ICON_SET : ICON_CANCEL));
}
BOOL    GetForward_Uncdtional(void)//考查是否存在无条件转移
{
    return (bFWD_UNCDTIONAL);
}
BOOL    GetForward_Condition(void)//考查是否存在条件转移
{
    return (bFWD_BUSY || bFWD_NOREPLY || bFWD_NOREACH);
}

void    SetForward_All(BOOL bForward)//设置是否存在所有转移
{
    bFWD_UNCDTIONAL = bForward;//无条件转移
    bFWD_BUSY       = bForward;//遇忙转移
    bFWD_NOREPLY    = bForward;//无应答转移
    bFWD_NOREACH    = bForward;//不在服务区或关机转移

    SetCallForwardIcon(bForward);
}
void    SetForward_Uncdtional(BOOL bForward)//设置是否存在无条件转移
{
    //无条件转移打开的时候,其它转移都会自动关闭,
    //当无条件转移关闭的时候,在先前设置的某个转移会自动打开
    //所以当无条件转移关闭的时候,需要重新查询各个呼叫转移的状态
    //但是这个查询操作需要由其它应用调用,不能在本函数中调用,否则的话会导致死循环.
    bFWD_UNCDTIONAL  = bForward;//无条件转移
    if (bForward)
    {
        bFWD_BUSY    = FALSE;//遇忙转移自动关闭
        bFWD_NOREPLY = FALSE;//无应答转移自动关闭
        bFWD_NOREACH = FALSE;//不在服务区或关机转移自动关闭
        SetCallForwardIcon(bForward);
    }
}
void    SetForward_Condition(BOOL bForward)//设置是否存在所有条件转移
{
    bFWD_BUSY    = bForward;//遇忙转移
    bFWD_NOREPLY = bForward;//无应答转移
    bFWD_NOREACH = bForward;//不在服务区或关机转移
    SetCallForwardIcon(bForward);
}
void    SetForward_Busy(BOOL bForward)//设置是否存在遇忙转移
{
    bFWD_BUSY = bForward;   //遇忙转移
    SetCallForwardIcon(bForward);
}
void    SetForward_NoReply(BOOL bForward)//设置是否存在无应答转移
{
    bFWD_NOREPLY = bForward;//无应答转移
    SetCallForwardIcon(bForward);
}
void    SetForward_NoReach(BOOL bForward)//设置是否存在不在服务区或关机转移
{
    bFWD_NOREACH = bForward;//不在服务区或关机转移
    SetCallForwardIcon(bForward);
}
//////////////////////////////////////////////////////////////////////////
/********************************************************************
* Function   FileIfExist  
* Purpose    考查当前文件是否存在
* Params     pFileName:文件名包括路径
* Return     存在ture,不存在false
* Remarks      
**********************************************************************/
BOOL    FileIfExist(const char * pFileName)
{
    HANDLE          hf;
    char          * p = NULL;
    unsigned int    iLen;

    iLen = strlen(pFileName);
    if (iLen == 0)
        return FALSE;

    if (strncmp(pFileName,pRomPath1,strlen(pRomPath1)) == 0)//"/rom"
    {
        p = (char *)malloc(iLen + 1);
        if (p == NULL)
        {
            hf = CreateFile(pFileName,ACCESS_READ,0);
        }
        else
        {
            memset(p,0x00,iLen + 1);
            strcpy(p,pRomPath2);//"rom:"
            strcat(p,pFileName + strlen(pRomPath1));
            hf = CreateFile(p,ACCESS_READ,0);
            free(p);
        }
    }
    else
    {
        hf = CreateFile(pFileName,ACCESS_READ,0);
    }
    if(INVALID_HANDLE_VALUE == hf)//未找到文件
    {
        CloseFile(hf);
        return FALSE;
    }
    CloseFile(hf);
    return TRUE;
}
/********************************************************************
* Function   SetRightButtonText  
* Purpose    如果窗口中有文本框控件,设置右键按钮上的文字
* Params     
* Return     
* Remarks      
**********************************************************************/
void    SetRightButtonText(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    //SendMessage(hWnd,PWM_SETBUTTONTEXT,0,lParam);
}
/********************************************************************
* Function   SetRightButtonText  
* Purpose    如果窗口中有文本框控件,选择右键按钮之后的操作
* Params     
* Return     
* Remarks      
**********************************************************************/
void    DealWithRightButton(HWND hWnd)
{
    /*int     nEnd = 0,nPos = 0;
    HWND    hCurWin = 0;

    hCurWin = GetFocus();
    SendMessage(hCurWin, EM_GETSEL, (WPARAM)&nPos, (LPARAM)&nEnd);
    if (!nEnd)
        DestroyWindow(hWnd);
    else
        SendMessage(hCurWin, WM_KEYDOWN, VK_BACK, 0);*/
    DestroyWindow(hWnd);
}
/********************************************************************
* Function   GetInputSoftKeyboardPlace  
* Purpose    获得屏幕半屏输入法的位置
* Params     
* Return     
* Remarks      
**********************************************************************/
int     GetInputSoftKeyboardPlace(void)
{
    return 105;
}
/********************************************************************
* Function   GetScreenUsableWH1  
* Purpose    获得屏幕中可以利用的宽度或者高度
* Params     nIndex:SM_CXSCREEN:获得屏幕中可以利用的宽度,
                SM_CYSCREEN:获得屏幕中可以利用的高度
* Return     屏幕中可以利用的宽度或者高度
* Remarks      
**********************************************************************/
int     GetScreenUsableWH1(int nIndex)
{
    switch (nIndex)
    {
    case SM_CXSCREEN:
        return GetSystemMetrics(SM_CXSCREEN);
    case SM_CYSCREEN:
        return 160;
    }
    return (0);
}
/********************************************************************
* Function   GetScreenUsableWH2  
* Purpose    获得屏幕中可以利用的宽度和高度
* Params     pwidth:获得屏幕中可以利用的宽度,
             pheight:获得屏幕中可以利用的高度
* Return     无
* Remarks      
**********************************************************************/
void    GetScreenUsableWH2(int * pwidth,int * pheight)
{
    if (pwidth != NULL)
        * pwidth = GetScreenUsableWH1(SM_CXSCREEN);

    if (pheight != NULL)
        * pheight = GetScreenUsableWH1(SM_CYSCREEN);
}
/********************************************************************
* Function   GetEditControlHeight  
* Purpose    获得EDIT控件合适的高度
* Params     
* Return     
* Remarks    
**********************************************************************/
int     GetEditControlHeight(void)
{
    return (GetCharactorHeight(NULL) + 5);
}
/********************************************************************
* Function   GetButtonControlHeight  
* Purpose    获得Button控件合适的高度
* Params     
* Return     
* Remarks    
**********************************************************************/
int     GetButtonControlHeight(void)
{
    return (GetCharactorHeight(NULL) + 14);
}
/********************************************************************
* Function   GetRadioButtonControlWidth  
* Purpose    获得单选按钮控件需要的宽度
* Params     pString:单选按钮上的文字
* Return     
* Remarks    
**********************************************************************/
int     GetRadioButtonControlWidth(const char * pString)
{
    return (GetStringWidth(NULL,pString) + 15);
}
/********************************************************************
* Function   TrimString  
* Purpose    删除字符串开头结尾的空格字符
* Params     
* Return     
* Remarks    
**********************************************************************/
char *  TrimString(char * pStr)
{
    unsigned char * pBegin = NULL,* pSetp = NULL,* pEnd = NULL;
    unsigned int iLen,i = 0;

    if (pStr == NULL)
        return NULL;

    iLen = strlen(pStr);
    if (0 == iLen)
        return pStr;

    pBegin = pStr;
    if (* pBegin == cCh_Space)//开头有空格
    {
        pSetp = pStr;
        while ((* pSetp == cCh_Space) && (i < iLen))//向后扫描,查找非空格字符
        {
            pSetp++;
            i++;
        }
        if (i == iLen)//字符串全是空格
        {
            * pBegin = cCh_0;
            return pStr;
        }
        //找到不是空格的字符
        while (* pSetp != cCh_0)
        {
            /** pBegin = * pSetp;
            pBegin++;
            * pSetp = cCh_Space;
            pSetp++;*/
            * pBegin++ = * pSetp;
            * pSetp++ = cCh_Space;
        }
    }

    iLen = strlen(pStr);
    //考查结尾是否有空格
    pEnd = pStr + iLen - 1;
    while ((* pEnd == cCh_Space) && (iLen-- > 0))
        * pEnd-- = cCh_0;

    return pStr;
}
/********************************************************************
* Function   GetFactFileNameFromPath  
* Purpose    从带路径的文件名当中,获得纯文件名
* Params     pFilePathName:带路径的文件名,pFileName:返回纯文件名,
             iBuflen:缓冲区长度   bExName:是否包括扩展名
* Return     
* Remarks    
**********************************************************************/
void    GetFactFileNameFromPath(const char * pFilePathName,char * pFileName,
                                unsigned int iBuflen,BOOL bExName)
{
    unsigned char * pEnd,* pPoint;
    unsigned int iLen,iFileNameLen;

    if (NULL == pFilePathName)
        return;

    iLen = strlen(pFilePathName);
    if (iLen == 0)
    {
        strcpy(pFileName,"");
        return;
    }
    memset(pFileName,0x00,iBuflen);
    pEnd = (char*)pFilePathName + iLen - 1;//指向文件结尾

    while ((* pEnd != cCh_Slash1) && (* pEnd != cCh_Slash2) && 
        (* pEnd != cCh_Colon) && (iLen > 0))//向前扫描,寻找目录分隔符
    {
        pEnd--;
        iLen--;
    }
    if ((* pEnd == cCh_Slash1) || (* pEnd == cCh_Slash2) || (* pEnd == cCh_Colon))//找到目录分隔符
    {
        if (bExName)//返回的文件名包括扩展名
        {
            iFileNameLen = strlen(pEnd) - 1;
            strncpy(pFileName,pEnd + 1,(int)iBuflen > iFileNameLen ? iFileNameLen + 1 : iBuflen - 1);
        }
        else//返回的文件名不包括扩展名
        {
            pEnd++;
            pPoint = strstr(pEnd,pChPoint);
            if (pPoint == NULL)
            {
                iFileNameLen = strlen(pEnd);
                strncpy(pFileName,pEnd,(int)iBuflen > iFileNameLen ? iFileNameLen + 1 : iBuflen - 1);
            }
            else
            {
                iFileNameLen = abs(pEnd - pPoint) - 1;
                strncpy(pFileName,pEnd,(int)iBuflen > iFileNameLen ? iFileNameLen + 1 : iBuflen - 1);
            }
        }
    }
    else//没找到目录分隔符
    {
        if (bExName)//返回的文件名包括扩展名
        {
            iFileNameLen = strlen(pFilePathName);
            strncpy(pFileName,pFilePathName,(int)iBuflen > iFileNameLen ? iFileNameLen + 1 : iBuflen - 1);
        }
        else//返回的文件名不包括扩展名
        {
            pPoint = strstr(pEnd,pChPoint);
            if (pPoint == NULL)
            {
                iFileNameLen = strlen(pEnd);
                strncpy(pFileName,pEnd,(int)iBuflen > iFileNameLen ? iFileNameLen + 1 : iBuflen - 1);
            }
            else
            {
                iFileNameLen = abs(pEnd - pPoint) - 1;
                strncpy(pFileName,pEnd,(int)iBuflen > iFileNameLen ? iFileNameLen + 1 : iBuflen - 1);
            }
        }
    }
}
/********************************************************************
* Function   StrAutoNewLine  
* Purpose    自动换行所需函数
* Params     hdc:用于打印输出,cSourstr:源字符串,cDesstr:目的字符串,
             pRect:输出区域,uFormat:输出参数(同DrawText函数中的uFormat参数)
* Return     生成的新字符串的长度
* Remarks    当cDesstr==NULL时,认为直接在函数中输出字符串,即直接调用DrawText函数
            cDesstr!=NULL时,返回生成的字符串,不执行DrawText函数
**********************************************************************/
int     StrAutoNewLine(HDC hdc,const char * cSourstr,char * cDesstr,const RECT * pRect,UINT uFormat)
{
    char  * pDesstr = NULL;//保存生成的字符串数组
    int     iRowNum = 0,ilen,iSourLen;
    SIZE    size;
    RECT    sRect;

    if ((NULL == cSourstr) || (NULL == pRect))
        return 0;

    iSourLen = strlen(cSourstr);
    if (iSourLen == 0)
    {
        if (cDesstr != NULL)
            strcpy(cDesstr,"");
        return 0;
    }
    if ((pRect->left < 0) || (pRect->right < 0) || (pRect->left >= pRect->right))
        return 0;

    ilen = iSourLen * (strlen(pChNewLine) + 1) + 1;//极端的情况,一个字符(加上换行符)占一行
    if ((pDesstr = (char *)malloc(ilen)) == NULL)
    {
        if ((cDesstr == NULL) && (NULL != hdc))
            DrawText(hdc,cSourstr,iSourLen,(RECT *)pRect,uFormat);
        else
            strcpy(cDesstr,cSourstr);

        return 0;
    }
    memset(pDesstr,0x00,ilen);

    memcpy(&sRect,pRect,sizeof(RECT));

    switch (GetStringLanguage(cSourstr))
    {
    case 0://英文
        iRowNum = StrAutoNewLine_eng(hdc,cSourstr,pDesstr,sRect.right - sRect.left);
        GetTextExtent(hdc,pChEng,strlen(pChEng),&size);//获得英文字符的高度
        break;
    case 1://中文
        iRowNum = StrAutoNewLine_chn(hdc,cSourstr,pDesstr,sRect.right - sRect.left);
        GetTextExtent(hdc,pChStr,strlen(pChStr),&size);//获得中文字符的高度
        break;
    default:
        free(pDesstr);
        return 0;
    }
    ilen = strlen(pDesstr);//生成的字符串的长度

    if (cDesstr != NULL)
    {
        strcpy(cDesstr,pDesstr);
    }
    else
    {
        if (hdc != NULL)
        {
            if (iRowNum != 0)//返回的行数不为零,修改矩形区域的高度用于输出
                sRect.bottom = sRect.top + iRowNum * size.cy + 5;

            DrawText(hdc,pDesstr,strlen(pDesstr),&sRect,uFormat);
        }
    }
    free(pDesstr);

    return (ilen);
}
/********************************************************************
* Function   StrAutoNewLine_chn  
* Purpose    中文字符的自动换行函数
* Params     hdc:用于获得字符高度,cSourstr:源字符串,cDesstr:目的字符串,iRectWidth:输出区域的宽度
* Return     返回行数
* Remarks    
**********************************************************************/
static  int     StrAutoNewLine_chn(HDC hdc,const char * cSourstr,char * cDesstr,int iRectWidth)
{
    int     iCharactorWidth;//一个英文字符或者一个汉字的宽度
    int     iCopyNum,iSourlen,iRowNum,iCopyWidth;
    char  * p = NULL;

    p           = (char *)cSourstr;
    iSourlen    = strlen(cSourstr);
    iCopyNum    = 0;//已经拷贝的字符个数
    iRowNum     = 1;//行数
    iCopyWidth  = 0;//一行中已经拷贝的字符一共有多长

    while(iCopyNum < iSourlen)
    {
        if ((* p == NULL) || (p == NULL))
            break;
        /*if ((* p == 0xa) || (* p == 0xd))//不添加换行符
        {
            p++;
            iCopyNum++;
            continue;
        }*/
        iCharactorWidth = GetCharactorWidth(hdc,p);//获得当前字符(或者汉字)的长度
        if (iRectWidth - iCopyWidth > iCharactorWidth)//剩余长度大于一个英文字符或者一个汉字的宽度
        {
            if ((unsigned char)(* p) > pChinese)//中文
            {
                strncat(cDesstr,p,2);
                iCopyNum += 2;
                p += 2;
            }
            else
            {
                strncat(cDesstr,p,1);
                iCopyNum++;
                p++;
            }
            iCopyWidth += iCharactorWidth;
        }
        else if (iRectWidth - iCopyWidth == iCharactorWidth)//剩余长度等于一个英文字符或者一个汉字的宽度
        {
            if ((unsigned char)(* p) > pChinese)//中文
            {
                strncat(cDesstr,p,2);
                iCopyNum += 2;
                p += 2;
            }
            else
            {
                strncat(cDesstr,p,1);
                iCopyNum++;
                p++;
            }
            strcat(cDesstr,pChNewLine);
            iCopyWidth = 0;
            iRowNum++;//行数加一
        }
        else if (iRectWidth - iCopyWidth < iCharactorWidth)
        {//剩余长度小于一个英文字符或者一个汉字的宽度,需要添加换行符,行数加一
            if (iCopyNum == 0)
            {
                //一个字符也没有拷贝就添加换行符,则这个中文文字的长度已经超过
                //了矩形区域的宽度
                return (0);
            }
            strcat(cDesstr,pChNewLine);
            iCopyWidth = 0;
            iRowNum++;
        }
    }
    strcat(cDesstr,pChEnd);//"\0"

    return (iRowNum);
}
/********************************************************************
* Function   StrAutoNewLine_eng  
* Purpose    英文字符的自动换行函数
* Params     hdc:用于获得字符高度,cSourstr:源字符串,cDesstr:目的字符串,iRectWidth:输出区域的宽度
* Return     返回行数
* Remarks    
**********************************************************************/
static  int     StrAutoNewLine_eng(HDC hdc,const char * cSourstr,char * cDesstr,int iRectWidth)
{
    int     iCopyNum,iSourlen,iRowNum,iCopyWidth,iWordWidth,iWordLength,iCopyNumExt;
    char  * p = NULL,* pNextSpace = NULL,* pScan = NULL;

    p           = (char *)cSourstr;
    iSourlen    = strlen(cSourstr);
    iCopyNum    = 0;//已经拷贝的字符个数
    iRowNum     = 1;//行数
    iCopyWidth  = 0;//一行中已经拷贝的字符一共有多长

    while (iCopyNum < iSourlen)
    {
        if ((* p == NULL) || (p == NULL))
            break;
        /*if ((* p == 0xa) || (* p == 0xd))//不添加换行符
        {
            p++;
            iCopyNum++;
            continue;
        }*/

        pScan       = p;
        pNextSpace  = strchr(p,cCh_Space);//非英文字母' '

        iWordWidth  = 0;

        if (pNextSpace != NULL)
        {
            iWordLength = abs(pScan - pNextSpace);//获得这个英文单词的字母个数

            while (pScan != pNextSpace)
            {
                iWordWidth += GetCharactorWidth(hdc,pScan);//获得这个英文单词的宽度
                pScan++;
            }
        }
        else//到了字符串结尾
        {
            iWordLength = iSourlen - iCopyNum;

            while (* pScan != NULL)
            {
                iWordWidth += GetCharactorWidth(hdc,pScan);//获得这个英文单词的宽度
                pScan++;
            }
        }

        if (iRectWidth - iCopyWidth > iWordWidth)//剩余长度>一个英文单词的宽度,直接拷贝
        {
            strncat(cDesstr,p,iWordLength + 1);//需要将空格(或者非英文字母)也拷贝进去
            iCopyNum   += (iWordLength + 1);//将已经拷贝的字符个数进行累加,包括空格(或者非英文字母)
            iCopyWidth += (iWordWidth + GetCharactorWidth(hdc,pNextSpace));//将宽度累加

            p += (iWordLength + 1);
        }
        else if(iRectWidth - iCopyWidth == iWordWidth)//剩余长度等于一个英文单词的宽度,直接拷贝,然后添加换行符
        {
            strncat(cDesstr,p,iWordLength + 1);//需要将空格(或者非英文字母)也拷贝进去
            strcat(cDesstr,pChNewLine);

            iCopyNum += (iWordLength + 1);//将已经拷贝的字符个数进行累加,包括空格(或者非英文字母)
            iRowNum++;
            iCopyWidth = 0;

            p += (iWordLength + 1);
        }
        else//剩余长度小于一个英文单词的宽度,直接添加换行符
        {
            if ((iCopyNum == 0) || //一个字符也没有拷贝
                (iRectWidth - iCopyWidth == iRectWidth))//剩余宽度就是矩形的最大宽度
            {
                iCopyNumExt = 0;//在一个单词的长度超过矩形最大宽度的时候,拷贝单词的个数的计数器
                //这个单词的长度已经超过了矩形区域的宽度
                while (iCopyNumExt <= iWordLength)//需要为这个单词强制添加换行符号//iCopyNum
                {
                    if ((* p == NULL) || (p == NULL))
                        break;
                    if ((* p == 0xa) || (* p == 0xd))//不添加换行符
                    {
                        p++;
                        iCopyNum++;
                        iCopyNumExt++;
                        continue;
                    }
                    iWordWidth = GetCharactorWidth(hdc,p);
                    if (iRectWidth - iCopyWidth > iWordWidth)//剩余长度大于一个英文字符的宽度
                    {
                        strncat(cDesstr,p,1);
                        iCopyNum++;
                        iCopyNumExt++;
                        p++;
                        iCopyWidth += iWordWidth;
                    }
                    else if (iRectWidth - iCopyWidth == iWordWidth)//剩余长度等于一个英文字符
                    {
                        strncat(cDesstr,p,1);
                        iCopyNum++;
                        iCopyNumExt++;
                        p++;
                        strcat(cDesstr,pChNewLine);
                        iCopyWidth = 0;
                        iRowNum++;//行数加一
                    }
                    else if (iRectWidth - iCopyWidth < iWordWidth)
                    {//剩余长度小于一个英文字符的宽度,需要添加换行符,行数加一
                        if (iCopyNum == 0)//矩形区域的宽度小于一个字符的宽度,直接返回0
                        {
                            return 0;
                        }
                        strcat(cDesstr,pChNewLine);
                        iCopyWidth = 0;
                        iRowNum++;
                        iCopyNumExt++;
                    }
                }
            }
            else
            {
                strcat(cDesstr,pChNewLine);
                iRowNum++;
                iCopyWidth = 0;
            }
        }
    }
    strcat(cDesstr,pChEnd);//"\0"

    return (iRowNum);
}
/********************************************************************
* Function   GetCharactorWidth  
* Purpose    获得单个字符或者单个汉字的宽度
* Params     
* Return     
* Remarks      
**********************************************************************/
static  int     GetCharactorWidth(HDC hdc,const char * ch)
{
    SIZE size;

    if ((ch == NULL) || (* ch == NULL))
        return 0;

    if ((unsigned char)(* ch) > pChinese)//当前字符是汉字
        GetTextExtent(hdc,ch,2,&size);
    else
        GetTextExtent(hdc,ch,1,&size);

    return (size.cx);
}
/********************************************************************
* Function   GetStringWidth  
* Purpose    获得一个字符串的宽度
* Params     
* Return     
* Remarks      
**********************************************************************/
int     GetStringWidth(HDC hdc,const char * pString)
{
    SIZE size = {0,0};
    int iLength;
    if (pString == NULL)
        return -1;

    iLength = strlen(pString);
    if (0 == iLength)
        return 0;
    GetTextExtentPoint(hdc,pString,iLength,&size);
    return size.cx;
}
/********************************************************************
* Function   GetCharactorHeight  
* Purpose    获得单个字符的高度
* Params     
* Return     
* Remarks      
**********************************************************************/
int     GetCharactorHeight(HWND hWnd)
{
    HDC hdc;
    SIZE size = {0,0};

    if (hWnd != NULL)
    {
        hdc = GetDC(hWnd);
        GetTextExtent(hdc,pChStr,2,&size);
        ReleaseDC(hWnd,hdc);
    }
    else
    {
        GetTextExtent(NULL,pChStr,2,&size);
    }
    return (size.cy);
}
/********************************************************************
* Function   GetStringLanguage  
* Purpose    获得当前字符串的语言
* Params     
* Return     
* Remarks      
**********************************************************************/
static  int     GetStringLanguage(const char * pString)
{
    char * pch = NULL;

    if (NULL == pString)
        return -1;

    pch = (char *)pString;
    while (* pch)
    {
        if ((unsigned char)(* pch) > pChinese)//当前字符是汉字
            return 1;//中文

        pch++;
    }
    return 0;//英文
}
///////////////////////////////以上部分为自动换行所需函数--end
/********************************************************************
* Function   LoadMenuList  
* Purpose    创建HMENULIST
* Params     bLostkeyNotify:
            如果希望点击左softkey的时候,menulist收不到消息,那么请取FALSE
            如果希望点击左softkey的时候后,只有左softkey收到消息,
            而menulis收不到消息,那么请取TRUE
* Return     
* Remarks      
**********************************************************************/
HMENULIST   LoadMenuList(MENULISTTEMPLATE * MenuListElement,BOOL bLostkeyNotify)
{
    int isw,ish;

    if (NULL == MenuListElement)
        return NULL;
    isw = GetScreenUsableWH1(SM_CXSCREEN);
    ish = GetScreenUsableWH1(SM_CYSCREEN);
    MenuListElement->cmlit          = 0;
    MenuListElement->lpszBgPic      = NULL;
    MenuListElement->nItemWidth     = isw;
    MenuListElement->nItemHeight    = 22;
    MenuListElement->nTopLeftX      = 0;
    MenuListElement->nTopLeftY      = 0;
    MenuListElement->nLeftGap       = 5;

    MenuListElement->nUArrowX       = isw - 20;
    MenuListElement->nUArrowY       = 5;
    MenuListElement->nUArrowHeight  = 10;
    MenuListElement->nUArrowWidth   = 10;

    MenuListElement->lpszUAPic      = NULL;
    MenuListElement->nDArrowX       = isw - 20;
    MenuListElement->nDArrowY       = ish - 10;
    MenuListElement->nDArrowHeight  = 10;
    MenuListElement->nDArrowWidth   = 10;
    MenuListElement->lpszDAPic      = NULL;
    MenuListElement->pmlItems       = 0;

    MenuListElement->dwStyle        = MLI_LEFT | MLI_NOTIFY;

    return (LoadMenuListIndirect((const MENULISTTEMPLATE *)MenuListElement));
}
/********************************************************************
* Function   MenuList_AddString  
* Purpose    重新加载数据到Menulist中
* Params     **P:需要添加的文字,* id:各个文字对应的项目的id,** pic:各个项目对应的图标文件名
* Return     
* Remarks      
**********************************************************************/
void    MenuList_AddString(HWND hMenuList,char ** p,int * id,char ** pic)
{
    MENULISTITEMTEMPLATE tmpValue;
    char cClassName[15] = "";

    if (!GetClassName(hMenuList,cClassName,sizeof(cClassName)))
        return;

    if (strcmp(cClassName,pMenuClassName) == 0)
    {
        while (** p)
        {
            memset(&tmpValue,0x00,sizeof(MENULISTITEMTEMPLATE));
            if (pic != NULL)
                tmpValue.lpszIconName = * pic++;
            else
                tmpValue.lpszIconName = NULL;

            tmpValue.dwFlags        = MLI_LEFT | MLI_NOTIFY;
            tmpValue.nPicAreaWidth  = 20;
            tmpValue.wID            = * id++;
            tmpValue.lpszItemName   = * p++;
            SendMessage(hMenuList, ML_ADDITEM, 0, (LPARAM)&tmpValue);
        }
        SendMessage(hMenuList,ML_SETCURSEL,0,0);
    }
    else if (strcmp(cClassName,pListClassName) == 0)
    {
        while (** p)
            SendMessage(hMenuList,LB_ADDSTRING,0,(LPARAM)* p++);

        SendMessage(hMenuList,LB_SETCURSEL,0,0);
    }
}
/********************************************************************
* Function   MenuList_AddStringOne  
* Purpose    加载一项数据到Menulist中
* Params     pItemName:需要添加的文字,id:文字对应的项目的id,pIconName:项目对应的图标文件名
* Return     
* Remarks      
**********************************************************************/
void    MenuList_AddStringOne(HWND hMenuList,const char * pItemName,int id,const char * pIconName)
{
    MENULISTITEMTEMPLATE tmpValue;
    memset(&tmpValue,0x00,sizeof(MENULISTITEMTEMPLATE));

    tmpValue.lpszIconName   = pIconName;
    tmpValue.dwFlags        = MLI_LEFT | MLI_NOTIFY;
    tmpValue.nPicAreaWidth  = 20;
    tmpValue.wID            = id;
    tmpValue.lpszItemName   = pItemName;
    SendMessage(hMenuList, ML_ADDITEM, 0, (LPARAM)&tmpValue);
}
/********************************************************************
* Function   ResetMenuList  
* Purpose    清除MENULIST中的全部数据,*id需要以-1为结束标志
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL    ResetMenuList(HWND hMenuList,int * id)
{
    if (NULL == id)
        return FALSE;

    while (* id > 0)
        SendMessage(hMenuList,ML_DELITEM,* id++,0);

    return TRUE;
}
/********************************************************************
* Function   CountControlsAbscissa  
* Purpose    计算控件横坐标的函数 
* Params     pConX:用于返回控件的横坐标,iControlW:各个控件的宽
            count:用于计算控件的个数
* Return     
* Remarks    要求各个控件在界面上从左到右排列,并且各个控件的间隔是一致的
**********************************************************************/
BOOL    CountControlsAbscissa(int * pConX,unsigned int iControlW,size_t count)
{
    int i;
    int iControlNum;            //控件个数
    int ixStep;                 //保存每个控件的间隔
    unsigned int iScreenWidth;  //屏幕宽
    unsigned int iControlsWidth;//控件宽度和

    if (pConX == NULL)
        return FALSE;

    iControlNum     = count / sizeof(int);
    iScreenWidth    = GetScreenUsableWH1(SM_CXSCREEN);
    iControlsWidth  = iControlNum * iControlW;
    if (iControlsWidth > iScreenWidth)
        return FALSE;

    ixStep = (iScreenWidth - iControlsWidth) / (iControlNum + 1);//计算每个控件的间隔
    for (i = 0;i < iControlNum ; i++)
        * pConX++ = ixStep + i * (iControlW + ixStep);

    return TRUE;
}
/********************************************************************
* Function   CountControlsYaxis  
* Purpose    计算控件纵坐标的函数 
* Params     piConY:用于返回控件的纵坐标,iControlH:各个控件的高
            count:用于计算控件的个数
* Return     
* Remarks    要求各个控件在界面上从上到下排列,并且各个控件的间隔是一致的
**********************************************************************/
BOOL    CountControlsYaxis(int * piConY,unsigned int iControlH,size_t count)
{
    int i;
    int iControlNum;                //控件个数
    int iyStep;                     //保存每个控件的间隔
    unsigned int iControlsHeight;   //控件高度和
    unsigned int iScreenHeight;     //屏幕高

    if (piConY == NULL)
        return FALSE;

    iControlNum     = count / sizeof(int);              //控件个数
    iScreenHeight   = GetScreenUsableWH1(SM_CYSCREEN);  //屏幕高
    iControlsHeight = iControlNum * iControlH;          //控件高度和

    if (iScreenHeight < iControlsHeight)//控件高度和大于屏幕高,无法进行计算
        return FALSE;

    iyStep = (iScreenHeight - iControlsHeight) / (iControlNum + 1);//计算每个控件的间隔
    for (i = 0;i < iControlNum;i++)
        * piConY++ = iyStep + i * (iControlH + iyStep);//计算控件的纵坐标

    return TRUE;
}
/********************************************************************
* Function   CountControlYaxisExt  
* Purpose    计算控件纵坐标的函数 
* Params     piConY:用于返回控件的纵坐标,iControlH:各个控件的高
            count:用于计算控件的个数
            iDirect:1表示要求界面上方留出一定的空间,界面的其余部分均匀排列控件
            iDirect:-1表示要求界面下方留出一定的空间,界面的其余部分均匀排列控件
            iSpace:表示留出空间的大小
* Return     
* Remarks    要求各个控件在界面上从上到下排列,并且各个控件的间隔是一致的
**********************************************************************/
BOOL    CountControslYaxisExt(int * piConY,unsigned int iControlH,size_t count,int iDirect,unsigned int iSpace)
{
    int i,iyStep,iControlNum;
    unsigned int iScreenHeight;     //屏幕高
    unsigned int iControlsHeight;   //控件高度和

    if (piConY == NULL)
        return FALSE;

    iControlNum     = count / sizeof(int);              //控件个数
    iScreenHeight   = GetScreenUsableWH1(SM_CYSCREEN);  //屏幕高
    iControlsHeight = iSpace + iControlNum * iControlH; //控件高度和

    if (iScreenHeight < iControlsHeight)//控件高度和大于屏幕高,无法进行计算
        return FALSE;

    iyStep = (iScreenHeight - iControlsHeight) / (iControlNum + 1);//计算每个控件的间隔

    switch (iDirect)
    {
    case 1://表示要求界面上方留出一定的空间,界面的其余部分均匀排列控件
        for (i = 0;i < iControlNum;i++)
            * piConY++ = iyStep + i * (iControlH + iyStep) + iSpace;//计算控件的纵坐标
        return TRUE;

    case -1://表示要求界面下方留出一定的空间,界面的其余部分均匀排列控件
        for (i = 0;i < iControlNum;i++)
            * piConY++ = iyStep + i * (iControlH + iyStep);//计算控件的纵坐标
        return TRUE;

    default:
        return FALSE;
    }
}
//文字可以自动换行的提示框
void    AppMessageBox(HWND hwnd,const char * szPrompt,const char * szCaption,int nTimeout)
{
    HDC     hdc;
    char  * pDesstr = NULL;
    int     ilen;
    const RECT rect = {60,81,148,118};//定义提示框显示文字的区域

    if (szPrompt == NULL)
        return;
    if ((hwnd == NULL) || (strlen(szPrompt) == 0))
    {
        PLXTipsWin((char *)szPrompt,MESSAGEBOXTITLE,WAITTIMEOUT);//(char *)szCaption
        return;
    }
    ilen = strlen(szPrompt) * (strlen(pChNewLine) + 1) + 1;//极端的情况,一个字符(加上换行符)占一行

    if ((pDesstr = (char *)malloc(ilen)) == NULL)
    {
        PLXTipsWin((char *)szPrompt,MESSAGEBOXTITLE,WAITTIMEOUT);
        return;
    }
    memset(pDesstr,0x00,ilen);

    hdc = GetDC(hwnd);
    StrAutoNewLine(hdc,szPrompt,pDesstr,&rect,0);
    ReleaseDC(hwnd,hdc);

    PLXTipsWin(pDesstr,MESSAGEBOXTITLE,WAITTIMEOUT);

    free(pDesstr);
}
////////////////////////////支持自动换行的提示窗口//////////////////////////////////////////////
BOOL    AppConfirmWin(HWND hwnd,const char * szPrompt,const char * szTipsPic,
                      const char * szCaption,const char * szOk,const char * szCancel)
{
    HDC     hdc;
    char  * pDesstr = NULL;
    int     ilen;
    BOOL    bres;
    const RECT rect = {58,75,155,125};//定义提示框显示文字的区域

    if (szPrompt == NULL)
        return FALSE;

    if ((hwnd == NULL) || (strlen(szPrompt) == 0))
    {
        return (PLXConfirmWin(szPrompt,szTipsPic,szCaption,CONFIRM_OK,CONFIRM_CANCEL));
    }

    ilen = strlen(szPrompt) * (strlen(pChNewLine) + 1) + 1;//极端的情况,一个字符(加上换行符)占一行

    if ((pDesstr = (char *)malloc(ilen)) == NULL)
    {
        return PLXConfirmWin(szPrompt,szTipsPic,szCaption,CONFIRM_OK,CONFIRM_CANCEL);
    }
    memset(pDesstr,0x00,ilen);

    hdc = GetDC(hwnd);
    StrAutoNewLine(hdc,szPrompt,pDesstr,&rect,0);
    ReleaseDC(hwnd,hdc);

    bres = PLXConfirmWin(pDesstr,szTipsPic,szCaption,CONFIRM_OK,CONFIRM_CANCEL);

    free(pDesstr);

    return(bres);
}
/********************************************************************
* Function   IfValidDTMFNumber  
* Purpose    当前字符串是否是合法的分机号
* Params    pStr:字符串
* Return    当前字符串是否是合法的分机号返回TRUE,否则返回FALSE
* Remarks   
**********************************************************************/
BOOL    IfValidDTMFNumber(const char * pStr)
{
    char * p = (char *)pStr;

    if (strlen(pStr) == 0)
        return FALSE;

    while (* p)
    {
        if ((cCh_Num0 <= * p) && (* p <= cCh_Num9))
            return TRUE;
        p++;
    }
    return FALSE;
}
/********************************************************************
* Function   StrIsNumber  
* Purpose    当前字符串是否可以完全转化成数字
* Params    pStr:字符串
* Return    当前字符串可以完全转化成数字返回TRUE,否则返回FALSE
* Remarks   
**********************************************************************/
BOOL    StrIsNumber(const char * pStr)
{
    char * p = (char *)pStr;

    if (strlen(pStr) == 0)
        return FALSE;

    while (* p)
    {
        if ((* p < cCh_Num0) || (* p > cCh_Num9))
            return FALSE;
        p++;
    }
    return TRUE;
}
/********************************************************************
* Function   StrIsDouble  
* Purpose    当前字符串是否可以完全转化成double数字
* Params    pStr:字符串
* Return    当前字符串可以完全转化成数字返回TRUE,否则返回FALSE
* Remarks   
**********************************************************************/
BOOL    StrIsDouble(const char * pStr)
{
    char * p = (char *)pStr;
    int iPointNum = 0;//小数点的个数

    if (strlen(pStr) == 0)
        return FALSE;

    if (* p == cCh_Point)//第一个字符是小数点
        return FALSE;

    while (* p)
    {
        if (((* p < cCh_Num0) || (* p > cCh_Num9)) && (* p != cCh_Point))
            return FALSE;
        if (* p == cCh_Point)
            iPointNum++;//统计小数点的个数

        if (iPointNum > 1)//小数点个数大于1
            return FALSE;
        p++;
    }

    p--;
    if (* p == cCh_Point)//最后一个字符是小数点
        return FALSE;

    return TRUE;
}
/********************************************************************
* Function   MB_Mtoa  
* Purpose   浮点数转换成字符串
* Params    dNumber:浮点数,pNumber:转换结果
* Return    
* Remarks   
**********************************************************************/
char *  MB_Mtoa(double dNumber,char * pNumber)
{    
    /*char * pBuffer = NULL;
    int iDec,iSign;

    pBuffer = ecvt(dNumber,10,&iDec,&iSign);

    if (iSign < 0)
        strcat(pNumber,pMinus);

    if (iDec > 0)
    {
        strncat(pNumber,pBuffer,iDec);  //拷贝整数部分
        strcat(pNumber,pChPoint);       //拷贝小数点
        strcat(pNumber,pBuffer + iDec); //拷贝剩余部分
    }
    else
    {
        strcat(pNumber,pChNumer0);  //拷贝整数部分
        strcat(pNumber,pChPoint);   //拷贝小数点
        while (iDec++ < 0)          //拷贝小数点之后的零
            strcat(pNumber,pChNumer0);

        strcat(pNumber,pBuffer);    //拷贝剩余部分
    }*/
    return pNumber;
}
/********************************************************************
* Function   MB_atof  
* Purpose   字符串转换成浮点数 
* Params    pString:需要转换的字符串
* Return    转换结果
* Remarks   
**********************************************************************/
double  MB_atof(const char * pString)
{
    double result;
    double dMult = 1.0;
    int iLen;
    BOOL bMult = FALSE;
    char * pNewString = NULL;

    iLen = strlen(pString);
    if (iLen == 0)
        return 0.0;

    pNewString = (char *)malloc(iLen + 1);
    if (pNewString == NULL)
        return 0.0;

    memset(pNewString,0x00,iLen + 1);

    while (* pString)
    {
        if (* pString == cCh_Point)
        {
            bMult = TRUE;
        }
        else
        {
            if (bMult)
                dMult *= 10;

            strncat(pNewString,pString,1);
        }
        pString++;
    }
    result = atol(pNewString) / dMult;

    free(pNewString);
    return result;
}
/********************************************************************
* Function     MB_DealDouble
* Purpose    将字符串中的小数点后几位的数字零清除
* Params    pSour:该字符串必须可以转换成double类型,iBit:小数点后需要清零的位数
* Return    
* Remarks   本函数将double数据小数点后iBit位以后的数据清零,若iBit为零,则将小数点之后的数据清零,包括小数点
**********************************************************************/
void    MB_DealDouble(char * pSour,unsigned int iBit)
{
    char * p;

    if (pSour == NULL)
        return;
    p = strstr(pSour,pChPoint);
    if (p == NULL)//没有小数点,直接返回
        return;

    if (iBit == 0)
    {
        * p = cCh_0;
        return;
    }

    p++;//跳过小数点
    if (strlen(p) > iBit)
    {
        p += iBit;
        * p = cCh_0;
    }
}
/********************************************************************
* Function   InStrArray  
* Purpose   在指针数组中查找字符串pSearchStr
* Params    pSearchStr:需要查找的字符串,pArray:指针数组
* Return    找到返回该字符串所在位置,否则返回-1
* Remarks   
**********************************************************************/
int     InStrArray(const char * pSearchStr,const char ** pArray)
{
    char ** p;
    int iIndex = 0;

    if (strlen(pSearchStr) == 0)
        return -1;

    p = (char**)pArray;
    while (**p)
    {
        if (strcmp(* p,pSearchStr) == 0)
            return iIndex;

        p++;
        iIndex++;
    }
    return -1;
}
/********************************************************************
* Function   InCharArray  
* Purpose   在指针数组中查找字符ch
* Params    ch:需要查找的字符,pArray:指针数组
* Return    找到返回该字符所在位置,否则返回-1
* Remarks   
**********************************************************************/
int     InCharArray(char ch,const char * pArray,size_t count)
{
    int iArrayNum,i;
    char * p;

    p         = (char*)pArray;
    iArrayNum = count / sizeof(char);//数组个数

    for (i = 0;i < iArrayNum;i++)
    {
        if (* p == ch)
            return i;
        p++;
    }
    return -1;
}
/********************************************************************
* Function   InIntArray
* Purpose   在指针数组中查找数字iSearchStr
* Params    iSearchStr:需要查找的数字,pArray:指针数组,count:数组大小
* Return    找到返回该数字所在位置,否则返回-1
* Remarks   
**********************************************************************/
int     InIntArray(int iSearchInt,const int * pArray,size_t count)
{
    int iArrayNum,i;
    int * p;

    p         = (int*)pArray;
    iArrayNum = count / sizeof(int);//数组个数

    for (i = 0;i < iArrayNum;i++)
    {
        if (* p == iSearchInt)
            return i;
        p++;
    }
    return -1;
}
/********************************************************************
* Function   TimeCmp  
* Purpose    比较时间
* Params     pTime1:第一个时间,pTime2:第二个时间
* Return     <0:第一个时间小于第二个时间,
            0:第一个时间等于第二个时间
            >0:第一个时间大于第二个时间
* Remarks     
**********************************************************************/
int     TimeCmp(const SYSTEMTIME * pTime1,const SYSTEMTIME * pTime2)
{
    if (pTime1->wYear < pTime2->wYear)
        return -1;
    if (pTime1->wYear > pTime2->wYear)
        return 1;
    //以上表示年相等
    if (pTime1->wMonth < pTime2->wMonth)
        return -1;
    if (pTime1->wMonth > pTime2->wMonth)
        return 1;
    //以上表示月相等
    if (pTime1->wDay < pTime2->wDay)
        return -1;
    if (pTime1->wDay > pTime2->wDay)
        return 1;
    //以上表示日相等
    if (pTime1->wHour < pTime2->wHour)
        return -1;
    if (pTime1->wHour > pTime2->wHour)
        return 1;
    //以上表示小时相等
    if (pTime1->wMinute < pTime2->wMinute)
        return -1;
    if (pTime1->wMinute > pTime2->wMinute)
        return 1;
    //以上表示分钟相等
    if (pTime1->wSecond < pTime2->wSecond)
        return -1;
    if (pTime1->wSecond > pTime2->wSecond)
        return 1;
    //时间相等
    return 0;
}
/********************************************************************
* Function   MBPlayMusic  
* Purpose   播放来电音乐
* Params    iMult:当前通话个数
* Return    
* Remarks   
**********************************************************************/
void    MBPlayMusic(int iMult)
{
    if (0 == iMult)//当前还没有通话
    {
        Call_PlayMusic(RING_PHONE,0);
        return;
    }
    PlayClewMusic(1);//当前已经有了个通话,需要播放提示音
}
//////////////////////////////////////////////////////////////////////////
BOOL    EarphoneStatus(void)//是否有耳机
{
    return (GetEarphone());
}
/********************************************************************
* Function   PlayClewMusic  
* Purpose   播放提示音
* Params    iPlayCount:播放次数
* Return    
* Remarks   
**********************************************************************/
void    PlayClewMusic(unsigned int iPlayCount)//播放提示音
{
    //Call_PlayMusic(RING_TOUCH,1);
}
/********************************************************************
* Function   GetPhoneItem  
* Purpose   为通话信息结构中的姓名,号码赋值
* Params    pPhoneItem:通话信息结构.pNumber:电话号码
* Return    该电话是否在联系人中定义
* Remarks  
**********************************************************************/
BOOL    GetPhoneItem(PHONEITEM * pPhoneItem,const char * pNumber)
{
    int     iSour;
    PBINFO  pbInfo;//从电话本中读取信息的结构

    memset(&pbInfo,0x00,sizeof(PBINFO));

    if (MB_GetInfoFromPhonebook(pNumber,&pbInfo))//拷贝姓名
    {
        iSour = strlen(pbInfo.Name);
        if (iSour > 0)  //该电话不是陌生的,拷贝姓名
        {
            strncpy(pPhoneItem->cName,pbInfo.Name,
                sizeof(pPhoneItem->cName) > iSour ? iSour + 1 : sizeof(pPhoneItem->cName) - 1);
        }
        else            //该电话不是陌生的,但是姓名是空,将姓名位置清空
        {
            memset(pPhoneItem->cName,0x00,sizeof(pPhoneItem->cName));
        }
        return TRUE;
    }
    memset(pPhoneItem->cName,0x00,sizeof(pPhoneItem->cName));
    return FALSE;
}
/********************************************************************
* Function   IfValidPhoneNumber  
* Purpose    当前字符串是否是合法的电话号码
* Params    pPhoneNumber:电话号码
* Return    
* Remarks   
**********************************************************************/
BOOL    IfValidPhoneNumber(const char * pPhoneNumber)
{
    char * p;

    if (NULL == pPhoneNumber)
        return FALSE;
    if (strlen(pPhoneNumber) == 0)
        return FALSE;

    p = (char *)pPhoneNumber;

    if ((cCh_ExtP == * p) || (cCh_ExtW == * p))//第一位就是p或者是w
        return FALSE;

    while (* p)
    {
        if ((cCh_Num0 <= * p && * p <= cCh_Num9) || 
            (cCh_Well == * p || cCh_Plus == * p || 
            cCh_Asterisk == * p || cCh_ExtP == * p || cCh_ExtW == * p))
            p++;
        else
            return FALSE;
    }
    return TRUE;
}
/********************************************************************
* Function     IfExistExtension
* Purpose      电话号码中是否包含分机号
* Params       pPhoneNumber:原电话号码,pPureNumber:若包含分机号的话,返回纯电话号码
                pExtension:若包含分机号的话,返回分机号
* Return     电话号码中包含分机号返回TRUE,否则返回FALSE
* Remarks      
**********************************************************************/
DTMF_TYPE   IfExistExtension(const char * pPhoneNumber,char * pPureNumber,
                                    char * pExtension)
{
    char * p = NULL,* pEnd = NULL;
    int iPureNumberLen;
    DTMF_TYPE dtmf_type = DTMF_NONE;

    p = (char*)pPhoneNumber;
    while (* p)
    {
        if ((cCh_ExtP == * p) || (cCh_ExtW == * p))//有分机号
        {
            dtmf_type = (cCh_ExtP == * p) ? DTMF_PAUSE : DTMF_WAIT;

            if (pPureNumber != NULL)//返回纯电话号码
            {
                iPureNumberLen = abs(pPhoneNumber - p);
                strncpy(pPureNumber,pPhoneNumber,iPureNumberLen);//拷贝分机号之前的号码
                pEnd   = pPureNumber + iPureNumberLen;
                * pEnd = cCh_0;
            }
            if (pExtension != NULL)//返回分机号
            {
                strcpy(pExtension,++p);
            }
            return dtmf_type;
        }
        p++;
    }
    return DTMF_NONE;
}
/********************************************************************
* Function   NumberIsEmergent  
* Purpose    当前号码是否紧急电话号码
* Params    pNumber:当前号码,
            bPureNumber:TRUE表示传入的号码(pNumber)是纯电话号码,
            FALSE表示传入的号码(pNumber)可能包含GSM前缀和分机号
* Return     当前号码是紧急电话号码返回TRUE;否则返回FALSE
* Remarks   
**********************************************************************/
BOOL    NumberIsEmergent(const char * pNumber,BOOL bPureNumber)
{
    int iGsmSpecific;
    char cPureNumber[PHONENUMMAXLEN + 1] = "";

    if (bPureNumber)
        return (InStrArray(pNumber,pEmergentNumber) != -1);

    iGsmSpecific = GsmSpecific(pNumber);//检测该号码是否包含GSM前缀

    if (DTMF_NONE != IfExistExtension(pNumber,cPureNumber,NULL))//当前号码中包含分机号
        return (InStrArray(cPureNumber + iGsmSpecific,pEmergentNumber) != -1);
    else
        return (InStrArray(pNumber     + iGsmSpecific,pEmergentNumber) != -1);
}
/********************************************************************
* Function   GetIntArrayMaxValue  
* Purpose    获得数字数组中元素的最大值
* Params    pArray:数组地址,count:数组大小
* Return    
* Remarks   
**********************************************************************/
int     GetIntArrayMaxValue(int * pArray,size_t count)
{
    int iArrayNum,i,iMaxValue = 0;
    int * p = NULL;

    p         = (int*)pArray;
    iArrayNum = count / sizeof(int);//数组个数

    for (i = 0;i < iArrayNum;i++)
    {
        if (* p > iMaxValue)
            iMaxValue = * p;
        p++;
    }
    return iMaxValue;
}
//////////////////////////////////////////////////////////////////////////
#define MBWAIT_TITLE        MBGetString(STR_MBPHONE_MBWAIT_TITLE)//"处理数据"
#define MBWAIT_STATE        MBGetString(STR_MBPHONE_WAIT)        //"请稍等"
#define IDC_BUTTON_QUIT     3//退出按钮
#define IDC_BUTTON_SET      4//确定
#define IDC_EDIT            5

static const char * pMBWaitClassName = "MBWaitWindow";
static const char * pMBWaitPic       = SETUPPIC_NETSETTING_WAIT;
static const int    iPicY            = 10;//图片纵坐标

static HWND         hParent;
static HWND         hMBWaitWindow = NULL;
static int          iCallBackMsg;//保存调用方定义的消息
static HINSTANCE    hInstanceMBWait;
static LRESULT MBWaitAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static BOOL    MBWaitWindow(HWND hWnd,int icallmsg,BOOL bRButton)
{
    WNDCLASS wc;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = MBWaitAppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pMBWaitClassName;

    if (!RegisterClass(&wc))
        return FALSE;

    hMBWaitWindow = CreateWindow(pMBWaitClassName,MBWAIT_TITLE, 
        WS_CAPTION|WS_BORDER |PWS_STATICBAR, 
        PLX_WIN_POSITION, hWnd, NULL, NULL, NULL);

    if (NULL == hMBWaitWindow)
    {
        UnregisterClass(pMBWaitClassName,NULL);
        return FALSE;
    }

    if (bRButton)//响应右键退出
    {
        hParent      = hWnd;
        iCallBackMsg = icallmsg;//保存消息值,退出的时候发给调用方一个消息
        SendMessage(hMBWaitWindow,PWM_CREATECAPTIONBUTTON,
            (WPARAM)IDC_BUTTON_QUIT,(LPARAM)MBGetString(STR_WINDOW_EXIT));
    }
    else
    {
        hParent      = NULL;
        iCallBackMsg = -1;//表示不响应右键退出
    }

    ShowWindow(hMBWaitWindow,SW_SHOW);
    UpdateWindow(hMBWaitWindow);
    return TRUE;
}
static LRESULT MBWaitAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static  HGIFANIMATE hgifanimate;
    static  RECT        rectState;
    static  SIZE        size = {0,0};
            int         isw,iFontHeight;
            LRESULT     lResult;
            HDC         hdc;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        hgifanimate = NULL;
        isw         = GetScreenUsableWH1(SM_CXSCREEN);
        iFontHeight = GetCharactorHeight(hWnd);

        if ((size.cx == 0) || (size.cy == 0))//第一次执行的时候需要获得文件尺寸
            GetImageDimensionFromFile(pMBWaitPic,&size);//获得文件尺寸

        hgifanimate = StartAnimatedGIFFromFile(hWnd,pMBWaitPic,
            (isw - size.cx) / 2,iPicY,DM_NONE);

        SetRect(&rectState,0,size.cy + iPicY,isw,size.cy + iPicY + iFontHeight * 2);
        break;

    case WM_TODESKTOP:
        return TRUE;

    case WM_DESTROY ://通知程序管理器,此程序退出;
        EndAnimatedGIF(hgifanimate);
        hMBWaitWindow = NULL;
        UnregisterClass(pMBWaitClassName,NULL);
        DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstanceMBWait );
        break;

    case WM_PAINT :
        hdc = BeginPaint(hWnd,NULL);
        DrawText(hdc,MBWAIT_STATE,strlen(MBWAIT_STATE),&rectState,DT_CENTER);
        PaintAnimatedGIF(hdc,hgifanimate);
        EndPaint(hWnd,NULL);
        break;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_F2:
        case VK_F10:
            if (iCallBackMsg != -1)//响应右键退出
            {
                if (hParent != NULL)
                    PostMessage(hParent,iCallBackMsg,0,0);//给调用方发个消息

                DestroyWindow(hWnd);
            }
            break;
        }
        break;

    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        case IDC_BUTTON_QUIT://退出
            if (iCallBackMsg != -1)//响应右键退出
                SendMessage(hWnd,WM_KEYDOWN,VK_F10,0);
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
* Function   MBWaitWindowState  
* Purpose   显示提示框
* Params    hWnd:调用方句柄,bState:是否显示提示框,
            icallmsg:调用方提供的消息值(bState=TURE的时候生效),
            bButton:是否响应右键退出(bState=TURE的时候生效)
* Return    
* Remarks   
**********************************************************************/
BOOL    MBWaitWindowState(HWND hWnd,BOOL bState,int icallmsg,BOOL bRButton)
{
    if (bState)
    {
        if (IsWindow(hMBWaitWindow))
            MoveWindowToTop(hMBWaitWindow);
        else
            MBWaitWindow(hWnd,icallmsg,bRButton);
    }
    else
    {
        if (IsWindow(hMBWaitWindow))
            DestroyWindow(hMBWaitWindow);
    }
    return TRUE;
}
//////////////////////////////////////////////////////////////////////////
static const char * pReplyControlClassName = "MBReplyControlWndClass";//窗口类名
static HINSTANCE    hInstanceReply;
static char       * pReplyResult;

static LRESULT ReplyControlAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
/********************************************************************
* Function   CallReplyControlWindow  
* Purpose   输入数据的控件
* Params    hwndCall:调用方句柄,pTitle:窗口标题,iEditMaxLen:输入最大长度,
            pResult:返回结果,dwFlags:控件风格(暂时没用)
* Return    
* Remarks   
**********************************************************************/
BOOL    CallReplyControlWindow(HWND hwndCall,const char * pTitle,
                               unsigned int iEditMaxLen,char * pResult,
                               const char * pBLeftTxt,const char * pBRightTxt,
                               DWORD dwFlags)
{
    HWND        hWnd;
    WNDCLASS    wc;
    MSG         msg;

    if ((iEditMaxLen == 0) || (pResult == NULL))
        return FALSE;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = ReplyControlAppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pReplyControlClassName;

    if (!RegisterClass(&wc))
        return FALSE;

    hWnd = CreateWindow(pReplyControlClassName,pTitle,
        WS_CAPTION|WS_BORDER |PWS_STATICBAR,PLX_WIN_POSITION , 
        hwndCall, NULL, NULL, &iEditMaxLen);

    if (NULL == hWnd)
    {
        UnregisterClass(pReplyControlClassName,NULL);
        return FALSE;
    }
    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_SET,1),(LPARAM)pBLeftTxt);
    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT,(LPARAM)pBRightTxt);

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd); 

    while (IsWindow(hWnd))
    {
        GetMessage(&msg, NULL, 0, 0);
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    if (pReplyResult != NULL)
    {
        strcpy(pResult,pReplyResult);
        free(pReplyResult);
        pReplyResult = NULL;
    }
    return (TRUE);
}
static LRESULT ReplyControlAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static  InputEdit   IReply;
            int         iEditMaxLen,iLen;
            HWND        hEdit;
            LRESULT     lResult;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        hEdit = CreateWindow ("INPUTEDIT", NULL, 
            WS_VISIBLE | WS_CHILD |WS_BORDER|ES_MULTILINE | WS_VSCROLL,
            0, 0, GetScreenUsableWH1(SM_CXSCREEN), GetScreenUsableWH1(SM_CYSCREEN),
            hWnd,(HMENU)IDC_EDIT,NULL,NULL);

        if (hEdit == NULL)
            return FALSE;

        IReply.lStyle = GetWindowLong(hEdit,GWL_STYLE);
        IReply.nKeyBoardType = GetImeHandle(pInputStr);
        strcpy(IReply.szName,MBGetString(STR_MBPHONE_USSD_REPLY));
        SetWindowLong(hEdit,GWL_USERDATA,(LONG)&IReply);

        iEditMaxLen = *((int*)((CREATESTRUCT*)lParam)->lpCreateParams);
        SendMessage(hEdit,EM_LIMITTEXT,iEditMaxLen,0);
        break;

    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd,IDC_EDIT));
        break;

    case WM_SETRBTNTEXT:
        SetRightButtonText(hWnd,wParam,lParam);
        break;

    case WM_ACTIVATE://去掉这个消息，单击右键可以退出，否则退出失败
        if (WA_INACTIVE == LOWORD(wParam))
            SetFocus(GetDlgItem(hWnd,IDC_EDIT));//激活后设置焦点

        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    case WM_CLOSE: //点击右上角的退出按钮时为保存退出;
        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_BUTTON_QUIT, 0 );   
        break;

    case WM_DESTROY ://通知程序管理器,此程序退出;
        UnregisterClass(pReplyControlClassName,NULL);
        DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)hInstanceReply);
        break;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_F10:
            DestroyWindow(hWnd);
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
        case IDC_BUTTON_SET://设置
            hEdit = GetDlgItem(hWnd,IDC_EDIT);
            iLen  = GetWindowTextLength(hEdit);
            if (0 == iLen)
                break;

            pReplyResult = (char *)malloc(iLen + 1);
            if (NULL == pReplyResult)
                break;
            memset(pReplyResult,0x00,iLen + 1);
            GetWindowText(hEdit,pReplyResult,iLen + 1);

            DestroyWindow(hWnd);
            break;
        case IDC_BUTTON_QUIT://退出
            DealWithRightButton(hWnd);
            break;
        }
        break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
//////////////////////////////////////////////////////////////////////////
/********************************************************************
* Function   MB_GetInfoFromPhonebook  
* Purpose   从联系人处获得数据的函数
* Params    
* Return    
* Remarks   这是对外获取信息的接口函数,可以随时虚掉,或者更改函数名,尽量减少打电话其它部分的更改
**********************************************************************/
BOOL    MB_GetInfoFromPhonebook(const char * pPhone, PBINFO * pInfo)
{
    int isour;
    BOOL bRes;

    bRes = APP_GetNameByPhone(pPhone,pInfo);
    if (bRes)
    {
        if (strlen(pInfo->Name) == 0)//如果姓名为空的话,将电话号码填入姓名的位置
        {
            isour = strlen(pPhone);
            strncpy(pInfo->Name,pPhone,sizeof(pInfo->Name) > isour ? isour + 1 : sizeof(pInfo->Name) - 1);
        }
    }
    return bRes;
}
/********************************************************************
* Function   MBGetDirectDial  
* Purpose   从联系人处获得快速拨号所需数据的函数
* Params    
* Return    -1:没有这个功能,0,该位置没有记录,1该位置有记录pNo返回电话号码
* Remarks   这是对外获取信息的接口函数,可以随时虚掉,或者更改函数名,尽量减少打电话其它部分的更改
**********************************************************************/
int     MB_GetDirectDial(WORD wDDIdx, char * pNo)
{
    return (APP_GetDirectDial(wDDIdx,pNo) ? 1 : 0);
}
/********************************************************************
* Function   MB_GetSIMRcdByOder  
* Purpose   据输入序号（非空间号）取SIM卡记录
* Params    
* Return    -1:没有这个功能,0,该位置没有记录,1该位置有记录pNo返回电话号码
* Remarks   这是对外获取信息的接口函数,可以随时虚掉,或者更改函数名,尽量减少打电话其它部分的更改
**********************************************************************/
int     MB_GetSIMRcdByOder(int iOrder,PPHONEBOOK pRtnRcd)
{
    return (APP_GetSIMRcdByOder((short)iOrder,pRtnRcd,1) == 1 ? 1 : 0);
}
/********************************************************************
* Function   MB_SaveToPhoneBook  
* Purpose   保存电话号码到联系人
* Params    hWnd:调用方句柄,pNumber电话号码
* Return    
* Remarks   这是对外获取信息的接口函数,可以随时虚掉,或者更改函数名,尽量减少打电话其它部分的更改
**********************************************************************/
BOOL    MB_SaveToPhoneBook(HWND hWnd,const char * pNumber)
{
    return APP_SaveToPhoneBook(hWnd,pNumber);
}
/********************************************************************
* Function   MB_SendSms  
* Purpose   发送短信
* Params    pNumber电话号码
* Return    
* Remarks   这是对外获取信息的接口函数,可以随时虚掉,或者更改函数名,尽量减少打电话其它部分的更改
**********************************************************************/
void    MB_SendSms(const char * pNumber)
{
    APP_EditSMS(pNumber, NULL);
}
/********************************************************************
* Function   MB_SendMms  
* Purpose   发送彩信
* Params   pNumber电话号码
* Return    
* Remarks   这是对外获取信息的接口函数,可以随时虚掉,或者更改函数名,尽量减少打电话其它部分的更改
**********************************************************************/
void    MB_SendMms(const char * pNumber)
{
    CallMMSEditWnd("",NULL,0, MMS_CALLEDIT_MOBIL, (char*)pNumber);
}
//通话之前,之后需要执行的操作
static  BOOL        bScreen;
static  DWORD       bLcdState;
//static  HWAVEOUT    hWaveOut = NULL;
void    DisableState(void)//通话之前执行的操作
{
    bScreen   = DlmNotify(PMM_NEWS_ENABLE_SCRSAVE,FALSE);//禁止屏保
    bLcdState = DlmNotify(PMM_NEWS_GET_FLAGS,0);

    DlmNotify(PMM_NEWS_DISABLE, PMF_AUTO_SHUT_DOWN |
                                PMF_AUTO_SHUT_MAIN_LCD |
                                PMF_AUTO_SHUT_MAIN_LIGHT |
                                PMF_AUTO_SHUT_CPU);

    DlmNotify(PMM_NEWS_DISABLE,PMF_AUTO_SHUT_DOWN);//禁止自动关机

    SetupHf_SetNotSleep();//禁止睡眠
#ifndef _EMULATE_
    //waveOutPauseAll(&hWaveOut);
#endif

    PhoneSetChannel(TRUE);//切换通道
}
void    EnableState(void)//通话之后执行的操作
{
    if (bScreen)
        DlmNotify(PMM_NEWS_ENABLE_SCRSAVE,TRUE);//恢复屏保

    DlmNotify(PMM_NEWS_ENABLE,bLcdState);

    SetupHf_ClearSleepFlag();//允许睡眠
#ifndef _EMULATE_
    //waveOutRestartAll(&hWaveOut);
#endif

    PhoneSetChannel(FALSE);//切换通道
    //hWaveOut = NULL;
}

void    PhoneSetChannel(BOOL bSet)//设置通道
{    
#ifndef _EMULATE_
    //printf("mb SetChannel:%d\r\n",bSet);
    //waveInSetChannel (NULL,bSet,WAVE_CHANNEL_ONLINE);//设置通道
    //waveOutSetChannel(NULL,bSet,WAVE_CHANNEL_ONLINE);//设置通道
#endif
}
void    PhoneSetChannelCalling(BOOL bSet)
{    
#ifndef _EMULATE_
    //printf("mb SetChannelCalling:waveOutSetChannel(NULL,TRUE,0x80000000)\r\n");
    //waveOutSetChannel(NULL,TRUE,0x80000000);
#endif
}
//////////////////////////////////////////////////////////////////////////
static const char * pLogFileName = "FLASH2:Logint.dat";
/********************************************************************
* Function   SaveLog  
* Purpose   保存某个需要记录的字符串到文件中
* Params    pString:保存内容
* Return    
* Remarks   
**********************************************************************/
void    SaveLog(const char * pString)
{
    HANDLE hf;

    if (pString == NULL)
        return;
    if(INVALID_HANDLE_VALUE == (hf = CreateFile(pLogFileName, ACCESS_WRITE|ACCESS_READ, 0)))
    {//未找到文件，创建一个文件
        if (INVALID_HANDLE_VALUE == 
            (hf = CreateFile(pLogFileName, ACCESS_WRITE|ACCESS_READ|CREATE_NEW|CREATE_OVERWRITE, 0)))
        {//创建文件失败
            CloseFile(hf);
            return;
        }
    }
    if (-1 == SetFilePointer(hf,0L,SEEK_END))//设置文件指针
    {
        CloseFile(hf);
        return;
    }
    WriteFile(hf,(PVOID)pString,strlen(pString));
    CloseFile(hf);
}
/********************************************************************
* Function   ReadLog  
* Purpose   读取保存的字符串内容
* Params    pString:返回保存内容
* Return    
* Remarks   
**********************************************************************/
DWORD   ReadLog(char * pString)
{
    HANDLE  hf;
    DWORD   dFileSize;

    if(INVALID_HANDLE_VALUE == (hf = CreateFile(pLogFileName, ACCESS_WRITE|ACCESS_READ, 0)))
    {//未找到文件，创建一个文件
        if (INVALID_HANDLE_VALUE == 
            (hf = CreateFile(pLogFileName, ACCESS_WRITE|ACCESS_READ|CREATE_NEW|CREATE_OVERWRITE, 0)))
        {//创建文件失败
            CloseFile(hf);
            return 0;
        }
    }
    dFileSize = GetFileSize(hf);
    if (NULL == pString)
    {
        CloseFile(hf);
        return dFileSize;
    }
    if (-1 == SetFilePointer(hf,0,SEEK_CUR))//设置文件指针
    {
        CloseFile(hf);
        return 0;
    }
    ReadFile(hf,pString,dFileSize);
    CloseFile(hf);
    return dFileSize;
}
/********************************************************************
* Function   RemoveLog  
* Purpose   删除log文件
* Params    
* Return    
* Remarks   
**********************************************************************/
void    RemoveLog(void)
{
    DeleteFile(pLogFileName);
}
