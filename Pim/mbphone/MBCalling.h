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
#include    "winpda.h"
#include    "str_mb_plx.h"
#include    "hp_pdaex.h"
#include    "MBPublic.h"
#include    "hpimage.h"
#include    "MBList.h"
#include    "../mbrecord/RcCommon.h"

//#define MBCALLING_RECORD//����������ʾ����¼������
#define MBCALLING_TIMEACCURATE//ͨ�������о�ȷ��ʱ
//#define MBCALLING_TIMESIMPLE//ͨ��������һ���ʱ

#define     DTMFMAXLEN                  30

#define     TIMER_CALLING               100//��ʱ���ĺ�����ֵ
#define     TIMER_CALLING_COST          2000//ͨ���Ʒѵļ�ʱ���ĺ�����ֵ
#define     TIMER_LONGKEY               1000//�жϳ������ļ�ʱ���ĺ�����ֵ
#define     TIMER_INIT                  2000//���г�ʼ������ļ�ʱ���ĺ�����ֵ
#define     TIMER_ERROR                 5555//�����������ʱ���ĺ�����ֵ

#define     IDC_BUTTON_MIDDLE           20//�м�
#define     IDC_RBUTTON                 30//����ͨ���ľ���/�⾲����ť(��softkey)
#define     IDC_LBUTTON_NONE            40//��softkey,����û��ͨ��ʱ�������ʾ
#define     IDC_RBUTTON_BACK            50//ͨ����������ķ��ذ�ť
#define     IDC_RBUTTON_CANCEL          51//����ͨ��/�����Ҷϵ�ȡ����ť
#define     IDC_LBUTTON_CONNECT         52//����ͨ��
#define     IDC_LBUTTON_HANGUP          53//�����Ҷ�
#define     IDC_LBUTTON_MENU            54//�в˵���ʱ��,��softkey������
#define     IDC_BUTTON_RECORD           55//¼����ť��id
#define     IDC_BUTTON_SET              56//���
#define     IDC_DTMF_EDIT               57//����DTMF���ı���
#define     MB_ACTIVE                   WM_USER + 55//ͨ�������б仯��ʱ��,��Ҫ���������Ϣ,��ʾ�����б仯

#define     IDC_MBLIST                  60//�෽ͨ���Ŀؼ�

#define     IDC_TIMER_CALLING           70//ͨ����ʱ�ļ�ʱ��
#define     IDC_TIMER_COST              80//ͨ���Ʒѵļ�ʱ��
#define     IDC_TIMER_INIT              90//��ʼ����Ϣ�ļ�ʱ��
#define     IDC_TIMER_LONGKEY           100//�������ּ�0�ļ�ʱ��
#define     IDC_TIMER_ERROR             110//������ļ�ʱ��
#define     IDC_TIMER_DTMF              120//���ͷֻ��ŵļ�ʱ��

#define     IDC_ME_SHORTCUTSWITCH       WM_USER + 99//�л��Ļص���Ϣ,���ڿ�ݲ���
#define     IDC_ONE_SWITCH              WM_USER + 100//����/����Ļص���Ϣ
#define     IDC_MULTME_SWITCH1          WM_USER + 120//�෽ͨ�����л���Ϣ(1���绰����,1���绰����)
#define     IDC_MULTME_SWITCH2          WM_USER + 121//�෽ͨ�����л���Ϣ(ȫ��Ϊ����ͨ��/ȫ��Ϊ����ͨ��)
#define     IDC_MULTME_ALLACTIVE        WM_USER + 130//�෽ͨ����ȫ��������Ϣ
#define     IDC_MULTME_ALLHANGUP        WM_USER + 140//�෽ͨ����ȫ���Ҷ���Ϣ
#define     IDC_ME_LISTCURCALLS         WM_USER + 150//�е�ǰͨ���Ļص���Ϣ
//#define     IDC_PHONEBOOK               WM_USER + 160//ѡ����ϵ�˵�ʱ������֮�ص���Ϣ
#define     IDC_ME_SUPERADDDIAL         WM_USER + 170//׷�Ӳ��ŵĻص���Ϣ
#define     IDC_ME_MUTE                 WM_USER + 180//�����Ļص���Ϣ

#define     IDC_ME_SHORTOTHER           WM_USER + 200//���������Ļص���Ϣ
#define     IDC_ME_CUT_HELD             WM_USER + 210//���еȴ�ʱæ�����߹Ҷϱ��ֵ�ͨ��
#define     IDC_ME_CUT_ACTIVED          WM_USER + 220//�Ҷϼ����ͨ��
#define     IDC_ME_CUT_ACTIVED_X        WM_USER + 230//�Ҷ�ָ����ŵļ���ͨ��
#define     IDC_ME_ACTIVE_SWITCH_X      WM_USER + 250//����ָ������������ͨ����Ϊ����
#define     IDC_ME_CALL_TRANSFER        WM_USER + 270//ͨ���е�������·�绰ʱ,ʹ����·�绰����,�û���ͨ�����˳�
#define     IDC_ME_GETPRICEUNIT         WM_USER + 280//��ȡ�Ƽ���Ϣ
#define     IDC_ME_GETCCM               WM_USER + 290//��ȡ��ǰCall Meterֵ
#define     IDC_ME_REPEAT               WM_USER + 300//���е�ǰͨ��
#define     IDC_RECORD_OVER             WM_USER + 310//¼��ֹ֮ͣ���յ�����Ϣ
#define     IDC_ME_MAXPRICE             WM_USER + 320//��ѯ����ۼ�Call Meterֵ����Ϣ
#define     IDC_ERROR_MESSAGE           WM_USER + 500//�������at��Ϣ
#define     IDC_ERROR_LISTCALLING       WM_USER + 510//������Ļص���Ϣ

#define     TITLECAPTION                MBGetString(STR_MBPHONE_CALLING)//ͨ��

#define     TXT_MUTE                    MBGetString(STR_MBPHONE_MUTE)   //����
#define     TXT_UNMUTE                  MBGetString(STR_MBPHONE_UNMUTE) //����
#define     TXT_BACK                    MBGetString(STR_WINDOW_BACK)    //�˳�
#define     TXT_CONNECT                 MBGetString(STR_MBPHONE_CALLING)//ͨ��
#define     TXT_HANGUP                  MBGetString(STR_MBPHONE_HANGUP)//�Ҷ�
#define     TXT_HANGUPMENU              MBGetString(STR_MBPHONE_HANGUPEXT)//�Ҷ�
#define     TXT_CANCEL                  MBGetString(STR_MBRECORD_PUB_CANCEL)//ȡ��
#define     TXT_MENU                    MBGetString(STR_WINDOW_MENU)//�˵�
#define     TXT_DIALNUMBER              MBGetString(STR_MBPHONE_DIALNUMBER)//����
#define     TEXT_WINDOWCANCEL           MBGetString(STR_WINDOW_CANCEL)//ȡ��
#define     TXT_LEFTBUTTON              MBGetString(STR_WINDOW_OK) 
#define     TXT_FINISH                  MBGetString(STR_MBPHONE_DONE)//���

#define     DIALLINKINGWAIT             MBGetString(STR_MBPHONE_WAIT)
#define     IDS_CALLTIME                MBGetString(STR_MBRECORD_DURATION)//ͨ����ʱ
#define     SWITCHFAIL                  MBGetString(STR_MBPHONE_SWITCH_FAIL)//�л�ʧ��
#define     ACTIVEFAIL                  MBGetString(STR_MBPHONE_ACTIVE_FAIL)//����ʧ��
#define     HANGUPFAIL                  MBGetString(STR_MBPHONE_HANGUP_FAIL)//�Ҷ�ʧ��
#define     HANGUPALLCLEW               MBGetString(STR_MBPHONE_HANGUP_ALL)//ȫ���Ҷ�?
#define     CALLEND                     MBGetString(STR_MBPHONE_CALLONEEND)//ͨ������
#define     MUTEFAIL                    MBGetString(STR_MBPHONE_MUTEFAIL)//����ʧ��
#define     UNMUTEFAIL                  MBGetString(STR_MBPHONE_UNMUTEFAIL)//�⾲��ʧ��
#define     ME_APPLY_FAIL               MBGetString(STR_MBPHONE_APPLY_FAIL)//����ʧ��
#define     DIALLING_FARE               MBGetString(STR_MBPHONE_DIALLING_FARE)//"�ѵ���������"
#define     TXT_RECORD                  MBGetString(STR_MBPHONE_RECORD_BEGIN)//"¼��"
#define     TXT_STOPRECORD              MBGetString(STR_MBPHONE_RECORD_END)//"ֹͣ¼��"
#define     TXT_VIEW                    MBGetString(STR_MBPHONE_VIEW)//"�鿴"

#define     TXT_SENDINGDTMF             MBGetString(STR_MBPHONE_DTMFSENDING)//"���ڷ���%s..."
#define     TXT_SENDDTMFCONFIRM         MBGetString(STR_MBPHONE_DTMFCONFIRM)//"ȷ������%s?"
#define     TEXT_DTMF_INPUT             MBGetString(STR_MBPHONE_INPUTDTMF)//"������ֻ���:"
//**********************�˵�����**********************
#define     TXT_HOLD                    MBGetString(STR_MBPHONE_HOLDING)//����
#define     TXT_RESTORE                 MBGetString(STR_MBPHONE_RESTORE)//����
#define     TXT_SWITCH                  MBGetString(STR_MBPHONE_SWITCH)//�л�
#define     TXT_ALLACTIVE               MBGetString(STR_MBPHONE_ACTIVEALL)//����绰����
#define     TXT_ALLHANGUP               MBGetString(STR_MBPHONE_HANGUPALL)//ȫ���Ҷ�
#define     TXT_HANGUPHOLD              MBGetString(STR_MBPHONE_HANGUPHOLD)//��������ͨ��
#define     TXT_SHIFT                   MBGetString(STR_MBPHONE_SHIFT)//ת��
#define     TXT_MEETING                 MBGetString(STR_MBPHONE_MEETING)//�绰����
#define     TXT_SINGLECALL              MBGetString(STR_MBPHONE_SINGLECALL)//����ͨ��
#define     TXT_SINGLEHANGUP            MBGetString(STR_MBPHONE_SINGLEHANGUP)//�����Ҷ�
#define     TXT_HANGUP_SINGLE           MBGetString(STR_MBPHONE_HANGUP_SINGLE)//"�Ҷϵ���ͨ��"
#define     TXT_HANGUP_MEETING          MBGetString(STR_MBPHONE_HANGUP_MEETING)//"�Ҷϵ绰����"
#define     TXT_HANGUPACTIVE            MBGetString(STR_MBPHONE_HANGUP_CURRENTCALL)//"������ǰ�绰"

#define     IDM_ONE_HOLD                300//����
#define     IDM_SWITCH2                 310//�л�(ȫ��Ϊ����ͨ��)
//#define     IDM_ADDRESS                 320//��ϵ��
//#define     IDM_SMS                     330//����
//#define     IDM_MMS                     340//����
#define     IDM_SWITCH1                 350//�л�(1���绰����,1���绰����)
#define     IDM_ALLACTIVE               360//ȫ������:����ȫ��ͨ��
#define     IDM_ALLHANGUP               370//ȫ���Ҷ�:�Ҷ�ȫ���绰
#define     IDM_HANGUPHOLD              380//�Ҷϱ��ֵ�ͨ��
#define     IDM_HANGUPACTIVE            450//�Ҷϼ���ĵ绰
#define     IDM_SHIFT                   400//ת��
#define     IDM_SINGLECALL              410//����ͨ��
#define     IDM_SINGLEHANGUP            420//�����Ҷ�
#define     IDM_MUTE                    480//����
#if defined(MBCALLING_RECORD)
#define     IDM_RECORD                  460//¼��
#endif
#define     IDM_VIEW                    470//�鿴

static MENUITEMTEMPLATE MBCallingMenu[] = //ͨ���˵�
{
    {MF_STRING,IDM_MUTE  ,"",NULL},//����
#if defined(MBCALLING_RECORD)
    {MF_STRING,IDM_RECORD,"",NULL},//¼��
#endif
    {MF_STRING,IDM_VIEW  ,"",NULL},//�鿴
    {MF_END   ,0          , NULL,NULL}
};
static const MENUTEMPLATE MBCallingMenuTemplate =
{
    0,
    (PMENUITEMTEMPLATE)MBCallingMenu
};
typedef enum//��ǰͨ������
{
    MBCALLINNONE    = 0,//û��ͨ��
    MBCALLINGONE    = 1,//����ͨ��
    MBCALLINGMULT   = 2 //�෽ͨ��
}MBCALLTYPE;
typedef enum//��������
{
    INTERFACE_NONE   = 0,//û��ͨ��
    INTERFACE_ONE    = 1,//����ͨ��
    INTERFACE_MULT1  = 2,//�෽ͨ���˵�1(1���绰����,1���绰����)
    INTERFACE_MULT21 = 3,//�෽ͨ���˵�2(ȫ��Ϊ����ͨ��)
    INTERFACE_MULT22 = 4,//�෽ͨ���˵�2(ȫ��Ϊ����ͨ��)
    INTERFACE_MULT31 = 5,//�෽ͨ���˵�3(1������ͨ��,���ౣ��)
    INTERFACE_MULT32 = 6,//�෽ͨ���˵�3(1������ͨ��,���༤��)
    INTERFACE_MULT5  = 7,//����ͨ��
    INTERFACE_MULT6  = 8 //�����Ҷ�
}INTERFACE;
////////////////////////////////////////////////////////////////
static  LRESULT AppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  void    ChangeTimeOnTimer(void);//����ͨ����ӦTIMER�ı�ʱ��

static  BOOL    SetAllType(int nType);//ȫ����Ϊָ������
static  int     GetTheNUsedItem(int nTheN);//�õ�n��ʹ�õļ�¼
static  BOOL    SetMenuSoftkey(void);//�޸�ͨ������İ�ť�Լ��˵�
static  BOOL    MBTalkingWindow(void);//��ʾͨ������
static  void    CallingDisplay(HDC hdc);//ͨ���������ʾ����
static  BOOL    AddMultCallInfo(void);//��Ӷ��ͨ�������ݵ��ؼ���
static  void    InvertItemStateMult(void);//�����뱣�ֽ���(�෽ͨ��)
static  BOOL    InvertItemStateOne(int index);//�����뱣�ֽ���(����ͨ��)
static  void    RemTime(MBRecordCostTime * Cost,int nCall);//����ͨ��ʱ��
static  void    AfterHangup_Single(void);//����ͨ���е������Ҷϴ���
static  int     GetFirstNotUsedItem(void);//�õ�һ��δ��ITEM
static  void    AfterHangup_Mult_All(void);//�෽ͨ���е�ȫ���Ҷϴ���
static  BOOL    OperateListCurCalls(void);//�е�ǰͨ��֮��ִ�еĲ���
static  int     GetItemTypeNum(int iType);//�����ü�����߱��ֵ�Item����
static  void    SetMBCallingMenu(INTERFACE iMenuType);//���ò˵�
static  void    SetChangelessMenu(HMENU hMenu);//���ò˵��Ĺ̶���
static  void    ExecuteHangup(void);//����Ҷϼ��Ĳ���
static  void    SetPhoneGroupNum(void);//���õ绰��·�ĸ���
static  void    GetPriceUnit(void);//��ȡ�Ƽ���Ϣ֮��Ĵ������
static  void    MB_RemoveAllMenu(HMENU hMenu);//ɾ�����в˵�
static  void    RemovePortraitBitmap(void);//ɾ������ͨ����ʾ��ͷ��

static  LRESULT AppWndProcDtmf( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  BOOL    SendDtmf(HWND hwndCall);
