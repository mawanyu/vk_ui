/*
 * file description here
 */
#ifndef _GPIO_H_
#define _GPIO_H_
/*****************/
/* Include Files */
/*****************/

/********************/
/* Defines & Macros */
/********************/
#define GPIO_OK                 (0)
#define GPIO_FAIL               (-1)
#define GPIO_PARAM_ERROR        (2)
#define GPIO_OPEN_DEVICE_FAIL   (3)
#define GPIO_READ_DEVICE_FAIL   (4)
#define GPIO_WRITE_DEVICE_FAIL  (5)

#define GPIO_DIRECTION_IN       (0)
#define GPIO_DIRECTION_OUT      (1)
#define GPIO_DIRECTION_LOW		(2)
#define GPIO_DIRECTION_HIGH		(3)

#define GPIO_EDGE_RISING        (1)
#define GPIO_EDGE_FALLING       (2)
#define GPIO_EDGE_BOTH			(3)
#define GPIO_EDGE_NONE			(0)

#define GPIO_VALUE_LOW          (0)
#define GPIO_VALUE_HI           (1)

#define GPIO_NUM_SPI_READY          (0)
#define GPIO_NUM_NURSECALL          (43)
#define GPIO_NUM_AUDIO_EN           (205)
#define GPIO_NUM_KNOB_RISING_LOCK   (148)
#define GPIO_NUM_KNOB_FALLING_LOCK  (149)

/*************/
/* Variables */
/*************/

/*************/
/* Functions */
/*************/
extern int gpio_export(unsigned int gpio_num);
extern int gpio_unexport(unsigned int gpio_num);
extern int gpio_set_direction(unsigned int gpio_num, unsigned int direction);
extern int gpio_get_direction(unsigned int gpio_num, unsigned int *direction);
extern int gpio_set_value(unsigned int gpio_num, unsigned int value);
extern int gpio_get_value(unsigned int gpio_num, unsigned int *value);
extern int gpio_set_edge(unsigned int gpio_num, unsigned int edge);
extern int gpio_get_edge(unsigned int gpio_num, unsigned int *edge);
extern int gpio_open(unsigned int gpio_num, int *fd);
extern int gpio_close(int fd);

extern int gpio_initialise(void);

#endif/*_GPIO_H_*/
