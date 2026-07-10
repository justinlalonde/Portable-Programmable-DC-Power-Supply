#ifndef ADS1015_H
#define ADS1015_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "driver/i2c_master.h"
#include "esp_log.h"

#include "constants.h"

void ads1015_init(i2c_master_bus_handle_t bus_handle);
esp_err_t ads1015_sample(int16_t *result, uint8_t channel);
bool getRDY0Status(void);
bool getRDY1Status(void);

#endif