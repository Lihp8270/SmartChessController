#include <stdio.h>
#include <string>
#include "pico/stdlib.h"

using namespace std;

const uint load = 20;
const uint clockEnable = 18;
const uint dataIn = 14;
const uint clockIn = 19;

unsigned char files[8];

const uint led = 25;

string getFileLetter(uint index) {
    switch (index) {
        case 0:
            return "a";
            break;
        case 1:
            return "b";
            break;
        case 2:
            return "c";
            break;
        case 3:
            return "d";
            break;
        case 4:
            return "e";
            break;
        case 5:
            return "f";
            break;
        case 6:
            return "g";
            break;
        case 7:
            return "h";
            break;
        default:
            break;
    }

    return "error";
}

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

        files[0] = readShiftRegister();

        printf("Piece coordinates:\n");
        for (int i = 0; i < 8; i++) {
            printf("%d", (files[0] >> (7 - i)) & 1);
        }

        printf("\n%d\n", files[0]);

        sleep_ms(500);
        gpio_put(led, false);
    }
}