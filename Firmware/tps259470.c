#include "tps259470.h"

void tps259470_init(void){
    gpio_config_t io_conf_0 = {
        .pin_bit_mask = (1ULL << AUXOFF_IO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf_0));
    gpio_config_t io_conf_1 = {
        .pin_bit_mask = (1ULL << NFAULT_IO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf_1));
}
bool getAUXOFFStatus(void){
    return gpio_get_level(AUXOFF_IO);
}
bool getNFAULTStatus(void){
    return gpio_get_level(NFAULT_IO);
}