/*
 * file description here
 */

/*****************/
/* Include Files */
/*****************/
#include "common.h"
#include "gpio.h"
#include "spi.h"
#include "uart.h"
#include "app_thread.h"
#include "app_data_process.h"
#include "app_time.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>

/********************/
/* Defines & Macros */
/********************/


/*************/
/* Variables */
/*************/
pthread_t tid_spi_rx;
pthread_t tid_uart_pb_rx;
pthread_t tid_timer;
pthread_t tid_knob_key;
pthread_t tid_uart_cb_rx;

char run_next = 0;

/*************/
/* Functions */
/*************/
void send_command_to_cb(int arg);


int main(int argc, char *argv[])
{
    int ret = 0;
    char *tpack = NULL;
    DP_PACK_DATA tpackdata;

    {
        sys_received_cache.data = (char*)malloc(512);
        sys_received_cache.size = 512;
        sys_received_cache.p_start = sys_received_cache.data;
        sys_received_cache.p_end = sys_received_cache.p_start + 512 - 1;
        sys_received_cache.p_pop = sys_received_cache.p_start;
        sys_received_cache.p_push = sys_received_cache.p_start;
        sys_received_cache.valid_num = 0;
        sys_received_cache.use_flag = 0;
    }

    /* Module initialise. */
    ret = gpio_initialise();
    if(ret != GPIO_OK) {
        DEBUG_PRINTF("<%s>:[%d]GPIO initialise fail.\n", __FUNCTION__, ret);
    }

    ret = spi_initialise();
    if(ret != SPI_OK) {
        DEBUG_PRINTF("<%s>:[%d]SPI initialise fail.\n", __FUNCTION__, ret);
    }

    ret = uart_initialise();
    if(ret != UART_OK) {
        DEBUG_PRINTF("<%s>:[%d]UART initialise fail.\n", __FUNCTION__, ret);
    }

    /* Create threads. */
    ret = pthread_create(&tid_spi_rx, NULL, (void*)&thread_spi_receive, 0);
    if(ret != 0) {
        printf("[Error]Create spi receive thread error.\t%s\n", strerror(ret));
        return (-1);
    }

    ret = pthread_create(&tid_uart_pb_rx, NULL, (void*)&thread_uart_pb_receive, 0);
    if(ret != 0) {
        printf("[Error]Create UART power board receive thread error.\t%s\n", strerror(ret));
        return (-1);
    }

    ret = pthread_create(&tid_knob_key, NULL, (void*)&thread_knob_key, 0);
    if(ret != 0) {
        printf("[Error]Create knob and key thread error.\t%s\n", strerror(ret));
        return (-1);
    }

    ret = pthread_create(&tid_uart_cb_rx, NULL, (void*)&thread_uart_cb_receive, 0);
    if(ret != 0) {
        printf("[Error]Create UART control board receive thread error.\t%s\n", strerror(ret));
        return (-1);
    }

    /* Setup 0.1s timer */
    //ret = timer_setup(100, dummy);
    ret = timer_setup(1000, (__sighandler_t)send_command_to_cb);

    tpack = (char*)malloc(4);

    while(1) {
        while(sys_received_cache.valid_num > 3) {
            memset((void*)tpack, 0x0, 4);
            memset((void*)&tpackdata, 0x0, sizeof(tpackdata));
            
            ret = pop_data_buffer_package(&sys_received_cache, tpack);
            if(ret == 0) {
                run_next = 1;
            }
            else if(ret == -4) {
                printf("Not enough date in buffer.\n");
                run_next = 0;
            }
            else {
                printf("[Error]Pop data from received buffer fail. (%d)\n",ret);
                run_next = 0;
            }

            if(1 == run_next) {
                ret = dp_decode_package(tpack, &tpackdata);
                if(ret != 0) {
                    printf("[Error]Decode a tranmit package fail. (%d)\n", ret);
                }
                ret = dp_sort_package(&tpackdata);
                if(ret != 0) {
                    printf("[Error]Sort package data fail. (%d)\n", ret);
                }
            }
        }//while(valid_num>3)

        /* Calculate current time if it is updated */
        if(sys_rtc.update == 0x11) {
            calculate_time();
        }

        /* Handle knob and key action */
        if(knob_turn_left) {
            knob_turn_left = 0;
            printf("Knob turn left  <-\n");
        }
        if(knob_turn_right) {
            knob_turn_right = 0;
            printf("Knob turn right  ->\n");
        }
        if(knob_push_key) {
            knob_push_key = 0;
            printf("Knob push\n");
        }
        if(start_key) {
            start_key = 0;
            printf("START key push\n");
        }
        if(mute_key) {
            mute_key = 0;
            printf("MUTE key push\n");
        }

        if(uart_cb_loopback.flag == 1) {
            int i;
            for(i=0; i<4; i++) {
                printf("%d  ", uart_cb_loopback.data[i]);
            }
            printf("\n");
            uart_cb_loopback.flag = 0;
        }
    }//while(1)
}

extern int dummy_cnt;
void send_command_to_cb(int arg)
{
    char cb_trans[4];
    
    cb_trans[0] = 0xab;
    cb_trans[1] = 0xcd;
    cb_trans[2] = 0xef;
    cb_trans[3] = dummy_cnt++;
    memset((void*)uart_cb_loopback.data, 0, 4);
    uart_cb_loopback.flag = 0;
    uart_transfer(UART_PORT_CB, cb_trans, 4);

    signal(SIGALRM, (__sighandler_t)send_command_to_cb);
}
