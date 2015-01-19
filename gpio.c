/*
 * file description here
 */

/*****************/
/* Include Files */
/*****************/
#include "gpio.h"
#include "common.h"

#include <stdio.h>
#include <fcntl.h>
#include <string.h>

/********************/
/* Defines & Macros */
/********************/
#define SYSFS_GPIO_DIR	"/sys/class/gpio"

#define GPIO_PORT_NUM_MIN		(0)
#define GPIO_PORT_NUM_MAX		(255)

/*************/
/* Variables */
/*************/

/*************/
/* Functions */
/*************/
int gpio_export(unsigned int gpio_num)
{
    int fd = -1;
    int ret = 0;
    char port_str[4];

    /* Input parameter check */
    if((gpio_num < GPIO_PORT_NUM_MIN) || (gpio_num > GPIO_PORT_NUM_MAX)) {
        DEBUG_PRINTF("<%s>GPIO port number is out of range.\n", __FUNCTION__);
        return (GPIO_PARAM_ERROR);
    }

    /* Export GPIO port from system */
    sprintf(port_str, "%d", gpio_num);

    fd = open("/sys/class/gpio/export", O_WRONLY);
    if(fd < 0) {
        DEBUG_PRINTF("<%s>Open /sys/class/gpio/export fail.\n", __FUNCTION__);
        return (GPIO_OPEN_DEVICE_FAIL);
    }

    ret = write(fd, port_str, strlen(port_str));
    if(ret != strlen(port_str)) {
        DEBUG_PRINTF("<%s>Write GPIO port number %d to /sys/class/gpio/export fail. (%d)\n",
                        __FUNCTION__, gpio_num, ret);
        close(fd);
        return (GPIO_WRITE_DEVICE_FAIL);
    }

    close(fd);
    return GPIO_OK;
}


int gpio_unexport(unsigned int gpio_num)
{
    int fd = -1;
    int ret = 0;
    char port_str[4];

    /* Input parameter check */
    if((gpio_num < GPIO_PORT_NUM_MIN) || (gpio_num > GPIO_PORT_NUM_MAX)) {
        DEBUG_PRINTF("<%s>GPIO port number is out of range.\n", __FUNCTION__);
        return (GPIO_PARAM_ERROR);
    }

    /* Unexport GPIO port from system */
    sprintf(port_str, "%d", gpio_num);

    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if(fd < 0) {
        DEBUG_PRINTF("<%s>Open /sys/class/gpio/unexport fail.\n", __FUNCTION__);
        return (GPIO_OPEN_DEVICE_FAIL);
    }

    ret = write(fd, port_str, strlen(port_str));
    if(ret != strlen(port_str)) {
        DEBUG_PRINTF("<%s>Write GPIO port number %d to /sys/class/gpio/unexport fail.\n",
                        __FUNCTION__, gpio_num);
        close(fd);
        return (GPIO_WRITE_DEVICE_FAIL);
    }

    close(fd);
    return GPIO_OK;
}


int gpio_set_direction(unsigned int gpio_num, unsigned int direction)
{
    int fd = -1;
    int ret = 0;
    char file_path[64];
    char direction_str[10];

    /* Input parameter check */
    if((gpio_num < GPIO_PORT_NUM_MIN) || (gpio_num > GPIO_PORT_NUM_MAX)) {
        DEBUG_PRINTF("<%s>GPIO port number is out of range.\n", __FUNCTION__);
        return (GPIO_PARAM_ERROR);
    }

    switch(direction) {
        case GPIO_DIRECTION_OUT:
            snprintf(direction_str, sizeof(direction_str), "out");
            break;

        case GPIO_DIRECTION_IN:
            snprintf(direction_str, sizeof(direction_str), "in");
            break;

        case GPIO_DIRECTION_HIGH:
            snprintf(direction_str, sizeof(direction_str), "high");
            break;

        case GPIO_DIRECTION_LOW:
            snprintf(direction_str, sizeof(direction_str), "low");
            break;

        default:
            DEBUG_PRINTF("<%s>gpio_set_direction input parameter error!\n", __FUNCTION__);
            break;
    }

    /* Open GPIO direction file */
    snprintf(file_path, sizeof(file_path), SYSFS_GPIO_DIR "/gpio%d/direction", gpio_num);
    fd = open(file_path, O_WRONLY);
    if(fd < 0) {
        DEBUG_PRINTF("<%s>Open %s fail.\n", __FUNCTION__, file_path);
        return (GPIO_OPEN_DEVICE_FAIL);
    }

    /* Set GPIO direction */
    ret = write(fd, direction_str, sizeof(direction_str));
    if(ret != sizeof(direction_str)) {
        DEBUG_PRINTF("<%s>Write GPIO direction to %s fail.\n", __FUNCTION__, file_path);
        close(fd);
        return (GPIO_WRITE_DEVICE_FAIL);
    }

    close(fd);
    return GPIO_OK;
}


int gpio_get_direction(unsigned int gpio_num, unsigned int *direction)
{
    int fd = -1;
    int ret = 0;
    char file_path[64];
    char dir_str[4];

    /* Input parameter check */
    if((gpio_num < GPIO_PORT_NUM_MIN) || (gpio_num > GPIO_PORT_NUM_MAX)) {
        DEBUG_PRINTF("<%s>GPIO port number is out of range.\n", __FUNCTION__);
        return (GPIO_PARAM_ERROR);
    }
    if(direction == NULL) {
        return (GPIO_PARAM_ERROR);
    }

    /* Open GPIO direction file */
    snprintf(file_path, sizeof(file_path), SYSFS_GPIO_DIR "/gpio%d/direction", gpio_num);
    fd = open(file_path, O_RDONLY);
    if(fd < 0) {
        DEBUG_PRINTF("<%s>Open %s fail.\n", __FUNCTION__, file_path);
        return (GPIO_OPEN_DEVICE_FAIL);
    }

    /* Get GPIO direction */
    ret = read(fd, dir_str, 3);
    if((ret != 3) && (ret != 2)) {
        DEBUG_PRINTF("<%s>Read GPIO direction from %s fail.\n", __FUNCTION__, file_path);
        close(fd);
        return (GPIO_READ_DEVICE_FAIL);
    }

    /* GPIO direction string should be "out" or "in" */
    if(dir_str[0] == 'o') {
        *direction = GPIO_DIRECTION_OUT;
    }
    else if(dir_str[0] == 'i') {
        *direction = GPIO_DIRECTION_IN;
    }

    close(fd);
    return GPIO_OK;
}


int gpio_set_value(unsigned int gpio_num, unsigned int value)
{
    int fd = -1;
    int ret = 0;
    char file_path[64];

    /* Input parameter check */
    if((gpio_num < GPIO_PORT_NUM_MIN) || (gpio_num > GPIO_PORT_NUM_MAX)) {
        DEBUG_PRINTF("<%s>GPIO port number is out of range.\n", __FUNCTION__);
        return (GPIO_PARAM_ERROR);
    }

    /* Open GPIO value file */
    snprintf(file_path, sizeof(file_path), SYSFS_GPIO_DIR "/gpio%d/value", gpio_num);
    fd = open(file_path, O_WRONLY);
    if(fd < 0) {
        DEBUG_PRINTF("<%s>Open %s fail.\n", __FUNCTION__, file_path);
        return (GPIO_OPEN_DEVICE_FAIL);
    }

    /* Set GPIO value */
    if(value == GPIO_VALUE_HI) {
        ret = write(fd, "1", 1);
    }
    else {
        ret = write(fd, "0", 1);
    }
    if(ret != 1) {
        DEBUG_PRINTF("<%s>Write GPIO direction to %s fail.\n", __FUNCTION__, file_path);
        close(fd);
        return (GPIO_WRITE_DEVICE_FAIL);
    }

    close(fd);
    return GPIO_OK;
}


int gpio_get_value(unsigned int gpio_num, unsigned int *value)
{
    int fd = -1;
    int ret = 0;
    char file_path[64];
    char value_str;

    /* Input parameter check */
    if((gpio_num < GPIO_PORT_NUM_MIN) || (gpio_num > GPIO_PORT_NUM_MAX)) {
        DEBUG_PRINTF("<%s>GPIO port number is out of range.\n", __FUNCTION__);
        return (GPIO_PARAM_ERROR);
    }
    if(value == NULL) {
        return (GPIO_PARAM_ERROR);
    }

    /* Open GPIO value file */
    snprintf(file_path, sizeof(file_path), SYSFS_GPIO_DIR "/gpio%d/value", gpio_num);
    fd = open(file_path, O_RDONLY);
    if(fd < 0) {
        DEBUG_PRINTF("<%s>Open %s fail.\n", __FUNCTION__, file_path);
        return (GPIO_OPEN_DEVICE_FAIL);
    }

    /* Get GPIO value */
    ret = read(fd, &value_str, 1);
    if(ret != 1) {
        DEBUG_PRINTF("<%s>Read GPIO value from %s fail.\n", __FUNCTION__, file_path);
        close(fd);
        return (GPIO_READ_DEVICE_FAIL);
    }

    if(value_str == '1') {
        *value = GPIO_VALUE_HI;
    }
    else if(value_str == '0') {
        *value = GPIO_VALUE_LOW;
    }

    close(fd);
    return GPIO_OK;
}


int gpio_set_edge(unsigned int gpio_num, unsigned int edge)
{
    int fd = -1;
    int ret = 0;
    char file_path[64];
    char edge_str[10];

    /* Input parameter check */
    if((gpio_num < GPIO_PORT_NUM_MIN) || (gpio_num > GPIO_PORT_NUM_MAX)) {
        DEBUG_PRINTF("<%s>GPIO port number is out of range.\n", __FUNCTION__);
        return (GPIO_PARAM_ERROR);
    }

    /* Open GPIO edge file */
    snprintf(file_path, sizeof(file_path), SYSFS_GPIO_DIR "/gpio%d/edge", gpio_num);
    fd = open(file_path, O_WRONLY);
    if(fd < 0) {
        DEBUG_PRINTF("<%s>Open %s fail.\n", __FUNCTION__, file_path);
        return (GPIO_OPEN_DEVICE_FAIL);
    }

    /* Set GPIO interrupt edge */
    if(edge == GPIO_EDGE_RISING) {
        strcpy(edge_str, "rising");
    }
    else if(edge == GPIO_EDGE_FALLING) {
        strcpy(edge_str, "falling");
    }
    else if (edge == GPIO_EDGE_BOTH) {
        strcpy(edge_str, "both");
    }
    else if(edge == GPIO_EDGE_NONE) {
        strcpy(edge_str, "none");
    }

    ret = write(fd, edge_str, sizeof(edge_str));
    if(ret != sizeof(edge_str)) {
        DEBUG_PRINTF("<%s>Write GPIO interrupt edge to %s fail.\n",
                        __FUNCTION__, file_path);
        close(fd);
        return (GPIO_WRITE_DEVICE_FAIL);
    }

    close(fd);
    return GPIO_OK;
}


int gpio_get_edge(unsigned int gpio_num, unsigned int *edge)
{
    int fd = -1;
    int ret = 0;
    char file_path[64];
    char edge_str[10];

    /* Input parameter check */
    if((gpio_num < GPIO_PORT_NUM_MIN) || (gpio_num > GPIO_PORT_NUM_MAX)) {
        DEBUG_PRINTF("<%s>GPIO port number is out of range.\n", __FUNCTION__);
        return (GPIO_PARAM_ERROR);
    }
    if(edge == NULL) {
        return (GPIO_PARAM_ERROR);
    }

    /* Open GPIO edge file */
    snprintf(file_path, sizeof(file_path), SYSFS_GPIO_DIR "/gpio%d/edge", gpio_num);
    fd = open(file_path, O_RDONLY);
    if(fd < 0) {
        DEBUG_PRINTF("Open %s fail.\n", file_path);
        return (GPIO_OPEN_DEVICE_FAIL);
    }

    /* Get GPIO interrupt edge */
    ret = read(fd, edge_str, 7);
    if((ret != 7) && (ret != 6) && (ret !=4)) {
        DEBUG_PRINTF("<%s>Read GPIO interrupt edge from %s fail.\n",
                        __FUNCTION__, file_path);
        close(fd);
        return (GPIO_READ_DEVICE_FAIL);        
    }

    if(edge_str[0] == 'r') {
        *edge = GPIO_EDGE_RISING;
    }
    else if(edge_str[0] == 'f') {
        *edge = GPIO_EDGE_FALLING;
    }
    else if(edge_str[0] == 'b') {
        *edge = GPIO_EDGE_BOTH;
    }
    else if(edge_str[0] == 'n') {
        *edge = GPIO_EDGE_NONE;
    }

    close(fd);
    return GPIO_OK;
}


int gpio_open(unsigned int gpio_num, int *fd)
{
    int ret = 0;
    char file_path[64];

    /* Input parameter check */
    if((gpio_num < GPIO_PORT_NUM_MIN) || (gpio_num > GPIO_PORT_NUM_MAX)) {
        DEBUG_PRINTF("<%s>GPIO port number is out of range.\n", __FUNCTION__);
        return (GPIO_PARAM_ERROR);
    }
    if(fd == NULL) {
        return (GPIO_PARAM_ERROR);
    }

    /* Open GPIO value file */
    snprintf(file_path, sizeof(file_path), SYSFS_GPIO_DIR "/gpio%d/value", gpio_num);
    *fd = open(file_path, O_RDONLY | O_NONBLOCK);
    if(*fd < 0) {
        DEBUG_PRINTF("<%s>Open %s fail.\n", __FUNCTION__, file_path);
        return (GPIO_OPEN_DEVICE_FAIL);
    }

    return GPIO_OK;
}


int gpio_close(int fd)
{
    return close(fd);
}


int gpio_initialise(void)
{
    int ret = GPIO_OK;
    unsigned int value;

    ret = gpio_export(GPIO_NUM_NURSECALL);
    if(ret != GPIO_OK) {
        DEBUG_PRINTF("<%s>Export GPIO of nurse call error.\n", __FUNCTION__);
        return (-1);
    }

    ret = gpio_set_direction(GPIO_NUM_NURSECALL, GPIO_DIRECTION_LOW);
    if(ret != GPIO_OK) {
        DEBUG_PRINTF("<%s>Setup nurse call error.\n", __FUNCTION__);
        goto EXIT_NC;
    }

    ret = gpio_export(GPIO_NUM_AUDIO_EN);
    if(ret != GPIO_OK) {
        DEBUG_PRINTF("<%s>Export GPIO of audio enable error.\n", __FUNCTION__);
        goto EXIT_NC;
    }

    ret = gpio_set_direction(GPIO_NUM_AUDIO_EN, GPIO_DIRECTION_LOW);
    if(ret != GPIO_OK) {
        DEBUG_PRINTF("<%s>Setup audio enable error.\n", __FUNCTION__);
        goto EXIT_AE;
    }

    ret = gpio_export(GPIO_NUM_SPI_READY);
    if(ret != GPIO_OK) {
        DEBUG_PRINTF("<%s>Export GPIO of spi ready interrupt error.\n", __FUNCTION__);
        goto EXIT_AE;
    }

    ret = gpio_set_direction(GPIO_NUM_SPI_READY, GPIO_DIRECTION_IN);
    ret += gpio_set_edge(GPIO_NUM_SPI_READY, GPIO_EDGE_FALLING);
    ret += gpio_get_value(GPIO_NUM_SPI_READY, &value);
    if(ret != GPIO_OK) {
        DEBUG_PRINTF("<%s>Setup spi ready error.\n", __FUNCTION__);
        goto EXIT_SR;
    }

    return 0;

EXIT_SR:
    gpio_unexport(GPIO_NUM_SPI_READY);
EXIT_AE:
    gpio_unexport(GPIO_NUM_AUDIO_EN);
EXIT_NC:
    gpio_unexport(GPIO_NUM_NURSECALL);
    return (GPIO_FAIL);
}
