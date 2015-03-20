/*
 * file description here
 */

/*****************/
/* Include Files */
/*****************/
#include "common.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <semaphore.h>

/********************/
/* Defines & Macros */
/********************/


/*************/
/* Variables */
/*************/
CHAR_DATA_BUFFER sys_received_cache;

sem_t timer_sem;

char patient_trigger_run = 0;

unsigned long cb_alarm_info = 0;
unsigned int  pb_alarm_info = 0;

unsigned long cb_sys_status = 0;

char cb_handshake = 0;              //标志控制板是否向接口板发送的握手信号
char cb_trans_rt_data = 0;          //标志控制板是否向接口板发送实时数据

struct sys_rtc_struct sys_rtc;

/*************/
/* Functions */
/*************/
int push_data_buffer(CHAR_DATA_BUFFER *buffer, char *data, unsigned int num)
{
    char *start = buffer->p_start;
    char *end = buffer->p_end;
    char *push = buffer->p_push;
    char *pop = buffer->p_pop;
    int push_count = 0;
    char push_overwrite_flag = 0;
    int timeout_count = 5;

    /* Check parameters */
    if((data == NULL) || (buffer == NULL)) {
        DEBUG_PRINTF("<%s>Input parameter error.\n", __FUNCTION__);
        return (-2);
    }
    /* Limit input data number. Min = 1, Max = buffer size. */
    if((num == 0) || (num > buffer->size)) {
        DEBUG_PRINTF("<%s>Input parameter error.\n", __FUNCTION__);
        return (-2);
    }

    /* Avoid data buffer access conflict. */
    /* Get buffer control. Time out 0.5ms. */
    while((1 == buffer->use_flag) && (timeout_count > 0)) {
        usleep(100);    //sleep 0.1ms
        timeout_count--;
        if(timeout_count == 0) {
            DEBUG_PRINTF("<%s>Get buffer access time out.\n", __FUNCTION__);
            return (-3);
        }
    }
    buffer->use_flag = 1;

    /* Push data into buffer. */
    /* Allow new data to overwrite unused old data. */
    for(; push_count < num; push_count++) {
        *(push++) = *(data++);
        if(push > end) {
            push = start;
        }
        /* Mark if new data overwrite old data. */
        if(push == pop) {
            push_overwrite_flag = 1;
            printf("[Caution]Data buffer overwrited.\n");
        }
    }

    /* Refresh buffer info. */
    /* If overwrite occurs, make sure to pop from the new data. */
    if(1 == push_overwrite_flag) {
        buffer->p_pop = buffer->p_push;
        buffer->valid_num = buffer->size;
    }
    else {
        buffer->valid_num += push_count;
    }
    buffer->p_push = push;
    /* Release buffer control. */
    buffer->use_flag = 0;

    /* Return successful pushed data number. */
    return (push_count);
}


int pop_data_buffer(CHAR_DATA_BUFFER *buffer, char *data, unsigned int num)
{
    char *start = buffer->p_start;
    char *end = buffer->p_end;
    //char *push = buffer->p_push;
    char *pop = buffer->p_pop;
    int pop_count = 0;
    int timeout_count = 5;

    /* Check parameters */
    if((buffer == NULL) || (data == NULL) || (num == 0)) {
        DEBUG_PRINTF("<%s>Input parameter error.\n", __FUNCTION__);
        return (-2);
    }
    /* Check if there is enough data in buffer to pop */
    if(num > buffer->valid_num) {
        DEBUG_PRINTF("<%s>No enough data in buffer.\n", __FUNCTION__);
        return (-4);
    }

    /* Avoid data buffer access conflict. */
    /* Get buffer control. Time out 0.5ms. */
    while((1 == buffer->use_flag) && (timeout_count > 0)) {
        usleep(100);    //sleep 0.1ms
        timeout_count--;
        if(timeout_count == 0) {
            DEBUG_PRINTF("<%s>Get buffer access time out.\n", __FUNCTION__);
            return (-3);
        }
    }
    buffer->use_flag = 1;

    /* Pop data from buffer. */
    for(; pop_count < num; pop_count++) {
        *(data++) = *(pop++);
        if(pop > end) {
            pop = start;
        }
    }

    /* Refresh buffer info. */
    buffer->p_pop = pop;
    buffer->valid_num -= pop_count;
    /* Release buffer control. */
    buffer->use_flag = 0;
    
    /* Return successful poped data number. */
    return(pop_count);
}


int pop_data_buffer_package(CHAR_DATA_BUFFER* buffer, char* package)
{
    //char *start = buffer.p_start;
    //char *end = buffer.p_end;
    //char *push = buffer.p_push;
    //char *pop = buffer.p_pop;
    char *pop_data = package;
    int package_size = 4;
    int pop_count = 0;
    int timeout_count = 5;

    /* Check parameters */
    if((buffer == NULL) || (package == NULL)) {
        DEBUG_PRINTF("<%s>Input parameter error.\n", __FUNCTION__);
        return (-2);
    }
    
    /* Avoid data buffer access conflict. */
    /* Get buffer control. Time out 0.5ms. */
    while((1 == buffer->use_flag) && (timeout_count > 0)) {
        usleep(100);    //sleep 0.1ms
        timeout_count--;
        if(timeout_count == 0) {
            DEBUG_PRINTF("<%s>Get buffer access time out.\n", __FUNCTION__);
            return (-3);
        }
    }
    buffer->use_flag = 1;

FIND_PACK:
    /* Check if there is enough data in buffer to pop */
    if(buffer->valid_num < package_size) {
        DEBUG_PRINTF("<%s>No enough data in buffer.\n", __FUNCTION__);
        buffer->use_flag = 0;
        return (-4);
    }

    pop_data = package;

    /* Pop data from buffer. */
    //find package header
    if((*buffer->p_pop & 0x80) == 0) {
        //not a package header
        buffer->p_pop++;
        if(buffer->p_pop > buffer->p_end) {
            buffer->p_pop = buffer->p_start;
        }
        buffer->valid_num--;
        goto FIND_PACK;
    }
    else {
        //find a package header
        *(pop_data++) = *(buffer->p_pop++);
    }
    //check if it is a valid package
    //continue to find next header if not
    if(((*buffer->p_pop & 0x80) != 0) || ((*(buffer->p_pop+1) & 0x80) != 0) \
        ||((*(buffer->p_pop+2) & 0x80) != 0))
    {
        //not a valid package
        buffer->p_pop++;
        if(buffer->p_pop > buffer->p_end) {
            buffer->p_pop = buffer->p_start;
        }
        buffer->valid_num--;
        goto FIND_PACK;
    }
    else {
        //get whole package
        for(pop_count=1; pop_count<package_size; pop_count++) {
            *(pop_data++) = *(buffer->p_pop++);
            if(buffer->p_pop > buffer->p_end) {
                buffer->p_pop = buffer->p_start;
            }
        }
        buffer->valid_num -= 3;
    }

    buffer->use_flag = 0;

    return 0;
}
