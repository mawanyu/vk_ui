/*
 * file description here
 */

#ifndef _COMMON_H_
#define _COMMON_H_
/*****************/
/* Include Files */
/*****************/
#include <semaphore.h>  //for sem_t

/********************/
/* Defines & Macros */
/********************/
/* Define debug print switcher */
#define DEBUG
#ifdef DEBUG
    #define DEBUG_PRINTF(format, args...) printf(format, ##args)
#else
    #define DEBUG_PRINTF(format, args...)
#endif

typedef struct {
    char *data;
    unsigned int size;
    char *p_start;
    char *p_end;
    char *p_push;
    char *p_pop;

    unsigned int valid_num;
    char use_flag;
} CHAR_DATA_BUFFER;

/*************/
/* Variables */
/*************/
/* Buffer for received data from spi and uart. */
extern CHAR_DATA_BUFFER sys_received_cache;

extern char patient_trigger_run;

extern unsigned long cb_alarm_info;
extern unsigned int  pb_alarm_info;

extern unsigned long cb_sys_status;

extern char cb_handshake;              //标志控制板是否向接口板发送的握手信号
extern char cb_trans_rt_data;          //标志控制板是否向接口板发送实时数据

struct sys_rtc_struct {
    unsigned long value;
    char year;
    char mon;
    char day;
    char hour;
    char min;
    char sec;
    char update;
};

extern struct sys_rtc_struct sys_rtc;

extern char knob_turn_left;
extern char knob_turn_right;
extern char knob_push_key;
extern char start_key;
extern char mute_key;

extern int trans_pack_num;

/*************/
/* Functions */
/*************/
extern int push_data_buffer(CHAR_DATA_BUFFER * buffer,char * data,unsigned int num);
extern int pop_data_buffer(CHAR_DATA_BUFFER * buffer,char * data,unsigned int num);
extern int pop_data_buffer_package(CHAR_DATA_BUFFER * buffer,char * package);

#endif/*_COMMON_H_*/
