/*
 * file description here
 */

/*****************/
/* Include Files */
/*****************/
#include "spi.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <sys/types.h>
#include <stdint.h>

/********************/
/* Defines & Macros */
/********************/
#define SPI_DEV "/dev/spidev1.0"

//#define SPI_INTERRUPT_TRIGGER

#define SPI_BITS_PER_WORD	(16)
#define SPI_SPEED			(8000000)

/*************/
/* Variables */
/*************/
int spi_fd = 0;

/*************/
/* Functions */
/*************/
int spi_initialise(void)
{
	int ret = 0;
	int tfd;
	//uint8_t mode = 0;
	uint8_t bits = SPI_BITS_PER_WORD;
	uint32_t speed = SPI_SPEED;

    /* Open SPI device */
	ret = open(SPI_DEV, O_RDWR);
	if(ret < 0) {
		DEBUG_PRINTF("<%s>: open device fail.\n", __FUNCTION__);
		return (SPI_FAIL);
	}
	else {
		tfd = ret;
	}

    /* Modify SPI mode if necessary */
#if 0
	ret = ioctl(tfd, SPI_IOC_RD_MODE, &mode);
	if(ret < 0) {
		DEBUG_PRINTF("<%s>: get spi mode fail.\n", __FUNCTION__);
		goto FAIL_EXIT;
	}

	ret = ioctl(tfd, SPI_IOC_WR_MODE, &mode);
	if(ret < 0) {
		DEBUG_PRINTF("<%s>: set spi mode fail.\n", __FUNCTION__);
		goto FAIL_EXIT;
	}
#endif

    /* Set SPI bits per word (to 16) */
	ret = ioctl(tfd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if(ret < 0) {
		DEBUG_PRINTF("<%s>: set spi bits per word fail.\n", __FUNCTION__);
		goto FAIL_EXIT;
	}

    /* Set SPI transmit MAX speed (to 8MHz) */
	ret = ioctl(tfd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if(ret < 0) {
		DEBUG_PRINTF("<%s>: set spi max speed fail.\n", __FUNCTION__);
		goto FAIL_EXIT;
	}

	spi_fd = tfd;
	return SPI_OK;
FAIL_EXIT:
	close(tfd);
	return (SPI_FAIL);
}

int spi_transfer(char *tx, char *rx,int bytes)
{
	int ret = 0;
	struct spi_ioc_transfer tr;

	tr.tx_buf = (unsigned long)tx;
	tr.rx_buf = (unsigned long)rx;
	tr.len = bytes;
	//tr.delay_usecs = 10;
	tr.speed_hz = SPI_SPEED;
	tr.bits_per_word = SPI_BITS_PER_WORD;

	ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
	if(ret < 0) {
		DEBUG_PRINTF("<%s>: spi transfer fail.\n", __FUNCTION__);
		return (SPI_PARAM_ERROR);
	}

	return SPI_OK;
}

/*
* some legacy function.
*/
#if 0
int spi_open(char *dev, int *fd)
{
	int ret;

	/* Input parameter check */
	if((dev == NULL) || (fd == NULL)) {
		DEBUG_PRINTF("<%s>: input parameter error.\n", __FUNCTION__);
		return (-1);
	}

	ret = open(dev, O_RDWR);
	if(ret < 0) {
		DEBUG_PRINTF("<%s>: open device fail.\n", __FUNCTION__);
		return (-1);
	}
	else {
		*fd = ret;
		return 0;
	}
}

int spi_close(int fd)
{
	return close(fd);
}
#endif
