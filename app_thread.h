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

/*************/
/* Variables */
/*************/

/*************/
/* Functions */
/*************/
extern void thread_spi_receive(void);
extern void thread_uart_pb_receive(void);
extern void thread_knob_key(void);

#endif/* _APP_THREAD_H_ */
