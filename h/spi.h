#ifndef __FANTM_SPI_H__
#define __FANTM_SPI_H__

#include "hardware/spi.h"

#define BAUDRATE 1000000
#define READ_BIT 0x80
#define PIN_SDI 4
#define PIN_SCK 2
#define PIN_SDO 3

typedef struct mySPI_t
{
    spi_inst_t *port;
    uint8_t cs_pin;
    uint8_t (*write)(struct mySPI_t *spi, uint8_t reg, uint8_t *payload, uint16_t len);
    uint8_t (*read)(struct mySPI_t *spi, uint8_t reg, uint8_t *buf, uint16_t len);
} mySPI_t;

int init_SPI(mySPI_t *spi, uint8_t cs_pin, spi_inst_t *spi_port);

#endif