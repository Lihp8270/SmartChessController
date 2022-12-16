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

void readShiftRegisterPins() {
    // Pulse load pin to get data from parallel input
    gpio_put(load, false);
    sleep_ms(5);
    gpio_put(load, true);
    sleep_ms(5);
}

unsigned char readShiftRegister() {
    unsigned char registerData = 0b00000000;

    readShiftRegisterPins();

    // Get data from shift register
    gpio_put(clockEnable, false);

    for (int i = 0; i < 8; i++) {
        gpio_put(clockIn, true);
        registerData |= gpio_get(dataIn) << i;
        gpio_put(clockIn, false);
    }
    gpio_put(clockEnable, true);

    return registerData;
}

int main() {
    stdio_init_all();
    gpio_init(led);
    gpio_set_dir(led, GPIO_OUT);

    initShiftRegPins();

    while(true) {
        sleep_ms(500);
        gpio_put(led, true);

        unsigned char aFile = readShiftRegister();

        printf("Pin states:\n");
        for (int i = 0; i < 8; i++) {
            printf("%d", (aFile >> (7 - i)) & 1);
        }

        printf("\n%d\n", aFile);

        sleep_ms(500);
        gpio_put(led, false);
    }
}