#include	"window.h"
#include	"string.h"
#include	"stdlib.h"
#include	"stdio.h" 
#include	"winpda.h"
#include	"str_hp.h"
#include	"fapi.h"
#include	"msgwin.h"

static char InitOptionFile[] = "FLASH2:InSet.ini";//��Ҫ��ȡ���ļ�������
typedef	struct //�ļ���ʼ���ṹ
{
	int	a;
	int	b;
	int c;
}InitOption;
extern	int		GetInitOption(int iInitSign);
extern	BOOL	SetInitOption(int iInitSign,int iData);
BOOL	SaveInitOption(HANDLE hf,InitOption initoption);

#define		INITCLEW		"��ʾ"
#define		INITERROR1		"��ȡ����ʧ��"
#define		INITERROR2		"�����ļ�ʧ��"
#define		INITERROR3		"�ļ�����ʧ��"
#define		INITFSSUCC		"�ļ�����ɹ�"