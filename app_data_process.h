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
#define MONITOR_VT          (0)     //潮气量
#define MONITOR_MV          (1)     //分钟通气量
#define MONITOR_LEAK        (2)     //漏气量
#define MONITOR_RR          (3)     //呼吸频率
#define MONITOR_IT          (4)     //吸气时间
#define MONITOR_IE          (5)     //吸呼比
#define MONITOR_TS          (6)     //触发方式
#define MONITOR_TR          (7)     //自主触发百分比
#define MONITOR_PP          (8)     //peak presure, 吸气峰压
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
#define PARAM_MED_SW        (26)    //喷药开关
#define PARAM_MED_INTERVAL  (27)    //喷药间隔
#define PARAM_MED_TIME      (28)    //喷药时间

#define ALARM_SETTING_NUM   (18)
#define ALARM_LOOP_BLOCK    (0)     //回路堵塞
#define ALARM_APNEA         (1)     //窒息
#define ALARM_MASK_OFF      (2)     //面罩脱落
#define ALARM_PRESS_HI      (3)     //压力过高
#define ALARM_IPAP_HI       (4)     //吸气压过高
#define ALARM_IPAP_LOW      (5)     //吸气压过低
#define ALARM_NO_O2         (6)     //未接氧气
#define ALARM_O2P_LOW       (7)     //氧压力过低
#define ALARM_O2P_HI        (8)     //氧压力过高
#define ALARM_LEAK_LOW      (9)     //漏气量过低
#define ALARM_LEAK_HI       (10)    //漏气量过高
#define ALARM_BPM_LOW       (11)    //呼吸频率过低
#define ALARM_BPM_HI        (12)    //呼吸频率过高
#define ALARM_MV_LOW        (13)    //分钟通气量过低
#define ALARM_VT_LOW        (14)    //潮气量过低
#define ALARM_VT_HI         (15)    //潮气量过高
#define ALARM_TVV_HI        (16)    //TVV超标
#define ALARM_TVV_LOW       (17)    //TVV低标

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

