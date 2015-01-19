/*
 * file description here
 */

#ifndef _COMMON_H_
#define _COMMON_H_
/*****************/
/* Include Files */
/*****************/

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

/*************/
/* Functions */
/*************/
extern int push_data_buffer(CHAR_DATA_BUFFER buffer,char * data,unsigned int num);
extern int pop_data_buffer(CHAR_DATA_BUFFER buffer,char * data,unsigned int num);

#endif/*_COMMON_H_*/
