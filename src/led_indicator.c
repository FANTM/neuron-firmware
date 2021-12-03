#include "led_indicator.h"
#include "pico/stdlib.h"

static volatile const uint LED_PIN = 25;
static volatile bool led_state = false;
static struct repeating_timer timer;

static bool led_callback(struct repeating_timer *t) {
    gpio_put(LED_PIN, led_state);
    led_state = !led_state;
    return true;
}

void init_led() {
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, led_state);
    add_repeating_timer_ms(500, led_callback, NULL, &timer);
}
