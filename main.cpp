#include <stdio.h>
#include "pico/stdlib.h"

const uint load = 20;
const uint clockEnable = 18;
const uint dataIn = 14;
const uint clockIn = 19;

const uint led = 25;

void initShiftRegPins() {
    gpio_init(load);
    gpio_init(clockEnable);
    gpio_init(dataIn);
    gpio_init(clockIn);

    gpio_set_dir(load, GPIO_OUT);
    gpio_set_dir(clockEnable, GPIO_OUT);
    gpio_set_dir(clockIn, GPIO_OUT);
    gpio_set_dir(dataIn, GPIO_IN);

    gpio_pull_up(dataIn);
}

int main() {
    stdio_init_all();
    gpio_init(led);
    gpio_set_dir(led, GPIO_OUT);

    initShiftRegPins();

    while(true) {
        sleep_ms(500);
        gpio_put(led, true);
        
        // Pulse load pin to get data from parallel input
        gpio_put(load, false);
        sleep_ms(5);
        gpio_put(load, true);
        sleep_ms(5);

        // Get data from shift register
        gpio_put(clockEnable, false);

        printf("Pin states:\n");
        for (int i = 0; i<8; i++) {
            gpio_put(clockIn, true);
            printf("%d\n", gpio_get(dataIn));
            gpio_put(clockIn, false);
        }
        gpio_put(clockEnable, true);

        sleep_ms(500);
        gpio_put(led, false);
    }
}