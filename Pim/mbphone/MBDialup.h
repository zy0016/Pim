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
#include    "hpimage.h"
#include    "MBPublic.h"
#include    "../mbrecord/RcCommon.h"

#define DEBUG_MBDIALUP          //֧��debug״̬��ӡ���

#define MBDIALUPSTYLE_KEYBORD   //���Ž������ü��̲���
//#define MBDIALUPSTYLE_TOUCH     //���Ž������ô���������
//#define MBDIALUPSTYLE_EDIT      //���Ž������������edit�ؼ�����(û��ʵ��)

#define MBDIAL_HAVEPROJECTMODE//����������ʾ�ڲ��Ž�����Ҫ֧�ֵ��ù���ģʽ

#define     IDC_BUTTON_MIDDLE       7//�м�
#define     IDC_BUTTON_QUIT         3//�˳���ť
#define     IDC_BUTTON_SET          4//ȷ����ť

#define     IDC_TIMER_BACKSPACE     5//�����˸���ļ�ʱ��
#define     IDC_TIMER_LONGKEY       6//����*,0���ļ�ʱ��
#define     IDC_TIMER_SHORTKEY      9//�̰��Ǻż��ļ�ʱ��
#define     IDC_TIMER_SPECIAL       8//ר�ò��Ž���ȴ�ʱ��ļ�ʱ��

#define     IDC_ME_SUPERADDDIAL     WM_USER + 100//׷�Ӳ��ŵĻص���Ϣ
#define     IDC_ME_SHORTCUT         WM_USER + 110//��ݲ���
#define     IDC_INIT112             WM_USER + 90//��SIM����ʱ���͵���Ϣ

#define     TIMER_SHORTKEY          1000//�̰��Ǻż���ʱʱ��
#define     TIMER_LONGKEY           1000//����*,0����ʱʱ��
#define     TIMER_BACKSPACE         1000//�����˸����ʱ���ļ�ʱʱ��

#define     TITLECAPTION            MBGetString(STR_MBPHONE_DIAL_CAPTION)//����
#define     BUTTONOK                (LPARAM)MBGetString(STR_WINDOW_OK)  //ȷ��
#define     BUTTONQUIT              (LPARAM)MBGetString(STR_WINDOW_EXIT)//�˳�
#define     BUTTONSAVE              (LPARAM)MBGetString(STR_MBPHONE_SAVE)//����
#define     BUTTONDELETE            (LPARAM)MBGetString(STR_MBPHONE_DELETE)//ɾ��
#define     BUTTONBACK              (LPARAM)MBGetString(STR_WINDOW_BACK)//����
#define     BUTTON_MIDDLE_TEXT      MBGetString(STR_MBRECORD_LIST_LINKMAN)//��ϵ��

#define     EMERGENT                MBGetString(STR_MBPHONE_ENERGENT)//ֻ�ܽ��н�������
#define     MULTME_DIAL             MBGetString(STR_MBPHONE_DIALCLEW)//�����ٲ�����
#define     ME_APPLY_FAIL           MBGetString(STR_MBPHONE_APPLY_FAIL)//����ʧ��
#define     QUICKDIAL_FAIL          MBGetString(STR_MBPHONE_QUICKDIAL_FAIL)//���������
#define     LEFTBUTTONTEXT          MBGetString(STR_MBPHONE_DIALUP)//"����"
#define     INVALIDATENUM           MBGetString(STR_MBPHONE_INVALIDATENUMBER)//��Ч�绰����

#define     MBBTN_NUM               15//�����ܸ���

typedef enum    //������ʾ����
{
    BTN_UP,     //̧��
    BTN_DOWN    //����
}BtnState;

typedef enum                //�����ʴ�����������
{
    AREAINFO_NONE   = -2,   //���Ž������������,���ǲ�������ˢ�µĲ���
    AREAINFO_BS     = -1    //�˸������
}AreaInfo;

#if defined(MBDIALUPSTYLE_TOUCH)
typedef struct tagDialItemBtn
{
    RECT            rPicNum;        //pPicNumberͼƬ������������ڵ�����(1234567890p+#*)
    RECT            rPicBtn;        //��ťͼƬ��ʾλ��
    SIZE            sBtnNum;        //��ťͼƬ����
    HBITMAP         hBtnUp;         //��ťͼƬ���(����)
    HBITMAP         hBtnDown;       //��ťͼƬ���(����)
    unsigned int    iVKey;          //���ַ���Ӧ�ļ�ֵ
    char            ch_short;       //�ַ�
    BtnState        iState;         //����״̬
}DIALITEMBTN,PDIALITEMBTN;
#elif defined(MBDIALUPSTYLE_KEYBORD)
typedef struct tagDialItemBtn
{
    RECT            rBtnPicLarge;   //PICNUMBERͼƬ������������ڵ�����(1234567890*#+)���
    RECT            rBtnPicSmall;   //PICNUMBERͼƬ������������ڵ�����(1234567890*#+)С��
    unsigned int    iVKey;          //���ַ���Ӧ�ļ�ֵ
    char            ch_short;       //�ַ�
    int             iState;         //����״̬
}DIALITEMBTN,PDIALITEMBTN;
#endif

typedef enum//ѡ�񲦺ż�֮��ִ�еĲ�������
{
    RESULT_NOINIT,      //��ʼ����δ���
    RESULT_SUCC,        //��ɲ���
    RESULT_ERRORNUMBER, //����Ƿ�
    RESULT_GSM,         //�ú��������GSM����
    RESUTL_USSD,        //�ú��������USSD����
    RESULT_PHONENUM,    //ͨ��·������ͨ������������
    RESULT_QUICKDIAL,   //���ٲ��Ų���û����Ӧ�ļ�¼
    RESULT_NONESIM,     //û��SIM����ʱ��,�����������
    RESUTL_PROJECTMODE, //��ǰ�����ǹ���ģʽ����������
    RESULT_LINKING,     //��ǰ���ڽ��в�������
    RESULT_END          //������־
}RESULT_BYSEND;
//////////////////////////////////////////////////////////////////////////
#if defined(MBDIALUPSTYLE_TOUCH)
static  int     GetPenAreaInfo(HWND hWnd,POINT Point,BtnState BState);//���ݴ����ʵ����λ��,�����������ĸ����� 
static  BOOL    GetOldAndNewRefreshArea(HWND hWnd,LPARAM lParam,int iOldPenArea);//�������ƶ�ʱ��ˢ�±�Ҫ������
static  void    SetRefreshNumbar(HWND hWnd);//����绰�����ʱ��,ˢ����Ӧ������
static  void    MBDialup_PenUp(HWND hWnd,LPARAM lParam);//��������̧������
static  void    MBDialup_PenDown(HWND hWnd,LPARAM lParam);//�������ʰ��µ����
#endif
#if defined(MBDIALUPSTYLE_KEYBORD)
static  void    RefreshLastNumberRect(int iAdd_Del);//ˢ���ַ����� 
static  void    MBDialup_BackSpaceKey(HWND hWnd);//���Ž������ü��̲��ŵĻ�,ѡ���˸���Ĳ���
static  void    MBDialup_BackSpaceButton(HWND hWnd);//���Ž������ü��̲��ŵĻ�,ѡ���˸�ť�Ĳ���
static  void    MBDialup_Keyboard(HWND hWnd,WPARAM wParam);//���Ž������ü��̲��ŵĻ�,ѡ�񰴼��Ĳ���
static  int     GetDialitemBtnKey(unsigned int iVKey);//���ĳ����ֵ��dialitembtn�����е�λ��
#endif

static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  void    ShowPhoneNumber(HDC hdc);//��ʾ����
static  BOOL    DialNumber(HWND hWnd);//ִ�в��Ų���
static  int     GetDialitemBtnIndex(char ch);//����ĳ���ַ���dialitembtn�����е�λ��
static  BOOL    DealWithNumber(HWND hWnd);//����GSM���������
static  BOOL    APP_CallPhoneWindow(const char * pPhoneNumber,BOOL bSuperAdd,BOOL bCanSend);//�������Ŵ���
static  void    MBDialupCreate(HWND hWnd);
static  RESULT_BYSEND   DealWithNumberBySend(const char * pPhoneNumber);//ѡ�񲦺ż�֮��ִ�еĲ���
static  RESULT_BYSEND   DealWithNumberBySendLink(const char * pPhoneNumber);//��APP_CallPhoneNumber��������,����ר�ýӿ�
