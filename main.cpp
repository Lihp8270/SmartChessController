#include <stdio.h>
#include "pico/stdlib.h"

// Shift register pins
const uint load = 20;
const uint clockEnable = 18;
const uint clockIn = 19;

// Data input pins
const uint firstDataPin = 14;
const uint numOfFiles = 2;

// Misc
const uint led = 25;

void initPins() {
    gpio_init(led);
    gpio_set_dir(led, GPIO_OUT);

    gpio_init(load);
    gpio_init(clockEnable);
    gpio_init(clockIn);

    gpio_set_dir(load, GPIO_OUT);
    gpio_set_dir(clockEnable, GPIO_OUT);
    gpio_set_dir(clockIn, GPIO_OUT);

    // Init all data input pins
    gpio_init(14);
    gpio_init(15);
    gpio_set_dir(14, GPIO_IN);
    gpio_set_dir(15, GPIO_IN);
    gpio_pull_up(14);
    gpio_pull_up(15);
}

void shiftPinStatesToRegister() {
    // Pulse load pin to get data from parallel input
    gpio_put(load, false);
    sleep_ms(5);
    gpio_put(load, true);
    sleep_ms(5);
}

void showFileStates(uint dataPin) {
    for (int k = 0; k < 8; k++) {
        gpio_put(clockIn, true);
        printf("%d", gpio_get(dataPin));
        gpio_put(clockIn, false);
    }
}

int main() {
    stdio_init_all();
    initPins();

    while(true) {
        sleep_ms(500);
        gpio_put(led, true);
        
        // Get data from shift register
        for (int i = firstDataPin; i < (firstDataPin + numOfFiles); i++) {
            shiftPinStatesToRegister();

            gpio_put(clockEnable, false);      

            printf("Pin %d states: ", i);
            
            showFileStates(i);

            gpio_put(clockEnable, true);
            printf("\n");
        }
        printf("\n");        

        sleep_ms(500);
        gpio_put(led, false);
    }
}