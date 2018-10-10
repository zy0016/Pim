#ifndef _SMS_STRUCT
#define _SMS_STRUCT

//���ڶ���������ṹ
typedef struct tagSmsWndClass
{
	WNDPROC		wndProc;
	char		szClassName[30];
	char		szWndCaption[30];
	PMENUTEMPLATE pmenuTemplate;

}SmsWndClass;

typedef struct tagSmsPNoTreeNode
{

	BOOL bSim;				//�Ƿ�ΪSIM����¼
	int nPNoNum;			//һ����¼�к��м����绰����
	char pName[20];			//����
	char pPNo[3][20];		//�绰
	
}PNoTreeNode, *PPNoTreeNode;
#define TreeNode_Size	(sizeof(struct tagSmsPNoTreeNode))

typedef struct tagPNoTreeLeaf
{
	BOOL bChecked;
	char pText[20];
	int	nY;
	HWND hChkBox;

}PNoTreeLeaf, *PPNoTreeLeaf;
#define TreeLeaf_Size	(sizeof(struct tagPNoTreeLeaf))

typedef struct tagPNoTreeRoot
{
	BOOL bChecked;
	char pText[20];
	int nY;
	HWND hChkBox;
	int  nLeafNum;
	PPNoTreeLeaf pLeaf[3];
}PNoTreeRoot, *PPNoTreeRoot;
#define TreeRoot_Size	(sizeof(struct tagPNoTreeRoot))


typedef struct tagSmsConfigPara
{
	char SCANo[20];		//�������ĺ���
	int	 SmsType;		//��Ϣ����

#define SMS_TEXT	0	//�ı�
#define SMS_VOICE	1	//����
#define SMS_FAX		2	//����
#define SMS_X400	3	//X400
#define SMS_ERMES	4	//ERMES

	int SmsPeriod;		//��Ч��

#define SMS_1HOUR	0	//һСʱ
#define SMS_6HOUR	1	//��Сʱ
#define SMS_24HOUR	2	//��ʮ��Сʱ
#define SMS_1WEEK	3	//һ����

	char SmsDefNo[20];	//Ĭ�Ϸ��ͺ���

#define SMS_SIMCARD 0	//SIM��
#define SMS_LOCAL	1	//�ֻ�

	int SmsSavePosition; //���Ŵ洢λ��
}SmsConfigPara, *PSmsConfigPara;
#define ConfigPara_Size	(sizeof(struct tagSmsConfigPara))


typedef struct tagSmsNode
{
	int  Type;
#define SMS_UNREAD		0	//δ����Ϣ
#define SMS_READED		1	//�Ѷ���Ϣ
#define SMS_SENDED		2	//�ѷ���Ϣ
#define SMS_UNSEND		3	//δ����Ϣ

	int  index;
	BOOL bReaded;
	BOOL bSave;
	char Order[4];
	char Time[ME_TIME_STRLEN];
	char PhoneNo[ME_TELEPHONE_STRLEN];
	char Context[ME_SMS_CONTEXT];
	struct tagSmsNode* pNext;
}SmsNode, *PSmsNode;
#define SmsNode_Size	(sizeof(struct tagSmsNode))

#endif
