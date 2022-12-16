#include <stdio.h>
#include "pico/stdlib.h"

const uint load = 20;
const uint clockEnable = 18;
const uint dataIn = 14;
const uint clockIn = 19;

unsigned char files[8];
unsigned char filesLastState[8];

char fileLetter[1];
char coord1[4];
char coord2[4];

const uint led = 25;

void getFileLetter(uint index, char* outStr) {
    switch (index) {
        case 0:
            outStr[0] = 'a';
            break;
        case 1:
            outStr[0] = 'b';
            break;
        case 2:
            outStr[0] = 'c';
            break;
        case 3:
            outStr[0] = 'd';
            break;
        case 4:
            outStr[0] = 'e';
            break;
        case 5:
            outStr[0] = 'f';
            break;
        case 6:
            outStr[0] = 'g';
            break;
        case 7:
            outStr[0] = 'h';
            break;
        default:
            outStr[0] = 'z';
            break;
    }
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

        unsigned char dataRead = readShiftRegister();

        if (dataRead != files[0]) {
            if (dataRead > files[0]) {
                printf("Piece placed\n");
                files[0] = dataRead;
            }

            if (dataRead < files[0]) {
                printf("Piece lifted\n");
                files[0] = dataRead;
            }

            getFileLetter(0, fileLetter);

            printf("Piece coordinates:\n");
            for (int i = 0; i < 8; i++) {
                if ((files[0] >> (7 - i) & 1) == 1) {
                    int rank = 8 - i;

                    coord1[0] = fileLetter[0];
                    coord1[1] = char(rank);
                    printf("%c", coord1[0]);
                    printf("%d", coord1[1]);
                }
            }
            printf("\n\n");
        }

        sleep_ms(500);
        gpio_put(led, false);
    }
}