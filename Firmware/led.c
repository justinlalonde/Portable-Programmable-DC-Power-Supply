#include "led.h"

static bool led_green_on = false, led_red_on = false;

void led_init(){
    gpio_config_t io_g_conf = {
        .pin_bit_mask = (1ULL << LEDG_IO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_g_conf);
    gpio_config_t io_r_conf = {
        .pin_bit_mask = (1ULL << LEDR_IO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_r_conf);
}
void ledGreenOn(){
    if(led_green_on) return;
    led_green_on = true;
    gpio_set_level(LEDG_IO, 1); 
}
void ledGreenOff(){
    if(!led_green_on) return;
    led_green_on = false;
    gpio_set_level(LEDG_IO, 0); 
}
void ledRedOn(){
    if(led_red_on) return;
    led_red_on = true;
    gpio_set_level(LEDR_IO, 1); 
}
void ledRedOff(){
    if (!led_red_on) return;
    led_red_on = false;
    gpio_set_level(LEDR_IO, 0); 
}