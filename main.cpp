#include <stdio.h>
#include "pico/stdlib.h"

// Shift register pins
const uint load = 20;
const uint clockEnable = 18;
const uint clockIn = 19;

// Data input pins
const uint firstDataPin = 14;
const uint numOfFiles = 2;

// Data stores
unsigned char lastState[numOfFiles];

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

char getFileLetter(uint dataPin) {
    switch (dataPin) {
        case firstDataPin:
            return 'a';
            break;
        case firstDataPin + 1:
            return 'b';
            break;
        case firstDataPin + 2:
            return 'v';
            break;
        case firstDataPin + 3:
            return 'd';
            break;
        case firstDataPin + 4:
            return 'e';
            break;
        case firstDataPin + 5:
            return 'f';
            break;
        case firstDataPin + 6:
            return 'g';
            break;
        case firstDataPin + 7:
            return 'h';
            break;
        default:
            return 'z';
            break;
    }
}

void shiftPinStatesToRegister() {
    // Pulse load pin to get data from parallel input
    gpio_put(load, false);
    sleep_ms(5);
    gpio_put(load, true);
    sleep_ms(5);
}

unsigned char readRegister(uint dataPin) {
    unsigned char registerData = 0b00000000;

    shiftPinStatesToRegister();

    gpio_put(clockEnable, false);

    for (int i = 0; i < 8; i++) {
        gpio_put(clockIn, true);
        registerData |= gpio_get(dataPin) << i;
        gpio_put(clockIn, false);
    }

    gpio_put(clockEnable, true);

    return registerData;
}

int main() {
    stdio_init_all();
    initPins();

    while(true) {
        sleep_ms(500);
        gpio_put(led, true);
        
        // Get data from shift register
        for (int i = firstDataPin; i < (firstDataPin + numOfFiles); i++) {
            unsigned char data = readRegister(i);
            char file = getFileLetter(i);

            printf("File %c pin states: ", file);
            for (int k = 0; k < 8; k++) {
                printf("%d", (data >> (7 - k)) & 1);
            }

            printf("\n");
        }
        printf("\n");        

        sleep_ms(500);
        gpio_put(led, false);
    }
}