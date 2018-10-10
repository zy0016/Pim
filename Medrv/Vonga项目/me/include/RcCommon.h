#ifndef _RC_COMMON_H_
#define _RC_COMMON_H_

#include "winpda.h"
#include "window.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "fapi.h"
#include "pdb.h"
#include "hp_pdaex.h"

#define UNANSWER_REC	0
#define ANSWERED_REC	1
#define DIALED_REC		2
#define UNANSWER_FILE	"FLASH2:/UnAnswer.dat"
#define ANSWERED_FILE	"FLASH2:/Answered.dat"
#define DIALED_FILE		"FLASH2:/Dialed.dat"
#define RECCOST_FILE	"FLASH2:/RecCost.dat"
typedef struct tagMBRecCostTime
{
	int nLastHour;
	int nLastMin;
	int nLastSecond;
	int nOutHour;
	int nOutMin;
	int nOutSecond;
	int nInHour;
	int nInMin;
	int nInSecond;
}MBRecCostTime;

typedef struct tagMBRecCostMoney
{
	double dLast;
	double dTotal;
	double dTop;
	double dPrice;
	int Currency;
}MBRecCostMoney;

typedef struct tagMBRecCostPara
{
	struct tagMBRecCostTime Time;
	struct tagMBRecCostMoney Money;
}CostPara;

#define CostPara_Size (sizeof(struct tagMBRecCostPara))
typedef struct tagCallRecNode
{
	int  Type;
	int  index;
	SYSTEMTIME Time;
	char PhoneNo[20];
	char Text[20];
	struct tagCallRecNode* pNext;
}CallRecNode, *PCallRecNode;
#define CallRecNode_Size	(sizeof(struct tagCallRecNode))

extern BOOL CreateCallRecListFromFile(CallRecNode**, int*, int);
extern void DestroyCallRecList(CallRecNode**, int*);
extern BOOL SaveCallRecListToFile(CallRecNode**, int*, int);
extern void CallRecListAddNode(CallRecNode**, int*, CallRecNode*);
extern CallRecNode* CallRecListFindNode(CallRecNode*, int);
extern BOOL CallRecListFreeNode(CallRecNode**, int*, int);
extern BOOL CallRecListDelNode(CallRecNode**, int*, int);
extern void SortCallRecByTime(CallRecNode**);
extern BOOL ReadCostParaFile(CostPara* pCost);
extern BOOL SaveCostParaFile(CostPara* pCost);
extern BOOL GotoMBRecLstWnd(int);

#endif //_RC_COMMON_H_