#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "mcpclock.pio.h"
#include "mcp3464.h"
#include <stdio.h>
#include "neuron_config.h"


static void init_mcpclock(void);
static void config_mcp3464(mcp3464_t *device);
static void read_data(mcp3464_t *device, adcpacket_t *packet);
static uint8_t write_mcp3464(mcp3464_t *device, uint8_t reg, uint8_t *buf, uint16_t len);
static uint8_t read_mcp3464(mcp3464_t *device, uint8_t reg, uint8_t *buf, uint16_t len);

volatile bool data_ready = false;
volatile uint irq_pin_events;

void set_data_ready(uint gpio) {
    gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_FALL, false);
    data_ready = true;
}

void clear_data_ready(uint gpio) {
    data_ready = false;
    gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_FALL, true);
}

static void data_ready_callback(uint gpio, uint32_t events) {
    if (gpio == irq_pin_events) {
        // data_ready = true;
        set_data_ready(gpio);
    }
}

static void init_mcpclock() {
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &mcpclock_program);
    mcpclock_init(pio, sm, offset, 28, 19660800);
}

void init_mcp3464(mcp3464_t *device, mySPI_t *spi, uint8_t irq_pin) {
    device->NUM_CHANNELS = 8;
    device->spi = spi;
    device->read = read_mcp3464;
    device->write = write_mcp3464;
    device->read_data = read_data;
    device->irq_pin = irq_pin;
    irq_pin_events = irq_pin;
    init_mcpclock();
    gpio_init(irq_pin);
    gpio_pull_up(irq_pin);
    config_mcp3464(device);
    gpio_set_irq_enabled_with_callback(device->irq_pin, GPIO_IRQ_EDGE_FALL, true, &data_ready_callback);
}

static void config_mcp3464(mcp3464_t *device) {
    uint8_t config_buf[3] = {0, 0, 0};
    config_buf[0] = 0b00001000;
    device->write(device, MCP3464_CONFIG1, config_buf, 1);
    
    config_buf[0] = 0b11001011;
    device->write(device, MCP3464_CONFIG2, config_buf, 1);

    config_buf[0] = 0b11110000;
    device->write(device, MCP3464_CONFIG3, config_buf, 1);

    config_buf[0] = 0b01110110;
    device->write(device, MCP3464_IRQ, config_buf, 1);

    config_buf[0] = 0b00000000;
    config_buf[1] = 0b00000000;
    config_buf[2] = (EMG7_EN) | (EMG6_EN) | (EMG5_EN) | (EMG4_EN) | (EMG3_EN) | (EMG2_EN) | (EMG1_EN) | (EMG0_EN);
    device->write(device, MCP3464_SCAN, config_buf, 3);

    config_buf[0] = 0b11100011;
    device->write(device, MCP3464_CONFIG0, config_buf, 1);
}

static uint8_t write_mcp3464(mcp3464_t *device, uint8_t reg, uint8_t *buf, uint16_t len) {
    reg = MCP3464_DEV_ID << 6 | reg << 2 | MCP3464_WRITE;
    #if NEURON_DEBUG_MCP3464 
        printf("REG: %u\n", reg); 
    #endif
    return device->spi->write(device->spi, reg, buf, len);
}

static uint8_t read_mcp3464(mcp3464_t *device, uint8_t reg, uint8_t *buf, uint16_t len) {
    reg = MCP3464_DEV_ID << 6 | reg << 2 | MCP3464_SINGLE_READ;
    #if NEURON_DEBUG_MCP3464 
        printf("REG: %u\n", reg); 
    #endif
    return device->spi->read(device->spi, reg, buf, len);
}

static void read_data(mcp3464_t *device, adcpacket_t *packet) {
    uint8_t buff[4];
    device->read(device, MCP3464_ADCDATA, buff, 4);
    #if NEURON_DEBUG_MCP3464 
        printf("RAW: %d, %d, %d, %d\n", buff[0], buff[1], buff[2], buff[3]);
    #endif
    packet->data    = (buff[2] << 8) | buff[3];
    packet->channel = (0xF0 & buff[0]) >> 4;
}