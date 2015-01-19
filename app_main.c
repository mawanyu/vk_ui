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

/*************/
/* Functions */
/*************/
int main(int argc, char *argv[])
{
    int ret = 0;

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
        printf("[Error]Create spi receive thread error.\n%s\n", strerror(ret));
        return (-1);
    }

    ret = pthread_create(&tid_uart_pb_rx, NULL, (void*)&thread_uart_pb_receive, 0);
    if(ret != 0) {
        printf("[Error]Create UART power board receive thread error.\n%s\n", strerror(ret));
        return (-1);
    }
    
}

