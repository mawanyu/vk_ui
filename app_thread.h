/*
 * file description here
 */
#ifndef _APP_THREAD_H_
#define _APP_THREAD_H_
/*****************/
/* Include Files */
/*****************/

/********************/
/* Defines & Macros */
/********************/
struct uart_cb_loopback_struct {
    char data[4];
    char flag;
};

/*************/
/* Variables */
/*************/
extern struct uart_cb_loopback_struct uart_cb_loopback;

/*************/
/* Functions */
/*************/
extern void thread_spi_receive(void);
extern void thread_uart_pb_receive(void);
extern void thread_uart_cb_receive(void);
extern void thread_knob_key(void);

#endif/* _APP_THREAD_H_ */
