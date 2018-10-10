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
#ifndef APP_MBPUBLIC_APP
#define APP_MBPUBLIC_APP

#include "setting.h"
#include "window.h"
#include "me_wnd.h"
#include "phonebookext.h"
#include "pubapp.h"
#include "hpime.h"
#include "string.h"
#include "../../public/setup/setup.h"

#define printf printf

#define VK_EARPHONE_KEY             VK_EARPHONE_BUTTON

#define KEYWELL                     VK_F4//���ż��ļ�ֵ
#define KEYASTERICK                 VK_F3//�Ǻż��ļ�ֵ

//#define KEYSWITCHTYPE_LONG  //ѡ���Ǻ�,0�ż���ʱ��,�л��������ַ��ķ�ʽ:��������ʽ
#define KEYSWITCHTYPE_SHORT //ѡ���Ǻż���ʱ��,�л��������ַ��ķ�ʽ:�̰�����ʽ

#define SHOWDEBUGINFO   //�Ƿ���ʾ�������
//#define SUPPORTMIDDLEBUTTON//֧���м��

#define PHONENUMMAXLEN              40  //�绰������󳤶�
#define MAX_COUNT                   5   //�绰�����·��
#define EXTENSIONCHAR               'p' //�ֻ��ķָ���p
#define EXTENSIONCHARW              'w' //�ֻ��ķָ���w
#define EXTENSIONSTRING             "p"

typedef enum
{
    DTMF_NONE,  //û�зֻ���
    DTMF_PAUSE, //p
    DTMF_WAIT   //w
}DTMF_TYPE;//�ֻ��ŵ����
//�е�ǰͨ��֮ǰ�Ĳ���
typedef enum
{
    LISTCURCALLS_NONE         ,//û�в���
    LISTCURCALLS_HANGUP_ONE   ,//����ͨ���ĹҶ�
    LISTCURCALLS_HANGUP_ALL   ,//�Ҷ�����ͨ��
    LISTCURCALLS_HANGUP_HELD  ,//�Ҷϱ��ֵ�ͨ��
    LISTCURCALLS_HANGUP_ACTIVE,//�Ҷϼ����ͨ��
    LISTCURCALLS_HANGUP_CUT_X ,//�Ҷ�ĳ��ͨ��
    LISTCURCALLS_SWITCH       ,//�л�
    LISTCURCALLS_REPEAT       ,//���е�ǰͨ��
    LISTCURCALLS_TRANSFER     ,//ת��
    LISTCURCALLS_ACTIVE_X      //��̸
}OPERATETYPE;

typedef struct tagPhoneItem
{
    char        cName[PHONENUMMAXLEN + 1];  //��ʾ����������
    CALLS_INFO  Calls_Info;
#define ITEMUSED    1                   //�ýڵ�����ͨ����
#define ITEMNOTUSED 0                   //�ýڵ��Ѿ��Ҷ�
    int         ItemStat;               //�ڵ�״̬,�Ƿ�����ͨ��
    SYSTEMTIME  TimeUsed;               //ͨ����ʱ
    SYSTEMTIME  TimeInDial;             //��������ʱ��/����ʱ��
}PHONEITEM, *PPHONEITEM;

#define WM_VOICE_HANGUPONE          WM_USER + 999//ͨ�������е���ͨ���ĹҶ���Ϣ
#define WM_CALLING_HANGUP           WM_USER + 998//�յ��Է��Ҷ���Ϣ
#define IDC_RING_HANGUP             WM_USER + 997
//���������յ������Ϣ��ʾ��ǰ�в���ͨ�����Ҷ�,��Ҫ���Ͻ�����Ӧ�Ĵ���

#define IDC_FORWARDING              WM_USER + 996
//���������յ������Ϣ��ʾ��ǰ�����Ǳ�����ֻ�ת�ƹ�����
//���ú궨��
#define MESSAGEBOXTITLE             MBGetString(STR_MBPHONE_CLEW)
#define CONFIRM_OK                  MBGetString(STR_WINDOW_OK)
#define CONFIRM_CANCEL              MBGetString(STR_WINDOW_CANCEL)

//////////ͼƬ����////////////////////////////////////////////////////////////////
#define MBPIC_VIEWLIST1             "Rom:/notepad.ico"
#define MBPIC_VIEWLIST2             "Rom:/dateapp.ico"
#define MBPIC_VIEWLIST3             "Rom:/contact.ico"
#define MBPIC_VIEWLIST4             "Rom:/mu_sms.ico"
#define MBPIC_VIEWLIST5             "Rom:/mu_mms.ico"
#define MBPIC_VIEWLIST6             "Rom:/mail_mal.ico"

#define MBPIC_DIALLINK              "ROM:/mb_linking.gif"//�������Ӷ������ļ���

#define MBPIC_DIALUP_NUMBER         "ROM:/dialnums.gif"//����ͼƬ
#define MBPIC_DIALUP_NUMBAR         "ROM:/diabar.gif"//�������ֵ�����ͼƬ
#define MBPIC_DIALUP_BSUP           "ROM:/dialarr.gif"//�˸����ͼƬ(����)
#define MBPIC_DIALUP_BSDOWN         "ROM:/backdown.gif"//�˸����ͼƬ(����)

#define MBPIC_DIALUP_BUTTONUP0      "ROM:/gsmkeyu0.gif"
#define MBPIC_DIALUP_BUTTONUP1      "ROM:/gsmkeyu1.gif"
#define MBPIC_DIALUP_BUTTONUP2      "ROM:/gsmkeyu2.gif"
#define MBPIC_DIALUP_BUTTONUP3      "ROM:/gsmkeyu3.gif"
#define MBPIC_DIALUP_BUTTONUP4      "ROM:/gsmkeyu4.gif"
#define MBPIC_DIALUP_BUTTONUP5      "ROM:/gsmkeyu5.gif"
#define MBPIC_DIALUP_BUTTONUP6      "ROM:/gsmkeyu6.gif"
#define MBPIC_DIALUP_BUTTONUP7      "ROM:/gsmkeyu7.gif"
#define MBPIC_DIALUP_BUTTONUP8      "ROM:/gsmkeyu8.gif"
#define MBPIC_DIALUP_BUTTONUP9      "ROM:/gsmkeyu9.gif"
#define MBPIC_DIALUP_BUTTONUPX      "ROM:/gsmkeyux.gif"
#define MBPIC_DIALUP_BUTTONUPJ      "ROM:/gsmkeyuj.gif"

#define MBPIC_DIALUP_BUTTONDOWN0    "ROM:/gsmkeyd0.gif"
#define MBPIC_DIALUP_BUTTONDOWN1    "ROM:/gsmkeyd1.gif"
#define MBPIC_DIALUP_BUTTONDOWN2    "ROM:/gsmkeyd2.gif"
#define MBPIC_DIALUP_BUTTONDOWN3    "ROM:/gsmkeyd3.gif"
#define MBPIC_DIALUP_BUTTONDOWN4    "ROM:/gsmkeyd4.gif"
#define MBPIC_DIALUP_BUTTONDOWN5    "ROM:/gsmkeyd5.gif"
#define MBPIC_DIALUP_BUTTONDOWN6    "ROM:/gsmkeyd6.gif"
#define MBPIC_DIALUP_BUTTONDOWN7    "ROM:/gsmkeyd7.gif"
#define MBPIC_DIALUP_BUTTONDOWN8    "ROM:/gsmkeyd8.gif"
#define MBPIC_DIALUP_BUTTONDOWN9    "ROM:/gsmkeyd9.gif"
#define MBPIC_DIALUP_BUTTONDOWNX    "ROM:/gsmkeydx.gif"
#define MBPIC_DIALUP_BUTTONDOWNJ    "ROM:/gsmkeydj.gif"

#define MBPIC_MBLIST_ACTIVE         "ROM:/mbincall.gif"//���ڼ����ͼ��
#define MBPIC_MBLIST_HOLD           "ROM:/mbhold.gif"  //���ڱ��ֵ�ͼ��
#define MBPIC_MBLIST_AM             "ROM:/mbam.gif"    //�����
#define MBPIC_MBLIST_HM             "ROM:/mbhm.gif"    //���־���

#define MBPIC_RECORD_UNANSWER       "Rom:/mbunan.ico"//δ�ӵ绰
#define MBPIC_RECORD_ANSWER         "Rom:/mbansw.ico"//�ѽӵ绰
#define MBPIC_RECORD_DIALED         "Rom:/mbdialed.ico"//�Ѳ��绰
#define MBPIC_RECORD_CALLTIME       "Rom:/mbtime.ico"//ͨ����ʱ
#define MBPIC_RECORD_CALLCOST       "Rom:/callcost.ico"//ͨ���Ʒ�
#define MBPIC_RECORD_COSTTYPE       "Rom:/costtype.ico"//�Ʒѷ�ʽ
#define MBPIC_RECORD_SETCOST        "Rom:/costlimit.ico"//�����޶�

#define MBPIC_RECORDLIST_IN         "ROM:mb_in.gif" //����ͼ��
#define MBPIC_RECORDLIST_OUT        "ROM:mb_out.gif"//����ͼ��
#define MBPIC_RECORDLIST_UN         "ROM:mb_unan.gif"//δ��ͼ��
//��������
#if defined(_EMULATE_)
void    printf( char * format, ... );
#endif
//ͨ������ר�ú���
BOOL    EarphoneStatus(void);//�Ƿ��ж���
void    bInitMBDialup(void);//��ʼ������
void    bInitMBLink(void);//��ʼ������
void    InitMBCalling(void);//��ʼ��ͨ����������ͼƬ��Ϣ
BOOL    APP_CallPhone(const char * pPhoneNumber);//���ò��Ž���
BOOL    APP_CallPhoneNumber(const char * pPhoneNumber);//���ò��Ž��沢��ֱ�Ӳ���ȥ
BOOL    MBCallEndWindow(const char * pName,const char * pPhoneCode,const char * pDuration,const char * pCost);//ͨ����������
BOOL    SuperAddDialNumber(const char * pPhoneNumber,BOOL bSuperAdd,BOOL bCanSend);//����׷�Ӳ��Ž���
void    SetCostLimit(BOOL bLimit);//���õ�ǰͨ���ж��Ƿ�����Ϊ��������

BOOL    MBCallingWindow(const PHONEITEM * pPhoneItem,const char * pExtension,DTMF_TYPE dtmf_type);//�ɽ���/�������ܵ���
BOOL    IsWindow_MBCalling(void);//��ǰ�Ƿ����ͨ������
BOOL    IsWindow_MBRing(void);//��ǰ�Ƿ��ǽ�������
BOOL    IsWindow_DialLink(void);//��ǰ�Ƿ������ӽ���
BOOL    IsWindow_Dialup(void);//��ǰ�Ƿ��ǲ��Ž���
BOOL    MBPhoneRing(const CALLS_INFO * pNewItemInfo);//��ʾ�ӵ绰����ĺ���
void    GetNewItemInfo(CALLS_INFO * pNewItemInfo);//����������Ϣ
void    SetnSelItem(int n);//���õ�ǰѡ����
int     GetUsedItemNum(void);//������Item����
int     GetPhoneGroupNum(void);//��õ绰��·�ĸ���
int     AfterHangup_MultOne(int iIndex);//�෽ͨ���еĹҶϴ���
BOOL    MBCalling_Shortcut(int iType,const char * pShortPhone);//�෽ͨ���еĿ�ݴ���
int     GsmSpecific(const char * pNumber);//��ǰ�����ǰ׺�Ƿ���һ��GSM���б�־
BOOL    IsAllType_Specific(int iCallType);//�Ƿ�ȫ��Ϊ������߱�������
void    SetListCurCalls(BOOL bListCur);//�����Ƿ������е�ǰͨ��
BOOL    GetListCurCalling(void);//����Ƿ������е�ǰͨ��
BOOL    MBListCurCalls(OPERATETYPE OperateType,WPARAM wParam,LPARAM lParam);//�е�ǰͨ��
void    MBRingAnswerCall(void);//�����绰֮��Ĵ���
BOOL    GetInitState(void);//��ô�绰���ֳ�ʼ���Ƿ����
void    SendMBDialLinkDialInfo(int iType);//�������ӽ�������Żص���Ϣ

void    DestroyViewList(void);//������ʾ�б���
void    MessageMBRing( UINT wMsgCmd, WPARAM wParam, LPARAM lParam);//��������淢����Ϣ
void    MessageMBDialUp(UINT wMsgCmd,WPARAM wParam,LPARAM lParam);//�򲦺Ž��淢����Ϣ
void    MessageMBCallingOne(UINT wMsgCmd, WPARAM wParam, LPARAM lParam);//�򵥷�ͨ�����淢����Ϣ
void    MessageMBPostMsg(int iCallMsg,WPARAM wParam, LPARAM lParam);//���ײ���Ϣ�ַ����ڷ�����Ϣ
void    MessageMBPostMsgCallForward(void);//���¿�ʼ��ѯ�йغ���ת�Ƶ���Ϣ

BOOL    SendAbortCommand(WPARAM wParam,LPARAM lParam);//����ȡ��������at����
BOOL    CreateMBGSMWindow(void);//����һ����������GSM����
BOOL    CallMBDialLinkingWindow(const char * pPhoneCode,const SYSTEMTIME * pDialupTime);//�����������ӵĽ���
void    GetCurrentPhoneNum(char * pPhoneNum,unsigned int iBuflen);//��õ�ǰ����ĵ绰����
BOOL    GetSceneModeCall(void);//��õ�ǰ�Ƿ�������ź���ģʽ
BOOL    ReadMobileConfigFile(MBConfig * pConfig);//��ȡͨ�����õ���Ϣ
void    ReadMobileConfig(MBConfig * pConfig);//��ȡͨ�����õ���Ϣ
BOOL    MB_SetHeadphone(HWND hwndCall,int iDir);//ͨ�������е�������
BOOL    GSMDealWith(const char * cPhoneNumber,BOOL bExecuteAtOnce);//��GSM�б��в����ַ���,ִ����Ӧ�ĺ���
BOOL    USSDDealWith(const char * pString,BOOL bCalling);//USSD����
BOOL    GSM_PhoneBookDealWith(const char * pPhoneNumber);//�������ּ�+#������ϵ�˼�¼�Ĺ���

BOOL    MB_GetInfoFromPhonebook(const char* pPhone, PBINFO* pInfo);//����ϵ�˴�������ݵĺ���
int     MB_GetDirectDial(WORD wDDIdx, char* pNo);//����ϵ�˴���ÿ��ٲ����������ݵĺ���
int     MB_GetSIMRcdByOder(int iOrder,PPHONEBOOK pRtnRcd);//��������ţ��ǿռ�ţ�ȡSIM����¼
BOOL    MB_SaveToPhoneBook(HWND hWnd,const char * pNumber);//����绰���뵽��ϵ��
void    MB_SendSms(const char * pNumber);//���Ͷ���
void    MB_SendMms(const char * pNumber);//���Ͳ���
//////////////////��������////////////////////////////////////////////////////////
void    MBPlayMusic(int iMult);//������������

void    SetForward_All(BOOL bForward);//�����Ƿ��������ת��
void    SetForward_Uncdtional(BOOL bForward);//�����Ƿ����������ת��
void    SetForward_Condition(BOOL bForward);//�����Ƿ������������ת��
void    SetForward_Busy(BOOL bForward);//�����Ƿ������æת��
void    SetForward_NoReply(BOOL bForward);//�����Ƿ������Ӧ��ת��
void    SetForward_NoReach(BOOL bForward);//�����Ƿ���ڲ��ڷ�������ػ�ת��
BOOL    GetForward_Condition(void);//�����Ƿ��������ת��
BOOL    GetForward_Uncdtional(void);//�����Ƿ����������ת��

void    EnableState(void);//ͨ��֮��ִ�еĲ���
void    DisableState(void);//ͨ��֮ǰִ�еĲ���

void    DealWithRightButton(HWND hWnd);//������������ı���ؼ�,ѡ���Ҽ���ť֮��Ĳ���
void    SetRightButtonText(HWND hWnd,WPARAM wParam,LPARAM lParam);//������������ı���ؼ�,�����Ҽ���ť�ϵ�����
int     GetIntArrayMaxValue(int * pArray,size_t count);//�������������Ԫ�ص����ֵ
int     TimeCmp(const SYSTEMTIME * pTime1,const SYSTEMTIME * pTime2);//�Ƚ�ʱ��
//char  * ecvt(double value,int ndigit,int * dec,int * sign);
char  * MB_Mtoa(double dNumber,char * pNumber);//������ת�����ַ���
double  MB_atof(const char * pString);//�ַ���ת���ɸ�����
void    MB_DealDouble(char * pSour,unsigned int iBit);//���ַ����е�������,���
BOOL    FileIfExist(const char * pFileName);//���鵱ǰ�ļ��Ƿ����
BOOL    StrIsNumber(const char * pStr);//��ǰ�ַ����Ƿ������ȫת��������
BOOL    StrIsDouble(const char * pStr);//��ǰ�ַ����Ƿ������ȫת����double����
BOOL    IfValidPhoneNumber(const char * pPhoneNumber);//��ǰ�ַ����Ƿ��ǺϷ��ĵ绰����
BOOL    IfValidDTMFNumber(const char * pStr);//��ǰ�ַ����Ƿ��ǺϷ��ķֻ���
void    PlayClewMusic(unsigned int iPlayCount);//������ʾ��
BOOL    GetPhoneItem(PHONEITEM * pPhoneItem,const char * pNumber);//Ϊͨ����Ϣ�ṹ�е�����,���븳ֵ
int     InStrArray(const char * pSearchStr,const char ** pArray);//��ָ�������в����ַ���pSearchStr
int     InIntArray(int iSearchInt,const int * pArray,size_t count);//��ָ�������в�������iSearchStr
int     InCharArray(char ch,const char * pArray,size_t count);//��ָ�������в����ַ�ch
BOOL    NumberIsEmergent(const char * pNumber,BOOL bPureNumber);//��ǰ�����Ƿ�����绰����
DTMF_TYPE   IfExistExtension(const char * pPhoneNumber,char * pPureNumber,
                                    char * pExtension);//�绰�������Ƿ�����ֻ���
void    SetLibrateIfSwitch(void);//
int     FindSIMState(BOOL bShow);//����SIM������
BOOL    MBWaitWindowState(HWND hWnd,BOOL bState,int icallmsg,BOOL bRButton);//��ʾ��ʾ��
BOOL    CountControslYaxisExt(int * piConY,unsigned int iControlH,size_t count,int iDirect,unsigned int iSpace);//����ؼ�������ĺ���
BOOL    CountControlsYaxis(int * piConY,unsigned int iControlH,size_t count);//����ؼ�������ĺ���
BOOL    CountControlsAbscissa(int * pConX,unsigned int iControlW,size_t count);//����ؼ�������ĺ���
int     StrAutoNewLine(HDC hdc,const char * cSourstr,char * cDesstr,const RECT * pRect,UINT uFormat);
int     GetCharactorHeight(HWND hWnd);//��õ����ַ��ĸ߶�
int     GetStringWidth(HDC hdc,const char * pString);//���һ���ַ����Ŀ��
int     GetScreenUsableWH1(int nIndex);//�����Ļ�п������õĿ�Ȼ��߸߶�
void    GetScreenUsableWH2(int * pwidth,int * pheight);//�����Ļ�п������õĿ�Ⱥ͸߶�
void    AppMessageBox(HWND hwnd,const char * szPrompt,const char * szCaption,int nTimeout);
BOOL    AppConfirmWin(HWND hwnd,const char * szPrompt,const char * szTipsPic,
                      const char * szCaption,const char * szOk,const char * szCancel);
char *  TrimString(char * pStr);//ɾ���ַ�����ͷ��β�Ŀո��ַ�
HMENULIST   LoadMenuList(MENULISTTEMPLATE * MenuListElement,BOOL bLostkeyNotify);//����HMENULIST
void    MenuList_AddString(HWND hMenuList,char ** p,int * id,char ** pic);//���¼������ݵ�Menulist��
void    MenuList_AddStringOne(HWND hMenuList,const char * pItemName,int id,const char * pIconName);//����һ�����ݵ�Menulist��
BOOL    ResetMenuList(HWND hMenuList,int * id);//���MENULIST�е�ȫ������
BOOL    CallReplyControlWindow(HWND hwndCall,const char * pTitle,
                               unsigned int iEditMaxLen,char * pResult,
                               const char * pBLeftTxt,const char * pBRightTxt,
                               DWORD dwFlags);
int     GetEditControlHeight(void);//���EDIT�ؼ����ʵĸ߶�
int     GetButtonControlHeight(void);//���Button�ؼ����ʵĸ߶�
int     GetRadioButtonControlWidth(const char * pString);//��õ�ѡ��ť�ؼ���Ҫ�Ŀ��
int     GetInputSoftKeyboardPlace(void);//�����Ļ�������뷨��λ��
void    GetFactFileNameFromPath(const char * pFilePathName,char * pFileName,unsigned int iBuflen,BOOL bExName);//�Ӵ�·�����ļ�������,��ô��ļ���
void    EndPicBrowser();//����ر�ͼƬԤ��
BOOL    IsSpareTimeLeft(void);//¼��ʱ���Ƿ�����
BOOL    EndRecorderUnvisualable(void);
BOOL    StartRecorderUnvisualable(HWND hWnd, UINT uRetMsg);
BOOL    MBViewList(HWND hwndCall);
void    PhoneSetChannel(BOOL bSet);//����ͨ��
void    PhoneSetChannelCalling(BOOL bSet);//����ͨ��

void    SaveLog(const char * pString);
DWORD   ReadLog(char * pString);
void    RemoveLog(void);
//////////////////////////////////////////////////////////////////////////
#endif
