#include	"window.h"
#include	"string.h"
#include	"stdlib.h"
#include	"stdio.h" 
#include	"winpda.h"
#include	"str_hp.h"
#include	"fapi.h"
#include	"msgwin.h"

static char InitOptionFile[] = "FLASH2:InSet.ini";//需要读取的文件的名字
typedef	struct //文件初始化结构
{
	int	a;
	int	b;
	int c;
}InitOption;
extern	int		GetInitOption(int iInitSign);
extern	BOOL	SetInitOption(int iInitSign,int iData);
BOOL	SaveInitOption(HANDLE hf,InitOption initoption);

#define		INITCLEW		"提示"
#define		INITERROR1		"读取数据失败"
#define		INITERROR2		"创建文件失败"
#define		INITERROR3		"文件保存失败"
#define		INITFSSUCC		"文件保存成功"