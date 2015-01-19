/*
 * file description here
 */

/*****************/
/* Include Files */
/*****************/
#include "common.h"

#include <stdio.h>
#include <unistd.h>

/********************/
/* Defines & Macros */
/********************/


/*************/
/* Variables */
/*************/
CHAR_DATA_BUFFER sys_received_cache;

/*************/
/* Functions */
/*************/
int push_data_buffer(CHAR_DATA_BUFFER buffer, char *data, unsigned int num)
{
    char *start = buffer.data;
    char *end = buffer.p_end;
    char *push = buffer.p_push;
    char *pop = buffer.p_pop;
    int push_count = 0;
    char push_overwrite_flag = 0;
    int timeout_count = 5;

    /* Check parameters */
    if(data == NULL) {
        DEBUG_PRINTF("<%s>Input parameter error.\n", __FUNCTION__);
        return (-2);
    }
    /* Limit input data number. Min = 1, Max = buffer size. */
    if((num == 0) || (num > buffer.size)) {
        DEBUG_PRINTF("<%s>Input parameter error.\n", __FUNCTION__);
        return (-2);
    }

    /* Avoid data buffer access conflict. */
    /* Get buffer control. Time out 0.5ms. */
    while((1 == buffer.use_flag) && (timeout_count > 0)) {
        usleep(100);    //sleep 0.1ms
        timeout_count--;
        if(timeout_count == 0) {
            DEBUG_PRINTF("<%s>Get buffer access time out.\n", __FUNCTION__);
            return (-3);
        }
    }
    buffer.use_flag = 1;

    /* Push data into buffer. */
    /* Allow new data to overwrite unused old data. */
    for(; push_count < num; push_count++) {
        *(push++) = *(data++);
        if(push == end) {
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
        buffer.p_pop = buffer.p_push;
        buffer.valid_num = buffer.size;
    }
    else {
        buffer.valid_num += push_count;
    }
    buffer.p_push = push;
    /* Release buffer control. */
    buffer.use_flag = 0;

    /* Return successful pushed data number. */
    return (push_count);
}


int pop_data_buffer(CHAR_DATA_BUFFER buffer, char *data, unsigned int num)
{
    char *start = buffer.data;
    char *end = buffer.p_end;
    //char *push = buffer.p_push;
    char *pop = buffer.p_pop;
    int pop_count = 0;
    int timeout_count = 5;

    /* Check parameters */
    if(data == NULL) {
        DEBUG_PRINTF("<%s>Input parameter error.\n", __FUNCTION__);
        return (-2);
    }
    /* Limit output data number. Min = 1, Max = valid data number. */
    if((num == 0) || (num > buffer.valid_num)) {
        DEBUG_PRINTF("<%s>Input parameter error.\n", __FUNCTION__);
        return (-2);
    }

    /* Avoid data buffer access conflict. */
    /* Get buffer control. Time out 0.5ms. */
    while((1 == buffer.use_flag) && (timeout_count > 0)) {
        usleep(100);    //sleep 0.1ms
        timeout_count--;
        if(timeout_count == 0) {
            DEBUG_PRINTF("<%s>Get buffer access time out.\n", __FUNCTION__);
            return (-3);
        }
    }
    buffer.use_flag = 1;

    /* Pop data from buffer. */
    for(; pop_count < num; pop_count++) {
        *(data++) = *(pop++);
        if(pop == end) {
            pop = start;
        }
    }

    /* Refresh buffer info. */
    buffer.p_pop = pop;
    buffer.valid_num -= pop_count;
    /* Release buffer control. */
    buffer.use_flag = 0;
    
    /* Return successful poped data number. */
    return(pop_count);
}
