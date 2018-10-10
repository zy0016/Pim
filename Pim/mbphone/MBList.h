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
#ifndef _MBLIST_
#define _MBLIST_
//�ؼ���Ϣ
#define MBLIST_SELCHANGE        1//�ı������ѡ�����Ϣ
#define MBLIST_DBLCLK           2//˫������

//�ؼ����
#define MBLIST_WS_DISABLE       1//��ֹѡ��
#define MBLIST_WS_ENABLED       2//����ѡ��

#define MBLIST_IPIC_UNCHANGE    -1//ͼƬ״̬:û�иı�
#define MBLIST_IPICNONE         0//ͼƬ״̬:û��
#define MBLIST_IPICSTATE_A      1//ͼƬ״̬:����
#define MBLIST_IPICSTATE_H      2//ͼƬ״̬:����
#define MBLIST_IPICSTATE_AM     4//ͼƬ״̬:�����
#define MBLIST_IPICSTATE_HM     8//ͼƬ״̬:���־���
//����MBList�ؼ�
HWND    CreateMBListControl(HWND hwndParent,int iId,int x,int y,int nWidth,int nHeight,DWORD dwFlags);
//�������
int     MBList_AddItem(const char * pFirstStr,const char * pSecondStr,
                       int iPicState,int iData,int iCallId);

//ɾ����iIndex���ڵ�
int     MBList_DeleteItem(int iIndex);
//���ٴ���
void    MBList_Destroy(void);

//iCallId�Ƿ���������Ľڵ��д���
BOOL    MBList_ExistCallId(int iCallId);

//�������ڵ���
int     MBList_GetCount(void);
//����ѡ�еĽڵ�����
int     MBList_GetCurSel(void);
//��õ�iIndex���ڵ������
int     MBList_GetItem(int iIndex);
//��õ�iIndex���ڵ�ĵ�һ�еڶ�������
BOOL    MBList_GetString(int iIndex,char * pFirstStr,char * pSecondStr);
//ˢ�µ�ǰ�б�
void    MBList_Refresh(void);
//ɾ�����нڵ�
void    MBList_ResetItem(void);

//����ѡ�е�index���ڵ������
void    MBList_SetCurSel(int index);
//���õ�iIndex���ڵ������
void    MBList_SetItem(int iIndex,const char * pFirstStr,const char * pSecondStr,
                       int iPicState,int iData,int iCallId,BOOL bRefresh);
//���ÿؼ����
void    MBList_SetStyle(DWORD dwStyle);
//���ֻ������ؿؼ�
BOOL    MBList_Show(int nCmdShow);
//�Ƿ���ʾͨ��״̬��ͼ��
BOOL    MBList_ShowListBmp(BOOL bShow);

#endif
