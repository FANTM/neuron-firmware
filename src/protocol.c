#include "pico/stdlib.h"

void format_packet(uint16_t value, uint8_t chan, uint8_t *buff_out) {
    buff_out[0] = (value >> 8) & 0xFE;
    buff_out[1] = (value >> 1) & 0xFE;
    buff_out[2] = ((value << 6) & 0xC0) | ((chan << 2) & 0x3C);
    buff_out[3] = 0x01;
}
