/*
 * file description here
 */
#ifndef _SPI_H_
#define _SPI_H_
/*****************/
/* Include Files */
/*****************/

/********************/
/* Defines & Macros */
/********************/
//#define SPI_TRANS_BYTES     (8)

#define SPI_OK              (0)
#define SPI_FAIL            (-1)
#define SPI_PARAM_ERROR     (-2)

/*************/
/* Variables */
/*************/

/*************/
/* Functions */
/*************/
extern int spi_initialise(void);
extern int spi_transfer(char *tx, char *rx, int bytes);

#endif/*_SPI_H_*/