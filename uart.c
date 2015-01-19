/*
 * file description here
 */

/*****************/
/* Include Files */
/*****************/
#include "uart.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

/********************/
/* Defines & Macros */
/********************/
#define UART_PATH_CB    "/dev/ttymxc0"
#define UART_PATH_PB    "/dev/ttymxc2"
#define UART_PATH_USER  "/dev/ttymxc3"

/*************/
/* Variables */
/*************/
int uart_fd[UART_PORT_NUM];

/*************/
/* Functions */
/*************/
static int uart_cb_init(void)
{
    struct termios tio;
    int ret = 0;
    int fd = 0;

    fd = open(UART_PATH_CB, O_RDWR|O_NOCTTY|O_NDELAY);
    if(fd < 0) {
        printf("[Caution]Open UART CB port fail.\n");
        goto U0_EXIT_OPEN;
    }

    /* Get UART port attribution. */
    ret = tcgetattr(fd, &tio);
    if(ret != 0) {
        DEBUG_PRINTF("<%s>Get UART0 attr fail.\n", __FUNCTION__);
        goto U0_EXIT_FAIL;
    }

#if 0
    bzero((void*)&tio, sizeof(tio));
    
    tio.c_cflag |= (CLOCAL | CREAD);
    tio.c_cflag &= ~CSIZE;
    tio.c_cflag |= CS8;
    tio.c_cflag &= ~PARENB;
    tio.c_cflag &= ~CSTOPB;
#endif
    /* Set UART as 'raw' mode. Input is 8 bits/1 stop/no parity, /*
    /* and no special functions*/
    cfmakeraw(&tio);

    /* Set minimum 8 bytes as read() uart returns, and there is no timeout */
    /* before read() returns. */
    //tio.c_cc[VTIME] = 0;
    //tio.c_cc[VMIN]  = 8;

    /* Setup UART baudrates. */
    //input
    ret = cfsetispeed(&tio, B57600);
    if(ret != 0) {
        DEBUG_PRINTF("<%s>Set UART0 input speed fail.\n", __FUNCTION__);
        goto U0_EXIT_FAIL;
    }
    //output
    ret = cfsetospeed(&tio, B57600);
    if(ret != 0) {
        DEBUG_PRINTF("<%s>Set UART0 output speed fail.\n", __FUNCTION__);
        goto U0_EXIT_FAIL;
    }

    /* Flush io port. */
    ret = tcflush(fd, TCIOFLUSH);
    if(ret < 0) {
        DEBUG_PRINTF("<%s>Flush UART0 port fail.\n", __FUNCTION__);
        goto U0_EXIT_FAIL;
    }

    /* Set UART port attribution. */
    ret = tcsetattr(fd, TCSANOW, &tio);
    if(ret < 0) {
        DEBUG_PRINTF("<%s>Set UART0 attr fail.\n", __FUNCTION__);
        goto U0_EXIT_FAIL;
    }

    ret = fcntl(fd, F_SETFL, 0);
    if(ret < 0) {
        DEBUG_PRINTF("<%s>fcntl fail.\n", __FUNCTION__);
        goto U0_EXIT_FAIL;
    }

    /* Store fd and return. */
    uart_fd[UART_PORT_CB] = fd;
    return (UART_OK);
    
U0_EXIT_FAIL:
    close(fd);
U0_EXIT_OPEN:
    uart_fd[UART_PORT_CB] = -1;
    return (UART_FAIL);
}


static int uart_pb_init(void)
{
    struct termios tio;
    int ret = 0;
    int fd = 0;

    fd = open(UART_PATH_PB, O_RDWR|O_NOCTTY|O_NDELAY);
    if(fd < 0) {
        printf("[Caution]Open UART PB port fail.\n");
        goto U2_EXIT_OPEN;
    }

    /* Get UART port attribution. */
    ret = tcgetattr(fd, &tio);
    if(ret != 0) {
        DEBUG_PRINTF("<%s>Get UART2 attr fail.\n", __FUNCTION__);
        goto U2_EXIT_FAIL;
    }

#if 0
    bzero((void*)&tio, sizeof(tio));
    
    tio.c_cflag |= (CLOCAL | CREAD);
    tio.c_cflag &= ~CSIZE;
    tio.c_cflag |= CS8;
    tio.c_cflag &= ~PARENB;
    tio.c_cflag &= ~CSTOPB;
#endif
    /* Set UART as 'raw' mode. Input is 8 bits/1 stop/no parity, /*
    /* and no special functions*/
    cfmakeraw(&tio);

    /* Set minimum 8 bytes as read() uart returns, and there is no timeout */
    /* before read() returns. */
    tio.c_cc[VTIME] = 0;
    tio.c_cc[VMIN]  = 8;

    /* Setup UART baudrates. */
    //input
    ret = cfsetispeed(&tio, B115200);
    if(ret != 0) {
        DEBUG_PRINTF("<%s>Set UART2 input speed fail.\n", __FUNCTION__);
        goto U2_EXIT_FAIL;
    }
    //output
    ret = cfsetospeed(&tio, B115200);
    if(ret != 0) {
        DEBUG_PRINTF("<%s>Set UART2 output speed fail.\n", __FUNCTION__);
        goto U2_EXIT_FAIL;
    }

    /* Flush io port. */
    ret = tcflush(fd, TCIOFLUSH);
    if(ret < 0) {
        DEBUG_PRINTF("<%s>Flush UART2 port fail.\n", __FUNCTION__);
        goto U2_EXIT_FAIL;
    }

    /* Set UART port attribution. */
    ret = tcsetattr(fd, TCSANOW, &tio);
    if(ret < 0) {
        DEBUG_PRINTF("<%s>Set UART2 attr fail.\n", __FUNCTION__);
        goto U2_EXIT_FAIL;
    }

    ret = fcntl(fd, F_SETFL, 0);
    if(ret < 0) {
        DEBUG_PRINTF("<%s>fcntl fail.\n", __FUNCTION__);
        goto U2_EXIT_FAIL;
    }

    /* Store fd and return. */
    uart_fd[UART_PORT_PB] = fd;
    return (UART_OK);
    
U2_EXIT_FAIL:
    close(fd);
U2_EXIT_OPEN:
    uart_fd[UART_PORT_PB] = -1;
    return (UART_FAIL);
}


static int uart_user_init(void)
{
    struct termios tio;
    int ret = 0;
    int fd = 0;

    fd = open(UART_PATH_USER, O_RDWR|O_NOCTTY|O_NDELAY);
    if(fd < 0) {
        printf("[Caution]Open UART USER port fail.\n");
        goto U3_EXIT_OPEN;
    }

    /* Get UART port attribution. */
    ret = tcgetattr(fd, &tio);
    if(ret != 0) {
        DEBUG_PRINTF("<%s>Get UART3 attr fail.\n", __FUNCTION__);
        goto U3_EXIT_FAIL;
    }

#if 0
    bzero((void*)&tio, sizeof(tio));
    
    tio.c_cflag |= (CLOCAL | CREAD);
    tio.c_cflag &= ~CSIZE;
    tio.c_cflag |= CS8;
    tio.c_cflag &= ~PARENB;
    tio.c_cflag &= ~CSTOPB;
#endif
    /* Set UART as 'raw' mode. Input is 8 bits/1 stop/no parity, /*
    /* and no special functions*/
    cfmakeraw(&tio);

    /* Set minimum 8 bytes as read() uart returns, and there is no timeout */
    /* before read() returns. */
    //tio.c_cc[VTIME] = 0;
    //tio.c_cc[VMIN]  = 8;

    /* Setup UART baudrates. */
    //input
    ret = cfsetispeed(&tio, B115200);
    if(ret != 0) {
        DEBUG_PRINTF("<%s>Set UART3 input speed fail.\n", __FUNCTION__);
        goto U3_EXIT_FAIL;
    }
    //output
    ret = cfsetospeed(&tio, B115200);
    if(ret != 0) {
        DEBUG_PRINTF("<%s>Set UART3 output speed fail.\n", __FUNCTION__);
        goto U3_EXIT_FAIL;
    }

    /* Flush io port. */
    ret = tcflush(fd, TCIOFLUSH);
    if(ret < 0) {
        DEBUG_PRINTF("<%s>Flush UART3 port fail.\n", __FUNCTION__);
        goto U3_EXIT_FAIL;
    }

    /* Set UART port attribution. */
    ret = tcsetattr(fd, TCSANOW, &tio);
    if(ret < 0) {
        DEBUG_PRINTF("<%s>Set UART3 attr fail.\n", __FUNCTION__);
        goto U3_EXIT_FAIL;
    }

    ret = fcntl(fd, F_SETFL, 0);
    if(ret < 0) {
        DEBUG_PRINTF("<%s>fcntl fail.\n", __FUNCTION__);
        goto U3_EXIT_FAIL;
    }

    /* Store fd and return. */
    uart_fd[UART_PORT_USER] = fd;
    return (UART_OK);
    
U3_EXIT_FAIL:
    close(fd);
U3_EXIT_OPEN:
    uart_fd[UART_PORT_USER] = -1;
    return (UART_FAIL);
}


int uart_initialise(void)
{
    int ret = 0;

    ret += uart_cb_init();
    ret += uart_pb_init();
    ret += uart_user_init();

    return ret;
}


int uart_receive(int uart_port, char *data, unsigned int num)
{
    int fd;
    int ret = 0;
    char *tbuff = NULL;
    int rcnt = 0;
    int remain = 0;

    /* Input parameter check. */
    if((uart_port > UART_PORT_NUM) || (NULL == data)) {
        DEBUG_PRINTF("<%s>Input parameter error.\n", __FUNCTION__);
        return (UART_PARAM_ERROR);
    }

    fd = uart_fd[uart_port];

    /* Allocate memory for temp read buffer. */
    tbuff = (char*)malloc(num);
    if(NULL == tbuff) {
        DEBUG_PRINTF("<%s>Allocate memory fail.\n", __FUNCTION__);
        return (UART_FAIL);
    }
    memset(tbuff, 0, num);

    /* Read data. */
    //do cycle read until specified number of data have all been read
    do {
        ret = read(fd, tbuff, num);
        if(ret < 0) {
            DEBUG_PRINTF("<%s>Read data from UART%d fail.\n", __FUNCTION__, uart_port);
            return (UART_FAIL);
        }
        rcnt += ret;
        //get remain data number
        ioctl(fd, FIONREAD, &remain);
    } while(remain > 0);

    /* Store read data. */
    for(ret=0; ret<rcnt; ret++) {
        *(data++) = *(tbuff++);
    }
    
    return rcnt;
}


int uart_transfer(int uart_port, char *data, unsigned int num)
{
    int fd;
    int ret = 0;

    /* Input parameter check. */
    if((uart_port > UART_PORT_NUM) || (NULL == data)) {
        DEBUG_PRINTF("<%s>Input parameter error.\n", __FUNCTION__);
        return (UART_PARAM_ERROR);
    }

    fd = uart_fd[uart_port];

    /* Transfer data. */
    ret = write(fd, data, num);
    if(ret < 0) {
        DEBUG_PRINTF("<%s>Write data to UART%d fail.\n", __FUNCTION__, uart_port);
        return (UART_FAIL);
    }

    return ret;
}

