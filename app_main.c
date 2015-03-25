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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>

/********************/
/* Defines & Macros */
/********************/


/*************/
/* Variables */
/*************/
pthread_t tid_spi_rx;
pthread_t tid_uart_pb_rx;
pthread_t tid_timer;

char run_next = 0;

/*************/
/* Functions */
/*************/
char is_rn(int year);
void calculate_time(void);


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

    ret = pthread_create(&tid_timer, NULL, (void*)&thread_timer, 0);
    if(ret != 0) {
        printf("[Error]Create timer thread error.\t%s\n", strerror(ret));
        return (-1);
    }

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
        }

        /* Calculate current time if it is updated */
        if(sys_rtc.update == 0x11) {
            calculate_time();
        }
    };
}


char is_rn(int year)
{
    if(((0 == year % 4) && (0 != year % 100)) || (0 == year % 400)) {
        return 0x1;
    }

    return 0x0;
}

void calculate_time(void)
{
    int year = 2000;
    int day = sys_rtc.value / (24 * 3600);
    int month, hour, minute, second;

    /* Calculate year */
    while(day > 365) {
        if(is_rn(year)) {
            day -= 366;
        }
        else {
            day -= 365;
        }
        year++;
    }

    if((day == 365) || !(is_rn(year))) {
        year++;
    }

    /**/
    if(is_rn(year)) {
        if(day <= 31) {
            month = 1;
        }
        else if(day <= 60 ) {
            month = 2;
            day -= 31;
        }
        else if(day <= 91) {
            month = 3;
            day -= 60;
        }
        else if ( day <= 121 ) {
			month = 4;
			day -= 91;
		}
   		else if ( day <= 152 ) {
			month = 5;
			day -= 121;
		}
		else if ( day <= 182) {
			month = 6;
			day -= 152;
		}
		else if ( day <= 213 ) {
			month = 7;
			day -= 182;
		}
		else if ( day <= 244 ) {
			month = 8;
			day -= 213;
		}
		else if ( day <= 274 ) {
			month = 9;
			day -= 244;
		}
		else if ( day <= 305 ) {
			month = 10;
			day -= 274;
		}
		else if ( day <= 335 ) {
			month = 11;
			day -= 305;
		}
		else if ( day > 335 ) {
			month = 12;
			day -= 335;
		}
    }
    //
    else {
        if ( day <= 31 ) {
			month = 1;
		}
		else if ( day <= 59 ) {
			month = 2;
			day -= 31;
		}
		else if ( day <= 90 ) {
			month = 3;
			day -= 59;
		}
		else if ( day <= 120 ) {
			month = 4;
			day -= 90;
		}
		else if ( day <= 151 ) {
			month = 5;
			day -= 120;
		}
		else if ( day <= 181) {
			month = 6;
			day -= 151;
		}
		else if ( day <= 212 ) {
			month = 7;
			day -= 181;
		}
		else if ( day <= 243 ) {
			month = 8;
			day -= 212;
		}
		else if ( day <= 273 ) {
			month = 9;
			day -= 243;
		}
		else if ( day <= 304 ) {
			month = 10;
			day -= 273;
		}
		else if ( day <= 334 ) {
			month = 11;
			day -= 304;
		}
		else if ( day > 334 ) {
			month = 12;
			day -= 334;
		}
    }

    day = day + 1; //之前求出来的天数是已经过去的天数，现在加1表示现在日期。

    /* 以下部分用于求现在的时间 */
	hour = (sys_rtc.value / 3600 ) % 24; 
	minute = (sys_rtc.value / 60) % 60;
	second = sys_rtc.value % 60;

	/**/
	sys_rtc.year = year;
	sys_rtc.mon  = month;
	sys_rtc.day  = day;
	sys_rtc.hour = hour;
	sys_rtc.min  = minute;
	sys_rtc.sec  = second;
}

