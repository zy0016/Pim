/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : ��ӵ绰����
 *
 * Purpose  : ͨ������
 *            
\**************************************************************************/
#include "MBCalling.h"

#define DEBUG_MBCALLING

static const int    iMaxCount   = MAX_COUNT;
static const int    iPortraitX  = 3;//ͷ�����������
static const int    iPortraitY  = 3;//ͷ������������
static const int    iPortraitW  = 80;//ͷ�����������
static const int    iPortraitH  = 80;//ͷ���������߶�
static const int    iDTMFEditX  = 10;
static const int    iDTMFEditY  = 10;

static const double dZero       = 0.0;
static const int    iSecond60   = 60;
static const char   ch_End      = '\0';
static const char * pClassName  = "MBCallingWndClass";//��������
static const char * pDTMFClass  = "MBCallingDTMFWndClass";
static const char * pNum0       = "0";
static const char * pPlus       = "+";
static const char * pAsterisk   = "*";
static const char * pWell       = "#";
static const char * pTime       = "%02d:%02d:%02d";
static const char * pTimeEnd    = "%s:%02d:%02d:%02d";
static const char * pCostInfo1  = "%s:%f";
static const char * pCostInfo2  = "%ld";
static const char   ch_Plus     = '+';
#if defined(WAVECOM_2C)
static const char * pAbortAT    = "AT+WAC\r";
static const int    iAbortValue = 515;
static const int    iTimeOut    = 4000;
#endif

static MBCALLTYPE   CallingType;                    //��ǰͨ����ʽ
static INTERFACE    InterfaceType = INTERFACE_NONE; //��ǰ��������

static char         cPhoneNumberExtension[PHONENUMMAXLEN + 1];//����ֻ���
static DTMF_TYPE    Dtmf_Type;              //�Ƿ��зֻ���

static HWND         hCalling;               //ͨ��������
static HWND         hMBList;                //�෽ͨ�������б�ؼ�
static PHONEITEM    PhoneItem[MAX_COUNT];   //�绰��Ϣ
static int          PhoneGroupNum = 0;      //�绰��·�ĸ���
static HINSTANCE    hInstance;

static BOOL         bSelectSingleConnect;   //�Ƿ�ѡ���˵���ͨ���İ�ť
static BOOL         bListCurCalling;        //�Ƿ������е�ǰͨ��(TRUE�����е�ǰͨ��,FALSEû���е�ǰͨ��)
/*�����ڵ�ǰͨ��ȫΪ���ֵ�ʱ��,���ͷֻ��Żᵼ�³���,������û�м���ͨ����ʱ����Ҫ
���ⷢ�ͷֻ���.���ͷֻ���֮ǰҪ��⵱ǰ����ͨ����״̬,ȫ��Ϊ���ֵ�ʱ���ܷ��ͷֻ���
��ͨ�������ı��ʱ��,��Ҫ�����е�ǰͨ��,ֻ�е��е�ǰͨ���Ĳ������֮��ſ��Է��ͷֻ���.
*/
static BOOL         bCanListCurCalling;     //�Ƿ������IDC_TIMER_ERROR��Ϣ���е�ǰͨ��
static OPERATETYPE  ListCurCallsType;       //�е�ǰͨ��֮ǰ�Ĳ���
static WPARAM       ListCurCallswParam;     //�е�ǰͨ��֮ǰ��at����ص���Ϣ��wparam����
static WPARAM       ListCurCallslParam;     //�е�ǰͨ��֮ǰ��at����ص���Ϣ��lparam����

static BOOL         bMute = FALSE;          //��ǰ�Ƿ����ھ���
static BOOL         bHolding = FALSE;       //�Ƿ����ڱ�Hold
#if defined(MBCALLING_RECORD)
static HWND         hBRecord;               //���水ť���
static BOOL         bRecord = FALSE;        //��ǰ�Ƿ�����¼��
#endif
//////////////////////////ͨ����������//////////////////////
static RECT         rName;                  //����
static RECT         rPhoneCode;             //����
static RECT         rTime;                  //ͨ��ʱ��
static RECT         rPhoto;                 //ͼƬ����
static RECT         rCostInfo;              //��ʾͨ�����õ�����
static int          iCallingOneNum;         //����ͨ����ʱ��,ͨ����Ϣλ�ڽṹ�����е�λ��
static HBITMAP      hPortrait = NULL;       //����ͨ��������ͷ��
static SIZE         sPortrait;              //����ͨ��������ͷ��ĳߴ�
static MBConfig     MBCongig_Calling;       //ͨ��������Ϣ
static int          iTime_Second;           //ͨ����ʱ,���ڲ���50����ʾ��
#if defined(MBCALLING_TIMEACCURATE)
static int          iInitSecond;            //Э��ͨ����ʱ(��)�ı���,���ڱ�֤��ʾ��ʱ׼ȷ
#endif
static char         cName[PHONENUMMAXLEN + 1];//������ʾ��������
static char         cPhone[PHONENUMMAXLEN + 1];//������ʾ�������
//****************���ڼƷ���Ϣ************************************
static BOOL         bCostLimit = FALSE;     //��ǰͨ���Ƿ��ѵ���������
static BOOL         bAOCInfo;               //�Ƿ���ʾͨ���Ʒ���Ϣ
static BOOL         bCanGetCurrentCCM;      //�Ƿ���Է�����Ϣ��ȡ��ǰ�Ʒ���Ϣ
static DWORD        dCostInfo;              //��ǰCall Meterֵ
static ME_PUCINFO   me_pucinfo;             //�Ƽ���Ϣ
typedef enum                                //ͨ���Ʒѵ���ʾ����
{
    COSTTYPE_UNIT,                          //����λ��ʾ
    COSTTYPE_PRICE                          //��������ʾ
}COSTTYPE;
static COSTTYPE     CostType;               //ͨ���Ʒ���ʾ��ʽ
static double       dCost;                  //ͨ���Ʒѵĵ�����Ϣ
/********************************************************************
* Function     MBCallingWindow
* Purpose      ����ͨ������
* Params       NewItemInfo:�����ĵ绰��Ϣ,pExtension:�ֻ���,dtmf_type:�ֻ������
* Return           
* Remarks      �������ɽ���/�������ܵ���
**********************************************************************/
BOOL    MBCallingWindow(const PHONEITEM * pPhoneItem,const char * pExtension,
                        DTMF_TYPE dtmf_type)
{
    int iFirstNotUsedItem;  //�õ�һ��δ��ITEM
    int nUsedItemNum;       //������Item����

    if ((1 == PhoneGroupNum) || (0 == PhoneGroupNum))
    {
        iFirstNotUsedItem = GetFirstNotUsedItem();//�õ�һ��δ��ITEM
        if (-1 == iFirstNotUsedItem)
            return FALSE;

        SetAllType(CALL_STAT_HELD); //ȫ����Ϊָ������:����״̬

        memcpy(&PhoneItem[iFirstNotUsedItem],pPhoneItem,sizeof(PHONEITEM));
        PhoneItem[iFirstNotUsedItem].ItemStat = ITEMUSED;//��־Ϊ�Ѿ�ʹ��

        SetPhoneGroupNum();//���õ绰��·��

        nUsedItemNum = GetUsedItemNum();//������Item����

        if (1 == nUsedItemNum)//����ͨ��
            CallingType = MBCALLINGONE;
        else if ((1 < nUsedItemNum) && (nUsedItemNum <= iMaxCount))//�෽ͨ��
            CallingType = MBCALLINGMULT;
        else
            return FALSE;

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("\nMB MBCallingWindow,CallingType:%d,����:%s\n",
            CallingType,PhoneItem[iFirstNotUsedItem].Calls_Info.PhoneNum);
#endif
        Dtmf_Type = dtmf_type;
        if (pExtension != NULL)
        {
            if (strlen(pExtension) == 0)
                Dtmf_Type = DTMF_NONE;                      //û�зֻ���
            else
                strcpy(cPhoneNumberExtension,pExtension);   //��¼�ֻ���
        }
        else
        {
            Dtmf_Type = DTMF_NONE;                          //û�зֻ���
        }
        return (MBTalkingWindow());
    }
    return FALSE;
}
/********************************************************************
* Function   MessageMBCallingOne  
* Purpose    �򵥷�ͨ�����淢����Ϣ
* Params    
* Return    
* Remarks   
**********************************************************************/
void    MessageMBCallingOne(UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    if ((hCalling != NULL) && (IsWindow(hCalling)))
        PostMessage(hCalling,wMsgCmd,wParam,lParam);
}
/********************************************************************
* Function   IsWindow_MBCalling  
* Purpose    ��ǰ�Ƿ����ͨ������ 
* Params     
* Return     
* Remarks    
**********************************************************************/
BOOL    IsWindow_MBCalling(void)
{
    if (hCalling == NULL)
        return FALSE;
    return (IsWindow(hCalling));
}
/********************************************************************
* Function     SetListCurCalls
* Purpose      �����Ƿ������е�ǰͨ��
* Params       
* Return      �� 
* Remarks     
**********************************************************************/
void    SetListCurCalls(BOOL bListCur)
{
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling �����Ƿ������е�ǰͨ��:%d\r\n",bListCur);
#endif
    bListCurCalling = bListCur;
}
/********************************************************************
* Function     GetListCurCalling
* Purpose      ����Ƿ������е�ǰͨ��
* Params       
* Return      
* Remarks     
**********************************************************************/
BOOL    GetListCurCalling(void)
{
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    if (bListCurCalling)
        printf("mb MBCalling �����е�ǰͨ��\r\n");
    else
        printf("mb MBCalling û���е�ǰͨ��\r\n");
#endif
    return bListCurCalling;
}
/********************************************************************
* Function     ReadMobileConfig
* Purpose      ��ȡͨ�����õ���Ϣ
* Params       
* Return      �� 
* Remarks     ��һ�ζ�ȡ��ʱ����ļ��ж�ȡ,�Ժ���ڴ��ж�ȡ
**********************************************************************/
void    ReadMobileConfig(MBConfig * pConfig)
{
    if (NULL == hCalling)//��δ��ʾͨ������
    {
        iTime_Second = 0;//ͨ����ʱ,���ڲ���50����ʾ��
        ReadMobileConfigFile(&MBCongig_Calling);
    }
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling ReadMobileConfig,MBCongig_Calling.b50Tip:%d\r\n",MBCongig_Calling.b50Tip);
#endif
    memcpy(pConfig,&MBCongig_Calling,sizeof(MBConfig));
}
/********************************************************************
* Function     GetFirstNotUsedItem
* Purpose      �õ�һ��δ��ITEM
* Params       
* Return           
* Remarks      
**********************************************************************/
static  int     GetFirstNotUsedItem(void)
{
    int i;
    for (i = 0;i < iMaxCount;i++)
    {
        if (ITEMNOTUSED == PhoneItem[i].ItemStat)
            return i;
    }
    return -1;
}
/********************************************************************
* Function     GetUsedItemNum
* Purpose      ������Item����(�����жϵ�ǰ�Ƿ��Ƕ෽ͨ��)
* Params       
* Return     0:û��ͨ��,1:����ͨ��,�������1�෽ͨ��       
* Remarks      
**********************************************************************/
int     GetUsedItemNum(void)
{
    int nRet = 0,i;
    for(i = 0;i < iMaxCount;i++)
    {
        if ((ITEMUSED == PhoneItem[i].ItemStat) && (0 != PhoneItem[i].Calls_Info.index))
            nRet++;
    }
    return nRet;
}
/********************************************************************
* Function     GetItemTypeNum
* Purpose      �����ü�����߱��ֵ�Item����
* Params       
* Return     -1:iType�Ƿ�
* Remarks      
**********************************************************************/
static  int     GetItemTypeNum(int iType)
{
    int nRet = 0,i;
    if ((CALL_STAT_ACTIVE != iType) && (CALL_STAT_HELD != iType))
        return -1;

    for (i = 0;i < iMaxCount;i++)
    {
        if ((ITEMUSED == PhoneItem[i].ItemStat) &&
            (0 != PhoneItem[i].Calls_Info.index) &&
            (iType == PhoneItem[i].Calls_Info.Stat))
            nRet++;
    }
    return nRet;
}
/********************************************************************
* Function     GetTheNUsedItem
* Purpose      �õ�n��ʹ�õļ�¼
* Params       
* Return           
* Remarks      
**********************************************************************/
static  int     GetTheNUsedItem(int nTheN)
{
    int i;
    if (nTheN <= 0)
        return -1;

    for (i = 0 ;i < iMaxCount;i++)
    {
        if ((ITEMUSED == PhoneItem[i].ItemStat) && (0 != PhoneItem[i].Calls_Info.index))
        {
            nTheN--;
            if (0 == nTheN)
                return i;
        }
    }
    return -1;
}
/********************************************************************
* Function     SetPhoneGroupNum
* Purpose      ���õ绰��·�ĸ���
* Params       
* Return       
* Remarks      
**********************************************************************/
static  void    SetPhoneGroupNum(void)
{
    int iHeldNum,iActiveNum;

    iHeldNum   = GetItemTypeNum(CALL_STAT_HELD);
    iActiveNum = GetItemTypeNum(CALL_STAT_ACTIVE);

    if ((iHeldNum > 0) && (iActiveNum > 0))
        PhoneGroupNum = 2;
    else if ((iHeldNum == 0) && (iActiveNum > 0) || (iHeldNum > 0) && (iActiveNum == 0))
        PhoneGroupNum = 1;
    else 
        PhoneGroupNum = 0;
}
/********************************************************************
* Function     GetPhoneGroupNum
* Purpose      ��õ绰��·�ĸ���
* Params       
* Return           
* Remarks      
**********************************************************************/
int     GetPhoneGroupNum(void)
{
    SetPhoneGroupNum();
    return (PhoneGroupNum);
}
/********************************************************************
* Function     ExecuteHangup
* Purpose      ����Ҷϼ��Ĳ���
* Params       
* Return       
* Remarks      
**********************************************************************/
static  void    ExecuteHangup(void)
{
    int iHeldNum,iActiveNum;

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("\nMB MBCalling,�Ҷϼ�,CallingType:%d\n",CallingType);
#endif
    switch (CallingType)
    {
    case MBCALLINNONE://��ʾͨ����������ʱ���˳�
        DestroyWindow(hCalling);
        break;

    case MBCALLINGONE://����ͨ��
        if (-1 == ME_SwitchCallStatus(hCalling,WM_VOICE_HANGUPONE,CUT_ALL,0))
            AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
        break;

    case MBCALLINGMULT://�෽ͨ��(�Ҷϼ����ͨ��)
        SetPhoneGroupNum();
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("MB MBCalling,�Ҷϼ� PhoneGroupNum:%d\r\n",PhoneGroupNum);
#endif
        if (PhoneGroupNum == 2)//�������·����END���Ҷϼ����
        {
            if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_CUT_ACTIVED,CUT_ACTIVED,0))
                AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
        }
        else if (PhoneGroupNum == 1)//�����һ·�����۱��ֻ��Ǽ��� ��END�����ǹҶ�
        {
            iHeldNum   = GetItemTypeNum(CALL_STAT_HELD);
            iActiveNum = GetItemTypeNum(CALL_STAT_ACTIVE);
            if ((iHeldNum == 0) && (iActiveNum > 0))//ֻ�м����ͨ��
            {
                if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_CUT_ACTIVED,CUT_ACTIVED,0))
                    AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
            }
            else//ֻ�б��ֵ�ͨ��
            {
                if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_CUT_HELD,CUT_HELD,0))
                    AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
            }
        }
        else//������
        {
            MBListCurCalls(LISTCURCALLS_REPEAT,-1,-1);//�е�ǰͨ��
        }
        break;
    }
}
/********************************************************************
* Function     AfterHangup_Single
* Purpose      ����ͨ���еĹҶ�֮��Ĵ���
* Params       
* Return       
* Remarks      ��ʱ��ͨ���϶��Ѿ��Ҷ���
**********************************************************************/
static  void    AfterHangup_Single(void)
{
    char                cLastTime[10] = "";//��ʾͨ����ʱ��������
    char                cCostInfo[50] = "";
    MBRecordCostTime    CurCostPara;//����ͨ����ʱ

    if (PhoneItem[iCallingOneNum].ItemStat != ITEMUSED)
    {
        MBCallEndWindow(CALLEND,NULL,NULL,NULL);//����ͨ����������
        DestroyWindow(hCalling);
        return;
    }

    ReadRecord_Cost(&CurCostPara);          //��õ���ͨ����ʱ��Ϣ
    RemTime(&CurCostPara,iCallingOneNum);   //����ͨ��ʱ��
    SaveRecord_Cost(&CurCostPara);          //��¼����ͨ����ʱ�䵽ͨ����ʱ����
    if (PhoneItem[iCallingOneNum].Calls_Info.Dir == CALL_DIR_ORIGINATED)
    {   //����ͨ����¼,��Ҫ���쵱ǰ�Ƿ����,�����ն�
        SaveMBRecordUsedTime(DIALED_REC,
            PhoneItem[iCallingOneNum].Calls_Info.PhoneNum,
            &PhoneItem[iCallingOneNum].TimeInDial,&PhoneItem[iCallingOneNum].TimeUsed);
    }
    else if (PhoneItem[iCallingOneNum].Calls_Info.Dir == CALL_DIR_TERMINATED)
    {
        SaveMBRecordUsedTime(ANSWERED_REC,
            PhoneItem[iCallingOneNum].Calls_Info.PhoneNum,
            &PhoneItem[iCallingOneNum].TimeInDial,&PhoneItem[iCallingOneNum].TimeUsed);
    }
    PhoneItem[iCallingOneNum].ItemStat = ITEMNOTUSED;

    sprintf(cLastTime,pTime,
        PhoneItem[iCallingOneNum].TimeUsed.wHour,
        PhoneItem[iCallingOneNum].TimeUsed.wMinute,
        PhoneItem[iCallingOneNum].TimeUsed.wSecond);

    if (bAOCInfo)//��ʾͨ���Ʒ���Ϣ
    {
        if (bCostLimit)//���õ�ǰͨ���ѵ���������
        {
            strcpy(cCostInfo,DIALLING_FARE);
        }
        else
        {
            switch (CostType)
            {
            case COSTTYPE_UNIT://��λ
                sprintf(cCostInfo,pCostInfo2,dCostInfo);//"%ld"
                break;
            case COSTTYPE_PRICE://����
                sprintf(cCostInfo,pCostInfo1,me_pucinfo.currency,dCostInfo * dCost);//"%s:%f"
                MB_DealDouble(cCostInfo,3);
                break;
            }
        }
        MBCallEndWindow(PhoneItem[iCallingOneNum].cName,//����ͨ����������,����Ʒ���Ϣ
            PhoneItem[iCallingOneNum].Calls_Info.PhoneNum,cLastTime,cCostInfo);
    }
    else
    {
        MBCallEndWindow(PhoneItem[iCallingOneNum].cName,//����ͨ����������
            PhoneItem[iCallingOneNum].Calls_Info.PhoneNum,cLastTime,NULL);
    }
    memset(&PhoneItem[iCallingOneNum],0x00,sizeof(PHONEITEM));
    DestroyWindow(hCalling);
}
/********************************************************************
* Function     AfterHangup_MultOne
* Purpose      �෽ͨ���еĹҶϴ���
* Params       iIndex:�ṹ�����е�λ��
* Return       -1:����,1,����
* Remarks      �෽ͨ���еĹҶ�һ���绰
**********************************************************************/
int     AfterHangup_MultOne(int iIndex)
{
    MBRecordCostTime MultCostPara;   //�෽ͨ����ʱ

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling AfterHangup_MultOne,iIndex:%d\r\n",iIndex);
#endif
    if ((iIndex < 0) || (iIndex > iMaxCount - 1) || 
        (PhoneItem[iIndex].ItemStat != ITEMUSED))
        return -1;

    ReadRecord_Cost(&MultCostPara); //��ö෽ͨ����ʱ��Ϣ
    RemTime(&MultCostPara,iIndex);  //����෽ͨ����ͨ����ʱ
    SaveRecord_Cost(&MultCostPara); //����෽ͨ����ͨ����ʱ

    PhoneItem[iIndex].ItemStat = ITEMNOTUSED;//���Ϊδʹ��

    if (PhoneItem[iIndex].Calls_Info.Dir == CALL_DIR_ORIGINATED)
    {
        SaveMBRecordUsedTime(DIALED_REC,
            PhoneItem[iIndex].Calls_Info.PhoneNum,
            &PhoneItem[iIndex].TimeInDial,&PhoneItem[iIndex].TimeUsed);
    }
    else if (PhoneItem[iIndex].Calls_Info.Dir == CALL_DIR_TERMINATED)
    {
        SaveMBRecordUsedTime(ANSWERED_REC,
            PhoneItem[iIndex].Calls_Info.PhoneNum,
            &PhoneItem[iIndex].TimeInDial,&PhoneItem[iIndex].TimeUsed);
    }
    memset(&PhoneItem[iIndex],0x00,sizeof(PHONEITEM));
    return 1;
}
/********************************************************************
* Function     AfterHangup_Mult_All
* Purpose      �෽ͨ����ȫ���Ҷ�֮��Ĵ���
* Params       
* Return       
* Remarks      ��ʱ������ͨ���϶��Ѿ��Ҷ�
**********************************************************************/
static  void    AfterHangup_Mult_All(void)
{
    int                 i;
    char                cTimer[40] = "";
    MBRecordCostTime    MultCostPara;   //�෽ͨ����ʱ

    ReadRecord_Cost(&MultCostPara);//��ö෽ͨ����ʱ��Ϣ

    MBList_ResetItem();//ɾ���෽ͨ�����нڵ�
    for (i = 0;i < iMaxCount;i++)
    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("MB �෽ͨ��ȫ���Ҷ�,PhoneItem[%d].ItemStat:%d,����:%s\r\n",
            i,PhoneItem[i].ItemStat,PhoneItem[i].Calls_Info.PhoneNum);
#endif
        if ((PhoneItem[i].ItemStat == ITEMUSED) && (0 != PhoneItem[i].Calls_Info.index))
        {
            RemTime(&MultCostPara,i);//����෽ͨ����ͨ����ʱ
            if (PhoneItem[i].Calls_Info.Dir == CALL_DIR_ORIGINATED)
            {
                SaveMBRecordUsedTime(DIALED_REC,
                    PhoneItem[i].Calls_Info.PhoneNum,
                    &PhoneItem[i].TimeInDial,&PhoneItem[i].TimeUsed);
            }
            else if (PhoneItem[i].Calls_Info.Dir == CALL_DIR_TERMINATED)
            {
                SaveMBRecordUsedTime(ANSWERED_REC,
                    PhoneItem[i].Calls_Info.PhoneNum,
                    &PhoneItem[i].TimeInDial,&PhoneItem[i].TimeUsed);
            }
            sprintf(cTimer,pTimeEnd,IDS_CALLTIME,
                PhoneItem[i].TimeUsed.wHour,
                PhoneItem[i].TimeUsed.wMinute,
                PhoneItem[i].TimeUsed.wSecond);

            if (strlen(PhoneItem[i].cName) != 0)
                MBList_AddItem(PhoneItem[i].cName,cTimer,MBLIST_IPICNONE,0,0);
            else
                MBList_AddItem(PhoneItem[i].Calls_Info.PhoneNum,cTimer,MBLIST_IPICNONE,0,0);
        }
        PhoneItem[i].ItemStat = ITEMNOTUSED;
        memset(&PhoneItem[i],0x00,sizeof(PHONEITEM));
    }
    SaveRecord_Cost(&MultCostPara);//����෽ͨ����ͨ����ʱ

    MBList_Refresh();
    MBList_SetCurSel(0);
    MBList_SetStyle(MBLIST_WS_ENABLED);

    SendMessage(hCalling,MB_ACTIVE,0,0);//��������ͨ��
}
/********************************************************************
* Function     MBCalling_Shortcut
* Purpose      �෽ͨ���еĿ�ݴ���
* Params       iType:��������,pShortPhone:�������
* Return       ME����ִ���Ƿ�ɹ�
* Remarks      
**********************************************************************/
BOOL    MBCalling_Shortcut(int iType,const char * pShortPhone)
{
    switch (iType)
    {
    case CUT_HELD://���еȴ�ʱæ�����߹Ҷϱ��ֵ�ͨ��
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling_Shortcut�Ҷϱ��ֵ�ͨ��\r\n");
#endif
        if (IsWindow_MBRing())//��ǰ���������
        {   //�����������ѡ��0,����Ҷ�����,������Ҷϵ�ǰ���ֵ�ͨ��,����Ҫ�ֱ���
            if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_REPEAT,CUT_HELD,0))
                return FALSE;
        }
        else
        {
            if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_CUT_HELD,CUT_HELD,0))
                return FALSE;
        }
        break;

    case CUT_ACTIVED://�Ҷϼ����ͨ������ֱ�ӽ���һ���ȴ��ĵ绰���߼���һ�����ֵĵ绰
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling_Shortcut�Ҷϼ����ͨ��\r\n");
#endif
        if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_CUT_ACTIVED,CUT_ACTIVED,0))
            return FALSE;

        break;

    case ACTIVE_SWITCH://�ڱ��ֺͼ����ͨ��״̬���л����ҽ��������ȴ��ĵ绰
        switch (CallingType)
        {
        case MBCALLINNONE://û��ͨ��
            break;
        case MBCALLINGONE://����ͨ��
        case MBCALLINGMULT://�෽ͨ��
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling_Shortcut ͨ���л�:%d\r\n",CallingType);
#endif
            if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_SHORTCUTSWITCH,ACTIVE_SWITCH,0))
                return FALSE;

            break;
        }
        break;

    case ADD_TO_ACTIVE://�����ֵ�ͨ�����뼤��ͨ��
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling_Shortcut �����ֵ�ͨ�����뼤��ͨ��\r\n");
#endif
        if (-1 == ME_SwitchCallStatus(hCalling,IDC_MULTME_ALLACTIVE,ADD_TO_ACTIVE,0))
            return FALSE;

        break;

    case ACTIVE_SWITCH_X://����ָ������������ͨ����Ϊ����
        if (NULL == pShortPhone)
            return FALSE;
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling_Shortcut ����ָ������������ͨ����Ϊ����,%s\r\n",pShortPhone);
#endif
        if (-1 == ME_VoiceDial(hCalling,IDC_ME_ACTIVE_SWITCH_X,(char *)pShortPhone,NULL))
            return FALSE;

        break;

    case CUT_ACTIVED_X://�Ҷ�ָ����ŵļ���ͨ��
        if (NULL == pShortPhone)
            return FALSE;
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling_Shortcut �Ҷ�ָ����ŵļ���ͨ��,%s\r\n",pShortPhone);
#endif
        if (-1 == ME_VoiceDial(hCalling,IDC_ME_CUT_ACTIVED_X,(char *)pShortPhone,NULL))
            return FALSE;

        break;

    case CALL_TRANSFER://ͨ���е�������·�绰ʱ,ʹ����·�绰����,�û���ͨ�����˳�
        if (NULL == pShortPhone)
            return FALSE;
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling_Shortcut ת��,PhoneGroupNum:%d,%s\r\n",PhoneGroupNum,pShortPhone);
#endif
        if (-1 == ME_VoiceDial(hCalling,IDC_ME_CALL_TRANSFER,(char *)pShortPhone,NULL))
            return FALSE;
        break;

    case CUT_ALL:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling_Shortcut �Ҷ�����ͨ��\r\n");
#endif
        if (-1 == ME_SwitchCallStatus(hCalling,IDC_MULTME_ALLHANGUP,CUT_ALL,0))
            return FALSE;

        break;
    default://�����Ŀ�ݲ���
        if (NULL == pShortPhone)
            return FALSE;
        if (-1 == ME_VoiceDial(hCalling,IDC_ME_SHORTOTHER,(char *)pShortPhone,NULL))
            return FALSE;

        break;
    }
    return TRUE;
}
//////////////////////////////////////////////////////////////////////////
static  BOOL    MBTalkingWindow(void)//��ʾͨ������
{
    WNDCLASS    wc;
    HMENU       hMenu;

    if (IsWindow(hCalling))
    {
        //MoveWindowToTop(hCalling);
        ShowWindow(hCalling,SW_SHOW);
        UpdateWindow(hCalling);
        SetFocus(hCalling);

        MBListCurCalls(LISTCURCALLS_REPEAT,0,0);
        PostMessage(hCalling,MB_ACTIVE,0,0);
        return TRUE;
    }
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

    hMenu = LoadMenuIndirect((PMENUTEMPLATE)&MBCallingMenuTemplate);
    InsertMenu(hMenu,IDM_MUTE,MF_BYCOMMAND,IDM_ONE_HOLD,TXT_HOLD);//����
    SetChangelessMenu(hMenu);

    hCalling = CreateWindow(pClassName,TITLECAPTION, 
        WS_CAPTION|WS_BORDER |PWS_STATICBAR, 
        PLX_WIN_POSITION,NULL,(HMENU)hMenu,NULL,NULL);

    if (NULL == hCalling)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }

    ShowWindow(hCalling, SW_SHOW);
    UpdateWindow(hCalling); 

    return (TRUE);
}
static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static BOOL bKeyDown;//��ǰ�Ƿ��а�������
    LRESULT     lResult;
    HDC         hdc;
    int         nUsedItemNum;//�����Ч��Item����
    int         iItemData;
    HMENU       hMenu;
    DWORD       dAcmMax = 0;

    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case WM_CREATE :
#if defined(MBCALLING_RECORD)
        {
            int iSW,iSH,iFontHeight;
            GetScreenUsableWH2(&iSW,&iSH);
            iFontHeight = GetCharactorHeight(NULL);//���ָ߶�
            hBRecord    = CreateWindow("BUTTON",TXT_RECORD,//����¼����ť
                WS_BORDER|WS_VISIBLE|WS_CHILD|WS_BITMAP,
                iPortraitX * 2,iSH - iFontHeight * 3,iSW - iPortraitX * 4,GetButtonControlHeight(),
                hWnd,(HMENU)IDC_BUTTON_RECORD,NULL,NULL);

            if (!IsSpareTimeLeft())//¼��ʱ������
                EnableWindow(hBRecord,FALSE);

            bRecord = FALSE;//�Ƿ�����¼��
        }        
#endif
#if defined(MBCALLING_TIMEACCURATE)
        {
            SYSTEMTIME sCurtime;
            GetLocalTime(&sCurtime);
            iInitSecond = sCurtime.wSecond;//Э��ͨ����ʱ(��)�ı���
        }
        SetTimer(hWnd,IDC_TIMER_CALLING,TIMER_CALLING,NULL);//����ͨ����ʱ�ļ�ʱ��
#endif
#if defined(MBCALLING_TIMESIMPLE)
        SetTimer(hWnd,IDC_TIMER_CALLING,TIMER_CALLING * 10,NULL);//����ͨ����ʱ�ļ�ʱ��
#endif
        SetTimer(hWnd,IDC_TIMER_INIT,TIMER_INIT,NULL);//���ó�ʼ���ļ�ʱ��
        SetTimer(hWnd,IDC_TIMER_ERROR,TIMER_ERROR,NULL);//���ó�����ļ�ʱ��

        hPortrait           = NULL;         //ͷ��Ϊ��
        bAOCInfo            = FALSE;        //����ʾͨ���Ʒ���Ϣ
        InterfaceType       = INTERFACE_ONE;//��ǰ��������
        bListCurCalling     = FALSE;        //��δ��ʼ�е�ǰͨ��
        bMute               = FALSE;        //��ǰ�����ر�
        bHolding            = FALSE;        //�Ƿ����ڱ�Hold
        bKeyDown            = FALSE;        //��ǰû�а�������
        bCanListCurCalling  = TRUE;         //�Ƿ������IDC_TIMER_ERROR��Ϣ���е�ǰͨ��
        //**************************************************************
        hMBList = CreateMBListControl(hWnd,IDC_MBLIST,0,0,
            GetScreenUsableWH1(SM_CXSCREEN),GetScreenUsableWH1(SM_CYSCREEN),
            WS_BORDER | WS_CHILD);//�����෽ͨ���ؼ�

        PostMessage(hWnd,MB_ACTIVE,0,0);

        //SaveLog("ͨ����ʼ\r\n");
        break;

    case MB_ACTIVE://ͨ�������б仯��ʱ��,��Ҫ���������Ϣ,��ʾ�����б仯
        SendDtmf(hWnd);//���ý��淢�ͷֻ���

        nUsedItemNum = GetUsedItemNum();//������Item����
        if (1 == nUsedItemNum)//����ͨ��
            CallingType = MBCALLINGONE;
        else if ((1 < nUsedItemNum) && (nUsedItemNum <= iMaxCount))//�෽ͨ��
            CallingType = MBCALLINGMULT;
        else//û��ͨ��
            CallingType = MBCALLINNONE;

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("\nMB MB_ACTIVE,nUsedItemNum:%d\n",nUsedItemNum);
#endif
        DestroyViewList();
        MoveWindowToTop(hWnd);
        SetMenuSoftkey();//�޸�ͨ������İ�ť�Լ��˵�
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("MB MB_ACTIVE,��Ϣ����\r\n");
#endif
        break;

    case WM_ACTIVATE:
        if (WA_INACTIVE != LOWORD(wParam))
        {
            if (IsWindow(hMBList) && (IsWindowVisible(hMBList)))
                SetFocus(hMBList);
            else
                SetFocus(hWnd);//��������ý���
        }
        break;

    case WM_TIMER:
        switch (wParam)
        {
        case IDC_TIMER_INIT://��ʼ����Ϣ�ļ�ʱ��
            KillTimer(hWnd,IDC_TIMER_INIT);
            ME_GetACMmax(hWnd,IDC_ME_MAXPRICE);//��ȡ����ۼ�Call Meterֵ
            break;

        case IDC_TIMER_CALLING://ͨ����ʱ�ļ�ʱ��
            ChangeTimeOnTimer();//�ı�ʱ��
            break;

        case IDC_TIMER_COST://ͨ���Ʒѵļ�ʱ��
            if (!bCanGetCurrentCCM)
                break;
            bCanGetCurrentCCM = FALSE;//�����Է�����Ϣ��ȡ��ǰ�Ʒ���Ϣ
            ME_GetCCM(hWnd,IDC_ME_GETCCM);//��ȡ��ǰCall Meterֵ
            break;

#if defined(KEYSWITCHTYPE_LONG)
        case IDC_TIMER_LONGKEY://�������ּ�0�ļ�ʱ��
            KillTimer(hWnd,IDC_TIMER_LONGKEY);
            SuperAddDialNumber(pPlus,TRUE,TRUE);//����Ӻ�
            break;
#endif

        case IDC_TIMER_ERROR://������ļ�ʱ��
            if (bListCurCalling)//�����е�ǰͨ��
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
                printf("mb MBCalling �����е�ǰͨ��\r\n");
#endif
                break;
            }
            if (!bCanListCurCalling)//��������IDC_TIMER_ERROR��Ϣ���е�ǰͨ��
                break;
            if (IsWindow_DialLink() || IsWindow_MBRing())
                break;
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling ��ʱ�е绰����\r\n");
#endif
            if (-1 == ME_ListCurCalls(hCalling,IDC_ERROR_LISTCALLING))//�е�ǰͨ��
            {
                bListCurCalling  = FALSE;
            }
            else
            {
                bListCurCalling  = TRUE;//�����е�ǰͨ��
                ListCurCallsType = LISTCURCALLS_REPEAT;
            }
            break;
        }
        break;

    case WM_DESTROY ://֪ͨ���������,�˳����˳�;
        //SaveLog("ͨ������\r\n");

#if defined(MBCALLING_RECORD)
        if (bRecord)//��ǰ����¼��
            EndRecorderUnvisualable();//ֹͣ¼��
#endif
        hCalling      = NULL;
        hMBList       = NULL;
        PhoneGroupNum = 0;//���õ绰·��
        CallingType   = MBCALLINNONE;//û��ͨ��
        bCostLimit    = FALSE;//���õ�ǰͨ���Ƿ��ѵ���������
        RemovePortraitBitmap();//ɾ������ͨ����ʾ��ͷ��

        KillTimer(hWnd,IDC_TIMER_CALLING);
        MBList_Destroy();//���ٶ෽ͨ���Ŀؼ�
        UnregisterClass(pClassName,NULL);
        DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)hInstance);
        EnableState();
        PhoneSetChannelCalling(FALSE);
        break;

    case WM_PAINT :
        hdc = BeginPaint( hWnd, NULL);
        CallingDisplay(hdc);
        EndPaint(hWnd, NULL);
        break;

    case WM_TODESKTOP:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("\nmb MBCalling WM_TODESKTOP,wParam:%ld\n",wParam);
#endif
        if (0 == wParam)//����������Զ�����
            return FALSE;
        if (!IsWindow_MBRing() &&   //�������
            !IsWindow_DialLink() && //���ӽ���
            !IsWindow_Dialup())     //���Ž���
            SendMessage(hWnd,WM_KEYDOWN,VK_F2,0);

        return FALSE;

#if defined(KEYSWITCHTYPE_LONG)
    case WM_KEYUP:
        switch(LOWORD(wParam))
        {
        case VK_0:  //�������ּ�0
            KillTimer(hWnd,IDC_TIMER_LONGKEY);
            bKeyDown = FALSE;//��ǰû�а�������
            SuperAddDialNumber(pNum0,TRUE,TRUE);//����׷�Ӳ���
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            break;
        }
        break;
#endif

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_UP://VK_PAGEUP://��������
            CallheadphoneWindow(1);//����ͨ������
            break;
        case VK_DOWN://VK_PAGEDOWN://��С����
            CallheadphoneWindow(-1);//����ͨ������
            break;
        case VK_LEFT:
            if (MBCALLINNONE == CallingType)//��ǰ��ͨ�������������˳�
                DestroyWindow(hWnd);
            break;
        case VK_RIGHT:
            if (MBCALLINNONE == CallingType)//��ǰ��ͨ�������������˳�
                DestroyWindow(hWnd);
            break;

#if defined(KEYSWITCHTYPE_LONG)
        case VK_0:  //�������ּ�0,�������ּ�0��ʱ��,��Ҫ����Ӻ�
            if (MBCALLINNONE == CallingType)//��ǰ��ͨ�������������˳�
            {
                DestroyWindow(hWnd);
                break;
            }
            //���ڳ������Ҫ������Ӻ�,����0�����µ�ʱ����Ҫ������ʱ��,����ʱ������
            //��ʱ��,��Ϊ������Ҫ����Ӻ�.����ʱ��û�е����ʱ��,����0
            if (bKeyDown)//��ǰ�а�������
                break;
            bKeyDown = TRUE;//��ǰ�а�������
            KillTimer(hWnd,IDC_TIMER_LONGKEY);
            SetTimer(hWnd,IDC_TIMER_LONGKEY,TIMER_LONGKEY,NULL);//����������0�ļ�ʱ��
            break;
#endif

#if defined(KEYSWITCHTYPE_SHORT)
        case VK_0:
#endif
        case VK_1:  //�������ּ�1
        case VK_2:  //�������ּ�2
        case VK_3:  //�������ּ�3
        case VK_4:  //�������ּ�4
        case VK_5:  //�������ּ�5
        case VK_6:  //�������ּ�6
        case VK_7:  //�������ּ�7
        case VK_8:  //�������ּ�8
        case VK_9:  //�������ּ�9
            if (MBCALLINNONE == CallingType)//��ǰ��ͨ�������������˳�
            {
                DestroyWindow(hWnd);
            }
            else
            {
                char chSend[2] = "";//׷�Ӳ�����������
                chSend[0] = (LOWORD(wParam) - VK_0 + 48);
                SuperAddDialNumber(chSend,TRUE,TRUE);//����׷�Ӳ���
            }
            break;
        case KEYASTERICK: //*
            if (MBCALLINNONE == CallingType)//��ǰ��ͨ�������������˳�
                DestroyWindow(hWnd);
            else
                SuperAddDialNumber(pAsterisk,TRUE,TRUE);//����׷�Ӳ���"*"
            break;
        case KEYWELL: //#
            if (MBCALLINNONE == CallingType)//��ǰ��ͨ�������������˳�
                DestroyWindow(hWnd);
            else
                SuperAddDialNumber(pWell,TRUE,TRUE);//����׷�Ӳ���"#"
            break;

        case VK_F1://���ż�
            switch (CallingType)
            {
            case MBCALLINNONE:
                DestroyWindow(hWnd);
                break;
            case MBCALLINGONE:
                SendMessage(hWnd,WM_COMMAND,IDM_ONE_HOLD,0);
                break;
            case MBCALLINGMULT:
                SendMessage(hWnd,WM_COMMAND,IDM_SWITCH2,0);
                break;
            }
            break;

#if defined(MBCALLING_RECORD)
        case VK_RETURN:
            SendMessage(hWnd,WM_COMMAND,IDM_RECORD,0);
            break;
#endif

        case VK_FLIP_OFF://�ϸǹҶ�
            if (MBCongig_Calling.bModeTurn)//�ϸǹҶ�
                ExecuteHangup();//����Ҷϼ��Ĳ���
            break;
        case VK_F10:
        case VK_F2://�Ҷ�
        case VK_EARPHONE_KEY://�����ϵİ���
            ExecuteHangup();//����Ҷϼ��Ĳ���
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        //***********��ť���ֿ�ʼ****************
#if defined(MBCALLING_RECORD)
        case IDC_BUTTON_RECORD://¼����ť
            switch(HIWORD(wParam))
            {
            case BN_CLICKED:
                SendMessage(hWnd,WM_COMMAND,IDM_RECORD,0);
                break;
            }
            break;
#endif
        case IDC_BUTTON_MIDDLE://�м�,����
            SuperAddDialNumber(NULL,TRUE,TRUE);
            break;
        case IDC_RBUTTON://�Ҷ�
            SendMessage(hWnd,WM_KEYDOWN,VK_F2,0);
            break;
        //***********��ť���ֽ���****************
        //***********�˵����ֿ�ʼ****************
        case IDM_MUTE://����
            //bMute=FALSE��ʾ��ǰ�����ر���Ҫ�򿪾���
            //bMute=TRUE��ʾ��ǰ��������Ҫ�رվ���
            if (-1 == ME_MuteControl(hWnd,IDC_ME_MUTE,!bMute))
            {
                AppMessageBox(hWnd,bMute ? MUTEFAIL : UNMUTEFAIL,TITLECAPTION,WAITTIMEOUT);
            }
            else
            {
                bCanListCurCalling = FALSE;//��������IDC_TIMER_ERROR��Ϣ���е�ǰͨ��
                WaitWindowState(hWnd,TRUE);//MBWaitWindowState(hWnd,TRUE,NULL,FALSE);
            }
            break;

#if defined(MBCALLING_RECORD)
        case IDM_RECORD://¼��
            if (bRecord)//����¼��,��ť������Ӧ��Ϊ"ֹͣ¼��",�����ť֮���Ϊ¼��
            {
                SetWindowText(hBRecord,TXT_RECORD);
                hMenu = PDAGetMenu(hWnd);
                ModifyMenu(hMenu,IDM_RECORD,MF_BYCOMMAND|MF_ENABLED,IDM_RECORD,TXT_RECORD);
                EndRecorderUnvisualable();
                bRecord = FALSE;
                if (!IsSpareTimeLeft())//¼��ʱ������
                    EnableWindow(hBRecord,FALSE);
            }
            else//��δ¼��,��ť������Ӧ��Ϊ"¼��",�����ť֮���Ϊֹͣ¼��
            {
                if (StartRecorderUnvisualable(hWnd,IDC_RECORD_OVER))
                {
                    SetWindowText(hBRecord,TXT_STOPRECORD);
                    hMenu = PDAGetMenu(hWnd);
                    ModifyMenu(hMenu,IDM_RECORD,MF_BYCOMMAND|MF_ENABLED,IDM_RECORD,TXT_STOPRECORD);
                    bRecord = TRUE;
                }
                else
                {
                    AppMessageBox(hWnd,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);
                }
            }
            break;
#endif
        case IDM_VIEW://�鿴
            MBViewList(hWnd);
            break;
        //*********************************************************************
        case IDM_ONE_HOLD://����/����(�˵���,����ͨ��)
            if (-1 == ME_SwitchCallStatus(hCalling,IDC_ONE_SWITCH,ACTIVE_SWITCH,0))
                AppMessageBox(hCalling,SWITCHFAIL,TITLECAPTION,WAITTIMEOUT);
            else
                WaitWindowState(hWnd,TRUE);
            break;

        //*********************************************************************
        case IDM_SWITCH1://�л�(�绰����1,1���绰����,1���绰����)
            if (-1 == ME_SwitchCallStatus(hCalling,IDC_MULTME_SWITCH1,ACTIVE_SWITCH,0))
                AppMessageBox(hCalling,SWITCHFAIL,TITLECAPTION,WAITTIMEOUT);
            else
                WaitWindowState(hWnd,TRUE);
            break;

        case IDM_SWITCH2://����(�绰����2,ȫ������/ȫ�����ֵ����)
            if (-1 == ME_SwitchCallStatus(hCalling,IDC_MULTME_SWITCH2,ACTIVE_SWITCH,0))
                AppMessageBox(hCalling,SWITCHFAIL,TITLECAPTION,WAITTIMEOUT);
            else
                WaitWindowState(hWnd,TRUE);
            break;

        //**********************************************************************
        case IDM_HANGUPACTIVE://�Ҷϼ���ĵ绰
            if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_CUT_ACTIVED,CUT_ACTIVED,0))
                AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
            break;

        case IDM_HANGUPHOLD://�Ҷϱ��ֵ�ͨ��
            if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_CUT_HELD,CUT_HELD,0))
                AppMessageBox(hWnd,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
            break;

        case IDM_ALLHANGUP://ȫ���Ҷ�
            if (-1 == ME_SwitchCallStatus(hCalling,IDC_MULTME_ALLHANGUP,CUT_ALL,0))
                AppMessageBox(hWnd,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
            break;

        case IDM_ALLACTIVE://ȫ������
            if (-1 == ME_SwitchCallStatus(hCalling,IDC_MULTME_ALLACTIVE,ADD_TO_ACTIVE,0))
                AppMessageBox(hCalling,ACTIVEFAIL,TITLECAPTION,WAITTIMEOUT);
            break;

        case IDM_SHIFT://ת��
            if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_CALL_TRANSFER,CALL_TRANSFER,0))
                AppMessageBox(hCalling,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//����ʧ��
            break;
        //*******************************************************************
        case IDM_SINGLECALL://����ͨ��
            bSelectSingleConnect = FALSE;//��δѡ�񵥶�ͨ���İ�ť
            SetMBCallingMenu(INTERFACE_MULT5);
            AddMultCallInfo();
            MBList_Show(SW_SHOW);
            MBList_SetStyle(MBLIST_WS_ENABLED);
            MBList_SetCurSel(0);
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_LBUTTON_CONNECT,1),(LPARAM)TXT_CONNECT);
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_MIDDLE,2),(LPARAM)"");
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_RBUTTON_CANCEL,(LPARAM)TXT_CANCEL);
            SetWindowText(hWnd,TXT_SINGLECALL);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling ����ͨ�� �������\r\n");
#endif
            break;

        case IDM_SINGLEHANGUP://�����Ҷ�
            SetMBCallingMenu(INTERFACE_MULT6);
            AddMultCallInfo();
            MBList_Show(SW_SHOW);
            MBList_SetStyle(MBLIST_WS_ENABLED);
            MBList_SetCurSel(0);
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_LBUTTON_HANGUP,1),(LPARAM)TXT_HANGUP);
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_MIDDLE,2),(LPARAM)"");
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_RBUTTON_CANCEL,(LPARAM)TXT_CANCEL);
            SetWindowText(hWnd,TXT_SINGLEHANGUP);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling �����Ҷ� �������\r\n");
#endif
            break;

        case IDC_LBUTTON_CONNECT://����ͨ��
            if (bSelectSingleConnect)//�Ѿ�ѡ���˵���ͨ���İ�ť
            {
                AppMessageBox(hWnd,DIALLINKINGWAIT,TITLECAPTION, WAITTIMEOUT);
                break;
            }
            iItemData = MBList_GetItem(MBList_GetCurSel());
            //��õ�ǰ�ؼ���ѡ����Ŀ�������ڽṹ�����е�λ��
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling ����ͨ�� iItemData:%d\r\n",iItemData);
#endif
            if ((iItemData < 0) || (iItemData > iMaxCount - 1))
                iItemData = 0;
            if (-1 == ME_SwitchCallStatus(hCalling,IDC_ME_ACTIVE_SWITCH_X,
                            ACTIVE_SWITCH_X,PhoneItem[iItemData].Calls_Info.index))
                AppMessageBox(hWnd,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling ����ͨ�� iItemData:%d,index:%d\r\n",
                iItemData,PhoneItem[iItemData].Calls_Info.index);
#endif
            bSelectSingleConnect = TRUE;//ѡ���˵���ͨ���İ�ť
            break;

        case IDC_LBUTTON_HANGUP://�����Ҷ�
            iItemData = MBList_GetItem(MBList_GetCurSel());
            //��õ�ǰ�ؼ���ѡ����Ŀ�������ڽṹ�����е�λ��
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling �����Ҷ� iItemData:%d\r\n",iItemData);
#endif
            if ((iItemData < 0) || (iItemData > iMaxCount - 1))
                iItemData = 0;
            if (-1 == ME_SwitchCallStatus(hWnd,IDC_ME_CUT_ACTIVED_X,//�Ҷ�ѡ�еĵ绰
                            CUT_ACTIVED_X,PhoneItem[iItemData].Calls_Info.index))
                AppMessageBox(hWnd,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling �����Ҷ� iItemData:%d,index:%d\r\n",
                iItemData,PhoneItem[iItemData].Calls_Info.index);
#endif
            break;

        case IDC_RBUTTON_CANCEL://����ͨ��/�����Ҷϵ�ȡ����ť
            SendMessage(hWnd,MB_ACTIVE,0,0);
            break;

        case IDC_LBUTTON_NONE://��ʾͨ����������ʱ���ȷ����ť
        case IDC_RBUTTON_BACK://��ʾͨ����������ʱ����˳���ť
            DestroyWindow(hWnd);
            break;
        }
        break;
    //*****************ME�ص���Ϣ**********************
    //********************��ʼ����ͨ���Ʒ���Ϣ**********************
    case IDC_ME_MAXPRICE://��ѯ����ۼ�Call Meterֵ����Ϣ
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            ME_GetResult(&dAcmMax,sizeof(DWORD));
            if ((dAcmMax > 0) && (dAcmMax < 0xffffff))//����ͨ���޶�
            {
                if (-1 == ME_GetPricePerUnit(hWnd,IDC_ME_GETPRICEUNIT))//��ȡ�Ƽ���Ϣ
                    bAOCInfo = FALSE;
            }
            break;
        default:
            bAOCInfo = FALSE;
            break;
        }
        break;

    case IDC_ME_GETPRICEUNIT://��ȡ�Ƽ���Ϣ
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling IDC_ME_GETPRICEUNIT wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            GetPriceUnit();//��ȡ�Ƽ���Ϣ֮��Ĵ������
            break;
        default:
            bAOCInfo = FALSE;
            break;
        }
        break;

    case IDC_ME_GETCCM://��ȡ��ǰCall Meterֵ
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling IDC_ME_GETCCM wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            if (-1 == ME_GetResult(&dCostInfo,sizeof(DWORD)))
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
                printf("mb MBCalling IDC_ME_GETCCM ME_GetResult==-1\r\n");
#endif
            }
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling dCostInfo:%ld\r\n",dCostInfo);
#endif
            bCanGetCurrentCCM = TRUE;//���Է�����Ϣ��ȡ��ǰ�Ʒ���Ϣ
            InvalidateRect(hWnd,&rCostInfo,TRUE);
            break;
        default:
            SendAbortCommand(wParam,lParam);//����ȡ��������at����
            dCostInfo = 0;
            break;
        }        
        break;
    //********************ͨ���Ʒ���Ϣ�������**********************    
    case IDC_ME_MUTE://�����Ļص���Ϣ(����ͨ��/�෽ͨ��)
        bCanListCurCalling = TRUE;//������IDC_TIMER_ERROR��Ϣ���е�ǰͨ��
        WaitWindowState(hWnd,FALSE);//MBWaitWindowState(hWnd,FALSE,NULL,FALSE);
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            bMute = !bMute;
            hMenu = PDAGetMenu(hWnd);
            ModifyMenu(hMenu,IDM_MUTE,MF_BYCOMMAND|MF_ENABLED,IDM_MUTE,
                bMute ? TXT_UNMUTE : TXT_MUTE);

            InvalidateRect(hWnd,NULL,TRUE);
            AppMessageBox(hWnd,TXT_FINISH,TITLECAPTION,WAITTIMEOUT);
            break;
        default:
            SendAbortCommand(wParam,lParam);//����ȡ��������at����
            AppMessageBox(hWnd,bMute ? UNMUTEFAIL : MUTEFAIL,TITLECAPTION,WAITTIMEOUT);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling ִ�о���ʧ��,bMute:%d,wParam:%ld,lParam:%ld\r\n",
                bMute,wParam,lParam);
#endif
            break;
        }
        break;

    case IDC_ME_LISTCURCALLS://�е�ǰͨ���Ļص���Ϣ
        MBWaitWindowState(hWnd,FALSE,NULL,FALSE);
        bListCurCalling = FALSE;
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            if (OperateListCurCalls())//�е�ǰͨ��֮��ִ�еĲ���
                SendMessage(hWnd,MB_ACTIVE,0,0);//���е绰֮��,��Ҫ���½���

            break;
        default:
            SendAbortCommand(wParam,lParam);//����ȡ��������at����
            AppMessageBox(hCalling,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);
            break;
        }
        break;

    case IDC_ERROR_LISTCALLING://������Ļص���Ϣ
        bListCurCalling = FALSE;
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            OperateListCurCalls();
            break;
        }
        break;

    //*************************�й��л��Ĳ�����ʼ*************
    case IDC_ME_SHORTCUTSWITCH://�л��Ļص���Ϣ,���ڿ�ݲ���
        switch (wParam)
        {
        case ME_RS_SUCCESS://�����л�֮����ܻ�����µĵ绰,������Ҫ���е�ǰͨ��
            MBListCurCalls(LISTCURCALLS_SWITCH,wParam,lParam);
            break;
        default:
            SendAbortCommand(wParam,lParam);//����ȡ��������at����
            AppMessageBox(hWnd,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);
            break;
        }
        break;

    case IDC_ONE_SWITCH://�ڱ��ֺͼ����ͨ��״̬���л�(����ͨ��)
        WaitWindowState(hWnd,FALSE);
        switch (wParam)
        {
        case ME_RS_SUCCESS://����ͨ����ִ���л��������ᵼ�½���ı�
            bHolding = !bHolding;
            hMenu = PDAGetMenu(hWnd);//���ò˵�
            ModifyMenu(hMenu,IDM_ONE_HOLD,MF_BYCOMMAND|MF_ENABLED,IDM_ONE_HOLD,
                bHolding ? TXT_RESTORE : TXT_HOLD);

            InvertItemStateOne(iCallingOneNum);//�ṹ�����н���״̬�л�
            AppMessageBox(hWnd,TXT_FINISH,TITLECAPTION,WAITTIMEOUT);
            break;
        default:
            SendAbortCommand(wParam,lParam);//����ȡ��������at����            
            AppMessageBox(hWnd,SWITCHFAIL,TITLECAPTION,WAITTIMEOUT);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling ����ͨ���л�ʧ��,bHolding:%d,wParam:%ld,lParam:%ld\r\n",
                bHolding,wParam,lParam);
#endif
            break;
        }
        break;

    case IDC_MULTME_SWITCH1://�෽ͨ�����л���Ϣ,1���绰����,1���绰����        
        WaitWindowState(hWnd,FALSE);
        switch (wParam)
        {
        case ME_RS_SUCCESS://1���绰����,1���绰���ֵ�ʱ��ִ���л��������ᵼ�½���ı�
            InvertItemStateMult();//�����뱣�ֽ���
            AppMessageBox(hWnd,TXT_FINISH,TITLECAPTION,WAITTIMEOUT);
            break;
        default:
            SendAbortCommand(wParam,lParam);//����ȡ��������at����
            AppMessageBox(hWnd,SWITCHFAIL,TITLECAPTION,WAITTIMEOUT);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling �෽ͨ���л�ʧ��,wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
            break;
        }
        break;

    case IDC_MULTME_SWITCH2://�෽ͨ�����л���Ϣ(ȫ��Ϊ����ͨ��/ȫ��Ϊ����ͨ��)
        WaitWindowState(hWnd,FALSE);
        switch (wParam)
        {
        case ME_RS_SUCCESS://���½���ı�
            InvertItemStateMult();//�����뱣�ֽ���
            SendMessage(hWnd,MB_ACTIVE,0,0);
            AppMessageBox(hWnd,TXT_FINISH,TITLECAPTION,WAITTIMEOUT);
            break;
        default:
            SendAbortCommand(wParam,lParam);//����ȡ��������at����
            AppMessageBox(hWnd,SWITCHFAIL,TITLECAPTION,WAITTIMEOUT);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling �෽ͨ���л�ʧ��,wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
            break;
        }
        break;
    //*************************�й��л��Ĳ�������*************
    //*************************�йعҶϵĲ�����ʼ*************
    case WM_VOICE_HANGUPONE://����ͨ���ĹҶ�
        MBListCurCalls(LISTCURCALLS_HANGUP_ONE,wParam,lParam);//�е�ǰͨ��
        break;
    case IDC_MULTME_ALLHANGUP://�෽ͨ����ȫ���Ҷ���Ϣ
        MBListCurCalls(LISTCURCALLS_HANGUP_ALL,wParam,lParam);//�е�ǰͨ��
        break;
    case IDC_ME_CUT_HELD://�Ҷϱ��ֵ�ͨ��(�෽ͨ��)
        MBListCurCalls(LISTCURCALLS_HANGUP_HELD,wParam,lParam);//�е�ǰͨ��
        break;
    case IDC_ME_CUT_ACTIVED://�Ҷϼ����ͨ��(�෽ͨ��)
        MBListCurCalls(LISTCURCALLS_HANGUP_ACTIVE,wParam,lParam);//�е�ǰͨ��
        break;
    case IDC_ME_CUT_ACTIVED_X://�Ҷ�ָ����ŵļ���ͨ��
        MBListCurCalls(LISTCURCALLS_HANGUP_CUT_X,wParam,lParam);//�е�ǰͨ��
        break;
    //*************************�йعҶϵĲ�������*************
    case IDC_MULTME_ALLACTIVE://�෽ͨ����ȫ��������Ϣ
        switch (wParam)
        {
        case ME_RS_SUCCESS://���½���ı�
            PhoneGroupNum = 1;
            SetAllType(CALL_STAT_ACTIVE);
            SendMessage(hWnd,MB_ACTIVE,0,0);
            AppMessageBox(hWnd,TXT_FINISH,TITLECAPTION,WAITTIMEOUT);
            break;
        default:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling ��ͨ����ȫ������ʧ��,wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
            SendAbortCommand(wParam,lParam);//����ȡ��������at����
            AppMessageBox(hWnd,ACTIVEFAIL,TITLECAPTION,WAITTIMEOUT);
            break;
        }
        break;

    case IDC_ME_ACTIVE_SWITCH_X://����ָ������������ͨ����Ϊ����
        bSelectSingleConnect = FALSE;//��δѡ�񵥶�ͨ���İ�ť
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            MBListCurCalls(LISTCURCALLS_ACTIVE_X,wParam,lParam);//���е�ǰͨ��
            break;
        default:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling ����ָ������������ͨ����Ϊ����ʧ��,wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
            SendAbortCommand(wParam,lParam);//����ȡ��������at����
            AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//����ʧ��
            break;
        }
        break;

    case IDC_ME_CALL_TRANSFER://ͨ���е�������·�绰ʱ,ʹ����·�绰����,�û���ͨ�����˳�
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            MBListCurCalls(LISTCURCALLS_TRANSFER,wParam,lParam);//�е�ǰͨ��
            break;
        default:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling ת��ʧ��wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
            SendAbortCommand(wParam,lParam);//����ȡ��������at����
            AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//����ʧ��
            break;
        }
        break;

    case IDC_ME_REPEAT://���е�ǰͨ��
        MBListCurCalls(LISTCURCALLS_REPEAT,wParam,lParam);
        break;

    case IDC_ME_SHORTOTHER://�����Ŀ�ݲ���
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            break;
        default:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling IDC_ME_SHORTOTHERʧ��wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
            SendAbortCommand(wParam,lParam);//����ȡ��������at����
            AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//����ʧ��
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
* Function     InitMBCalling
* Purpose      ��ʼ��ͨ����������ͼƬ��Ϣ
* Params       
* Return           
* Remarks      ����ͨ����ʼ������
**********************************************************************/
void    InitMBCalling(void)
{
    int iFontHeight,iSW;

    iFontHeight = GetCharactorHeight(NULL);//���ָ߶�
    iSW         = GetScreenUsableWH1(SM_CXSCREEN);

    SetRect(&rPhoto,iPortraitX,iPortraitY,
        iPortraitX + iPortraitW,iPortraitY + iPortraitH);

    SetRect(&rName      ,rPhoto.right,rPhoto.top,
        iSW,rPhoto.top + iFontHeight * 2);
    SetRect(&rPhoneCode ,rPhoto.right,rName.bottom + iFontHeight ,
        iSW,rName.bottom + iFontHeight * 3);
    SetRect(&rTime      ,rPhoto.left ,rPhoto.bottom + iFontHeight,
        iSW,rPhoto.bottom + iFontHeight * 2);
    SetRect(&rCostInfo  ,rPhoto.left ,rTime.bottom + iFontHeight ,
        iSW,rTime.bottom + iFontHeight * 2);
}
/********************************************************************
* Function     GetPriceUnit
* Purpose      ��ȡ�Ƽ���Ϣ֮��Ĵ������
* Params       
* Return           
* Remarks      
**********************************************************************/
static  void    GetPriceUnit(void)
{
    if (-1 == ME_GetResult(&me_pucinfo,sizeof(ME_PUCINFO)))
    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling IDC_ME_GETPRICEUNIT ME_GetResult==-1\r\n");
#endif
        bAOCInfo = FALSE;
    }
    else
    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling,me_pucinfo.currency:%s,me_pucinfo.ppu:%s\r\n",
            me_pucinfo.currency,me_pucinfo.ppu);
#endif
        TrimString(me_pucinfo.currency);
        TrimString(me_pucinfo.ppu);
        if ((strlen(me_pucinfo.currency) == 0) && (strlen(me_pucinfo.ppu) == 0))
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling,û�����üƼ���Ϣ\r\n");
#endif
            bAOCInfo = FALSE;
        }
        else
        {
            bAOCInfo          = TRUE;//��ʾͨ���Ʒ���Ϣ
            bCanGetCurrentCCM = TRUE;//���Է�����Ϣ��ȡ��ǰ�Ʒ���Ϣ
            SetTimer(hCalling,IDC_TIMER_COST,TIMER_CALLING_COST,NULL);//����ͨ���Ʒѵļ�ʱ��
            if (strlen(me_pucinfo.currency) > 0)//�����˼Ƽ���Ϣ,ppu ���Դ�ME_GetCCM�л�õ�ֵ
            {
                CostType = COSTTYPE_PRICE;
                if (StrIsDouble(me_pucinfo.ppu))
                    dCost = MB_atof(me_pucinfo.ppu);
                else
                    dCost = dZero;//0.0
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
                printf("mb MBCalling,�����˼Ƽ���Ϣ,%f\r\n",dCost);
#endif
            }
            else//��ʾ,��ME_GetCCM������ȡ,ֱ����ʾ
            {
                CostType = COSTTYPE_UNIT;
                dCost    = dZero;//0.0
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
                printf("mb MBCalling,��ME_GetCCM������ȡ\r\n");
#endif
            }
        }
    }
}
/********************************************************************
* Function     ChangeTimeOnTimer
* Purpose      ����ͨ����ӦTIMER�ı�ʱ��
* Params       
* Return           
* Remarks      
**********************************************************************/
static  void    ChangeTimeOnTimer(void)
{
    int i,iTimeMargin;
#if defined(MBCALLING_TIMEACCURATE)
    SYSTEMTIME sCurtime;
    GetLocalTime(&sCurtime);//���Ȼ�õ�ǰʱ��
    if (sCurtime.wSecond == iInitSecond)//ʱ��û�иı�
        return;

    if (sCurtime.wSecond > iInitSecond)//����ʱ����
        iTimeMargin = sCurtime.wSecond - iInitSecond;
    else
        iTimeMargin = iSecond60 - iInitSecond + sCurtime.wSecond;

    iInitSecond = sCurtime.wSecond;//���±��濪ʼ��ʱ��ʱ��
#endif
#if defined(MBCALLING_TIMESIMPLE)
    iTimeMargin = 1;
#endif

    switch (CallingType)
    {
    case MBCALLINGONE://����ͨ��
        PhoneItem[iCallingOneNum].TimeUsed.wSecond += iTimeMargin;
        if (PhoneItem[iCallingOneNum].TimeUsed.wSecond >= iSecond60)
        {
            PhoneItem[iCallingOneNum].TimeUsed.wSecond = 0;
            PhoneItem[iCallingOneNum].TimeUsed.wMinute++;
            if (PhoneItem[iCallingOneNum].TimeUsed.wMinute >= iSecond60)
            {
                PhoneItem[iCallingOneNum].TimeUsed.wHour++;
                PhoneItem[iCallingOneNum].TimeUsed.wMinute = 0;
            }
        }
        if (MBCongig_Calling.b50Tip)//����50����ʾ��
        {
            if (((iTime_Second + 10) % iSecond60)== 0)//����50����ʾ��
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
                printf("mb MBCalling ����50����ʾ��,iTime_Second:%d\r\n",iTime_Second);
#endif
                PlayClewMusic(1);
            }
            iTime_Second++;
        }
        InvalidateRect(hCalling,&rTime ,TRUE);//ˢ��ͨ�������ʱ��
        InvalidateRect(hCalling,&rPhoto,TRUE);//ˢ��ͨ�������ͼ��
        break;
    case MBCALLINGMULT://�෽ͨ��
        for (i = 0;i < iMaxCount;i++)
        {
            if ((ITEMUSED == PhoneItem[i].ItemStat) && (0 != PhoneItem[i].Calls_Info.index))
            {
                PhoneItem[i].TimeUsed.wSecond += iTimeMargin;
                if (PhoneItem[i].TimeUsed.wSecond >= iSecond60)
                {
                    PhoneItem[i].TimeUsed.wSecond = 0;
                    PhoneItem[i].TimeUsed.wMinute++;
                    if (PhoneItem[i].TimeUsed.wMinute >= iSecond60)
                    {
                        PhoneItem[i].TimeUsed.wHour++;
                        PhoneItem[i].TimeUsed.wMinute = 0;
                    }
                }
            }
        }
        switch (InterfaceType)
        {
        case INTERFACE_MULT1://�෽ͨ���˵�1(1���绰����,1���绰����)
        case INTERFACE_MULT31://�෽ͨ���˵�3(1������ͨ��,���ౣ��)
        case INTERFACE_MULT32://�෽ͨ���˵�3(1������ͨ��,���༤��)
        case INTERFACE_MULT5://����ͨ��
        case INTERFACE_MULT6://�����Ҷ�
            InvalidateRect(hCalling,NULL,TRUE);
            break;
        case INTERFACE_MULT21://�෽ͨ���˵�2(ȫ��Ϊ����ͨ��)
        case INTERFACE_MULT22://�෽ͨ���˵�2(ȫ��Ϊ����ͨ��)
            break;
        }
        break;
    }
}
/********************************************************************
* Function   RemovePortraitBitmap  
* Purpose    ɾ������ͨ����ʾ��ͷ��
* Params    
* Return    
* Remarks   
**********************************************************************/
static  void    RemovePortraitBitmap(void)
{
    if (hPortrait != NULL)
    {
        DeleteObject(hPortrait);
        hPortrait = NULL;
    }
}
/********************************************************************
* Function   SetMenuSoftkey  
* Purpose    �޸�ͨ������İ�ť�Լ��˵�
* Params    
* Return    
* Remarks   
**********************************************************************/
static  BOOL    SetMenuSoftkey(void)
{
    HMENU       hMenu;
    int         iActiveNum,iHoldNum;
    HDC         hdc;
    PBINFO      pbNameIcon;
    COLORREF    Color;
    BOOL        bTran;

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling SetMenuSoftkey:%d\r\n",CallingType);
#endif
    switch (CallingType)
    {
    case MBCALLINNONE://û��ͨ��
#if defined(MBCALLING_RECORD)
        ShowWindow(hBRecord,FALSE);
#endif
        SetMBCallingMenu(INTERFACE_NONE);//û��ͨ��
        SendMessage(hCalling,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_LBUTTON_NONE,1),(LPARAM)"");
        SendMessage(hCalling,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BUTTON_MIDDLE,2),(LPARAM)"");
        SendMessage(hCalling,PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_RBUTTON_BACK,(LPARAM)TXT_BACK);
        SetWindowText(hCalling,CALLEND);//�޸ı���
        break;

    case MBCALLINGONE://����ͨ��
#if defined(MBCALLING_RECORD)
        ShowWindow(hBRecord,TRUE);
#endif
        MBList_ResetItem();//ɾ���෽ͨ�����нڵ�
        MBList_Show(SW_HIDE);//���ض෽ͨ���ؼ�
#if defined(SUPPORTMIDDLEBUTTON)
        SendMessage(hCalling,PWM_CREATECAPTIONBUTTON,//����
            MAKEWPARAM(IDC_BUTTON_MIDDLE,2),(LPARAM)TXT_DIALNUMBER);
#endif
        SendMessage(hCalling,PWM_CREATECAPTIONBUTTON,//�Ҷ�
            (WPARAM)IDC_RBUTTON,(LPARAM)TXT_HANGUP);

        SetWindowText(hCalling,TITLECAPTION);//ͨ��

        SetMBCallingMenu(INTERFACE_ONE);//����ͨ��

        iCallingOneNum = GetTheNUsedItem(1);
        if (-1 == iCallingOneNum)
            break;

        hMenu = PDAGetMenu(hCalling);
        if (PhoneItem[iCallingOneNum].Calls_Info.Stat == CALL_STAT_ACTIVE)
        {   //���ڼ���,���ֱ�Ϊ"����"
            ModifyMenu(hMenu,IDM_ONE_HOLD,MF_BYCOMMAND|MF_ENABLED,IDM_ONE_HOLD,TXT_HOLD);
            bHolding = FALSE;
        }
        else if (PhoneItem[iCallingOneNum].Calls_Info.Stat == CALL_STAT_HELD)//���ڱ���,���ֱ�Ϊ"����"
        {
            ModifyMenu(hMenu,IDM_ONE_HOLD,MF_BYCOMMAND|MF_ENABLED,IDM_ONE_HOLD,TXT_RESTORE);
            bHolding = TRUE;
        }
        PDADrawMenuBar(hCalling);

        hdc = GetDC(hCalling);
        if (MB_GetInfoFromPhonebook(PhoneItem[iCallingOneNum].Calls_Info.PhoneNum,&pbNameIcon))
        {   //����İ����
            if (FileIfExist(pbNameIcon.Icon))//����ͷ��,������ͼ�����ʾͷ��
            {
                RemovePortraitBitmap();//ɾ������ͨ����ʾ��ͷ��
                GetImageDimensionFromFile(pbNameIcon.Icon,&sPortrait);
                hPortrait = CreateBitmapFromImageFile(hdc,pbNameIcon.Icon,&Color,&bTran);
            }
            else
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
                printf("mb MBCalling ͷ��:%sû���ҵ�\r\n",pbNameIcon.Icon);
#endif
                RemovePortraitBitmap();//ɾ������ͨ����ʾ��ͷ��
            }
        }
        else
        {
            RemovePortraitBitmap();//ɾ������ͨ����ʾ��ͷ��
        }
        memset(cPhone,0x00,sizeof(cPhone));
        memset(cName ,0x00,sizeof(cName ));
        StrAutoNewLine(hdc,PhoneItem[iCallingOneNum].cName,cName,&rName,DT_HCENTER|DT_CENTER);
        StrAutoNewLine(hdc,PhoneItem[iCallingOneNum].Calls_Info.PhoneNum,cPhone,&rPhoneCode,DT_HCENTER|DT_CENTER);

        ReleaseDC(hCalling,hdc);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling ����ͨ�����ɺ���:%s,ԭ����:%s\r\n",cPhone,PhoneItem[iCallingOneNum].Calls_Info.PhoneNum);
#endif
        break;

    case MBCALLINGMULT://�෽ͨ��
#if defined(MBCALLING_RECORD)
        ShowWindow(hBRecord,FALSE);
#endif
        SetWindowText(hCalling,TXT_MEETING);//�绰����

#if defined(SUPPORTMIDDLEBUTTON)
        SendMessage(hCalling,PWM_CREATECAPTIONBUTTON,//����
            MAKEWPARAM(IDC_BUTTON_MIDDLE,2),(LPARAM)TXT_DIALNUMBER);
#endif

        SendMessage(hCalling,PWM_CREATECAPTIONBUTTON,//�Ҷ�
            (WPARAM)IDC_RBUTTON,(LPARAM)TXT_HANGUP);

        iActiveNum  = GetItemTypeNum(CALL_STAT_ACTIVE);//�����ͨ������
        iHoldNum    = GetItemTypeNum(CALL_STAT_HELD);//���ֵ�ͨ������

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling �෽ͨ��,iActiveNum:%d,iHoldNum:%d\r\n",iActiveNum,iHoldNum);
#endif
        if ((1 == iActiveNum) && (1 == iHoldNum))//�绰����1(1���绰����,1���绰����)
        {
            SetMBCallingMenu(INTERFACE_MULT1);
            if (AddMultCallInfo())//��Ӷ෽ͨ�������ݵ��ؼ���
            {
                MBList_Show(SW_SHOW);//��ʾ�෽ͨ���ؼ�
                MBList_SetStyle(MBLIST_WS_DISABLE);//���ÿؼ����:��ֹѡ��
            }
        }
        else if ((iActiveNum > 1) && (0 == iHoldNum))//�绰����2(ȫ���Ǽ���ͨ��)
        {
            SetMBCallingMenu(INTERFACE_MULT21);
            if (AddMultCallInfo())
            {
                MBList_Show(SW_SHOW);
                MBList_SetStyle(MBLIST_WS_DISABLE);//���ÿؼ����:��ֹѡ��
            }
        }
        else if ((0 == iActiveNum) && (iHoldNum > 1))//�绰����2(ȫ���Ǳ���ͨ��)
        {
            SetMBCallingMenu(INTERFACE_MULT22);
            if (AddMultCallInfo())
            {
                MBList_Show(SW_SHOW);
                MBList_SetStyle(MBLIST_WS_DISABLE);//���ÿؼ����:��ֹѡ��
            }
        }
        else if ((1 == iActiveNum) && (iHoldNum > 1))//�绰����3(1������ͨ��,���ౣ��)
        {
            SetMBCallingMenu(INTERFACE_MULT31);//�绰����Ϊ����
            if (AddMultCallInfo())
            {
                MBList_Show(SW_SHOW);
                MBList_SetStyle(MBLIST_WS_DISABLE);//���ÿؼ����:��ֹѡ��
            }
        }
        else if ((iActiveNum > 1) && (1 == iHoldNum))//�绰����3(1������ͨ��,���༤��)
        {
            SetMBCallingMenu(INTERFACE_MULT32);//�绰����Ϊ����
            if (AddMultCallInfo())
            {
                MBList_Show(SW_SHOW);
                MBList_SetStyle(MBLIST_WS_DISABLE);//���ÿؼ����:��ֹѡ��
            }
        }
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling �෽ͨ�� SetMenuSoftkey����\r\n");
#endif
        break;
    }
    InvalidateRect(hCalling,NULL,TRUE);
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling SetMenuSoftkey���������˳�\r\n");
#endif
    return TRUE;
}
/********************************************************************
* Function   SetMBCallingMenu  
* Purpose    ���ò˵�
* Params    
* Return    
* Remarks   
**********************************************************************/
static  void    SetMBCallingMenu(INTERFACE iMenuType)
{
    HMENU hMenu,hSubMenu,hSubMenu2;

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling SetMBCallingMenu,iMenuType:%d,InterfaceType:%d\r\n",iMenuType,InterfaceType);
#endif
    if (InterfaceType == iMenuType)//�˵�����û�иı�
        return;

    hMenu = PDAGetMenu(hCalling);
    switch (iMenuType)
    {
    case INTERFACE_NONE://û��ͨ��  
    case INTERFACE_MULT5://����ͨ��
    case INTERFACE_MULT6://�����Ҷ�
        SendMessage(GetCapture(), WM_CANCELMODE, NULL, NULL);
        DestroyMenu(hMenu);
        PDASetMenu(hCalling,NULL);
        break;
    case INTERFACE_ONE://����ͨ��
        SendMessage(hCalling,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_LBUTTON_MENU,1),(LPARAM)TXT_MENU);
        if (NULL == hMenu)
            hMenu = LoadMenuIndirect((PMENUTEMPLATE)&MBCallingMenuTemplate);
        else
            MB_RemoveAllMenu(hMenu);//ɾ�����в˵�

        if (NULL == hMenu)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling SetMBCallingMenu LoadMenuIndirect fail,hMenu == NULL,iMenuType:%d\r\n",iMenuType);
#endif
            break;
        }
        InsertMenu(hMenu,IDM_MUTE,MF_BYCOMMAND,IDM_ONE_HOLD,TXT_HOLD);//����
        SetChangelessMenu(hMenu);
        PDASetMenu(hCalling,hMenu);
        break;
    case INTERFACE_MULT1://�෽ͨ���˵�1(1���绰����,1���绰����)
        SendMessage(hCalling,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_LBUTTON_MENU,1),(LPARAM)TXT_MENU);
        if (NULL == hMenu)
            hMenu = LoadMenuIndirect((PMENUTEMPLATE)&MBCallingMenuTemplate);
        else
            MB_RemoveAllMenu(hMenu);

        if (NULL == hMenu)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling SetMBCallingMenu LoadMenuIndirect fail,hMenu == NULL,iMenuType:%d\r\n",iMenuType);
#endif
            break;
        }
        hSubMenu = CreateMenu();
        InsertMenu(hMenu   ,IDM_MUTE,MF_BYCOMMAND         ,IDM_SWITCH1     ,TXT_SWITCH);       //�л�
        InsertMenu(hMenu   ,IDM_MUTE,MF_BYCOMMAND|MF_POPUP,(DWORD)hSubMenu ,TXT_HANGUPMENU);   //�Ҷ�
        InsertMenu(hSubMenu,0       ,MF_BYPOSITION        ,IDM_HANGUPACTIVE,TXT_HANGUPACTIVE); //�Ҷϼ���ĵ绰
        InsertMenu(hSubMenu,1       ,MF_BYPOSITION        ,IDM_HANGUPHOLD  ,TXT_HANGUPHOLD);   //��������ͨ��
        InsertMenu(hSubMenu,2       ,MF_BYPOSITION        ,IDM_ALLHANGUP   ,TXT_ALLHANGUP);    //ȫ���Ҷ�
        InsertMenu(hMenu   ,IDM_MUTE,MF_BYCOMMAND         ,IDM_ALLACTIVE   ,TXT_ALLACTIVE);    //����绰����
        InsertMenu(hMenu   ,IDM_MUTE,MF_BYCOMMAND         ,IDM_SHIFT       ,TXT_SHIFT);        //ת��
        SetChangelessMenu(hMenu);
        PDASetMenu(hCalling,hMenu);
        break;
    case INTERFACE_MULT21://�෽ͨ���˵�2(ȫ��Ϊ����ͨ��)
        SendMessage(hCalling,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_LBUTTON_MENU,1),(LPARAM)TXT_MENU);
        if (NULL == hMenu)
            hMenu = LoadMenuIndirect((PMENUTEMPLATE)&MBCallingMenuTemplate);
        else
            MB_RemoveAllMenu(hMenu);

        if (NULL == hMenu)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling SetMBCallingMenu LoadMenuIndirect fail,hMenu == NULL,iMenuType:%d\r\n",iMenuType);
#endif
            break;
        }
        hSubMenu = CreateMenu();
        InsertMenu(hMenu    ,IDM_MUTE,MF_BYCOMMAND         ,IDM_SWITCH2     ,TXT_HOLD);         //����
        InsertMenu(hMenu    ,IDM_MUTE,MF_BYCOMMAND|MF_POPUP,(DWORD)hSubMenu ,TXT_MEETING);      //�绰����
        InsertMenu(hSubMenu ,0       ,MF_BYPOSITION        ,IDM_SINGLECALL  ,TXT_SINGLECALL);   //����ͨ��
        InsertMenu(hSubMenu ,1       ,MF_BYPOSITION        ,IDM_SINGLEHANGUP,TXT_SINGLEHANGUP); //�����Ҷ�
        SetChangelessMenu(hMenu);
        PDASetMenu(hCalling,hMenu);
        break;
    case INTERFACE_MULT22://�෽ͨ���˵�2(ȫ��Ϊ����ͨ��)
        SendMessage(hCalling,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_LBUTTON_MENU,1),(LPARAM)TXT_MENU);
        if (NULL == hMenu)
            hMenu = LoadMenuIndirect((PMENUTEMPLATE)&MBCallingMenuTemplate);
        else
            MB_RemoveAllMenu(hMenu);

        if (NULL == hMenu)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling SetMBCallingMenu LoadMenuIndirect fail,hMenu == NULL,iMenuType:%d\r\n",iMenuType);
#endif
            break;
        }
        InsertMenu(hMenu,IDM_MUTE,MF_BYCOMMAND,IDM_SWITCH2,TXT_RESTORE);//����
        SetChangelessMenu(hMenu);
        PDASetMenu(hCalling,hMenu);
        break;
    case INTERFACE_MULT31://�෽ͨ���˵�3(1������ͨ��,���ౣ��)
        SendMessage(hCalling,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_LBUTTON_MENU,1),(LPARAM)TXT_MENU);
        if (NULL == hMenu)
            hMenu = LoadMenuIndirect((PMENUTEMPLATE)&MBCallingMenuTemplate);
        else
            MB_RemoveAllMenu(hMenu);

        if (NULL == hMenu)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling SetMBCallingMenu LoadMenuIndirect fail,hMenu == NULL,iMenuType:%d\r\n",iMenuType);
#endif
            break;
        }
        hSubMenu = CreateMenu();
        InsertMenu(hMenu   ,IDM_MUTE,MF_BYCOMMAND         ,IDM_SWITCH2     ,TXT_SWITCH);       //�л�
        InsertMenu(hMenu   ,IDM_MUTE,MF_BYCOMMAND|MF_POPUP,(DWORD)hSubMenu ,TXT_HANGUPMENU);   //�Ҷ�
        InsertMenu(hSubMenu,0       ,MF_BYPOSITION        ,IDM_HANGUPACTIVE,TXT_HANGUPACTIVE); //�Ҷϼ���ĵ绰
        InsertMenu(hSubMenu,1       ,MF_BYPOSITION        ,IDM_HANGUPHOLD  ,TXT_HANGUPHOLD);   //��������ͨ��
        InsertMenu(hSubMenu,2       ,MF_BYPOSITION        ,IDM_ALLHANGUP   ,TXT_ALLHANGUP);    //ȫ���Ҷ�
        InsertMenu(hMenu   ,IDM_MUTE,MF_BYCOMMAND         ,IDM_ALLACTIVE   ,TXT_ALLACTIVE);    //ȫ������
        InsertMenu(hMenu   ,IDM_MUTE,MF_BYCOMMAND         ,IDM_SHIFT       ,TXT_SHIFT);        //ת��
        SetChangelessMenu(hMenu);
        PDASetMenu(hCalling,hMenu);
        break;
    case INTERFACE_MULT32://�෽ͨ���˵�3(1������ͨ��,���༤��)
        SendMessage(hCalling,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_LBUTTON_MENU,1),(LPARAM)TXT_MENU);
        if (NULL == hMenu)
            hMenu = LoadMenuIndirect((PMENUTEMPLATE)&MBCallingMenuTemplate);
        else
            MB_RemoveAllMenu(hMenu);

        if (NULL == hMenu)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling SetMBCallingMenu LoadMenuIndirect fail,hMenu == NULL,iMenuType:%d\r\n",iMenuType);
#endif
            break;
        }
        hSubMenu  = CreateMenu();
        hSubMenu2 = CreateMenu();
        InsertMenu(hMenu   ,IDM_MUTE,MF_BYCOMMAND         ,IDM_SWITCH2     ,TXT_SWITCH);        //�л�
        InsertMenu(hMenu   ,IDM_MUTE,MF_BYCOMMAND|MF_POPUP,(DWORD)hSubMenu ,TXT_HANGUPMENU);    //�Ҷ�
        InsertMenu(hSubMenu,0       ,MF_BYPOSITION        ,IDM_HANGUPACTIVE,TXT_HANGUPACTIVE);  //�Ҷϼ���ĵ绰
        InsertMenu(hSubMenu,1       ,MF_BYPOSITION        ,IDM_HANGUPHOLD  ,TXT_HANGUPHOLD);    //��������ͨ��
        InsertMenu(hSubMenu,2       ,MF_BYPOSITION        ,IDM_ALLHANGUP   ,TXT_ALLHANGUP);     //ȫ���Ҷ�

        InsertMenu(hMenu    ,IDM_MUTE,MF_BYCOMMAND|MF_POPUP,(DWORD)hSubMenu2,TXT_MEETING);       //�绰����
        InsertMenu(hSubMenu2,0       ,MF_BYPOSITION        ,IDM_ALLACTIVE   ,TXT_ALLACTIVE);     //ȫ������
        InsertMenu(hSubMenu2,1       ,MF_BYPOSITION        ,IDM_SINGLEHANGUP,TXT_SINGLEHANGUP);  //�����Ҷ�
        InsertMenu(hMenu    ,IDM_MUTE,MF_BYCOMMAND         ,IDM_SHIFT       ,TXT_SHIFT);         //ת��
        SetChangelessMenu(hMenu);
        PDASetMenu(hCalling,hMenu);
        break;
    }
    PDADrawMenuBar(hCalling);
    InterfaceType = iMenuType;

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling SetMBCallingMenu ������������\r\n");
#endif
}
/********************************************************************
* Function   MB_RemoveAllMenu  
* Purpose    ɾ�����в˵�
* Params    
* Return    
* Remarks   
**********************************************************************/
static  void    MB_RemoveAllMenu(HMENU hMenu)
{
    int iCount,i,j,iSubCount,iMenuID;
    HMENU hSubMenu;

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling MB_RemoveAllMenu��ʼɾ���˵�\r\n");
#endif
    iCount = GetMenuItemCount(hMenu);
    for (i = iCount - 1;i >= 0 ;i--)
    {
        hSubMenu = GetSubMenu(hMenu,i);
        if (hSubMenu != NULL)
        {
            iSubCount = GetMenuItemCount(hSubMenu);
            for (j = iSubCount - 1;j >= 0 ;j--)
            {
                iMenuID = GetMenuItemID(hSubMenu,j);
                if ((iMenuID == IDM_MUTE) || (iMenuID == IDM_VIEW) ||
#if defined(MBCALLING_RECORD)
                    (iMenuID == IDM_RECORD) || 
#endif
                    (iMenuID == -1))
                    continue;
                RemoveMenu(hSubMenu,j,MF_BYPOSITION);
            }
            RemoveMenu(hMenu,i,MF_BYPOSITION);
        }
        else
        {
            iMenuID = GetMenuItemID(hMenu,i);
            if ((iMenuID == IDM_MUTE) || (iMenuID == IDM_VIEW) ||
#if defined(MBCALLING_RECORD)
                (iMenuID == IDM_RECORD) || 
#endif
                (iMenuID == -1))
                continue;
            RemoveMenu(hMenu,i,MF_BYPOSITION);
        }
    }
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling MB_RemoveAllMenu�˵�ɾ�����\r\n");
#endif
}
/********************************************************************
* Function   SetChangelessMenu  
* Purpose    ���ò˵��Ĺ̶���
* Params    
* Return    
* Remarks   
**********************************************************************/
static  void    SetChangelessMenu(HMENU hMenu)
{
    ModifyMenu(hMenu,IDM_MUTE,MF_BYCOMMAND|MF_ENABLED,IDM_MUTE,
        bMute ? TXT_UNMUTE : TXT_MUTE);//����
#if defined(MBCALLING_RECORD)
    ModifyMenu(hMenu,IDM_RECORD,MF_BYCOMMAND|MF_ENABLED,IDM_RECORD,
        bRecord ? TXT_STOPRECORD : TXT_RECORD);//¼��
#endif
    ModifyMenu(hMenu,IDM_VIEW,MF_BYCOMMAND|MF_ENABLED,IDM_VIEW,TXT_VIEW);//�鿴
}
/********************************************************************
* Function   CallingDisplay  
* Purpose    ͨ���������ʾ����
* Params    
* Return    
* Remarks   
**********************************************************************/
static  void    CallingDisplay(HDC hdc)
{
    char    cLastTime[10] = "";
    char    cCostInfo[50] = "";
    int     i,iCalllingNum,iPicState;
    int     nDestX,nDestY,nDestW,nDestH;

    switch (CallingType)
    {
    case MBCALLINGONE://����ͨ��
        DrawText(hdc,cName ,strlen(cName) ,&rName     ,DT_VCENTER|DT_CENTER);//��ʾ����
        DrawText(hdc,cPhone,strlen(cPhone),&rPhoneCode,DT_VCENTER|DT_CENTER);//��ʾ����

        if (hPortrait != NULL)//��ʾͼƬ
        {
            if (iPortraitW > sPortrait.cx)
            {
                nDestX = (iPortraitW - sPortrait.cx) / 2 + iPortraitX;
                nDestW = sPortrait.cx;
            }
            else
            {
                nDestX = iPortraitX;
                nDestW = iPortraitW;
            }
            if (iPortraitH > sPortrait.cy)
            {
                nDestY = (iPortraitH - sPortrait.cy) / 2 + iPortraitY;
                nDestH = sPortrait.cy;
            }
            else
            {
                nDestY = iPortraitY;
                nDestH = iPortraitH;
            }
            StretchBlt(hdc,nDestX,nDestY,nDestW,nDestH,
                (HDC)hPortrait,0,0,sPortrait.cx,sPortrait.cy,SRCCOPY);
        }
        sprintf(cLastTime,pTime,
            PhoneItem[iCallingOneNum].TimeUsed.wHour,
            PhoneItem[iCallingOneNum].TimeUsed.wMinute,
            PhoneItem[iCallingOneNum].TimeUsed.wSecond);
        DrawText(hdc,cLastTime,strlen(cLastTime),&rTime,DT_VCENTER|DT_CENTER);//��ʾʱ��

        if (bAOCInfo)
        {
            switch (CostType)
            {
            case COSTTYPE_UNIT://��λ
                sprintf(cCostInfo,pCostInfo2,dCostInfo);//"%ld"
                break;
            case COSTTYPE_PRICE://����
                sprintf(cCostInfo,pCostInfo1,me_pucinfo.currency,dCostInfo * dCost);//"%s:%f"
                MB_DealDouble(cCostInfo,3);
                break;
            }
            DrawText(hdc,cCostInfo,strlen(cCostInfo),&rCostInfo,DT_VCENTER|DT_CENTER);
        }
        break;
    case MBCALLINGMULT://�෽ͨ��
        switch (InterfaceType)
        {
        case INTERFACE_MULT5://����ͨ��
        case INTERFACE_MULT6://�����Ҷ�
        case INTERFACE_MULT1://�෽ͨ���˵�1(1���绰����,1���绰����)
            iCalllingNum = 0;//�б���ļ���
            for (i = 0;i < iMaxCount;i++)
            {
                if ((ITEMUSED == PhoneItem[i].ItemStat) && (0 != PhoneItem[i].Calls_Info.index))
                {
                    if (CALL_STAT_ACTIVE == PhoneItem[i].Calls_Info.Stat)
                        iPicState = (bMute ? MBLIST_IPICSTATE_AM : MBLIST_IPICSTATE_A);
                    else if (CALL_STAT_HELD == PhoneItem[i].Calls_Info.Stat)
                        iPicState = (bMute ? MBLIST_IPICSTATE_HM : MBLIST_IPICSTATE_H);
                    else
                        iPicState = MBLIST_IPIC_UNCHANGE;//ͼƬ״̬:û�иı�

                    sprintf(cLastTime,pTime,
                        PhoneItem[i].TimeUsed.wHour, 
                        PhoneItem[i].TimeUsed.wMinute, 
                        PhoneItem[i].TimeUsed.wSecond);
                    MBList_SetItem(iCalllingNum,NULL,cLastTime,iPicState,-1,-1,FALSE);//����ͨ����ʱ
                    iCalllingNum++;
                }
            }
            break;
        case INTERFACE_MULT21://�෽ͨ���˵�2(ȫ��Ϊ����ͨ��)
            iPicState = (bMute ? MBLIST_IPICSTATE_AM : MBLIST_IPICSTATE_A);
            MBList_SetItem(0,NULL,NULL,iPicState,-1,-1,FALSE);
            break;
        case INTERFACE_MULT22://�෽ͨ���˵�2(ȫ��Ϊ����ͨ��)
            iPicState = (bMute ? MBLIST_IPICSTATE_HM : MBLIST_IPICSTATE_H);
            MBList_SetItem(0,NULL,NULL,iPicState,-1,-1,FALSE);
            break;
        case INTERFACE_MULT31://�෽ͨ���˵�3(1������ͨ��,���ౣ��)
            for (i = 0;i < iMaxCount;i++)
            {
                if ((ITEMUSED == PhoneItem[i].ItemStat) && (0 != PhoneItem[i].Calls_Info.index))
                {
                    if (CALL_STAT_ACTIVE == PhoneItem[i].Calls_Info.Stat)
                    {
                        iPicState = (bMute ? MBLIST_IPICSTATE_AM : MBLIST_IPICSTATE_A);
                        sprintf(cLastTime,pTime,
                            PhoneItem[i].TimeUsed.wHour, 
                            PhoneItem[i].TimeUsed.wMinute, 
                            PhoneItem[i].TimeUsed.wSecond);
                        MBList_SetItem(0,NULL,cLastTime,iPicState,-1,-1,FALSE);//����ͨ����ʱ
                        break;
                    }
                }
            }
            iPicState = (bMute ? MBLIST_IPICSTATE_HM : MBLIST_IPICSTATE_H);
            MBList_SetItem(1,NULL,NULL,iPicState,-1,-1,FALSE);//�绰����Ϊ����
            break;
        case INTERFACE_MULT32://�෽ͨ���˵�3(1������ͨ��,���༤��)
            for (i = 0;i < iMaxCount;i++)
            {
                if ((ITEMUSED == PhoneItem[i].ItemStat) && (0 != PhoneItem[i].Calls_Info.index))
                {
                    if (CALL_STAT_HELD == PhoneItem[i].Calls_Info.Stat)
                    {
                        iPicState = (bMute ? MBLIST_IPICSTATE_HM : MBLIST_IPICSTATE_H);
                        sprintf(cLastTime,pTime,
                            PhoneItem[i].TimeUsed.wHour, 
                            PhoneItem[i].TimeUsed.wMinute, 
                            PhoneItem[i].TimeUsed.wSecond);
                        MBList_SetItem(0,NULL,cLastTime,iPicState,-1,-1,FALSE);//����ͨ����ʱ
                        break;
                    }
                }
            }
            iPicState = (bMute ? MBLIST_IPICSTATE_AM : MBLIST_IPICSTATE_A);
            MBList_SetItem(1,NULL,NULL,iPicState,-1,-1,FALSE);//�绰����Ϊ����
            break;
        }
        MBList_Refresh();
        break;
    }
}
/********************************************************************
* Function   AddMultCallInfo  
* Purpose    ��Ӷ෽ͨ�������ݵ��ؼ���
* Params    
* Return    
* Remarks   
**********************************************************************/
static  BOOL    AddMultCallInfo(void)
{
    int     nItemNum,i,iPicState;
    char    cLastTime[10] = "";

    nItemNum = GetUsedItemNum();
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling AddMultCallInfo,nItemNum:%d,InterfaceType:%d\r\n",nItemNum,InterfaceType);
#endif

    if (0 == nItemNum)
        return FALSE;

    MBList_ResetItem();//����б�
    switch (InterfaceType)
    {
    case INTERFACE_MULT1://�෽ͨ���˵�1(1���绰����,1���绰����)
        for (i = 0;i < iMaxCount;i++)
        {
            if ((ITEMUSED == PhoneItem[i].ItemStat) && (0 != PhoneItem[i].Calls_Info.index))
            {
                if (CALL_STAT_ACTIVE == PhoneItem[i].Calls_Info.Stat)
                    iPicState = (bMute ? MBLIST_IPICSTATE_AM : MBLIST_IPICSTATE_A);
                else if (CALL_STAT_HELD == PhoneItem[i].Calls_Info.Stat)
                    iPicState = (bMute ? MBLIST_IPICSTATE_HM : MBLIST_IPICSTATE_H);
                else
                    iPicState = MBLIST_IPIC_UNCHANGE;//ͼƬ״̬:û�иı�

                sprintf(cLastTime, pTime, 
                    PhoneItem[i].TimeUsed.wHour, 
                    PhoneItem[i].TimeUsed.wMinute, 
                    PhoneItem[i].TimeUsed.wSecond);
                if (strlen(PhoneItem[i].cName) != 0)
                    MBList_AddItem(PhoneItem[i].cName,cLastTime,iPicState,i,
                    PhoneItem[i].Calls_Info.index);
                else
                    MBList_AddItem(PhoneItem[i].Calls_Info.PhoneNum,cLastTime,iPicState,i,
                    PhoneItem[i].Calls_Info.index);
                //����õ绰�ڽṹ�����е�λ��
            }
        }
        break;
    case INTERFACE_MULT21://�෽ͨ���˵�2(ȫ��Ϊ����ͨ��)
        iPicState = (bMute ? MBLIST_IPICSTATE_AM : MBLIST_IPICSTATE_A);
        MBList_AddItem(TXT_MEETING,NULL,iPicState,-1,-1);
        break;

    case INTERFACE_MULT22://�෽ͨ���˵�2(ȫ��Ϊ����ͨ��)
        iPicState = (bMute ? MBLIST_IPICSTATE_HM : MBLIST_IPICSTATE_H);
        MBList_AddItem(TXT_MEETING,NULL,iPicState,-1,-1);
        break;

    case INTERFACE_MULT31://�෽ͨ���˵�3(1������ͨ��,���ౣ��)
        for (i = 0;i < iMaxCount;i++)
        {
            if ((ITEMUSED == PhoneItem[i].ItemStat) && (0 != PhoneItem[i].Calls_Info.index))
            {
                if (CALL_STAT_ACTIVE == PhoneItem[i].Calls_Info.Stat)//��Ӽ�����Ŀ
                {
                    iPicState = (bMute ? MBLIST_IPICSTATE_AM : MBLIST_IPICSTATE_A);
                    sprintf(cLastTime, pTime, 
                        PhoneItem[i].TimeUsed.wHour, 
                        PhoneItem[i].TimeUsed.wMinute, 
                        PhoneItem[i].TimeUsed.wSecond);
                    if (strlen(PhoneItem[i].cName) != 0)
                        MBList_AddItem(PhoneItem[i].cName,cLastTime,iPicState,i,
                        PhoneItem[i].Calls_Info.index);
                    else
                        MBList_AddItem(PhoneItem[i].Calls_Info.PhoneNum,cLastTime,iPicState,i,
                        PhoneItem[i].Calls_Info.index);
                    //����õ绰�ڽṹ�����е�λ��
                    break;
                }
            }
        }
        //��ӱ�����Ŀ
        iPicState = (bMute ? MBLIST_IPICSTATE_HM : MBLIST_IPICSTATE_H);
        MBList_AddItem(TXT_MEETING,NULL,iPicState,-1,-1);
        break;

    case INTERFACE_MULT32://�෽ͨ���˵�3(1������ͨ��,���༤��)
        for (i = 0;i < iMaxCount;i++)
        {
            if ((ITEMUSED == PhoneItem[i].ItemStat) && (0 != PhoneItem[i].Calls_Info.index))
            {
                if (CALL_STAT_HELD == PhoneItem[i].Calls_Info.Stat)//��ӱ�����Ŀ
                {
                    iPicState = (bMute ? MBLIST_IPICSTATE_HM : MBLIST_IPICSTATE_H);
                    sprintf(cLastTime, pTime, 
                        PhoneItem[i].TimeUsed.wHour, 
                        PhoneItem[i].TimeUsed.wMinute, 
                        PhoneItem[i].TimeUsed.wSecond);
                    if (strlen(PhoneItem[i].cName) != 0)
                        MBList_AddItem(PhoneItem[i].cName,cLastTime,iPicState,i,
                        PhoneItem[i].Calls_Info.index);
                    else
                        MBList_AddItem(PhoneItem[i].Calls_Info.PhoneNum,cLastTime,iPicState,i,
                        PhoneItem[i].Calls_Info.index);
                    //����õ绰�ڽṹ�����е�λ��
                    break;
                }
            }
        }
        //��Ӽ�����Ŀ
        iPicState = (bMute ? MBLIST_IPICSTATE_AM : MBLIST_IPICSTATE_A);
        MBList_AddItem(TXT_MEETING,NULL,iPicState,-1,-1);
        break;
    case INTERFACE_MULT5://����ͨ��
    case INTERFACE_MULT6://�����Ҷ�
        for (i = 0;i < iMaxCount;i++)
        {
            if ((ITEMUSED == PhoneItem[i].ItemStat) && (0 != PhoneItem[i].Calls_Info.index))
            {
                if (CALL_STAT_ACTIVE == PhoneItem[i].Calls_Info.Stat)//��Ӽ�����Ŀ
                {
                    iPicState = (bMute ? MBLIST_IPICSTATE_AM : MBLIST_IPICSTATE_A);
                    sprintf(cLastTime,pTime, 
                        PhoneItem[i].TimeUsed.wHour, 
                        PhoneItem[i].TimeUsed.wMinute, 
                        PhoneItem[i].TimeUsed.wSecond);
                    if (strlen(PhoneItem[i].cName) != 0)
                        MBList_AddItem(PhoneItem[i].cName,cLastTime,iPicState,i,
                        PhoneItem[i].Calls_Info.index);
                    else
                        MBList_AddItem(PhoneItem[i].Calls_Info.PhoneNum,cLastTime,iPicState,i,
                        PhoneItem[i].Calls_Info.index);
                    //����õ绰�ڽṹ�����е�λ��
                }
            }
        }
        break;
    }
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling AddMultCallInfo������������!\r\n");
#endif
    return TRUE;
}
/********************************************************************
* Function     IsAllType_Specific
* Purpose      �Ƿ�ȫ��ΪiCallType����,CALL_STAT_ACTIVE,CALL_STAT_HELD
* Params       iCallType:CALL_STAT_ACTIVE,CALL_STAT_HELD������߱���
* Return       ȫ��ΪiCallType���ͷ���TRUE,���򷵻�FALSE
* Remarks      
**********************************************************************/
BOOL    IsAllType_Specific(int iCallType)
{
    int i;

    if ((iCallType != CALL_STAT_ACTIVE) && (iCallType != CALL_STAT_HELD))
        return FALSE;

    if (0 == GetUsedItemNum())
        return FALSE;
    for (i = 0;i < iMaxCount;i++)
    {
        if ((ITEMUSED == PhoneItem[i].ItemStat) && (0 != PhoneItem[i].Calls_Info.index))
        {
            if (iCallType != PhoneItem[i].Calls_Info.Stat)
                return FALSE;
        }
    }
    return TRUE;
}
/********************************************************************
* Function     InvertItemStateOne
* Purpose      �����뱣�ֽ���(����ͨ��)
* Params       index:��Ҫ���ĵĽṹ�����е�λ��
* Return           
* Remarks      
**********************************************************************/
static  BOOL    InvertItemStateOne(int index)
{
    if ((ITEMUSED == PhoneItem[index].ItemStat) && (0 != PhoneItem[index].Calls_Info.index))
    {
        if (CALL_STAT_ACTIVE == PhoneItem[index].Calls_Info.Stat)
            PhoneItem[index].Calls_Info.Stat = CALL_STAT_HELD;
        else if (CALL_STAT_HELD == PhoneItem[index].Calls_Info.Stat)
            PhoneItem[index].Calls_Info.Stat = CALL_STAT_ACTIVE;

        return TRUE;
    }
    return FALSE;
}
/********************************************************************
* Function     InvertItemStateMult
* Purpose      �����뱣�ֽ���(�෽ͨ��)
* Params       
* Return           
* Remarks      
**********************************************************************/
static  void    InvertItemStateMult(void)
{
    int i;
    for (i = 0;i < iMaxCount;i++)
    {
        if ((ITEMUSED == PhoneItem[i].ItemStat) && (0 != PhoneItem[i].Calls_Info.index))
        {
            if (CALL_STAT_ACTIVE == PhoneItem[i].Calls_Info.Stat)
                PhoneItem[i].Calls_Info.Stat = CALL_STAT_HELD;
            else if (CALL_STAT_HELD == PhoneItem[i].Calls_Info.Stat)
                PhoneItem[i].Calls_Info.Stat = CALL_STAT_ACTIVE;
        }
    }
}
/********************************************************************
* Function     SetAllType
* Purpose      ȫ����Ϊָ������
* Params       
* Return           
* Remarks      
**********************************************************************/
static  BOOL    SetAllType(int nType)
{
    int i;
    for (i = 0;i < iMaxCount;i++)
    {
        if ((ITEMUSED == PhoneItem[i].ItemStat) && (0 != PhoneItem[i].Calls_Info.index))
            PhoneItem[i].Calls_Info.Stat = nType;
    }
    return TRUE;
}
/********************************************************************
* Function     RemTime
* Purpose      ����ͨ��ʱ��
* Params       Cost:���ؼ�ʱ���,nCall:��ǰ�绰�ڽṹ�����е�λ��
* Return           
* Remarks      
**********************************************************************/
static  void    RemTime(MBRecordCostTime * Cost,int nCall)
{
    Cost->nLastHour   = PhoneItem[nCall].TimeUsed.wHour;      
    Cost->nLastMin    = PhoneItem[nCall].TimeUsed.wMinute;        
    Cost->nLastSecond = PhoneItem[nCall].TimeUsed.wSecond;    

    if (CALL_DIR_ORIGINATED == PhoneItem[nCall].Calls_Info.Dir)
    {
        Cost->nOutSecond += PhoneItem[nCall].TimeUsed.wSecond;
        if (Cost->nOutSecond >= iSecond60)
        {
            Cost->nOutSecond -= iSecond60;
            Cost->nOutMin = Cost->nOutMin + PhoneItem[nCall].TimeUsed.wMinute + 1;
        }
        else
        {
            Cost->nOutMin += PhoneItem[nCall].TimeUsed.wMinute;
        }
        if (Cost->nOutMin >= iSecond60)
        {
            Cost->nOutMin -= iSecond60;
            Cost->nOutHour = Cost->nOutHour + PhoneItem[nCall].TimeUsed.wHour + 1;
        }
        else
        {
            Cost->nOutHour += PhoneItem[nCall].TimeUsed.wHour;
        }
    }
    else if (CALL_DIR_TERMINATED == PhoneItem[nCall].Calls_Info.Dir)
    {
        Cost->nInSecond += PhoneItem[nCall].TimeUsed.wSecond;
        if (Cost->nInSecond >= iSecond60)
        {
            Cost->nInSecond -= iSecond60;
            Cost->nInMin = Cost->nInMin + PhoneItem[nCall].TimeUsed.wMinute + 1;
        }
        else
        {
            Cost->nInMin += PhoneItem[nCall].TimeUsed.wMinute;
        }
        if (Cost->nInMin >= iSecond60)
        {
            Cost->nInMin -= iSecond60;
            Cost->nInHour = Cost->nInHour + PhoneItem[nCall].TimeUsed.wHour + 1;
        }
        else
        {
            Cost->nInHour += PhoneItem[nCall].TimeUsed.wHour;
        }
    }
}
/********************************************************************
* Function     MBListCurCalls
* Purpose      �е�ǰͨ��
* Params       OperateType:��ͨ��֮ǰ�Ĳ���
* Return       �е�ǰͨ������Ϣ�Ƿ��ͳ�ȥ
* Remarks      
**********************************************************************/
BOOL    MBListCurCalls(OPERATETYPE OperateType,WPARAM wParam,LPARAM lParam)
{
    if (!IsWindow(hCalling))
        return FALSE;

    MBWaitWindowState(hCalling,TRUE,NULL,FALSE);
    if (-1 == ME_ListCurCalls(hCalling,IDC_ME_LISTCURCALLS))//�е�ǰͨ��
    {
        AppMessageBox(hCalling,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);
        bListCurCalling = FALSE;
        return FALSE;
    }
    ListCurCallswParam = wParam;
    ListCurCallslParam = lParam;
    ListCurCallsType   = OperateType;
    bListCurCalling    = TRUE;//�����е�ǰͨ��
    return TRUE;
}
/********************************************************************
* Function     OperateListCurCalls
* Purpose      �е�ǰͨ��֮��ִ�еĲ���
* Params       
* Return    �Ƿ���Ҫ���÷�����MB_ACTIVE��Ϣ,FALSE:����Ҫ,TRUE:��Ҫ   
* Remarks      
**********************************************************************/
static  BOOL    OperateListCurCalls(void)
{
    CALLS_INFO calls_info[MAX_COUNT];
    PBINFO pbNameIcon;
    int i,j,iDataLen,isour;
    int iHangup = 0;        //�绰���Ҷϵĸ���
    BOOL bAnswer = FALSE;   //�Ƿ��е绰���ӽ���
    BOOL bWaiting = FALSE;  //�Ƿ��е绰���ڵȴ�
    int iHeldNums;          //��ǰ����ͨ���ĸ���
    int iActiveNums;        //��ǰ����ͨ���ĸ���

    memset(calls_info,0x00,sizeof(calls_info));
    iDataLen = ME_GetResult(calls_info,sizeof(calls_info));

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling �е�ǰͨ��֮��ִ�еĲ���,iDataLen:%d,CallingType:%d\r\n",iDataLen,CallingType);
#endif

    if (iDataLen <= 0)//��ǰ�Ѿ�û��ͨ����
    {
        switch (CallingType)//���쵱ǰ��������
        {
        case MBCALLINNONE:
            return TRUE;
        case MBCALLINGONE:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling OperateListCurCalls-��ǰ�ǵ���ͨ������\r\n");
#endif
            AfterHangup_Single();//�ú��������ٵ�ǰ����,���Է���FALSE
            return FALSE;

        case MBCALLINGMULT:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling OperateListCurCalls-��ǰ�Ƕ෽ͨ������\r\n");
#endif
            AfterHangup_Mult_All();//�ڸú������Ѿ�������MB_ACTIVE��Ϣ,���Է���FALSE
            return FALSE;
        }
    }
#if 0
    {//����ͨ����¼
        char cCall[100] = "";
        char cCallsResult[800] = "";
        int iDataCount;

        if (iDataLen != -1)
        {
            iDataCount = iDataLen / sizeof(CALLS_INFO);
            for (i = 0;i < iDataCount;i++)
            {
                sprintf(cCall,"CI:i=%d,index=%d,Stat=%d,Dir=%d,PhoneNum=%s\r\n",
                    i,calls_info[i].index,calls_info[i].Stat,calls_info[i].Dir,calls_info[i].PhoneNum);
                strcat(cCallsResult,cCall);
            }
            strcat(cCallsResult,"\r\n");
            SaveLog(cCallsResult);

            memset(cCallsResult,0x00,sizeof(cCallsResult));
            for (i = 0;i < iMaxCount;i++)
            {
                if (PhoneItem[i].ItemStat != ITEMUSED)
                    continue;

                memset(cCall,0x00,sizeof(cCall));
                sprintf(cCall,"PI:i=%d,Name=%s,index=%d,Stat=%d,PhoneNum=%s\r\n",
                    i,PhoneItem[i].cName,PhoneItem[i].Calls_Info.index,
                    PhoneItem[i].Calls_Info.Stat,PhoneItem[i].Calls_Info.PhoneNum);
                strcat(cCallsResult,cCall);
            }
            strcat(cCallsResult,"\r\n");
            SaveLog(cCallsResult);
        }
    }
#endif
    //��ǰ��ͨ������(�п������е�ĳ��ͨ����û�б��ӽ���,�����ڵȴ�״̬)
    iHeldNums   = GetItemTypeNum(CALL_STAT_HELD);   //��ǰ����ͨ���ĸ���
    iActiveNums = GetItemTypeNum(CALL_STAT_ACTIVE); //��ǰ����ͨ���ĸ���

    iDataLen /= sizeof(CALLS_INFO);//��ǰͨ������

    //���ȼ��һ���Ƿ���ͨ���Ѿ��Ҷ�
    for (i = 0;i < iMaxCount;i++)
    {
        for (j = 0;j < iDataLen;j++)
        {
            if ((PhoneItem[i].Calls_Info.index == calls_info[j].index) &&
                (PhoneItem[i].ItemStat == ITEMUSED))
            {
                PhoneItem[i].Calls_Info.Stat = calls_info[j].Stat;//���¼���/���ֵ�״̬
                if (strcmp(PhoneItem[i].Calls_Info.PhoneNum,calls_info[j].PhoneNum) != 0)
                {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
                    printf("mb MBCalling OperateListCurCalls PhoneItem�����еĵ�%d���绰�ĺ���Ϊ:%s,calls_info�����е�%d���绰�ĺ���Ϊ:%s\r\n",
                        i,PhoneItem[i].Calls_Info.PhoneNum,j,calls_info[j].PhoneNum);
//                    {
//                        char cCall[100] = "";
//                        sprintf(cCall,"PhoneItem��%d���绰:%s,calls_info��%d���绰:%s\r\n",
//                            i,PhoneItem[i].Calls_Info.PhoneNum,j,calls_info[j].PhoneNum);
//                        SaveLog(cCall);
//                    }
#endif
                    strcpy(PhoneItem[i].Calls_Info.PhoneNum,calls_info[j].PhoneNum);
                    if (MB_GetInfoFromPhonebook(PhoneItem[i].Calls_Info.PhoneNum,&pbNameIcon))
                    {
                        isour = strlen(pbNameIcon.Name);
                        strncpy(PhoneItem[i].cName,pbNameIcon.Name,
                            sizeof(PhoneItem[i].cName) > isour ? isour + 1 : sizeof(PhoneItem[i].cName) - 1);
                    }
                }
                break;
            }
        }
        //PhoneItem�еĵ�i���绰û����calls_info�ṹ���ҵ�,����绰�����Ѿ��Ҷ���
        if ((j == iDataLen) && (PhoneItem[i].ItemStat == ITEMUSED))
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling OperateListCurCalls,i:%d,����:%s,�õ绰�Ѿ��Ҷ�\r\n",i,PhoneItem[i].Calls_Info.PhoneNum);
#endif
            AfterHangup_MultOne(i);//�෽ͨ���еı����Ҷϴ���
            iHangup++;
        }
    }

    if ((iDataLen < iHeldNums + iActiveNums) && (iHangup == 0))
    {
        //�������������ʾPhoneItem���������������ϵĽṹ��index���,��ʾ������
        for (i = 0;i < iMaxCount - 1;i++)
        {
            for (j = i + 1;j < iMaxCount;j++)
            {
                if ((i != j) && (PhoneItem[i].Calls_Info.index == PhoneItem[j].Calls_Info.index))
                {
                    PhoneItem[j].ItemStat = ITEMNOTUSED;
                }
            }
        }
    }
    //��Ϊ����绰��ʱ����Ҫ����绰��·��,�����������õ绰��·��
    SetPhoneGroupNum();

    //Ȼ����һ���Ƿ���ͨ���Ѿ�������������ڵȴ�
    for (i = 0;i < iDataLen;i++)
    {
        for (j = 0;j < iMaxCount;j++)
        {
            if ((PhoneItem[j].Calls_Info.index == calls_info[i].index) &&
                (PhoneItem[j].ItemStat == ITEMUSED))
                break;
        }
        if (iMaxCount == j)//��һ��(ֻ������һ��)�绰�Ѿ�������������ڵȴ�
        {
            if ((calls_info[i].Stat == CALL_STAT_ACTIVE) || //�绰�Ѿ���ͨ
                (calls_info[i].Stat == CALL_STAT_HELD))
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
                printf("mb MBCalling OperateListCurCalls,i:%d,����:%s,stat:%d,�õ绰�Ѿ�������\r\n",
                    i,calls_info[i].PhoneNum,calls_info[i].Stat);
#endif
                if (IsWindow_MBRing())
                {
                    MBRingAnswerCall();//����绰
                    bAnswer = TRUE;
                }
                break;
            }
            else if ((calls_info[i].Stat == CALL_STAT_INCOMING) || //�绰���ڵȴ�
                     (calls_info[i].Stat == CALL_STAT_WAITING))
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
                printf("mb MBCalling OperateListCurCalls �绰���ڵȴ�,%s,i:%d,index:%d\r\n",
                    calls_info[i].PhoneNum,i,calls_info[i].index);
#endif
                MessageMBRing(IDC_RING_HANGUP,0,0);//֪ͨ������������Ӧ�Ĵ���
                bWaiting = TRUE;//��ͨ�����ڵȴ�
                break;
            }
        }
    }

    //û�е绰���ӽ���,����ͨ�����Ҷ�,��ʱ�����ͨ��ȫ�Ǳ��ֵ�ʱ��,������Ҫ��ͨ��ȫ������
    if ((!bAnswer) && (iHangup > 0))
    {
        if (GetUsedItemNum() == 0)//��ǰû��ͨ��
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling OperateListCurCalls ��ǰû��ͨ��\r\n");
#endif
            //�������������ʾ,��ǰ��һ��ͨ�����ڵȴ�,�����ͨ��ȫ�����йҶ���
            DestroyWindow(hCalling);//�˳�ͨ������,�Ҳ���ʾͨ�������Ĵ���
            return FALSE;
        }
        if ((!bWaiting) &&                          //û��ͨ�����ڵȴ�
            (GetItemTypeNum(CALL_STAT_ACTIVE) == 0))//��ǰ����ͨ�����Ǳ��ֵ�
        {   //�����ͨ�����ڵȴ��Ļ�,ִ���л������п��ܻὫ�绰�ӽ���
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling OperateListCurCalls��ǰ����ͨ�����Ǳ��ֵ�,ִ���л�����\r\n");
#endif
            MBCalling_Shortcut(ACTIVE_SWITCH,NULL);//ִ���л�����
            return FALSE;//���ط���MB_ACTIVE
        }
    }

    switch (ListCurCallsType)
    {
    case LISTCURCALLS_HANGUP_ONE://��ͨ��֮ǰ�Ĳ����ǹҶϵ���ͨ��
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling OperateListCurCalls �����Ҷ�ʧ��\r\n");
#endif
        SendAbortCommand(ListCurCallswParam,ListCurCallslParam);//����ȡ��������at����
        AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
        break;

    case LISTCURCALLS_HANGUP_ALL://��ͨ��֮ǰ�Ĳ�����ȫ���Ҷ�
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling OperateListCurCalls ȫ���Ҷ�ʧ��\r\n");
#endif
        SendAbortCommand(ListCurCallswParam,ListCurCallslParam);//����ȡ��������at����
        AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
        break;

    case LISTCURCALLS_HANGUP_HELD://��ͨ��֮ǰ�Ĳ����ǹҶϱ��ֵ�ͨ��
        //ԭ���б��ֵ�ͨ��,���Ǳ��Ҷϵ�ͨ�����������ڱ���ͨ���ĸ���
        if ((iHeldNums > 0) && (iHangup != iHeldNums))
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling OperateListCurCalls ԭ�б���ͨ������:%d,�Ҷϸ���:%d\r\n",iHeldNums,iHangup);
#endif
            SendAbortCommand(ListCurCallswParam,ListCurCallslParam);//����ȡ��������at����
            AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
        }
        break;

    case LISTCURCALLS_HANGUP_ACTIVE://��ͨ��֮ǰ�Ĳ����ǹҶϼ����ͨ��
        //ԭ���м����ͨ��,���Ǳ��Ҷϵ�ͨ�����������ڼ���ͨ���ĸ���
        if ((iActiveNums > 0) && (iHangup != iActiveNums))
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling OperateListCurCalls ԭ�м���ͨ������:%d,�Ҷϸ���:%d\r\n",iHeldNums,iHangup);
#endif
            SendAbortCommand(ListCurCallswParam,ListCurCallslParam);//����ȡ��������at����
            AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
        }
        break;

    case LISTCURCALLS_HANGUP_CUT_X://��ͨ��֮ǰ�Ĳ����ǹҶ�ָ����ŵļ���ͨ��
        if (1 != iHangup)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling OperateListCurCalls �Ҷ�ָ����ŵļ���ͨ��ʧ��\r\n");
#endif
            SendAbortCommand(ListCurCallswParam,ListCurCallslParam);//����ȡ��������at����
            AppMessageBox(hCalling,HANGUPFAIL,TITLECAPTION,WAITTIMEOUT);
        }
        break;

    case LISTCURCALLS_ACTIVE_X://��ͨ��֮ǰ�Ĳ�������̸
        if (GetItemTypeNum(CALL_STAT_ACTIVE) != 1)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
            printf("mb MBCalling ��̸ʧ��,iActiveNums:%d,iHeldNums:%d\r\n",iActiveNums,iHeldNums);
#endif
            SendAbortCommand(ListCurCallswParam,ListCurCallslParam);//����ȡ��������at����
            AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//����ʧ��
        }
        break;

    case LISTCURCALLS_TRANSFER://��ͨ��֮ǰ�Ĳ�����ת��
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
        printf("mb MBCalling OperateListCurCalls ת��ʧ��\r\n");
#endif
        SendAbortCommand(ListCurCallswParam,ListCurCallslParam);//����ȡ��������at����
        AppMessageBox(NULL,ME_APPLY_FAIL,TITLECAPTION,WAITTIMEOUT);//����ʧ��
        break;
    }
    //����MBRingAnswerCall����֮��,����������MB_ACTIVE��Ϣ,���Է���FALSE

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling OperateListCurCalls ����:%d\r\n",bAnswer);
#endif
    return (bAnswer ? FALSE : TRUE);
}
/********************************************************************
* Function     SendAbortCommand
* Purpose      ����ȡ��������at����
* Params       
* Return           
* Remarks      
**********************************************************************/
BOOL    SendAbortCommand(WPARAM wParam,LPARAM lParam)
{
#if defined(WAVECOM_2C)
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBCALLING)
    printf("mb MBCalling SendAbortCommand,wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
    if ((wParam != ME_RS_SUCCESS) && (lParam == iAbortValue))
    {
        ME_SendATCmd(hCalling,IDC_ERROR_MESSAGE,(char *)pAbortAT,iTimeOut);
        return TRUE;
    }
    return FALSE;
#else
    return TRUE;
#endif
}
/********************************************************************
* Function     SetCostLimit
* Purpose      ���õ�ǰͨ���Ƿ��ѵ���������
* Params       bLimit:TRUE��,FALSE��
* Return           
* Remarks      
**********************************************************************/
void    SetCostLimit(BOOL bLimit)
{
    bCostLimit = bLimit;
}
//////////////////////////////////////////////////////////////////////////
/********************************************************************
* Function     SendDtmf
* Purpose      ���ͷֻ���
* Params       
* Return           
* Remarks      
**********************************************************************/
static BOOL    SendDtmf(HWND hwndCall)
{
    HWND        hWnd;
    WNDCLASS    wc;

    if (Dtmf_Type == DTMF_NONE)
        return FALSE;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AppWndProcDtmf;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pDTMFClass;

    if (!RegisterClass(&wc))
        return FALSE;

    hWnd = CreateWindow(pDTMFClass,TITLECAPTION,WS_CAPTION|WS_BORDER|PWS_STATICBAR,  
        PLX_WIN_POSITION,hwndCall,NULL,NULL,NULL);

    if (NULL == hWnd)
    {
        UnregisterClass(pDTMFClass,NULL);
        return FALSE;
    }
    return TRUE;
}
static LRESULT AppWndProcDtmf ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static  HWND    hFocus = 0;
    static  int     iExtension;//���͵ڼ����ֻ���
    static  RECT    rect;
    static  char    cDTMFClew[70] = "";
            char    cDTMF[DTMFMAXLEN + 1] = "";
            HWND    hEdit;
            LRESULT lResult;
            HDC     hdc;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        hFocus      = hWnd;
        iExtension  = 0;//���͵�0���ֻ���
        memset(cDTMFClew,0x00,sizeof(cDTMFClew));
        SetRect(&rect,iDTMFEditX,iDTMFEditY,
            GetScreenUsableWH1(SM_CXSCREEN) - iDTMFEditX * 2,
            iDTMFEditY + GetCharactorHeight(hWnd) * 2);

        SetTimer(hWnd,IDC_TIMER_DTMF,1000,NULL);//1��֮����ʾ����
        break;

    case WM_TIMER:
        switch (wParam)
        {
        case IDC_TIMER_DTMF:
            ShowWindow(hWnd, SW_SHOW);
            UpdateWindow(hWnd); 

            KillTimer(hWnd,IDC_TIMER_DTMF);
            if (Dtmf_Type == DTMF_PAUSE)
            {
                PostMessage(hWnd,IDC_ME_SUPERADDDIAL,0,0);
                sprintf(cDTMFClew,TXT_SENDINGDTMF,cPhoneNumberExtension);
            }
            else
            {
                if (IfValidDTMFNumber(cPhoneNumberExtension))//�Ϸ��ķֻ���
                {
                    sprintf(cDTMFClew,TXT_SENDDTMFCONFIRM,cPhoneNumberExtension);
                    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, //���ȷ��
                        MAKEWPARAM(IDC_BUTTON_SET,1),(LPARAM)TXT_LEFTBUTTON);
                }
                else//�Ƿ��ķֻ���,��ʾ�û���������
                {
                    hEdit = CreateWindow ("DINPUTEDIT","",
                        WS_VISIBLE|WS_CHILD|WS_TABSTOP|WS_BORDER|ES_NUMBER,
                        iDTMFEditX,rect.bottom,GetScreenUsableWH1(SM_CXSCREEN) - iDTMFEditX * 2,
                        GetEditControlHeight(),hWnd,(HMENU)IDC_DTMF_EDIT,NULL,NULL);

                    if (hEdit == NULL)
                    {
                        DestroyWindow(hWnd);
                        return FALSE;
                    }
                    hFocus = hEdit;

                    SetFocus(hFocus);
                    SendMessage(hEdit,EM_LIMITTEXT,DTMFMAXLEN,0);//�����ַ�����
                    strcpy(cDTMFClew,TEXT_DTMF_INPUT);
                }
                InvalidateRect(hWnd,&rect,TRUE);
            }
            SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,
                (WPARAM)IDC_RBUTTON,(LPARAM)TEXT_WINDOWCANCEL);
            break;
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
        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_RBUTTON, 0 );
        break;

    case WM_DESTROY ://֪ͨ���������,�˳����˳�;
        memset(cPhoneNumberExtension,0x00,sizeof(cPhoneNumberExtension));
        Dtmf_Type = DTMF_NONE;//û�зֻ���
        UnregisterClass(pDTMFClass,NULL);
        DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
        break;

    case WM_PAINT :
        hdc = BeginPaint( hWnd, NULL);
        StrAutoNewLine(hdc,cDTMFClew,NULL,&rect,DT_LEFT);
        EndPaint(hWnd, NULL);
        break;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_RETURN:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SET,0);
            break;
        case VK_F2:
        case VK_F10:
            SendMessage(hWnd,WM_COMMAND,IDC_RBUTTON,0);
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;
    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        case IDC_DTMF_EDIT:
            switch(HIWORD(wParam))
            {
            case EN_CHANGE:
                hEdit = GetDlgItem(hWnd, IDC_DTMF_EDIT);
                if (hEdit != NULL)
                {
                    if (GetWindowTextLength(hEdit) < 1)
                        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,
                        MAKEWPARAM(IDC_BUTTON_SET,1),(LPARAM)"");
                    else
                        SendMessage(hWnd,PWM_CREATECAPTIONBUTTON, //���ȷ��
                        MAKEWPARAM(IDC_BUTTON_SET,1),(LPARAM)TXT_LEFTBUTTON);
                }
                break;
            }
            break;

        case IDC_BUTTON_SET://����
            hEdit = GetDlgItem(hWnd,IDC_DTMF_EDIT);
            if (hEdit == NULL)//�Ϸ��ķֻ���
            {
                PostMessage(hWnd,IDC_ME_SUPERADDDIAL,ME_RS_SUCCESS,0);
            }
            else//��ȡ�û�������
            {
                GetWindowText(hEdit,cDTMF,GetWindowTextLength(hEdit) + 1);
                if (strlen(cDTMF) > 0)
                {
                    strcpy(cPhoneNumberExtension,cDTMF);
                    ShowWindow(hEdit,SW_HIDE);
                    SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,
                        MAKEWPARAM(IDC_BUTTON_SET,1),(LPARAM)"");
                    hFocus = hWnd;
                    SetFocus(hWnd);
                    PostMessage(hWnd,IDC_ME_SUPERADDDIAL,ME_RS_SUCCESS,0);
                }
            }
            break;
        case IDC_RBUTTON://�˳�
            DealWithRightButton(hWnd);
            break;
        }
        break;
    //***********************ME�ص���Ϣ**************************
    case IDC_ME_SUPERADDDIAL://׷�Ӳ��ŵĻص���Ϣ
        if ((cPhoneNumberExtension[iExtension] != ch_End) && //�ֻ�����δ�������
            (!GetListCurCalling()) &&                   //û���е�ǰͨ��
            (!IsAllType_Specific(CALL_STAT_HELD)))      //��ǰ����ȫ������
        {
            while ((cPhoneNumberExtension[iExtension] == ch_Plus) || 
                (cPhoneNumberExtension[iExtension] == EXTENSIONCHAR) ||
                (cPhoneNumberExtension[iExtension] == EXTENSIONCHARW))
                iExtension++;

            if (cPhoneNumberExtension[iExtension] == ch_End)
            {
                PostMessage(hWnd,IDC_ME_SUPERADDDIAL,ME_RS_SUCCESS,0);
                break;
            }

            if (-1 == ME_SendDTMF(hWnd,IDC_ME_SUPERADDDIAL,cPhoneNumberExtension[iExtension]))
            {
                PostMessage(hWnd,IDC_ME_SUPERADDDIAL,ME_RS_SUCCESS,0);
            }
            else
            {
                sprintf(cDTMFClew,TXT_SENDINGDTMF,cPhoneNumberExtension + iExtension);
                InvalidateRect(hWnd,&rect,TRUE);
            }
            iExtension++;
        }
        else//�ֻ��ŷ������
        {
            DestroyWindow(hWnd);
        }
        break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
