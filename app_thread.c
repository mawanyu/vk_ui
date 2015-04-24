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
#include <fcntl.h>
#include <sys/time.h>
//#include <sys/types.h>
//#include <sys/ioctl.h>
#include <linux/input.h>
#include <termios.h>

/********************/
/* Defines & Macros */
/********************/
#define SPI_INT_WAIT_TIMEOUT    (50)    //50ms
#define SPI_TRANS_BYTES         (8)

#define UART_PB_RX_BYTES        (4)
#define UART_CB_RX_BYTES        (4)

#define KNOB_TURN_KEYCODE       (KEY_1)
#define KNOB_PUSH_KEYCODE       (KEY_2)
#define START_KEY_KEYCODE       (KEY_3)
#define MUTE_KEY_KEYCODE        (KEY_4)

#define KNOB_TURN_FORWARD       (0)
#define KNOB_TURN_BACKWARD      (1)

/*************/
/* Variables */
/*************/
struct uart_cb_loopback_struct uart_cb_loopback;

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
        printf("<%s>Open spi ready gpio fail.\n", __FUNCTION__);
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
                if(ret == 0) {
                    push_data_buffer(&sys_received_cache, spi_rx_data, SPI_TRANS_BYTES);
                }
                else {
                    DEBUG_PRINTF("<%s>Get data from spi fail.\n", __FUNCTION__);
                }
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

        ret = uart_receive_package(UART_PORT_PB, uart_pb_rx_data);
        if(ret < 0) {
            DEBUG_PRINTF("<%s>Get data from uart of power board fail.\n", __FUNCTION__);
        }
        else {
            push_data_buffer(&sys_received_cache, uart_pb_rx_data, UART_PB_RX_BYTES);
        }
    }
}


void thread_uart_cb_receive(void)
{
    int ret = 0;
    //char uart_pb_rx_data[UART_PB_RX_BYTES];

    while(1) {
        //memset((void*)uart_cb_loopback.data, 0, 4);

        ret = uart_receive_package(UART_PORT_CB, uart_cb_loopback.data);
        if(ret < 0) {
            DEBUG_PRINTF("<%s>Get data from uart of power board fail.\n", __FUNCTION__);
        }
        else {
            uart_cb_loopback.flag = 1;
        }
    }
}

void thread_knob_key(void)
{
    int kk_input_fd;
    int ret = 0, nread = 0;
    int knob_rl_value = 0, knob_fl_value = 0;
    char knob_left_pre = 0, knob_right_pre = 0;
    struct pollfd fd_set;
    struct input_event kk_event;
    //struct timeval timeout;

    kk_input_fd = open("/dev/input/event0", O_RDONLY | O_NONBLOCK);
    if(kk_input_fd < 0) {
        printf("<%s>Open knob and key input event fail.\n", __FUNCTION__);
        return;
    }

    memset((void*)&fd_set, 0x0, sizeof(fd_set));
    fd_set.fd = kk_input_fd;
    fd_set.events = POLLPRI | POLLIN;

    while(1) {
        /* Wait for knob or key action, block when no action. */
        ret = poll(&fd_set, (nfds_t)1, -1);
        if(ret == 0) {
            DEBUG_PRINTF("<%s>Poll time out.\n", __FUNCTION__);
            continue;
        }
        else if(ret == -1) {
            DEBUG_PRINTF("<%s>Poll error.\n", __FUNCTION__);
            continue;
        }
        else {
            if(fd_set.revents & (POLLPRI | POLLIN)) {
                //get action event
                nread = read(fd_set.fd, &kk_event, sizeof(kk_event));
                if(nread != sizeof(kk_event)) {
                    printf("[Info]Read knob and key event fail.\n");
                    continue;
                }
            }
        }

        /* Get knob turn action. It is mapped to a key push up/down action. */
        /* The turning direction is decided by three signals: */
        /* ------------------------------------------------------------- */
        /* |                | KEY UP/DOWN | RISING LOCK | FALLING LOCK | */
        /* | LEFT(BACKWARD) |    UP(0)    |     0       |       -      | */
        /* | RIGHT(FORWARD) |    UP(0)    |     1       |       -      | */
        /* | LEFT(BACKWARD) |   DOWN(1)   |     -       |       0      | */
        /* | RIGHT(FORWARD) |   DOWN(1)   |     -       |       1      | */
        /* ------------------------------------------------------------- */
        /* Determine the direction when 2 actions stand for the same */
        if((kk_event.type == EV_KEY) && (kk_event.code == KNOB_TURN_KEYCODE)) {
            //mapped key up
            if(kk_event.value == 0) {
                gpio_get_value(GPIO_NUM_KNOB_RISING_LOCK, &knob_rl_value);
                //knob turn left action 2 times, mark that it turns left
                if((knob_rl_value == 0) && (knob_left_pre == 1)) {
                    knob_turn_left = 1;
                    knob_left_pre = 0;
                }
                //knob turn left action 1 time, remember status and wait for next action
                else if((knob_rl_value == 0) && (knob_left_pre == 0)) {
                    knob_left_pre = 1;
                    knob_right_pre = 0;
                }
                //knob turn right action 2 times, mark that it turns right
                else if((knob_rl_value == 1) && (knob_right_pre == 1)) {
                    knob_turn_right = 1;
                    knob_right_pre = 0;
                }
                //knob turn right action 1 time, remember status and wait for next action
                else if((knob_rl_value == 1) && (knob_right_pre == 0)) {
                    knob_right_pre = 1;
                    knob_left_pre = 0;
                }
            }
            //mapped key down
            else {
                gpio_get_value(GPIO_NUM_KNOB_FALLING_LOCK, &knob_fl_value);
                //knob turn right action 2 times, mark that it turns right
                if((knob_fl_value == 0) && (knob_right_pre == 1)) {
                    knob_turn_right = 1;
                    knob_right_pre = 0;
                }
                //knob turn right action 1 time, remember status and wait for next action
                else if((knob_fl_value == 0) && (knob_right_pre == 0)) {
                    knob_right_pre = 1;
                    knob_left_pre = 0;
                }
                //knob turn left action 2 times, mark that it turns left
                else if((knob_fl_value == 1) && (knob_left_pre == 1)) {
                    knob_turn_left = 1;
                    knob_left_pre = 0;
                }
                //knob turn left action 1 time, remember status and wait for next action
                else if((knob_fl_value == 1) && (knob_left_pre == 0)) {
                    knob_left_pre = 1;
                    knob_right_pre = 0;
                }
            }
        }
        /* Get knob push action */
        else if((kk_event.type == EV_KEY) && (kk_event.code == KNOB_PUSH_KEYCODE)) {
            //mark knob PUSH when the key release
            if(kk_event.value == 0) {
                knob_push_key = 1;
            }
        }
        /* Get start key action */
        else if((kk_event.type == EV_KEY) && (kk_event.code == START_KEY_KEYCODE)) {
            //mark START when key release
            if(kk_event.value == 0) {
                start_key = 1;
            }
        }
        /* Get mute key action */
        else if((kk_event.type == EV_KEY) && (kk_event.code == MUTE_KEY_KEYCODE)) {
            //mark MUTE when key release
            if(kk_event.value == 0) {
                mute_key = 1;
            }
        }
    }
}
