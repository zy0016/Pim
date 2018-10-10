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
#include    "me_wnd.h"
#include    "MBPublic.h"
#include    "stdio.h"
#include    "stdlib.h"
#include    "../mbrecord/RcCommon.h"

#define     IDC_BUTTON_QUIT         3//�˳���ť
#define     IDC_BUTTON_SET          4//ȷ��
#define     IDC_USSD_LEFTBUTTON     6//����USSD���ܵ����
#define     IDC_USSD_RIGHTBUTTON    7//����USSD���ܵ��Ҽ�
#define     IDC_PHONE_LEFTBUTTON    8//���ڲ�ѯ��ϵ�˹��ܵ����
#define     IDC_PHONE_RIGHTBUTTON   9//���ڲ�ѯ��ϵ�˹��ܵ��Ҽ�
#define     IDC_GSMEDIT             5
#define     IDC_GSMOPERATE          WM_USER + 110//GSM�����Ļص���Ϣ
#define     IDC_USSD_NOTIFY         WM_USER + 120//ussdע����Ϣ
#define     IDC_ME_PHONE_SIM_AMX    WM_USER + 130//��ѯSIM������ϵ�˵�������
#define     IDC_CANCELOPERATE       WM_USER + 140//��־ȡ�������Ļص���Ϣ

#define     TITLECAPTION            MBGetString(STR_MBPHONE_GSM_CAPTION)    //GSM����
#define     BUTTONOK                (LPARAM)MBGetString(STR_WINDOW_OK)      //ȷ��
#define     BUTTONQUIT              (LPARAM)MBGetString(STR_WINDOW_EXIT)    //�˳�
#define     BUTTONTXTLEFT_REPLY     MBGetString(STR_MBPHONE_USSD_REPLY)     //�ظ�
#define     BUTTONTXTRIGHT          MBGetString(STR_WINDOW_BACK)            //����

#define     GSMOPERATESUCC_1        MBGetString(STR_MBPHONE_GSM_SUCC_1)     //"�����ɹ�"
#define     GSMOPERATEFAIL_1        MBGetString(STR_MBPHONE_GSM_FAIL_2)     //"����ʧ��"
#define     GSMOPERATESEARCH_FAIL   MBGetString(STR_MBPHONE_GSM_FIND_FAIL)  //��ѯʧ��
#define     GSMOPERATECANCEL        MBGetString(STR_MBPHONE_GSM_OPERATECANCEL)//����ȡ��

#define     GSMOPERATESUCC          MBGetString(STR_MBPHONE_GSM_SUCC)       //�ɹ�
#define     GSMOPERATEFAIL          MBGetString(STR_MBPHONE_GSM_FAIL)       //ʧ��
#define     GSMOPERATEACTIVATE      MBGetString(STR_MBRECORD_PUB_ACTIVATE)  //����
#define     GSMOPERATEDEACTIVATE    MBGetString(STR_MBRECORD_PUB_DEACTIVATE)//����
#define     GSMOPERATE_RA           MBGetString(STR_MBRECORD_PUB_RA)        //ע�ἤ��
#define     GSMOPERATE_UD           MBGetString(STR_MBRECORD_PUB_UD)        //ע����ֹ
#define     GSMOPERATESEARCH        MBGetString(STR_MBPHONE_GSM_FIND)       //��ѯ
#define     GSMOPERATEHAVED         MBGetString(STR_MBPHONE_GSM_HAVE)       //�� 

#define     IMEIFAIL                MBGetString(STR_MBPHONE_GSM_IMEIFAIL)   //IMEI��ѯʧ��

#define     GSMPIN1MODIFY           MBGetString(STR_MBPHONE_GSM_PIN1_MODIFY)//pin1���޸�%s
#define     GSMPIN2MODIFY           MBGetString(STR_MBPHONE_GSM_PIN2_MODIFY)//pin2���޸�%s
#define     GSMPUK1MODIFY           MBGetString(STR_MBPHONE_GSM_PUK1_MODIFY)//PIN����%s
#define     GSMPUK2MODIFY           MBGetString(STR_MBPHONE_GSM_PUK2_MODIFY)//PIN2����%s
#define     GSMBARRINGMODIFY        MBGetString(STR_MBPHONE_GSM_BARRING_MODIFY)//ע����������%s
#define     GSMUNLOCKPS             MBGetString(STR_MBPHONE_GSM_UNLOCKPS)   //"�ֻ�������%s"
#define     GSM_WRONGPASSWORD       MBGetString(STR_MBPHONE_GSM_WRONGPASSWORD)//"���������"
#define     GSMBARRING_ERROR1       MBGetString(STR_MBPHONE_GSM_BARRING_ERROR1)//"����δԤ��"
#define     GSMBARRING_ERROR2       MBGetString(STR_MBPHONE_GSM_BARRING_ERROR2)//"�������"
#define     GSMBARRING_ERROR3       MBGetString(STR_MBPHONE_GSM_BARRING_ERROR3)//"�Ƿ��Ĳ�������"

#define     GSMCALLFORWARDING1      MBGetString(STR_MBPHONE_GSM_FORWARD1)//���к���ת��
#define     GSMCALLFORWARDING2      MBGetString(STR_MBPHONE_GSM_FORWARD2)//��������ת��
#define     GSMCALLFORWARDING3      MBGetString(STR_MBPHONE_GSM_FORWARD3)//������ת��
#define     GSMCALLFORWARDING4      MBGetString(STR_MBPHONE_GSM_FORWARD4)//��Ӧ��ת��
#define     GSMCALLFORWARDING5      MBGetString(STR_MBPHONE_GSM_FORWARD5)//���ڷ�������ػ�ת��
#define     GSMCALLFORWARDING6      MBGetString(STR_MBPHONE_GSM_FORWARD6)//��æת��
#define     GSMCALLFORWARDING_NO    MBGetString(STR_MBPHONE_GSM_FORWARD_NO)//"����:"

#define     GSMCALLBARRING33        MBGetString(STR_MBPHONE_GSM_BARRING1)//�������в����绰
#define     GSMCALLBARRING330       MBGetString(STR_MBPHONE_GSM_BARRING2)//�������ͨ������
#define     GSMCALLBARRING331       MBGetString(STR_MBPHONE_GSM_BARRING3)//�������в������ʳ�;
#define     GSMCALLBARRING332       MBGetString(STR_MBPHONE_GSM_BARRING4)//�������в������ʳ�;(������)
#define     GSMCALLBARRING333       MBGetString(STR_MBPHONE_GSM_BARRING5)//������в�������
#define     GSMCALLBARRING35        MBGetString(STR_MBPHONE_GSM_BARRING6)//�������в���绰
#define     GSMCALLBARRING351       MBGetString(STR_MBPHONE_GSM_BARRING7)//���ƹ�������ʱ����
#define     GSMCALLBARRING353       MBGetString(STR_MBPHONE_GSM_BARRING8)//������в�������

#define     GSMCALLWAITING          MBGetString(STR_MBPHONE_GSM_CALLWAIT)//���еȴ�

#define     GSMCLIR                 MBGetString(STR_MBPHONE_GSM_CLIR) //CLIR%s%s
#define     GSMCLIR1                MBGetString(STR_MBPHONE_GSM_CLIR1)//"not provisioned"
#define     GSMCLIR2                MBGetString(STR_MBPHONE_GSM_CLIR2)//"provisioned in permanent mode"
#define     GSMCLIR3                MBGetString(STR_MBPHONE_GSM_CLIR3)//"unknown (e.g. no network, etc."
#define     GSMCLIR4                MBGetString(STR_MBPHONE_GSM_CLIR4)//"temporary mode presentation restricted"
#define     GSMCLIR5                MBGetString(STR_MBPHONE_GSM_CLIR5)//"temporary mode presentation allowed"

#define     GSMCLIP                 MBGetString(STR_MBPHONE_GSM_CLIP)//"CLIP%s%s"
#define     GSMCOLP                 MBGetString(STR_MBPHONE_GSM_COLP)//"COLP %s %s"
#define     GSMCOLR                 MBGetString(STR_MBPHONE_GSM_COLR)//"COLR %s %s"

#define     GSMCLIRINVOKE           MBGetString(STR_MBPHONE_GSM_CLIR_INVOKE)//"Activate CLIR %s"
#define     GSMCLIRSUPPRESS         MBGetString(STR_MBPHONE_GSM_CLIR_SUPPRESS)//"Suppress CLIR %s"

#define     GSMCLIP_NONE            MBGetString(STR_MBPHONE_GSM_CLIP_NONE)//"δ�ṩCLIP����"
#define     GSMCLIP_ALLOW           MBGetString(STR_MBPHONE_GSM_CLIP_ALLOW)//"�ṩCLIP����"
#define     GSMCLIP_UNKNOWN         MBGetString(STR_MBPHONE_GSM_CLIP_UNKNOWN)//"���δ֪"

#define     USSD_NOSERVICE          MBGetString(STR_MBPHONE_USSD_NOSERVICE)//���������Ч

#define     WITHOUTDATA             MBGetString(STR_MBPHONE_PBLIST_WITHOUTDATA)//�޺���
#define     INVALIDATENUM           MBGetString(STR_MBPHONE_INVALIDATENUMBER)//��Ч�绰����
typedef struct 
{
    HWND        hWnd;           //ִ��me����������
    int         iCallMsg;       //ִ�е���Ϣ
    char        * pPhoneNumber; //�绰����
    char        ** pGsmPre;     //��ǰ׺��ַ
    int         iStation;       //����GSMǰ׺�ĺ�����ǰ׺���ַ�е�λ��
    DWORD       wParam;         //���Ӳ���
}GSMSTRUCT;
typedef struct 
{
    const char  **pGSMPre;      //ǰ׺���ַ
    int         (* pFunction)(GSMSTRUCT * pGsmStruct);//������ַ
    DWORD       wParam;         //���Ӳ���
    BOOL        bExecuteAtOnce; 
    //ִ�з�ʽ,TRUE:��⵽�������Ļ�����ִ��,FALSE:ѡ�񲦺ż�֮����ִ��
    BOOL        bGSMLengthMatch;
    //�Ƿ����GSM����ĳ���ƥ��,��ЩGSM����ĳ����ǹ̶���,���Կ����������ɨ�������
    //Э��ƥ��,�������*#43##�����
}GSMOPERATE;
typedef enum                        //������־
{
    GSM_IMEI,                       //��IMEI��
    GSM_PIN1,                       //�ı�pin��
    GSM_PIN2,                       //�ı�pin2��
    GSM_PUK1,                       //��puk��
    GSM_PUK2,                       //��puk2��
    GSM_CALLBAR,                    //�޸ĺ���������
    GSM_UNLOCKPS,                   //�ֻ���,����

    GSM_CALLFORWARDING1,            //����ת��
    GSM_CALLFORWARDING2,            //��������ת��
    GSM_CALLFORWARDING3,            //������ת��
    GSM_CALLFORWARDING4,            //��Ӧ��ת��
    GSM_CALLFORWARDING5,            //���ڷ�������ػ�ת��
    GSM_CALLFORWARDING6,            //��æת��

    GSM_CALLFORWARDING_CHECKBEGIN,  //��ʼ��ѯ����ת�Ʋ���
    GSM_CALLFORWARDING1_CHECK,      //��ѯ����ת��
    GSM_CALLFORWARDING2_CHECK,      //��ѯ��������ת��
    GSM_CALLFORWARDING3_CHECK,      //��ѯ������ת��
    GSM_CALLFORWARDING4_CHECK,      //��ѯ��Ӧ��ת��
    GSM_CALLFORWARDING5_CHECK,      //��ѯ���ڷ�������ػ�ת��
    GSM_CALLFORWARDING6_CHECK,      //��ѯ��æת��
    GSM_CALLFORWARDING_CHECKEND,    //��ѯ����ת�Ʋ��ֽ���

    GSM_CALLBARRING33,              //�������в����绰
    GSM_CALLBARRING331,             //�������в������ʳ�;
    GSM_CALLBARRING332,             //�������в������ʳ�;(������)    
    GSM_CALLBARRING35,              //�������в���绰
    GSM_CALLBARRING351,             //���ƹ�������ʱ����
    GSM_CALLBARRING330,             //�������ͨ������
    GSM_CALLBARRING333,             //������в�������
    GSM_CALLBARRING353,             //������в�������

    GSM_CALLBARRING_CHECKBEGIN,     //��ʼ��ѯ�������Ʋ���
    GSM_CALLBARRING33_CHECK,        //�������в����绰
    GSM_CALLBARRING331_CHECK,       //�������ͨ������
    GSM_CALLBARRING332_CHECK,       //�������в������ʳ�;
    GSM_CALLBARRING35_CHECK,        //�������в������ʳ�;(������)
    GSM_CALLBARRING351_CHECK,       //������в�������
    GSM_CALLBARRING330_CHECK,       //�������в���绰
    GSM_CALLBARRING333_CHECK,       //���ƹ�������ʱ����
    GSM_CALLBARRING353_CHECK,       //������в�������
    GSM_CALLBARRING_CHECKEND,       //��ѯ�������Ʋ��ֽ���

    GSM_CALLWAIT_ACTIVE,            //������еȴ�
    GSM_CALLWAIT_CHECK,             //��ѯ���еȴ�
    GSM_CALLWAIT_DEACTIVE,          //��ֹ���еȴ�

    GSM_CLIP_CHECK,                 //CLIP check status
    GSM_CLIR_CHECK,                 //CLIR check status
    GSM_COLP_CHECK,                 //COLP check status(Connected Line identification Presentation)
    GSM_COLR_CHECK,                 //Check status COLR(connected Line identification Restriction)

    GSM_CLIR_INVOKE,                //Invoke CLIR for a voice call
    GSM_CLIR_SUPPRESS,              //Suppress CLIR for a voice call

    GSM_FILELOG_DELETE,
    GSM_FILELOG_READ,
    GSM_END                         //GSM��������
}GSMTYPE;

static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  void    OperateGSMWindow(GSMTYPE iGSM_Type,BOOL bShow,BOOL bAutoClose);
static  BOOL    GSMClewText_Succ(HWND hWnd);
static  BOOL    GSMClewText_Fail(HWND hWnd,WPARAM wParam,WPARAM lParam);

static  int     GsmIMEI(GSMSTRUCT * pGsmStruct);
static  int     GsmSecurity(GSMSTRUCT * pGsmStruct);
static  int     GsmModifyCallBarring(GSMSTRUCT * pGsmStruct);
static  int     GsmUnlockPS(GSMSTRUCT * pGsmStruct);
static  int     GsmForwardingOperate(GSMSTRUCT * pGsmStruct);
static  int     GsmForwardingCheck(GSMSTRUCT * pGsmStruct);
static  int     GsmCallBarringOperate(GSMSTRUCT * pGsmStruct);
static  int     GsmCallBarringCheck(GSMSTRUCT * pGsmStruct);
static  int     GsmCallWaitingOperate(GSMSTRUCT * pGsmStruct);
static  int     GsmPNPCheck(GSMSTRUCT * pGsmStruct);
static  int     GsmCLIRDeal(GSMSTRUCT * pGsmStruct);
static  int     ReadFileLog_Delete(GSMSTRUCT * pGsmStruct);
static  int     ReadFileLog_Read(GSMSTRUCT * pGsmStruct);

static  int     GetGSMParam(const char * pGsmCode,char * param1,
                            char * param2,char * param3);
static  void    GetGSMSC(const char * pGsmCode,char * pSC);
static  void    SetWindowTextExt(HWND hEdit,const char * pString);
static  void    StringCombination(char * pResult,const char * p1,const char * p2,
                                  const char * p3,const char * p4);

static  BOOL    USSDDealRespose(HWND hWnd,WPARAM wParam,LPARAM lParam);
static  BOOL    SetUssdInfo(HWND hWnd);
