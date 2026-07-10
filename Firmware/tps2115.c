#include "tps2115.h"

void tps2115_init(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << STAT_IO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));
}
bool getSTATStatus(void)
{
    return gpio_get_level(STAT_IO);
}