#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "spi.h"
#include <stdio.h>
#include "neuron_config.h"

static inline void cs_select(uint8_t cs_pin);
static inline void cs_deselect(uint8_t cs_pin);
static uint8_t write(mySPI_t *spi, uint8_t reg, uint8_t *payload, uint16_t len);
static uint8_t read(mySPI_t *spi, uint8_t reg, uint8_t *buf, uint16_t len);


int init_SPI(mySPI_t *spi, uint8_t cs_pin, spi_inst_t *spi_port) {
    spi_set_format(spi_port, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    uint set_baud = spi_init(spi_port, 2000 * 1000);
    #if NEURON_DEBUG_SPI
        printf("BAUD: %d\n", set_baud);
    #endif
    gpio_set_function(PIN_SDI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SDO, GPIO_FUNC_SPI);
    gpio_set_function(cs_pin, GPIO_FUNC_SIO);

    gpio_init(cs_pin);
    gpio_set_dir(cs_pin, GPIO_OUT);
    gpio_put(cs_pin, 1);

    spi->cs_pin = cs_pin;
    spi->write = write;
    spi->read = read;
    spi->port = spi_port;
    return 0;
}

static uint8_t write(mySPI_t *spi, uint8_t reg, uint8_t *payload, uint16_t len) {
    int ret;
    uint8_t status = 0;
    uint8_t write_buf[len + 1];
    uint8_t read_buf[len + 1];
    write_buf[0] = reg;
    for (int i = 0; i < len; i++) {
        write_buf[i + 1] = payload[i];
    }
    cs_select(spi->cs_pin);
    ret = spi_write_read_blocking(spi->port, write_buf, read_buf, len + 1);
    if (ret != len + 1) {
        printf("FAILED TO WRITE DATA");
    }
    cs_deselect(spi->cs_pin);
    status = read_buf[0];
    return status;
}

static uint8_t read(mySPI_t *spi, uint8_t reg, uint8_t *buf, uint16_t len) {
    int ret;
    uint8_t status = 0;
    // uint8_t src[len + 1];
    // uint8_t dst[len + 1];
    // src[0] = reg;  
    cs_select(spi->cs_pin);
    // ret = spi_write_read_blocking(spi->port, src, dst, len + 1);
    // for (int i = 0; i < len; i++) {
    //     buf[i] = dst[i + 1];
    // }
    ret = spi_write_read_blocking(spi->port, &reg, &status, 1);
    #if NEURON_DEBUG_SPI
        printf("Status: %d || ", status);
    #endif
    if (ret != 1) {
        printf("FAILED TO WRITE REG");
    }
    ret = spi_read_blocking(spi->port, 0, buf, len);
    #if NEURON_DEBUG_SPI
        printf("Data returned: ");
        for (int i = 0; i < len; i++) {
            printf("%d = %d, ", i, buf[i]);
        }
        printf("\n");
    #endif
    if (ret != len) {
        printf("FAILED TO READ SPI");
    }
    cs_deselect(spi->cs_pin);
    return ret;
}

static inline void cs_select(uint8_t cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 0);
    asm volatile("nop \n nop \n nop"); // FIXME
}

static inline void cs_deselect(uint8_t cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 1);
    asm volatile("nop \n nop \n nop"); // FIXME
}
