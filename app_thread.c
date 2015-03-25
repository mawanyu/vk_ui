/*
 * file description here
 */

/*****************/
/* Include Files */
/*****************/
#include "app_thread.h"
#include "common.h"
#include "gpio.h"
#include "spi.h"
#include "uart.h"

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <poll.h>
#include <unistd.h>
#include <signal.h>
//#include <fcntl.h>
#include <sys/time.h>
//#include <sys/types.h>
//#include <sys/ioctl.h>

/********************/
/* Defines & Macros */
/********************/
#define SPI_INT_WAIT_TIMEOUT    (50)    //50ms
#define SPI_TRANS_BYTES         (8)

#define UART_PB_RX_BYTES        (8)

/*************/
/* Variables */
/*************/

/*************/
/* Functions */
/*************/
void thread_spi_receive(void)
{
    int ret = 0;
    int gpio_spi_ready_fd;
    char spi_rx_data[SPI_TRANS_BYTES];
    struct pollfd fd_set;

    ret = gpio_open(GPIO_NUM_SPI_READY, &gpio_spi_ready_fd);
    if(ret != 0) {
        DEBUG_PRINTF("<%s>Open spi ready gpio fail.\n", __FUNCTION__);
        return;
    }


    while(1) {
        memset((void *)spi_rx_data, 0, SPI_TRANS_BYTES);
        memset((void *)&fd_set, 0, sizeof(fd_set));

        fd_set.fd     = gpio_spi_ready_fd;
        fd_set.events = POLLPRI;
        ret = poll(&fd_set, (nfds_t)1, SPI_INT_WAIT_TIMEOUT);
        if(ret == 0) {
            DEBUG_PRINTF("<%s>Poll timeout.\n", __FUNCTION__);
        }
        else if(ret == -1) {
            DEBUG_PRINTF("<%s>Poll error.\n", __FUNCTION__);
        }
        else {
            if(fd_set.revents & POLLPRI) {
                //clear interrupt status
                read(fd_set.fd, &ret, 1);

                ret = spi_transfer(NULL, spi_rx_data, SPI_TRANS_BYTES);
                if(ret != 0) {
                    DEBUG_PRINTF("<%s>Get data from spi fail.\n", __FUNCTION__);
                    continue;
                }

                push_data_buffer(&sys_received_cache, spi_rx_data, SPI_TRANS_BYTES);
            }
            else {
                DEBUG_PRINTF("<%s>Poll error.\n", __FUNCTION__);
            }
        }
    }
}


void thread_uart_pb_receive(void)
{
    int ret = 0;
    char uart_pb_rx_data[UART_PB_RX_BYTES];

    while(1) {
        memset((void*)uart_pb_rx_data, 0, UART_PB_RX_BYTES);

        ret = uart_receive(UART_PORT_PB, uart_pb_rx_data, UART_PB_RX_BYTES);
        if(ret < 0) {
            DEBUG_PRINTF("<%s>Get data from uart of power board fail.\n", __FUNCTION__);
            continue;
        }

        push_data_buffer(&sys_received_cache, uart_pb_rx_data, UART_PB_RX_BYTES);
    }
}

int dummy_cnt = 1;
void dummy(void)
{
    printf("Timer reached, count = %d\n", dummy_cnt);
    dummy_cnt++;
    signal(SIGALRM, dummy);
    sem_post(&timer_sem);
}

void thread_timer(void)
{
    int ret = 0;
    struct itimerval timer;

    /* Initialise timer semaphore. */
    ret = sem_init(&timer_sem, 0, 0);
    if(ret != 0) {
        printf("<%s>Initialise timer semaphore fail.\n", __FUNCTION__);
        pthread_exit(NULL);
    }
    
    /* Registe function to SLGALRM. */
    signal(SIGALRM, dummy);

    /* Set timer interval to 0.1s. */
    memset((void*)&timer, 0, sizeof(timer));
    timer.it_value.tv_sec  = 0;
    timer.it_value.tv_usec = 100*1000;
    timer.it_interval.tv_sec  = 0;
    timer.it_interval.tv_usec = 100*1000;
    ret = setitimer(ITIMER_REAL, &timer, NULL);
    if(ret != 0) {
        printf("<%s>Set 0.1s timer fail.\n", __FUNCTION__);
        pthread_exit(NULL);
    }

    /* Wait for timer signal. */
    while(1) {
        sem_wait(&timer_sem);
    }
}
