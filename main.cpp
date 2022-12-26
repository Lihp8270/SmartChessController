#include <stdio.h>
#include "pico/stdlib.h"

// Enums
enum PieceAction {
    PIECE_LIFTED = true,
    PIECE_PLACED = false,
};

enum PieceColour {
    WHITE = true,
    BLACK = false,
};

// Shift register pins
const uint load = 20;
const uint clockEnable = 18;
const uint clockIn = 19;

// Data input pins
const uint firstDataPin = 14;
const uint numOfFiles = 2;

// Data stores
unsigned char lastState[numOfFiles];
unsigned char liftedPieceUCIFile;
unsigned char liftedPieceUCIRank;
unsigned char placedPieceUCIFile;
unsigned char placedPieceUCIRank;
bool moveStarted = false;
bool moveComplete = false;
bool playerColour = WHITE;
bool colourToPlay = WHITE;

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

void populatePieceUCI(PieceAction lifted, unsigned char newState, unsigned char savedState, uint dataPin) {
    unsigned char file = getFileLetter(dataPin);

    for (int i = 0; i < 8; i++) {
        if ((newState >> (7 - i) & 1) != (savedState >> (7 - i) & 1)) {
            uint rank = 8 - i;
            if (lifted) {
                liftedPieceUCIFile = file;
                liftedPieceUCIRank = rank;
            } else {
                placedPieceUCIFile = file;
                placedPieceUCIRank = rank;
            }
        }
    }
}

void setPlayerColour(PieceColour colour) {
    playerColour = colour;
}

void changeColourToPlay() {
    colourToPlay = !colourToPlay;
}

void completeMove() {
    if (placedPieceUCIFile != liftedPieceUCIFile) {
        moveComplete = true;
    } else {
        if (placedPieceUCIRank != liftedPieceUCIRank) {
            moveComplete = true;
        } else {
            moveComplete = false;
        }
    }
    moveStarted = false;
}

int main() {
    stdio_init_all();
    initPins();

    while(true) {
        gpio_put(led, true);
        
        // Get data from shift register for each file
        for (int dataPin = firstDataPin; dataPin < (firstDataPin + numOfFiles); dataPin++) {
            unsigned char savedState = lastState[dataPin - firstDataPin];
            unsigned char newState = readRegister(dataPin);
            char file = getFileLetter(dataPin);

            // Check if file state has changed
            if (newState != savedState) {
    
                // If current state is greater than saved, piece has been placed
                if ((newState > savedState) && moveStarted) {
                    populatePieceUCI(PIECE_PLACED, newState, savedState, dataPin);
                    isPieceLifted = false;

                    // Move is only complete if placed coords do not equal the lifted coords
                    completeMove();
                }

                // If current state is less than saved, place has been lifted
                if ((newState < savedState) && !moveStarted && !isPieceLifted) {
                    moveStarted = true;
                    isPieceLifted = true;
                    populatePieceUCI(PIECE_LIFTED, newState, savedState, dataPin);
                }
                
                // Save current state
                lastState[dataPin - firstDataPin] = newState;
            }
        }

        if (moveComplete) {
            printf("%c%d%c%d\n\n", liftedPieceUCIFile, liftedPieceUCIRank, placedPieceUCIFile, placedPieceUCIRank);
            changeColourToPlay();
            moveComplete = false;
            moveStarted = false;
            printf("%d to play\n", colourToPlay);
        }

        sleep_ms(1);
    }
}