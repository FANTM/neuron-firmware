/**
 * FANTM 
 */

#define PICO_STDIO_ENABLE_CRLF_SUPPORT 0
#define PICO_STDIO_DEFAULT_CRLF        0
#define PARAM_ASSERTIONS_ENABLE_SPI    1

#include <stdio.h>
#include "spi.h"
#include "mcp3464.h"
#include "pico/stdlib.h"
#include "neuron_config.h"
#include "protocol.h"
#include "pico/stdio.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include "hardware/pio.h"
#include "led_indicator.h"

#include "mcpclock.pio.h"

#define MAJOR_NEURON_VERSION 0
#define MINOR_NEURON_VERSION 1
#define PATCH_NEURON_VERSION 0

void cdc_task(void);

mcp3464_t adc;
mySPI_t spi;
uint8_t dbuf[4];

extern volatile bool data_ready;

int init_hw() {
    printf("Begin FANTM Power Up Sequence");
    init_SPI(&spi, MCP_CS_PIN, MCP_SPI_PORT);
    init_mcp3464(&adc, &spi, 6);
    init_led();
    printf("Finished FANTM Power Up Sequence");
    return 0;
}

int main() {
    // set_sys_clock_khz(180000, true);
    stdio_init_all();
    init_hw();
    tusb_init();
    adcpacket_t packet;
    while(true) {
        // printf("Hello Neuron %d.%d.%d\n", MAJOR_NEURON_VERSION, MINOR_NEURON_VERSION, PATCH_NEURON_VERSION);
        tud_task();
        if (data_ready) {
            adc.read_data(&adc, &packet);
            format_packet(packet.data, packet.channel, dbuf);
            cdc_task();
            //_write(1, dbuf, 4);
            // data_ready = true;
            clear_data_ready(adc.irq_pin);
        }
        //sleep_us(10);
    }
    return 0;
}

void cdc_task(void) {
    tud_cdc_n_write(0, dbuf, 4);
    // tud_cdc_n_write_flush(0);
}
