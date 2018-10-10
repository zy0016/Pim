#ifndef _SMS_STRUCT
#define _SMS_STRUCT

//窗口对象和类名结构
typedef struct tagSmsWndClass
{
	WNDPROC		wndProc;
	char		szClassName[30];
	char		szWndCaption[30];
	PMENUTEMPLATE pmenuTemplate;

}SmsWndClass;

typedef struct tagSmsPNoTreeNode
{

	BOOL bSim;				//是否为SIM卡记录
	int nPNoNum;			//一条记录中含有几个电话号码
	char pName[20];			//姓名
	char pPNo[3][20];		//电话
	
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
	char SCANo[20];		//短信中心号码
	int	 SmsType;		//信息类型

#define SMS_TEXT	0	//文本
#define SMS_VOICE	1	//语音
#define SMS_FAX		2	//传真
#define SMS_X400	3	//X400
#define SMS_ERMES	4	//ERMES

	int SmsPeriod;		//有效期

#define SMS_1HOUR	0	//一小时
#define SMS_6HOUR	1	//六小时
#define SMS_24HOUR	2	//二十四小时
#define SMS_1WEEK	3	//一星期

	char SmsDefNo[20];	//默认发送号码

#define SMS_SIMCARD 0	//SIM卡
#define SMS_LOCAL	1	//手机

	int SmsSavePosition; //短信存储位置
}SmsConfigPara, *PSmsConfigPara;
#define ConfigPara_Size	(sizeof(struct tagSmsConfigPara))


typedef struct tagSmsNode
{
	int  Type;
#define SMS_UNREAD		0	//未读信息
#define SMS_READED		1	//已读信息
#define SMS_SENDED		2	//已发信息
#define SMS_UNSEND		3	//未发信息

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
