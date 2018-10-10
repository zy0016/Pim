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
static const int    pChinese            = 0x80;//����ASCII���־
static const char * pRomPath1           = "/rom";
static const char * pRomPath2           = "rom:";
static const char * pInputStr           = "Ӣ��";
static const char * pChStr              = "ʮ";//�����ַ���ʵ��
static const char * pChEng              = "H";//Ӣ���ַ���ʵ��
static const char * pChNewLine          = "\n";//��Ҫ��ӵĻ��з�
static const char * pChPoint            = ".";
static const char * pChEnd              = "\0";
static const char * pString1            = "%ld";
static const char * pChNumer0           = "0";
static const char * pMinus              = "-";
static const char * pMenuClassName      = "MENULIST";
static const char * pListClassName      = "LISTBOX";
static const char * pEmergentNumber[]   = //������뼯��(û��SIM����ʱ��Ҳ���Բ���ĺ���)
{
    "112",//�����绰����
    ""
};

static  int     StrAutoNewLine_chn(HDC hdc,const char * cSourstr,char * cDesstr,int iRectWidth);//��������
static  int     StrAutoNewLine_eng(HDC hdc,const char * cSourstr,char * cDesstr,int iRectWidth);//��������
static  int     GetCharactorWidth(HDC hdc,const char * ch); //��õ����ַ����ߵ������ֵĿ��
static  int     GetStringLanguage(const char * pString);    //��õ�ǰ�ַ���������

//�йغ���ת�Ʋ��ֵĺ���
static BOOL bFWD_UNCDTIONAL;//������ת��
static BOOL bFWD_BUSY;      //��æת��
static BOOL bFWD_NOREPLY;   //��Ӧ��ת��
static BOOL bFWD_NOREACH;   //���ڷ�������ػ�ת��

static  void    SetCallForwardIcon(BOOL bForward)//���ú���ת�Ƶ�ͼ��
{
    DlmNotify(PS_SETCALL,(LPARAM)(bForward ? ICON_SET : ICON_CANCEL));
}
BOOL    GetForward_Uncdtional(void)//�����Ƿ����������ת��
{
    return (bFWD_UNCDTIONAL);
}
BOOL    GetForward_Condition(void)//�����Ƿ��������ת��
{
    return (bFWD_BUSY || bFWD_NOREPLY || bFWD_NOREACH);
}

void    SetForward_All(BOOL bForward)//�����Ƿ��������ת��
{
    bFWD_UNCDTIONAL = bForward;//������ת��
    bFWD_BUSY       = bForward;//��æת��
    bFWD_NOREPLY    = bForward;//��Ӧ��ת��
    bFWD_NOREACH    = bForward;//���ڷ�������ػ�ת��

    SetCallForwardIcon(bForward);
}
void    SetForward_Uncdtional(BOOL bForward)//�����Ƿ����������ת��
{
    //������ת�ƴ򿪵�ʱ��,����ת�ƶ����Զ��ر�,
    //��������ת�ƹرյ�ʱ��,����ǰ���õ�ĳ��ת�ƻ��Զ���
    //���Ե�������ת�ƹرյ�ʱ��,��Ҫ���²�ѯ��������ת�Ƶ�״̬
    //���������ѯ������Ҫ������Ӧ�õ���,�����ڱ������е���,����Ļ��ᵼ����ѭ��.
    bFWD_UNCDTIONAL  = bForward;//������ת��
    if (bForward)
    {
        bFWD_BUSY    = FALSE;//��æת���Զ��ر�
        bFWD_NOREPLY = FALSE;//��Ӧ��ת���Զ��ر�
        bFWD_NOREACH = FALSE;//���ڷ�������ػ�ת���Զ��ر�
        SetCallForwardIcon(bForward);
    }
}
void    SetForward_Condition(BOOL bForward)//�����Ƿ������������ת��
{
    bFWD_BUSY    = bForward;//��æת��
    bFWD_NOREPLY = bForward;//��Ӧ��ת��
    bFWD_NOREACH = bForward;//���ڷ�������ػ�ת��
    SetCallForwardIcon(bForward);
}
void    SetForward_Busy(BOOL bForward)//�����Ƿ������æת��
{
    bFWD_BUSY = bForward;   //��æת��
    SetCallForwardIcon(bForward);
}
void    SetForward_NoReply(BOOL bForward)//�����Ƿ������Ӧ��ת��
{
    bFWD_NOREPLY = bForward;//��Ӧ��ת��
    SetCallForwardIcon(bForward);
}
void    SetForward_NoReach(BOOL bForward)//�����Ƿ���ڲ��ڷ�������ػ�ת��
{
    bFWD_NOREACH = bForward;//���ڷ�������ػ�ת��
    SetCallForwardIcon(bForward);
}
//////////////////////////////////////////////////////////////////////////
/********************************************************************
* Function   FileIfExist  
* Purpose    ���鵱ǰ�ļ��Ƿ����
* Params     pFileName:�ļ�������·��
* Return     ����ture,������false
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
    if(INVALID_HANDLE_VALUE == hf)//δ�ҵ��ļ�
    {
        CloseFile(hf);
        return FALSE;
    }
    CloseFile(hf);
    return TRUE;
}
/********************************************************************
* Function   SetRightButtonText  
* Purpose    ������������ı���ؼ�,�����Ҽ���ť�ϵ�����
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
* Purpose    ������������ı���ؼ�,ѡ���Ҽ���ť֮��Ĳ���
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
* Purpose    �����Ļ�������뷨��λ��
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
* Purpose    �����Ļ�п������õĿ�Ȼ��߸߶�
* Params     nIndex:SM_CXSCREEN:�����Ļ�п������õĿ��,
                SM_CYSCREEN:�����Ļ�п������õĸ߶�
* Return     ��Ļ�п������õĿ�Ȼ��߸߶�
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
* Purpose    �����Ļ�п������õĿ�Ⱥ͸߶�
* Params     pwidth:�����Ļ�п������õĿ��,
             pheight:�����Ļ�п������õĸ߶�
* Return     ��
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
* Purpose    ���EDIT�ؼ����ʵĸ߶�
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
* Purpose    ���Button�ؼ����ʵĸ߶�
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
* Purpose    ��õ�ѡ��ť�ؼ���Ҫ�Ŀ��
* Params     pString:��ѡ��ť�ϵ�����
* Return     
* Remarks    
**********************************************************************/
int     GetRadioButtonControlWidth(const char * pString)
{
    return (GetStringWidth(NULL,pString) + 15);
}
/********************************************************************
* Function   TrimString  
* Purpose    ɾ���ַ�����ͷ��β�Ŀո��ַ�
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
    if (* pBegin == cCh_Space)//��ͷ�пո�
    {
        pSetp = pStr;
        while ((* pSetp == cCh_Space) && (i < iLen))//���ɨ��,���ҷǿո��ַ�
        {
            pSetp++;
            i++;
        }
        if (i == iLen)//�ַ���ȫ�ǿո�
        {
            * pBegin = cCh_0;
            return pStr;
        }
        //�ҵ����ǿո���ַ�
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
    //�����β�Ƿ��пո�
    pEnd = pStr + iLen - 1;
    while ((* pEnd == cCh_Space) && (iLen-- > 0))
        * pEnd-- = cCh_0;

    return pStr;
}
/********************************************************************
* Function   GetFactFileNameFromPath  
* Purpose    �Ӵ�·�����ļ�������,��ô��ļ���
* Params     pFilePathName:��·�����ļ���,pFileName:���ش��ļ���,
             iBuflen:����������   bExName:�Ƿ������չ��
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
    pEnd = (char*)pFilePathName + iLen - 1;//ָ���ļ���β

    while ((* pEnd != cCh_Slash1) && (* pEnd != cCh_Slash2) && 
        (* pEnd != cCh_Colon) && (iLen > 0))//��ǰɨ��,Ѱ��Ŀ¼�ָ���
    {
        pEnd--;
        iLen--;
    }
    if ((* pEnd == cCh_Slash1) || (* pEnd == cCh_Slash2) || (* pEnd == cCh_Colon))//�ҵ�Ŀ¼�ָ���
    {
        if (bExName)//���ص��ļ���������չ��
        {
            iFileNameLen = strlen(pEnd) - 1;
            strncpy(pFileName,pEnd + 1,(int)iBuflen > iFileNameLen ? iFileNameLen + 1 : iBuflen - 1);
        }
        else//���ص��ļ�����������չ��
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
    else//û�ҵ�Ŀ¼�ָ���
    {
        if (bExName)//���ص��ļ���������չ��
        {
            iFileNameLen = strlen(pFilePathName);
            strncpy(pFileName,pFilePathName,(int)iBuflen > iFileNameLen ? iFileNameLen + 1 : iBuflen - 1);
        }
        else//���ص��ļ�����������չ��
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
* Purpose    �Զ��������躯��
* Params     hdc:���ڴ�ӡ���,cSourstr:Դ�ַ���,cDesstr:Ŀ���ַ���,
             pRect:�������,uFormat:�������(ͬDrawText�����е�uFormat����)
* Return     ���ɵ����ַ����ĳ���
* Remarks    ��cDesstr==NULLʱ,��Ϊֱ���ں���������ַ���,��ֱ�ӵ���DrawText����
            cDesstr!=NULLʱ,�������ɵ��ַ���,��ִ��DrawText����
**********************************************************************/
int     StrAutoNewLine(HDC hdc,const char * cSourstr,char * cDesstr,const RECT * pRect,UINT uFormat)
{
    char  * pDesstr = NULL;//�������ɵ��ַ�������
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

    ilen = iSourLen * (strlen(pChNewLine) + 1) + 1;//���˵����,һ���ַ�(���ϻ��з�)ռһ��
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
    case 0://Ӣ��
        iRowNum = StrAutoNewLine_eng(hdc,cSourstr,pDesstr,sRect.right - sRect.left);
        GetTextExtent(hdc,pChEng,strlen(pChEng),&size);//���Ӣ���ַ��ĸ߶�
        break;
    case 1://����
        iRowNum = StrAutoNewLine_chn(hdc,cSourstr,pDesstr,sRect.right - sRect.left);
        GetTextExtent(hdc,pChStr,strlen(pChStr),&size);//��������ַ��ĸ߶�
        break;
    default:
        free(pDesstr);
        return 0;
    }
    ilen = strlen(pDesstr);//���ɵ��ַ����ĳ���

    if (cDesstr != NULL)
    {
        strcpy(cDesstr,pDesstr);
    }
    else
    {
        if (hdc != NULL)
        {
            if (iRowNum != 0)//���ص�������Ϊ��,�޸ľ�������ĸ߶��������
                sRect.bottom = sRect.top + iRowNum * size.cy + 5;

            DrawText(hdc,pDesstr,strlen(pDesstr),&sRect,uFormat);
        }
    }
    free(pDesstr);

    return (ilen);
}
/********************************************************************
* Function   StrAutoNewLine_chn  
* Purpose    �����ַ����Զ����к���
* Params     hdc:���ڻ���ַ��߶�,cSourstr:Դ�ַ���,cDesstr:Ŀ���ַ���,iRectWidth:�������Ŀ��
* Return     ��������
* Remarks    
**********************************************************************/
static  int     StrAutoNewLine_chn(HDC hdc,const char * cSourstr,char * cDesstr,int iRectWidth)
{
    int     iCharactorWidth;//һ��Ӣ���ַ�����һ�����ֵĿ��
    int     iCopyNum,iSourlen,iRowNum,iCopyWidth;
    char  * p = NULL;

    p           = (char *)cSourstr;
    iSourlen    = strlen(cSourstr);
    iCopyNum    = 0;//�Ѿ��������ַ�����
    iRowNum     = 1;//����
    iCopyWidth  = 0;//һ�����Ѿ��������ַ�һ���ж೤

    while(iCopyNum < iSourlen)
    {
        if ((* p == NULL) || (p == NULL))
            break;
        /*if ((* p == 0xa) || (* p == 0xd))//����ӻ��з�
        {
            p++;
            iCopyNum++;
            continue;
        }*/
        iCharactorWidth = GetCharactorWidth(hdc,p);//��õ�ǰ�ַ�(���ߺ���)�ĳ���
        if (iRectWidth - iCopyWidth > iCharactorWidth)//ʣ�೤�ȴ���һ��Ӣ���ַ�����һ�����ֵĿ��
        {
            if ((unsigned char)(* p) > pChinese)//����
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
        else if (iRectWidth - iCopyWidth == iCharactorWidth)//ʣ�೤�ȵ���һ��Ӣ���ַ�����һ�����ֵĿ��
        {
            if ((unsigned char)(* p) > pChinese)//����
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
            iRowNum++;//������һ
        }
        else if (iRectWidth - iCopyWidth < iCharactorWidth)
        {//ʣ�೤��С��һ��Ӣ���ַ�����һ�����ֵĿ��,��Ҫ��ӻ��з�,������һ
            if (iCopyNum == 0)
            {
                //һ���ַ�Ҳû�п�������ӻ��з�,������������ֵĳ����Ѿ�����
                //�˾�������Ŀ��
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
* Purpose    Ӣ���ַ����Զ����к���
* Params     hdc:���ڻ���ַ��߶�,cSourstr:Դ�ַ���,cDesstr:Ŀ���ַ���,iRectWidth:�������Ŀ��
* Return     ��������
* Remarks    
**********************************************************************/
static  int     StrAutoNewLine_eng(HDC hdc,const char * cSourstr,char * cDesstr,int iRectWidth)
{
    int     iCopyNum,iSourlen,iRowNum,iCopyWidth,iWordWidth,iWordLength,iCopyNumExt;
    char  * p = NULL,* pNextSpace = NULL,* pScan = NULL;

    p           = (char *)cSourstr;
    iSourlen    = strlen(cSourstr);
    iCopyNum    = 0;//�Ѿ��������ַ�����
    iRowNum     = 1;//����
    iCopyWidth  = 0;//һ�����Ѿ��������ַ�һ���ж೤

    while (iCopyNum < iSourlen)
    {
        if ((* p == NULL) || (p == NULL))
            break;
        /*if ((* p == 0xa) || (* p == 0xd))//����ӻ��з�
        {
            p++;
            iCopyNum++;
            continue;
        }*/

        pScan       = p;
        pNextSpace  = strchr(p,cCh_Space);//��Ӣ����ĸ' '

        iWordWidth  = 0;

        if (pNextSpace != NULL)
        {
            iWordLength = abs(pScan - pNextSpace);//������Ӣ�ĵ��ʵ���ĸ����

            while (pScan != pNextSpace)
            {
                iWordWidth += GetCharactorWidth(hdc,pScan);//������Ӣ�ĵ��ʵĿ��
                pScan++;
            }
        }
        else//�����ַ�����β
        {
            iWordLength = iSourlen - iCopyNum;

            while (* pScan != NULL)
            {
                iWordWidth += GetCharactorWidth(hdc,pScan);//������Ӣ�ĵ��ʵĿ��
                pScan++;
            }
        }

        if (iRectWidth - iCopyWidth > iWordWidth)//ʣ�೤��>һ��Ӣ�ĵ��ʵĿ��,ֱ�ӿ���
        {
            strncat(cDesstr,p,iWordLength + 1);//��Ҫ���ո�(���߷�Ӣ����ĸ)Ҳ������ȥ
            iCopyNum   += (iWordLength + 1);//���Ѿ��������ַ����������ۼ�,�����ո�(���߷�Ӣ����ĸ)
            iCopyWidth += (iWordWidth + GetCharactorWidth(hdc,pNextSpace));//������ۼ�

            p += (iWordLength + 1);
        }
        else if(iRectWidth - iCopyWidth == iWordWidth)//ʣ�೤�ȵ���һ��Ӣ�ĵ��ʵĿ��,ֱ�ӿ���,Ȼ����ӻ��з�
        {
            strncat(cDesstr,p,iWordLength + 1);//��Ҫ���ո�(���߷�Ӣ����ĸ)Ҳ������ȥ
            strcat(cDesstr,pChNewLine);

            iCopyNum += (iWordLength + 1);//���Ѿ��������ַ����������ۼ�,�����ո�(���߷�Ӣ����ĸ)
            iRowNum++;
            iCopyWidth = 0;

            p += (iWordLength + 1);
        }
        else//ʣ�೤��С��һ��Ӣ�ĵ��ʵĿ��,ֱ����ӻ��з�
        {
            if ((iCopyNum == 0) || //һ���ַ�Ҳû�п���
                (iRectWidth - iCopyWidth == iRectWidth))//ʣ���Ⱦ��Ǿ��ε������
            {
                iCopyNumExt = 0;//��һ�����ʵĳ��ȳ�����������ȵ�ʱ��,�������ʵĸ����ļ�����
                //������ʵĳ����Ѿ������˾�������Ŀ��
                while (iCopyNumExt <= iWordLength)//��ҪΪ�������ǿ����ӻ��з���//iCopyNum
                {
                    if ((* p == NULL) || (p == NULL))
                        break;
                    if ((* p == 0xa) || (* p == 0xd))//����ӻ��з�
                    {
                        p++;
                        iCopyNum++;
                        iCopyNumExt++;
                        continue;
                    }
                    iWordWidth = GetCharactorWidth(hdc,p);
                    if (iRectWidth - iCopyWidth > iWordWidth)//ʣ�೤�ȴ���һ��Ӣ���ַ��Ŀ��
                    {
                        strncat(cDesstr,p,1);
                        iCopyNum++;
                        iCopyNumExt++;
                        p++;
                        iCopyWidth += iWordWidth;
                    }
                    else if (iRectWidth - iCopyWidth == iWordWidth)//ʣ�೤�ȵ���һ��Ӣ���ַ�
                    {
                        strncat(cDesstr,p,1);
                        iCopyNum++;
                        iCopyNumExt++;
                        p++;
                        strcat(cDesstr,pChNewLine);
                        iCopyWidth = 0;
                        iRowNum++;//������һ
                    }
                    else if (iRectWidth - iCopyWidth < iWordWidth)
                    {//ʣ�೤��С��һ��Ӣ���ַ��Ŀ��,��Ҫ��ӻ��з�,������һ
                        if (iCopyNum == 0)//��������Ŀ��С��һ���ַ��Ŀ��,ֱ�ӷ���0
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
* Purpose    ��õ����ַ����ߵ������ֵĿ��
* Params     
* Return     
* Remarks      
**********************************************************************/
static  int     GetCharactorWidth(HDC hdc,const char * ch)
{
    SIZE size;

    if ((ch == NULL) || (* ch == NULL))
        return 0;

    if ((unsigned char)(* ch) > pChinese)//��ǰ�ַ��Ǻ���
        GetTextExtent(hdc,ch,2,&size);
    else
        GetTextExtent(hdc,ch,1,&size);

    return (size.cx);
}
/********************************************************************
* Function   GetStringWidth  
* Purpose    ���һ���ַ����Ŀ��
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
* Purpose    ��õ����ַ��ĸ߶�
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
* Purpose    ��õ�ǰ�ַ���������
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
        if ((unsigned char)(* pch) > pChinese)//��ǰ�ַ��Ǻ���
            return 1;//����

        pch++;
    }
    return 0;//Ӣ��
}
///////////////////////////////���ϲ���Ϊ�Զ��������躯��--end
/********************************************************************
* Function   LoadMenuList  
* Purpose    ����HMENULIST
* Params     bLostkeyNotify:
            ���ϣ�������softkey��ʱ��,menulist�ղ�����Ϣ,��ô��ȡFALSE
            ���ϣ�������softkey��ʱ���,ֻ����softkey�յ���Ϣ,
            ��menulis�ղ�����Ϣ,��ô��ȡTRUE
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
* Purpose    ���¼������ݵ�Menulist��
* Params     **P:��Ҫ��ӵ�����,* id:�������ֶ�Ӧ����Ŀ��id,** pic:������Ŀ��Ӧ��ͼ���ļ���
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
* Purpose    ����һ�����ݵ�Menulist��
* Params     pItemName:��Ҫ��ӵ�����,id:���ֶ�Ӧ����Ŀ��id,pIconName:��Ŀ��Ӧ��ͼ���ļ���
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
* Purpose    ���MENULIST�е�ȫ������,*id��Ҫ��-1Ϊ������־
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
* Purpose    ����ؼ�������ĺ��� 
* Params     pConX:���ڷ��ؿؼ��ĺ�����,iControlW:�����ؼ��Ŀ�
            count:���ڼ���ؼ��ĸ���
* Return     
* Remarks    Ҫ������ؼ��ڽ����ϴ���������,���Ҹ����ؼ��ļ����һ�µ�
**********************************************************************/
BOOL    CountControlsAbscissa(int * pConX,unsigned int iControlW,size_t count)
{
    int i;
    int iControlNum;            //�ؼ�����
    int ixStep;                 //����ÿ���ؼ��ļ��
    unsigned int iScreenWidth;  //��Ļ��
    unsigned int iControlsWidth;//�ؼ���Ⱥ�

    if (pConX == NULL)
        return FALSE;

    iControlNum     = count / sizeof(int);
    iScreenWidth    = GetScreenUsableWH1(SM_CXSCREEN);
    iControlsWidth  = iControlNum * iControlW;
    if (iControlsWidth > iScreenWidth)
        return FALSE;

    ixStep = (iScreenWidth - iControlsWidth) / (iControlNum + 1);//����ÿ���ؼ��ļ��
    for (i = 0;i < iControlNum ; i++)
        * pConX++ = ixStep + i * (iControlW + ixStep);

    return TRUE;
}
/********************************************************************
* Function   CountControlsYaxis  
* Purpose    ����ؼ�������ĺ��� 
* Params     piConY:���ڷ��ؿؼ���������,iControlH:�����ؼ��ĸ�
            count:���ڼ���ؼ��ĸ���
* Return     
* Remarks    Ҫ������ؼ��ڽ����ϴ��ϵ�������,���Ҹ����ؼ��ļ����һ�µ�
**********************************************************************/
BOOL    CountControlsYaxis(int * piConY,unsigned int iControlH,size_t count)
{
    int i;
    int iControlNum;                //�ؼ�����
    int iyStep;                     //����ÿ���ؼ��ļ��
    unsigned int iControlsHeight;   //�ؼ��߶Ⱥ�
    unsigned int iScreenHeight;     //��Ļ��

    if (piConY == NULL)
        return FALSE;

    iControlNum     = count / sizeof(int);              //�ؼ�����
    iScreenHeight   = GetScreenUsableWH1(SM_CYSCREEN);  //��Ļ��
    iControlsHeight = iControlNum * iControlH;          //�ؼ��߶Ⱥ�

    if (iScreenHeight < iControlsHeight)//�ؼ��߶Ⱥʹ�����Ļ��,�޷����м���
        return FALSE;

    iyStep = (iScreenHeight - iControlsHeight) / (iControlNum + 1);//����ÿ���ؼ��ļ��
    for (i = 0;i < iControlNum;i++)
        * piConY++ = iyStep + i * (iControlH + iyStep);//����ؼ���������

    return TRUE;
}
/********************************************************************
* Function   CountControlYaxisExt  
* Purpose    ����ؼ�������ĺ��� 
* Params     piConY:���ڷ��ؿؼ���������,iControlH:�����ؼ��ĸ�
            count:���ڼ���ؼ��ĸ���
            iDirect:1��ʾҪ������Ϸ�����һ���Ŀռ�,��������ಿ�־������пؼ�
            iDirect:-1��ʾҪ������·�����һ���Ŀռ�,��������ಿ�־������пؼ�
            iSpace:��ʾ�����ռ�Ĵ�С
* Return     
* Remarks    Ҫ������ؼ��ڽ����ϴ��ϵ�������,���Ҹ����ؼ��ļ����һ�µ�
**********************************************************************/
BOOL    CountControslYaxisExt(int * piConY,unsigned int iControlH,size_t count,int iDirect,unsigned int iSpace)
{
    int i,iyStep,iControlNum;
    unsigned int iScreenHeight;     //��Ļ��
    unsigned int iControlsHeight;   //�ؼ��߶Ⱥ�

    if (piConY == NULL)
        return FALSE;

    iControlNum     = count / sizeof(int);              //�ؼ�����
    iScreenHeight   = GetScreenUsableWH1(SM_CYSCREEN);  //��Ļ��
    iControlsHeight = iSpace + iControlNum * iControlH; //�ؼ��߶Ⱥ�

    if (iScreenHeight < iControlsHeight)//�ؼ��߶Ⱥʹ�����Ļ��,�޷����м���
        return FALSE;

    iyStep = (iScreenHeight - iControlsHeight) / (iControlNum + 1);//����ÿ���ؼ��ļ��

    switch (iDirect)
    {
    case 1://��ʾҪ������Ϸ�����һ���Ŀռ�,��������ಿ�־������пؼ�
        for (i = 0;i < iControlNum;i++)
            * piConY++ = iyStep + i * (iControlH + iyStep) + iSpace;//����ؼ���������
        return TRUE;

    case -1://��ʾҪ������·�����һ���Ŀռ�,��������ಿ�־������пؼ�
        for (i = 0;i < iControlNum;i++)
            * piConY++ = iyStep + i * (iControlH + iyStep);//����ؼ���������
        return TRUE;

    default:
        return FALSE;
    }
}
//���ֿ����Զ����е���ʾ��
void    AppMessageBox(HWND hwnd,const char * szPrompt,const char * szCaption,int nTimeout)
{
    HDC     hdc;
    char  * pDesstr = NULL;
    int     ilen;
    const RECT rect = {60,81,148,118};//������ʾ����ʾ���ֵ�����

    if (szPrompt == NULL)
        return;
    if ((hwnd == NULL) || (strlen(szPrompt) == 0))
    {
        PLXTipsWin((char *)szPrompt,MESSAGEBOXTITLE,WAITTIMEOUT);//(char *)szCaption
        return;
    }
    ilen = strlen(szPrompt) * (strlen(pChNewLine) + 1) + 1;//���˵����,һ���ַ�(���ϻ��з�)ռһ��

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
////////////////////////////֧���Զ����е���ʾ����//////////////////////////////////////////////
BOOL    AppConfirmWin(HWND hwnd,const char * szPrompt,const char * szTipsPic,
                      const char * szCaption,const char * szOk,const char * szCancel)
{
    HDC     hdc;
    char  * pDesstr = NULL;
    int     ilen;
    BOOL    bres;
    const RECT rect = {58,75,155,125};//������ʾ����ʾ���ֵ�����

    if (szPrompt == NULL)
        return FALSE;

    if ((hwnd == NULL) || (strlen(szPrompt) == 0))
    {
        return (PLXConfirmWin(szPrompt,szTipsPic,szCaption,CONFIRM_OK,CONFIRM_CANCEL));
    }

    ilen = strlen(szPrompt) * (strlen(pChNewLine) + 1) + 1;//���˵����,һ���ַ�(���ϻ��з�)ռһ��

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
* Purpose    ��ǰ�ַ����Ƿ��ǺϷ��ķֻ���
* Params    pStr:�ַ���
* Return    ��ǰ�ַ����Ƿ��ǺϷ��ķֻ��ŷ���TRUE,���򷵻�FALSE
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
* Purpose    ��ǰ�ַ����Ƿ������ȫת��������
* Params    pStr:�ַ���
* Return    ��ǰ�ַ���������ȫת�������ַ���TRUE,���򷵻�FALSE
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
* Purpose    ��ǰ�ַ����Ƿ������ȫת����double����
* Params    pStr:�ַ���
* Return    ��ǰ�ַ���������ȫת�������ַ���TRUE,���򷵻�FALSE
* Remarks   
**********************************************************************/
BOOL    StrIsDouble(const char * pStr)
{
    char * p = (char *)pStr;
    int iPointNum = 0;//С����ĸ���

    if (strlen(pStr) == 0)
        return FALSE;

    if (* p == cCh_Point)//��һ���ַ���С����
        return FALSE;

    while (* p)
    {
        if (((* p < cCh_Num0) || (* p > cCh_Num9)) && (* p != cCh_Point))
            return FALSE;
        if (* p == cCh_Point)
            iPointNum++;//ͳ��С����ĸ���

        if (iPointNum > 1)//С�����������1
            return FALSE;
        p++;
    }

    p--;
    if (* p == cCh_Point)//���һ���ַ���С����
        return FALSE;

    return TRUE;
}
/********************************************************************
* Function   MB_Mtoa  
* Purpose   ������ת�����ַ���
* Params    dNumber:������,pNumber:ת�����
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
        strncat(pNumber,pBuffer,iDec);  //������������
        strcat(pNumber,pChPoint);       //����С����
        strcat(pNumber,pBuffer + iDec); //����ʣ�ಿ��
    }
    else
    {
        strcat(pNumber,pChNumer0);  //������������
        strcat(pNumber,pChPoint);   //����С����
        while (iDec++ < 0)          //����С����֮�����
            strcat(pNumber,pChNumer0);

        strcat(pNumber,pBuffer);    //����ʣ�ಿ��
    }*/
    return pNumber;
}
/********************************************************************
* Function   MB_atof  
* Purpose   �ַ���ת���ɸ����� 
* Params    pString:��Ҫת�����ַ���
* Return    ת�����
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
* Purpose    ���ַ����е�С�����λ�����������
* Params    pSour:���ַ����������ת����double����,iBit:С�������Ҫ�����λ��
* Return    
* Remarks   ��������double����С�����iBitλ�Ժ����������,��iBitΪ��,��С����֮�����������,����С����
**********************************************************************/
void    MB_DealDouble(char * pSour,unsigned int iBit)
{
    char * p;

    if (pSour == NULL)
        return;
    p = strstr(pSour,pChPoint);
    if (p == NULL)//û��С����,ֱ�ӷ���
        return;

    if (iBit == 0)
    {
        * p = cCh_0;
        return;
    }

    p++;//����С����
    if (strlen(p) > iBit)
    {
        p += iBit;
        * p = cCh_0;
    }
}
/********************************************************************
* Function   InStrArray  
* Purpose   ��ָ�������в����ַ���pSearchStr
* Params    pSearchStr:��Ҫ���ҵ��ַ���,pArray:ָ������
* Return    �ҵ����ظ��ַ�������λ��,���򷵻�-1
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
* Purpose   ��ָ�������в����ַ�ch
* Params    ch:��Ҫ���ҵ��ַ�,pArray:ָ������
* Return    �ҵ����ظ��ַ�����λ��,���򷵻�-1
* Remarks   
**********************************************************************/
int     InCharArray(char ch,const char * pArray,size_t count)
{
    int iArrayNum,i;
    char * p;

    p         = (char*)pArray;
    iArrayNum = count / sizeof(char);//�������

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
* Purpose   ��ָ�������в�������iSearchStr
* Params    iSearchStr:��Ҫ���ҵ�����,pArray:ָ������,count:�����С
* Return    �ҵ����ظ���������λ��,���򷵻�-1
* Remarks   
**********************************************************************/
int     InIntArray(int iSearchInt,const int * pArray,size_t count)
{
    int iArrayNum,i;
    int * p;

    p         = (int*)pArray;
    iArrayNum = count / sizeof(int);//�������

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
* Purpose    �Ƚ�ʱ��
* Params     pTime1:��һ��ʱ��,pTime2:�ڶ���ʱ��
* Return     <0:��һ��ʱ��С�ڵڶ���ʱ��,
            0:��һ��ʱ����ڵڶ���ʱ��
            >0:��һ��ʱ����ڵڶ���ʱ��
* Remarks     
**********************************************************************/
int     TimeCmp(const SYSTEMTIME * pTime1,const SYSTEMTIME * pTime2)
{
    if (pTime1->wYear < pTime2->wYear)
        return -1;
    if (pTime1->wYear > pTime2->wYear)
        return 1;
    //���ϱ�ʾ�����
    if (pTime1->wMonth < pTime2->wMonth)
        return -1;
    if (pTime1->wMonth > pTime2->wMonth)
        return 1;
    //���ϱ�ʾ�����
    if (pTime1->wDay < pTime2->wDay)
        return -1;
    if (pTime1->wDay > pTime2->wDay)
        return 1;
    //���ϱ�ʾ�����
    if (pTime1->wHour < pTime2->wHour)
        return -1;
    if (pTime1->wHour > pTime2->wHour)
        return 1;
    //���ϱ�ʾСʱ���
    if (pTime1->wMinute < pTime2->wMinute)
        return -1;
    if (pTime1->wMinute > pTime2->wMinute)
        return 1;
    //���ϱ�ʾ�������
    if (pTime1->wSecond < pTime2->wSecond)
        return -1;
    if (pTime1->wSecond > pTime2->wSecond)
        return 1;
    //ʱ�����
    return 0;
}
/********************************************************************
* Function   MBPlayMusic  
* Purpose   ������������
* Params    iMult:��ǰͨ������
* Return    
* Remarks   
**********************************************************************/
void    MBPlayMusic(int iMult)
{
    if (0 == iMult)//��ǰ��û��ͨ��
    {
        Call_PlayMusic(RING_PHONE,0);
        return;
    }
    PlayClewMusic(1);//��ǰ�Ѿ����˸�ͨ��,��Ҫ������ʾ��
}
//////////////////////////////////////////////////////////////////////////
BOOL    EarphoneStatus(void)//�Ƿ��ж���
{
    return (GetEarphone());
}
/********************************************************************
* Function   PlayClewMusic  
* Purpose   ������ʾ��
* Params    iPlayCount:���Ŵ���
* Return    
* Remarks   
**********************************************************************/
void    PlayClewMusic(unsigned int iPlayCount)//������ʾ��
{
    //Call_PlayMusic(RING_TOUCH,1);
}
/********************************************************************
* Function   GetPhoneItem  
* Purpose   Ϊͨ����Ϣ�ṹ�е�����,���븳ֵ
* Params    pPhoneItem:ͨ����Ϣ�ṹ.pNumber:�绰����
* Return    �õ绰�Ƿ�����ϵ���ж���
* Remarks  
**********************************************************************/
BOOL    GetPhoneItem(PHONEITEM * pPhoneItem,const char * pNumber)
{
    int     iSour;
    PBINFO  pbInfo;//�ӵ绰���ж�ȡ��Ϣ�Ľṹ

    memset(&pbInfo,0x00,sizeof(PBINFO));

    if (MB_GetInfoFromPhonebook(pNumber,&pbInfo))//��������
    {
        iSour = strlen(pbInfo.Name);
        if (iSour > 0)  //�õ绰����İ����,��������
        {
            strncpy(pPhoneItem->cName,pbInfo.Name,
                sizeof(pPhoneItem->cName) > iSour ? iSour + 1 : sizeof(pPhoneItem->cName) - 1);
        }
        else            //�õ绰����İ����,���������ǿ�,������λ�����
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
* Purpose    ��ǰ�ַ����Ƿ��ǺϷ��ĵ绰����
* Params    pPhoneNumber:�绰����
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

    if ((cCh_ExtP == * p) || (cCh_ExtW == * p))//��һλ����p������w
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
* Purpose      �绰�������Ƿ�����ֻ���
* Params       pPhoneNumber:ԭ�绰����,pPureNumber:�������ֻ��ŵĻ�,���ش��绰����
                pExtension:�������ֻ��ŵĻ�,���طֻ���
* Return     �绰�����а����ֻ��ŷ���TRUE,���򷵻�FALSE
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
        if ((cCh_ExtP == * p) || (cCh_ExtW == * p))//�зֻ���
        {
            dtmf_type = (cCh_ExtP == * p) ? DTMF_PAUSE : DTMF_WAIT;

            if (pPureNumber != NULL)//���ش��绰����
            {
                iPureNumberLen = abs(pPhoneNumber - p);
                strncpy(pPureNumber,pPhoneNumber,iPureNumberLen);//�����ֻ���֮ǰ�ĺ���
                pEnd   = pPureNumber + iPureNumberLen;
                * pEnd = cCh_0;
            }
            if (pExtension != NULL)//���طֻ���
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
* Purpose    ��ǰ�����Ƿ�����绰����
* Params    pNumber:��ǰ����,
            bPureNumber:TRUE��ʾ����ĺ���(pNumber)�Ǵ��绰����,
            FALSE��ʾ����ĺ���(pNumber)���ܰ���GSMǰ׺�ͷֻ���
* Return     ��ǰ�����ǽ����绰���뷵��TRUE;���򷵻�FALSE
* Remarks   
**********************************************************************/
BOOL    NumberIsEmergent(const char * pNumber,BOOL bPureNumber)
{
    int iGsmSpecific;
    char cPureNumber[PHONENUMMAXLEN + 1] = "";

    if (bPureNumber)
        return (InStrArray(pNumber,pEmergentNumber) != -1);

    iGsmSpecific = GsmSpecific(pNumber);//���ú����Ƿ����GSMǰ׺

    if (DTMF_NONE != IfExistExtension(pNumber,cPureNumber,NULL))//��ǰ�����а����ֻ���
        return (InStrArray(cPureNumber + iGsmSpecific,pEmergentNumber) != -1);
    else
        return (InStrArray(pNumber     + iGsmSpecific,pEmergentNumber) != -1);
}
/********************************************************************
* Function   GetIntArrayMaxValue  
* Purpose    �������������Ԫ�ص����ֵ
* Params    pArray:�����ַ,count:�����С
* Return    
* Remarks   
**********************************************************************/
int     GetIntArrayMaxValue(int * pArray,size_t count)
{
    int iArrayNum,i,iMaxValue = 0;
    int * p = NULL;

    p         = (int*)pArray;
    iArrayNum = count / sizeof(int);//�������

    for (i = 0;i < iArrayNum;i++)
    {
        if (* p > iMaxValue)
            iMaxValue = * p;
        p++;
    }
    return iMaxValue;
}
//////////////////////////////////////////////////////////////////////////
#define MBWAIT_TITLE        MBGetString(STR_MBPHONE_MBWAIT_TITLE)//"��������"
#define MBWAIT_STATE        MBGetString(STR_MBPHONE_WAIT)        //"���Ե�"
#define IDC_BUTTON_QUIT     3//�˳���ť
#define IDC_BUTTON_SET      4//ȷ��
#define IDC_EDIT            5

static const char * pMBWaitClassName = "MBWaitWindow";
static const char * pMBWaitPic       = SETUPPIC_NETSETTING_WAIT;
static const int    iPicY            = 10;//ͼƬ������

static HWND         hParent;
static HWND         hMBWaitWindow = NULL;
static int          iCallBackMsg;//������÷��������Ϣ
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

    if (bRButton)//��Ӧ�Ҽ��˳�
    {
        hParent      = hWnd;
        iCallBackMsg = icallmsg;//������Ϣֵ,�˳���ʱ�򷢸����÷�һ����Ϣ
        SendMessage(hMBWaitWindow,PWM_CREATECAPTIONBUTTON,
            (WPARAM)IDC_BUTTON_QUIT,(LPARAM)MBGetString(STR_WINDOW_EXIT));
    }
    else
    {
        hParent      = NULL;
        iCallBackMsg = -1;//��ʾ����Ӧ�Ҽ��˳�
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

        if ((size.cx == 0) || (size.cy == 0))//��һ��ִ�е�ʱ����Ҫ����ļ��ߴ�
            GetImageDimensionFromFile(pMBWaitPic,&size);//����ļ��ߴ�

        hgifanimate = StartAnimatedGIFFromFile(hWnd,pMBWaitPic,
            (isw - size.cx) / 2,iPicY,DM_NONE);

        SetRect(&rectState,0,size.cy + iPicY,isw,size.cy + iPicY + iFontHeight * 2);
        break;

    case WM_TODESKTOP:
        return TRUE;

    case WM_DESTROY ://֪ͨ���������,�˳����˳�;
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
            if (iCallBackMsg != -1)//��Ӧ�Ҽ��˳�
            {
                if (hParent != NULL)
                    PostMessage(hParent,iCallBackMsg,0,0);//�����÷�������Ϣ

                DestroyWindow(hWnd);
            }
            break;
        }
        break;

    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        case IDC_BUTTON_QUIT://�˳�
            if (iCallBackMsg != -1)//��Ӧ�Ҽ��˳�
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
* Purpose   ��ʾ��ʾ��
* Params    hWnd:���÷����,bState:�Ƿ���ʾ��ʾ��,
            icallmsg:���÷��ṩ����Ϣֵ(bState=TURE��ʱ����Ч),
            bButton:�Ƿ���Ӧ�Ҽ��˳�(bState=TURE��ʱ����Ч)
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
static const char * pReplyControlClassName = "MBReplyControlWndClass";//��������
static HINSTANCE    hInstanceReply;
static char       * pReplyResult;

static LRESULT ReplyControlAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
/********************************************************************
* Function   CallReplyControlWindow  
* Purpose   �������ݵĿؼ�
* Params    hwndCall:���÷����,pTitle:���ڱ���,iEditMaxLen:������󳤶�,
            pResult:���ؽ��,dwFlags:�ؼ����(��ʱû��)
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

    case WM_ACTIVATE://ȥ�������Ϣ�������Ҽ������˳��������˳�ʧ��
        if (WA_INACTIVE == LOWORD(wParam))
            SetFocus(GetDlgItem(hWnd,IDC_EDIT));//��������ý���

        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    case WM_CLOSE: //������Ͻǵ��˳���ťʱΪ�����˳�;
        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_BUTTON_QUIT, 0 );   
        break;

    case WM_DESTROY ://֪ͨ���������,�˳����˳�;
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
        case IDC_BUTTON_SET://����
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
        case IDC_BUTTON_QUIT://�˳�
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
* Purpose   ����ϵ�˴�������ݵĺ���
* Params    
* Return    
* Remarks   ���Ƕ����ȡ��Ϣ�Ľӿں���,������ʱ���,���߸��ĺ�����,�������ٴ�绰�������ֵĸ���
**********************************************************************/
BOOL    MB_GetInfoFromPhonebook(const char * pPhone, PBINFO * pInfo)
{
    int isour;
    BOOL bRes;

    bRes = APP_GetNameByPhone(pPhone,pInfo);
    if (bRes)
    {
        if (strlen(pInfo->Name) == 0)//�������Ϊ�յĻ�,���绰��������������λ��
        {
            isour = strlen(pPhone);
            strncpy(pInfo->Name,pPhone,sizeof(pInfo->Name) > isour ? isour + 1 : sizeof(pInfo->Name) - 1);
        }
    }
    return bRes;
}
/********************************************************************
* Function   MBGetDirectDial  
* Purpose   ����ϵ�˴���ÿ��ٲ����������ݵĺ���
* Params    
* Return    -1:û���������,0,��λ��û�м�¼,1��λ���м�¼pNo���ص绰����
* Remarks   ���Ƕ����ȡ��Ϣ�Ľӿں���,������ʱ���,���߸��ĺ�����,�������ٴ�绰�������ֵĸ���
**********************************************************************/
int     MB_GetDirectDial(WORD wDDIdx, char * pNo)
{
    return (APP_GetDirectDial(wDDIdx,pNo) ? 1 : 0);
}
/********************************************************************
* Function   MB_GetSIMRcdByOder  
* Purpose   ��������ţ��ǿռ�ţ�ȡSIM����¼
* Params    
* Return    -1:û���������,0,��λ��û�м�¼,1��λ���м�¼pNo���ص绰����
* Remarks   ���Ƕ����ȡ��Ϣ�Ľӿں���,������ʱ���,���߸��ĺ�����,�������ٴ�绰�������ֵĸ���
**********************************************************************/
int     MB_GetSIMRcdByOder(int iOrder,PPHONEBOOK pRtnRcd)
{
    return (APP_GetSIMRcdByOder((short)iOrder,pRtnRcd,1) == 1 ? 1 : 0);
}
/********************************************************************
* Function   MB_SaveToPhoneBook  
* Purpose   ����绰���뵽��ϵ��
* Params    hWnd:���÷����,pNumber�绰����
* Return    
* Remarks   ���Ƕ����ȡ��Ϣ�Ľӿں���,������ʱ���,���߸��ĺ�����,�������ٴ�绰�������ֵĸ���
**********************************************************************/
BOOL    MB_SaveToPhoneBook(HWND hWnd,const char * pNumber)
{
    return APP_SaveToPhoneBook(hWnd,pNumber);
}
/********************************************************************
* Function   MB_SendSms  
* Purpose   ���Ͷ���
* Params    pNumber�绰����
* Return    
* Remarks   ���Ƕ����ȡ��Ϣ�Ľӿں���,������ʱ���,���߸��ĺ�����,�������ٴ�绰�������ֵĸ���
**********************************************************************/
void    MB_SendSms(const char * pNumber)
{
    APP_EditSMS(pNumber, NULL);
}
/********************************************************************
* Function   MB_SendMms  
* Purpose   ���Ͳ���
* Params   pNumber�绰����
* Return    
* Remarks   ���Ƕ����ȡ��Ϣ�Ľӿں���,������ʱ���,���߸��ĺ�����,�������ٴ�绰�������ֵĸ���
**********************************************************************/
void    MB_SendMms(const char * pNumber)
{
    CallMMSEditWnd("",NULL,0, MMS_CALLEDIT_MOBIL, (char*)pNumber);
}
//ͨ��֮ǰ,֮����Ҫִ�еĲ���
static  BOOL        bScreen;
static  DWORD       bLcdState;
//static  HWAVEOUT    hWaveOut = NULL;
void    DisableState(void)//ͨ��֮ǰִ�еĲ���
{
    bScreen   = DlmNotify(PMM_NEWS_ENABLE_SCRSAVE,FALSE);//��ֹ����
    bLcdState = DlmNotify(PMM_NEWS_GET_FLAGS,0);

    DlmNotify(PMM_NEWS_DISABLE, PMF_AUTO_SHUT_DOWN |
                                PMF_AUTO_SHUT_MAIN_LCD |
                                PMF_AUTO_SHUT_MAIN_LIGHT |
                                PMF_AUTO_SHUT_CPU);

    DlmNotify(PMM_NEWS_DISABLE,PMF_AUTO_SHUT_DOWN);//��ֹ�Զ��ػ�

    SetupHf_SetNotSleep();//��ֹ˯��
#ifndef _EMULATE_
    //waveOutPauseAll(&hWaveOut);
#endif

    PhoneSetChannel(TRUE);//�л�ͨ��
}
void    EnableState(void)//ͨ��֮��ִ�еĲ���
{
    if (bScreen)
        DlmNotify(PMM_NEWS_ENABLE_SCRSAVE,TRUE);//�ָ�����

    DlmNotify(PMM_NEWS_ENABLE,bLcdState);

    SetupHf_ClearSleepFlag();//����˯��
#ifndef _EMULATE_
    //waveOutRestartAll(&hWaveOut);
#endif

    PhoneSetChannel(FALSE);//�л�ͨ��
    //hWaveOut = NULL;
}

void    PhoneSetChannel(BOOL bSet)//����ͨ��
{    
#ifndef _EMULATE_
    //printf("mb SetChannel:%d\r\n",bSet);
    //waveInSetChannel (NULL,bSet,WAVE_CHANNEL_ONLINE);//����ͨ��
    //waveOutSetChannel(NULL,bSet,WAVE_CHANNEL_ONLINE);//����ͨ��
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
* Purpose   ����ĳ����Ҫ��¼���ַ������ļ���
* Params    pString:��������
* Return    
* Remarks   
**********************************************************************/
void    SaveLog(const char * pString)
{
    HANDLE hf;

    if (pString == NULL)
        return;
    if(INVALID_HANDLE_VALUE == (hf = CreateFile(pLogFileName, ACCESS_WRITE|ACCESS_READ, 0)))
    {//δ�ҵ��ļ�������һ���ļ�
        if (INVALID_HANDLE_VALUE == 
            (hf = CreateFile(pLogFileName, ACCESS_WRITE|ACCESS_READ|CREATE_NEW|CREATE_OVERWRITE, 0)))
        {//�����ļ�ʧ��
            CloseFile(hf);
            return;
        }
    }
    if (-1 == SetFilePointer(hf,0L,SEEK_END))//�����ļ�ָ��
    {
        CloseFile(hf);
        return;
    }
    WriteFile(hf,(PVOID)pString,strlen(pString));
    CloseFile(hf);
}
/********************************************************************
* Function   ReadLog  
* Purpose   ��ȡ������ַ�������
* Params    pString:���ر�������
* Return    
* Remarks   
**********************************************************************/
DWORD   ReadLog(char * pString)
{
    HANDLE  hf;
    DWORD   dFileSize;

    if(INVALID_HANDLE_VALUE == (hf = CreateFile(pLogFileName, ACCESS_WRITE|ACCESS_READ, 0)))
    {//δ�ҵ��ļ�������һ���ļ�
        if (INVALID_HANDLE_VALUE == 
            (hf = CreateFile(pLogFileName, ACCESS_WRITE|ACCESS_READ|CREATE_NEW|CREATE_OVERWRITE, 0)))
        {//�����ļ�ʧ��
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
    if (-1 == SetFilePointer(hf,0,SEEK_CUR))//�����ļ�ָ��
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
* Purpose   ɾ��log�ļ�
* Params    
* Return    
* Remarks   
**********************************************************************/
void    RemoveLog(void)
{
    DeleteFile(pLogFileName);
}
