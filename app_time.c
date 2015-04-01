/*
 * file description here
 */

/*****************/
/* Include Files */
/*****************/
#include "common.h"
#include "app_time.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>     //for signal()
#include <string.h>     //for memset()

/********************/
/* Defines & Macros */
/********************/

/*************/
/* Variables */
/*************/

/*************/
/* Functions */
/*************/
int dummy_cnt = 1;
void dummy(int signum)
{
    printf("Timer reached, count = %d\n", dummy_cnt);
    dummy_cnt++;
    signal(SIGALRM, dummy);
}

int timer_setup(int interval, void (*handler)(int))
{
    int ret = 0;
    struct itimerval timer;

    /* Check input parameter */
    if((interval == 0) || (handler == NULL)) {
        DEBUG_PRINTF("<%s>Input parameter error.\n", __FUNCTION__);
        return (-2);
    }
    
    /* Registe function to SLGALRM. */
    signal(SIGALRM, (__sighandler_t)handler);

    /* Set timer interval to 0.1s. */
    memset((void*)&timer, 0, sizeof(timer));
    timer.it_value.tv_sec  = interval / 1000;
    timer.it_value.tv_usec = (interval % 1000) * 1000;
    timer.it_interval.tv_sec  = interval /1000;
    timer.it_interval.tv_usec = (interval % 1000) * 1000;
    ret = setitimer(ITIMER_REAL, &timer, NULL);
    if(ret != 0) {
        printf("<%s>Set %dms timer fail.\n", __FUNCTION__, interval);
        return (-1);
    }

    return 0;
}

/* 判断是否是闰年 */
static char is_rn(int year)
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


