/*
 * file description here
 */

/*****************/
/* Include Files */
/*****************/


/********************/
/* Defines & Macros */
/********************/
typedef struct {
    char channel;
    char sub_chn;
    int value;
} DP_PACK_DATA;

typedef struct {
    char update;
    unsigned int value;
} MONITOR_DATA;

typedef struct {
    char modify;
    unsigned int current_value;
    unsigned int old_value;
} PARAM_SETTING;

#define MONITOR_NUM         (12)
#define MONITOR_VT          (0)     //������
#define MONITOR_MV          (1)     //����ͨ����
#define MONITOR_LEAK        (2)     //©����
#define MONITOR_RR          (3)     //����Ƶ��
#define MONITOR_IT          (4)     //����ʱ��
#define MONITOR_IE          (5)     //������
#define MONITOR_TS          (6)     //������ʽ
#define MONITOR_TR          (7)     //���������ٷֱ�
#define MONITOR_PP          (8)     //peak presure, ������ѹ
#define MONITOR_TVV_IPAP    (9)

#define PARAM_SETTING_NUM   (30)
#define PARAM_CPAPPRESS     (0)
#define PARAM_IPAP          (1)
#define PARAM_EPAP          (2)
#define PARAM_BPM           (3)
#define PARAM_IE            (4)
#define PARAM_ISENSE        (5)
#define PARAM_ESENSE        (6)
#define PARAM_ISLOP         (7)
#define PARAM_ESLOP         (8)
#define PARAM_MINIT         (9)
#define PARAM_MAXIT         (10)
#define PARAM_RAMP          (11)
#define PARAM_FIO2          (12)
#define PARAM_TVV_SW        (13)
#define PARAM_TVV_VOL       (14)
#define PARAM_TVV_MAXIP     (15)
#define PARAM_TVV_MINIP     (16)
#define PARAM_TVV_MAXEP     (17)    //not use
#define PARAM_TVV_MINEP     (18)    //not use
#define PARAM_PAV_EPAP      (19)
#define PARAM_PAV_K         (20)
#define PARAM_PAV_E         (21)
#define PARAM_PAV_R         (22)
#define PARAM_PAV_MAXP      (23)
#define PARAM_PAV_MAXV      (24)
//#define PARAM_AUTO_SW       (25)
#define PARAM_MED_SW        (26)    //��ҩ����
#define PARAM_MED_INTERVAL  (27)    //��ҩ���
#define PARAM_MED_TIME      (28)    //��ҩʱ��

#define ALARM_SETTING_NUM   (18)
#define ALARM_LOOP_BLOCK    (0)     //��·����
#define ALARM_APNEA         (1)     //��Ϣ
#define ALARM_MASK_OFF      (2)     //��������
#define ALARM_PRESS_HI      (3)     //ѹ������
#define ALARM_IPAP_HI       (4)     //����ѹ����
#define ALARM_IPAP_LOW      (5)     //����ѹ����
#define ALARM_NO_O2         (6)     //δ������
#define ALARM_O2P_LOW       (7)     //��ѹ������
#define ALARM_O2P_HI        (8)     //��ѹ������
#define ALARM_LEAK_LOW      (9)     //©��������
#define ALARM_LEAK_HI       (10)    //©��������
#define ALARM_BPM_LOW       (11)    //����Ƶ�ʹ���
#define ALARM_BPM_HI        (12)    //����Ƶ�ʹ���
#define ALARM_MV_LOW        (13)    //����ͨ��������
#define ALARM_VT_LOW        (14)    //����������
#define ALARM_VT_HI         (15)    //����������
#define ALARM_TVV_HI        (16)    //TVV����
#define ALARM_TVV_LOW       (17)    //TVV�ͱ�

/*************/
/* Variables */
/*************/
extern MONITOR_DATA  monitor_data[MONITOR_NUM];
extern PARAM_SETTING param_setting[PARAM_SETTING_NUM];
extern PARAM_SETTING alarm_setting[ALARM_SETTING_NUM];
extern int rt_volume;
extern int rt_pressure;
extern int rt_airflow;

/*************/
/* Functions */
/*************/
extern dp_code_package(DP_PACK_DATA * package_data,char * package);
extern dp_decode_package(char * package,DP_PACK_DATA * package_data);
extern dp_sort_package(DP_PACK_DATA * package_data);

