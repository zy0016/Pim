/**************************************************************************
                                                                           
   SEF CONFIDENTIAL AND PROPRIETARY                                        
                                                                           
   This source is the sole property of SEF Inc. Reproduction or            
   utilization of this source in whole or in part is forbidden without     
   the written consent of SEF Inc.                                         
                                                                           
   (c) Copyright SEF Inc.,         2002.  All Rights Reserved.             
                                                                           
************************************************************************** 
                                                                           
   str_public_eng.h: English language string                               
                                                                           
************************************************************************** 
    Date          Author              Reference                    
    ========   =============    ==========================                 
    02-06-06      Liyingjun             CR - BJC00031   
    02-10-08	  Tanglei		PR - BJP00173                   
/*---------- Dependencies-------- --------------------------------------*/ 
static const unsigned char *strChn[]={
	/*STR_PUBLIC_NULL */
	(unsigned char *) "",	/* 0 */
	/*STR_PUBLIC_EMPTY_NUM */
	(unsigned char *) "�պ���!",
	/*STR_PUBLIC_OK */
	(unsigned char *) "ȷ��",
	/*STR_PUBLIC_EXIT */
	(unsigned char *) "�˳�",
	/*STR_PUBLIC_OPERATION_FAILED*/
	(unsigned char *) "����ʧ��!",
	/*STR_PUBLIC_OPEN*/
	(unsigned char *) "��",
	/*STR_PUBLIC_CLOSE */
	(unsigned char *) "�ر�",
	/*STR_PUBLIC_PROMPT */
	(unsigned char *) "��ʾ",
	/*STR_PUBLIC_WAITING*/
	(unsigned char *) "���Ե�...",
	/*STR_PUBLIC_PHONE_BOOK*/
	(unsigned char *) "�绰��",
	/*STR_PUBLIC_FILE_MANAGER*/
	(unsigned char *) "�ļ�������",	/* 10 */
	/*STR_PUBLIC_RETURN*/
	(unsigned char *) "����",
	/*STR_PUBLIC_FORMAT_FLASH*/
	(unsigned char *) "��ʽ��flash2",//tbd
	/*STR_PUBLIC_ENTER*/
	(unsigned char *) "����",
	/*STR_AS_RING_VOLUME_CONTROL*/
	(unsigned char *) "������������",
	/*STR_AS_SPEAKER_VOLUME_CONTROL*/
	(unsigned char *) "����ͨ������",
	/*STR_AS_RING_STATUS*/
	(unsigned char *) "����ģʽ",
	/*STR_AS_RS_RING*/
	(unsigned char *) "����",
	/*STR_AS_RS_VIBRATE*/
	(unsigned char *) "��",
	/*STR_AS_RS_VIBRING*/
	(unsigned char *) "������",
	/*STR_AS_RS_SILENT*/
	(unsigned char *) "����",
	/*STR_AS_SET_SUCCESS*/
	(unsigned char *) "���óɹ�",
	/*STR_AS_RING_LIST*/
	(unsigned char *) "�����б�",
	/*STR_AS_GROUP_TYPE*/
	(unsigned char *) "�绰������",
	/*STR_AS_KEY_TONE*/
	(unsigned char *) "����������",
	/*STR_AS_OPEN*/
	(unsigned char *) "��",
	/*STR_AS_CLOSE*/
	(unsigned char *) "�ر�",
	/*STR_AS_RING_SELE*/
	(unsigned char *) "����ѡ��",
	/*STR_AS_CALL*/
	(unsigned char *) "�绰",
	/*STR_AS_SMS*/
	(unsigned char *) "����Ϣ",
	/*STR_AS_MINU_NOTI*/
	(unsigned char *) "ʱ����ʾ",
	/*STR_AS_ONE_MINU*/
	(unsigned char *) "һ����",
	/*STR_AS_FIVE_MINU*/
	(unsigned char *) "�����",
	/*STR_AS_TEN_MINU*/
	(unsigned char *) "ʮ����",
	/*STR_AS_FIFT_MINU*/
	(unsigned char *) "ʮ�����",
	/*STR_AS_CONN_NOTI*/
	(unsigned char *) "��ͨ��ʾ",
	/*STR_AS_OPEN_RING*/
	(unsigned char *) "��������",
	/*STR_AS_KEYTONE_SELE*/
	(unsigned char *) "������",
	/*STR_AS_AUDITION*/
	(unsigned char *) "����",
	/*STR_AS_STOP*/
	(unsigned char *) "ֹͣ",
	/*STR_AS_SAVE*/
	(unsigned char *) "����",
	/*STR_AS_KEYTONE_LIST*/
	(unsigned char *) "�������б�",
	/*STR_PM_POWER_ON*/
	(unsigned char *) "����",
	/*STR_PM_VOLTAGE_ALARM*/
	(unsigned char *) "��ص�������",
	/*STR_PM_CHARGING*/
	(unsigned char *) "���ڳ��",
	/*STR_PM_CHARGED*/
	(unsigned char *) "������",
	/*STR_CB_EMPTY */
	(unsigned char *) "�����벻��Ϊ��!",
	/*STR_CB_CONFIRMCANCEL_CONTENT_UP */
	(unsigned char *) "�Ƿ�ȡ��",
	/*STR_CB_CONFIRMCANCEL_CB_CONTENT_DOWN */
	(unsigned char *) "ȫ����������!",
	/*STR_CB_CANCELNO */
	(unsigned char *) "ȡ��ʧ��!",
	/*STR_CB_CANCELOK */
	(unsigned char *) "���������ѱ�ȡ��!",
	/*STR_CB_BARRING_SUCC */
	(unsigned char *) "�������Ƴɹ�!",
	/*STR_CB_BARRING_FAIL */
	(unsigned char *) "��������ʧ��!",
	/*STR_CB_MBRSTRCT */
	(unsigned char *) "��������",
	/*STR_CB_BAOC */
	(unsigned char *) "��������",
	/*STR_CB_BOIC */
	(unsigned char *) "���ƹ�������",
	/*STR_CB_BOICEXHC */
	(unsigned char *) "���ƹ�����...",
	/*STR_CB_BOICEXHC_ALL */
	(unsigned char *) "���ƹ������γ�����",
	/*STR_CB_BAIC */
	(unsigned char *) "��������",
	/*STR_CB_BICROAM */
	(unsigned char *) "��������ʱ...",
	/*STR_CB_BICROAM_ALL */
	(unsigned char *) "��������ʱ��������",
	/*STR_CB_CANCEL */
	(unsigned char *) "ȡ�����к�...",
	/*STR_CB_CANCEL_ALL */
	(unsigned char *) "ȡ�����к�������",
	/*STR_CB_BOICEXHC_1 */
	(unsigned char *) "���ƹ�������",
	/*STR_CB_BOICEXHC_2 */
	(unsigned char *) "�ƹ������γ�",
	/*STR_CB_BOICEXHC_3 */
	(unsigned char *) "�������γ���",
	/*STR_CB_BOICEXHC_4 */
	(unsigned char *) "�����γ�����",
	/*STR_CB_BICROAM_1 */
	(unsigned char *) "��������ʱ��",
	/*STR_CB_BICROAM_2 */
	(unsigned char *) "������ʱ����",
	/*STR_CB_BICROAM_3 */
	(unsigned char *) "����ʱ������",
	/*STR_CB_BICROAM_4 */
	(unsigned char *) "��ʱ��������",
	/*STR_CB_CANCEL_1 */
	(unsigned char *) "ȡ�����к���",
	/*STR_CB_CANCEL_2 */
	(unsigned char *) "�����к�����",
	/*STR_CB_CANCEL_3 */
	(unsigned char *) "���к�������",
	/*STR_CF_MBTRANSF */
	(unsigned char *) "����ת��",
	/*STR_CF_TRANSALL */
	(unsigned char *) "������ת��",
	/*STR_CF_SHUTOROUT */
	(unsigned char *) "�ػ�/��������",
	/*STR_CF_TRANSBUSY */
	(unsigned char *) "��æת��",
	/*STR_CF_TRANSANSWER */
	(unsigned char *) "��Ӧ��ת��",
	/*STR_CF_UNTRANSALL */
	(unsigned char *) "ȡ������ת��",
	/*STR_CF_TRANS_SUCC */
	(unsigned char *) "����ת�Ƴɹ�!",
	/*STR_CF_TRANS_FAIL */
	(unsigned char *) "����ת��ʧ��!",
	/*STR_CF_EMPTY */
	(unsigned char *) "���벻��Ϊ��!",
	/*STR_CF_CONFIRMCANCEL_CONTENT_UP */
	(unsigned char *) "�Ƿ�ȡ��",
	/*STR_CF_CONFIRMCANCEL_CF_CONTENT_DOWN */
	(unsigned char *) "ȫ������ת��?",
	/*STR_CF_CANCEL    */
	(unsigned char *) "ȡ��",
	/*STR_CF_CANCEL_ALL_DIVERT*/
	(unsigned char *) "��ȡ��!",
	/*STR_CF_SELECT_STATUS*/
	(unsigned char *) "ѡ��״̬",
	/*STR_CF_DIVERT_NUMBER*/
	(unsigned char *) "������ת�ƺ���",
	/*STR_HIM_CONTR_CONTRAST*/
	(unsigned char *) "�����Աȶ�",
	/*STR_HIM_CONTR_BRIGHT*/
	(unsigned char *) "��������",
	/*STR_HIM_DISPLAY*/
	(unsigned char *) "��ʾ����",
	/*STR_HIM_LANGUAGE*/
	(unsigned char *) "��������",
	/*STR_HIM_FACTORY*/
	(unsigned char *) "��������",
	/*STR_HIM_POWER_SAVING*/
	(unsigned char *) "�ڵ�ģʽ",
	/*STR_HIM_PHONE_SET*/
	(unsigned char *) "�ֻ�����",
	/*STR_HIM_CONTRAST*/
	(unsigned char *) "�Աȶ�",
	/*STR_HIM_BRIGHT*/
	(unsigned char *) "����",
	/*STR_HIM_LCD_BLIGHT*/
	(unsigned char *) "��Ļ",
	/*STR_HIM_KEYPAD_BLIGHT*/
	(unsigned char *) "����",
	/*STR_HIM_20S*/
	(unsigned char *) "20��",	/* 100 */
	/*STR_HIM_30S*/
	(unsigned char *) "30��",
	/*STR_HIM_60S*/
	(unsigned char *) "60��",
	/*STR_HIM_90S*/
	(unsigned char *) "90��",
	/*STR_HIM_10S*/
	(unsigned char *) "10��",
	/*STR_HIM_15S*/
	(unsigned char *) "15��",
	/*STR_HIM_5S*/
	(unsigned char *) "5��",
	/*STR_INIT_OK */
	(unsigned char *) "ȷ��",
	/*STR_INIT_CANCEL */
	(unsigned char *) "ȡ��",
	/*STR_INIT_YES */
	(unsigned char *) "��",
	/*STR_INIT_NO */
	(unsigned char *) "��",
	/*STR_INIT_NOW */
	(unsigned char *) "�廪���-Unis920",
	/*STR_INIT_HELLO */
	(unsigned char *) "��ӭ��!",
	/*STR_INIT_ENTER_PHONECODE */
	(unsigned char *) "�����뿪������",
	/*STR_INIT_CODE_WRONG */
	(unsigned char *) "�����������",
	/*STR_INIT_PHCD_RETRY */
	(unsigned char *) "������",
	/*STR_INIT_ENTER_PIN */
	(unsigned char *) "������PIN",
	/*STR_INIT_ENTER_PUK */
	(unsigned char *) "������PUK",
	/*STR_INIT_ENTER_NEWPIN */
	(unsigned char *) "���µ�PIN",
	/*STR_INIT_WAIT_CLEAR */
	(unsigned char *) "���Ժ�",
	/*STR_INIT_MASTER_CLEAR */
	(unsigned char *) "���ڽ��������",
	/*STR_INIT_MASTER_RESET */
	(unsigned char *) "���ڽ�������λ",
	/*STR_INIT_IF_SHUTDOWN */
	(unsigned char *) "SIM��δ����\n�Ƿ�ػ�?",
	/*STR_INIT_NO_PH_FUN */
	(unsigned char *) "�����޷�ʹ��\n�绰����",
	/*STR_INIT_PIN_RTM */
	(unsigned char *) "PIN�������\n����%d�λ���\n������",
	/*STR_INIT_PUK_RTM */
	(unsigned char *) "PUK�������\n����%d�λ���\n������",
	/*STR_INIT_PIN_BLOCK */
	(unsigned char *) "PIN Blocked\n������PUK\n����%d�λ���\n",
	/*STR_PB_ALLCONTACT */
	(unsigned char *) "ȫ����ϵ��",
	/*STR_PB_FIND */
	(unsigned char *) "����",
	/*STR_PB_ADD */
	(unsigned char *) "����",
	/*STR_PB_MULTIDELETE */
	(unsigned char *) "����ɾ��",
	/*STR_PB_MULTICOPY */
	(unsigned char *) "�����",
	/*STR_PB_SETTING */
	(unsigned char *) "����",
	/*STR_PB_CONTACTINFO */
	(unsigned char *) "��ϵ����Ϣ",
	/*STR_PB_EDIT */
	(unsigned char *) "�༭",
	/*STR_PB_DELETE */
	(unsigned char *) "ɾ��",
	/*STR_PB_COPY */
	(unsigned char *) "����",
	/*STR_PB_CHANGEGROUP */
	(unsigned char *) "�����",
	/*STR_PB_NAME */
	(unsigned char *) "����",
	/*STR_PB_MOBILE */
	(unsigned char *) "�ƶ��绰",
	/*STR_PB_HOME */
	(unsigned char *) "סլ�绰",
	/*STR_PB_OFFICE */
	(unsigned char *) "�칫�绰",
	/*STR_PB_MEMO */
	(unsigned char *) "��ע",
	/*STR_PB_NUMBER */
	(unsigned char *) "����",
	/*STR_PB_ADDTOPHONE */
	(unsigned char *) "��ӵ�����",
	/*STR_PB_ADDTOSIM */
	(unsigned char *) "��ӵ�SIM��",
	/*STR_PB_MULTISELECT */
	(unsigned char *) "��ѡ",
	/*STR_PB_SELECTCONTACTS */
	(unsigned char *) "ѡ����ϵ��",
	/*STR_PB_SELECTEDCONTACTS */
	(unsigned char *) "ѡ�е���ϵ��",
	/*STR_PB_FINDBY */
	(unsigned char *) "��������",
	/*STR_PB_TEXT */
	(unsigned char *) "��ע",
	/*STR_PB_FINDNAME */
	(unsigned char *) "����������",
	/*STR_PB_FINDNUMBER */
	(unsigned char *) "���������",
	/*STR_PB_FINDTEXT */
	(unsigned char *) "����ע����",
	/*STR_PB_SEARCHRESULT */
	(unsigned char *) "��ѯ���",
	/*STR_PB_SEARCHNONE */
	(unsigned char *) "û���ҵ�",
	/*STR_PB_COPYTOPHONE */
	(unsigned char *) "���Ƶ�����",
	/*STR_PB_COPYTOSIM */
	(unsigned char *) "���Ƶ�SIM��",
	/*STR_PB_SELECTTYPE */
	(unsigned char *) "ѡ������",
	/*STR_PB_SELECTCOPYFIELD */
	(unsigned char *) "ѡ���ƺ���",
	/*STR_PB_MULTICOPYTYPE */
	(unsigned char *) "���������",
	/*STR_PB_SIMTOPHONE */
	(unsigned char *) "��SIM��������",
	/*STR_PB_PHONETOSIM */
	(unsigned char *) "�ӻ�����SIM��",
	/*STR_PB_REMAINSPACE */
	(unsigned char *) "ʣ��ռ� %d",
	/*STR_PB_PERSONAL */
	(unsigned char *) "˽��",
	/*STR_PB_BUSINESS */
	(unsigned char *) "����",
	/*STR_PB_OTHER */
	(unsigned char *) "����",
	/*STR_PB_DELETING */
	(unsigned char *) "����ɾ��...",
	/*STR_PB_COPYING */
	(unsigned char *) "���ڸ���...",
	/*STR_PB_LOADING */
	(unsigned char *) "����װ��...",
	/*STR_PB_LISTNAME */
	(unsigned char *) "�г�����",
	/*STR_PB_PHONE_FULL */
	(unsigned char *) "�����ռ�����",
	/*STR_PB_SIM_FULL */
	(unsigned char *) "SIM���ռ�����",
	/*STR_PB_SELECT_NONE */
	(unsigned char *) "û��ѡ���κμ�¼",
	/*STR_PB_SELECT_LIMIT */
	(unsigned char *) "�Ѵﵽ����ѡֵ",
	/*STR_PB_NAME_EMPTY */
	(unsigned char *) "��������Ϊ��",
	/*STR_PB_NUMBER_EMPTY */
	(unsigned char *) "�绰���벻��Ϊ��",
	/*STR_PB_CONFIRM_DELETE */
	(unsigned char *) "���Ҫɾ����?",
	/*STR_PB_LISTNAMENUMBER */
	(unsigned char *) "�г������ͺ���",
	/*STR_PB_SETTING_MAIN_TITLE */
	(unsigned char *) "�绰������",
	/*STR_PB_SETTING_BTN_OK */
	(unsigned char *) "ȷ��",
	/*STR_PB_SETTING_BTN_EXIT */
	(unsigned char *) "�˳�",
	/*STR_PB_SETTING_SIM */
	(unsigned char *) "SIM",
	/*STR_PB_SETTING_PHONE */
	(unsigned char *) "�绰",
	/*STR_PB_SETTING_GROUPSET */
	(unsigned char *) "������",
	/*STR_PB_SETTING_MEMORYSTATUS */
	(unsigned char *) "�ڴ�״̬",
	/*STR_PB_SETTING_VIEWTYPE */
	(unsigned char *) "��ʾ����",
	/*STR_PB_SETTING_STORAGETYPE */
	(unsigned char *) "�洢����",
	/*STR_PB_SETTING_ERROR_READ */
	(unsigned char *) "��ȡ����ʧ��",
	/*STR_PB_SETTING_ERROR_WRITE */
	(unsigned char *) "д������ʧ��",
	/*STR_PB_SETTING_SUCCESS_READ */
	(unsigned char *) "��ȡ�������",
	/*STR_PB_SETTING_SUCCESS_WRITE */
	(unsigned char *) "д���������",
	/*STR_PB_SETTING_PROMPT */
	(unsigned char *) "��ʾ",
	/*STR_PB_SETTING_MAIN_BTN_OK */
	(unsigned char *) "ȷ��",
	/*STR_PB_SETTING_MAIN_BTN_EXIT */
	(unsigned char *) "�˳�",
	/*STR_PB_SETTING_MAIN_GROUPSET */
	(unsigned char *) "������",
	/*STR_PB_SETTING_MAIN_MEMORY */
	(unsigned char *) "�ڴ�״̬",
	/*STR_PB_SETTING_MAIN_VIEWTYPE */
	(unsigned char *) "�鿴����",
	/*STR_PB_SETTING_MAIN_STORAGE */
	(unsigned char *) "�洢״̬",
	/*STR_PB_SETTING_SG_TITLE */
	(unsigned char *) "ѡ�����",
	/*STR_PB_SETTING_SG_BTN_OK */
	(unsigned char *) "ȷ��",	/* 200 */
	/*STR_PB_SETTING_SG_BTN_EXIT */
	(unsigned char *) "�˳�",
	/*STR_PB_SETTING_SG_SIM */
	(unsigned char *) "SIM",
	/*STR_PB_SETTING_SG_PERSON */
	(unsigned char *) "����",
	/*STR_PB_SETTING_SG_BUSINESS */
	(unsigned char *) "����",
	/*STR_PB_SETTING_SG_OTHER */
	(unsigned char *) "����",
	/*STR_PB_SETTING_SG_MENU_ITEM_1 */
	(unsigned char *) "ѡ��ͼ��",
	/*STR_PB_SETTING_SG_MENU_ITEM_2 */
	(unsigned char *) "ѡ������",
	/*STR_PB_SETTING_SP_TITLE */
	(unsigned char *) "ѡ��ͼ��",
	/*STR_PB_SETTING_SP_BTN_OK */
	(unsigned char *) "�洢",
	/*STR_PB_SETTING_SP_BTN_EXIT */
	(unsigned char *) "�˳�",
	/*STR_PB_SETTING_MEMORY_TITLE */
	(unsigned char *) "�ڴ�״̬",
	/*STR_PB_SETTING_MEMORY_BTN_EXIT */
	(unsigned char *) "�˳�",
	/*STR_PB_SETTING_MEMORY_SIM */
	(unsigned char *) "SIM",
	/*STR_PB_SETTING_MEMORY_PHONE */
	(unsigned char *) "�绰",
	/*STR_PB_SETTING_MEMORY_SIMTITLE */
	(unsigned char *) "SIM",
	/*STR_PB_SETTING_MEMORY_PHNTITLE */
	(unsigned char *) "�绰",
	/*STR_PB_SETTING_MEMORY_TOTAL */
	(unsigned char *) "����:%d",
	/*STR_PB_SETTING_MEMORY_FREE */
	(unsigned char *) "ʣ��:%d",
	/*STR_PB_SETTING_VIEW_TITLE */
	(unsigned char *) "��ʾ����",
	/*STR_PB_SETTING_VIEW_BTN_OK */
	(unsigned char *) "ȷ��",
	/*STR_PB_SETTING_VIEW_BTN_EXIT */
	(unsigned char *) "�˳�",
	/*STR_PB_SETTING_VIEW_NAME */
	(unsigned char *) "����",
	/*STR_PB_SETTING_VIEW_NAMNUM */
	(unsigned char *) "�����ͺ���",
	/*STR_PB_SETTING_STORAGE_TITLE */
	(unsigned char *) "�洢����",
	/*STR_PB_SETTING_STORAGE_BTN_OK */
	(unsigned char *) "ȷ��",
	/*STR_PB_SETTING_STORAGE_BTN_EXT */
	(unsigned char *) "�˳�",
	/*STR_PB_SETTING_STORAGE_SIM */
	(unsigned char *) "SIM",
	/*STR_PB_SETTING_STORAGE_PHONE */
	(unsigned char *) "�绰",
	/*STR_PB_COPYNUMBERAS*/
	(unsigned char *) "���ƺ���Ϊ",
	/*STR_PB_DELETE_SUCCESSFUL*/
	(unsigned char *) "ɾ���ɹ�!",
	/*STR_PB_DELETE_FAILED*/
	(unsigned char *) "ɾ��ʧ��!",
	/*STR_PB_COPY_SUCCESSFUL*/
	(unsigned char *) "���Ƴɹ�!",
	/*STR_PB_COPY_FAILED*/
	(unsigned char *) "����ʧ��!",
	/*STR_PB_ADD_SUCCESSFUL*/
	(unsigned char *) "��ӳɹ�!",
	/*STR_PB_ADD_FAILED*/
	(unsigned char *) "���ʧ��!",
	/*STR_PB_UPDATE_SUCCESSFUL*/
	(unsigned char *) "�޸ĳɹ�!",
	/*STR_PB_UPDATE_FAILED*/
	(unsigned char *) "�޸�ʧ��!",
	/*STR_PB_GRPCHG_SUCCESSFUL*/
	(unsigned char *) "����ĳɹ�!",
	/*STR_PB_GRPCHG_FAILED*/
	(unsigned char *) "�����ʧ��!",
	/*STR_PB_SAVE_FAILED*/
	(unsigned char *) "�洢ʧ��!",
	/*STR_PB_NO_CONTACTS*/
	(unsigned char *) "û����ϵ��!",
	/*STR_PB_LOADCONTACT_FAILED*/
	(unsigned char *) "װ��ʧ��!",
	/*STR_PB_MSGWIN_ALARM*/
	(unsigned char *) "����",
	/*STR_PB_MSGWIN_PROMPT*/
	(unsigned char *) "��ʾ",
	/*STR_PB_OK*/
	(unsigned char *) "ȷ��",
	/*STR_PB_EXIT*/
	(unsigned char *) "�˳�",
	/*STR_PB_CANCEL*/
	(unsigned char *) "ȡ��",
	/*STR_PB_COPY_REPORT_TOTAL*/
	(unsigned char *) "%d����ϵ��",
	/*STR_PB_COPY_REPORT_OK*/
	(unsigned char *) "%d�����Ƴɹ�",
	/*STR_PB_DELETE_REPORT_TOTAL*/
	(unsigned char *) "%d����ϵ��",
	/*STR_PB_DELETE_REPORT_OK*/
	(unsigned char *) "%d��ɾ���ɹ�",
	/*STR_PB_SIM_LOCATION*/
	(unsigned char *) "SIM λ��:%d",
	/*STR_PB_NOSIM*/
	(unsigned char *) "û��SIM��",
	/*STR_PB_CHINESE_NAME_LIMITED*/
	(unsigned char *) "�������ض�",
	/*STR_PB_DB_SIM_INIT_ERR*/
	(unsigned char *) "��SIM�绰��ʧ��",
	/*STR_SWITCH_ON */
	(unsigned char *) "��",
	/*STR_SWITCH_OFF */
	(unsigned char *) "��",
	/*STR_NS_NETWORK_SELECT */
	(unsigned char *) "����ѡ��",
	/*STR_NS_CALL_WAITING */
	(unsigned char *) "���еȴ�",
	/*STR_NS_CLIR */
	(unsigned char *) "���к��뱣��",
	/*STR_NS_AUTO_SELELCT */
	(unsigned char *) "�Զ�ѡ��",
	/*STR_NS_MANUAL_SELELCT */
	(unsigned char *) "�˹�ѡ��",
	/*STR_NS_PREFER_NETWORK */
	(unsigned char *) "��������",
	/*STR_NS_BUTTON_REGISTER */
	(unsigned char *) "ע��",
	/*STR_NS_MENU_ADD */
	(unsigned char *) "����",
	/*STR_NS_MENU_DELETE */
	(unsigned char *) "ɾ��",
	/*STR_NS_INPUT_PRIORITY */
	(unsigned char *) "���������ȼ���(<=10)",
	/*STR_NS_CHOOSE */
	(unsigned char *) "��ѡ������",
	/*STR_NS_PRIORITY_WRONG */
	(unsigned char *) "���ִ���!",
	/*STR_NS_SEARCHING_NETWORK */
	(unsigned char *) "������Ѱ...",
	/*STR_NS_NO_AVAILABLE_NETWORK */
	(unsigned char *) "û�п�������",
	/*STR_NS_CMCC */
	(unsigned char *) "�й��ƶ�",
	/*STR_NS_UNICOM */
	(unsigned char *) "�й���ͨ",
	/*STR_NS_REGISERING_NETWORK */
	(unsigned char *) "����ע��...",
	/*STR_NS_PLEASE_WAIT */
	(unsigned char *) "���Ե�...",
	/*STR_NS_CLIR_DEFAULT */
	(unsigned char *) "Ĭ������",
	/*STR_NS_CLIR_SHOW */
	(unsigned char *) "��ʾ����",
	/*STR_NS_CLIR_HIDE */
	(unsigned char *) "���غ���",
	/*STR_NS_NO_NETWORK */
	(unsigned char *) "û������",
	/*STR_NS_NETWORK_UNKNOWN */
	(unsigned char *) "δ֪����",
	/*STR_NS_FAILURE */
	(unsigned char *) "ʧ��",
	/*STR_NS_WARNING */
	(unsigned char *) "ע��",
	/*STR_NS_GET_NO_NETWORKNAME*/
	(unsigned char *) "�޷��õ���������",
	/*STR_NS_SIM_FAILURE*/
	(unsigned char *) "����SIM��!",
	/*STR_NS_OPERATION_FORBIDDEN*/
	(unsigned char *) "����������",
	/*STR_NS_OPERATION_NOT_SUPPORT*/
	(unsigned char *) "��֧�ִ������",
	/*STR_NS_NO_SIM*/
	(unsigned char *) "û��SIM��",
	/*STR_NS_TRY*/
	(unsigned char *) "���Ժ���������ѡ��",
	/*STR_NS_UPDATE_FAILURE*/
	(unsigned char *) "λ�ø���ʧ��",
	/*STR_NS_SELECT_FAILURE*/
	(unsigned char *) "ѡ��ʧ��",
	/*STR_CS_CALL_SETTING */
	(unsigned char *) "Ӧ������",
	/*STR_CS_AUTO_ANSWER */
	(unsigned char *) "�Զ�:",
	/*STR_CS_FLIP_ANSWER */
	(unsigned char *) "����:",
	/*STR_CS_ANY_KEY_ANSWER */
	(unsigned char *) "�����:",
	/*STR_CS_AUTOANSWER_FAILUER*/
	(unsigned char *) "�Զ�Ӧ������ʧ��",
	/*STR_CS_NO_FIND_EARPHONE*/
	(unsigned char *) "û�з��ֶ���",
	/*STR_CS_NO_FUNCTION*/
	(unsigned char *) "û�д����",
	/*STR_CH_CALL_HISTORY */
	(unsigned char *) "ͨ����¼",
	/*STR_CH_MISSED_RECORD */
	(unsigned char *) "δ����",
	/*STR_CH_ANSWERED_RECORD */
	(unsigned char *) "�ѽ���",	/* 300 */
	/*STR_CH_DIALED_RECORD */
	(unsigned char *) "��������",
	/*STR_CH_CALL_TIME */
	(unsigned char *) "ͨ��ʱ��",
	/*STR_CH_MENU_DETAIL */
	(unsigned char *) "�鿴��ϸ��Ϣ",
	/*STR_CH_MENU_DELETE */
	(unsigned char *) "ɾ��������¼",
	/*STR_CH_MENU_DELETE_ALL */
	(unsigned char *) "ɾ�����м�¼",
	/*STR_CH_MENU_SAVE */
	(unsigned char *) "�洢���绰��",
	/*STR_CH_20_MISSED_RECORD */
	(unsigned char *) "���20��δ������",
	/*STR_CH_20_ANSWERED_RECORD */
	(unsigned char *) "���20���ѽ�����",
	/*STR_CH_20_DIALED_RECORD */
	(unsigned char *) "���20����������",
	/*STR_CH_LAST_CALL_TIME */
	(unsigned char *) "�ϴ�ͨ��:",
	/*STR_CH_TOTAL_INCOMING_TIME */
	(unsigned char *) "�ۼƽ���:",
	/*STR_CH_TOTAL_OUTGOING_TIME */
	(unsigned char *) "�ۼƺ���:",
	/*STR_CH_RESET_TIMER */
	(unsigned char *) "��ʱ����:",
	/*STR_CH_RESET_TIMER_CONFIRM */
	(unsigned char *) "ȷʵҪ������?",
	/*STR_CH_DETAIL_MENU_CALL */
	(unsigned char *) "���б�����",
	/*STR_CH_DETAIL_MENU_SMS */
	(unsigned char *) "���Ͷ���Ϣ",
	/*STR_CH_NAME */
	(unsigned char *) "����",
	/*STR_CH_NUMBER */
	(unsigned char *) "����",
	/*STR_CH_DATE */
	(unsigned char *) "����",
	/*STR_CH_TIME */
	(unsigned char *) "ʱ��",
	/*STR_CH_DURATION */
	(unsigned char *) "ʱ��",
	/*STR_CH_NO_RECORD*/
	(unsigned char *) "û��ͨ����¼",
	/*STR_CH_NO_NAME*/
	(unsigned char *) "<δ֪����>",
	/*STR_CH_CLEAR_ZERO*/
	(unsigned char *) "���Ҫ������?",
	/*STR_CH_MISSED_NUMBER*/
	(unsigned char *) "��δ�ӵ绰",
	/*STR_LAN_SWITCH_LANGUAGE */
	(unsigned char *) "��������",
	/*STR_LAN_LANGUAGE */
	(unsigned char *) "��������",
	/*STR_TON_PHONECALL */
	(unsigned char *) "��������",
	/*STR_TON_SIMCALL */
	(unsigned char *) "SIM������",
	/*STR_TON_MENU_STORE */
	(unsigned char *) "����",
	/*STR_TON_MENU_SHOWID */
	(unsigned char *) "��ʾID",
	/*STR_TON_MENU_HIDEID */
	(unsigned char *) "����ID",
	/*STR_TON_CLEAR */
	(unsigned char *) "���",
	/*STR_TON_CHECKNET */
	(unsigned char *) "���ڼ������...",
	/*STR_TON_NETNOTSUPPORT */
	(unsigned char *) "���粻֧��",
	/*STR_TON_SIMNOFOUND*/
	(unsigned char *) "λ��%d��",
	/*STR_TON_SIMNUM*/
	(unsigned char *) "�洢��ΧΪ1-%d",
	/*STR_TON_NOSIM*/
	(unsigned char *) "û��SIM��!",
	/*STR_CC_ANSWER */
	(unsigned char *) "Ӧ��",
	/*STR_CC_END_HOLD_AND_ACCEPT */
	(unsigned char *) "�Ҷϱ��ֲ�����",
	/*STR_CC_HOLD_ACTIVE_AND_ACCEPT */
	(unsigned char *) "����ͨ��������",
	/*STR_CC_END_ACTIVE_AND_ACCEPT */
	(unsigned char *) "�Ҷ�ͨ��������",
	/*STR_CC_MUTE */
	(unsigned char *) "����",
	/*STR_CC_HOLDMUTE */
	(unsigned char *) "���ֺ;���",
	/*STR_CC_UNMUTE */
	(unsigned char *) "����",
	/*STR_CC_HOLD */
	(unsigned char *) "����",
	/*STR_CC_NEW_CALL */
	(unsigned char *) "�½�����",
	/*STR_CC_END */
	(unsigned char *) "�Ҷ�",
	/*STR_CC_REDIAL */
	(unsigned char *) "�ز�",
	/*STR_CC_RESUME */
	(unsigned char *) "�ָ�",
	/*STR_CC_SWITCH */
	(unsigned char *) "�л�",
	/*STR_CC_END_CALL_ON_HOLD */
	(unsigned char *) "�Ҷϱ���",
	/*STR_CC_END_CALL_ON_ACTIVE */
	(unsigned char *) "�Ҷ�ͨ��",
	/*STR_CC_CONNECTING_NOW */
	(unsigned char *) "ͨ����",
	/*STR_CC_HOLD_NOW */
	(unsigned char *) "���б���",
	/*STR_CC_INCOMMING */
	(unsigned char *) "�绰����...",
	/*STR_CC_CONNECTING_END */
	(unsigned char *) "������ֹ",
	/*STR_CC_CONNECTING_FAILED */
	(unsigned char *) "ͨ��ʧ��",
	/*STR_CC_END_UNKNOW */
	(unsigned char *) "δ֪״̬",
	/*STR_CC_DAILING */
	(unsigned char *) "���ں���...",
	/*STR_CC_DAILING_END */
	(unsigned char *) "����",
	/*STR_CC_EMERGENCY */
	(unsigned char *) "��������",
	/*STR_CC_DROP */
	(unsigned char *) "�ܾ�",
	/*STR_CC_UNKNOW_NUM */
	(unsigned char *) "δ֪����",
	/*STR_CC_NUM_TOO_LONG */
	(unsigned char *) "����̫��!",
	/*STR_SMS_CONFIRM */
	(unsigned char *) "ȷ��",
	/*STR_SMS_EXIT */
	(unsigned char *) "�˳�",
	/*STR_SMS_CANCEL */
	(unsigned char *) "ȡ��",
	/*STR_SMS_ERROR */
	(unsigned char *) "����",
	/*STR_SMS */
	(unsigned char *) "����Ϣ",
	/*STR_SMS_INITIALIZING */
	(unsigned char *) "���ڳ�ʼ��...",
	/*STR_SMS_FLASH_IS_FULL */
	(unsigned char *) "�ֻ��洢�ռ�����!",
	/*STR_SMS_SIM_IS_FULL */
	(unsigned char *) "SIM���ռ�����!",
	/*STR_SMS_NEED_DELETE_SOME_MSG */
	(unsigned char *) "ɾ��һЩ����!",
	/*STR_SMS_SAVE */
	(unsigned char *) "����",
	/*STR_SMS_WRITE */
	(unsigned char *) "д��Ϣ",
	/*STR_SMS_READ_MSG */
	(unsigned char *) "����Ϣ",
	/*STR_SMS_CONFIG */
	(unsigned char *) "����",
	/*STR_SMS_VOICEMAIL */
	(unsigned char *) "��������",
	/*STR_SMS_QUICKTEXT */
	(unsigned char *) "���ö���",
	/*STR_SMS_SAVED */
	(unsigned char *) "�Ѵ���Ϣ",
	/*STR_SMS_VOICEMAIL_LISTEN */
	(unsigned char *) "��������",
	/*STR_SMS_VOICEMAIL_SETUP */
	(unsigned char *) "���ĺ���",
	/*STR_SMS_VOICEMAIL_NUMBER */
	(unsigned char *) "�����������:",
	/*STR_SMS_VOICEMAIL_NONUMBER */
	(unsigned char *) "δ����",
	/*STR_SMS_GET_VOICEMAIL_ERROR */
	(unsigned char *) "��ѯ����ʧ��!",
	/*STR_SMS_SET_VOICEMAIL_ERROR */
	(unsigned char *) "���ú���ʧ��!",
	/*STR_SMS_SET_VOICEMAIL_OK */
	(unsigned char *) "������������Ѵ�!",
	/*STR_SMS_RECEIVER */
	(unsigned char *) "�ռ���",
	/*STR_SMS_SEND */
	(unsigned char *) "����",
	/*STR_SMS_IS_SENDING_NOW */
	(unsigned char *) "����Ϣ���ڷ�����...",
	/*STR_SMS_MUST_INPUT_DA_NUMBER */
	(unsigned char *) "�������ռ��˺���!",
	/*STR_SMS_MUST_INPUT_CONTENT */
	(unsigned char *) "����Ϣ���ݲ���Ϊ��!",
	/*STR_SMS_ONE_ITEM_TOO_LONG */
	(unsigned char *) "����̫��!",
	/*STR_SMS_RECEIVER_IS_TOO_MORE */
	(unsigned char *) "ָ�����ռ���̫��!",
	/*STR_SMS_MSG_SEND_SUCCESS */
	(unsigned char *) "����Ϣ���ͳɹ�!",
	/*STR_SMS_MSG_SEND_FAIL */
	(unsigned char *) "����Ϣ����ʧ��!",
	/*STR_SMS_MSG_SENDING_TO */
	(unsigned char *) "���ڷ��Ͷ���Ϣ��:",
	/*STR_SMS_TOTAL_SEND_COUNT */
	(unsigned char *) "�ռ�������:",
	/*STR_SMS_SEND_OK_COUNT */
	(unsigned char *) "�ѷ��ͳɹ���:",	/* 400 */
	/*STR_SMS_SEND_FAIL_LIST */
	(unsigned char *) "δ���ͳɹ��ռ���:",
	/*STR_SMS_RESEND */
	(unsigned char *) "����",
	/*STR_SMS_SAVING_MSG */
	(unsigned char *) "���ڴ������Ϣ...",
	/*STR_SMS_MSG_SAVE_FAIL */
	(unsigned char *) "����Ϣ����ʧ��!",
	/*STR_SMS_MSG_SAVE_SUCCESS */
	(unsigned char *) "����Ϣ�Ѵ�!",
	/*STR_SMS_BOX_EMPTY */
	(unsigned char *) "������������Ϣ!",
	/*STR_SMS_READ */
	(unsigned char *) "�Ķ�",
	/*STR_SMS_SENDER_NAME */
	(unsigned char *) "������:\r\n",
	/*STR_SMS_SENDER_TIME */
	(unsigned char *) "������:\r\n",
	/*STR_SMS_CONTENT */
	(unsigned char *) "����:\r\n",
	/*STR_SMS_DELETE */
	(unsigned char *) "ɾ��",
	/*STR_SMS_REPLY */
	(unsigned char *) "�ظ�",
	/*STR_SMS_EDIT */
	(unsigned char *) "�༭",
	/*STR_SMS_USERNUMBER */
	(unsigned char *) "��ȡ����",
	/*STR_SMS_FORWARD */
	(unsigned char *) "ת��",
	/*STR_SMS_COPY */
	(unsigned char *) "���ƶ���",
	/*STR_SMS_COPY_WND_TITLE */
	(unsigned char *) "��������һ�洢��",
	/*STR_SMS_MOVE */
	(unsigned char *) "�ƶ�����",//tbd
	/*STR_SMS_DELETE_CONFIRM*/
	(unsigned char *) "ȷ��ɾ��?",
	/*STR_SMS_DELETING */
	(unsigned char *) "����ɾ��...",
	/*STR_SMS_DELETE_FAIL */
	(unsigned char *) "����Ϣɾ��ʧ��!",
	/*STR_SMS_DELETE_SUCCESS */
	(unsigned char *) "����Ϣ��ɾ��!",
	/*STR_SMS_KEEP_ORIGINAL */
	(unsigned char *) "����ԭ��¼",
	/*STR_SMS_DELETE_ORIGINAL */
	(unsigned char *) "ɾ��ԭ��¼",
	/*STR_SMS_COPYING */
	(unsigned char *) "���ڸ���...",
	/*STR_SMS_COPY_MSG_OK */
	(unsigned char *) "����Ϣ�Ѹ���!",
	/*STR_SMS_COPY_MSG_FAIL */
	(unsigned char *) "���ƶ���Ϣʧ��!",
	/*STR_SMS_MOVING */
	(unsigned char *) "�����ƶ�����Ϣ",
	/*STR_SMS_MOVE_MSG_OK */
	(unsigned char *) "����Ϣ�ƶ��ɹ�!",
	/*STR_SMS_MOVE_MSG_FAIL */
	(unsigned char *) "�ƶ�����Ϣʧ��!",
	/*STR_SMS_NOT_SUPPORT */
	(unsigned char *) "�ݲ�֧�ִ˲���!",
	/*STR_SMS_NEW_MESSAGE_INCOMING */
	(unsigned char *) "%d ���¶���Ϣ",
	/*STR_SMS_SETTING */
	(unsigned char *) "����",
	/*STR_SMS_STORAGE_INFO */
	(unsigned char *) "�ڴ�״̬",
	/*STR_SMS_SET_SCA */
	(unsigned char *) "��Ϣ���ĵ�ַ",
	/*STR_SMS_SET_TYPE */
	(unsigned char *) "�洢����",
	/*STR_SMS_SET_PERIOD */
	(unsigned char *) "��Ч��",
	/*STR_SMS_SET_PERIOD_1_HOUR */
	(unsigned char *) "1 Сʱ",
	/*STR_SMS_SET_PERIOD_6_HOURS */
	(unsigned char *) "6 Сʱ",
	/*STR_SMS_SET_PERIOD_24_HOURS */
	(unsigned char *) "24 Сʱ",
	/*STR_SMS_SET_PERIOD_72_HOURS */
	(unsigned char *) "72 Сʱ",
	/*STR_SMS_SET_PERIO_1_WEEK */
	(unsigned char *) "1 ����",
	/*STR_SMS_SET_PERIOD_MAX */
	(unsigned char *) "���ֵ",
	/*STR_SMS_SET_STORAGE_INFO */
	(unsigned char *) "�洢��Ϣ",
	/*STR_SMS_SET_SUCC */
	(unsigned char *) "���óɹ�!",
	/*STR_SMS_SET_FAILED */
	(unsigned char *) "����ʧ��!",
	/*STR_SMS_INFO_FLASH */
	(unsigned char *) "�ֻ�",
	/*STR_SMS_INFO_SIM */
	(unsigned char *) "SIM��",
	/*STR_SMS_INFO_USED */
	(unsigned char *) "����:",
	/*STR_SMS_INFO_TOTAL */
	(unsigned char *) "�ܹ�:",
	/*STR_SMS_INFO_OK */
	(unsigned char *) "ȷ��",
	/*STR_SMS_INFO_EXIT */
	(unsigned char *) "�˳�",
	/*STR_SMS_INFO_CLEAR */
	(unsigned char *) "���",
	/*STR_SMS_INFO_NOTICE_INVALID */
	(unsigned char *) "�������!",
	/*STR_SMS_OUTBOX */
	(unsigned char *) "�Ѵ���Ϣ",
	/*STR_SMS_OUTBOX_OK */
	(unsigned char *) "�Ķ�",
	/*STR_SMS_OUTBOX_EXIT */
	(unsigned char *) "�˳�",
	/*STR_SMS_OUTBOX_DETAIL */
	(unsigned char *) "��ϸ��Ϣ",
	/*STR_SMS_OUTBOX_DETAIL_DELETE */
	(unsigned char *) "ɾ��",
	/*STR_SMS_OUTBOX_DETAIL_EDIT */
	(unsigned char *) "�༭",
	/*STR_SMS_OUTBOX_DETAIL_FORWARD */
	(unsigned char *) "ת��",
	/*STR_SMS_OUTBOX_NO_REC */
	(unsigned char *) "û���Ѵ���Ϣ!",
	/*STR_SMS_OUTBOX_DEL_SUCC */
	(unsigned char *) "ɾ���ɹ�!",
	/*STR_SMS_OUTBOX_DEL_FAILED */
	(unsigned char *) "ɾ��ʧ��!",
	/*STR_SMS_IME_NAME_FULL_SCREEN_HAND_WRITE*/
	(unsigned char *) "ȫ����д",
	/*STR_QT_SEND */
	(unsigned char *) "����",
	/*STR_QT_NEW */
	(unsigned char *) "����",
	/*STR_QT_MODIFY */
	(unsigned char *) "�޸�",
	/*STR_QT_DELETE */
	(unsigned char *) "ɾ��",
	/*STR_QT_DETAIL */
	(unsigned char *) "�鿴",
	/*STR_QT_EXIT */
	(unsigned char *) "�˳�",
	/*STR_QT_CANCEL */
	(unsigned char *) "ȡ��",
	/*STR_QT_MENU */
	(unsigned char *) "�˵�",
	/*STR_QUICK_TEXT */
	(unsigned char *) "���ö���",
	/*STR_QT_DETAIL_TIT */
	(unsigned char *) "��ϸ��Ϣ",
	/*STR_QT_NEW_TITLE */
	(unsigned char *) "����",
	/*STR_QT_MODIFY_TITLE */
	(unsigned char *) "�޸�",
	/*STR_QT_SAVE */
	(unsigned char *) "ȷ��",
	/*STR_QT_STATIC_0 */
	(unsigned char *) "�°�������?һ��Է�?",
	/*STR_QT_STATIC_1 */
	(unsigned char *) "�����·ɻ�,��Լ ���Ӻ�������",
	/*STR_QT_STATIC_2 */
	(unsigned char *) "�ɻ������,��Լ �㵽��",
	/*STR_QT_STATIC_3 */
	(unsigned char *) "�����ؼҳԷ�,�Ӱ�",
	/*STR_QT_STATIC_4 */
	(unsigned char *) "���ѵ�",
	/*STR_QT_MODIFY_SUCC */
	(unsigned char *) "�޸ĳɹ�!",
	/*STR_QT_MODIFY_FAILED */
	(unsigned char *) "�޸�ʧ��!",
	/*STR_QT_ADD_SUCC */
	(unsigned char *) "���ӳɹ�!",
	/*STR_QT_DELETE_SUCC */
	(unsigned char *) "ɾ���ɹ�!",
	/*STR_QT_DELETE_FAILED */
	(unsigned char *) "ɾ��ʧ��!",
	/*STR_QT_ADD_NO_SPACE */
	(unsigned char *) "�绰����!",
	/*STR_QT_EMPTY */
	(unsigned char *) "<��>",
	/*STR_SS_CLEAR */
	(unsigned char *) "���",
	/*STR_SS_CANCEL */
	(unsigned char *) "ȡ��",
	/*STR_SS_SECURITY */
	(unsigned char *) "��ȫ����",
	/*STR_SS_CHGPASS */
	(unsigned char *) "�޸�����",
	/*STR_SS_OLDPASS */
	(unsigned char *) "������",
	/*STR_SS_NEWPASS */
	(unsigned char *) "������",
	/*STR_SS_REPASS */
	(unsigned char *) "ȷ��������",
	/*STR_SS_STATUS */
	(unsigned char *) "����״̬",
	/*STR_SS_LockNow */
	(unsigned char *) "��������",
	/*STR_SS_AutoLock */
	(unsigned char *) "�Զ�����",	/* 500 */
	/*STR_SS_PINSetting */
	(unsigned char *) "PIN1����",
	/*STR_SS_CallBarring */
	(unsigned char *) "��������",
	/*STR_SS_SecurityCode */
	(unsigned char *) "�޸İ�ȫ��",
	/*STR_SS_PIN1Code */
	(unsigned char *) "�޸�PIN1��",
	/*STR_SS_PIN2Code */
	(unsigned char *) "�޸�PIN2��",
	/*STR_SS_BarringCode */
	(unsigned char *) "�޸�������",
	/*STR_SS_InputPUK */
	(unsigned char *) "����PUK1��",
	/*STR_SS_InputNewPIN */
	(unsigned char *) "�µ�PIN1��",
	/*STR_SS_InputRepPIN */
	(unsigned char *) "ȷ��PIN1��",
	/*STR_SS_InputPUK2 */
	(unsigned char *) "����PUK2��",
	/*STR_SS_InputNewPIN2 */
	(unsigned char *) "�µ�PIN2��",
	/*STR_SS_InputRepPIN2 */
	(unsigned char *) "ȷ��PIN2��",
	/*STR_SS_INPUT_BARRINGCODE */
	(unsigned char *) "������������",
	/*STR_SS_INPUT_PHONECODE */
	(unsigned char *) "���밲ȫ��",
	/*STR_SS_LockNowSetting */
	(unsigned char *) "��������������!",
	/*STR_SS_AutoLock_Open */
	(unsigned char *) "�Զ������Ѵ�!",
	/*STR_SS_CH_SECURITY_YES */
	(unsigned char *) "��ȫ���ѱ��޸�!",
	/*STR_SS_CH_PIN_YES */
	(unsigned char *) "PIN���ѱ��޸�!",
	/*STR_SS_CH_PIN_NO */
	(unsigned char *) "PIN���޸�ʧ��!",
	/*STR_SS_CH_PIN2_YES */
	(unsigned char *) "PIN2���ѱ��޸�!",
	/*STR_SS_CH_PIN2_NO */
	(unsigned char *) "PIN2���޸�ʧ��!",
	/*STR_SS_CH_BAR_YES */
	(unsigned char *) "�������ѱ��޸�!",
	/*STR_SS_CH_BAR_NO */
	(unsigned char *) "�������޸�ʧ��!",
	/*STR_SS_NoLockNowSetting */
	(unsigned char *) "����ʧ��!",
	/*STR_SS_NoAutoLockSetting */
	(unsigned char *) "����ʧ��!",
	/*STR_SS_InputPIN */
	(unsigned char *) "������PIN��",
	/*STR_SS_InputBARRINGCODE */
	(unsigned char *) "������������",
	/*STR_SS_TWOPWDERROR */
	(unsigned char *) "����ȷ�ϴ���!",
	/*STR_SS_PWDERROR */
	(unsigned char *) "�������!",
	/*STR_SS_ISEMPTY */
	(unsigned char *) "���벻��Ϊ��!",
	/*STR_SS_UNLOCKFAILED */
	(unsigned char *) "����ʧ��!",
	/*STR_SS_UNBLOCKPIN_SUCC */
	(unsigned char *) "PIN������ɹ�!",
	/*STR_SS_UNBLOCKPIN_FAIL */
	(unsigned char *) "����ʧ��!",
	/*STR_SS_PINSTATUS_CHANGED*/
	(unsigned char *) "��״̬�ѱ�����!",
	/*STR_SS_TIMEOUT*/
	(unsigned char *) "����ʧ��,������!",
	/*STR_SS_BARRING_OPERATE*/
	(unsigned char *) "����������!",
	/*STR_SS_SELECT_STATUS*/
	(unsigned char *) "ѡ��״̬",
	/*STR_SS_NO_NETWORK*/
	(unsigned char *) "��ǰ������!",
	/*STR_SS_PIN_LOCKED*/
	(unsigned char *) "PIN���ѱ���!",
	/*STR_SS_PIN2_LOCKED*/
	(unsigned char *) "PIN2���ѱ���!",
	/*STR_SS_TIMES_PROMPT*/
	(unsigned char *) "����%d�λ���!",
	/*STR_SEQL_INVALIDATE*/
	(unsigned char *) "������Ч",
	/*STR_SEQL_SUC*/
	(unsigned char *) "�����ɹ�",
	/*STR_SEQL_FAIL*/
	(unsigned char *) "����ʧ��",
	/*STR_SEQL_TIME_OUT*/
	(unsigned char *) "������ʱ",
	/*STR_SEQL_ERROR*/
	(unsigned char *) "��������",
	/*STR_SEQL_CAPTION*/
	(unsigned char *) "�ַ�������",
	/*STR_SEQL_WAITING*/
	(unsigned char *) "���ں���\n\n��ȴ�...",
	/*STR_SEQL_CF*/
	(unsigned char *) "����ת��\n\n",
	/*STR_SEQL_CF_UNCODITIONAL*/
	(unsigned char *) "������ת��\n\n",
	/*STR_SEQL_CF_BUSY*/
	(unsigned char *) "��æת��\n\n",
	/*STR_SEQL_CF_NOREPLY*/
	(unsigned char *) "��Ӧ��ת��\n\n",
	/*STR_SEQL_CF_NOTREACHABLE*/
	(unsigned char *) "���ɴ�ת��\n\n",
	/*STR_SEQL_CF_ALL*/
	(unsigned char *) "ȫ��ת��\n\n",
	/*STR_SEQL_CF_ALLCONDITIONAL*/
	(unsigned char *) "ȫ������ת��\n\n",
	/*STR_SEQL_CLIR*/
	(unsigned char *) "���к�������\n",
	/*STR_SEQL_CLIP*/
	(unsigned char *) "������ʾ����\n\n",
	/*STR_SEQL_COLP*/
	(unsigned char *) "���Ӻ�����ʾ\n",
	/*STR_SEQL_CLIR_INDICATOR*/
	(unsigned char *) "Ĭ��\n",
	/*STR_SEQL_CLIR_INVOCATION*/
	(unsigned char *) "��\n",
	/*STR_SEQL_CLIR_SUPPRESSION*/
	(unsigned char *) "��\n",
	/*STR_SEQL_CLIR_NOTPROVISION*/
	(unsigned char *) "δ�ṩ�˷���\n",
	/*STR_SEQL_CLIR_PERMATNENT*/
	(unsigned char *) "�����ṩ\n",
	/*STR_SEQL_CLIR_UNKNOWN*/
	(unsigned char *) "δ֪\n",
	/*STR_SEQL_CLIR_TEMP_RESTRICT*/
	(unsigned char *) "��ʱ�������ṩ\n",
	/*STR_SEQL_CLIR_TEMP_ALLOW*/
	(unsigned char *) "��ʱ�ṩ\n",
	/*STR_SEQL_CLIP_ENABLE*/
	(unsigned char *) "��",
	/*STR_SEQL_CLIP_DISABLE*/
	(unsigned char *) "��",
	/*STR_SEQL_CLIP_NOTPROVISION*/
	(unsigned char *) "�޴˷���\n",
	/*STR_SEQL_CLIP_PROVISION*/
	(unsigned char *) "�ṩ\n",
	/*STR_SEQL_CLIP_UNKNOWN*/
	(unsigned char *) "����δ֪\n",
	/*STR_SEQL_CB*/
	(unsigned char *) "��������\n\n",
	/*STR_SEQL_CW*/
	(unsigned char *) "���еȴ�\n\n",
	/*STR_SEQL_CLASS_VOICE*/
	(unsigned char *) "����",
	/*STR_SEQL_CLASS_DATA*/
	(unsigned char *) "����",
	/*STR_SEQL_CLASS_FAX*/
	(unsigned char *) "����",
	/*STR_SEQL_CLASS_SHORTMESSAGE*/
	(unsigned char *) "����Ϣ",
	/*STR_SEQL_CLASS_ALL*/
	(unsigned char *) "����",
	/*STR_SEQL_STATUS_ENABLE*/
	(unsigned char *) "��\n",
	/*STR_SEQL_STATUS_DISABLE*/
	(unsigned char *) "��\n",
	/*STR_STK_TITLE*/
	(unsigned char *) "STK",
	/*STR_STK_LEFT_CAP_BTN*/
	(unsigned char *) "ȷ��",
	/*STR_STK_RIGHT_CAP_BTN*/
	(unsigned char *) "ȡ��",
	/*STR_STK_DEDITBOX_LENGTH_TOO_SHORT*/
	(unsigned char *) "���볤�Ȳ���!",
	/*STR_STK_FMT_NOT_SUPPORTED*/
	(unsigned char *) "��ʽ��֧��!",
	/*STR_STK_CALL_DEFAULT_NAME*/
	(unsigned char *) "STKӦ��",
	/*STR_STK_CALL_NOW*/
	(unsigned char *) "����ͨ��?",
	/*STR_STK_CALL_CONFIRM*/
	(unsigned char *) "��ȷ��",
	/*STR_CEER_1 */
	(unsigned char *) "��Ч����",
	/*STR_CEER_3 */
	(unsigned char *) "�û����ɼ�",
	/*STR_CEER_6 */
	(unsigned char *) "�޿����ŵ�",
	/*STR_CEER_8 */
	(unsigned char *) "��������",
	/*STR_CEER_16 */
	(unsigned char *) "ͨ������",
	/*STR_CEER_17 */
	(unsigned char *) "�Է�æ",
	/*STR_CEER_18 */
	(unsigned char *) "����Ӧ",
	/*STR_CEER_19 */
	(unsigned char *) "��Ӧ��",
	/*STR_CEER_21 */
	(unsigned char *) "���б��ܾ�",
	/*STR_CEER_22 */
	(unsigned char *) "�����Ѹ���",
	/*STR_CEER_26 */
	(unsigned char *) "ͨ����ֹ",
	/*STR_CEER_27 */
	(unsigned char *) "����ʧ��",	/* 600 */
	/*STR_CEER_28 */
	(unsigned char *) "���벻����",
	/*STR_CEER_29 */
	(unsigned char *) "�豸�ܾ�",
	/*STR_CEER_30 */
	(unsigned char *) "ͨ������",
	/*STR_CEER_31 */
	(unsigned char *) "ͨ������",
	/*STR_CEER_34 */
	(unsigned char *) "����æ",
	/*STR_CEER_38 */
	(unsigned char *) "����ʧ��",
	/*STR_CEER_41 */
	(unsigned char *) "����ʧ��",
	/*STR_CEER_42 */
	(unsigned char *) "����æ",
	/*STR_CEER_43 */
	(unsigned char *) "����ʧ��",
	/*STR_CEER_44 */
	(unsigned char *) "����ʧ��",
	/*STR_CEER_47 */
	(unsigned char *) "�޴˷���",
	/*STR_CEER_49 */
	(unsigned char *) "�޴˷���",
	/*STR_CEER_50 */
	(unsigned char *) "�޴˷���",
	/*STR_CEER_55 */
	(unsigned char *) "�û����ֹ",
	/*STR_CEER_57 */
	(unsigned char *) "����ʧ��",
	/*STR_CEER_58 */
	(unsigned char *) "����ʧ��",
	/*STR_CEER_63 */
	(unsigned char *) "�޴˷���",
	/*STR_CEER_65 */
	(unsigned char *) "����ʧ��",
	/*STR_CEER_68 */
	(unsigned char *) "����",
	/*STR_CEER_69 */
	(unsigned char *) "����ʧ��",
	/*STR_CEER_70 */
	(unsigned char *) "����ʧ��",
	/*STR_CEER_79 */
	(unsigned char *) "����ʧ��",
	/*STR_CEER_81 */
	(unsigned char *) "����ʧ��",
	/*STR_CEER_87 */
	(unsigned char *) "�û����ֹ",
	/*STR_CEER_88 */
	(unsigned char *) "����ʧ��",
	/*STR_CEER_91 */
	(unsigned char *) "������",
	/*STR_CEER_95 */
	(unsigned char *) "����ʧ��",
	/*STR_CEER_96 */
	(unsigned char *) "����ʧ��",
	/*STR_CEER_97 */
	(unsigned char *) "����ʧ��",
	/*STR_CEER_98 */
	(unsigned char *) "����ʧ��",
	/*STR_CEER_99 */
	(unsigned char *) "����ʧ��",
	/*STR_CEER_100 */
	(unsigned char *) "����ʧ��",
	/*STR_CEER_101 */
	(unsigned char *) "����ʧ��",
	/*STR_CEER_102 */
	(unsigned char *) "����ʧ��",
	/*STR_CEER_111 */
	(unsigned char *) "����ʧ��",
	/*STR_CEER_127 */
	(unsigned char *) "�ǹ̶�����",
	/*STR_CEER_240 */
	(unsigned char *) "����ʧ��",
	/*STR_CEER_241 */
	(unsigned char *) "���н�ֹ",
	/*STR_CEER_252 */
	(unsigned char *) "�����ֹ",
	/*STR_CEER_253 */
	(unsigned char *) "����ʧ��",	/* 640 */
	/*STR_CEER_254 */
	(unsigned char *) "����ʧ��",
	/*STR_CEER_255 */
	(unsigned char *) "״̬δ֪",
	/*STR_CC_NOTSUPPORT*/
	(unsigned char *) "�޴˹���!",		
	/*STR_CF_CANCEL_SUCC*/
	(unsigned char *) "ȡ������ת�Ƴɹ�!",
	/*STR_CF_CANCEL_FAILED*/
	(unsigned char *) "ȡ������ת��ʧ��!",
	/*STR_NS_TIME_OUT*/
	(unsigned char *) "��ʱ",
	/*STR_CH_ERROR*/
	(unsigned char *) "���ش���!",	
	/*STR_NS_NO_NETWORK_SERVICE*/
	(unsigned char *) "û���������",
	/*STR_NS_NO_PREFERRED_NETWORK*/
	(unsigned char *) "û����������",
	/*STR_NS_NO_AVAILALBLE_NETWORK*/
	(unsigned char *) "û�з��ֿɼ�����",	/* 650 */
	/*STR_SS_UNBLOCKPIN2_SUCC*/
	(unsigned char *) "PIN2������ɹ�!",
	/*STR_SS_PWD_LENGTH_4*/
	(unsigned char *) "���붼Ӧ��4λ!",
	/*STR_SS_PWD_LENGTH_6*/
	(unsigned char *) "���붼Ӧ��6λ!",		
	/*STR_SS_PWD_LENGTH_8*/
	(unsigned char *) "PUK��Ӧ��8λ!",
	/*STR_CB_BOICEXHC_5 */		/* 0904 add */
	(unsigned char *) "���γ����� ",
	/*STR_CB_BICROAM_5 */
	(unsigned char *) "ʱ�������� ",
	/*STR_CB_CANCEL_4 */
	(unsigned char *) "�к������� ",
	/*STR_SEQL_NOSERVICE*/	/*0906add*/
	(unsigned char *) "�޴˷���!",
	/*STR_SEQL_INVALID_PASSWORD*/ 
	(unsigned char *) "�������!",
	/*STR_SMS_CANCEL_ALL_SENDING */
	(unsigned char *) "ȡ��ȫ��",
	/*STR_SMS_SET_IN_SIM*/
	(unsigned char *) "SIM",
	/*STR_SMS_SET_IN_FLASH*/
	(unsigned char *) "�ֻ�",
	/*STR_CC_BEHELD */
	(unsigned char *) "ͨ��������",		
	/*STR_CC_BERECOVERED*/
	(unsigned char *) "ͨ�����ָ�",
	/*STR_HIM_KEYPAD_BLIGHT_SET*/
	(unsigned char *) "���̱���������",
	/*STR_HIM_LCD_BLIGHT_SET*/
	(unsigned char *) "��Ļ����������",	
	/*STR_PUBLIC_SYSTEM_BUSY */
	(unsigned char *) "ϵͳæ,���Ժ�...",
	/*STR_SMS_SIM_UNKNOWN*/
	(unsigned char *) "δ֪",
	/*STR_NS_REGISTER_OK*/
	(unsigned char *) "ע��ɹ�",
	/*STR_PM_VOLTAGE_POWER_OFF*/
	(unsigned char *) "����ѵ��ػ���ѹ",/* 670 */
	/*STR_HIM_CLOCK_TYPE*/
	(unsigned char *) "ʱ������",
	/*STR_HIM_NUM_CLOCK*/
	(unsigned char *) "�� ��",
	/*STR_HIM_SIMU_CLOCK*/
	(unsigned char *) "ģ ��",
	/*STR_CEER_USER_1*/
	(unsigned char *) "�绰δ��",	
	/*STR_SS_LENGTH_LESS_4*/
	(unsigned char *) "���Ȳ�ӦС��4!",
	/*IDS_SELECT*/
	(unsigned char *) "ѡ��",
	/*IDS_CAPTION*/
	(unsigned char *) "��ɽ�ʰ�",
	/*STR_PB_SIM_SETTING_MEMORY_TOTAL */
	(unsigned char *) "����:%s",
	/*STR_PB_SIM_SETTING_MEMORY_FREE */
	(unsigned char *) "ʣ��:%s",
	/*STR_VOICE_PROMPT*/
	(unsigned char *) "������ʾ",/* 680 */
	/*STR_TIME_PROMPT*/
	(unsigned char *) "ʱ����ʾ",
	/*STR_CONNECT_PROMPT*/
	(unsigned char *) "��ͨ��ʾ",
	/*STR_SMS_WRITER_TYPE_TITLE*/
	(unsigned char *)	"����Ϣ����ѡ��",/*liuyingxin add*//*683*/
	/*STR_EMS_WRITER_TITLE*/
	(unsigned char *)	"д��������",
	/*STR_EMS_PURE_TEXT*/
	(unsigned char *)	"�ı�����",
	/*STR_EMS_ANIMATION*/
	(unsigned char *)	"��������",
	/*STR_EMS_IN_COMMON_USE*/
	(unsigned char *)	"���ö���",
	/*STR_EMS_SEND_ITEM_1*/
	(unsigned char *)	"����",
	/*STR_EMS_SEND_ITEM_2*/
	(unsigned char *)	"�ݲ�",
	/*STR_EMS_SEND_ITEM_3*/
	(unsigned char *)	"��������",
	/*STR_EMS_SEND_ITEM_4*/
	(unsigned char *)	"˯��",
	/*STR_EMS_SEND_ITEM_5*/
	(unsigned char *)	"���տ���",
	/*STR_EMS_SEND_ITEM_6*/
	(unsigned char *)	"��������",
	/*STR_EMS_SEND_ITEM_7*/
	(unsigned char *)	"�������",
	/*STR_EMS_SEND_ITEM_8*/
	(unsigned char *)	"˼��",
	/*STR_EMS_SEND_ITEM_9*/
	(unsigned char *)	"ʥ������",
	/*STR_EMS_SEND_ITEM_10*/
	(unsigned char *)	"���ܽӴ�",
	/*STR_EMS_SEND_ITEM_11*/
	(unsigned char *)	"�������",
	/*STR_EMS_SEND_ITEM_12*/
	(unsigned char *)	"��л",
	/*STR_EMS_SEND_ITEM_13*/
	(unsigned char *)	"�Բ���",/*700*/
	/*STR_EMS_SEND_ITEM_14*/
	(unsigned char *)	"�¹�",
	/*STR_EMS_SEND_ITEM_15*/
	(unsigned char *)	"����",
	/*STR_EMS_SEND_ITEM_16*/
	(unsigned char *)	"ЦһЦ",
	/*STR_EMS_SEND_ITEM_17*/
	(unsigned char *)	"����",
	/*STR_EMS_SEND_ITEM_18*/
	(unsigned char *)	"һ·˳��",
	/*STR_EMS_SEND_ITEM_19*/
	(unsigned char *)	"�Ҷ���",
	/*STR_EMS_SEND_ITEM_20*/
	(unsigned char *)	"�Ҵ���",
	/*STR_EMS_MENU_INSERT*/
	(unsigned char *)	"����",
	/*STR_EMS_MENU_PREVIEW*/
	(unsigned char *)	"Ԥ��",
	/*STR_EMS_MENU_EXIT*/
	(unsigned char *)	"�˳�",
	/*STR_EMS_MENU_OK*/
	(unsigned char *)	"ȷ��",
	/*STR_EMS_SENDER_TITLE*/
	(unsigned char *)	"Ԥ��/ѡȡ����",
	/*STR_EMS_PLAYER_TITLE*/
	(unsigned char *)	"��������",
	/*STR_EMS_SEND*/
	(unsigned char *)	"����",
	/*STR_EMS_DETAIL*/
	(unsigned char *)	"����������ϸ",
	/*STR_EMS_SHORT_MESSAGE*/
	(unsigned char *)	"��������",
	/*STR_EMS_SMS_SHORT_MESSAGE*/
	(unsigned char *)	"�ı�����",
	/*STR_EMS_STATUS*/
	(unsigned char *)	"״̬: ",
	/*STR_EMS_STATUS_UNREAD*/
	(unsigned char *)	"δ�� ",
	/*STR_EMS_STATUS_READED*/
	(unsigned char *)	"�Ѷ� ",
	/*STR_EMS_STATUS_UNSEND*/
	(unsigned char *)	"δ�� ",
	/*STR_EMS_STATUS_UNALLREADY*/
	(unsigned char *)	"δ���� ",
	/*STR_EMS_STATUS_NUMBER*/
	(unsigned char *)	"����:",
	/*STR_EMS_STATUS_TOTALNUMBER*/
	(unsigned char *)	"����%d��",
	/*STR_EMS_STATUS_ALREADYRECEIVEDNUMBER*/
	(unsigned char *)	"���յ�%d��",
	/*STR_EMS_DETAIL_INFORMATION*/
	(unsigned char *)	"��ϸ",
	/*STR_EMS_LOOKOVER_UNRECEIVEDRALLEADY*/
	(unsigned char *)	"�鿴δ���붯������",
	/*STR_EMS_SUCCESS_DUMP*/
	(unsigned char *)	"ת���ɹ�!",
	/*STR_EMS_FAILED_DUMP*/
	(unsigned char *)	"ת��ʧ��!",
	/*STR_EMS_TOO_BIG*/
	(unsigned char *)	"����̫��!",
	/*STR_EMS_DATA_FORMAT_ERROR*/
	(unsigned char *)	"���ݸ�ʽ����!",
	/*STR_EMS_TOO_LONG*/
	(unsigned char *)	"����̫��!",
	/*STR_EMS_STORE_SPACE_OVERLOAD*/
	(unsigned char *)	"EMS�洢�ռ�����!",
	/*STR_EMS_SAVE_FAILED*/
	(unsigned char *)	"�洢ʧ��!",
	/*STR_EMS_ANIMATION_TITLE*/
	(unsigned char *)	"< �������� >",
	/*STR_EMS_PLAY_SELECTED*/
	(unsigned char *)	"������ѡ����",
	/*STR_EMS_DELETE_SELECTED*/
	(unsigned char *)	"ɾ����ѡ����",
	/*STR_EMS_FAILED_READ_FILE*/
	(unsigned char *) 	"���ļ�ʧ��",
	/*STR_EMS_NO_MEMORY*/
	(unsigned char *) 	"�ڴ治��",
	/*STR_EMS_NO_SYSTEM_RESOURCE*/
	(unsigned char *) 	"ϵͳ��Դ����",
	/*STR_EMS_UNSUPPORTED_FILE_FORMAT*/
	(unsigned char *) 	"��֧�ֵĶ��Ÿ�ʽ",
	/*STR_EMS_STORAGE_INFO*/
	(unsigned char *) 	"�������Ŵ洢��Ϣ",
	/*STR_EMS_VIEW_NONALLREADY_INFO*/
	(unsigned char *) 	"�鿴δ���붯��",
	/*STR_EMS_DUMP_WVG*/
	(unsigned char *) 	"ת�涯��",
	/*STR_EMS_MENU_SEE*/
	(unsigned char *) 	"�鿴",
	/*STR_EMS_TEXT*/
	(unsigned char *) 	"���������ı�",
	/*STR_EMS_ANIMATOR*/
	(unsigned char *)	"����",
	/*STR_EMS_LOADING*/
    (unsigned char *)	"װ����...",
	/*STR_EMS_SPLIT*/
    (unsigned char *)	"���Ž���%d������",	
	/*STR_EMS_FILE_NUMBER_TOO_MANY*/
    (unsigned char *)	"�����ļ����Ѵ����",
	/*STR_EMS_CANNOT_SAVE*/
    (unsigned char *)	"�޷��洢",/*liuyingxin end*/
	/*STR_SS_AutoLock_Close */
    (unsigned char *) 	"�Զ������ѹر�!",
    /*STR_NS_RESET */
    (unsigned char *) 	"����",
    /*STR_MENU_PENADJUST */
    (unsigned char *) 	"��У׼",
    /*STR_MENU_PERSONALSET*/
    (unsigned char *) 	"���Ի�����",
    /*STR_MENU_PERSONALDESK*/
    (unsigned char *) 	"���Ի�����",
    /*STR_MENU_CUSTOMSTR*/
    (unsigned char *) 	"������ӭ��", 
    /*STR_CC_NOTALLOWED*/
    (unsigned char *) 	"����������",  
    /*STR_VOICE_HINT_SETTING*/
    (unsigned char *) 	"������ʾ", 
    /*STR_PB_HANDLEING_DATA	*/
    (unsigned char *) 	"����������..", 
    /*STR_PS_SETTING_INIT*/
    (unsigned char *) 	"������ӭ��", 
    /*STR_PS_INFO_CLEAR*/
    (unsigned char *) 	"���", 
    /*STR_PS_INFO_EXIT*/
    (unsigned char *) 	"�˳�",	
    /*STR_PS_INFO_OK*/
    (unsigned char *) 	"ȷ��",
    /*STR_SMS_NEW_MSG_INCOMING*/
    (unsigned char *) 	"�յ��¶��š�\r\n�����Ķ�?",     	
    /*STR_INIT_NO_SIMCARD*/
    (unsigned char *) 	"û��SIM��!",	
    /*STR_INIT_CAN_NOT_INITION*/
    (unsigned char *) 	"�������޷���ʼ��",
    /*STR_INIT_CHECK_SIMCARD*/
    (unsigned char *) 	"����SIM��",
    /*STR_INIT_SIMCARD_DESTORYED*/
    (unsigned char *) 	"SIM���Ѿ��ٻ�",
    /*STR_INIT_SIMCARD_ERROR*/
    (unsigned char *) 	"SIM����!",
    /*STR_INIT_ERROR*/
    (unsigned char *) 	"����",	
    /*STR_HIM_BLIGHT*/
    (unsigned char *) 	"������",
    /*STR_PUBLIC_TIMESET*/
    (unsigned char *) 	"ʱ������",     	
    /*STR_SMS_DELETE_ALL*/
    (unsigned char *) 	"ɾ������",
    /*STR_SMS_DELETE_ALL_FLASH*/
    (unsigned char *) 	"�ֻ�",
    /*STR_SMS_DELETE_ALL_SIM*/
    (unsigned char *) 	"SIM��",     	
    /*STR_SMS_NO_FLASH_MSG*/
    (unsigned char *) 	"�ֻ���û����Ϣ!",
    /*STR_SMS_NO_SIM_MSG*/
    (unsigned char *) 	"SIM����û����Ϣ!",
    /*STR_NS_CALL_COME*/
    (unsigned char *) 	"�е绰����",  
    /*STR_INIT_DEL*/
    (unsigned char *) 	"ɾ��",   
    /*STR_PUBLIC_TIMESET_OK*/
  	(unsigned char *)	"ʱ�����óɹ�",
  	/*STR_PUBLIC_TIMESET_FAIL*/
  	(unsigned char *)  	"ʱ������ʧ��",	
  	/*STR_TIME_IS_SETTING*/
 	(unsigned char *)  	"ʱ�����ã����Ժ�",
 	/*STR_QT_INSERT*/
 	(unsigned char *)  	"����",
 	/*STR_PUBLIC_MON*/
 	(unsigned char *)  	"����һ",
 	/*STR_PUBLIC_TUE*/
 	(unsigned char *)  	"���ڶ�",
 	/*STR_PUBLIC_WED*/
 	(unsigned char *)  	"������",
 	/*STR_PUBLIC_THU*/
 	(unsigned char *)  	"������",
 	/*STR_PUBLIC_FRI*/
 	(unsigned char *)  	"������",
 	/*STR_PUBLIC_SAT*/
 	(unsigned char *)  	"������",
 	/*STR_PUBLIC_SUN*/
 	(unsigned char *)  	"������", 
 	/*STR_PUBLIC_PCMANAGER*/
 	(unsigned char *)  	"PC������",	
 	/*STR_INIT_RECOVERY_OK*/
 	(unsigned char *)  	"�ָ����óɹ�!",
 	/*STR_INIT_RECOVERY_FAIL*/
 	(unsigned char *)  	"�ָ�����ʧ��!",
 	/*STR_INIT_RECOVERY_NOTICE*/
 	(unsigned char *)  	"ע��:�������ö�\n\n���ָ�Ϊ��������\n\n(�����Զ��ػ�)",
 	/*STR_INIT_RECOVERY*/
 	(unsigned char *)  	"�ָ���������",
 	/*STR_INIT_RECOVERY_CONFIRM*/
 	(unsigned char *)  	"��ȷ����?",
 	/*STR_AS_ALARM_CLOCK*/
 	(unsigned char *)  	"����",
 	/*STR_AS_ALARM_SET*/
 	(unsigned char *)  	"��������",
 	/*STR_AS_ALARM_ONE*/
 	(unsigned char *)  	"����1",/*800*/
 	/*STR_AS_ALARM_TWO*/
 	(unsigned char *)  	"����2",
 	/*STR_AS_ALARM_THREE*/
 	(unsigned char *)  	"����3",
 	/*STR_AS_SET_TIME*/
 	(unsigned char *)  	"ʱ��:",
 	/*STR_AS_ALARM_STATUS*/
 	(unsigned char *)  	"״̬:", 
 	/*STR_CH_DELETE_ALL_CONFIRM*/
 	(unsigned char *)  	"ȷʵҪɾ����?",
	/*STR_PS_SETTING_INIT_OLD*/
    (unsigned char *)  "ԭ��ӭ��",	
	/*STR_PS_SETTING_INIT_NEW*/
    (unsigned char *)  "�»�ӭ��",      	
    /*STR_AS_ALARM_TYPE*/
    (unsigned char *)  "����:",
    /*STR_AS_RING_SELE2*/
    (unsigned char *)  "����:",
    /*STR_AS_ALARM_ONCE*/
    (unsigned char *)  "����һ��",
    /*STR_AS_ALARM_DAYS*/
    (unsigned char *)  "ÿ������",
    /*STR_AS_ALARM_ERROR*/
    (unsigned char *)  "���������ظ�",
    /*STR_ENGINE_MODE*/
    (unsigned char *)   "����ģʽ",
 	/*STR_CIT_MODE*/
    (unsigned char *)   "CIT ģʽ",
 	/*STR_ERR_LOG_MODE*/
	(unsigned char *)   "���������־",
 	/*STR_ATCMD_MODE*/
 	(unsigned char *)   "AT����ģʽ",
 	/*STR_DISPLAY_TEST*/
 	(unsigned char *)   "��Ļ����",
 	/*STR_BACKLIGHT_TEST*/
 	(unsigned char *)   "�����Ʋ���",
 	/*STR_DISPLAY_BACKLIGHT*/
    (unsigned char *)   "��Ļ�����Ʋ���",
 	/*STR_KEYPAD_BACKLIGHT*/
 	(unsigned char *)   "���̱����Ʋ���",
 	/*STR_ALERT_TEST*/
 	(unsigned char *)   "����������",
 	/*STR_VIBRATOR_TEST*/
 	(unsigned char *)   "�𶯲���",
 	/*STR_LED_TEST*/
 	(unsigned char *)   "LED �Ʋ���",
 	/*STR_VERSION_TEST*/
 	(unsigned char *)   "�汾��Ϣ",
 	/*STR_SW_VESION*/
    (unsigned char *)   "����汾��Ϣ",
 	/*STR_HW_VESION*/
 	(unsigned char *)   "Ӳ���汾��Ϣ",
 	/*STR_PDA_SIDE*/
    (unsigned char *)   "PDA �汾:",
 	/*STR_WAVECOME_SIDE*/
 	(unsigned char *)   "WAVECOME �汾:",
 	/*STR_KEYPAD_TEST*/
 	(unsigned char *)   "���̲���",
 	/*STR_OFF_KEY_EXIT*/
 	(unsigned char *)   "�밴�����Ҷϼ��˳���",
 	/*STR_TEST_ON*/
 	(unsigned char *)   "���Կ�ʼ",
	/*STR_TEST_OFF*/
	(unsigned char *)   "����ֹͣ",
	/*STR_EM_INPUT_ATCMD*/
	(unsigned char *)   "������ AT ����:",
	/*STR_EM_SEND_AT*/
	(unsigned char *)   "����",
	/*STR_EM_VIEW_AT_RESULT*/
	(unsigned char *)   "���Խ��",
	/*STR_EM_ERR_INFO*/
	(unsigned char *)   "������Ϣ",
	/*STR_BACKLIGHT_ON*/
    (unsigned char *)   "�򿪱�����",
 	/*STR_BACKLIGHT_OFF*/
 	(unsigned char *)   "�رձ�����",
 	/*STR_AS_SCHEDULE_ALARM*/
 	(unsigned char *)   "�ճ̱�����",  	
 	/*STR_QT_STATIC_5*/
 	(unsigned char *)   "������æ,�Ժ����ص�",
 	/*STR_QT_STATIC_6*/
 	(unsigned char *)   "�³�,�Ҽ����Ӻ�",
 	/*STR_QT_STATIC_7*/
 	(unsigned char *)   "������·����,���Ժ�",
 	/*STR_QT_STATIC_8*/
 	(unsigned char *)   "��л",
 	/*STR_QT_STATIC_9*/
 	(unsigned char *)   "ף�����念��",
 	/*STR_QT_STATIC_10*/
 	(unsigned char *)   "��ϲ����",
 	/*STR_QT_STATIC_11*/
 	(unsigned char *)   "�������,�ϼһ���",
 	/*STR_QT_STATIC_12*/
 	(unsigned char *)   "ף��һ·ƽ��",
 	/*STR_QT_STATIC_13*/
 	(unsigned char *)   "���տ���",
 	/*STR_QT_STATIC_14*/
 	(unsigned char *)   "���տ���",
 	/*STR_NS_EMERGENCY_ONLY*/
 	(unsigned char *)   "ֻ�ܽ�������", /*850*/
	/*STR_INIT_RECOVERY_WAIT*/
	(unsigned char *)   "���Ե�...",
	/*STR_PUBLIC_ISWAITING*/ 
	(unsigned char *)   "ϵͳæ��������!",
	/*STR_EMS_KILL*/ 
	(unsigned char *)   "ɾ��",
	/*STR_POKER_EASY*/
	(unsigned char *)   "����",
	/*STR_POKER_NORMAL*/ 
	(unsigned char *)   "�м�",
	/*STR_POKER_DIFFICULT*/ 
	(unsigned char *)   "�߼�",
};
