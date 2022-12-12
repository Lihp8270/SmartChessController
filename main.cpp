#include <stdio.h>
#include "pico/stdlib.h"

const uint led = 25;
const uint h2 = 14;
const uint h3 = 17;

int main() {
    stdio_init_all();
    gpio_init(led);
    gpio_set_dir(led, GPIO_OUT);
    gpio_init(h2);
    gpio_set_dir(h2, GPIO_IN);
    gpio_pull_up(h2);
    gpio_init(h3);
    gpio_set_dir(h3, GPIO_IN);  
    gpio_pull_up(h3);  

    while(true) {
        sleep_ms(500);
        gpio_put(led, true);
        printf("Hello World\n");
        printf("%d\n", gpio_get(h2));
        sleep_ms(500);
        gpio_put(led, false);
    }
}