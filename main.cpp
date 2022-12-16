#include <stdio.h>
#include "pico/stdlib.h"

const uint load = 20;
const uint clockEnable = 18;
const uint dataIn = 14;
const uint clockIn = 19;

unsigned char files[8];
unsigned char filesLastState[8];

char fileLetter[1];
char coord1[2];
char coord2[2];

bool moveComplete = true;
bool gameStarted = false;

// TODO Remove test bool
bool tempStart = false;

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

void getChangedCoordinate(int fileIndex, char newData) {
    for (int i = 0; i < 8; i++) {
        if ( (files[0] >> (7 - i) & 1) != (newData >> (7 - i) & 1) ) {
            int rank = 8 - i;

            if (moveComplete == false) {
                coord1[0] = fileLetter[0];
                coord1[1] = char(rank);
            } else {
                coord2[0] = fileLetter[0];
                coord2[1] = char(rank);
            }
        }
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

bool wasMoveCancelled() {
    if (coord1[0] == coord2[0] && coord1[1] == coord2[1]) {
        return true;
    }

    return false;
}

int main() {
    stdio_init_all();
    gpio_init(led);
    gpio_set_dir(led, GPIO_OUT);
    gpio_put(led, true);
    initShiftRegPins();

    while(true) {

        // TODO Remove this holding loop used for testing.  Main program does not start until a piece is placed
        while (files[0] != 0 && tempStart == false) {
            sleep_ms(100);
        }
        tempStart = true;
        // End of test code

        unsigned char dataRead = readShiftRegister();

        if (dataRead != files[0]) {
            getFileLetter(0, fileLetter);

            if ((dataRead > files[0]) && moveComplete == false) {
                getChangedCoordinate(0, dataRead);
                printf("MOVE ENDED\n");
                    
                moveComplete = true;
                if (!gameStarted) {
                    gameStarted = true;
                }

                if (moveComplete && gameStarted && !wasMoveCancelled()) {
                    printf("UCI: ");
                    printf("%c", coord1[0]);
                    printf("%d", coord1[1]);
                    printf("%c", coord2[0]);
                    printf("%d", coord2[1]);
                    printf("\n\n");
                }

                if (wasMoveCancelled()) {
                    printf("Piece replaced! Move Cancelled\n\n");
                }
            }

            if (dataRead < files[0]) {
                if (moveComplete == true) {
                    getChangedCoordinate(0, dataRead);
                    printf("MOVE STARTED\n");
                }
                moveComplete = false;
            }

            files[0] = dataRead;
        }

        sleep_ms(10);
    }
}