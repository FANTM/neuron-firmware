#ifndef __MCP3464_H__
#define __MCP3464_H__

#include "hardware/spi.h"
#include "spi.h"

#define NUM_CHAN_MCP 8
#define MCP_CS_PIN 5
#define MCP_SPI_PORT spi0

#define MCP3464_ADCDATA 0x0
#define MCP3464_CONFIG0 0x1
#define MCP3464_CONFIG1 0x2
#define MCP3464_CONFIG2 0x3
#define MCP3464_CONFIG3 0x4
#define MCP3464_IRQ 0x5
#define MCP3464_MUX 0x6
#define MCP3464_SCAN 0x7
#define MCP3464_TIMER 0x8
#define MCP3464_OFFSETCAL 0x9
#define MCP3464_GAINCAL 0xA
#define MCP3464_LOCK 0xD
#define MCP3464_CRCREG 0xF
#define MCP3464_DEV_ID 0x01

#define MCP3464_SINGLE_READ 0b01
#define MCP3464_WRITE 0b10

#define EMG0_EN 0 << 0
#define EMG1_EN 0 << 1
#define EMG2_EN 0 << 2
#define EMG3_EN 0 << 3
#define EMG4_EN 0 << 4
#define EMG5_EN 0 << 5
#define EMG6_EN 1 << 6
#define EMG7_EN 0 << 7

typedef struct adcpacket_t {
    uint8_t channel;
    uint16_t data;
} adcpacket_t;

typedef struct mcp3464_t
{
    void (*read_data)(struct mcp3464_t *device, adcpacket_t *packet);
    uint8_t (*read) (struct mcp3464_t *device, uint8_t reg, uint8_t *buf, uint16_t len);
    uint8_t (*write) (struct mcp3464_t *device, uint8_t reg, uint8_t *buf, uint16_t len);
    mySPI_t *spi;
    uint8_t irq_pin;
    uint8_t NUM_CHANNELS;
} mcp3464_t;

void init_mcp3464(mcp3464_t *device, mySPI_t *spi, uint8_t irq_pin);
void set_data_ready(uint gpio);
void clear_data_ready(uint gpio);

#endif