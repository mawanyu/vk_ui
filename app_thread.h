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
void thread_spi_receive(void);
void thread_uart_pb_receive(void);
void thread_timer(void);

#endif/* _APP_THREAD_H_ */
