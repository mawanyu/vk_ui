/*
 * file description here
 */
#ifndef _UART_H_
#define _UART_H_
/*****************/
/* Include Files */
/*****************/

/********************/
/* Defines & Macros */
/********************/
/* Total UART port amount */
#define UART_PORT_NUM   (3)
/* Defines of UART port number */
#define UART_PORT_CB    (0)
#define UART_PORT_PB    (1)
#define UART_PORT_USER  (2)

/* Defines of function return value */
#define UART_OK             (0)
#define UART_FAIL           (-1)
#define UART_PARAM_ERROR    (-2)

/*************/
/* Variables */
/*************/
//extern int uart_fd[];

/*************/
/* Functions */
/*************/
extern int uart_initialise(void);
extern int uart_transfer(int uart_port,char * data,unsigned int num);
extern int uart_receive(int uart_port,char * data,unsigned int num);

#endif/*_UART_H_*/

